import FWCore.ParameterSet.Config as cms

muonAnalysis = cms.EDAnalyzer('MuonAnalyzer',
    muonSrc     = cms.InputTag("muons1Leg"),
    tagLegSrc   = cms.InputTag("upperMuons"),
    probeLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(1), #Different values for track type can be found in the config file.
    debug       = cms.int32(0), #debugging uses integer for verbosity of mode 0 = debug mode "off"
    maxDR       = cms.double(0.15), # not currently used, but to match upper/lower legs
    maxDPhi     = cms.double(0.1),  # match between upper/lower legs
    maxDEta     = cms.double(0.05), # match between upper/lower legs
    minPt       = cms.double(45.0), # minimum pT to consider the muon
)
