#include "TrackingAnalysis/EDAnalyzers/interface/VertexReProducer.h"

#include "RecoVertex/VertexPrimitives/interface/VertexState.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include <algorithm>

VertexReProducer::VertexReProducer(const edm::ParameterSet &iConfig, edm::ConsumesCollector &iC)
    : builder_token_(iC.esConsumes<TransientTrackBuilder, TransientTrackRecord>(edm::ESInputTag("", "TransientTrackBuilder")))
{
    // only the offline PV setup (track filter + DA_vect clustering) is implemented
    std::string trackSelectionAlgorithm = iConfig.getParameter<edm::ParameterSet>("TkFilterParameters").getParameter<std::string>("algorithm");
    if (trackSelectionAlgorithm != "filter")
        throw cms::Exception("Configuration") << "VertexReProducer: unsupported TkFilterParameters.algorithm " << trackSelectionAlgorithm;
    std::string clusteringAlgorithm = iConfig.getParameter<edm::ParameterSet>("TkClusParameters").getParameter<std::string>("algorithm");
    if (clusteringAlgorithm != "DA_vect")
        throw cms::Exception("Configuration") << "VertexReProducer: unsupported TkClusParameters.algorithm " << clusteringAlgorithm;

    theTrackFilter_ = new TrackFilterForPVFinding(iConfig.getParameter<edm::ParameterSet>("TkFilterParameters"));
    theTrackClusterizer_ = new DAClusterizerInZ_vect(iConfig.getParameter<edm::ParameterSet>("TkClusParameters").getParameter<edm::ParameterSet>("TkDAClusParameters"));

    vertexSelector_ = new VertexCompatibleWithBeam(VertexDistanceXY(), iConfig.getParameter<edm::ParameterSet>("VxFitterParameters").getParameter<double>("maxDistanceToBeam"));

    minNdof_ = iConfig.getParameter<edm::ParameterSet>("VxFitterParameters").getParameter<double>("minNdof");
}

std::vector<TransientVertex> VertexReProducer::makeVertices(const reco::TrackCollection &tracks, const reco::BeamSpot &bs, const edm::EventSetup &iSetup) const
{
    edm::ESHandle<TransientTrackBuilder> theB = iSetup.getHandle(builder_token_);

    std::vector<reco::TransientTrack> t_tks;
    t_tks.reserve(tracks.size());

    for (size_t i = 0; i < tracks.size(); i++)
    {
        const reco::Track &pseudoTrack = tracks[i];

        reco::TransientTrack transTrack = (*theB).build(pseudoTrack);
        t_tks.push_back(transTrack);
        t_tks.back().setBeamSpot(bs);
    }

    return makeVertices(t_tks, bs);
}

std::vector<TransientVertex> VertexReProducer::makeVertices(const std::vector<reco::TransientTrack> &t_tks, const reco::BeamSpot &bs) const
{
    bool validBS = true;
    VertexState beamVertexState(bs);
    if ((beamVertexState.error().cxx() <= 0.) ||
        (beamVertexState.error().cyy() <= 0.) ||
        (beamVertexState.error().czz() <= 0.))
    {
        validBS = false;
        edm::LogError("UnusableBeamSpot") << "Beamspot with invalid errors " << beamVertexState.error().matrix();
    }

    std::vector<reco::TransientTrack> seltks = theTrackFilter_->select(t_tks);

    std::vector<std::vector<reco::TransientTrack>> clusters = theTrackClusterizer_->clusterize(seltks);

    AdaptiveVertexFitter fit;

    std::vector<TransientVertex> pvs;

    for (std::vector<std::vector<reco::TransientTrack>>::const_iterator iclus = clusters.begin(); iclus != clusters.end(); iclus++)
    {
        TransientVertex v;

        if ((*iclus).size() > 1)
            v = fit.vertex(*iclus);

        if (v.isValid() && (v.degreesOfFreedom() >= minNdof_) && (!validBS || (*(vertexSelector_))(v, beamVertexState)))
            pvs.push_back(v);
    }

    if (pvs.size() > 1)
        sort(pvs.begin(), pvs.end(), VertexHigherPtSquared());

    return pvs;
}

VertexReProducer::~VertexReProducer()
{
    delete theTrackFilter_;
    delete theTrackClusterizer_;
    delete vertexSelector_;
}
