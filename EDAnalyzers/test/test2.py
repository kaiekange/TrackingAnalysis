import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing("analysis")
options.register("RunOnData", True, VarParsing.multiplicity.singleton, VarParsing.varType.bool, "Run on data")
options.register("GlobalTag", "150X_dataRun3_v2", VarParsing.multiplicity.singleton, VarParsing.varType.string, "Global tag")
options.register("EventScale", 1, VarParsing.multiplicity.singleton, VarParsing.varType.int, "Event scale")
options.register("EventModulo", 0, VarParsing.multiplicity.singleton, VarParsing.varType.int, "Event modulo")
options.register("SampleType", "ZeroBias", VarParsing.multiplicity.singleton, VarParsing.varType.string, "Sample type")
options.parseArguments()

readFiles = cms.untracked.vstring()

source = cms.Source("PoolSource", fileNames=readFiles)
readFiles.extend(["/store/data/Run2024C/ZeroBias/MINIAOD/MINIv6NANOv15-v1/2530000/004c523d-7e53-40e5-9ca0-7063f00703cb.root"])

process = cms.Process("IpResiduals")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(reportEvery=cms.untracked.int32(1))

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

process.GlobalTag.globaltag = options.GlobalTag

process.load("CondCore.CondDB.CondDB_cfi")
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

process.source = source

process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load("RecoVertex.PrimaryVertexProducer.OfflinePrimaryVertices_cfi")

process.load("TrackingAnalysis.EDAnalyzers.residuals_cfi")
process.residuals.BeamSpotConfig = ""

process.residuals.stageL1Trigger = cms.uint32(2)

process.residuals.RunOnData = cms.bool(options.RunOnData)
process.residuals.EventScale = cms.int32(options.EventScale)
process.residuals.EventModulo = cms.int32(options.EventModulo)
process.residuals.SampleType = cms.string(options.SampleType)

process.TFileService = cms.Service("TFileService", fileName=cms.string("test.root"), closeFileFast=cms.untracked.bool(True))

process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(False), TryToContinue=cms.untracked.vstring("ProductNotFound"))

process.p = cms.Path(process.residuals)
