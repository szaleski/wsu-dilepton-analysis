from CRABClient.UserUtilities import config, getUserNameFromSiteDB

config = config()

config.General.requestName = 'MuonAnalysis_Oct21_2015'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'wsuMuonAnalyzer_cfg.py'
#config.JobType.pyCfgParams = []

#config.Data.inputDataSet = ''
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 20
#NJOBS = 50
#config.Data.totalUnits = config.Data.unitsPerJob * NJOBS
config.Data.lumiMask = '/afs/cern.ch/work/s/szaleski/private/CMSSW_7_4_12/src/WSUCosmicAnalysis/MuonAnalyzer/test/cosmics_craft2015_json.txt'
#config.Data.runRange = ''
#config.Data.outLFNDirBase = '/store/user/%s/' %(getUserNameFromSiteDB())
config.Data.publication = True
config.Data.publishDataName = 'MuonAnalysis_Oct_2015'

config.Site.storageSite = 'T3_US_FNALLPC'
