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

	/// Get the configuration used in the VertexProducer
	const edm::ParameterSet &inputConfig() const { return config_; }

private:
	TrackFilterForPVFindingBase *theTrackFilter_;
	TrackClusterizerInZ *theTrackClusterizer_;
	VertexCompatibleWithBeam *vertexSelector_;

	edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> builder_token_;

	double minNdof_;

	edm::ParameterSet config_;
	std::string beamSpotConfig_;
};

#endif
