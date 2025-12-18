import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
import FWCore.PythonUtilities.LumiList as LumiList

options = VarParsing('analysis')
options.register('RunOnData', True, VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Run on data')
options.register('GlobalTag', '130X_dataRun3_PromptAnalysis_v1', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Global tag')
options.register('EventScale', 10, VarParsing.multiplicity.singleton, VarParsing.varType.int, 'Event scale')
options.register('EventModulo', 0, VarParsing.multiplicity.singleton, VarParsing.varType.int, 'Event modulo')
options.register('InputFile', 'file:input.root', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Input file')
options.register('OutputFile', 'file:output.root', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Output file')
options.register('GoldenJSON', '/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2023_366442_370790_Golden.json', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Golden JSON')
options.parseArguments()

# input_files = ['/store/data/Run2023C/JetMET0/MINIAOD/22Sep2023_v2-v1/30000/5b42bc84-8eb1-4bee-b18b-773fa40aaeba.root']
# input_files = ['/store/data/Run2023C/JetMET0/MINIAOD/22Sep2023_v2-v1/2540000/6acc76d4-7cff-4bc4-9fa4-7f2ff8208b28.root']
input_files = ["root://cms-xrd-global.cern.ch//store/data/Run2023D/JetMET1/MINIAOD/22Sep2023_v1-v1/2540000/16812c27-c7da-4a28-a98b-b9e2c12f05f5.root"]

print(input_files)

source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(*input_files),
    skipBadFiles    = cms.untracked.bool(True),
    skipEvents = cms.untracked.uint32(16000)
)

process = cms.Process("IpResiduals")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet( reportEvery = cms.untracked.int32(1) )

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.globaltag = options.GlobalTag 

process.load("CondCore.CondDB.CondDB_cfi")
process.load('Configuration.Geometry.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = source

if(options.RunOnData) :
    print("is data")
    process.source.lumisToProcess = LumiList.LumiList(
        filename = options.GoldenJSON
    ).getVLuminosityBlockRange()

process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load('RecoVertex.PrimaryVertexProducer.OfflinePrimaryVertices_cfi')

process.load('TrackingAnalysis.EDAnalyzers.residuals_cfi')
process.residuals.BeamSpotConfig = ''

process.residuals.stageL1Trigger = cms.uint32(2)

process.residuals.RunOnData = cms.bool(options.RunOnData)
process.residuals.EventScale = cms.int32(options.EventScale)
process.residuals.EventModulo = cms.int32(options.EventModulo)

process.TFileService = cms.Service( "TFileService", fileName = cms.string(options.OutputFile), closeFileFast = cms.untracked.bool(True) )

# process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False), TryToContinue = cms.untracked.vstring('ProductNotFound') )

process.p = cms.Path( process.residuals )
