import ROOT as r
import math

def dPhi(phi1,phi2,debug=False):
    import math
    dphi = phi1-phi2
    if dphi < -math.pi:
        dphi = dphi + 2*math.pi
    if dphi > math.pi:
        dphi = dphi - 2*math.pi
    return dphi

def dR(eta1,phi1,eta2,phi2,debug=False):
    import math
    dphi2 = dPhi(phi1,phi2)*dPhi(phi1,phi2)
    deta2 = (eta1-eta2)*(eta1-eta2)
    dR2Val = deta2+dphi2
    return math.sqrt(dR2Val)

def passDxyDz(event,muIdx, dxyCut=4.0, dzCut=10.0, debug=False):
    return abs(event.dxy[muIdx]) < dxyCut and abs(event.dz[muIdx]) < dzCut

def passHighPtMuon(event, muIdx, first=False, debug=False):
    if not event.isTracker[muIdx]:
        return False
    if not event.isGlobal[muIdx]:
        return False
    if not event.nValidMuonHits[muIdx] > 0:
        return False
    if not event.nMatchedStations[muIdx] > 1:
        return False
    if not (event.ptError[muIdx]/event.trackpT[muIdx]) < 0.3:
        return False
    if first and not event.firstPixel[muIdx] > 0:
        return False
    if not event.pixelHits[muIdx] > 0:
        return False
    if not event.tkLayersWMeas[muIdx] > 5:
        return False
    
    return True

def matchSimTrack(event, muIdx, minDEta=0.05, minDPhi=0.15, debug=False):
    # sim tracks have phi positive and negative, while the reco::Muons and reco::Tracks seem always negative
    # only consider lower legs for now?
    bestDEta = 10
    bestDPhi = 10
    matchIdx = -1
    for sim in range(event.nSimTracks):
        if event.simTrackPhi[sim] > 0:
            continue
        tmpDEta = abs(event.simTrackEta[sim] - event.trackEta[muIdx])
        tmpDPhi = abs(dPhi(event.simTrackPhi[sim], event.trackPhi[muIdx]))
        if tmpDEta < bestDEta and tmpDPhi < bestDPhi:
            bestDEta = tmpDEta
            bestDPhi = tmpDPhi
            if bestDEta < minDEta and bestDPhi < minDPhi:
                matchIdx = sim
                pass
            pass
        pass
    if debug:
        print "muon %d matched to sim track %d with dEta=%2.2f and dPhi=%2.2f"%(muIdx,matchIdx,bestDEta,bestDPhi)
    return matchIdx

def matchL1SingleMu(event, muIdx, minDEta=0.05, minDPhi=0.15, debug=False):
    bestDEta = 10
    bestDPhi = 10
    matchIdx = -1
    for l1m in range(event.nL1Muons):
        if not (event.l1MuonIsFwd[l1m]==0):
            continue
        if event.l1MuonPhi[l1m] < -2.88:
            continue
        if event.l1MuonPhi[l1m] > -0.26:
            continue
        
        tmpDEta = abs(event.l1MuonEta[l1m] - event.trackEta[muIdx])
        tmpDPhi = abs(dPhi(event.l1MuonPhi[l1m], event.trackPhi[muIdx]))
        if tmpDEta < bestDEta and tmpDPhi < bestDPhi:
            bestDEta = tmpDEta
            bestDPhi = tmpDPhi
            if bestDEta < minDEta and bestDPhi < minDPhi:
                matchIdx = l1m
                pass
            pass
        pass
    if debug:
        print "muon %d matched to l1Muon %d with dEta=%2.2f and dPhi=%2.2f"%(muIdx,matchIdx,bestDEta,bestDPhi)
    return matchIdx

def passL1SingleMu(event, debug=False):
    isL1SingleMu = False
    for l1m in range(event.nL1Muons):
        if not (event.l1MuonIsFwd[l1m]==0):
            continue
        if event.l1MuonPhi[l1m] > -2.88 and event.l1MuonPhi[l1m] < -0.26:
            isL1SingleMu = True
            pass
        pass
    return isL1SingleMu

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
        

