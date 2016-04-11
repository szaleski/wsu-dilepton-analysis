import FWCore.ParameterSet.Config as cms
process = cms.Process("MuonAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
#process.options.SkipEvent = cms.untracked.vstring('ProductNotFound')

# load conditions from the global tag, what to use here?
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')

from WSUDiLeptons.MuonAnalyzer.inputfiles import *

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        #datafiles
        interfillfiles
    )
)

process.source.inputCommands = cms.untracked.vstring(
    "keep *",
    "drop FEDRawDataCollection_rawDataCollector_*_*",
)
process.source.dropDescendantsOfDroppedBranches = cms.untracked.bool(False)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000) )

process.load("WSUDiLeptons.MuonAnalyzer.wsuMuonCollections_cfi")
process.load("WSUDiLeptons.MuonAnalyzer.wsuTrackCollections_cfi")
process.COSMICoutput.fileName = cms.untracked.string('CosmicTree_data_CosmicSP_76X.root')

from WSUDiLeptons.MuonAnalyzer.wsuTrackCollections_cfi import COSMICTrackoutput
process.COSMICoutput.outputCommands.append(COSMICTrackoutput)

process.load("WSUDiLeptons.MuonAnalyzer.wsuFakeL1SingleMuFilter_cfi")
process.singleMuFilter.l1MuonSrc   = cms.InputTag("l1extraParticles")
process.singleMuFilter.filterEvent = cms.bool(False)

from WSUDiLeptons.MuonAnalyzer.wsuMuonTree_cfi import *

process.analysisSPMuons = muonTree.clone(
    muonSrc         = cms.InputTag("zprimeMuons"),
    upperLegSrc     = cms.InputTag("zprimeUpperMuons"),
    lowerLegSrc     = cms.InputTag("zprimeLowerMuons"),
    globalTrackSrc  = cms.InputTag("globalSPMuonTracks"),
    cosmicTrackSrc  = cms.InputTag("cosmicSPMuonTracks"),
    trackerTrackSrc = cms.InputTag("trackerSPMuonTracks"),
    algoType        = cms.int32(5),
    debug           = cms.int32(2),
    trigResultsSrc  = cms.InputTag('TriggerResults','','HLT'),
    hltTrigCut      = cms.string('L1SingleMuOpen'),
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('CosmicMuonTree_data_76X.root')
)

process.muonSPFilter.src = cms.InputTag("zprimeMuons")

process.muonanalysis = cms.Path(
    process.singleMuFilter
    +process.zprimeMuons
    +process.zprimeLowerMuons
    +process.zprimeUpperMuons
    +process.cosmicMuonTracks
    +process.globalMuonTracks
    +process.trackerMuonTracks
    +process.cosmicSPMuonTracks
    +process.globalSPMuonTracks
    +process.trackerSPMuonTracks
    #+process.muonSPFilter
    +process.analysisSPMuons
    )

# generate EDM output
process.COSMICoutput_step = cms.EndPath(process.COSMICoutput)

# Schedule definition
process.schedule = cms.Schedule(
    process.muonanalysis
#    ,process.COSMICoutput_step
)
