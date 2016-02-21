import FWCore.ParameterSet.Config as cms
process = cms.Process("MuonAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

from WSUDiLeptons.MuonAnalyzer.inputfiles import *

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        mcfiles
    )
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000) )

process.load("WSUDiLeptons.MuonAnalyzer.wsuMuonCollections_cfi")
process.COSMICoutput.fileName = cms.untracked.string('Cosmics_deco_p100_CosmicSP_generic.root')

from WSUDiLeptons.MuonAnalyzer.wsuMuonTree_cfi import *

process.analysisMuons = muonTree.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    upperLegSrc = cms.InputTag("betterMuons"),
    lowerLegSrc = cms.InputTag("betterMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(-1)
)
process.analysisGlobalMuons = muonTree.clone(
    muonSrc     = cms.InputTag("globalMuons"),
    upperLegSrc = cms.InputTag("globalMuons"),
    lowerLegSrc = cms.InputTag("globalMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(-1)
)
process.analysisSPMuons = muonTree.clone(
    muonSrc     = cms.InputTag("betterSPMuons"),
    upperLegSrc = cms.InputTag("upperMuons"),
    lowerLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(5)
)
process.analysisGlobalSPMuons = muonTree.clone(
    muonSrc     = cms.InputTag("globalSPMuons"),
    upperLegSrc = cms.InputTag("upperGlobalMuons"),
    lowerLegSrc = cms.InputTag("lowerGlobalMuons"),
    algoType    = cms.int32(5),
    debug       = cms.int32(5)
)
process.analysisTrackerMuons = muonTree.clone(
    muonSrc     = cms.InputTag("betterSPMuons"),
    upperLegSrc = cms.InputTag("upperMuons"),
    lowerLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(-1)
)
process.analysisTPFMSMuons = muonTree.clone(
    muonSrc     = cms.InputTag("betterSPMuons"),
    upperLegSrc = cms.InputTag("upperMuons"),
    lowerLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(2),
    debug       = cms.int32(-1)
)
process.analysisDYTMuons = muonTree.clone(
    muonSrc     = cms.InputTag("betterSPMuons"),
    upperLegSrc = cms.InputTag("upperMuons"),
    lowerLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(3),
    debug       = cms.int32(-1)
)
process.analysisPickyMuons = muonTree.clone(
    muonSrc     = cms.InputTag("betterSPMuons"),
    upperLegSrc = cms.InputTag("upperMuons"),
    lowerLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(4),
    debug       = cms.int32(-1)
)

process.analysisTunePMuons = muonTree.clone(
    muonSrc     = cms.InputTag("betterSPMuons"),
    upperLegSrc = cms.InputTag("upperMuons"),
    lowerLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(5),
    debug       = cms.int32(2)
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('CosmicMuonTree_MC_76X.root')
)

process.muonanalysis = cms.Path(
    #process.reconstructionCosmics
    process.betterMuons
    +process.globalMuons
    +process.betterSPMuons
    +process.globalSPMuons
    +process.upperMuons
    +process.lowerMuons
    +process.upperGlobalMuons
    +process.lowerGlobalMuons
    +process.muonSPFilter
    #+process.globalMuonSPFilter
    #+process.analysisMuons
    #+process.analysisGlobalMuons
    #+process.analysisSPMuons
    #+process.analysisGlobalSPMuons
    +process.analysisTrackerMuons
    +process.analysisTPFMSMuons
    +process.analysisDYTMuons
    +process.analysisPickyMuons
    +process.analysisTunePMuons
    )

# generate EDM output
process.COSMICoutput_step = cms.EndPath(process.COSMICoutput)

# Schedule definition
process.schedule = cms.Schedule(
    process.muonanalysis
#    ,process.COSMICoutput_step
)
