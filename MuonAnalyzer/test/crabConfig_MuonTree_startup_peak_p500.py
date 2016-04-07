from WMCore.Configuration import Configuration

config = Configuration()

config.section_('General')
config.General.requestName = 'MuonEfficiencyTree_startup_peak_p500_Mar9'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'wsuMuonAnalyzer_MC.py'

config.section_('Data')
config.Data.inputDataset = '/SPLooseMuCosmic_38T_p500/CosmicWinter15DR-startupPEAK_76X_mcRun2cosmics_startup_peak_v0-v1/GEN-SIM-RECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 5
config.Data.allowNonValidInputDataset = True
#config.Data.publication = True
config.Data.outLFNDirBase = '/store/user/sturdy/'
config.Data.outputDatasetTag = 'MuonEfficiencyTree_startup_peak_p500_Mar9'

config.section_('Site')
config.Site.storageSite = 'T3_US_FNALLPC'