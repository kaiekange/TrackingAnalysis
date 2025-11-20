import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.register('RunOnData', False, VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Run on data')
# options.register('GlobalTag', '124X_dataRun3_v15', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Global tag')
# options.register('GlobalTag', '130X_dataRun3_PromptAnalysis_v1', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Global tag')
options.register('GlobalTag', '150X_mcRun3_2024_realistic_v2', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Global tag')
options.register('EventScale', 100, VarParsing.multiplicity.singleton, VarParsing.varType.int, 'Event scale')
options.register('EventModulo', 10, VarParsing.multiplicity.singleton, VarParsing.varType.int, 'Event modulo')
options.parseArguments()

readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()

source = cms.Source("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
# readFiles.extend( ['/store/data/Run2024C/ZeroBias/MINIAOD/MINIv6NANOv15-v1/2530000/8aaad4e6-eb36-4314-b223-4c9916141f59.root'] )
# readFiles.extend( ['/store/data/Run2023C/ZeroBias1/MINIAOD/22Sep2023_v4-v1/60000/b37d1dea-45c1-4814-818c-4226a4264cde.root'] )
# readFiles.extend( ['/store/mc/RunIII2024Summer24MiniAODv6/SingleNeutrino_Par-E-10_gun/MINIAODSIM/150X_mcRun3_2024_realistic_v2-v2/120000/096f7efe-6846-4acc-b5d5-f5aa7db44286.root'] )
readFiles.extend( ['/store/mc/RunIII2024Summer24MiniAODv6/QCD_Bin-PT-170to300_Fil-MuEnriched_TuneCP5_13p6TeV_pythia8/MINIAODSIM/150X_mcRun3_2024_realistic_v2-v2/120000/faa37b4f-5227-4b03-b613-5fd30c8f6350.root'] )

process = cms.Process("IpResiduals")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet( reportEvery = cms.untracked.int32(1) )

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = options.GlobalTag 

process.load("CondCore.CondDB.CondDB_cfi")
process.load('Configuration.Geometry.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(50) )

process.source = source

process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load('RecoVertex.PrimaryVertexProducer.OfflinePrimaryVertices_cfi')

process.load('TrackingAnalysis.EDAnalyzers.residuals_cfi')
process.residuals.BeamSpotConfig = ''

process.residuals.stageL1Trigger = cms.uint32(2)

process.residuals.RunOnData = cms.bool(options.RunOnData)
process.residuals.EventScale = cms.int32(options.EventScale)
process.residuals.EventModulo = cms.int32(options.EventModulo)

process.TFileService = cms.Service("TFileService",
        fileName = cms.string("output2.root"),
        closeFileFast = cms.untracked.bool(True)
        )

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.p = cms.Path( process.residuals )
