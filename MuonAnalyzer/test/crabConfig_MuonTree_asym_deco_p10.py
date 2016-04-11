from WMCore.Configuration import Configuration

config = Configuration()

config.section_('General')

config.General.requestName = 'MuonEfficiencyTree_asym_deco_p10_Apr07'

config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'wsuMuonAnalyzer_MC.py'

config.section_('Data')
config.Data.inputDataset = '/SPLooseMuCosmic_38T_p10/CosmicWinter15DR-DECO_76X_mcRun2cosmics_asymptotic_deco_v0-v2/GEN-SIM-RECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 3
config.Data.allowNonValidInputDataset = False
#config.Data.publication = True
config.Data.outLFNDirBase = '/store/user/sturdy/MuonEfficiency'

config.Data.outputDatasetTag = 'MuonEfficiencyTree_asym_deco_p10_Apr07'


config.section_('Site')
config.Site.storageSite = 'T3_US_FNALLPC'
