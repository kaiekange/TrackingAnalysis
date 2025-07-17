from WMCore.Configuration import Configuration
config = Configuration()

config.section_('General')
config.General.requestName = 'ZeroBias_Run2022F_22Sep2023_v1_MINIAOD_ver0'
config.General.transferLogs = True
config.section_('JobType')
config.JobType.psetName = '../test/residuals.py'
config.JobType.pluginName = 'Analysis'
config.JobType.pyCfgParams = ['isData=1', 'withBS=0', 'is2022=0']
config.JobType.allowUndistributedCMSSW = True
#config.JobType.maxMemoryMB = 4000
##config.JobType.maxJobRuntimeMin = 2749 # min

config.section_('Data')
config.Data.splitting='LumiBased'
config.Data.totalUnits = -1
config.Data.unitsPerJob = 50

#config.Data.allowNonValidInputDataset = True
config.Data.lumiMask = 'JSON/Cert_Collisions2022_355100_362760_Golden.json'
config.Data.publication = False
config.Data.inputDataset = '/ZeroBias/Run2022F-22Sep2023-v1/MINIAOD'
config.Data.outputDatasetTag = 'Run2022F_22Sep2023_v1_MINIAOD'
config.Data.publishDBS = 'https://cmsweb.cern.ch/dbs/prod/phys03/DBSWriter'
config.Data.outLFNDirBase = '/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250716_noBS'

config.section_('User')
config.User.voGroup = 'becms'
config.section_('Site')
config.Site.storageSite = 'T2_BE_IIHE'
#config.Site.whitelist = ['T2_BE_IIHE']
