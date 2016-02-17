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
        filterName = cms.untracked.string('muonanalysis')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('Cosmics_deco_p100_CosmicSP_ReReco.root'),
    #outputCommands = process.RECOEventContent.outputCommands,
    outputCommands = cms.untracked.vstring('drop *',
                                           'keep *_betterMuons_*_*',
                                           'keep *_globalMuons_*_*',
                                           'keep *_betterSPMuons_*_*',
                                           'keep *_globalSPMuons_*_*',
                                           'keep *_lowerMuons_*_*',
                                           'keep *_lowerGlobalMuons_*_*',
                                           'keep *_upperMuons_*_*',
                                           'keep *_upperGlobalMuons_*_*',
                                           "keep *_tevMuons_*_*",
                                           "keep *_beamhaloTracks_*_*",
                                           "keep *_ckfInOutTracksFromConversions_*_*",
                                           "keep *_ckfOutInTracksFromConversions_*_*",
                                           "keep *_cosmicMuons_*_*",
                                           "keep *_cosmicMuons1Leg_*_*",
                                           "keep *_cosmicMuonsEndCapsOnly_*_*",
                                           "keep *_cosmicMuonsNoRPC_*_*",
                                           "keep *_cosmicMuonsWitht0Correction_*_*",
                                           "keep *_cosmictrackfinderP5_*_*",
                                           "keep *_ctfWithMaterialTracksP5_*_*",
                                           "keep *_ctfWithMaterialTracksP5LHCNavigation_*_*",
                                           "keep *_globalBeamHaloMuonEndCapslOnly_*_*",
                                           "keep *_globalCosmicMuons_*_*",
                                           "keep *_globalCosmicMuons1Leg_*_*",
                                           "keep *_globalCosmicMuonsNoRPC_*_*",
                                           "keep *_globalCosmicMuonsWitht0Correction_*_*",
                                           "keep *_globalCosmicSplitMuons_*_*",
                                           "keep *_regionalCosmicTracks_*_*",
                                           "keep *_splittedTracksP5_*_*",
                                           "keep *_standAloneMuons_*_*",
                                           "keep *_beamhaloTracks_*_*",
                                           "keep *_ckfInOutTracksFromConversions_*_*",
                                           "keep *_ckfOutInTracksFromConversions_*_*",
                                           "keep *_cosmicMuons_*_*",
                                           "keep *_cosmicMuons1Leg_*_*",
                                           "keep *_cosmicMuonsEndCapsOnly_*_*",
                                           "keep *_cosmicMuonsNoRPC_*_*",
                                           "keep *_cosmicMuonsWitht0Correction_*_*",
                                           "keep *_cosmictrackfinderP5_*_*",
                                           "keep *_ctfWithMaterialTracksP5_*_*",
    ),
    splitLevel = cms.untracked.int32(0)
)

#process.load("Configuration.StandardSequences.ReconstructionCosmics_cff")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(15000) )

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
        #'/store/data/Commissioning2015/Cosmics/RECO/PromptReco-v1/000/232/928/00000/94B73492-63A8-E411-A07B-02163E01054C.root'
        #'/store/data/Commissioning2015/Cosmics/AOD/PromptReco-v1/000/232/928/00000/E64DC997-63A8-E411-BB15-02163E0124F8.root',
        #'root://cms-xrd-global.cern.ch///store/data/Commissioning2015/Cosmics/RAW-RECO/CosmicSP-04Jun2015-v1/00000/48E03A75-FB0D-E511-AE49-0025905B85AE.root'
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/02D2E238-63C5-E511-AAAA-BC305B3909F1.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/3ED5EB45-63C5-E511-A125-003048F2E8C2.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/703EE012-43C5-E511-8067-003048F2E64E.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/7CF947E3-39C5-E511-B505-782BCB54BAAE.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/B278C22A-63C5-E511-9B8A-782BCB54BA75.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/E286F1AF-2AC5-E511-BC8D-0023AEEEB6CD.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/007BE3C4-89C5-E511-8CA8-0022195578C8.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/009A91F8-88C5-E511-ACCE-001EC9ADF941.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/00AA48F0-88C5-E511-8150-901B0E5427A4.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/02574473-5FC6-E511-86BD-0090FAA58204.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/04AB0F56-89C5-E511-8A4C-00259021A53E.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/06522118-8AC5-E511-90FF-20CF305B050E.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/085542FD-88C5-E511-8815-001E6757CD34.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/08EC153C-8AC5-E511-B96F-901B0E5427B0.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/0A5C03C4-89C5-E511-A573-00259021A4B2.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/0CDD0D0E-8AC5-E511-AEBB-001EC9ADC726.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/163BFE7E-89C5-E511-AEFE-00259021A39E.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/165F29F4-88C5-E511-9D0A-20CF3019DEEF.root',
        #'/store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/1890F42B-8AC5-E511-A6E3-20CF3027A592.root',
