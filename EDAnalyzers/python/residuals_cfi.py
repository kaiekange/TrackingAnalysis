import FWCore.ParameterSet.Config as cms

residuals = cms.EDAnalyzer("Residuals",

        # Run on data
        RunOnData = cms.bool(False),

        # Beam spot
        BeamSpotLabel = cms.InputTag("offlineBeamSpot"),
        BeamSpotConfig = cms.string(""),

        # Whether to add lost tracks
        AddLostTracks = cms.bool(False),

        # Rho
        RhoLabel = cms.InputTag("fixedGridRhoFastjetAll"),

        # Trigger results
        TriggerResultsLabel = cms.InputTag("TriggerResults", "", "HLT"),

        trackProducer = cms.untracked.InputTag('lostTracks'),

        # https://github.com/cms-sw/cmssw/blob/master/RecoVertex/PrimaryVertexProducer/python/OfflinePrimaryVertices_cfi.py#L12
        TkFilterParameters = cms.PSet(algorithm=cms.string('filter'),
            maxNormalizedChi2 = cms.double(10.0),
            minPixelLayersWithHits = cms.int32(2),
            minSiliconLayersWithHits = cms.int32(5),
            maxD0Significance = cms.double(4.0),
            maxD0Error = cms.double(1.0),
            maxDzError = cms.double(1.0),
            minPt = cms.double(0.0),
            maxEta = cms.double(5.0), # 2.4
            trackQuality = cms.string("any")
            ),                                

        # https://github.com/cms-sw/cmssw/blob/master/RecoVertex/PrimaryVertexProducer/python/TkClusParameters_cff.py#L3
        # TkClusParameters = cms.PSet(algorithm = cms.string("DA_vect"),
        #     TkDAClusParameters = cms.PSet(coolingFactor = cms.double(0.6),  #  moderate annealing speed
        #         Tmin = cms.double(2.0),            #  end of annealing
        #         Tpurge = cms.double(2.0),         # cleaning
        #         Tstop = cms.double(0.5),          # end of annealing
        #         uniquetrkweight = cms.double(0.8), # require at least two tracks with this weight at T=Tpurge
        #         zrange = cms.double(4.),          # consider only clusters within 4 sigma*sqrt(T) of a track
        #         zmerge = cms.double(1e-2),        # merge intermediat clusters separated by less than zmerge
        #         vertexSize = cms.double(0.006),    #  ~ resolution / sqrt(Tmin)
        #         d0CutOff = cms.double(3.),        # downweight high IP tracks 
        #         dzCutOff = cms.double(3.),        # outlier rejection after freeze-out (T<Tmin)
        #         uniquetrkminp = cms.double(0.0),  # minimal a priori track weight for counting unique tracks
        #         convergence_mode = cms.int32(0),  # 0 = two steps, 1 = dynamic with sqrt(T)
        #         delta_highT = cms.double(1.e-2),  # convergence requirement at high T
        #         delta_lowT = cms.double(1.e-3),   # convergence requirement at low T,
        #         runInBlocks = cms.bool(False),    # activate the DA running in blocks of z sorted tracks
        #         )
#     ),

        TkClusParameters = cms.PSet(
                algorithm   = cms.string("DA_vect"),
                TkDAClusParameters = cms.PSet(
                    coolingFactor = cms.double(0.6),  # moderate annealing speed
                    zrange = cms.double(4.),          # consider only clusters within 4 sigma*sqrt(T) of a track
                    delta_highT = cms.double(1.e-2),  # convergence requirement at high T
                    delta_lowT = cms.double(1.e-3),   # convergence requirement at low T
                    convergence_mode = cms.int32(0),  # 0 = two steps, 1 = dynamic with sqrt(T)
                    Tmin = cms.double(2.0),           # end of vertex splitting
                    Tpurge = cms.double(2.0),         # cleaning
                    Tstop = cms.double(0.5),          # end of annealing
                    vertexSize = cms.double(0.006),   # added in quadrature to track-z resolutions
                    d0CutOff = cms.double(3.),        # downweight high IP tracks
                    dzCutOff = cms.double(3.),        # outlier rejection after freeze-out (T<Tmin)
                    zmerge = cms.double(1e-2),        # merge intermediat clusters separated by less than zmerge
                    uniquetrkweight = cms.double(0.8),# require at least two tracks with this weight at T=Tpurge
                    uniquetrkminp = cms.double(0.0),  # minimal a priori track weight for counting unique tracks
                    runInBlocks = cms.bool(False),    # activate the DA running in blocks of z sorted tracks
                    block_size = cms.uint32(10000),   # block size in tracks
                    overlap_frac = cms.double(0.0)    # overlap between consecutive blocks (blocks_size*overlap_frac)
                    )
                ),



        VxFitterParameters = cms.PSet(algorithm=cms.string('AVF'),
                minNdof = cms.double(0.0),
                maxDistanceToBeam = cms.double(1.0)
                ),

        VxFitterBSParameters = cms.PSet(algorithm=cms.string('AVFBS'),
                minNdof = cms.double(2.0),
                maxDistanceToBeam = cms.double(1.0)
                ),

        # Pileup
        puInfoLabel = cms.InputTag("slimmedAddPileupInfo"),

        # Selection of Tracks
        TrackLabel = cms.InputTag("packedPFCandidates"),
        LostTrackLabel = cms.InputTag("lostTracks"),
        TkMinPt = cms.double(0.0),
        TkMinXLayers = cms.int32(7),
        TkMaxMissedOuterLayers = cms.int32(4),
        TkMaxMissedInnerLayers = cms.int32(0),

        # Selection of Vertices
        VertexLabel = cms.InputTag("offlineSlimmedPrimaryVertices"),
        # VertexPrimaryLabel = cms.InputTag("offlinePrimaryVertices"),
        # VertexPrimaryLabel = cms.InputTag("offlinePrimaryVertices","WithBS"),
        VtxTracksSizeMin = cms.int32(2),
        VtxTracksSizeMax = cms.int32(1000),

        # PF jets
        PFJetsLabel = cms.InputTag("slimmedJets"),

        # Vertex selection for Jet6U trigger
        # VtxErrorXMin = cms.double(0.0015),
        # VtxErrorXMax = cms.double(0.0037),
        # VtxErrorYMin = cms.double(0.0015),
        # VtxErrorYMax = cms.double(0.0037),
        # VtxErrorZMin = cms.double(0.0020),
        # VtxErrorZMax = cms.double(0.0036),

        # Event filter
        EventScale = cms.int32(100),
        # TrackScale = cms.int32(100),
        TrackScale = cms.int32(1),

        # Vertex selection for MinBias trigger
        ##VtxErrorXMin = cms.double(0.0020),
        ##VtxErrorXMax = cms.double(0.0055),
        ##VtxErrorYMin = cms.double(0.0020),
        ##VtxErrorYMax = cms.double(0.0055),
        ##VtxErrorZMin = cms.double(0.0025),
        ##VtxErrorZMax = cms.double(0.0060),
)
