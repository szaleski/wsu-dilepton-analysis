#!/bin/env python

import sys,os
import ROOT as r
import numpy as np
from wsuPythonUtils import checkRequiredArguments

from optparse import OptionParser

tkType = {"global":0, "cosmic":1, "tracker":2}

def dPhi(phi1,phi2,debug=False):
    import math
    dphi = phi1-phi2
    if dphi < -math.pi:
        dphi = dphi + 2*math.pi
    if dphi > math.pi:
        dphi = dphi - 2*math.pi
    return dphi

def findTrackMatch(ev,idx,src,coll,debug=False):
    nTracks = 0
    if coll == src:
        print "source and comparison come from same collection, returning -1"
        return -1
    if coll == 0:
        nTracks = ev.nGlobalTracks
    elif coll == 1:
        nTracks = ev.nCosmicTracks
    elif coll == 2:
        nTracks = ev.nTrackerTracks

    yCompatible = False
    bestDEta = 10
    bestDPhi = 10
    matchIdx = -1
    if debug:
        print "   (%5s, %5s, %5s, %5s)"%("eta", "phi", "y-in", "y-out")
        print "%2d (%2.2f, %2.2f, %2.2f, %2.2f) "%(idx,
                                                   ev.trk_trackEta[10*src+idx],
                                                   ev.trk_trackPhi[10*src+idx],
                                                   ev.trk_innerY[10*src+idx],
                                                   ev.trk_outerY[10*src+idx])
    for tk in range(nTracks):
        tmpDEta = abs(ev.trk_trackEta[10*src+idx] - ev.trk_trackEta[10*coll+tk])
        tmpDPhi = dPhi(ev.trk_trackPhi[10*src+idx], ev.trk_trackPhi[10*coll+tk])
        if debug:
            print "%2d (%2.2f, %2.2f, %2.2f, %2.2f) "%(tk,
                                                       ev.trk_trackEta[10*coll+tk],
                                                       ev.trk_trackPhi[10*coll+tk],
                                                       ev.trk_innerY[10*coll+tk],
                                                       ev.trk_outerY[10*coll+tk])
        if ev.trk_innerY[10*src+idx] > 0 and ev.trk_innerY[10*coll+tk] > 0:
            yCompatible = True
        elif ev.trk_innerY[10*src+idx] < 0 and ev.trk_innerY[10*coll+tk] < 0:
            yCompatible = True
        elif ev.trk_outerY[10*src+idx] > 0 and ev.trk_outerY[10*coll+tk] > 0:
            yCompatible = True
        elif ev.trk_outerY[10*src+idx] < 0 and ev.trk_outerY[10*coll+tk] < 0:
            yCompatible = True
        if yCompatible and tmpDEta < bestDEta and tmpDPhi < bestDPhi:
            matchIdx = 10*coll+tk
            bestDEta = tmpDEta
            bestDPhi = tmpDPhi
    if matchIdx < 0 and debug:
        print "unable to match track %d from collection %d to a track in collection %d"%(idx,src,coll)
    return matchIdx

def passMuDen(ev,idx,tracker=False,debug=False):
    result = ev.trackpT[idx] > 53. and abs(ev.trackEta[idx]) < 0.9 and ev.isTracker[idx]
    if tracker:
        result = result and passMuIDTrk(ev,idx)
    return result

def passMuID(ev,idx,glbl=True,tracker=False,debug=False):
    passNVMuHits     = (ev.nValidMuonHits[idx] > 0)
    passNMMuStations = (ev.nMatchedStations[idx] > 1)
    passRelPtErr     = ((ev.ptError[idx]/ev.trackpT[idx]) < 0.3)
    result = passNVMuHits and passNMMuStations and passRelPtErr
    if glbl:
        result = result and ev.isGlobal[idx]
    return result and passMuDen(ev,idx,tracker,debug)

