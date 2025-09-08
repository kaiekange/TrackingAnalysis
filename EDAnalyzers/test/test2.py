import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.parseArguments()
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()

source = cms.Source("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( ['/store/mc/Run3Summer22EEMiniAODv4/QCD_PT-50to80_TuneCP5_13p6TeV_pythia8/MINIAODSIM/130X_mcRun3_2022_realistic_postEE_v6-v2/2550000/56599597-2e05-403e-b75d-471f600d2d9b.root'] );

process = cms.Process("IpResiduals")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet( reportEvery = cms.untracked.int32(1) )

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '130X_mcRun3_2022_realistic_postEE_v6', '')

process.load("CondCore.CondDB.CondDB_cfi")
process.load('Configuration.Geometry.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = source

process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load('RecoVertex.PrimaryVertexProducer.OfflinePrimaryVertices_cfi')

process.load('TrackingAnalysis.EDAnalyzers.residuals_cfi')
process.residuals.BeamSpotConfig = ''
process.residuals.AddLostTracks = False

process.residuals.RunOnData = False

process.residuals.stageL1Trigger = cms.uint32(2)

process.residuals.EventScale = cms.int32(1)

process.TFileService = cms.Service("TFileService",
        fileName = cms.string("QCD_test.root"),
        closeFileFast = cms.untracked.bool(True)
        )

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.p = cms.Path( process.residuals )
