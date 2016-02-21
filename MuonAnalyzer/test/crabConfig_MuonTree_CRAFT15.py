from WMCore.Configuration import Configuration

config = Configuration()

config.section_('General')
config.General.requestName = 'MuonEfficiencyTree_CRAFT15_Feb21'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'wsuMuonTree_data.py'

config.section_('Data')
config.Data.inputDataset = '/Cosmics/Commissioning2015-CosmicSP-20Jan2016-v1/RAW-RECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 200
config.Data.lumiMask = 'http://cmsdoc.cern.ch/~sturdy/Cosmics/JSON/cosmics_CRAFT15_pix_strip_DT_RPC_complete.json'
config.Data.allowNonValidInputDataset = True
#config.Data.publication = True
config.Data.outLFNDirBase = /store/user/sturdy07/MuonEfficiency
config.Data.outputDatasetTag = 'MuonEfficiencyTree_CRAFT15_Feb21'

config.section_('Site')
config.Site.storageSite = 'T3_US_FNALLPC'
