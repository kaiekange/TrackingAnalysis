#ifndef TrackingAnalysis_EDAnalyzers_interface_VertexReProducer_h
#define TrackingAnalysis_EDAnalyzers_interface_VertexReProducer_h

#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/PrimaryVertexProducer/interface/TrackFilterForPVFinding.h"
#include "RecoVertex/PrimaryVertexProducer/interface/DAClusterizerInZ_vect.h"
#include "RecoVertex/VertexTools/interface/VertexCompatibleWithBeam.h"
#include "RecoVertex/AdaptiveVertexFit/interface/AdaptiveVertexFitter.h"
#include "RecoVertex/PrimaryVertexProducer/interface/VertexHigherPtSquared.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

class VertexReProducer
{

public:
	/// This is the real constructor to be used
	VertexReProducer(const edm::ParameterSet &config, edm::ConsumesCollector &iC);

	~VertexReProducer();

	/// Make the vertices
	std::vector<TransientVertex> makeVertices(const reco::TrackCollection &tracks,
											  const reco::BeamSpot &bs,
											  const edm::EventSetup &iSetup) const;

	/// Make the vertices from already built transient tracks (beam spot set on each track)
	std::vector<TransientVertex> makeVertices(const std::vector<reco::TransientTrack> &tracks,
											  const reco::BeamSpot &bs) const;

private:
	TrackFilterForPVFindingBase *theTrackFilter_ = nullptr;
	TrackClusterizerInZ *theTrackClusterizer_ = nullptr;
	VertexCompatibleWithBeam *vertexSelector_ = nullptr;

	edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> builder_token_;

	double minNdof_;
};

#endif