def findFunky(event,isUpper=False,debug=False):
    # want to find the evnets where we have a duplicated Muon object pt eta phi upper all identical
    # optionally, use combined information?
    funky = False
    if event.nMuons < 2:
        return funky

    for ref in range(event.nMuons):
        refPt  = event.globalpT[ref]
        refEta = event.globalEta[ref]
        refPhi = event.globalPhi[ref]

        refQ   = event.charge[ref]

        refGlb = event.isGlobal[ref]
        refTrk = event.isTracker[ref]
        refSta = event.isStandAlone[ref]

        refLeg = event.isLower[ref]
        if isUpper:
            refLeg = event.isUpper[ref]
            if event.isLower[ref] > 0:
                continue
        else:
            if event.isUpper[ref] > 0:
                continue
    
        for mu in range(ref, event.nMuons):
            if mu == ref:
                continue
            chkPt  = event.globalpT[mu]
            chkEta = event.globalEta[mu]
            chkPhi = event.globalPhi[mu]

            chkQ   = event.charge[mu]

            chkGlb = event.isGlobal[mu]
            chkTrk = event.isTracker[mu]
            chkSta = event.isStandAlone[mu]

            chkLeg = event.isLower[mu]
            if isUpper:
                chkLeg = event.isUpper[mu]
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
                    event.run,event.lumi,event.event,
                    mu,chkLeg,ref,refLeg,
                    (chkPt-refPt),
                    dPhi(chkPhi,refPhi),
                    (chkEta-refEta)
                    )
                print "chk%d up=%d/lo=%d,q=%d (%dt/%dg/%dsa) pT:%2.4f,phi:%2.4f,eta:%2.4f - dxy:%2.2f,dz:%2.2f,chi2:%2.2f,ndf:%d,pt:%2.4f,dpt/pt:%2.4f,pix:%d,tkhits:%d,tklaywmeas:%d,mustahits:%d,vmuhits:%d,matched:%d"%(
                    mu,event.isUpper[mu],event.isLower[mu],chkQ,chkTrk,chkGlb,chkSta,chkPt,chkPhi,chkEta,
                    event.dxy[mu],
                    event.dz[mu],
                    event.chi2[mu],
                    event.ndof[mu],
                    event.trackpT[mu],
                    event.ptError[mu]/event.trackpT[mu],
                    event.pixelHits[mu],
                    event.trackerHits[mu],
                    event.tkLayersWMeas[mu],
                    event.muonStationHits[mu],
                    event.nValidMuonHits[mu],
                    event.nMatchedStations[mu])

                print "ref%d,up=%d/lo=%d,q=%d (%dt/%dg/%dsa) pT:%2.4f,phi:%2.4f,eta:%2.4f - dxy:%2.2f,dz:%2.2f,chi2:%2.2f,ndf:%d,pt:%2.4f,dpt/pt:%2.4f,pix:%d,tkhits:%d,tklaywmeas:%d,mustahits:%d,vmuhits:%d,matched:%d"%(
                    ref,event.isUpper[ref],event.isLower[ref],refQ,refTrk,refGlb,refSta,refPt,refPhi,refEta,
                    event.dxy[ref],
                    event.dz[ref],
                    event.chi2[ref],
                    event.ndof[ref],
                    event.trackpT[ref],
                    event.ptError[ref]/event.trackpT[ref],
                    event.pixelHits[ref],
                    event.trackerHits[ref],
                    event.tkLayersWMeas[ref],
                    event.muonStationHits[ref],
                    event.nValidMuonHits[ref],
                    event.nMatchedStations[ref])
                funky = True
        
    return funky

