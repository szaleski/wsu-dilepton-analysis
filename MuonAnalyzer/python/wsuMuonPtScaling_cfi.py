import FWCore.ParameterSet.Config as cms

muonPtScaling = cms.EDAnalyzer('MuonPtScaling',
    muonSrc = cms.InputTag("muons"),
    debug   = cms.bool(False)
)
