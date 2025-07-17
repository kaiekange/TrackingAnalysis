import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.parseArguments()

readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()

source = cms.Source("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)

readFiles.extend( [
    "file:/user/kakang/IPres/3DBC2199-8FE7-F746-85BB-4668C75726A8.root"
    #"/store/data/Run2022A/JetHT/MINIAOD/22Sep2023-v1/50000/e8a996e4-f1df-4da4-9dd3-6c23bbdcc78e.root"
    #'/store/data/Run2016B/JetHT/MINIAOD/ver2_HIPM_UL2016_MiniAODv2-v2/230000/45CF386B-286D-7545-B097-238839251127.root'
]);

process = cms.Process("IpResiduals")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet( reportEvery = cms.untracked.int32(1) )

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag.globaltag = '130X_dataRun3_v2'
#process.GlobalTag.globaltag = '106X_dataRun2_v35'
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_dataRun2_v35', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '130X_dataRun3_v2', '')

process.load("CondCore.CondDB.CondDB_cfi")
process.load('Configuration.Geometry.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(50) )

process.source = source

#process.HLT = cms.EDFilter("HLTHighLevel",
#                           TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
#                           HLTPaths = cms.vstring('HLT_ZeroBiasPixel_DoubleTrack_v*','HLT_ZeroBias_v*'),
#                           eventSetupPathsKey = cms.string(''), # not empty => use read paths from AlCaRecoTriggerBitsRcd via this key
#                           andOr = cms.bool(True),              # how to deal with multiple triggers: True (OR) accept if ANY is true, False (AND) accept if ALL are true
#                           throw = cms.bool(True)               # throw exception on unknown path names
#)

process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load('RecoVertex.PrimaryVertexProducer.OfflinePrimaryVertices_cfi')

#primVtx = process.offlinePrimaryVertices
#process.offlinePrimaryVerticesRerun = primVtx.clone( TrackLabel = cms.InputTag("lostTracks"),
#                                                     maxDistanceToBeam = primVtx.vertexCollections[0].maxDistanceToBeam,
#                                                     useBeamConstraint = primVtx.vertexCollections[0].useBeamConstraint,
#                                                     chi2cutoff = primVtx.vertexCollections[0].chi2cutoff,
#                                                     verbose = False,
#                                                     algorithm = primVtx.vertexCollections[0].algorithm,
#                                                     minNdof = primVtx.vertexCollections[0].minNdof,
#                                                     label = primVtx.vertexCollections[0].label
#)

#if options.withBS:
#    process.offlinePrimaryVerticesRerun = primVtx.clone( TrackLabel = cms.InputTag("lostTracks"),
#                                                         maxDistanceToBeam = primVtx.vertexCollections[1].maxDistanceToBeam,
#                                                         useBeamConstraint = primVtx.vertexCollections[1].useBeamConstraint,
#                                                         chi2cutoff = primVtx.vertexCollections[1].chi2cutoff,
#                                                         verbose = False,
#                                                         algorithm = primVtx.vertexCollections[1].algorithm,
#                                                         minNdof = primVtx.vertexCollections[1].minNdof,
#                                                         label = primVtx.vertexCollections[1].label
#    )

#print process.offlinePrimaryVerticesRerun.dumpPython()

process.load('TrackingAnalysis.EDAnalyzers.residuals_cfi')
process.residuals.BeamSpotConfig = ''

process.residuals.AddLostTracks = False 
    
process.residuals.RunOnData = True

process.residuals.stageL1Trigger = cms.uint32(0)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output2016.root"),
                                   closeFileFast = cms.untracked.bool(True)
                                   )

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.p = cms.Path(
#    process.offlinePrimaryVerticesRerun*
    process.residuals
)