def findTrackMatch(event,idx,src,coll,debug=False):
    nTracks = 0
    if coll == src:
        print "source and comparison come from same collection, returning -1"
        return -1
    if coll == 0:
        nTracks = event.nGlobalTracks
    elif coll == 1:
        nTracks = event.nCosmicTracks
    elif coll == 2:
        nTracks = event.nTrackerTracks

    yCompatible = False
    bestDEta = 10
    bestDPhi = 10
    matchIdx = -1
    if debug:
        print "   (%5s, %5s, %5s, %5s)"%("eta", "phi", "y-in", "y-out")
        print "%2d (%2.2f, %2.2f, %2.2f, %2.2f) "%(idx,
                                                   event.trk_trackEta[10*src+idx],
                                                   event.trk_trackPhi[10*src+idx],
                                                   event.trk_innerY[10*src+idx],
                                                   event.trk_outerY[10*src+idx])
    for tk in range(nTracks):
        tmpDEta = abs(event.trk_trackEta[10*src+idx] - event.trk_trackEta[10*coll+tk])
        tmpDPhi = abs(dPhi(event.trk_trackPhi[10*src+idx], event.trk_trackPhi[10*coll+tk]))
        if debug:
            print "%2d (%2.2f, %2.2f, %2.2f, %2.2f) "%(tk,
                                                       event.trk_trackEta[10*coll+tk],
                                                       event.trk_trackPhi[10*coll+tk],
                                                       event.trk_innerY[10*coll+tk],
                                                       event.trk_outerY[10*coll+tk])
        if event.trk_innerY[10*src+idx] > 0 and event.trk_innerY[10*coll+tk] > 0:
            yCompatible = True
        elif event.trk_innerY[10*src+idx] < 0 and event.trk_innerY[10*coll+tk] < 0:
            yCompatible = True
        elif event.trk_outerY[10*src+idx] > 0 and event.trk_outerY[10*coll+tk] > 0:
            yCompatible = True
        elif event.trk_outerY[10*src+idx] < 0 and event.trk_outerY[10*coll+tk] < 0:
            yCompatible = True
        if yCompatible and tmpDEta < bestDEta and tmpDPhi < bestDPhi:
            matchIdx = 10*coll+tk
            bestDEta = tmpDEta
            bestDPhi = tmpDPhi
    if matchIdx < 0 and debug:
        print "unable to match track %d from collection %d to a track in collection %d"%(idx,src,coll)
    return matchIdx

def findTagMuon(event,tightSel=False,useUpper=True,useGlobal=True,useTracker=True,debug=False):
    tagIdx   = -1
    tagCount = 0
    for mu in range(event.nMuons):
        if useUpper:
            # process upper rather than lower legs
            if (abs(event.innerY[mu]) < abs(event.outerY[mu])):
                if debug:
                    print "l%d/u%d"%(event.isLower[mu],event.isUpper[mu])
                continue
            pass
        else:
            # if isUpper[mu]
            if (abs(event.innerY[mu]) > abs(event.outerY[mu])):
                if debug:
                    print "l%d/u%d"%(event.isLower[mu],event.isUpper[mu])
                continue
            pass
        
        if not (event.trackpT[mu] > 45. and abs(event.trackEta[mu]) < 0.9):
            # skip muons that fail the basic Z' kinematic cuts and barrel region
            continue
        if not (abs(event.dxy[mu]) < 4. and abs(event.dz[mu]) < 10.):
            continue
        if tightSel:
            # optionally apply tighter pixel requirements
            # if not (abs(event.dxy[mu]) < 2.5 and abs(event.dz[mu]) < 10.):
            # if not (abs(event.dxy[mu]) < 2.5 and abs(event.dz[mu]) < 10.):
            if not (abs(event.dxy[mu]) < 1.0 and abs(event.dz[mu]) < 2.5):
                continue
            pass
        if passMuDen(event,mu,useTracker):
            if passMuIDTrk(event,mu,useGlobal,useTracker):
                if passMuID(event,mu,useGlobal,useTracker):
                    tagIdx = mu
                    tagCount = tagCount + 1
    if tagCount > 0:
        print "Found %d tags"%(tagCount)
    return tagIdx

def findProbeMuon(event,tagIdx,tightSel=False,useUpper=False,useGlobal=True,useTracker=True,debug=False):
    probeIdx   = -1
    probeCount = 0
    yCompatible = False
    bestDEta = 10
    bestDPhi = 10
    for mu in range(event.nMuons):
        if mu == tagIdx:
            continue
        if useUpper:
            # process upper rather than lower legs
            if (abs(event.innerY[mu]) < abs(event.outerY[mu])):
                if debug:
                    print "l%d/u%d"%(event.isLower[mu],event.isUpper[mu])
                continue
            pass
        else:
            # if isUpper[mu]
            if (abs(event.innerY[mu]) > abs(event.outerY[mu])):
                if debug:
                    print "l%d/u%d"%(event.isLower[mu],event.isUpper[mu])
                continue
            pass
        tmpDEta = abs(event.trackEta[tagIdx] - event.trackEta[mu])
        tmpDPhi = abs(dPhi(event.trackPhi[tagIdx], event.trackPhi[mu]))
        if tmpDEta < bestDEta and tmpDPhi < bestDPhi:
            probeIdx = mu
            bestDEta = tmpDEta
            bestDPhi = tmpDPhi

    return probeIdx

