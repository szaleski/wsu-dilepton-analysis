import FWCore.ParameterSet.Config as cms

################################################################
###algoType determines the track type that you are interested in
###1 = Tracker Only
###2 = Tracker Plus First Muon Station
###3 = DYT
###4 = Picky
###5 = TuneP
################################################################

###############################################################
###debug determines the verbosity for debugging mode
###0 = "off"
###1 = minor problems, check for new problems for fresh code
###2 = moderate problems, check new plus affected old code
###3 = nothing works, check everything
###############################################################

muonTree = cms.EDAnalyzer('MuonTree',
    muonSrc     = cms.InputTag("muons1Leg"),
    upperLegSrc = cms.InputTag("upperMuons"),
    lowerLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(1),

    globalTrackSrc  = cms.InputTag("globalSPMuonTracks"),
    cosmicTrackSrc  = cms.InputTag("cosmicSPMuonTracks"),
    trackerTrackSrc = cms.InputTag("trackerSPMuonTracks"),
    simTrackSrc     = cms.InputTag("g4SimHits"),
    l1MuonSrc       = cms.InputTag("l1extraParticles","","RECO"),
    trigResultsSrc  = cms.InputTag('TriggerResults','','HLT'),
    fakeL1SingleMuSrc = cms.InputTag("singleMuFilter"),
    hltTrigCut      = cms.string('L1SingleMuOpen'),
 

    isGen       = cms.bool(False),
    debug       = cms.int32(0),
    maxDR       = cms.double(0.15), # not currently used, but to match upper/lower legs
    maxDPhi     = cms.double(0.10), # match requirement between upper/lower legs
    maxDEta     = cms.double(0.05), # match requirement between upper/lower legs
    minPt       = cms.double(45.0), # minimum pT to consider the muon
)
