import FWCore.ParameterSet.Config as cms
process = cms.Process("MuonAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

# load conditions from the global tag, what to use here?
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '') ## default = ?
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc_50nsGRun', '') ## L1GtTriggerMenu_L1Menu_Collisions2012_v3_mc
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', 'FULL') ## L1Menu_Collisions2015_25nsStage1_v5

l1path = 'L1_SingleMuOpen'
from HLTrigger.HLTfilters.triggerResultsFilter_cfi import triggerResultsFilter
process.trigFilter = triggerResultsFilter.clone()
process.trigFilter.triggerConditions = cms.vstring("HLT_L1SingleMuOpen*")
process.trigFilter.l1tResults        = cms.InputTag('gtDigis','','HLT')
process.trigFilter.hltResults        = cms.InputTag('TriggerResults','','HLT')

from WSUDiLeptons.MuonAnalyzer.inputfiles import *

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        mcfilespt100startup
        #mcfilespt100asym
        #dyfiles
        )
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000) )

process.load("WSUDiLeptons.MuonAnalyzer.wsuMuonCollections_cfi")
process.load("WSUDiLeptons.MuonAnalyzer.wsuTrackCollections_cfi")
process.COSMICoutput.fileName = cms.untracked.string('CosmicTree_MC_CosmicSP_76X.root')

process.load("WSUDiLeptons.MuonAnalyzer.wsuFakeL1SingleMuFilter_cfi")
process.singleMuFilter.filterEvent = cms.bool(False)

from WSUDiLeptons.MuonAnalyzer.wsuTrackCollections_cfi import COSMICTrackoutput
process.COSMICoutput.outputCommands.append(COSMICTrackoutput)

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
    fakeL1SingleMuSrc = cms.InputTag("singleMuFilter"),
    isGen           = cms.bool(True)
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('CosmicMuonTree_MC_76X.root')
)

process.muonSPFilter.src = cms.InputTag("zprimeMuons")

process.muonanalysis = cms.Path(
    #process.trigFilter
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
