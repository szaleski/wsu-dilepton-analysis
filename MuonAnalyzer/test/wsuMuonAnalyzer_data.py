import FWCore.ParameterSet.Config as cms
process = cms.Process("MuonAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

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
    fileName = cms.untracked.string('Cosmics_CRAFT15_CosmicSP_ReReco.root'),
    outputCommands = process.RECOEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

#process.load("Configuration.StandardSequences.ReconstructionCosmics_cff")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(50000) )

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

# from Configuration.AlCa.GlobalTag import GlobalTag
# process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc')
#process.GlobalTag.globaltag = "74X_CRAFTR_V1::All"
#if runningOnMC == False:
#    process.GlobalTag.globaltag = "GR_R_52_V7::All"

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        #'file:///afs/cern.ch/user/s/sturdy/work/public/WSUAnalysis/DiLeptons/CosmicSP_2012Av1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/public/WSUAnalysis/DiLeptons/CosmicSP_2012Bv1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/public/WSUAnalysis/DiLeptons/CosmicSP_2012Cv1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/public/WSUAnalysis/DiLeptons/CosmicSP_2012Dv1RECO.root'
        #'file:///afs/cern.ch/user/s/sturdy/work/public/WSUAnalysis/DiLeptons/CosmicSP_2015v1RECO.root'
        #'/store/data/Commissioning2015/Cosmics/RECO/PromptReco-v1/000/232/928/00000/94B73492-63A8-E411-A07B-02163E01054C.root'
        #'/store/data/Commissioning2015/Cosmics/AOD/PromptReco-v1/000/232/928/00000/E64DC997-63A8-E411-BB15-02163E0124F8.root',
        #'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/00000/48E03A75-FB0D-E511-AE49-0025905B85AE.root'
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/00000/48E03A75-FB0D-E511-AE49-0025905B85AE.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/00000/4A2DF4C9-E90D-E511-8039-0025905B85D6.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/00000/4CFD7F03-D40E-E511-8F22-0025905A60E0.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/00000/F4CE8C38-F10D-E511-ACB9-002590593872.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/00000/FCA74C01-0A0E-E511-AAD1-0025905B8582.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/02DFAB4A-E80D-E511-8215-0025905A6068.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/1451FDD0-270E-E511-9EDA-0025905A60CA.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/161342C3-180E-E511-B412-0025905B85B2.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/38AB2B41-290E-E511-A046-0025905A6134.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/44252F6B-F30D-E511-A672-0025905A6070.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/4A95CCFD-330E-E511-9FB8-0025905B8596.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/522451C1-180E-E511-963A-0025905A60CA.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/56012F9E-180E-E511-8CE2-002618FDA208.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/58CE13FC-1A0E-E511-B47E-002590593920.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/6C2DBB20-1D0E-E511-A089-0025905AA9CC.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/744E19F8-2C0E-E511-8775-0025905A60AA.root',
        'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/10000/7CA6B1B4-180E-E511-B4B2-003048FFD7D4.root',

        #'file:///afs/cern.ch/work/s/szaleski/private/CMSSW_7_4_12/src/WSUCosmicAnalysis/MuonAnalyzer/test/crab_projects/crab_MuonAnalysis_Oct22_New_2015/results/CosmicMuonAnalysis_2015_4.root '
    )
)
basic_cut  = "pt > 45"
#basic_cut += " && (abs(muonBestTrack.dxy) < 50.)"
#basic_cut += " && (abs(muonBestTrack.dz)  < 100.)"
process.betterMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("muons"),
    cut = cms.string(basic_cut),
)

process.globalMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterMuons"),
    cut = cms.string("isGlobalMuon"),
)

process.upperMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterMuons"),
    cut = cms.string("muonBestTrack.innerPosition.Y > 0"),
)

process.lowerMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterMuons"),
    cut = cms.string("muonBestTrack.innerPosition.Y < 0"),
)


process.upperGlobalMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("upperMuons"),
    cut = cms.string("isGlobalMuon"),
)

process.lowerGlobalMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("lowerMuons"),
    cut = cms.string("isGlobalMuon"),
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
    fileName = cms.string('CosmicMuonAnalysis_2015_no-reRECO_nodxydz.root')
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
    process.betterMuons
    +process.upperMuons
    +process.lowerMuons
    +process.upperGlobalMuons
    +process.lowerGlobalMuons
    #process.reconstructionCosmics
    #process.analysis1Leg
    #+process.analysisSplit
    #process.analysisLHC
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
    #+process.analysisT0Corr
    #+process.analysisBHECOnly
    #+process.analysisNoRPC
    )

# Schedule definition
process.schedule = cms.Schedule(
#    process.raw2digi_step,
#    process.L1Reco_step,
#    process.reconstruction_step,
    process.muonanalysis
#    process.RECOoutput_step
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
