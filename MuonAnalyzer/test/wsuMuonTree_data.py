import FWCore.ParameterSet.Config as cms
process = cms.Process("MuonAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

from WSUDiLeptons.MuonAnalyzer.inputfiles import *

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        datafiles
    )
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000) )

process.load("WSUDiLeptons.MuonAnalyzer.wsuMuonCollections_cfi")
process.load("WSUDiLeptons.MuonAnalyzer.wsuTrackCollections_cfi")
process.COSMICoutput.fileName = cms.untracked.string('CosmicTree_CRAFT15_CosmicSP_76X.root')

from WSUDiLeptons.MuonAnalyzer.wsuTrackCollections_cfi import COSMICTrackoutput
process.COSMICoutput.outputCommands.append(COSMICTrackoutput)

from WSUDiLeptons.MuonAnalyzer.wsuMuonTree_cfi import *

## for comparing with standard collision data/MC
# can't get y position from AOD/AODSIM, lives in TrackExtra not stored in AOD
#process.cosmicMuonTracks.src  = cms.InputTag("standAloneMuons")
#process.globalMuonTracks.src  = cms.InputTag("globalMuons")
#process.trackerMuonTracks.src = cms.InputTag("generalTracks")

process.analysisMuons = muonTree.clone(
    muonSrc     = cms.InputTag("betterMuons"),
    upperLegSrc = cms.InputTag("betterMuons"),
    lowerLegSrc = cms.InputTag("betterMuons"),
    algoType    = cms.int32(1),
    debug       = cms.int32(2)
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
    algoType    = cms.int32(5),
    debug       = cms.int32(2)
)
process.analysisGlobalSPMuons = muonTree.clone(
    muonSrc     = cms.InputTag("globalSPMuons"),
    upperLegSrc = cms.InputTag("upperGlobalMuons"),
    lowerLegSrc = cms.InputTag("lowerGlobalMuons"),
    algoType    = cms.int32(5),
    debug       = cms.int32(2)
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
    fileName = cms.string('CosmicMuonTree_76X_reRECO.root')
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
    +process.cosmicMuonTracks
    +process.globalMuonTracks
    +process.trackerMuonTracks
    +process.cosmicSPMuonTracks
    +process.globalSPMuonTracks
    +process.trackerSPMuonTracks
    #+process.muonSPFilter
    #+process.globalMuonSPFilter
    #+process.analysisMuons
    #+process.analysisGlobalMuons
    +process.analysisSPMuons
    #+process.analysisGlobalSPMuons
    #+process.analysisTrackerMuons
    #+process.analysisTPFMSMuons
    #+process.analysisDYTMuons
    #+process.analysisPickyMuons
    #+process.analysisTunePMuons
    )

# generate EDM output
process.COSMICoutput_step = cms.EndPath(process.COSMICoutput)

# Schedule definition
process.schedule = cms.Schedule(
    process.muonanalysis
    ,process.COSMICoutput_step
)