def passMuDen(event,idx,tracker=False,debug=False):
    result = event.trackpT[idx] > 45. and abs(event.trackEta[idx]) < 0.9 and event.isTracker[idx]
    if tracker:
        result = result and passMuIDTrk(event,idx,tracker)
    return result

def passMuID(event,idx,glbl=True,tracker=False,debug=False):
    passNVMuHits     = (event.nValidMuonHits[idx] > 0)
    passNMMuStations = (event.nMatchedStations[idx] > 1)
    passRelPtErr     = ((event.ptError[idx]/event.trackpT[idx]) < 0.3)
    result = passNVMuHits and passNMMuStations and passRelPtErr
    if glbl:
        result = result and event.isGlobal[idx]
    return result and passMuDen(event,idx,tracker,debug)

def passMuIDTrk(event,idx,first=False,debug=False):
    passFirstPix = True
    if first:
        passFirstPix = (event.firstPixel[idx] > 0)
    passNPHits   = (event.pixelHits[idx] > 0)
    passTKLays   = (event.tkLayersWMeas[idx] > 5)
    return passFirstPix and passNPHits and passTKLays

def passTrkDen(event,idx,coll,match=False,debug=False):
    muonMatch = event.trk_matchedMuIdx[10*coll+idx]
    if not match:
        muonMatch = 0
    if (muonMatch < 0):
        if debug:
            print "passTrkDen: unable to find a matched muon %d"%(muonMatch)
        sys.stdout.flush()
        return False
    
    # definitely want to try to match to a tracker (index 2 in the array) track here
    isTracker = (findTrackMatch(event,idx,coll,2) > -1)
    if match:
        event.isTracker[muonMatch]

    return event.trk_trackpT[10*coll+idx] > 45. and abs(event.trk_trackEta[10*coll+idx]) < 0.9 and isTracker

def passTrkMuID(event,idx,coll,match=False,debug=False):
    muonMatch = event.trk_matchedMuIdx[10*coll+idx]
    if not match:
        muonMatch = 0
    if (muonMatch < 0):
        if debug:
            print "passTrkMuID: unable to find a matched muon %d"%(muonMatch)
        sys.stdout.flush()
        return False

    # for track, we want to preliminarily match to the global track tracker information, though we could match to the cosmic track
    passNVMuHits     = (event.trk_nValidMuonHits[10*coll+idx] > 0)
    passNMMuStations = (event.trk_nMatchedStations[10*coll+idx] > 1) #probably already requires a match to a muon here...
    passRelPtErr     = ((event.trk_ptError[10*coll+idx]/event.trk_trackpT[10*coll+idx]) < 0.3)
    # definitely want to try to match to a global track here
    isGlobal = (findTrackMatch(event,idx,coll,0) > -1)
    if match:
        passNVMuHits     = (event.nValidMuonHits[muonMatch] > 0)
        passNMMuStations = (event.nMatchedStations[muonMatch] > 1)
        passRelPtErr     = ((event.ptError[muonMatch]/event.trackpT[muonMatch]) < 0.3)
        isGlobal         = event.isGlobal[muonMatch]
    result = passNVMuHits and passNMMuStations and passRelPtErr and isGlobal
    return result and passTrkDen(event,idx,coll,match,debug)

def passTrkMuIDTrk(event,idx,coll,match=False,debug=False):
    muonMatch = event.trk_matchedMuIdx[10*coll+idx]
    if not match:
        # not requiring a match to a muon
        muonMatch = 0
    if (muonMatch < 0):
        if debug:
            print "passTrkMuIDTrk: unable to find a matched muon %d"%(muonMatch)
        sys.stdout.flush()
        return False

    # for track, we want to preliminarily match to the global track tracker information, though we could match to the tracker track
    passFirstPix = (event.trk_firstPixel[10*coll+idx]     > 0)
    passNPHits   = (event.trk_pixelHits[10*coll+idx]     > 0)
    passTKLays   = (event.trk_tkLayersWMeas[10*coll+idx] > 5)
    if match:
        passFirstPix = (event.firstPixel[muonMatch]     > 0)
        passNPHits   = (event.pixelHits[muonMatch]     > 0)
        passTKLays   = (event.tkLayersWMeas[muonMatch] > 5)
    result = passFirstPix and passNPHits and passTKLays and passTrkMuID(ev,idx,coll,match,debug)
    return result

