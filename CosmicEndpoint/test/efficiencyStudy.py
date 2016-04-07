import sys,os
import ROOT as r
import numpy as np

r.gROOT.SetBatch(True)
def passMuDen(ev,idx):
    return ev.trackpT[idx] > 53. and ev.isTracker[idx]

def passMuID(ev,idx):
    passNVMuHits     = (ev.nValidMuonHits[idx] > 0)
    passNMMuStations = (ev.nMatchedStations[idx] > 1)
    passRelPtErr     = ((ev.ptError[idx]/ev.trackpT[idx]) < 0.3)
    result = passNVMuHits and passNMMuStations and passRelPtErr and ev.isGlobal[idx]
    return result and passMuDen(ev,idx)

def passMuIDTrk(ev,idx):
    passNPHits = (ev.pixelHits[idx] > 0)
    passTKLays = (ev.tkLayersWMeas[idx] > 5)
    result = passNPHits and passTKLays and passMuID(ev,idx)
    return result

def passTrkDen(ev,idx):
    muonMatch = ev.trk_matchedMuIdx[idx]
    if (muonMatch < 0):
        print "passTrkDen: unable to find a matched muon %d"%(muonMatch)
        sys.stdout.flush()
        return False
    return ev.trk_trackpT[idx] > 53. and ev.isTracker[muonMatch]

def passTrkID(ev,idx):
    passNVMuHits     = (ev.trk_nValidMuonHits[idx] > 0)
    passNMMuStations = (ev.trk_nMatchedStations[idx] > 1)
    muonMatch = ev.trk_matchedMuIdx[idx]
    if (muonMatch < 0):
        print "passTrkID: unable to find a matched muon %d"%(muonMatch)
        sys.stdout.flush()
        return False
    passRelPtErr     = ((ev.ptError[muonMatch]/ev.trackpT[muonMatch]) < 0.3)
    result = passNVMuHits and passNMMuStations and passRelPtErr and ev.isGlobal[muonMatch]
    return result and passTrkDen(ev,idx)

def passTrkIDTrk(ev,idx):
    muonMatch = ev.trk_matchedMuIdx[idx]
    if (muonMatch < 0):
        print "passTrkIDTrk: unable to find a matched muon %d"%(muonMatch)
        sys.stdout.flush()
        return False
    # for track, we want to preliminarily match to the global track tracker information, though we could match to the tracker track
    passNPHits = (ev.pixelHits[muonMatch] > 0)
    passTKLays = (ev.tkLayersWMeas[muonMatch] > 5)
    #passNPHits = (ev.trk_pixelHits[idx] > 0)
    #passTKLays = (ev.trk_tkLayersWMeas[idx] > 5)
    result = passNPHits and passTKLays and passTrkID(ev,idx)
    return result

#[0,10,20,30,40,50,75,100,150,200,250,300,400,500,750,1000,1500,3000]
ptBins  = np.array([0.,10.,20.,30.,40.,50.,75.,100.,150.,200.,250.,300.,400.,500.,750.,1000.,1500.,3000.])
etaBins = np.array([-2., -1.6, -0.9, 0., 0.9, 1.6,  2.])
phiBins = np.array([-4., -1.6, 0., 1.6,  4.])

outfile = r.TFile("efficiencies.root","recreate")

myfile = r.TFile("CosmicMuonTree_MC_76X.root","r")
mytree = myfile.Get("analysisSPMuons/MuonTree")
tkType = {"global":0, "cosmic":1, "tracker":2}

#

muDenPtHisto  = r.TH1D("muonDenominatorPt","",len(ptBins)-1,ptBins)
muNum1PtHisto = r.TH1D("muonPassIDPt",     "",len(ptBins)-1,ptBins)
muNum2PtHisto = r.TH1D("muonPassIDTrkPt",  "",len(ptBins)-1,ptBins)
muDenPtHisto.Sumw2()
muNum1PtHisto.Sumw2()
muNum2PtHisto.Sumw2()

trkDenPtHisto  = r.TH1D("trackDenominatorPt","",len(ptBins)-1,ptBins)
trkNum1PtHisto = r.TH1D("trackPassIDPt",     "",len(ptBins)-1,ptBins)
trkNum2PtHisto = r.TH1D("trackPassIDTrkPt",  "",len(ptBins)-1,ptBins)
trkDenPtHisto.Sumw2()
trkNum1PtHisto.Sumw2()
trkNum2PtHisto.Sumw2()

for event in mytree:
    #print "g=%d c=%d t=%d"%(event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks)
    nTracks = [event.nGlobalTracks,event.nCosmicTracks,event.nTrackerTracks]
    for mu in range(event.nMuons):
        if (passMuDen(event,mu)):
            muDenPtHisto.Fill(event.trackpT[mu])
            if (passMuID(event,mu)):
                muNum1PtHisto.Fill(event.trackpT[mu])
            if (passMuIDTrk(event,mu)):
                muNum2PtHisto.Fill(event.trackpT[mu])

    for tk in range(nTracks[1]):
        if (passTrkDen(event,10+tk)):
            trkDenPtHisto.Fill(event.trk_trackpT[10+tk])
            if (passTrkID(event,10+tk)):
                trkNum1PtHisto.Fill(event.trk_trackpT[10+tk])
            if (passTrkIDTrk(event,10+tk)):
                trkNum2PtHisto.Fill(event.trk_trackpT[10+tk])
        else:
            sys.stdout.flush()
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
outfile.cd()
muDenPtHisto.Write()
muNum1PtHisto.Write()
muNum2PtHisto.Write()

trkDenPtHisto.Write()
trkNum1PtHisto.Write()
trkNum2PtHisto.Write()
outfile.Close()

# matching track to track, check y-position compatibility
# matching track to muon,  check y-position compatibility
#cuts_global = isGlobalMuon
#cuts_ID = (ptError/pt < 0.3) and (numberOfValidMuonHits > 0) and (numberOfMatchedStations > 1)
#cuts_RECO = (numberOfValidPixelHits > 0) and (trackerLayersWithMeasurement > 5)