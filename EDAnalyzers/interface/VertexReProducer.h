#ifndef TrackingAnalysis_EDAnalyzers_interface_VertexReProducer_h
#define TrackingAnalysis_EDAnalyzers_interface_VertexReProducer_h

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/PrimaryVertexProducer/interface/PrimaryVertexProducerAlgorithm.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

class VertexReProducer 
{
   
 public:
   /// This is the real constructor to be used
   VertexReProducer(const edm::ParameterSet &config, edm::ConsumesCollector& iC);
   
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

   //edm::ESHandle<TransientTrackBuilder> theB;
   //edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> track_builder_token_;
 
   //edm::ESHandle<TransientTrackBuilder> theB;
   edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> builder_token_;


   double minNdof_;
   
   edm::ParameterSet config_;
   std::string beamSpotConfig_;
};

#endif
