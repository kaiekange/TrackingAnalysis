from WMCore.Configuration import Configuration
config = Configuration()

config.section_('General')
config.General.requestName = 'REQUESTNAME'
config.General.transferLogs = True
config.section_('JobType')
config.JobType.psetName = '../test/residuals.py'
config.JobType.pluginName = 'Analysis'
config.JobType.pyCfgParams = ['RunOnData=True', 'GlobalTag=GLOBALTAG', 'EventScale=EVENTSCALE', 'EventModulo=EVENTMODULO', 'SampleType=SAMPLETYPE']
config.JobType.allowUndistributedCMSSW = True
#config.JobType.maxMemoryMB = 4000
config.JobType.maxJobRuntimeMin = 3000 # min

config.section_('Data')
config.Data.splitting='LumiBased'
config.Data.totalUnits = -1
config.Data.unitsPerJob = 150

#config.Data.allowNonValidInputDataset = True
config.Data.lumiMask = "LUMIMASK"
config.Data.publication = False
config.Data.inputDataset = 'INPUTDATASET'
config.Data.outputDatasetTag = 'OUTPUTDATASETTAG'
config.Data.publishDBS = 'https://cmsweb.cern.ch/dbs/prod/phys03/DBSWriter'
config.Data.outLFNDirBase = 'OUTLFN'

config.section_('User')
config.User.voGroup = 'becms'
config.section_('Site')
# config.Site.storageSite = 'T2_CH_CERN'
config.Site.storageSite = 'T3_CH_CERNBOX'