def passMuIDTrk(ev,idx,glbl=True,tracker=False,debug=False):
    passNPHits = (ev.pixelHits[idx] > 0)
    passTKLays = (ev.tkLayersWMeas[idx] > 5)
    return passNPHits and passTKLays

def passTrkDen(ev,idx,coll,match=False,debug=False):
    muonMatch = ev.trk_matchedMuIdx[10*coll+idx]
    if not match:
        muonMatch = 0
    if (muonMatch < 0):
        if debug:
            print "passTrkDen: unable to find a matched muon %d"%(muonMatch)
        sys.stdout.flush()
        return False
    
    # definitely want to try to match to a tracker (index 2 in the array) track here
    isTracker = (findTrackMatch(ev,idx,coll,2) > -1)
    if match:
        ev.isTracker[muonMatch]

    return ev.trk_trackpT[10*coll+idx] > 53. and abs(ev.trk_trackEta[10*coll+idx]) < 0.9 and isTracker

def passTrkMuID(ev,idx,coll,match=False,debug=False):
    muonMatch = ev.trk_matchedMuIdx[10*coll+idx]
    if not match:
        muonMatch = 0
    if (muonMatch < 0):
        if debug:
            print "passTrkMuID: unable to find a matched muon %d"%(muonMatch)
        sys.stdout.flush()
        return False

    # for track, we want to preliminarily match to the global track tracker information, though we could match to the cosmic track
    passNVMuHits     = (ev.trk_nValidMuonHits[10*coll+idx] > 0)
    passNMMuStations = (ev.trk_nMatchedStations[10*coll+idx] > 1) #probably already requires a match to a muon here...
    passRelPtErr     = ((ev.trk_ptError[10*coll+idx]/ev.trk_trackpT[10*coll+idx]) < 0.3)
    # definitely want to try to match to a global track here
    isGlobal = (findTrackMatch(ev,idx,coll,0) > -1)
    if match:
        passNVMuHits     = (ev.nValidMuonHits[muonMatch] > 0)
        passNMMuStations = (ev.nMatchedStations[muonMatch] > 1)
        passRelPtErr     = ((ev.ptError[muonMatch]/ev.trackpT[muonMatch]) < 0.3)
        isGlobal         = ev.isGlobal[muonMatch]
    result = passNVMuHits and passNMMuStations and passRelPtErr and isGlobal
    return result and passTrkDen(ev,idx,coll,match,debug)

