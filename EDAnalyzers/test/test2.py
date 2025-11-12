import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.register('GlobalTag', '124X_dataRun3_v15', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Global tag')
options.register('EventScale', 100, VarParsing.multiplicity.singleton, VarParsing.varType.int, 'Event scale')
options.register('EventModulo', 10, VarParsing.multiplicity.singleton, VarParsing.varType.int, 'Event modulo')
options.parseArguments()

readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()

source = cms.Source("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( ['/store/data/Run2024C/ZeroBias/MINIAOD/MINIv6NANOv15-v1/2530000/8aaad4e6-eb36-4314-b223-4c9916141f59.root'] )

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

process.residuals.EventScale = cms.int32(options.EventScale)
process.residuals.EventModulo = cms.int32(options.EventModulo)

process.TFileService = cms.Service("TFileService",
        fileName = cms.string("output.root"),
        closeFileFast = cms.untracked.bool(True)
        )

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.p = cms.Path( process.residuals )
