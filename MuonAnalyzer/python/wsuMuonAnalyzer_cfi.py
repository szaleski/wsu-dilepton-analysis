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

muonAnalysis = cms.EDAnalyzer('MuonAnalyzer',
    muonSrc     = cms.InputTag("muons1Leg"),
    tagLegSrc   = cms.InputTag("upperMuons"),
    probeLegSrc = cms.InputTag("lowerMuons"),

    simTrackSrc     = cms.InputTag("g4SimHits"),
    trigResultsSrc  = cms.InputTag('TriggerResults','','HLT'),
    fakeL1SingleMuSrc = cms.InputTag("singleMuFilter"),
    hltTrigCut      = cms.string('L1SingleMuOpen'),
 
    isGen       = cms.bool(False),

    algoType    = cms.int32(1),
    debug       = cms.int32(0),
    maxDR       = cms.double(0.15), # not currently used, but to match upper/lower legs
    maxDPhi     = cms.double(0.1),  # match requirement between upper/lower legs
    maxDEta     = cms.double(0.05), # match requirement between upper/lower legs
    minPt       = cms.double(45.0), # minimum pT to consider the muon
)
