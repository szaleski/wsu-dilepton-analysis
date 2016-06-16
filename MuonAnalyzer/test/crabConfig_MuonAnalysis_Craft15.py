from WMCore.Configuration import Configuration

config = Configuration()
config.section_('General')
config.General.requestName = 'CosmicMuonAnalysis_CRAFT_Apr8_2016'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False
config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'wsuMuonAnalyzer_data.py'
#config.JobType.outputFiles = ['Commisioning2015.root']
#config.JobType.pyCfgParams = []
config.section_('Data')
config.Data.inputDataset = '/Cosmics/Commissioning2015-CosmicSP-20Jan2016-v1/RAW-RECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 200
#NJOBS = 50
#config.Data.totalUnits = config.Data.unitsPerJob * NJOBS
config.Data.lumiMask = '/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/MuonAnalyzer/test/lumisToProcess.json'
#config.Data.runRange = ''
config.Data.allowNonValidInputDataset = True
#config.Data.outLFNDirBase = '/store/user/%s/' %(getUserNameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = 'MuonAnalysis_data_Apr_2016'
config.section_('Site')
config.Site.storageSite = 'T3_US_FNALLPC'
