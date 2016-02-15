import FWCore.ParameterSet.Config as cms
process = cms.Process("MuonAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

from WSUDiLeptons.MuonAnalyzer.inputfiles import *

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        datafiles
    )
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(500) )

process.load("WSUDiLeptons.MuonAnalyzer.wsuMuonCollections_cfi")
process.COSMICoutput.fileName = cms.untracked.string('Cosmics_CRAFT15_CosmicSP_ReReco_outer_new_changed.root')

from WSUDiLeptons.MuonAnalyzer.wsuMuonAnalyzer_cfi import muonAnalysis

process.analysisMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("betterMuons"),
    probeLegSrc = cms.InputTag("betterMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(2)
)
process.analysisGlobalMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("globalMuons"),
    tagLegSrc   = cms.InputTag("globalMuons"),
    probeLegSrc = cms.InputTag("globalMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(-1)
)
process.analysisSPMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterSPMuons"),
    tagLegSrc   = cms.InputTag("betterSPMuons"),
    probeLegSrc = cms.InputTag("betterSPMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(-1)
)
process.analysisGlobalSPMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("globalSPMuons"),
    tagLegSrc   = cms.InputTag("globalSPMuons"),
    probeLegSrc = cms.InputTag("globalSPMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(-1)
)
process.analysisLowerTagTrackerMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("lowerGlobalMuons"),
    probeLegSrc = cms.InputTag("upperMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(-1)
)
process.analysisLowerTagTPFMSMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("lowerGlobalMuons"),
    probeLegSrc = cms.InputTag("upperMuons"),
    algoType    = cms.int32(2),
    debug       = cms.int32(-1)
)
process.analysisLowerTagDYTMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("lowerGlobalMuons"),
    probeLegSrc = cms.InputTag("upperMuons"),
    algoType    = cms.int32(3),
    debug       = cms.int32(-1)
)
process.analysisLowerTagPickyMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("lowerGlobalMuons"),
    probeLegSrc = cms.InputTag("upperMuons"),
    algoType    = cms.int32(4),
    debug       = cms.int32(-1)
)
process.analysisLowerTagTunePMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("lowerGlobalMuons"),
    probeLegSrc = cms.InputTag("upperMuons"),
    algoType    = cms.int32(5),
    debug       = cms.int32(2)
)

process.analysisUpperTagTrackerMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("upperGlobalMuons"),
    probeLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(-1)
)
process.analysisUpperTagTPFMSMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("upperGlobalMuons"),
    probeLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(2),
    debug       = cms.int32(-1)
)
process.analysisUpperTagDYTMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("upperGlobalMuons"),
    probeLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(3),
    debug       = cms.int32(-1)
)
process.analysisUpperTagPickyMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("upperGlobalMuons"),
    probeLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(4),
    debug       = cms.int32(-1)
)

process.analysisUpperTagTunePMuons = muonAnalysis.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    tagLegSrc   = cms.InputTag("upperGlobalMuons"),
    probeLegSrc = cms.InputTag("lowerMuons"),
    algoType    = cms.int32(5),
    debug       = cms.int32(2)
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('CosmicMuonAnalysis_2015_no-reRECO_nodxydz.root')
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
    +process.globalMuonSPFilter
    +process.analysisMuons
    +process.analysisGlobalMuons
    +process.analysisSPMuons
    +process.analysisGlobalSPMuons
    +process.analysisLowerTagTrackerMuons
    +process.analysisLowerTagTPFMSMuons
    +process.analysisLowerTagDYTMuons
    +process.analysisLowerTagPickyMuons
    +process.analysisLowerTagTunePMuons
    +process.analysisUpperTagTrackerMuons
    +process.analysisUpperTagTPFMSMuons
    +process.analysisUpperTagDYTMuons
    +process.analysisUpperTagPickyMuons
    +process.analysisUpperTagTunePMuons
    )

# generate EDM output
process.COSMICoutput_step = cms.EndPath(process.COSMICoutput)

# Schedule definition
process.schedule = cms.Schedule(
    process.muonanalysis
#    ,process.COSMICoutput_step
)
