import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
import FWCore.PythonUtilities.LumiList as LumiList

options = VarParsing("analysis")
options.register("RunOnData", False, VarParsing.multiplicity.singleton, VarParsing.varType.bool, "Run on data")
options.register("GlobalTag", "124X_dataRun3_v15", VarParsing.multiplicity.singleton, VarParsing.varType.string, "Global tag")
options.register("EventScale", 100, VarParsing.multiplicity.singleton, VarParsing.varType.int, "Event scale")
options.register("EventModulo", 0, VarParsing.multiplicity.singleton, VarParsing.varType.int, "Event modulo")
options.register("SampleType", "ZeroBias", VarParsing.multiplicity.singleton, VarParsing.varType.string, "Sample type")
options.register("InputFile", "file:input.root", VarParsing.multiplicity.singleton, VarParsing.varType.string, "Input file")
options.register("OutputFile", "file:output.root", VarParsing.multiplicity.singleton, VarParsing.varType.string, "Output file")
options.register("GoldenJSON", "GOLDENJSON.json", VarParsing.multiplicity.singleton, VarParsing.varType.string, "Golden JSON")
options.parseArguments()

input_files = [f.strip() for f in options.InputFile.split() if f.strip()]

source = cms.Source("PoolSource", fileNames=cms.untracked.vstring(*input_files), skipBadFiles=cms.untracked.bool(True))

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
if options.RunOnData:
    print("is data")
    process.source.lumisToProcess = LumiList.LumiList(filename=options.GoldenJSON).getVLuminosityBlockRange()

process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load("RecoVertex.PrimaryVertexProducer.OfflinePrimaryVertices_cfi")

process.load("TrackingAnalysis.EDAnalyzers.residuals_cfi")
process.residuals.BeamSpotConfig = ""

process.residuals.stageL1Trigger = cms.uint32(2)

process.residuals.RunOnData = cms.bool(options.RunOnData)
process.residuals.EventScale = cms.int32(options.EventScale)
process.residuals.EventModulo = cms.int32(options.EventModulo)
process.residuals.SampleType = cms.string(options.SampleType)

process.TFileService = cms.Service("TFileService", fileName=cms.string(options.OutputFile), closeFileFast=cms.untracked.bool(True))

process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(False), TryToContinue=cms.untracked.vstring("ProductNotFound"))

process.p = cms.Path(process.residuals)
