from WMCore.Configuration import Configuration

config = Configuration()

config.section_('General')
config.General.requestName = 'MuonEfficiencyTree_CRAFT15_Apr07_recovery'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'wsuMuonAnalyzer_data.py'

config.section_('Data')
config.Data.inputDataset = '/Cosmics/Commissioning2015-CosmicSP-20Jan2016-v1/RAW-RECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 250
#config.Data.lumiMask = 'https://cmsdoc.cern.ch/~sturdy/Cosmics/JSON/21.03.2016/cosmics_CRAFT15_pix_strip_DT_RPC_complete.json'
config.Data.lumiMask = './crab_projects/crab_MuonEfficiencyTree_CRAFT15_Apr07/results/notFinishedLumis.json'
config.Data.allowNonValidInputDataset = False
#config.Data.publication = True
config.Data.outLFNDirBase = '/store/user/sturdy/MuonEfficiency'
config.Data.outputDatasetTag = 'MuonEfficiencyTree_CRAFT15_Apr07_recovery'

config.section_('Site')
config.Site.storageSite = 'T3_US_FNALLPC'
