#!/bin/env python

import sys,os
import ROOT as r
import numpy as np
from wsuPythonUtils import checkRequiredArguments
#from cosmicEfficiencyTools import *

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
    # find at least one muon with pT > 45 and |eta| < 0.9
    for mu in range(ev.nMuons):
        muPt  = ev.globalpT[mu]
        muEta = ev.globalEta[mu]
        
        muLeg = ev.isLower[mu]
        if isUpper:
            muLeg = ev.isUpper[mu]
        if muPt > 45. and abs(muEta) < 0.9 and muLeg == 1:
            return True
    return False
        

def fillChi2Hists(ev,idx,chi2Hist,trkChi2Hist,glbChi2Hist,staChi2Hist,debug=False):
    rchi2 = -1.
    if (ev.ndof[idx] > 0):
        rchi2 = ev.chi2[idx]/ev.ndof[idx]
    chi2Hist.Fill(ev.trackpT[idx],rchi2)
    rchi2 = -1.
    if (ev.innerNDF[idx] > 0):
        rchi2 = ev.innerChi2[idx]/ev.innerNDF[idx]
    trkChi2Hist.Fill(ev.trackpT[idx],rchi2)
    rchi2 = -1.
    if (ev.globalNDF[idx] > 0):
        rchi2 = ev.globalChi2[idx]/ev.globalNDF[idx]
    glbChi2Hist.Fill(ev.trackpT[idx],rchi2)
    rchi2 = -1.
    if (ev.outerNDF[idx] > 0):
        rchi2 = ev.outerChi2[idx]/ev.outerNDF[idx]
    staChi2Hist.Fill(ev.trackpT[idx],rchi2)
    

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
                print "chk%d up=%d/lo=%d,q=%d (%dt/%dg/%dsa) pT:%2.4f,phi:%2.4f,eta:%2.4f - dxy:%2.2f,dz:%2.2f,chi2:%2.2f,ndf:%d,pt:%2.4f,dpt/pt:%2.4f,pix:%d,tkhits:%d,tklaywmeas:%d,mustahits:%d,vmuhits:%d,matched:%d"%(
                    mu,ev.isUpper[mu],ev.isLower[mu],chkQ,chkTrk,chkGlb,chkSta,chkPt,chkPhi,chkEta,
                    ev.dxy[mu],
                    ev.dz[mu],
                    ev.chi2[mu],
                    ev.ndof[mu],
                    ev.trackpT[mu],
                    ev.ptError[mu]/ev.trackpT[mu],
                    ev.pixelHits[mu],
                    ev.trackerHits[mu],
                    ev.tkLayersWMeas[mu],
                    ev.muonStationHits[mu],
                    ev.nValidMuonHits[mu],
                    ev.nMatchedStations[mu])

                print "ref%d,up=%d/lo=%d,q=%d (%dt/%dg/%dsa) pT:%2.4f,phi:%2.4f,eta:%2.4f - dxy:%2.2f,dz:%2.2f,chi2:%2.2f,ndf:%d,pt:%2.4f,dpt/pt:%2.4f,pix:%d,tkhits:%d,tklaywmeas:%d,mustahits:%d,vmuhits:%d,matched:%d"%(
                    ref,ev.isUpper[ref],ev.isLower[ref],refQ,refTrk,refGlb,refSta,refPt,refPhi,refEta,
                    ev.dxy[ref],
                    ev.dz[ref],
                    ev.chi2[ref],
                    ev.ndof[ref],
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

def findTagTrack(ev,tightSel=False,useUpper=False,useGlobal=True,useTracker=True,debug=False):
    tagIdx   = -1
    tagCount = 0
    for trk in range(ev.nTrackerTracks):
        if debug:
            print "l%d/u%d"%(ev.trk_isLower[10*2+trk],ev.trk_isUpper[10*2+trk])
        if useUpper:
            # process upper rather than lower legs
            #if (abs(ev.trk_innerY[10*2+trk]) < abs(ev.trk_outerY[10*2+trk])):
            if ev.trk_isLower[10*2+trk]:
                continue
            pass
        else:
            #if (abs(ev.trk_innerY[10*2+trk]) > abs(ev.trk_outerY[10*2+trk])):
            if ev.trk_isUpper[10*2+trk]:
                continue
            pass
        
        if debug:
            print "trk:%d-l%d/u%d:q%d pT:%2.4f,phi:%2.4f,eta:%2.4f,dxy:%2.2f,dz:%2.2f,muIdx:%d"%(
                trk,ev.trk_isLower[10*2+trk],ev.trk_isUpper[10*2+trk],
                ev.trk_charge[10*2+trk],
                ev.trk_trackpT[10*2+trk],ev.trk_trackPhi[10*2+trk],ev.trk_trackEta[10*2+trk],
                ev.trk_dxy[10*2+trk],ev.trk_dz[10*2+trk],ev.trk_matchedMuIdx[10*2+trk])
            pass

        if not (ev.trk_trackpT[10*2+trk] > 45. and abs(ev.trk_trackEta[10*2+trk]) < 0.9):
            # skip muons that fail the basic Z' kinematic cuts and barrel region
            if debug:
                print "kinematic cuts failed"
            continue

        if not (abs(ev.trk_dxy[10*2+trk]) < 4. and abs(ev.trk_dz[10*2+trk]) < 10.):
            if debug:
                print "dxy/dz cuts failed"
            continue

        if tightSel:
            # optionally apply tighter pixel requirements
            # if not (abs(ev.trk_dxy[10*2+trk]) < 2.5 and abs(ev.trk_dz[10*2+trk]) < 10.):
            # if not (abs(ev.trk_dxy[10*2+trk]) < 2.5 and abs(ev.trk_dz[10*2+trk]) < 10.):
            if not (abs(ev.trk_dxy[10*2+trk]) < 1.0 and abs(ev.trk_dz[10*2+trk]) < 2.5):
                if debug:
                    print "tight dxy/dz cuts failed"
                continue
            pass

        if passTrkDen(ev,trk,2,True,False):
            if debug:
                print "track denominator cuts passed"
            tagIdx = trk
            tagCount = tagCount + 1
    if tagCount > 0:
        print "Found %d tags"%(tagCount)
    return tagIdx

def findTrkProbeMuon(ev,tagIdx,tightSel=False,useUpper=False,useGlobal=True,useTracker=True,debug=False):
    probeIdx   = -1
    probeCount = 0
    yCompatible = False
    bestDEta = 10
    bestDPhi = 10
    for mu in range(ev.nMuons):
        if debug:
            print "l%d/u%d"%(ev.isLower[mu],ev.isUpper[mu])
        if useUpper:
            # process upper rather than lower legs
            #if (abs(ev.innerY[mu]) < abs(ev.outerY[mu])):
            if ev.isLower[mu]:
                continue
            pass
        else:
            #if (abs(ev.innerY[mu]) > abs(ev.outerY[mu])):
            if ev.isUpper[mu]:
                continue
            pass
        tmpDEta = abs(ev.trk_trackEta[10*2+tagIdx] - ev.trackEta[mu])
        tmpDPhi = dPhi(ev.trk_trackPhi[10*2+tagIdx], ev.trackPhi[mu])
        if tmpDEta < bestDEta and tmpDPhi < bestDPhi:
            probeIdx = mu
            bestDEta = tmpDEta
            bestDPhi = tmpDPhi

    return probeIdx

def findTagMuon(ev,tightSel=False,useUpper=True,useGlobal=True,useTracker=True,debug=False):
    tagIdx   = -1
    tagCount = 0
    for mu in range(ev.nMuons):
        if debug:
            print "l%d/u%d"%(ev.isLower[mu],ev.isUpper[mu])
        if useUpper:
            # process upper rather than lower legs
            #if (abs(ev.innerY[mu]) < abs(ev.outerY[mu])):
            if ev.isLower[mu]:
                continue
            pass
        else:
            #if (abs(ev.innerY[mu]) > abs(ev.outerY[mu])):
            if ev.isUpper[mu]:
                continue
            pass
        
        if not (ev.trackpT[mu] > 45. and abs(ev.trackEta[mu]) < 0.9):
            # skip muons that fail the basic Z' kinematic cuts and barrel region
            continue
        if not (abs(ev.dxy[mu]) < 4. and abs(ev.dz[mu]) < 10.):
            continue
        if tightSel:
            # optionally apply tighter pixel requirements
            # if not (abs(ev.dxy[mu]) < 2.5 and abs(ev.dz[mu]) < 10.):
            # if not (abs(ev.dxy[mu]) < 2.5 and abs(ev.dz[mu]) < 10.):
            if not (abs(ev.dxy[mu]) < 1.0 and abs(ev.dz[mu]) < 2.5):
                continue
            pass
        if passMuDen(ev,mu,useTracker):
            if passMuIDTrk(ev,mu,useGlobal,useTracker):
                if passMuID(ev,mu,useGlobal,useTracker):
                    tagIdx = mu
                    tagCount = tagCount + 1
    if tagCount > 0:
        print "Found %d tags"%(tagCount)
    return tagIdx

def findProbeMuon(ev,tagIdx,tightSel=False,useUpper=False,useGlobal=True,useTracker=True,debug=False):
    probeIdx   = -1
    probeCount = 0
    yCompatible = False
    bestDEta = 10
    bestDPhi = 10
    for mu in range(ev.nMuons):
        if mu == tagIdx:
            continue
        if debug:
            print "l%d/u%d"%(ev.isLower[mu],ev.isUpper[mu])
        if useUpper:
            # process upper rather than lower legs
            #if (abs(ev.innerY[mu]) < abs(ev.outerY[mu])):
            if ev.isLower[mu]:
                continue
            pass
        else:
            #if (abs(ev.innerY[mu]) > abs(ev.outerY[mu])):
            if ev.isUpper[mu]:
                continue
            pass
        tmpDEta = abs(ev.trackEta[tagIdx] - ev.trackEta[mu])
        tmpDPhi = dPhi(ev.trackPhi[tagIdx], ev.trackPhi[mu])
        if tmpDEta < bestDEta and tmpDPhi < bestDPhi:
            probeIdx = mu
            bestDEta = tmpDEta
            bestDPhi = tmpDPhi

    return probeIdx

def passMuDen(ev,idx,tracker=False,debug=False):
    result = ev.trackpT[idx] > 45. and abs(ev.trackEta[idx]) < 0.9 #and ev.isTracker[idx]
    if tracker:
        result = result and passMuIDTrk(ev,idx,tracker)
    return result

def passMuID(ev,idx,glbl=True,tracker=False,debug=False):
    passNVMuHits     = (ev.nValidMuonHits[idx] > 0)
    passNMMuStations = (ev.nMatchedStations[idx] > 1)
    passRelPtErr     = ((ev.ptError[idx]/ev.trackpT[idx]) < 0.3)
    result = passNVMuHits and passNMMuStations and passRelPtErr
    if glbl:
        result = result and ev.isGlobal[idx]
    return result #and passMuIDTrk(ev,idx,tracker)#passMuDen(ev,idx,tracker,debug)

def passMuIDTrk(ev,idx,first=False,debug=False):
    passFirstPix = True
    if first:
        passFirstPix = (ev.firstPixel[idx] > 0)
    passNPHits   = (ev.pixelHits[idx] > 0)
    passTKLays   = (ev.tkLayersWMeas[idx] > 5)
    return passFirstPix and passNPHits and passTKLays

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

    return ev.trk_trackpT[10*coll+idx] > 45. and abs(ev.trk_trackEta[10*coll+idx]) < 0.9
    # and passTrkMuIDTrk(ev,idx,coll,match,debug) and isTracker

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
    passFirstPix = (ev.trk_firstPixel[10*coll+idx]    > 0)
    passNPHits   = (ev.trk_pixelHits[10*coll+idx]     > 0)
    passTKLays   = (ev.trk_tkLayersWMeas[10*coll+idx] > 5)
    if match:
        passFirstPix = (ev.firstPixel[muonMatch]    > 0)
        passNPHits   = (ev.pixelHits[muonMatch]     > 0)
        passTKLays   = (ev.tkLayersWMeas[muonMatch] > 5)
    result = passFirstPix and passNPHits and passTKLays and passTrkMuID(ev,idx,coll,match,debug)
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

    print "debug",options.debug
    if options.debug:
        print "debugging turned on"
    else:
        print "debugging turned off"

    ptBins  = np.array([0.,10.,20.,30.,40.,50.,75.,100.,150.,200.,250.,300.,400.,500.,750.,1000.,1500.,2000.,3000.])
    ptBins  = np.array([0., 53., 100., 200., 300., 400., 500., 750., 1000., 1500., 2000.])
    ptBins  = np.array([0., 50., 100., 150., 200., 250., 300., 500., 1000.])
    ptBins  = np.array([0., 50., 100., 150., 200., 300., 500., 1000.])
    ptBins  = np.array([50.,60.,75.,100.,125.,150.,175.,200.,250.,300.,500.,3000.])
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
    
    nMuonsUpperLower = r.TH2D("nMuons","", 10, -0.5,9.5, 10, -0.5,9.5)
    nMuonsUpperLower.Sumw2()
    eventCounter = r.TH1D("eventCounter","", 3, -0.5, 2.5)
    eventCounter.Sumw2()

    # denominator cuts on pT, eta, isTracker and isGlobal
    muDenPixHitHisto          = r.TH2D("muonDenominatorPixHit",         "", len(ptBins)-1, ptBins, 10, -0.5,  9.5)
    muDenTkMeasHisto          = r.TH2D("muonDenominatorTkMeas",         "", len(ptBins)-1, ptBins, 20, -0.5, 19.5)
    muDenRelPtErrorHisto      = r.TH2D("muonDenominatorRelPtError",     "", len(ptBins)-1, ptBins, 100, 0.,   1. )
    muDenValidMuonHitsHisto   = r.TH2D("muonDenominatorValidMuonHits",  "", len(ptBins)-1, ptBins, 75, -0.5, 74.5)
    muDenMatchedStationsHisto = r.TH2D("muonDenominatorMatchedStations","", len(ptBins)-1, ptBins, 10, -0.5,  9.5)
    muDenChi2Histo            = r.TH2D("muonDenominatorChi2",           "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muDenTrkChi2Histo         = r.TH2D("muonDenominatorTrkChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muDenGlbChi2Histo         = r.TH2D("muonDenominatorGlbChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muDenStaChi2Histo         = r.TH2D("muonDenominatorStaChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muDenNormChi2Histo        = r.TH2D("muonDenominatorNormChi2",       "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muDenTrkNormChi2Histo     = r.TH2D("muonDenominatorTrkNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muDenGlbNormChi2Histo     = r.TH2D("muonDenominatorGlbNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muDenStaNormChi2Histo     = r.TH2D("muonDenominatorStaNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muDenDXYHisto             = r.TH2D("muonDenominatorDXY",            "", len(ptBins)-1, ptBins, 100,-10., 10. )
    muDenDZHisto              = r.TH2D("muonDenominatorDZ",             "", len(ptBins)-1, ptBins, 100,-50., 50. )

    muDenPtHisto     = r.TH1D("muonDenominatorPt",   "", 300, 0., 3000.)
    muNum1PtHisto    = r.TH1D("muonPassIDPt",        "", 300, 0., 3000.)
    muNum2PtHisto    = r.TH1D("muonPassIDTrkPt",     "", 300, 0., 3000.)

    muNumFirstPixPtHisto      = r.TH1D("muonPassFirstPix",        "", 300, 0., 3000.)
    muNumFirstPixChi2Histo    = r.TH2D("muonPassFirstPixChi2",    "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumFirstPixTrkChi2Histo = r.TH2D("muonPassFirstPixTrkChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumFirstPixGlbChi2Histo = r.TH2D("muonPassFirstPixGlbChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumFirstPixStaChi2Histo = r.TH2D("muonPassFirstPixStaChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumFirstPixNormChi2Histo    = r.TH2D("muonPassFirstPixNormChi2",    "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumFirstPixTrkNormChi2Histo = r.TH2D("muonPassFirstPixTrkNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumFirstPixGlbNormChi2Histo = r.TH2D("muonPassFirstPixGlbNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumFirstPixStaNormChi2Histo = r.TH2D("muonPassFirstPixStaNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )

    muNumNPixHitPtHisto      = r.TH1D("muonPassNPixHit",        "", 300, 0., 3000.)
    muNumNPixHitChi2Histo    = r.TH2D("muonPassNPixHitChi2",    "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNPixHitTrkChi2Histo = r.TH2D("muonPassNPixHitTrkChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNPixHitGlbChi2Histo = r.TH2D("muonPassNPixHitGlbChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNPixHitStaChi2Histo = r.TH2D("muonPassNPixHitStaChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNPixHitNormChi2Histo    = r.TH2D("muonPassNPixHitNormChi2",    "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNPixHitTrkNormChi2Histo = r.TH2D("muonPassNPixHitTrkNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNPixHitGlbNormChi2Histo = r.TH2D("muonPassNPixHitGlbNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNPixHitStaNormChi2Histo = r.TH2D("muonPassNPixHitStaNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )

    muNumNTkLayersPtHisto      = r.TH1D("muonPassNTkLayers",        "", 300, 0., 3000.)
    muNumNTkLayersChi2Histo    = r.TH2D("muonPassNTkLayersChi2",    "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNTkLayersTrkChi2Histo = r.TH2D("muonPassNTkLayersTrkChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNTkLayersGlbChi2Histo = r.TH2D("muonPassNTkLayersGlbChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNTkLayersStaChi2Histo = r.TH2D("muonPassNTkLayersStaChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNTkLayersNormChi2Histo    = r.TH2D("muonPassNTkLayersNormChi2",    "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNTkLayersTrkNormChi2Histo = r.TH2D("muonPassNTkLayersTrkNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNTkLayersGlbNormChi2Histo = r.TH2D("muonPassNTkLayersGlbNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNTkLayersStaNormChi2Histo = r.TH2D("muonPassNTkLayersStaNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )

    muNumRelPtErrPtHisto      = r.TH1D("muonPassRelPtErr",        "", 300, 0., 3000.)
    muNumRelPtErrChi2Histo    = r.TH2D("muonPassRelPtErrChi2",    "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumRelPtErrTrkChi2Histo = r.TH2D("muonPassRelPtErrTrkChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumRelPtErrGlbChi2Histo = r.TH2D("muonPassRelPtErrGlbChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumRelPtErrStaChi2Histo = r.TH2D("muonPassRelPtErrStaChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumRelPtErrNormChi2Histo    = r.TH2D("muonPassRelPtErrNormChi2",    "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumRelPtErrTrkNormChi2Histo = r.TH2D("muonPassRelPtErrTrkNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumRelPtErrGlbNormChi2Histo = r.TH2D("muonPassRelPtErrGlbNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumRelPtErrStaNormChi2Histo = r.TH2D("muonPassRelPtErrStaNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )

    muNumNValidMuHitPtHisto       = r.TH1D("muonPassNValidMuHits",        "", 300, 0., 3000.)
    muNumNValidMuHitChi2Histo    = r.TH2D("muonPassNValidMuHitsChi2",    "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNValidMuHitTrkChi2Histo = r.TH2D("muonPassNValidMuHitsTrkChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNValidMuHitGlbChi2Histo = r.TH2D("muonPassNValidMuHitsGlbChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNValidMuHitStaChi2Histo = r.TH2D("muonPassNValidMuHitsStaChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNValidMuHitNormChi2Histo    = r.TH2D("muonPassNValidMuHitsNormChi2",    "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNValidMuHitTrkNormChi2Histo = r.TH2D("muonPassNValidMuHitsTrkNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNValidMuHitGlbNormChi2Histo = r.TH2D("muonPassNValidMuHitsGlbNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNValidMuHitStaNormChi2Histo = r.TH2D("muonPassNValidMuHitsStaNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )

    muNumNMuStationsPtHisto      = r.TH1D("muonPassNMuStations",        "", 300, 0., 3000.)
    muNumNMuStationsChi2Histo    = r.TH2D("muonPassNMuStationsChi2",    "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNMuStationsTrkChi2Histo = r.TH2D("muonPassNMuStationsTrkChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNMuStationsGlbChi2Histo = r.TH2D("muonPassNMuStationsGlbChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNMuStationsStaChi2Histo = r.TH2D("muonPassNMuStationsStaChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumNMuStationsNormChi2Histo    = r.TH2D("muonPassNMuStationsNormChi2",    "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNMuStationsTrkNormChi2Histo = r.TH2D("muonPassNMuStationsTrkNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNMuStationsGlbNormChi2Histo = r.TH2D("muonPassNMuStationsGlbNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumNMuStationsStaNormChi2Histo = r.TH2D("muonPassNMuStationsStaNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )

    muNumIsGlobalPtHisto      = r.TH1D("muonPassIsGlobal",        "", 300, 0., 3000.)
    muNumIsGlobalChi2Histo    = r.TH2D("muonPassIsGlobalChi2",    "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumIsGlobalTrkChi2Histo = r.TH2D("muonPassIsGlobalTrkChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumIsGlobalGlbChi2Histo = r.TH2D("muonPassIsGlobalGlbChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumIsGlobalStaChi2Histo = r.TH2D("muonPassIsGlobalStaChi2", "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNumIsGlobalNormChi2Histo    = r.TH2D("muonPassIsGlobalNormChi2",    "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumIsGlobalTrkNormChi2Histo = r.TH2D("muonPassIsGlobalTrkNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumIsGlobalGlbNormChi2Histo = r.TH2D("muonPassIsGlobalGlbNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )
    muNumIsGlobalStaNormChi2Histo = r.TH2D("muonPassIsGlobalStaNormChi2", "", len(ptBins)-1, ptBins, 150,  0., 50. )


    muDenPixHitHisto.Sumw2()
    muDenTkMeasHisto.Sumw2()
    muDenRelPtErrorHisto.Sumw2()
    muDenValidMuonHitsHisto.Sumw2()
    muDenMatchedStationsHisto.Sumw2()
    muDenChi2Histo.Sumw2()
    muDenTrkChi2Histo.Sumw2()
    muDenGlbChi2Histo.Sumw2()
    muDenStaChi2Histo.Sumw2()
    muDenNormChi2Histo.Sumw2()
    muDenTrkNormChi2Histo.Sumw2()
    muDenGlbNormChi2Histo.Sumw2()
    muDenStaNormChi2Histo.Sumw2()
    muDenDXYHisto.Sumw2()
    muDenDZHisto.Sumw2()

    muDenPtHisto.Sumw2()
    muNum1PtHisto.Sumw2()
    muNum2PtHisto.Sumw2()

    muNumFirstPixPtHisto   .Sumw2()
    muNumFirstPixChi2Histo   .Sumw2()
    muNumFirstPixTrkChi2Histo.Sumw2()
    muNumFirstPixGlbChi2Histo.Sumw2()
    muNumFirstPixStaChi2Histo.Sumw2()
    muNumFirstPixNormChi2Histo   .Sumw2()
    muNumFirstPixTrkNormChi2Histo.Sumw2()
    muNumFirstPixGlbNormChi2Histo.Sumw2()
    muNumFirstPixStaNormChi2Histo.Sumw2()

    muNumNPixHitPtHisto    .Sumw2()
    muNumNPixHitChi2Histo   .Sumw2()
    muNumNPixHitTrkChi2Histo.Sumw2()
    muNumNPixHitGlbChi2Histo.Sumw2()
    muNumNPixHitStaChi2Histo.Sumw2()
    muNumNPixHitNormChi2Histo   .Sumw2()
    muNumNPixHitTrkNormChi2Histo.Sumw2()
    muNumNPixHitGlbNormChi2Histo.Sumw2()
    muNumNPixHitStaNormChi2Histo.Sumw2()

    muNumNTkLayersPtHisto  .Sumw2()
    muNumNTkLayersChi2Histo   .Sumw2()
    muNumNTkLayersTrkChi2Histo.Sumw2()
    muNumNTkLayersGlbChi2Histo.Sumw2()
    muNumNTkLayersStaChi2Histo.Sumw2()
    muNumNTkLayersNormChi2Histo   .Sumw2()
    muNumNTkLayersTrkNormChi2Histo.Sumw2()
    muNumNTkLayersGlbNormChi2Histo.Sumw2()
    muNumNTkLayersStaNormChi2Histo.Sumw2()

    muNumRelPtErrPtHisto   .Sumw2()
    muNumRelPtErrChi2Histo   .Sumw2()
    muNumRelPtErrTrkChi2Histo.Sumw2()
    muNumRelPtErrGlbChi2Histo.Sumw2()
    muNumRelPtErrStaChi2Histo.Sumw2()
    muNumRelPtErrNormChi2Histo   .Sumw2()
    muNumRelPtErrTrkNormChi2Histo.Sumw2()
    muNumRelPtErrGlbNormChi2Histo.Sumw2()
    muNumRelPtErrStaNormChi2Histo.Sumw2()

    muNumNValidMuHitPtHisto.Sumw2()
    muNumNValidMuHitChi2Histo   .Sumw2()
    muNumNValidMuHitTrkChi2Histo.Sumw2()
    muNumNValidMuHitGlbChi2Histo.Sumw2()
    muNumNValidMuHitStaChi2Histo.Sumw2()
    muNumNValidMuHitNormChi2Histo   .Sumw2()
    muNumNValidMuHitTrkNormChi2Histo.Sumw2()
    muNumNValidMuHitGlbNormChi2Histo.Sumw2()
    muNumNValidMuHitStaNormChi2Histo.Sumw2()

    muNumNMuStationsPtHisto.Sumw2()
    muNumNMuStationsChi2Histo   .Sumw2()
    muNumNMuStationsTrkChi2Histo.Sumw2()
    muNumNMuStationsGlbChi2Histo.Sumw2()
    muNumNMuStationsStaChi2Histo.Sumw2()
    muNumNMuStationsNormChi2Histo   .Sumw2()
    muNumNMuStationsTrkNormChi2Histo.Sumw2()
    muNumNMuStationsGlbNormChi2Histo.Sumw2()
    muNumNMuStationsStaNormChi2Histo.Sumw2()

    muNumIsGlobalPtHisto   .Sumw2()
    muNumIsGlobalChi2Histo   .Sumw2()
    muNumIsGlobalTrkChi2Histo.Sumw2()
    muNumIsGlobalGlbChi2Histo.Sumw2()
    muNumIsGlobalStaChi2Histo.Sumw2()
    muNumIsGlobalNormChi2Histo   .Sumw2()
    muNumIsGlobalTrkNormChi2Histo.Sumw2()
    muNumIsGlobalGlbNormChi2Histo.Sumw2()
    muNumIsGlobalStaNormChi2Histo.Sumw2()
    
    #  not isGlobal distributions
    muNotGlobalPixHitHisto          = r.TH2D("muonIsNotGlobalPixHit",         "", len(ptBins)-1, ptBins, 10, -0.5,  9.5)
    muNotGlobalTkMeasHisto          = r.TH2D("muonIsNotGlobalTkMeas",         "", len(ptBins)-1, ptBins, 20, -0.5, 19.5)
    muNotGlobalRelPtErrorHisto      = r.TH2D("muonIsNotGlobalRelPtError",     "", len(ptBins)-1, ptBins, 100, 0.,   1. )
    muNotGlobalValidMuonHitsHisto   = r.TH2D("muonIsNotGlobalValidMuonHits",  "", len(ptBins)-1, ptBins, 75, -0.5, 74.5)
    muNotGlobalMatchedStationsHisto = r.TH2D("muonIsNotGlobalMatchedStations","", len(ptBins)-1, ptBins, 10, -0.5,  9.5)
    muNotGlobalChi2Histo            = r.TH2D("muonIsNotGlobalChi2",           "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNotGlobalTrkChi2Histo         = r.TH2D("muonIsNotGlobalTrkChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNotGlobalGlbChi2Histo         = r.TH2D("muonIsNotGlobalGlbChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNotGlobalStaChi2Histo         = r.TH2D("muonIsNotGlobalStaChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muNotGlobalNormChi2Histo        = r.TH2D("muonIsNotGlobalNormChi2",       "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muNotGlobalTrkNormChi2Histo     = r.TH2D("muonIsNotGlobalTrkNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muNotGlobalGlbNormChi2Histo     = r.TH2D("muonIsNotGlobalGlbNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muNotGlobalStaNormChi2Histo     = r.TH2D("muonIsNotGlobalStaNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muNotGlobalDXYHisto             = r.TH2D("muonIsNotGlobalDXY",            "", len(ptBins)-1, ptBins, 100,-10., 10. )
    muNotGlobalDZHisto              = r.TH2D("muonIsNotGlobalDZ",             "", len(ptBins)-1, ptBins, 100,-50., 50. )

    muNotGlobalPixHitHisto.Sumw2()
    muNotGlobalTkMeasHisto.Sumw2()
    muNotGlobalRelPtErrorHisto.Sumw2()
    muNotGlobalValidMuonHitsHisto.Sumw2()
    muNotGlobalMatchedStationsHisto.Sumw2()
    muNotGlobalChi2Histo.Sumw2()
    muNotGlobalTrkChi2Histo.Sumw2()
    muNotGlobalGlbChi2Histo.Sumw2()
    muNotGlobalStaChi2Histo.Sumw2()
    muNotGlobalNormChi2Histo.Sumw2()
    muNotGlobalTrkNormChi2Histo.Sumw2()
    muNotGlobalGlbNormChi2Histo.Sumw2()
    muNotGlobalStaNormChi2Histo.Sumw2()
    muNotGlobalDXYHisto.Sumw2()
    muNotGlobalDZHisto.Sumw2()

    # denominator adds cuts on pixel hits and tracker layers
    muIsTrkDenPixHitHisto          = r.TH2D("muonIsTrkDenominatorPixHit",         "", len(ptBins)-1, ptBins, 10, -0.5,  9.5)
    muIsTrkDenTkMeasHisto          = r.TH2D("muonIsTrkDenominatorTkMeas",         "", len(ptBins)-1, ptBins, 20, -0.5, 19.5)
    muIsTrkDenRelPtErrorHisto      = r.TH2D("muonIsTrkDenominatorRelPtError",     "", len(ptBins)-1, ptBins, 100, 0.,   1. )
    muIsTrkDenValidMuonHitsHisto   = r.TH2D("muonIsTrkDenominatorValidMuonHits",  "", len(ptBins)-1, ptBins, 75, -0.5, 74.5)
    muIsTrkDenMatchedStationsHisto = r.TH2D("muonIsTrkDenominatorMatchedStations","", len(ptBins)-1, ptBins, 10, -0.5,  9.5)
    muIsTrkDenChi2Histo            = r.TH2D("muonIsTrkDenominatorChi2",           "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsTrkDenTrkChi2Histo         = r.TH2D("muonIsTrkDenominatorTrkChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsTrkDenGlbChi2Histo         = r.TH2D("muonIsTrkDenominatorGlbChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsTrkDenStaChi2Histo         = r.TH2D("muonIsTrkDenominatorStaChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsTrkDenNormChi2Histo        = r.TH2D("muonIsTrkDenominatorNormChi2",       "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsTrkDenTrkNormChi2Histo     = r.TH2D("muonIsTrkDenominatorTrkNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsTrkDenGlbNormChi2Histo     = r.TH2D("muonIsTrkDenominatorGlbNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsTrkDenStaNormChi2Histo     = r.TH2D("muonIsTrkDenominatorStaNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsTrkDenDXYHisto             = r.TH2D("muonIsTrkDenominatorDXY",            "", len(ptBins)-1, ptBins, 100,-10., 10. )
    muIsTrkDenDZHisto              = r.TH2D("muonIsTrkDenominatorDZ",             "", len(ptBins)-1, ptBins, 100,-50., 50. )

    muIsTrkDenPtHisto     = r.TH1D("muonIsTrkDenominatorPt","", 300, 0., 3000.)
    muIsTrkNum1PtHisto    = r.TH1D("muonIsTrkPassIDPt",     "", 300, 0., 3000.)
    muIsTrkNum2PtHisto    = r.TH1D("muonIsTrkPassIDTrkPt",  "", 300, 0., 3000.)

    muIsTrkDenPtHisto.Sumw2()
    muIsTrkDenPixHitHisto.Sumw2()
    muIsTrkDenTkMeasHisto.Sumw2()
    muIsTrkDenRelPtErrorHisto.Sumw2()
    muIsTrkDenValidMuonHitsHisto.Sumw2()
    muIsTrkDenMatchedStationsHisto.Sumw2()
    muIsTrkDenChi2Histo.Sumw2()
    muIsTrkDenTrkChi2Histo.Sumw2()
    muIsTrkDenGlbChi2Histo.Sumw2()
    muIsTrkDenStaChi2Histo.Sumw2()
    muIsTrkDenNormChi2Histo.Sumw2()
    muIsTrkDenTrkNormChi2Histo.Sumw2()
    muIsTrkDenGlbNormChi2Histo.Sumw2()
    muIsTrkDenStaNormChi2Histo.Sumw2()
    muIsTrkDenDXYHisto.Sumw2()
    muIsTrkDenDZHisto.Sumw2()
    muIsTrkNum1PtHisto.Sumw2()
    muIsTrkNum2PtHisto.Sumw2()

    # Probe isGlobal in the numerator, denominator only has pT, eta, and isTracker
    muIsGlbDenPixHitHisto          = r.TH2D("muonIsGlbDenominatorPixHit",         "", len(ptBins)-1, ptBins, 10, -0.5,  9.5)
    muIsGlbDenTkMeasHisto          = r.TH2D("muonIsGlbDenominatorTkMeas",         "", len(ptBins)-1, ptBins, 20, -0.5, 19.5)
    muIsGlbDenRelPtErrorHisto      = r.TH2D("muonIsGlbDenominatorRelPtError",     "", len(ptBins)-1, ptBins, 100, 0.,   1. )
    muIsGlbDenValidMuonHitsHisto   = r.TH2D("muonIsGlbDenominatorValidMuonHits",  "", len(ptBins)-1, ptBins, 75, -0.5, 74.5)
    muIsGlbDenMatchedStationsHisto = r.TH2D("muonIsGlbDenominatorMatchedStations","", len(ptBins)-1, ptBins, 10, -0.5,  9.5)
    muIsGlbDenChi2Histo            = r.TH2D("muonIsGlbDenominatorChi2",           "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsGlbDenTrkChi2Histo         = r.TH2D("muonIsGlbDenominatorTrkChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsGlbDenGlbChi2Histo         = r.TH2D("muonIsGlbDenominatorGlbChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsGlbDenStaChi2Histo         = r.TH2D("muonIsGlbDenominatorStaChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsGlbDenNormChi2Histo        = r.TH2D("muonIsGlbDenominatorNormChi2",       "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsGlbDenTrkNormChi2Histo     = r.TH2D("muonIsGlbDenominatorTrkNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsGlbDenGlbNormChi2Histo     = r.TH2D("muonIsGlbDenominatorGlbNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsGlbDenStaNormChi2Histo     = r.TH2D("muonIsGlbDenominatorStaNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsGlbDenDXYHisto             = r.TH2D("muonIsGlbDenominatorDXY",            "", len(ptBins)-1, ptBins, 100,-10., 10. )
    muIsGlbDenDZHisto              = r.TH2D("muonIsGlbDenominatorDZ",             "", len(ptBins)-1, ptBins, 100,-50., 50. )

    muIsGlbDenPtHisto     = r.TH1D("muonIsGlbDenominatorPt","", 300, 0., 3000.)
    muIsGlbNum1PtHisto    = r.TH1D("muonIsGlbPassIDPt",     "", 300, 0., 3000.)
    muIsGlbNum2PtHisto    = r.TH1D("muonIsGlbPassIDTrkPt",  "", 300, 0., 3000.)

    muIsGlbDenPixHitHisto.Sumw2()
    muIsGlbDenTkMeasHisto.Sumw2()
    muIsGlbDenRelPtErrorHisto.Sumw2()
    muIsGlbDenValidMuonHitsHisto.Sumw2()
    muIsGlbDenMatchedStationsHisto.Sumw2()
    muIsGlbDenTrkChi2Histo.Sumw2()
    muIsGlbDenGlbChi2Histo.Sumw2()
    muIsGlbDenStaChi2Histo.Sumw2()
    muIsGlbDenChi2Histo.Sumw2()
    muIsGlbDenTrkNormChi2Histo.Sumw2()
    muIsGlbDenGlbNormChi2Histo.Sumw2()
    muIsGlbDenStaNormChi2Histo.Sumw2()
    muIsGlbDenNormChi2Histo.Sumw2()
    muIsGlbDenDXYHisto.Sumw2()
    muIsGlbDenDZHisto.Sumw2()

    muIsGlbDenPtHisto.Sumw2()
    muIsGlbNum1PtHisto.Sumw2()
    muIsGlbNum2PtHisto.Sumw2()
    
    # denominator adds cuts on pixel hits and tracker layers
    muIsGlbIsTrkDenPixHitHisto          = r.TH2D("muonIsGlbIsTrkDenominatorPixHit",         "", len(ptBins)-1, ptBins, 10, -0.5,  9.5)
    muIsGlbIsTrkDenTkMeasHisto          = r.TH2D("muonIsGlbIsTrkDenominatorTkMeas",         "", len(ptBins)-1, ptBins, 20, -0.5, 19.5)
    muIsGlbIsTrkDenRelPtErrorHisto      = r.TH2D("muonIsGlbIsTrkDenominatorRelPtError",     "", len(ptBins)-1, ptBins, 100, 0.,   1. )
    muIsGlbIsTrkDenValidMuonHitsHisto   = r.TH2D("muonIsGlbIsTrkDenominatorValidMuonHits",  "", len(ptBins)-1, ptBins, 75, -0.5, 74.5)
    muIsGlbIsTrkDenMatchedStationsHisto = r.TH2D("muonIsGlbIsTrkDenominatorMatchedStations","", len(ptBins)-1, ptBins, 10, -0.5,  9.5)
    muIsGlbIsTrkDenChi2Histo            = r.TH2D("muonIsGlbIsTrkDenominatorChi2",           "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsGlbIsTrkDenTrkChi2Histo         = r.TH2D("muonIsGlbIsTrkDenominatorTrkChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsGlbIsTrkDenGlbChi2Histo         = r.TH2D("muonIsGlbIsTrkDenominatorGlbChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsGlbIsTrkDenStaChi2Histo         = r.TH2D("muonIsGlbIsTrkDenominatorStaChi2",        "", len(ptBins)-1, ptBins, 50,  0., 150. )
    muIsGlbIsTrkDenNormChi2Histo        = r.TH2D("muonIsGlbIsTrkDenominatorNormChi2",       "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsGlbIsTrkDenTrkNormChi2Histo     = r.TH2D("muonIsGlbIsTrkDenominatorTrkNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsGlbIsTrkDenGlbNormChi2Histo     = r.TH2D("muonIsGlbIsTrkDenominatorGlbNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsGlbIsTrkDenStaNormChi2Histo     = r.TH2D("muonIsGlbIsTrkDenominatorStaNormChi2",    "", len(ptBins)-1, ptBins, 150, 0.,  50. )
    muIsGlbIsTrkDenDXYHisto             = r.TH2D("muonIsGlbIsTrkDenominatorDXY",            "", len(ptBins)-1, ptBins, 100,-10., 10. )
    muIsGlbIsTrkDenDZHisto              = r.TH2D("muonIsGlbIsTrkDenominatorDZ",             "", len(ptBins)-1, ptBins, 100,-50., 50. )

    muIsGlbIsTrkDenPtHisto     = r.TH1D("muonIsGlbIsTrkDenominatorPt","", 300, 0., 3000.)
    muIsGlbIsTrkNum1PtHisto    = r.TH1D("muonIsGlbIsTrkPassIDPt",     "", 300, 0., 3000.)
    muIsGlbIsTrkNum2PtHisto    = r.TH1D("muonIsGlbIsTrkPassIDTrkPt",  "", 300, 0., 3000.)

    muIsGlbIsTrkDenPixHitHisto.Sumw2()
    muIsGlbIsTrkDenTkMeasHisto.Sumw2()
    muIsGlbIsTrkDenRelPtErrorHisto.Sumw2()
    muIsGlbIsTrkDenValidMuonHitsHisto.Sumw2()
    muIsGlbIsTrkDenMatchedStationsHisto.Sumw2()
    muIsGlbIsTrkDenChi2Histo.Sumw2()
    muIsGlbIsTrkDenTrkChi2Histo.Sumw2()
    muIsGlbIsTrkDenGlbChi2Histo.Sumw2()
    muIsGlbIsTrkDenStaChi2Histo.Sumw2()
    muIsGlbIsTrkDenNormChi2Histo.Sumw2()
    muIsGlbIsTrkDenTrkNormChi2Histo.Sumw2()
    muIsGlbIsTrkDenGlbNormChi2Histo.Sumw2()
    muIsGlbIsTrkDenStaNormChi2Histo.Sumw2()
    muIsGlbIsTrkDenDXYHisto.Sumw2()
    muIsGlbIsTrkDenDZHisto.Sumw2()

    muIsGlbIsTrkDenPtHisto.Sumw2()
    muIsGlbIsTrkNum1PtHisto.Sumw2()
    muIsGlbIsTrkNum2PtHisto.Sumw2()
    
    # histograms for study starting from raw track collection.
    trkDenPtHisto  = r.TH1D("trackDenominatorPt","", 300, 0., 3000.)
    trkNum1PtHisto = r.TH1D("trackPassIDPt",     "", 300, 0., 3000.)
    trkNum2PtHisto = r.TH1D("trackPassIDTrkPt",  "", 300, 0., 3000.)
    trkDenPtHisto.Sumw2()
    trkNum1PtHisto.Sumw2()
    trkNum2PtHisto.Sumw2()
    
    trkMuDenPtHisto  = r.TH1D("trackMuDenominatorPt","", 300, 0., 3000.)
    trkMuNum1PtHisto = r.TH1D("trackMuPassIDPt",     "", 300, 0., 3000.)
    trkMuNum2PtHisto = r.TH1D("trackMuPassIDTrkPt",  "", 300, 0., 3000.)
    trkMuDenPtHisto.Sumw2()
    trkMuNum1PtHisto.Sumw2()
    trkMuNum2PtHisto.Sumw2()
    
    nEvents = mytree.GetEntries()
    eid = 0
    checkUpper = False
    if options.upper:
        checkUpper = True
    print "checkUpper %d"%(checkUpper)
    strange = open("%s_interesting.events.txt"%(options.outfile),"w")
    for event in mytree:
        if eid%1000 == 0:
            print "event=%d/%d: g=%d c=%d t=%d"%(eid,nEvents,event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks)
        if options.debug and eid%10 == 0:
            print "event=%d/%d: g=%d c=%d t=%d"%(eid,nEvents,event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks)
        nTracks = [event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks]

        if isGoodEvent(event,checkUpper,options.debug):
            # should have at least one muon with pT > 45. and |eta| < 0.9
            nMuonsUpperLower.Fill(event.nUpperLegs,event.nLowerLegs)
            eventCounter.Fill(0)
            # this will reject any events where we find duplicate muons
            # can we be clever and keep the muon that is 1t/1g/1sa if it exists?
            # or will this be an additional bias?
            if findFunky(event,checkUpper,options.debug):
                # skip events where we find the "duplicate" muons
                # write out run/lumi/event for further analysis?
                strange.write("funky: %d/%d/%d\n"%(event.run,event.lumi,event.event))
                eventCounter.Fill(2)
                continue
            eventCounter.Fill(1)
            if options.debug:
                print "nTracks=%d - %d/%d/%d\n"%(event.nTrackerTracks,event.run,event.lumi,event.event)
                pass
            tagIdx = findTagTrack(event,False,checkUpper,True,True,options.debug)
            if tagIdx < 0:
                if options.debug:
                    print "no track tag found, tagIdx:%d"%(tagIdx)
                continue
            probeIdx = findTrkProbeMuon(event,tagIdx,False,checkUpper,True,True,options.debug)
            print "tag=%d, probe=%d, nUppers=%d - %d/%d/%d\n"%(tagIdx,probeIdx,event.nUpperLegs,event.run,event.lumi,event.event)
            print "tag:%d-l%d/u%d:q%d pT:%2.4f,phi:%2.4f,eta:%2.4f"%(
                tagIdx,event.trk_isLower[10*2+tagIdx],event.trk_isUpper[10*2+tagIdx],
                event.trk_charge[10*2+tagIdx],
                event.trk_trackpT[10*2+tagIdx],event.trk_trackPhi[10*2+tagIdx],event.trk_trackEta[10*2+tagIdx])
            print "probe:%d-l%d/u%d:q%d (%dt/%dg/%dsa) pT:%2.4f,phi:%2.4f,eta:%2.4f"%(
                probeIdx,event.isLower[probeIdx],event.isUpper[probeIdx],event.charge[probeIdx],
                event.isTracker[probeIdx],event.isGlobal[probeIdx],event.isStandAlone[probeIdx],
                event.trackpT[probeIdx],event.trackPhi[probeIdx],event.trackEta[probeIdx])
            for mu in range(event.nMuons):
                if event.isUpper[mu]:
                    print "up:%d-l%d/u%d:q%d (%dt/%dg/%dsa) pT:%2.4f,phi:%2.4f,eta:%2.4f,dxy:%2.2f,dz:%2.2f"%(
                        mu,event.isLower[mu],event.isUpper[mu],
                        event.charge[mu],event.isTracker[mu],event.isGlobal[mu],event.isStandAlone[mu],
                        event.trackpT[mu],event.trackPhi[mu],event.trackEta[mu],
                        event.dxy[mu],event.dz[mu])

            #  you should for example use as a denominator the inner track with just kinematical and impact parameter cuts
            if (passTrkDen(event,tagIdx,2,False,options.debug)):
                muDenPtHisto.Fill(event.trackpT[probeIdx])
                muDenTkMeasHisto.Fill(event.trackpT[probeIdx],event.tkLayersWMeas[probeIdx])
                muDenPixHitHisto.Fill(event.trackpT[probeIdx],event.pixelHits[probeIdx])
                muDenRelPtErrorHisto.Fill(event.trackpT[probeIdx],event.ptError[probeIdx]/event.trackpT[probeIdx])
                muDenValidMuonHitsHisto.Fill(event.trackpT[probeIdx],event.nValidMuonHits[probeIdx])
                muDenMatchedStationsHisto.Fill(event.trackpT[probeIdx],event.nMatchedStations[probeIdx])
                fillChi2Hists(event,probeIdx,muDenChi2Histo,muDenTrkChi2Histo,muDenGlbChi2Histo,muDenStaChi2Histo)
                fillChi2Hists(event,probeIdx,muDenNormChi2Histo,muDenTrkNormChi2Histo,muDenGlbNormChi2Histo,muDenStaChi2Histo,True)
                muDenDXYHisto.Fill(event.trackpT[probeIdx],event.dxy[probeIdx])
                muDenDZHisto.Fill(event.trackpT[probeIdx],event.dz[probeIdx])
                if (passMuID(event,probeIdx,False,False,options.debug)):
                    muNum1PtHisto.Fill(event.trackpT[probeIdx])
                if (passMuIDTrk(event,probeIdx,False,options.debug)):
                    muNum2PtHisto.Fill(event.trackpT[probeIdx])
                    pass

            # 
            if (passTrkDen(event,tagIdx,2,False,options.debug)):
                muIsTrkDenPtHisto.Fill(event.trackpT[probeIdx])
                muIsTrkDenTkMeasHisto.Fill(event.trackpT[probeIdx],event.tkLayersWMeas[probeIdx])
                muIsTrkDenPixHitHisto.Fill(event.trackpT[probeIdx],event.pixelHits[probeIdx])
                muIsTrkDenRelPtErrorHisto.Fill(event.trackpT[probeIdx],event.ptError[probeIdx]/event.trackpT[probeIdx])
                muIsTrkDenValidMuonHitsHisto.Fill(event.trackpT[probeIdx],event.nValidMuonHits[probeIdx])
                muIsTrkDenMatchedStationsHisto.Fill(event.trackpT[probeIdx],event.nMatchedStations[probeIdx])
                fillChi2Hists(event,probeIdx,muIsTrkDenChi2Histo,muIsTrkDenTrkChi2Histo,
                              muIsTrkDenGlbChi2Histo,muIsTrkDenStaChi2Histo)
                fillChi2Hists(event,probeIdx,muIsTrkDenNormChi2Histo,muIsTrkDenTrkNormChi2Histo,
                              muIsTrkDenGlbNormChi2Histo,muIsTrkDenStaChi2Histo,True)
                muIsTrkDenDXYHisto.Fill(event.trackpT[probeIdx],event.dxy[probeIdx])
                muIsTrkDenDZHisto.Fill(event.trackpT[probeIdx],event.dz[probeIdx])
                if (passMuID(event,probeIdx,False,False,options.debug)):
                    muIsTrkNum1PtHisto.Fill(event.trackpT[probeIdx])
                if (passMuIDTrk(event,probeIdx,True,options.debug)):
                    muIsTrkNum2PtHisto.Fill(event.trackpT[probeIdx])
                
            # probing isGlobal in the numerator
            # denominator cuts do not include track ID cuts, but include isTracker
            if (passTrkDen(event,tagIdx,2,False,options.debug)):
                #if event.isGlobal[probeIdx]:
                if event.firstPixel[probeIdx] > 0:
                    muNumFirstPixPtHisto.Fill(event.trackpT[probeIdx])
                    fillChi2Hists(event,probeIdx,muNumFirstPixChi2Histo,muNumFirstPixTrkChi2Histo,
                                  muNumFirstPixGlbChi2Histo,muNumFirstPixStaChi2Histo)
                    fillChi2Hists(event,probeIdx,muNumFirstPixNormChi2Histo,muNumFirstPixTrkNormChi2Histo,
                                  muNumFirstPixGlbNormChi2Histo,muNumFirstPixStaChi2Histo,True)
                    
                if event.pixelHits[probeIdx] > 0:
                    muNumNPixHitPtHisto.Fill(event.trackpT[probeIdx])
                    fillChi2Hists(event,probeIdx,muNumNPixHitChi2Histo,muNumNPixHitTrkChi2Histo,
                                  muNumNPixHitGlbChi2Histo,muNumNPixHitStaChi2Histo)
                    fillChi2Hists(event,probeIdx,muNumNPixHitNormChi2Histo,muNumNPixHitTrkNormChi2Histo,
                                  muNumNPixHitGlbNormChi2Histo,muNumNPixHitStaChi2Histo,True)
                    
                if event.tkLayersWMeas[probeIdx] > 5:
                    muNumNTkLayersPtHisto.Fill(event.trackpT[probeIdx])
                    fillChi2Hists(event,probeIdx,muNumNTkLayersChi2Histo,muNumNTkLayersTrkChi2Histo,
                                  muNumNTkLayersGlbChi2Histo,muNumNTkLayersStaChi2Histo)
                    fillChi2Hists(event,probeIdx,muNumNTkLayersNormChi2Histo,muNumNTkLayersTrkNormChi2Histo,
                                  muNumNTkLayersGlbNormChi2Histo,muNumNTkLayersStaChi2Histo,True)
                    
                if (event.ptError[probeIdx]/event.trackpT[probeIdx]) < 0.3:
                    muNumRelPtErrPtHisto.Fill(event.trackpT[probeIdx])
                    fillChi2Hists(event,probeIdx,muNumRelPtErrChi2Histo,muNumRelPtErrTrkChi2Histo,
                                  muNumRelPtErrGlbChi2Histo,muNumRelPtErrStaChi2Histo)
                    fillChi2Hists(event,probeIdx,muNumRelPtErrNormChi2Histo,muNumRelPtErrTrkNormChi2Histo,
                                  muNumRelPtErrGlbNormChi2Histo,muNumRelPtErrStaChi2Histo,True)
                    
                if event.nValidMuonHits[probeIdx] > 0:
                    muNumNValidMuHitPtHisto.Fill(event.trackpT[probeIdx])
                    fillChi2Hists(event,probeIdx,muNumNValidMuHitChi2Histo,muNumNValidMuHitTrkChi2Histo,
                                  muNumNValidMuHitGlbChi2Histo,muNumNValidMuHitStaChi2Histo)
                    fillChi2Hists(event,probeIdx,muNumNValidMuHitNormChi2Histo,muNumNValidMuHitTrkNormChi2Histo,
                                  muNumNValidMuHitGlbNormChi2Histo,muNumNValidMuHitStaChi2Histo,True)
                    
                if event.nMatchedStations[probeIdx] > 1:
                    muNumNMuStationsPtHisto.Fill(event.trackpT[probeIdx])
                    fillChi2Hists(event,probeIdx,muNumNMuStationsChi2Histo,muNumNMuStationsTrkChi2Histo,
                                  muNumNMuStationsGlbChi2Histo,muNumNMuStationsStaChi2Histo)
                    fillChi2Hists(event,probeIdx,muNumNMuStationsNormChi2Histo,muNumNMuStationsTrkNormChi2Histo,
                                  muNumNMuStationsGlbNormChi2Histo,muNumNMuStationsStaChi2Histo,True)
                    
                if event.isGlobal[probeIdx] > 0:
                    muNumIsGlobalPtHisto.Fill(event.trackpT[probeIdx])
                    fillChi2Hists(event,probeIdx,muNumIsGlobalChi2Histo,muNumIsGlobalTrkChi2Histo,
                                  muNumIsGlobalGlbChi2Histo,muNumIsGlobalStaChi2Histo)
                    fillChi2Hists(event,probeIdx,muNumIsGlobalNormChi2Histo,muNumIsGlobalTrkNormChi2Histo,
                                  muNumIsGlobalGlbNormChi2Histo,muNumIsGlobalStaChi2Histo,True)
                
                else:
                    # write out run/lumi/event for further analysis?
                    strange.write("ng: %d/%d/%d\n"%(event.run,event.lumi,event.event))
                    # maybe also look at distributions for failing global cuts
                    muNotGlobalPixHitHisto.Fill(event.trackpT[probeIdx],event.pixelHits[probeIdx])
                    muNotGlobalTkMeasHisto.Fill(event.trackpT[probeIdx],event.tkLayersWMeas[probeIdx])
                    muNotGlobalRelPtErrorHisto.Fill(event.trackpT[probeIdx],event.ptError[probeIdx]/event.trackpT[probeIdx])
                    muNotGlobalValidMuonHitsHisto.Fill(event.trackpT[probeIdx],event.nValidMuonHits[probeIdx])
                    muNotGlobalMatchedStationsHisto.Fill(event.trackpT[probeIdx],event.nMatchedStations[probeIdx])
                    muNotGlobalChi2Histo.Fill(event.trackpT[probeIdx],event.chi2[probeIdx])
                    muNotGlobalTrkChi2Histo.Fill(event.trackpT[probeIdx],event.innerChi2[probeIdx])
                    muNotGlobalGlbChi2Histo.Fill(event.trackpT[probeIdx],event.globalChi2[probeIdx])
                    muNotGlobalStaChi2Histo.Fill(event.trackpT[probeIdx],event.outerChi2[probeIdx])
                    if event.ndof[probeIdx] > 0:
                        muNotGlobalNormChi2Histo.Fill(event.trackpT[probeIdx],event.chi2[probeIdx]/event.ndof[probeIdx])
                    if event.innerNDF[probeIdx] > 0:
                        muNotGlobalTrkNormChi2Histo.Fill(event.trackpT[probeIdx],event.innerChi2[probeIdx]/event.innerNDF[probeIdx])
                    if event.globalNDF[probeIdx] > 0:
                        muNotGlobalGlbNormChi2Histo.Fill(event.trackpT[probeIdx],event.globalChi2[probeIdx]/event.globalNDF[probeIdx])
                    if event.outerNDF[probeIdx] > 0:
                        muNotGlobalStaNormChi2Histo.Fill(event.trackpT[probeIdx],event.outerChi2[probeIdx]/event.outerNDF[probeIdx])
                    muNotGlobalDXYHisto.Fill(event.trackpT[probeIdx],event.dxy[probeIdx])
                    muNotGlobalDZHisto.Fill(event.trackpT[probeIdx],event.dz[probeIdx])
                    
                    pass
                
                muIsGlbDenPtHisto.Fill(event.trackpT[probeIdx])
                muIsGlbDenTkMeasHisto.Fill(event.trackpT[probeIdx],event.tkLayersWMeas[probeIdx])
                muIsGlbDenPixHitHisto.Fill(event.trackpT[probeIdx],event.pixelHits[probeIdx])
                muIsGlbDenRelPtErrorHisto.Fill(event.trackpT[probeIdx],event.ptError[probeIdx]/event.trackpT[probeIdx])
                muIsGlbDenValidMuonHitsHisto.Fill(event.trackpT[probeIdx],event.nValidMuonHits[probeIdx])
                muIsGlbDenMatchedStationsHisto.Fill(event.trackpT[probeIdx],event.nMatchedStations[probeIdx])
                fillChi2Hists(event,probeIdx,muIsGlbDenChi2Histo,muIsGlbDenTrkChi2Histo,
                              muIsGlbDenGlbChi2Histo,muIsGlbDenStaChi2Histo)
                fillChi2Hists(event,probeIdx,muIsGlbDenNormChi2Histo,muIsGlbDenTrkNormChi2Histo,
                              muIsGlbDenGlbNormChi2Histo,muIsGlbDenStaChi2Histo,True)
                muIsGlbDenDXYHisto.Fill(event.trackpT[probeIdx],event.dxy[probeIdx])
                muIsGlbDenDZHisto.Fill(event.trackpT[probeIdx],event.dz[probeIdx])
                if (passMuID(event,probeIdx,True,False,options.debug)):
                    muIsGlbNum1PtHisto.Fill(event.trackpT[probeIdx])
                if (passMuIDTrk(event,probeIdx,False,options.debug)):
                    muIsGlbNum2PtHisto.Fill(event.trackpT[probeIdx])
                    
            # 
            if (passTrkDen(event,tagIdx,2,False,options.debug)):
                muIsGlbIsTrkDenPtHisto.Fill(event.trackpT[probeIdx])
                muIsGlbIsTrkDenTkMeasHisto.Fill(event.trackpT[probeIdx],event.tkLayersWMeas[probeIdx])
                muIsGlbIsTrkDenPixHitHisto.Fill(event.trackpT[probeIdx],event.pixelHits[probeIdx])
                muIsGlbIsTrkDenRelPtErrorHisto.Fill(event.trackpT[probeIdx],event.ptError[probeIdx]/event.trackpT[probeIdx])
                muIsGlbIsTrkDenValidMuonHitsHisto.Fill(event.trackpT[probeIdx],event.nValidMuonHits[probeIdx])
                muIsGlbIsTrkDenMatchedStationsHisto.Fill(event.trackpT[probeIdx],event.nMatchedStations[probeIdx])
                fillChi2Hists(event,probeIdx,muIsGlbIsTrkDenChi2Histo,muIsGlbIsTrkDenTrkChi2Histo,
                              muIsGlbIsTrkDenGlbChi2Histo,muIsGlbIsTrkDenStaChi2Histo)
                fillChi2Hists(event,probeIdx,muIsGlbIsTrkDenNormChi2Histo,muIsGlbIsTrkDenTrkNormChi2Histo,
                              muIsGlbIsTrkDenGlbNormChi2Histo,muIsGlbIsTrkDenStaChi2Histo,True)
                muIsGlbIsTrkDenDXYHisto.Fill(event.trackpT[probeIdx],event.dxy[probeIdx])
                muIsGlbIsTrkDenDZHisto.Fill(event.trackpT[probeIdx],event.dz[probeIdx])
                if (passMuID(event,probeIdx,True,False,options.debug)):
                    muIsGlbIsTrkNum1PtHisto.Fill(event.trackpT[probeIdx])
                if (passMuIDTrk(event,probeIdx,True,options.debug)):
                    muIsGlbIsTrkNum2PtHisto.Fill(event.trackpT[probeIdx])
            
            continue ## no longer use the old study
            for mu in range(event.nMuons):
                if not (event.trackpT[mu] > 45. and abs(event.trackEta[mu]) < 0.9):
                    # skip muons that fail the basic Z' kinematic cuts and barrel region
                    continue
                if options.tight:
                    # optionally apply tighter pixel requirements
                    # if not (abs(event.dxy[mu]) < 2.5 and abs(event.dz[mu]) < 10.):
                    # if not (abs(event.dxy[mu]) < 2.5 and abs(event.dz[mu]) < 10.):
                    if not (abs(event.dxy[mu]) < 1.0 and abs(event.dz[mu]) < 2.5):
                        continue
                    pass
                if checkUpper:
                    # process upper rather than lower legs
                    # if isLower[mu]
                    if (abs(event.innerY[mu]) < abs(event.outerY[mu])):
                        continue
                    pass
                else:
                    # if isUpper[mu]
                    if (abs(event.innerY[mu]) > abs(event.outerY[mu])):
                        continue
                    pass
                
                # just require first pixel layer for everything
                if not (event.firstPixel[mu] > 0):
                    #continue
                    pass
                
            
                # first go, don't probe isGlobal in the numerator, but require it in the full selection
                if event.isGlobal[mu] > 0:
                    pass
                # denominator cuts do not include track ID cuts, but include isTracker
                if (passMuDen(event,mu,False,options.debug)):
                    muDenPtHisto.Fill(event.trackpT[mu])
                    muDenTkMeasHisto.Fill(event.trackpT[mu],event.tkLayersWMeas[mu])
                    muDenPixHitHisto.Fill(event.trackpT[mu],event.pixelHits[mu])
                    muDenRelPtErrorHisto.Fill(event.trackpT[mu],event.ptError[mu]/event.trackpT[mu])
                    muDenValidMuonHitsHisto.Fill(event.trackpT[mu],event.nValidMuonHits[mu])
                    muDenMatchedStationsHisto.Fill(event.trackpT[mu],event.nMatchedStations[mu])
                    fillChi2Hists(event,mu,muDenChi2Histo,muDenTrkChi2Histo,muDenGlbChi2Histo,muDenStaChi2Histo)
                    fillChi2Hists(event,mu,muDenNormChi2Histo,muDenTrkNormChi2Histo,muDenGlbNormChi2Histo,muDenStaChi2Histo,True)
                    muDenDXYHisto.Fill(event.trackpT[mu],event.dxy[mu])
                    muDenDZHisto.Fill(event.trackpT[mu],event.dz[mu])
                    if (passMuID(event,mu,False,False,options.debug)):
                        muNum1PtHisto.Fill(event.trackpT[mu])
                    if (passMuIDTrk(event,mu,True,options.debug)):
                        muNum2PtHisto.Fill(event.trackpT[mu])
                        
                # denominator cuts include track ID cuts, in addition to isTracker
                if (passMuDen(event,mu,True,options.debug)):
                    muIsTrkDenPtHisto.Fill(event.trackpT[mu])
                    muIsTrkDenTkMeasHisto.Fill(event.trackpT[mu],event.tkLayersWMeas[mu])
                    muIsTrkDenPixHitHisto.Fill(event.trackpT[mu],event.pixelHits[mu])
                    muIsTrkDenRelPtErrorHisto.Fill(event.trackpT[mu],event.ptError[mu]/event.trackpT[mu])
                    muIsTrkDenValidMuonHitsHisto.Fill(event.trackpT[mu],event.nValidMuonHits[mu])
                    muIsTrkDenMatchedStationsHisto.Fill(event.trackpT[mu],event.nMatchedStations[mu])
                    fillChi2Hists(event,mu,muIsTrkDenChi2Histo,muIsTrkDenTrkChi2Histo,
                                  muIsTrkDenGlbChi2Histo,muIsTrkDenStaChi2Histo)
                    fillChi2Hists(event,mu,muIsTrkDenNormChi2Histo,muIsTrkDenTrkNormChi2Histo,
                                  muIsTrkDenGlbNormChi2Histo,muIsTrkDenStaChi2Histo,True)
                    muIsTrkDenDXYHisto.Fill(event.trackpT[mu],event.dxy[mu])
                    muIsTrkDenDZHisto.Fill(event.trackpT[mu],event.dz[mu])
                    if (passMuID(event,mu,False,True,options.debug)):
                        muIsTrkNum1PtHisto.Fill(event.trackpT[mu])
                    if (passMuIDTrk(event,mu,True,options.debug)):
                        muIsTrkNum2PtHisto.Fill(event.trackpT[mu])
                    
                # probing isGlobal in the numerator
                # denominator cuts do not include track ID cuts, but include isTracker
                if (passMuDen(event,mu,False,options.debug)):
                    #if event.isGlobal[mu]:
                    if event.firstPixel[mu] > 0:
                        muNumFirstPixPtHisto.Fill(event.trackpT[mu])
                        fillChi2Hists(event,mu,muNumFirstPixChi2Histo,muNumFirstPixTrkChi2Histo,
                                      muNumFirstPixGlbChi2Histo,muNumFirstPixStaChi2Histo)
                        fillChi2Hists(event,mu,muNumFirstPixNormChi2Histo,muNumFirstPixTrkNormChi2Histo,
                                      muNumFirstPixGlbNormChi2Histo,muNumFirstPixStaChi2Histo,True)
                        
                    if event.pixelHits[mu] > 0:
                        muNumNPixHitPtHisto.Fill(event.trackpT[mu])
                        fillChi2Hists(event,mu,muNumNPixHitChi2Histo,muNumNPixHitTrkChi2Histo,
                                      muNumNPixHitGlbChi2Histo,muNumNPixHitStaChi2Histo)
                        fillChi2Hists(event,mu,muNumNPixHitNormChi2Histo,muNumNPixHitTrkNormChi2Histo,
                                      muNumNPixHitGlbNormChi2Histo,muNumNPixHitStaChi2Histo,True)
                        
                    if event.tkLayersWMeas[mu] > 5:
                        muNumNTkLayersPtHisto.Fill(event.trackpT[mu])
                        fillChi2Hists(event,mu,muNumNTkLayersChi2Histo,muNumNTkLayersTrkChi2Histo,
                                      muNumNTkLayersGlbChi2Histo,muNumNTkLayersStaChi2Histo)
                        fillChi2Hists(event,mu,muNumNTkLayersNormChi2Histo,muNumNTkLayersTrkNormChi2Histo,
                                      muNumNTkLayersGlbNormChi2Histo,muNumNTkLayersStaChi2Histo,True)
                        
                    if (event.ptError[mu]/event.trackpT[mu]) < 0.3:
                        muNumRelPtErrPtHisto.Fill(event.trackpT[mu])
                        fillChi2Hists(event,mu,muNumRelPtErrChi2Histo,muNumRelPtErrTrkChi2Histo,
                                      muNumRelPtErrGlbChi2Histo,muNumRelPtErrStaChi2Histo)
                        fillChi2Hists(event,mu,muNumRelPtErrNormChi2Histo,muNumRelPtErrTrkNormChi2Histo,
                                      muNumRelPtErrGlbNormChi2Histo,muNumRelPtErrStaChi2Histo,True)
                        
                    if event.nValidMuonHits[mu] > 0:
                        muNumNValidMuHitPtHisto.Fill(event.trackpT[mu])
                        fillChi2Hists(event,mu,muNumNValidMuHitChi2Histo,muNumNValidMuHitTrkChi2Histo,
                                      muNumNValidMuHitGlbChi2Histo,muNumNValidMuHitStaChi2Histo)
                        fillChi2Hists(event,mu,muNumNValidMuHitNormChi2Histo,muNumNValidMuHitTrkNormChi2Histo,
                                      muNumNValidMuHitGlbNormChi2Histo,muNumNValidMuHitStaChi2Histo,True)
                        
                    if event.nMatchedStations[mu] > 1:
                        muNumNMuStationsPtHisto.Fill(event.trackpT[mu])
                        fillChi2Hists(event,mu,muNumNMuStationsChi2Histo,muNumNMuStationsTrkChi2Histo,
                                      muNumNMuStationsGlbChi2Histo,muNumNMuStationsStaChi2Histo)
                        fillChi2Hists(event,mu,muNumNMuStationsNormChi2Histo,muNumNMuStationsTrkNormChi2Histo,
                                      muNumNMuStationsGlbNormChi2Histo,muNumNMuStationsStaChi2Histo,True)
                        
                    if event.isGlobal[mu] > 0:
                        muNumIsGlobalPtHisto.Fill(event.trackpT[mu])
                        fillChi2Hists(event,mu,muNumIsGlobalChi2Histo,muNumIsGlobalTrkChi2Histo,
                                      muNumIsGlobalGlbChi2Histo,muNumIsGlobalStaChi2Histo)
                        fillChi2Hists(event,mu,muNumIsGlobalNormChi2Histo,muNumIsGlobalTrkNormChi2Histo,
                                      muNumIsGlobalGlbNormChi2Histo,muNumIsGlobalStaChi2Histo,True)
                    
                    else:
                        # write out run/lumi/event for further analysis?
                        strange.write("ng: %d/%d/%d\n"%(event.run,event.lumi,event.event))
                        # maybe also look at distributions for failing global cuts
                        muNotGlobalPixHitHisto.Fill(event.trackpT[mu],event.pixelHits[mu])
                        muNotGlobalTkMeasHisto.Fill(event.trackpT[mu],event.tkLayersWMeas[mu])
                        muNotGlobalRelPtErrorHisto.Fill(event.trackpT[mu],event.ptError[mu]/event.trackpT[mu])
                        muNotGlobalValidMuonHitsHisto.Fill(event.trackpT[mu],event.nValidMuonHits[mu])
                        muNotGlobalMatchedStationsHisto.Fill(event.trackpT[mu],event.nMatchedStations[mu])
                        muNotGlobalChi2Histo.Fill(event.trackpT[mu],event.chi2[mu])
                        muNotGlobalTrkChi2Histo.Fill(event.trackpT[mu],event.innerChi2[mu])
                        muNotGlobalGlbChi2Histo.Fill(event.trackpT[mu],event.globalChi2[mu])
                        muNotGlobalStaChi2Histo.Fill(event.trackpT[mu],event.outerChi2[mu])
                        if event.ndof[mu] > 0:
                            muNotGlobalNormChi2Histo.Fill(event.trackpT[mu],event.chi2[mu]/event.ndof[mu])
                        if event.innerNDF[mu] > 0:
                            muNotGlobalTrkNormChi2Histo.Fill(event.trackpT[mu],event.innerChi2[mu]/event.innerNDF[mu])
                        if event.globalNDF[mu] > 0:
                            muNotGlobalGlbNormChi2Histo.Fill(event.trackpT[mu],event.globalChi2[mu]/event.globalNDF[mu])
                        if event.outerNDF[mu] > 0:
                            muNotGlobalStaNormChi2Histo.Fill(event.trackpT[mu],event.outerChi2[mu]/event.outerNDF[mu])
                        muNotGlobalDXYHisto.Fill(event.trackpT[mu],event.dxy[mu])
                        muNotGlobalDZHisto.Fill(event.trackpT[mu],event.dz[mu])
                        
                        pass
                    
                    muIsGlbDenPtHisto.Fill(event.trackpT[mu])
                    muIsGlbDenTkMeasHisto.Fill(event.trackpT[mu],event.tkLayersWMeas[mu])
                    muIsGlbDenPixHitHisto.Fill(event.trackpT[mu],event.pixelHits[mu])
                    muIsGlbDenRelPtErrorHisto.Fill(event.trackpT[mu],event.ptError[mu]/event.trackpT[mu])
                    muIsGlbDenValidMuonHitsHisto.Fill(event.trackpT[mu],event.nValidMuonHits[mu])
                    muIsGlbDenMatchedStationsHisto.Fill(event.trackpT[mu],event.nMatchedStations[mu])
                    fillChi2Hists(event,mu,muIsGlbDenChi2Histo,muIsGlbDenTrkChi2Histo,
                                  muIsGlbDenGlbChi2Histo,muIsGlbDenStaChi2Histo)
                    fillChi2Hists(event,mu,muIsGlbDenNormChi2Histo,muIsGlbDenTrkNormChi2Histo,
                                  muIsGlbDenGlbNormChi2Histo,muIsGlbDenStaChi2Histo,True)
                    muIsGlbDenDXYHisto.Fill(event.trackpT[mu],event.dxy[mu])
                    muIsGlbDenDZHisto.Fill(event.trackpT[mu],event.dz[mu])
                    if (passMuID(event,mu,True,False,options.debug)):
                        muIsGlbNum1PtHisto.Fill(event.trackpT[mu])
                    if (passMuIDTrk(event,mu,True,options.debug)):
                        muIsGlbNum2PtHisto.Fill(event.trackpT[mu])
                        
                # denominator cuts include track ID cuts, in addition to isTracker
                if (passMuDen(event,mu,True,options.debug)):
                    muIsGlbIsTrkDenPtHisto.Fill(event.trackpT[mu])
                    muIsGlbIsTrkDenTkMeasHisto.Fill(event.trackpT[mu],event.tkLayersWMeas[mu])
                    muIsGlbIsTrkDenPixHitHisto.Fill(event.trackpT[mu],event.pixelHits[mu])
                    muIsGlbIsTrkDenRelPtErrorHisto.Fill(event.trackpT[mu],event.ptError[mu]/event.trackpT[mu])
                    muIsGlbIsTrkDenValidMuonHitsHisto.Fill(event.trackpT[mu],event.nValidMuonHits[mu])
                    muIsGlbIsTrkDenMatchedStationsHisto.Fill(event.trackpT[mu],event.nMatchedStations[mu])
                    fillChi2Hists(event,mu,muIsGlbIsTrkDenChi2Histo,muIsGlbIsTrkDenTrkChi2Histo,
                                  muIsGlbIsTrkDenGlbChi2Histo,muIsGlbIsTrkDenStaChi2Histo)
                    fillChi2Hists(event,mu,muIsGlbIsTrkDenNormChi2Histo,muIsGlbIsTrkDenTrkNormChi2Histo,
                                  muIsGlbIsTrkDenGlbNormChi2Histo,muIsGlbIsTrkDenStaChi2Histo,True)
                    muIsGlbIsTrkDenDXYHisto.Fill(event.trackpT[mu],event.dxy[mu])
                    muIsGlbIsTrkDenDZHisto.Fill(event.trackpT[mu],event.dz[mu])
                    if (passMuID(event,mu,True,True,options.debug)):
                        muIsGlbIsTrkNum1PtHisto.Fill(event.trackpT[mu])
                    if (passMuIDTrk(event,mu,True,options.debug)):
                        muIsGlbIsTrkNum2PtHisto.Fill(event.trackpT[mu])
                        
        for tk in range(nTracks[1]):
            if not (event.trk_trackpT[tk] > 45. and abs(event.trk_trackEta[tk]) < 0.9):
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
    muDenRelPtErrorHisto.Write()
    muDenValidMuonHitsHisto.Write()
    muDenMatchedStationsHisto.Write()
    muDenChi2Histo.Write()
    muDenTrkChi2Histo.Write()
    muDenGlbChi2Histo.Write()
    muDenStaChi2Histo.Write()
    muDenNormChi2Histo.Write()
    muDenTrkNormChi2Histo.Write()
    muDenGlbNormChi2Histo.Write()
    muDenStaNormChi2Histo.Write()
    muDenDXYHisto.Write()
    muDenDZHisto.Write()
    muNum1PtHisto.Write()
    muNum2PtHisto.Write()

    muNumFirstPixPtHisto   .Write()
    muNumFirstPixChi2Histo   .Write()
    muNumFirstPixTrkChi2Histo.Write()
    muNumFirstPixGlbChi2Histo.Write()
    muNumFirstPixStaChi2Histo.Write()
    muNumFirstPixNormChi2Histo   .Write()
    muNumFirstPixTrkNormChi2Histo.Write()
    muNumFirstPixGlbNormChi2Histo.Write()
    muNumFirstPixStaNormChi2Histo.Write()

    muNumNPixHitPtHisto    .Write()
    muNumNPixHitChi2Histo   .Write()
    muNumNPixHitTrkChi2Histo.Write()
    muNumNPixHitGlbChi2Histo.Write()
    muNumNPixHitStaChi2Histo.Write()
    muNumNPixHitNormChi2Histo   .Write()
    muNumNPixHitTrkNormChi2Histo.Write()
    muNumNPixHitGlbNormChi2Histo.Write()
    muNumNPixHitStaNormChi2Histo.Write()

    muNumNTkLayersPtHisto  .Write()
    muNumNTkLayersChi2Histo   .Write()
    muNumNTkLayersTrkChi2Histo.Write()
    muNumNTkLayersGlbChi2Histo.Write()
    muNumNTkLayersStaChi2Histo.Write()
    muNumNTkLayersNormChi2Histo   .Write()
    muNumNTkLayersTrkNormChi2Histo.Write()
    muNumNTkLayersGlbNormChi2Histo.Write()
    muNumNTkLayersStaNormChi2Histo.Write()

    muNumRelPtErrPtHisto   .Write()
    muNumRelPtErrChi2Histo   .Write()
    muNumRelPtErrTrkChi2Histo.Write()
    muNumRelPtErrGlbChi2Histo.Write()
    muNumRelPtErrStaChi2Histo.Write()
    muNumRelPtErrNormChi2Histo   .Write()
    muNumRelPtErrTrkNormChi2Histo.Write()
    muNumRelPtErrGlbNormChi2Histo.Write()
    muNumRelPtErrStaNormChi2Histo.Write()

    muNumNValidMuHitPtHisto.Write()
    muNumNValidMuHitChi2Histo   .Write()
    muNumNValidMuHitTrkChi2Histo.Write()
    muNumNValidMuHitGlbChi2Histo.Write()
    muNumNValidMuHitStaChi2Histo.Write()
    muNumNValidMuHitNormChi2Histo   .Write()
    muNumNValidMuHitTrkNormChi2Histo.Write()
    muNumNValidMuHitGlbNormChi2Histo.Write()
    muNumNValidMuHitStaNormChi2Histo.Write()

    muNumNMuStationsPtHisto.Write()
    muNumNMuStationsChi2Histo   .Write()
    muNumNMuStationsTrkChi2Histo.Write()
    muNumNMuStationsGlbChi2Histo.Write()
    muNumNMuStationsStaChi2Histo.Write()
    muNumNMuStationsNormChi2Histo   .Write()
    muNumNMuStationsTrkNormChi2Histo.Write()
    muNumNMuStationsGlbNormChi2Histo.Write()
    muNumNMuStationsStaNormChi2Histo.Write()

    muNumIsGlobalPtHisto   .Write()
    muNumIsGlobalChi2Histo   .Write()
    muNumIsGlobalTrkChi2Histo.Write()
    muNumIsGlobalGlbChi2Histo.Write()
    muNumIsGlobalStaChi2Histo.Write()
    muNumIsGlobalNormChi2Histo   .Write()
    muNumIsGlobalTrkNormChi2Histo.Write()
    muNumIsGlobalGlbNormChi2Histo.Write()
    muNumIsGlobalStaNormChi2Histo.Write()
    
    muNotGlobalPixHitHisto.Write()
    muNotGlobalTkMeasHisto.Write()
    muNotGlobalRelPtErrorHisto.Write()
    muNotGlobalValidMuonHitsHisto.Write()
    muNotGlobalMatchedStationsHisto.Write()
    muNotGlobalChi2Histo.Write()
    muNotGlobalTrkChi2Histo.Write()
    muNotGlobalGlbChi2Histo.Write()
    muNotGlobalStaChi2Histo.Write()
    muNotGlobalNormChi2Histo.Write()
    muNotGlobalTrkNormChi2Histo.Write()
    muNotGlobalGlbNormChi2Histo.Write()
    muNotGlobalStaNormChi2Histo.Write()
    muNotGlobalDXYHisto.Write()
    muNotGlobalDZHisto.Write()

    muIsTrkDenPtHisto.Write()
    muIsTrkDenPixHitHisto.Write()
    muIsTrkDenTkMeasHisto.Write()
    muIsTrkDenRelPtErrorHisto.Write()
    muIsTrkDenValidMuonHitsHisto.Write()
    muIsTrkDenMatchedStationsHisto.Write()
    muIsTrkDenChi2Histo.Write()
    muIsTrkDenTrkChi2Histo.Write()
    muIsTrkDenGlbChi2Histo.Write()
    muIsTrkDenStaChi2Histo.Write()
    muIsTrkDenNormChi2Histo.Write()
    muIsTrkDenTrkNormChi2Histo.Write()
    muIsTrkDenGlbNormChi2Histo.Write()
    muIsTrkDenStaNormChi2Histo.Write()
    muIsTrkDenDXYHisto.Write()
    muIsTrkDenDZHisto.Write()
    muIsTrkNum1PtHisto.Write()
    muIsTrkNum2PtHisto.Write()

    muIsGlbDenPtHisto.Write()
    muIsGlbDenPixHitHisto.Write()
    muIsGlbDenTkMeasHisto.Write()
    muIsGlbDenRelPtErrorHisto.Write()
    muIsGlbDenValidMuonHitsHisto.Write()
    muIsGlbDenMatchedStationsHisto.Write()
    muIsGlbDenChi2Histo.Write()
    muIsGlbDenTrkChi2Histo.Write()
    muIsGlbDenGlbChi2Histo.Write()
    muIsGlbDenStaChi2Histo.Write()
    muIsGlbDenNormChi2Histo.Write()
    muIsGlbDenTrkNormChi2Histo.Write()
    muIsGlbDenGlbNormChi2Histo.Write()
    muIsGlbDenStaNormChi2Histo.Write()
    muIsGlbDenDXYHisto.Write()
    muIsGlbDenDZHisto.Write()
    muIsGlbNum1PtHisto.Write()
    muIsGlbNum2PtHisto.Write()

    muIsGlbIsTrkDenPtHisto.Write()
    muIsGlbIsTrkDenPixHitHisto.Write()
    muIsGlbIsTrkDenTkMeasHisto.Write()
    muIsGlbIsTrkDenRelPtErrorHisto.Write()
    muIsGlbIsTrkDenValidMuonHitsHisto.Write()
    muIsGlbIsTrkDenMatchedStationsHisto.Write()
    muIsGlbIsTrkDenChi2Histo.Write()
    muIsGlbIsTrkDenTrkChi2Histo.Write()
    muIsGlbIsTrkDenGlbChi2Histo.Write()
    muIsGlbIsTrkDenStaChi2Histo.Write()
    muIsGlbIsTrkDenNormChi2Histo.Write()
    muIsGlbIsTrkDenTrkNormChi2Histo.Write()
    muIsGlbIsTrkDenGlbNormChi2Histo.Write()
    muIsGlbIsTrkDenStaNormChi2Histo.Write()
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

    strange.close()
    # matching track to track, check y-position compatibility
    # matching track to muon,  check y-position compatibility
