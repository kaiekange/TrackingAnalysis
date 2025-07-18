from WMCore.Configuration import Configuration
config = Configuration()

config.section_('General')
config.General.requestName = 'REQUESTNAME'
config.General.transferLogs = True
config.section_('JobType')
config.JobType.psetName = '../test/residuals.py'
config.JobType.pluginName = 'Analysis'
config.JobType.pyCfgParams = ['isData=0', 'withBS=0', 'is2022=1']
config.JobType.allowUndistributedCMSSW = True
##config.JobType.maxMemoryMB = 2500
##config.JobType.maxJobRuntimeMin = 2749 # min

config.section_('Data')
config.Data.splitting='FileBased'
#config.Data.splitting='EventAwareLumiBased'
config.Data.totalUnits = -1
#config.Data.unitsPerJob = 1
#config.Data.unitsPerJob = 100
config.Data.unitsPerJob = 1

#config.Data.allowNonValidInputDataset = True
config.Data.publication = False
config.Data.inputDataset = 'INPUTDATASET'
# config.Data.secondaryInputDataset = 'SECONDARYDATASET'
config.Data.outputDatasetTag = 'OUTPUTDATASETTAG'
config.Data.publishDBS = 'https://cmsweb.cern.ch/dbs/prod/phys03/DBSWriter'
config.Data.outLFNDirBase = 'OUTLFN'

config.section_('User')
config.User.voGroup = 'becms'
config.section_('Site')
config.Site.storageSite = 'T2_CH_CERN'
#config.Site.whitelist = ['T2_BE_IIHE']