#######################################################################################################################################################################
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/02D2E238-63C5-E511-AAAA-BC305B3909F1.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/3ED5EB45-63C5-E511-A125-003048F2E8C2.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/703EE012-43C5-E511-8067-003048F2E64E.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/7CF947E3-39C5-E511-B505-782BCB54BAAE.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/B278C22A-63C5-E511-9B8A-782BCB54BA75.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/50000/E286F1AF-2AC5-E511-BC8D-0023AEEEB6CD.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/007BE3C4-89C5-E511-8CA8-0022195578C8.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/009A91F8-88C5-E511-ACCE-001EC9ADF941.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/00AA48F0-88C5-E511-8150-901B0E5427A4.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/02574473-5FC6-E511-86BD-0090FAA58204.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/04AB0F56-89C5-E511-8A4C-00259021A53E.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/06522118-8AC5-E511-90FF-20CF305B050E.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/085542FD-88C5-E511-8815-001E6757CD34.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/08EC153C-8AC5-E511-B96F-901B0E5427B0.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/0A5C03C4-89C5-E511-A573-00259021A4B2.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/0CDD0D0E-8AC5-E511-AEBB-001EC9ADC726.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/163BFE7E-89C5-E511-AEFE-00259021A39E.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/165F29F4-88C5-E511-9D0A-20CF3019DEEF.root',
        'root://cms-xrd-global.cern.ch///store/mc/CosmicWinter15DR/SPLooseMuCosmic_38T_p100/GEN-SIM-RECO/DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v1/70000/1890F42B-8AC5-E511-A6E3-20CF3027A592.root',
    )
)
basic_cut  = "pt > 45"
process.betterMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("muons"),
    cut = cms.string(basic_cut),
)

process.globalMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterMuons"),
    cut = cms.string("isGlobalMuon"),
)

dxy_cut = "(abs(muonBestTrack.dxy) < 50.)"
dz_cut  = " && (abs(muonBestTrack.dz)  < 100.)"
process.betterSPMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterMuons"),
    cut = cms.string(dxy_cut+dz_cut),
)

process.globalSPMuons = cms.EDFilter("MuonSelector",
    src = cms.InputTag("betterSPMuons"),
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
    probeLegSrc = cms.InputTag("upperGlobalMuons"),
    algoType    = cms.int32(5),
    debug       = cms.int32(-1)
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
    probeLegSrc = cms.InputTag("lowerGlobalMuons"),
    algoType    = cms.int32(5),
    debug       = cms.int32(-1)
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
    fileName = cms.string('CosmicMuonAnalysis_2015_deco_p100_nodxydz_moreNew.root')
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
    process.betterMuons
    +process.globalMuons
    +process.betterSPMuons
    +process.globalSPMuons
    +process.upperMuons
    +process.lowerMuons
    +process.upperGlobalMuons
    +process.lowerGlobalMuons
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

# Schedule definition
process.schedule = cms.Schedule(
#    process.raw2digi_step,
#    process.L1Reco_step,
#    process.reconstruction_step,
    process.muonanalysis,
    process.RECOoutput_step
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
