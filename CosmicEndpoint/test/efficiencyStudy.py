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

def isGoodEvent(ev,isUpper=False,debug=False):
    # find at least one muon with pT > 53 and |eta| < 0.9
    for mu in range(ev.nMuons):
        muPt  = ev.globalpT[mu]
        muEta = ev.globalEta[mu]
        
        muLeg = ev.isLower[mu]
        if isUpper:
            muLeg = ev.isUpper[mu]
        if muPt > 53. and abs(muEta) < 0.9 and muLeg == 1:
            return True
    return False
        

def findFunky(ev,isUpper=False,debug=False):
    # want to find the evnets where we have a duplicated Muon object pt eta phi upper all identical
    # optionally, use combined information?
    funky = False
    if ev.nMuons < 2:
        return funky

    for ref in range(ev.nMuons):
        refPt  = ev.globalpT[ref]
        refEta = ev.globalEta[ref]
        refPhi = ev.globalPhi[ref]

        refQ   = ev.charge[ref]

        refGlb = ev.isGlobal[ref]
        refTrk = ev.isTracker[ref]
        refSta = ev.isStandAlone[ref]

        refLeg = ev.isLower[ref]
        if isUpper:
            refLeg = ev.isUpper[ref]
            if ev.isLower[ref] > 0:
                continue
        else:
            if ev.isUpper[ref] > 0:
                continue
    
        for mu in range(ref, ev.nMuons):
            if mu == ref:
                continue
            chkPt  = ev.globalpT[mu]
            chkEta = ev.globalEta[mu]
            chkPhi = ev.globalPhi[mu]

            chkQ   = ev.charge[mu]

            chkGlb = ev.isGlobal[mu]
            chkTrk = ev.isTracker[mu]
            chkSta = ev.isStandAlone[mu]

            chkLeg = ev.isLower[mu]
            if isUpper:
                chkLeg = ev.isUpper[mu]
            if (chkLeg != refLeg):
                continue
            if debug:
                print "chk%d-%d:q%d (%dt/%dg/%dsa) pT:%2.4f,phi:%2.4f,eta:%2.4f"%(
                    mu, chkLeg,chkQ,chkTrk,chkGlb,chkSta,chkPt,chkPhi,chkEta)
                print "ref%d-%d:q%d (%dt/%dg/%dsa) pT:%2.4f,phi:%2.4f,eta:%2.4f"%(
                    ref,refLeg,refQ,refTrk,refGlb,refSta,refPt,refPhi,refEta)

                print "funky check: c%d-%d(%dt/%dg/%dsa),r%d-%d(%dt/%dg/%dsa) dPt:%2.4f,dPhi:%2.4f,dEta:%2.4f"%(
                    mu,chkLeg,chkTrk,chkGlb,chkSta,
                    ref,refLeg,refTrk,refGlb,refSta,
                    chkPt-refPt,
                    dPhi(chkPhi,refPhi),
                    (chkEta-refEta)
                    )
            if (chkPt - refPt) == 0 and dPhi(chkPhi,refPhi) == 0 and (chkEta-refEta) == 0 and (chkLeg == refLeg) and (chkQ == refQ):
                print "found funky (%d/%d/%d): c%d-%d,r%d-%d dPt:%2.4f,dPhi:%2.4f,dEta:%2.4f"%(
                    ev.run,ev.lumi,ev.event,
                    mu,chkLeg,ref,refLeg,
                    (chkPt-refPt),
                    dPhi(chkPhi,refPhi),
                    (chkEta-refEta)
                    )
                print "chk%d up=%d/lo=%d,q=%d (%dt/%dg/%dsa) pT:%2.4f,phi:%2.4f,eta:%2.4f - dxy:%2.2f,dz:%2.2f,pt:%2.4f,dpt/pt:%2.4f,pix:%d,tkhits:%d,tklaywmeas:%d,mustahits:%d,vmuhits:%d,matched:%d"%(
                    mu,ev.isUpper[mu],ev.isLower[mu],chkQ,chkTrk,chkGlb,chkSta,chkPt,chkPhi,chkEta,
                    ev.dxy[mu],
                    ev.dz[mu],
                    ev.trackpT[mu],
                    ev.ptError[mu]/ev.trackpT[mu],
                    ev.pixelHits[mu],
                    ev.trackerHits[mu],
                    ev.tkLayersWMeas[mu],
                    ev.muonStationHits[mu],
                    ev.nValidMuonHits[mu],
                    ev.nMatchedStations[mu])

                print "ref%d,up=%d/lo=%d,q=%d (%dt/%dg/%dsa) pT:%2.4f,phi:%2.4f,eta:%2.4f - dxy:%2.2f,dz:%2.2f,pt:%2.4f,dpt/pt:%2.4f,pix:%d,tkhits:%d,tklaywmeas:%d,mustahits:%d,vmuhits:%d,matched:%d"%(
                    ref,ev.isUpper[ref],ev.isLower[ref],refQ,refTrk,refGlb,refSta,refPt,refPhi,refEta,
                    ev.dxy[ref],
                    ev.dz[ref],
                    ev.trackpT[ref],
                    ev.ptError[ref]/ev.trackpT[ref],
                    ev.pixelHits[ref],
                    ev.trackerHits[ref],
                    ev.tkLayersWMeas[ref],
                    ev.muonStationHits[ref],
                    ev.nValidMuonHits[ref],
                    ev.nMatchedStations[ref])
                funky = True
        
    return funky

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
    parser.add_option("--tight", action="store_true", dest="tight",
                      metavar="tight",
                      help="[OPTIONAL] Use the tight cuts on dxy/dz")
    parser.add_option("-t", "--tchain", action="store_true", dest="tchain",
                      metavar="tchain",
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
    
    nMuonsUpperLower   = r.TH2D("nMuons","",10, -0.5,9.5, 10, -0.5,9.5)
    nMuonsUpperLower.Sumw2()
    eventCounter   = r.TH1D("eventCounter","",3, -0.5, 2.5)
    eventCounter.Sumw2()

    muDenPixHitHisto = r.TH1D("muonDenominatorPixHit","",10, -0.5, 9.5)
    muDenTkMeasHisto = r.TH1D("muonDenominatorTkMeas","",20, -0.5, 19.5)
    muDenDXYHisto    = r.TH1D("muonDenominatorDXY","",100, -10., 10.)
    muDenDZHisto     = r.TH1D("muonDenominatorDZ","",100, -50., 50.)
    muDenPtHisto     = r.TH1D("muonDenominatorPt","",300, 0., 3000.)
    muNum1PtHisto    = r.TH1D("muonPassIDPt",     "",300, 0., 3000.)
    muNum2PtHisto    = r.TH1D("muonPassIDTrkPt",  "",300, 0., 3000.)

    muNumNPixHitPtHisto      = r.TH1D("muonPassNPixHit",      "",300, 0., 3000.)
    muNumNTkLayersPtHisto    = r.TH1D("muonPassNTkLayers",    "",300, 0., 3000.)
    muNumRelPtErrPtHisto     = r.TH1D("muonPassRelPtErr",     "",300, 0., 3000.)
    muNumNValidMuHitPtHisto  = r.TH1D("muonPassNValidMuHits", "",300, 0., 3000.)
    muNumNMuStationsPtHisto  = r.TH1D("muonPassNMuStations",  "",300, 0., 3000.)
    muNumIsGlobalPtHisto     = r.TH1D("muonPassIsGlobal",     "",300, 0., 3000.)

    muDenPtHisto.Sumw2()
    muDenPixHitHisto.Sumw2()
    muDenTkMeasHisto.Sumw2()
    muDenDXYHisto.Sumw2()
    muDenDZHisto.Sumw2()
    muNum1PtHisto.Sumw2()
    muNum2PtHisto.Sumw2()

    muNumNPixHitPtHisto    .Sumw2()
    muNumNTkLayersPtHisto  .Sumw2()
    muNumRelPtErrPtHisto   .Sumw2()
    muNumNValidMuHitPtHisto.Sumw2()
    muNumNMuStationsPtHisto.Sumw2()
    muNumIsGlobalPtHisto   .Sumw2()
    
    muIsTrkDenPixHitHisto = r.TH1D("muonIsTrkDenominatorPixHit","",10, -0.5, 9.5)
    muIsTrkDenTkMeasHisto = r.TH1D("muonIsTrkDenominatorTkMeas","",20, -0.5, 19.5)
    muIsTrkDenDXYHisto    = r.TH1D("muonIsTrkDenominatorDXY","",100, -10., 10.)
    muIsTrkDenDZHisto     = r.TH1D("muonIsTrkDenominatorDZ","",100, -50., 50.)
    muIsTrkDenPtHisto     = r.TH1D("muonIsTrkDenominatorPt","",300, 0., 3000.)
    muIsTrkNum1PtHisto    = r.TH1D("muonIsTrkPassIDPt",     "",300, 0., 3000.)
    muIsTrkNum2PtHisto    = r.TH1D("muonIsTrkPassIDTrkPt",  "",300, 0., 3000.)
    muIsTrkDenPtHisto.Sumw2()
    muIsTrkDenPixHitHisto.Sumw2()
    muIsTrkDenTkMeasHisto.Sumw2()
    muIsTrkDenDXYHisto.Sumw2()
    muIsTrkDenDZHisto.Sumw2()
    muIsTrkNum1PtHisto.Sumw2()
    muIsTrkNum2PtHisto.Sumw2()

    muIsGlbDenPixHitHisto = r.TH1D("muonIsGlbDenominatorPixHit","",10, -0.5, 9.5)
    muIsGlbDenTkMeasHisto = r.TH1D("muonIsGlbDenominatorTkMeas","",20, -0.5, 19.5)
    muIsGlbDenDXYHisto    = r.TH1D("muonIsGlbDenominatorDXY","",100, -10., 10.)
    muIsGlbDenDZHisto     = r.TH1D("muonIsGlbDenominatorDZ","",100, -50., 50.)
    muIsGlbDenPtHisto     = r.TH1D("muonIsGlbDenominatorPt","",300, 0., 3000.)
    muIsGlbNum1PtHisto    = r.TH1D("muonIsGlbPassIDPt",     "",300, 0., 3000.)
    muIsGlbNum2PtHisto    = r.TH1D("muonIsGlbPassIDTrkPt",  "",300, 0., 3000.)
    muIsGlbDenPtHisto.Sumw2()
    muIsGlbDenPixHitHisto.Sumw2()
    muIsGlbDenTkMeasHisto.Sumw2()
    muIsGlbDenDXYHisto.Sumw2()
    muIsGlbDenDZHisto.Sumw2()
    muIsGlbNum1PtHisto.Sumw2()
    muIsGlbNum2PtHisto.Sumw2()
    
    muIsGlbIsTrkDenPixHitHisto = r.TH1D("muonIsGlbIsTrkDenominatorPixHit","",10, -0.5, 9.5)
    muIsGlbIsTrkDenTkMeasHisto = r.TH1D("muonIsGlbIsTrkDenominatorTkMeas","",20, -0.5, 19.5)
    muIsGlbIsTrkDenDXYHisto    = r.TH1D("muonIsGlbIsTrkDenominatorDXY","",100, -10., 10.)
    muIsGlbIsTrkDenDZHisto     = r.TH1D("muonIsGlbIsTrkDenominatorDZ","",100, -50., 50.)
    muIsGlbIsTrkDenPtHisto     = r.TH1D("muonIsGlbIsTrkDenominatorPt","",300, 0., 3000.)
    muIsGlbIsTrkNum1PtHisto    = r.TH1D("muonIsGlbIsTrkPassIDPt",     "",300, 0., 3000.)
    muIsGlbIsTrkNum2PtHisto    = r.TH1D("muonIsGlbIsTrkPassIDTrkPt",  "",300, 0., 3000.)
    muIsGlbIsTrkDenPtHisto.Sumw2()
    muIsGlbIsTrkDenPixHitHisto.Sumw2()
    muIsGlbIsTrkDenTkMeasHisto.Sumw2()
    muIsGlbIsTrkDenDXYHisto.Sumw2()
    muIsGlbIsTrkDenDZHisto.Sumw2()
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
    checkUpper = False
    if options.upper:
        checkUpper = True
    print "checkUpper %d"%(checkUpper)
    
    for event in mytree:
        if eid%1000 == 0:
            print "event=%d/%d: g=%d c=%d t=%d"%(eid,nEvents,event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks)
        if options.debug and eid%10 == 0:
            print "event=%d/%d: g=%d c=%d t=%d"%(eid,nEvents,event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks)
        nTracks = [event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks]

        if isGoodEvent(event,checkUpper,options.debug):
            # should have at least one muon with pT > 53. and |eta| < 0.9
            nMuonsUpperLower.Fill(event.nUpperLegs,event.nLowerLegs)
            eventCounter.Fill(0)
            if findFunky(event,checkUpper,options.debug):
                # skip events where we find the "duplicate" muons
                eventCounter.Fill(2)
                continue
            eventCounter.Fill(1)
            for mu in range(event.nMuons):
                if not (event.trackpT[mu] > 53. and abs(event.trackEta[mu]) < 0.9):
                    # skip muons that fail the basic Z' kinematic cuts and barrel region
                    continue
                if options.tight:
                    # optionally apply tighter pixel requirements
                    if not (abs(event.dxy[mu]) < 7.5 and abs(event.dz[mu]) < 25.):
                        continue
                if checkUpper:
                    # process upper rather than lower legs
                    # if isLower[mu]
                    if (abs(event.innerY[mu]) < abs(event.outerY[mu])):
                        continue
                else:
                    # if isUpper[mu]
                    if (abs(event.innerY[mu]) > abs(event.outerY[mu])):
                        continue
            
                # first go, don't probe isGlobal in the numerator, but require it in the full selection
                if event.isGlobal[mu] > 0:
                    # denominator cuts do not include track ID cuts, but include isTracker
                    if (passMuDen(event,mu,False,options.debug)):
                        muDenPtHisto.Fill(event.trackpT[mu])
                        muDenTkMeasHisto.Fill(event.tkLayersWMeas[mu])
                        muDenPixHitHisto.Fill(event.pixelHits[mu])
                        muDenDXYHisto.Fill(event.dxy[mu])
                        muDenDZHisto.Fill(event.dz[mu])
                        if (passMuID(event,mu,False,False,options.debug)):
                            muNum1PtHisto.Fill(event.trackpT[mu])
                        if (passMuIDTrk(event,mu,False,False,options.debug)):
                            muNum2PtHisto.Fill(event.trackpT[mu])
                            
                    # denominator cuts include track ID cuts, in addition to isTracker
                    if (passMuDen(event,mu,True,options.debug)):
                        muIsTrkDenPtHisto.Fill(event.trackpT[mu])
                        muIsTrkDenTkMeasHisto.Fill(event.tkLayersWMeas[mu])
                        muIsTrkDenPixHitHisto.Fill(event.pixelHits[mu])
                        muIsTrkDenDXYHisto.Fill(event.dxy[mu])
                        muIsTrkDenDZHisto.Fill(event.dz[mu])
                        if (passMuID(event,mu,False,True,options.debug)):
                            muIsTrkNum1PtHisto.Fill(event.trackpT[mu])
                        if (passMuIDTrk(event,mu,False,True,options.debug)):
                            muIsTrkNum2PtHisto.Fill(event.trackpT[mu])
                
                # probing isGlobal in the numerator
                # denominator cuts do not include track ID cuts, but include isTracker
                if (passMuDen(event,mu,False,options.debug)):
                    #if event.isGlobal[mu]:
                    if event.pixelHits[mu] > 0:
                        muNumNPixHitPtHisto.Fill()
                    if event.tkLayersWMeas[mu] > 5:
                        muNumNTkLayersPtHisto.Fill()
                    if (event.ptError[mu]/event.trackpT[mu]) < 0.3:
                        muNumRelPtErrPtHisto.Fill()
                    if event.nValidMuonHits[mu] > 0:
                        muNumNValidMuHitPtHisto.Fill()
                    if event.nMatchedStations[mu] > 1:
                        muNumNMuStationsPtHisto.Fill()
                    if event.isGlobal[mu] > 0:
                        muNumIsGlobalPtHisto.Fill()
                    
                    muIsGlbDenPtHisto.Fill(event.trackpT[mu])
                    muIsGlbDenTkMeasHisto.Fill(event.tkLayersWMeas[mu])
                    muIsGlbDenPixHitHisto.Fill(event.pixelHits[mu])
                    muIsGlbDenDXYHisto.Fill(event.dxy[mu])
                    muIsGlbDenDZHisto.Fill(event.dz[mu])
                    if (passMuID(event,mu,True,False,options.debug)):
                        muIsGlbNum1PtHisto.Fill(event.trackpT[mu])
                    if (passMuIDTrk(event,mu,True,False,options.debug)):
                        muIsGlbNum2PtHisto.Fill(event.trackpT[mu])
                        
                # denominator cuts include track ID cuts, in addition to isTracker
                if (passMuDen(event,mu,True,options.debug)):
                    #if event.isGlobal[mu]:
                    muIsGlbIsTrkDenPtHisto.Fill(event.trackpT[mu])
                    muIsGlbIsTrkDenTkMeasHisto.Fill(event.tkLayersWMeas[mu])
                    muIsGlbIsTrkDenPixHitHisto.Fill(event.pixelHits[mu])
                    muIsGlbIsTrkDenDXYHisto.Fill(event.dxy[mu])
                    muIsGlbIsTrkDenDZHisto.Fill(event.dz[mu])
                    if (passMuID(event,mu,True,True,options.debug)):
                        muIsGlbIsTrkNum1PtHisto.Fill(event.trackpT[mu])
                    if (passMuIDTrk(event,mu,True,True,options.debug)):
                        muIsGlbIsTrkNum2PtHisto.Fill(event.trackpT[mu])
                        
        for tk in range(nTracks[1]):
            if not (event.trk_trackpT[tk] > 53. and abs(event.trk_trackEta[tk]) < 0.9):
                continue
            if checkUpper:
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
    nMuonsUpperLower.Write()
    eventCounter.Write()

    muDenPtHisto.Write()
    muDenPixHitHisto.Write()
    muDenTkMeasHisto.Write()
    muDenDXYHisto.Write()
    muDenDZHisto.Write()
    muNum1PtHisto.Write()
    muNum2PtHisto.Write()

    muNumNPixHitPtHisto.Write()
    muNumNTkLayersPtHisto.Write()
    muNumRelPtErrPtHisto.Write()
    muNumNValidMuHitPtHisto.Write()
    muNumNMuStationsPtHisto.Write()
    muNumIsGlobalPtHisto.Write()
    
    muIsTrkDenPtHisto.Write()
    muIsTrkDenPixHitHisto.Write()
    muIsTrkDenTkMeasHisto.Write()
    muIsTrkDenDXYHisto.Write()
    muIsTrkDenDZHisto.Write()
    muIsTrkNum1PtHisto.Write()
    muIsTrkNum2PtHisto.Write()

    muIsGlbDenPtHisto.Write()
    muIsGlbDenPixHitHisto.Write()
    muIsGlbDenTkMeasHisto.Write()
    muIsGlbDenDXYHisto.Write()
    muIsGlbDenDZHisto.Write()
    muIsGlbNum1PtHisto.Write()
    muIsGlbNum2PtHisto.Write()

    muIsGlbIsTrkDenPtHisto.Write()
    muIsGlbIsTrkDenPixHitHisto.Write()
    muIsGlbIsTrkDenTkMeasHisto.Write()
    muIsGlbIsTrkDenDXYHisto.Write()
    muIsGlbIsTrkDenDZHisto.Write()
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