def passTrkMuIDTrk(ev,idx,coll,match=False,debug=False):
    muonMatch = ev.trk_matchedMuIdx[10*coll+idx]
    if not match:
        # not requiring a match to a muon
        muonMatch = 0
    if (muonMatch < 0):
        if debug:
            print "passTrkMuIDTrk: unable to find a matched muon %d"%(muonMatch)
        sys.stdout.flush()
        return False

    # for track, we want to preliminarily match to the global track tracker information, though we could match to the tracker track
    passNPHits = (ev.trk_pixelHits[10*coll+idx]     > 0)
    passTKLays = (ev.trk_tkLayersWMeas[10*coll+idx] > 5)
    if match:
        passNPHits = (ev.pixelHits[muonMatch]     > 0)
        passTKLays = (ev.tkLayersWMeas[muonMatch] > 5)
    result = passNPHits and passTKLays and passTrkMuID(ev,idx,coll,match,debug)
    return result

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-i", "--infile", type="string", dest="infile",
                      metavar="infile", default="CosmicMuonTree_MC_76X",
                      help="[REQUIRED] Name of the input ROOT file, or list of files (for -t mode)")
    parser.add_option("-o", "--outfile", type="string", dest="outfile",
                      metavar="outfile", default="efficiencies",
                      help="[REQUIRED] Name of the output ROOT file")
    parser.add_option("-d", "--debug", action="store_true", dest="debug",
                      metavar="debug",
                      help="[OPTIONAL] Run in debug mode")
    parser.add_option("-u", "--upper", action="store_true", dest="upper",
                      metavar="upper",
                      help="[OPTIONAL] Use the upper leg, rather than the (default) lower leg")
    parser.add_option("-t", "--tchain", action="store_true", dest="tchain",
                      metavar="debug",
                      help="[OPTIONAL] Use a TChain rather than the raw file, must specify a list of input files as a text file to -i")

    (options, args) = parser.parse_args()
    checkRequiredArguments(options, parser)
    r.gROOT.SetBatch(True)


    ptBins  = np.array([0.,10.,20.,30.,40.,50.,75.,100.,150.,200.,250.,300.,400.,500.,750.,1000.,1500.,2000.,3000.])
    ptBins  = np.array([0., 53., 100., 200., 300., 400., 500., 750., 1000., 1500., 2000.])
    ptBins  = np.array([0., 50., 100., 150., 200., 250., 300., 500., 1000.])
    etaBins = np.array([-2., -1.6, -0.9, 0., 0.9, 1.6,  2.])
    phiBins = np.array([-4., -1.6, 0., 1.6,  4.])
    
    outfile = r.TFile("%s.root"%(options.outfile),"recreate")

    myfile = None    
    if (options.infile).find("root://") > -1:
        print "using TNetXNGFile for EOS access"
        myfile = r.TNetXNGFile("%s.root"%(options.infile),"r")
    else:        
        myfile = r.TFile("%s.root"%(options.infile),"r")

    mytree = myfile.Get("analysisSPMuons/MuonTree")
    
    if options.tchain:
        mychain = r.TChain("analysisSPMuons/MuonTree")
        for line in open("%s"%(options.infile),"r"):
            mychain.Add("%s"%(line))
        mytree = mychain
    
    muDenPtHistoNB = r.TH1D("muonDenominatorPtNB","",300, 0., 3000.)
    muDenPtHisto   = r.TH1D("muonDenominatorPt","",300, 0., 3000.)
    muDenPixHitHisto = r.TH1D("muonDenominatorPixHit","",10, -0.5, 9.5)
    muDenTkMeasHisto = r.TH1D("muonDenominatorTkMeas","",20, -0.5, 19.5)
    muNum1PtHisto  = r.TH1D("muonPassIDPt",     "",300, 0., 3000.)
    muNum2PtHisto  = r.TH1D("muonPassIDTrkPt",  "",300, 0., 3000.)
    muDenPtHistoNB.Sumw2()
    muDenPtHisto.Sumw2()
    muDenPixHitHisto.Sumw2()
    muDenTkMeasHisto.Sumw2()
    muNum1PtHisto.Sumw2()
    muNum2PtHisto.Sumw2()
    
    muIsTrkDenPtHistoNB = r.TH1D("muonIsTrkDenominatorPtNB","",300, 0., 3000.)
    muIsTrkDenPtHisto   = r.TH1D("muonIsTrkDenominatorPt","",300, 0., 3000.)
    muIsTrkDenPixHitHisto = r.TH1D("muonIsTrkDenominatorPixHit","",10, -0.5, 9.5)
    muIsTrkDenTkMeasHisto = r.TH1D("muonIsTrkDenominatorTkMeas","",20, -0.5, 19.5)
    muIsTrkNum1PtHisto  = r.TH1D("muonIsTrkPassIDPt",     "",300, 0., 3000.)
    muIsTrkNum2PtHisto  = r.TH1D("muonIsTrkPassIDTrkPt",  "",300, 0., 3000.)
    muIsTrkDenPtHistoNB.Sumw2()
    muIsTrkDenPtHisto.Sumw2()
    muIsTrkDenPixHitHisto.Sumw2()
    muIsTrkDenTkMeasHisto.Sumw2()
    muIsTrkNum1PtHisto.Sumw2()
    muIsTrkNum2PtHisto.Sumw2()

    muIsGlbDenPtHisto   = r.TH1D("muonIsGlbDenominatorPt","",300, 0., 3000.)
    muIsGlbDenPixHitHisto = r.TH1D("muonIsGlbDenominatorPixHit","",10, -0.5, 9.5)
    muIsGlbDenTkMeasHisto = r.TH1D("muonIsGlbDenominatorTkMeas","",20, -0.5, 19.5)
    muIsGlbNum1PtHisto  = r.TH1D("muonIsGlbPassIDPt",     "",300, 0., 3000.)
    muIsGlbNum2PtHisto  = r.TH1D("muonIsGlbPassIDTrkPt",  "",300, 0., 3000.)
    muIsGlbDenPtHisto.Sumw2()
    muIsGlbDenPixHitHisto.Sumw2()
    muIsGlbDenTkMeasHisto.Sumw2()
    muIsGlbNum1PtHisto.Sumw2()
    muIsGlbNum2PtHisto.Sumw2()
    
    muIsGlbIsTrkDenPtHisto   = r.TH1D("muonIsGlbIsTrkDenominatorPt","",300, 0., 3000.)
    muIsGlbIsTrkDenPixHitHisto = r.TH1D("muonIsGlbIsTrkDenominatorPixHit","",10, -0.5, 9.5)
    muIsGlbIsTrkDenTkMeasHisto = r.TH1D("muonIsGlbIsTrkDenominatorTkMeas","",20, -0.5, 19.5)
    muIsGlbIsTrkNum1PtHisto  = r.TH1D("muonIsGlbIsTrkPassIDPt",     "",300, 0., 3000.)
    muIsGlbIsTrkNum2PtHisto  = r.TH1D("muonIsGlbIsTrkPassIDTrkPt",  "",300, 0., 3000.)
    muIsGlbIsTrkDenPtHisto.Sumw2()
    muIsGlbIsTrkDenPixHitHisto.Sumw2()
    muIsGlbIsTrkDenTkMeasHisto.Sumw2()
    muIsGlbIsTrkNum1PtHisto.Sumw2()
    muIsGlbIsTrkNum2PtHisto.Sumw2()
    
    trkDenPtHisto  = r.TH1D("trackDenominatorPt","",300, 0., 3000.)
    trkNum1PtHisto = r.TH1D("trackPassIDPt",     "",300, 0., 3000.)
    trkNum2PtHisto = r.TH1D("trackPassIDTrkPt",  "",300, 0., 3000.)
    trkDenPtHisto.Sumw2()
    trkNum1PtHisto.Sumw2()
    trkNum2PtHisto.Sumw2()
    
    trkMuDenPtHisto  = r.TH1D("trackMuDenominatorPt","",300, 0., 3000.)
    trkMuNum1PtHisto = r.TH1D("trackMuPassIDPt",     "",300, 0., 3000.)
    trkMuNum2PtHisto = r.TH1D("trackMuPassIDTrkPt",  "",300, 0., 3000.)
    trkMuDenPtHisto.Sumw2()
    trkMuNum1PtHisto.Sumw2()
    trkMuNum2PtHisto.Sumw2()
    
    nEvents = mytree.GetEntries()
    eid = 0
    for event in mytree:
        if eid%1000 == 0:
            print "event=%d/%d: g=%d c=%d t=%d"%(eid,nEvents,event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks)
        if options.debug and eid%10 == 0:
            print "event=%d/%d: g=%d c=%d t=%d"%(eid,nEvents,event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks)
        nTracks = [event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks]
        for mu in range(event.nMuons):
            if not (event.trackpT[mu] > 53. and abs(event.trackEta[mu]) < 0.9):
                continue
            #if not (abs(event.dxy[mu]) < 5. and abs(event.dz[mu]) < 30):
            #    continue
            if options.upper:
                if (abs(event.innerY[mu]) < abs(event.outerY[mu])):
                    continue
            else:
                if (abs(event.innerY[mu]) > abs(event.outerY[mu])):
                    continue

            if (passMuDen(event,mu,False,options.debug)):
                muDenPtHistoNB.Fill(event.trackpT[mu])
                muDenPtHisto.Fill(event.trackpT[mu])
                muDenTkMeasHisto.Fill(event.tkLayersWMeas[mu])
                muDenPixHitHisto.Fill(event.pixelHits[mu])
                if (passMuID(event,mu,False,False,options.debug)):
                    muNum1PtHisto.Fill(event.trackpT[mu])
                if (passMuIDTrk(event,mu,False,False,options.debug)):
                    muNum2PtHisto.Fill(event.trackpT[mu])
                    
            if (passMuDen(event,mu,True,options.debug)):
                muIsTrkDenPtHistoNB.Fill(event.trackpT[mu])
                muIsTrkDenPtHisto.Fill(event.trackpT[mu])
                muIsTrkDenTkMeasHisto.Fill(event.tkLayersWMeas[mu])
                muIsTrkDenPixHitHisto.Fill(event.pixelHits[mu])
                if (passMuID(event,mu,False,True,options.debug)):
                    muIsTrkNum1PtHisto.Fill(event.trackpT[mu])
                if (passMuIDTrk(event,mu,False,True,options.debug)):
                    muIsTrkNum2PtHisto.Fill(event.trackpT[mu])

            if (passMuDen(event,mu,False,options.debug)):
                if event.isGlobal[mu]:
                    muIsGlbDenPtHisto.Fill(event.trackpT[mu])
                    muIsGlbDenTkMeasHisto.Fill(event.tkLayersWMeas[mu])
                    muIsGlbDenPixHitHisto.Fill(event.pixelHits[mu])
                if (passMuID(event,mu,True,False,options.debug)):
                    muIsGlbNum1PtHisto.Fill(event.trackpT[mu])
                if (passMuIDTrk(event,mu,True,False,options.debug)):
                    muIsGlbNum2PtHisto.Fill(event.trackpT[mu])
                    
            if (passMuDen(event,mu,True,options.debug)):
                if event.isGlobal[mu]:
                    muIsGlbIsTrkDenPtHisto.Fill(event.trackpT[mu])
                    muIsGlbIsTrkDenTkMeasHisto.Fill(event.tkLayersWMeas[mu])
                    muIsGlbIsTrkDenPixHitHisto.Fill(event.pixelHits[mu])
                if (passMuID(event,mu,True,True,options.debug)):
                    muIsGlbIsTrkNum1PtHisto.Fill(event.trackpT[mu])
                if (passMuIDTrk(event,mu,True,True,options.debug)):
                    muIsGlbIsTrkNum2PtHisto.Fill(event.trackpT[mu])
                    
        for tk in range(nTracks[1]):
            if not (event.trk_trackpT[tk] > 53. and abs(event.trk_trackEta[tk]) < 0.9):
                continue
            if options.upper:
                #if (abs(event.trk_innerY[tk]) < abs(event.trk_outerY[tk])):
                #    continue
                if (event.trk_innerY[tk] < 0):
                    continue
                if (event.trk_outerY[tk] < 0):
                    continue
            else:
                #if (abs(event.trk_innerY[tk]) > abs(event.trk_outerY[tk])):
                #    continue
                if (event.trk_innerY[tk] > 0):
                    continue
                if (event.trk_outerY[tk] > 0):
                    continue

            if (passTrkDen(event,tk,1,False,options.debug)):
                trkDenPtHisto.Fill(event.trk_trackpT[10+tk])
                if (passTrkMuID(event,tk,1,False,options.debug)):
                    trkNum1PtHisto.Fill(event.trk_trackpT[10+tk])
                if (passTrkMuIDTrk(event,tk,1,False,options.debug)):
                    trkNum2PtHisto.Fill(event.trk_trackpT[10+tk])

            muMatch = event.trk_matchedMuIdx[10+tk]
            if (passTrkDen(event,tk,1,True,options.debug)):
                trkMuDenPtHisto.Fill(event.trackpT[muMatch])
                if (passTrkMuID(event,tk,1,True,options.debug)):
                    trkMuNum1PtHisto.Fill(event.trackpT[muMatch])
                if (passTrkMuIDTrk(event,tk,1,True,options.debug)):
                    trkMuNum2PtHisto.Fill(event.trackpT[muMatch])
            else:
                sys.stdout.flush()
                if options.debug:
                    print "failed track denominator cut"
                    print "track = %d"%(tk)
                    print "track   pt %3.2f, eta %2.2f, phi %2.2f, y %4.2f/%4.2f, matched %d"%(event.trk_trackpT[10+tk],
                                                                                               event.trk_trackEta[10+tk],
                                                                                               event.trk_trackPhi[10+tk],
                                                                                               event.trk_innerY[10+tk],
                                                                                               event.trk_outerY[10+tk],
                                                                                               event.trk_matchedMuIdx[10+tk])
                    if (event.trk_matchedMuIdx[10+tk] > -1):
                        print "matched pt %3.2f, eta %2.2f, phi %2.2f, y %4.2f/%4.2f %dt/%dg/%dsa"%(
                            event.trackpT[event.trk_matchedMuIdx[10+tk]],
                            event.trackEta[event.trk_matchedMuIdx[10+tk]],
                            event.trackPhi[event.trk_matchedMuIdx[10+tk]],
                            event.innerY[event.trk_matchedMuIdx[10+tk]],
                            event.outerY[event.trk_matchedMuIdx[10+tk]],
                            event.isTracker[event.trk_matchedMuIdx[10+tk]],
                            event.isGlobal[event.trk_matchedMuIdx[10+tk]],
                            event.isStandAlone[event.trk_matchedMuIdx[10+tk]])
                        
                    print "p %d, tk %d, mSH %d, nVH %d, nVMH %d, nMS %d, nTLM %d"%(event.trk_pixelHits[10+tk],
                                                                                   event.trk_trackerHits[10+tk],
                                                                                   event.trk_muonStationHits[10+tk],
                                                                                   event.trk_nValidHits[10+tk],
                                                                                   event.trk_nValidMuonHits[10+tk],
                                                                                   event.trk_nMatchedStations[10+tk],
                                                                                   event.trk_tkLayersWMeas[10+tk])
                
        sys.stdout.flush()
        eid = eid + 1
    # end event loop
    outfile.cd()
    muDenPtHistoNB.Write()
    muDenPtHisto.Write()
    muDenPixHitHisto.Write()
    muDenTkMeasHisto.Write()
    muNum1PtHisto.Write()
    muNum2PtHisto.Write()
    
    muIsTrkDenPtHistoNB.Write()
    muIsTrkDenPtHisto.Write()
    muIsTrkDenPixHitHisto.Write()
    muIsTrkDenTkMeasHisto.Write()
    muIsTrkNum1PtHisto.Write()
    muIsTrkNum2PtHisto.Write()

    muIsGlbDenPtHisto.Write()
    muIsGlbDenPixHitHisto.Write()
    muIsGlbDenTkMeasHisto.Write()
    muIsGlbNum1PtHisto.Write()
    muIsGlbNum2PtHisto.Write()

    muIsGlbIsTrkDenPtHisto.Write()
    muIsGlbIsTrkDenPixHitHisto.Write()
    muIsGlbIsTrkDenTkMeasHisto.Write()
    muIsGlbIsTrkNum1PtHisto.Write()
    muIsGlbIsTrkNum2PtHisto.Write()
    
    trkDenPtHisto.Write()
    trkNum1PtHisto.Write()
    trkNum2PtHisto.Write()
    
    trkMuDenPtHisto.Write()
    trkMuNum1PtHisto.Write()
    trkMuNum2PtHisto.Write()
    outfile.Close()
    
    # matching track to track, check y-position compatibility
    # matching track to muon,  check y-position compatibility
