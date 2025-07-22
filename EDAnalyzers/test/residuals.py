import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.register('withBS', False, VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Primary vertex reconstruction with BS constraint')
options.register('isData', False, VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Run on data')
options.register('addLost', True, VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Add lost tracks')
options.register('is2022', False, VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Run on 2022 MC')
options.register('is2023', False, VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Run on 2023 MC')
options.register('is2024', False, VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Run on 2024 MC')
options.parseArguments()

readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()

source = cms.Source("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)

readFiles.extend( [
'/store/data/Run2022A/JetHT/MINIAOD/10Dec2022-v2/40000/f6aefde1-32d8-4f3c-b13c-863c9d8583ab.root'
]);

process = cms.Process("IpResiduals")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet( reportEvery = cms.untracked.int32(1) )

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

if options.isData: process.GlobalTag.globaltag = '130X_dataRun3_v2'
# elif options.is2022: process.GlobalTag.globaltag = '130X_mcRun3_2022_realistic_v5' # jet ht
elif options.is2022: process.GlobalTag.globaltag = '124X_mcRun3_2022_realistic_v12' #min bias
elif options.is2023: process.GlobalTag.globaltag = '126X_mcRun3_2023_forPU65_v4'
elif options.is2024: process.GlobalTag.globaltag = '133X_mcRun3_2024_realistic_v9'

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
if options.withBS:
    process.residuals.BeamSpotConfig = 'WithBS'

process.residuals.AddLostTracks = False
if options.addLost:
    process.residuals.AddLostTracks = True
    
process.residuals.RunOnData = False
if options.isData:
    process.residuals.RunOnData = True

process.residuals.stageL1Trigger = cms.uint32(0)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("output.root"),
                                   closeFileFast = cms.untracked.bool(True)
                                   )

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.p = cms.Path(
#    process.offlinePrimaryVerticesRerun*
    process.residuals
)
