import FWCore.ParameterSet.Config as cms
process = cms.Process("MuonPtScaling")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

from WSUDiLeptons.MuonAnalyzer.inputfiles import *

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'root://xrootd-cms.infn.it///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/0CDD0D0E-8AC5-E511-AEBB-001EC9ADC726.root'
        #mcfilespt10asym
        #dyfiles
    )
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.load("WSUDiLeptons.MuonAnalyzer.wsuMuonCollections_cfi")

from WSUDiLeptons.MuonAnalyzer.wsuTrackCollections_cfi import COSMICTrackoutput
process.COSMICoutput.outputCommands.append(COSMICTrackoutput)

from WSUDiLeptons.MuonAnalyzer.wsuMuonPtScaling_cfi import *
process.load("WSUDiLeptons.MuonAnalyzer.wsuMuonCollections_cfi")
## for comparing with standard collision data/MC
# can't get y position from AOD/AODSIM, lives in TrackExtra not stored in AOD

# load conditions from the global tag, what to use here?
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

l1path = 'L1_SingleMuOpen'
from HLTrigger.HLTfilters.triggerResultsFilter_cfi import triggerResultsFilter
process.trigFilter = triggerResultsFilter.clone()
process.trigFilter.triggerConditions = cms.vstring("HLT_L1SingleMuOpen*")
process.trigFilter.l1tResults        = cms.InputTag('gtDigis','','HLT')
process.trigFilter.hltResults        = cms.InputTag('TriggerResults','','HLT')

process.analysisPtScalingLower = muonPtScaling.clone(
    muonSrc = cms.InputTag("zprimeLowerMuons"),
    debug   = cms.bool(True)
)
process.analysisPtScaling = muonPtScaling.clone(
    muonSrc     = cms.InputTag("zprimeMuons"),
)
process.analysisPtScalingUpper = muonPtScaling.clone(
    muonSrc     = cms.InputTag("zprimeUpperMuons"),
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('CosmicMuonPtScaling_MC_p10asym.root')
)

process.muonSPFilter.src = cms.InputTag("zprimeMuons")

process.muonanalysis = cms.Path(
    process.trigFilter
    +process.zprimeMuons
    +process.zprimeLowerMuons
    +process.zprimeUpperMuons
    +process.muonSPFilter
    +process.analysisPtScaling
    +process.analysisPtScalingLower
    +process.analysisPtScalingUpper
)

# Schedule definition
process.schedule = cms.Schedule(
    process.muonanalysis
)
