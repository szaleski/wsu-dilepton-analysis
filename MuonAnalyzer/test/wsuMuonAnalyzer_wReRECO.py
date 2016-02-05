import FWCore.ParameterSet.Config as cms
process = cms.Process("MuonAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContentCosmics_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.ReconstructionCosmics_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

# Output definition

process.RECOoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('RECO'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('Cosmics_CRAFT15_CosmicSP_ReRECO.root'),
    outputCommands = process.RECOEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

#process.load("Configuration.StandardSequences.ReconstructionCosmics_cff")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(500) )

# choose GlobalTag among: 74X_CRAFTR_V1, 74X_CRAFTR_V2, 74X_CRAFTR_V3  (without Muon APE)
#                         74X_CRAFT_V1A, 74X_CRAFT_V2A, 74X_CRAFT_V3A  (with Muon APE) 
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '74X_CRAFTR_V1', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '74X_CRAFT_V1A', '') #with APEs
#
# add a necessary condition for CMSSW release >= 7_4_8 
process.GlobalTag.toGet = cms.VPSet(
  cms.PSet(record = cms.string("HBHENegativeEFilterRcd"),
           tag = cms.string("HBHENegativeEFilter_V00_data"),
           connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
          )
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/00000/48E03A75-FB0D-E511-AE49-0025905B85AE.root'
    )
)
from WSUDiLeptons.MuonAnalyzer.wsuMuonAnalyzer_cfi import muonAnalysis

process.analysis1Leg = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons1Leg")
    )
process.analysisLHC = muonAnalysis.clone(
    muonSrc = cms.InputTag("lhcSTAMuons")
)
process.analysisSplit = muonAnalysis.clone(
    muonSrc = cms.InputTag("splitMuons")
)
process.analysisTrackerMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons"),
    algoType = cms.int32(1),
    debug = cms.int32(0)
)
process.analysisTPFMSMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons"),
    algoType = cms.int32(2),
    debug = cms.int32(0)
)
process.analysisDYTMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons"),
    algoType = cms.int32(3),
    debug = cms.int32(0)
)
process.analysisPickyMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons"),
    algoType = cms.int32(4),
    debug = cms.int32(0)
)
process.analysisTunePMuons = muonAnalysis.clone(
    muonSrc = cms.InputTag("muons"),
    algoType = cms.int32(5),
    debug = cms.int32(3)
)
process.analysisT0Corr = muonAnalysis.clone(
    muonSrc = cms.InputTag("muonsWitht0Correction")
)
process.analysisBHECOnly = muonAnalysis.clone(
    muonSrc = cms.InputTag("muonsBeamHaloEndCapsOnly")
)
process.analysisNoRPC = muonAnalysis.clone(
    muonSrc = cms.InputTag("muonsNoRPC")
)


process.TFileService = cms.Service("TFileService",
    fileName = cms.string('CosmicMuonAnalysis_2015_reRECO.root')
)

# fix: remove some L1 modules (useless here)
#process.my_RawToDigi = cms.Sequence(
#    process.csctfDigis
#    +process.dttfDigis
#    +process.L1RawToDigiSeq
#    +process.caloStage1Digis
#    +process.caloStage1LegacyFormatDigis
#    +process.gtDigis
#    +process.siPixelDigis
#    +process.siStripDigis
#    +process.ecalDigis
#    +process.ecalPreshowerDigis
#    +process.hcalDigis
#    +process.muonCSCDigis
#    +process.muonDTDigis
#    +process.muonRPCDigis
#    +process.castorDigis
#    +process.scalersRawToDigi
#    +process.tcdsDigis
#    )

process.my_RawToDigi = cms.Sequence(
    process.csctfDigis+process.dttfDigis
    +process.gtDigis+process.siPixelDigis
    +process.siStripDigis
    +process.ecalDigis
    +process.ecalPreshowerDigis
    +process.hcalDigis
    +process.muonCSCDigis
    +process.muonDTDigis
    +process.muonRPCDigis
    +process.castorDigis
    +process.scalersRawToDigi
    +process.tcdsDigis
)

# Path and EndPath definitions
#process.raw2digi_step      = cms.Path(process.RawToDigi)
process.raw2digi_step       = cms.Path(process.my_RawToDigi)
process.L1Reco_step         = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstructionCosmics)
process.RECOoutput_step     = cms.EndPath(process.RECOoutput)

process.muonanalysis = cms.Path(
    #process.reconstructionCosmics
    #process.analysis1Leg
    #+process.analysisSplit
    #process.analysisLHC
    process.analysisTrackerMuons
    +process.analysisTPFMSMuons
    +process.analysisDYTMuons
    +process.analysisPickyMuons
    +process.analysisTunePMuons
    #+process.analysisT0Corr
    #+process.analysisBHECOnly
    #+process.analysisNoRPC
    )

# Schedule definition
process.schedule = cms.Schedule(
    process.raw2digi_step,
    process.L1Reco_step,
    process.reconstruction_step,
    process.muonanalysis
    #process.RECOoutput_step
)

# customisation of the process.

# Automatic addition of the customisation function from Configuration.DataProcessing.RecoTLR
from Configuration.DataProcessing.RecoTLR import customiseCosmicDataRun2 

#call to customisation function customiseCosmicDataRun2 imported from Configuration.DataProcessing.RecoTLR
process = customiseCosmicDataRun2(process)

# End of customisation functions


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
