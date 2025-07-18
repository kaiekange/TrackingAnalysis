// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "MagneticField/Engine/interface/MagneticField.h" 
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h" 

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/RefToPtr.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

//#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
//#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
//#include "SimTracker/TrackAssociation/interface/TrackingParticleIP.h"
//#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
//#include "SimDataFormats/Associations/interface/TrackToTrackingParticleAssociator.h"
//#include "SimTracker/TrackAssociation/plugins/ParametersDefinerForTPESProducer.h"
//#include "SimTracker/TrackAssociation/interface/ParametersDefinerForTP.h"
//#include "SimDataFormats/Associations/interface/VertexToTrackingVertexAssociator.h"
//#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
//#include "SimTracker/TrackHistory/interface/VertexClassifier.h"
//#include "SimTracker/TrackHistory/interface/TrackClassifier.h"

#include "DataFormats/PatCandidates/interface/Jet.h"

#include <TrackingTools/TrajectoryState/interface/PerigeeConversions.h>
#include <TrackingTools/TrajectoryState/interface/TrajectoryStateClosestToPoint.h>
#include <TrackingTools/PatternTools/interface/TSCPBuilderNoMaterial.h>

#include "TrackingTools/IPTools/interface/IPTools.h"

#include "TrackingAnalysis/EDAnalyzers/interface/VertexReProducer.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include <CommonTools/UtilAlgos/interface/TFileService.h>

#include "TROOT.h"
#include "TH1F.h"
#include "TTree.h"
#include "TRandom3.h"
#include "Compression.h"

#include "TrackingAnalysis/EDAnalyzers/interface/Tree.h"

namespace
{
    bool sortPt(const reco::TransientTrack & t1,
            const reco::TransientTrack & t2) 
    {
        return t1.track().pt() > t2.track().pt();
    }
}

class Residuals : public edm::one::EDAnalyzer<> 
{  

    public:
        explicit Residuals(const edm::ParameterSet& pset);
        ~Residuals();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void beginRun(const edm::Run&, const edm::EventSetup&);
        virtual void analyze(const edm::Event&, const edm::EventSetup&);
        virtual void endRun();

        edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> theMFToken_;

        bool trackSelection(const reco::Track& track) const;
        bool vertexSelection(const reco::Vertex& vertex) const;

        float getDeltaR(float eta1, float phi1, float eta2, float phi2);

        class TrackEqual 
        {	
            public:

                TrackEqual( const edm::Ptr<reco::Track> & t ) : track_( t ) {}

                bool operator()( const edm::Ptr<reco::Track> & t ) const {
                    return t->pt()==track_->pt();
                }

            private:

                const edm::Ptr<reco::Track> & track_;
        };

        class TrackEqualReco
        {	
            public:

                TrackEqualReco( const reco::Track & t ) : track_( t ) {}

                bool operator()( const reco::Track & t ) const {
                    return t.pt()==track_.pt();
                }

            private:

                const reco::Track & track_;
        };

        class TrackEqualRef
        {	
            public:
                TrackEqualRef( const reco::TrackRef & t) : track_( t ) {}

                bool operator()( const reco::TrackRef & t ) const 
                {
                    return t->pt()==track_->pt();
                }

            private:
                const reco::TrackRef & track_;
        };

        class VertexEqual
        {	
            public:
                VertexEqual( const reco::Vertex::Point & p) : p_( p ) {}

                bool operator()( const reco::Vertex::Point & p ) const 
                {
                    return (p.x()==p_.x() && p.y()==p_.y() && p.z()==p_.z());
                }

            private:
                const reco::Vertex::Point & p_;
        };

        // ----------member data ---------------------------
        edm::EDGetTokenT<reco::VertexCollection> thePVToken_;
        //   edm::EDGetTokenT<reco::TrackCollection> theTracksToken_;
        edm::EDGetTokenT<edm::View<pat::PackedCandidate> > theTracksToken_;
        edm::EDGetTokenT<edm::View<pat::PackedCandidate> > theLostTracksToken_;
        edm::EDGetTokenT<edm::View<reco::Track> > theTrackViewsToken_;
        edm::EDGetTokenT<reco::BeamSpot> theBeamspotToken_;
        edm::EDGetTokenT<double> theRhoToken_;
        edm::EDGetTokenT< vector<pat::Jet> > thePFJetsToken_;
        edm::EDGetTokenT<edm::TriggerResults> theTriggerBitsToken_;
        edm::EDGetTokenT<std::vector<PileupSummaryInfo> > puInfoToken_;

        // --- track selection variables
        double tkMinPt;
        int tkMinXLayers,tkMaxMissedOuterLayers,tkMaxMissedInnerLayers;

        // --- vertex selection variables
        unsigned int vtxTracksSizeMin;  
        unsigned int vtxTracksSizeMax;  
        //   double vtxErrorXMin,vtxErrorXMax;
        //   double vtxErrorYMin,vtxErrorYMax;
        //   double vtxErrorZMin,vtxErrorZMax;

        bool addLostTracks;

        std::string beamSpotConfig;   

        VertexReProducer *revertex;

        bool runOnData;

        int eventScale;
        int trackScale;

        TRandom3 *rnd;

        HLTConfigProvider hltConfig_;
        HLTPrescaleProvider hltPrescale_;

        const edm::Service<TFileService> fs;
        ResTree* ftree;

        int ncount;
};

Residuals::Residuals(const edm::ParameterSet& pset):
    theMFToken_(esConsumes<MagneticField, IdealMagneticFieldRecord>()),
    hltPrescale_(pset, consumesCollector(), *this)
{
    edm::InputTag TrackCollectionTag_ = pset.getParameter<edm::InputTag>("TrackLabel");
    theTracksToken_= consumes<edm::View<pat::PackedCandidate> >(TrackCollectionTag_);

    edm::InputTag LostTrackCollectionTag_ = pset.getParameter<edm::InputTag>("LostTrackLabel");
    theLostTracksToken_= consumes<edm::View<pat::PackedCandidate> >(LostTrackCollectionTag_);

    /*
       theTrackViewsToken_= consumes<edm::View<reco::Track> >(TrackCollectionTag_);
       */   
    edm::InputTag VertexCollectionTag_ = pset.getParameter<edm::InputTag>("VertexLabel");
    thePVToken_ = consumes<reco::VertexCollection>(VertexCollectionTag_);

    edm::InputTag BeamspotTag_ = edm::InputTag("offlineBeamSpot");
    theBeamspotToken_ = consumes<reco::BeamSpot>(BeamspotTag_);

    edm::InputTag RhoTag_ = pset.getParameter<edm::InputTag>("RhoLabel");
    theRhoToken_ = consumes<double>(RhoTag_);

    edm::InputTag PFJetsTag_ = pset.getParameter<edm::InputTag>("PFJetsLabel");
    thePFJetsToken_ = consumes< vector<pat::Jet> >(PFJetsTag_);

    edm::InputTag TriggerBitsTag_ = pset.getParameter<edm::InputTag>("TriggerResultsLabel");
    theTriggerBitsToken_ = consumes<edm::TriggerResults>(TriggerBitsTag_);

    edm::InputTag PUInfoTag_ = pset.getParameter<edm::InputTag>("puInfoLabel");
    puInfoToken_ = consumes<std::vector<PileupSummaryInfo> >(PUInfoTag_);

    beamSpotConfig = pset.getParameter<std::string>("BeamSpotConfig");

    addLostTracks = pset.getParameter<bool>("AddLostTracks");

    tkMinPt = pset.getParameter<double>("TkMinPt");
    tkMinXLayers = pset.getParameter<int>("TkMinXLayers");
    tkMaxMissedOuterLayers = pset.getParameter<int>("TkMaxMissedOuterLayers");
    tkMaxMissedInnerLayers = pset.getParameter<int>("TkMaxMissedInnerLayers");

    vtxTracksSizeMin = pset.getParameter<int>("VtxTracksSizeMin");
    vtxTracksSizeMax = pset.getParameter<int>("VtxTracksSizeMax");
    //   vtxErrorXMin     = pset.getParameter<double>("VtxErrorXMin");
    //   vtxErrorXMax     = pset.getParameter<double>("VtxErrorXMax");
    //   vtxErrorYMin     = pset.getParameter<double>("VtxErrorYMin");
    //   vtxErrorYMax     = pset.getParameter<double>("VtxErrorYMax");
    //   vtxErrorZMin     = pset.getParameter<double>("VtxErrorZMin");
    //   vtxErrorZMax     = pset.getParameter<double>("VtxErrorZMax");

    edm::ConsumesCollector c{consumesCollector()};
    revertex = new VertexReProducer(pset, c);

    eventScale = pset.getParameter<int>("EventScale");
    trackScale = pset.getParameter<int>("TrackScale");

    runOnData = pset.getParameter<bool>("RunOnData");

    rnd = new TRandom3();

    TFile& f = fs->file();
    f.SetCompressionAlgorithm(ROOT::kZLIB);
    f.SetCompressionLevel(9);
    ftree = new ResTree(fs->make<TTree>("tree", "tree"));
    ftree->CreateBranches(32000, runOnData);

    ncount = 0;
}

Residuals::~Residuals()
{
    if( revertex ) delete revertex;
    delete rnd;
}

void Residuals::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    ncount++;
    if( (ncount-1) % eventScale != 0 && eventScale > 0 ) return;

    using namespace edm;
    using namespace reco;
    using namespace std;

    ftree->Init();

    Handle<edm::View<pat::PackedCandidate> > tracksPackedHandle;
    iEvent.getByToken(theTracksToken_, tracksPackedHandle);

    Handle<edm::View<pat::PackedCandidate> > lostTracksPackedHandle;
    iEvent.getByToken(theLostTracksToken_, lostTracksPackedHandle);

    // Create pseudo-track collection
    edm::View<pat::PackedCandidate> tracksPacked = (*tracksPackedHandle.product());
    edm::View<pat::PackedCandidate> lostTracksPacked = (*lostTracksPackedHandle.product());

    TrackCollection tracks;

    for( size_t it=0;it<tracksPacked.size();it++ )
    {
        const pat::PackedCandidate &trkPacked = tracksPacked[it];

        if( ! trkPacked.hasTrackDetails() ) continue;

        reco::Track trk = trkPacked.pseudoTrack();
        tracks.push_back(trk);
    }

    if( addLostTracks )
    {	
        for( size_t it=0;it<lostTracksPacked.size();it++ )
        {
            const pat::PackedCandidate &trkPacked = lostTracksPacked[it];

            if( ! trkPacked.hasTrackDetails() ) continue;

            reco::Track trk = trkPacked.pseudoTrack();
            tracks.push_back(trk);
        }
    }   

    /*   
         Handle<View<Track> >  trackViews;
         iEvent.getByToken(theTrackViewsToken_, trackViews);
         */   
    Handle<VertexCollection> vtxH;
    iEvent.getByToken(thePVToken_, vtxH);

    Handle<BeamSpot> pvbeamspot;
    iEvent.getByToken(theBeamspotToken_, pvbeamspot);

    if( !vtxH.isValid() ) return;

    if( vtxH->size() == 0 ) return;

    Handle< vector<pat::Jet> > pfJets;
    iEvent.getByToken(thePFJetsToken_, pfJets);

    // refit primary vertices and put it in a new handle (note: TrackBaseRefs are different)
    vector<TransientVertex> pvs = revertex->makeVertices(tracks, *pvbeamspot, iSetup);
    std::cout << "Primary vertices = " << vtxH->size() << ", refitted vertices = " << pvs.size() << ", tracks = " << tracks.size() << std::endl;
    if( pvs.empty() ) return;

    reco::VertexCollection pvr;
    for(unsigned int ipv=0;ipv<pvs.size();ipv++)
        pvr.push_back(reco::Vertex(pvs[ipv]));

    edm::Handle<reco::VertexCollection> pvrh = edm::Handle(const_cast<reco::VertexCollection*>(&pvr), vtxH.provenance());

    reco::Vertex vtx = reco::Vertex(pvs.front());
    TransientVertex vtxTrans = pvs.front();

    if( !vertexSelection(vtx) ) return;

    //ESHandle<MagneticField> theMF;

    //iSetup.get<IdealMagneticFieldRecord>().get(theMF);

    //edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> theMF;

    edm::ESHandle<MagneticField> theMF = iSetup.getHandle(theMFToken_);

    Handle<double> rhoPtr;
    iEvent.getByToken(theRhoToken_, rhoPtr);

    // General event info
    ftree->ev_run = iEvent.id().run();
    ftree->ev_id = iEvent.id().event();
    ftree->ev_lumi = iEvent.id().luminosityBlock();
    ftree->ev_bunchCrossing = iEvent.bunchCrossing();
    ftree->ev_orbitNumber = iEvent.orbitNumber();
    ftree->ev_time = iEvent.time().unixTime();
    ftree->ev_rho = *rhoPtr;
    ftree->ev_nPV = pvr.size();

    // Trigger
    Handle<TriggerResults> triggerBits;
    iEvent.getByToken(theTriggerBitsToken_, triggerBits);
    const TriggerNames &names = iEvent.triggerNames(*triggerBits);
    for( unsigned int i=0;i<names.size();i++ )
    {
        TString trigName = TString(names.triggerName(i));

        //	std::cout << i << " " << trigName << std::endl;

        bool pass = (triggerBits->accept(i) ? true : false);

        if( trigName.Contains("HLT_ZeroBias_v") ) ftree->trig_ZeroBias_pass = pass;

        else if( trigName.Contains("HLT_ZeroBias_part0_v") ) ftree->trig_ZeroBias_part0_pass = pass;
        else if( trigName.Contains("HLT_ZeroBias_part1_v") ) ftree->trig_ZeroBias_part1_pass = pass;
        else if( trigName.Contains("HLT_ZeroBias_part2_v") ) ftree->trig_ZeroBias_part2_pass = pass;
        else if( trigName.Contains("HLT_ZeroBias_part3_v") ) ftree->trig_ZeroBias_part3_pass = pass;
        else if( trigName.Contains("HLT_ZeroBias_part4_v") ) ftree->trig_ZeroBias_part4_pass = pass;
        else if( trigName.Contains("HLT_ZeroBias_part5_v") ) ftree->trig_ZeroBias_part5_pass = pass;
        else if( trigName.Contains("HLT_ZeroBias_part6_v") ) ftree->trig_ZeroBias_part6_pass = pass;
        else if( trigName.Contains("HLT_ZeroBias_part7_v") ) ftree->trig_ZeroBias_part7_pass = pass;

        else if( trigName.Contains("HLT_PFJet40_v") ) ftree->trig_PFJet40_pass = pass;
        else if( trigName.Contains("HLT_PFJet60_v") ) ftree->trig_PFJet60_pass = pass;
        else if( trigName.Contains("HLT_PFJet80_v") ) ftree->trig_PFJet80_pass = pass;
        else if( trigName.Contains("HLT_PFJet140_v") ) ftree->trig_PFJet140_pass = pass;
        else if( trigName.Contains("HLT_PFJet200_v") ) ftree->trig_PFJet200_pass = pass;
        else if( trigName.Contains("HLT_PFJet260_v") ) ftree->trig_PFJet260_pass = pass;
        else if( trigName.Contains("HLT_PFJet320_v") ) ftree->trig_PFJet320_pass = pass;
        else if( trigName.Contains("HLT_PFJet400_v") ) ftree->trig_PFJet400_pass = pass;
        else if( trigName.Contains("HLT_PFJet450_v") ) ftree->trig_PFJet450_pass = pass;
        else if( trigName.Contains("HLT_PFJet500_v") ) ftree->trig_PFJet500_pass = pass;
        else if( trigName.Contains("HLT_PFJet550_v") ) ftree->trig_PFJet550_pass = pass;

        else if( trigName.Contains("HLT_AK4PFJet30_v") ) ftree->trig_AK4PFJet30_pass = pass;
        else if( trigName.Contains("HLT_AK4PFJet50_v") ) ftree->trig_AK4PFJet50_pass = pass;
        else if( trigName.Contains("HLT_AK4PFJet80_v") ) ftree->trig_AK4PFJet80_pass = pass;
        else if( trigName.Contains("HLT_AK4PFJet100_v") ) ftree->trig_AK4PFJet100_pass = pass;
        else if( trigName.Contains("HLT_AK4PFJet120_v") ) ftree->trig_AK4PFJet120_pass = pass;

        else if( trigName.Contains("HLT_PFHT180_v") ) ftree->trig_PFHT180_pass = pass;
        else if( trigName.Contains("HLT_PFHT250_v") ) ftree->trig_PFHT250_pass = pass;	
        else if( trigName.Contains("HLT_PFHT370_v") ) ftree->trig_PFHT370_pass = pass;
        else if( trigName.Contains("HLT_PFHT430_v") ) ftree->trig_PFHT430_pass = pass;
        else if( trigName.Contains("HLT_PFHT510_v") ) ftree->trig_PFHT510_pass = pass;
        else if( trigName.Contains("HLT_PFHT590_v") ) ftree->trig_PFHT590_pass = pass;
        else if( trigName.Contains("HLT_PFHT680_v") ) ftree->trig_PFHT680_pass = pass;
        else if( trigName.Contains("HLT_PFHT780_v") ) ftree->trig_PFHT780_pass = pass;
        else if( trigName.Contains("HLT_PFHT890_v") ) ftree->trig_PFHT890_pass = pass;
        else if( trigName.Contains("HLT_PFHT1050_v") ) ftree->trig_PFHT1050_pass = pass;
        else if( trigName.Contains("HLT_PFHT350_v") ) ftree->trig_PFHT350_pass = pass;

        //	     std::cout << i << " " << trigName << " L1=" << L1prescale << " HLT=" << HLTprescale << std::endl;
    }

    // Pileup info
    if( !runOnData )
    {	
        edm::Handle<std::vector< PileupSummaryInfo> > pileupInfo;
        iEvent.getByToken(puInfoToken_,pileupInfo);

        ftree->mc_pu_Npvi = pileupInfo->size();
        for(std::vector<PileupSummaryInfo>::const_iterator pvi=pileupInfo->begin();pvi!=pileupInfo->end();pvi++)
        {
            signed int n_bc = pvi->getBunchCrossing();
            ftree->mc_pu_BunchCrossing.push_back(n_bc);
            if( n_bc == 0 )
            {		  
                ftree->mc_pu_intime_NumInt = pvi->getPU_NumInteractions();
                ftree->mc_pu_trueNumInt = pvi->getTrueNumInteractions();
            }	     
            else if( n_bc == -1 ) ftree->mc_pu_before_npu = pvi->getPU_NumInteractions();
            else if( n_bc == +1 ) ftree->mc_pu_after_npu  = pvi->getPU_NumInteractions();

            std::vector<float> mc_pu_zpositions;
            std::vector<float> mc_pu_sumpT_lowpT;
            std::vector<float> mc_pu_sumpT_highpT;
            std::vector<int> mc_pu_ntrks_lowpT;
            std::vector<int> mc_pu_ntrks_highpT;

            ftree->mc_pu_Nzpositions.push_back(pvi->getPU_zpositions().size());
            for( unsigned int ipu=0;ipu<pvi->getPU_zpositions().size();ipu++ )
            {		  
                mc_pu_zpositions.push_back((pvi->getPU_zpositions())[ipu]);
                mc_pu_sumpT_lowpT.push_back((pvi->getPU_sumpT_lowpT())[ipu]);
                mc_pu_sumpT_highpT.push_back((pvi->getPU_sumpT_highpT())[ipu]);
                mc_pu_ntrks_lowpT.push_back((pvi->getPU_ntrks_lowpT())[ipu]);
                mc_pu_ntrks_highpT.push_back((pvi->getPU_ntrks_highpT())[ipu]);
            }	     

            ftree->mc_pu_zpositions.push_back(mc_pu_zpositions);
            ftree->mc_pu_sumpT_lowpT.push_back(mc_pu_sumpT_lowpT);
            ftree->mc_pu_sumpT_highpT.push_back(mc_pu_sumpT_highpT);
            ftree->mc_pu_ntrks_lowpT.push_back(mc_pu_ntrks_lowpT);
            ftree->mc_pu_ntrks_highpT.push_back(mc_pu_ntrks_highpT);
        }	
    }   

    double micron = 10000;

    // Beam spot   
    ftree->bs_type = pvbeamspot->type();
    ftree->bs_x0 = pvbeamspot->x0();
    ftree->bs_y0 = pvbeamspot->y0();
    ftree->bs_z0 = pvbeamspot->z0();
    ftree->bs_x_zpv = pvbeamspot->x(vtx.z());
    ftree->bs_y_zpv = pvbeamspot->y(vtx.z());
    ftree->bs_sigmaZ = pvbeamspot->sigmaZ();
    ftree->bs_dxdz = pvbeamspot->dxdz();
    ftree->bs_dydz = pvbeamspot->dydz();
    ftree->bs_BeamWidthX = pvbeamspot->BeamWidthX();
    ftree->bs_BeamWidthY = pvbeamspot->BeamWidthY();
    ftree->bs_x0Error = pvbeamspot->x0Error();
    ftree->bs_y0Error = pvbeamspot->y0Error();
    ftree->bs_z0Error = pvbeamspot->z0Error();
    ftree->bs_sigmaZ0Error = pvbeamspot->sigmaZ0Error();
    ftree->bs_dxdzError = pvbeamspot->dxdzError();
    ftree->bs_dydzError = pvbeamspot->dydzError();
    ftree->bs_BeamWidthXError = pvbeamspot->BeamWidthXError();
    ftree->bs_BeamWidthYError = pvbeamspot->BeamWidthYError();
    ftree->bs_emittanceX = pvbeamspot->emittanceX();
    ftree->bs_emittanceY = pvbeamspot->emittanceY();
    ftree->bs_betaStar = pvbeamspot->betaStar();

    // Primary vertex   
    for( unsigned int ipv=0;ipv<pvr.size();ipv++ )
    {
        std::vector<reco::TransientTrack> vtxTracks = pvs[ipv].originalTracks();
        stable_sort(vtxTracks.begin(), vtxTracks.end(), sortPt);

        int nTracks = pvr[ipv].tracksSize();

        Track::Point vtxPosition = Track::Point(pvr[ipv].position().x(),
                pvr[ipv].position().y(),
                pvr[ipv].position().z());	

        float pv_SumTrackPt = 0.;
        float pv_SumTrackPt2 = 0.;
        float pv_fracHighPurity = 0.;

        std::vector<float> pv_trackWeight;
        std::vector<bool> pv_trk_isHighPurity;
        std::vector<int> pv_trk_algo;
        std::vector<int> pv_trk_originalAlgo;

        std::vector<int> pv_trk_idx;

        std::vector<int> pv_trk_pvN;
        std::vector<int> pv_trk_pv1N;
        std::vector<int> pv_trk_pv2N;

        std::vector<bool> pv_trk_pvunbiased_IsValid;
        std::vector<bool> pv_trk_pvunbiased_IsFake;
        std::vector<int> pv_trk_pvunbiased_NTracks;
        std::vector<float> pv_trk_pvunbiased_SumTrackPt;
        std::vector<float> pv_trk_pvunbiased_SumTrackPt2;
        std::vector<float> pv_trk_pvunbiased_fracHighPurity;
        std::vector<float> pv_trk_pvunbiased_chi2;
        std::vector<int> pv_trk_pvunbiased_ndof;
        std::vector<float> pv_trk_pvunbiased_x;
        std::vector<float> pv_trk_pvunbiased_y;
        std::vector<float> pv_trk_pvunbiased_z;
        std::vector<float> pv_trk_pvunbiased_xError;
        std::vector<float> pv_trk_pvunbiased_yError;
        std::vector<float> pv_trk_pvunbiased_zError;

        std::vector<float> pv_trk_d0_pvunbiased;
        std::vector<float> pv_trk_dz_pvunbiased;
        std::vector<float> pv_trk_d0_bs_zpvunbiased;

        std::vector<bool> pv_trk_pvunbiased_IsValid_p1;
        std::vector<bool> pv_trk_pvunbiased_IsFake_p1;
        std::vector<int> pv_trk_pvunbiased_NTracks_p1;
        std::vector<float> pv_trk_pvunbiased_SumTrackPt_p1;
        std::vector<float> pv_trk_pvunbiased_SumTrackPt2_p1;
        std::vector<float> pv_trk_pvunbiased_fracHighPurity_p1;
        std::vector<float> pv_trk_pvunbiased_chi2_p1;
        std::vector<int> pv_trk_pvunbiased_ndof_p1;
        std::vector<float> pv_trk_pvunbiased_x_p1;
        std::vector<float> pv_trk_pvunbiased_y_p1;
        std::vector<float> pv_trk_pvunbiased_z_p1;
        std::vector<float> pv_trk_pvunbiased_xError_p1;
        std::vector<float> pv_trk_pvunbiased_yError_p1;
        std::vector<float> pv_trk_pvunbiased_zError_p1;

        std::vector<float> pv_trk_d0_pvunbiased_p1;
        std::vector<float> pv_trk_dz_pvunbiased_p1;
        std::vector<float> pv_trk_d0_bs_zpvunbiased_p1;

        std::vector<bool> pv_trk_pvunbiased_IsValid_p2;
        std::vector<bool> pv_trk_pvunbiased_IsFake_p2;
        std::vector<int> pv_trk_pvunbiased_NTracks_p2;
        std::vector<float> pv_trk_pvunbiased_SumTrackPt_p2;
        std::vector<float> pv_trk_pvunbiased_SumTrackPt2_p2;
        std::vector<float> pv_trk_pvunbiased_fracHighPurity_p2;
        std::vector<float> pv_trk_pvunbiased_chi2_p2;
        std::vector<int> pv_trk_pvunbiased_ndof_p2;
        std::vector<float> pv_trk_pvunbiased_x_p2;
        std::vector<float> pv_trk_pvunbiased_y_p2;
        std::vector<float> pv_trk_pvunbiased_z_p2;
        std::vector<float> pv_trk_pvunbiased_xError_p2;
        std::vector<float> pv_trk_pvunbiased_yError_p2;
        std::vector<float> pv_trk_pvunbiased_zError_p2;

        std::vector<float> pv_trk_d0_pvunbiased_p2;
        std::vector<float> pv_trk_dz_pvunbiased_p2;
        std::vector<float> pv_trk_d0_bs_zpvunbiased_p2;

        std::vector<float> pv_trk_pt;
        std::vector<float> pv_trk_px;
        std::vector<float> pv_trk_py;
        std::vector<float> pv_trk_pz;
        std::vector<float> pv_trk_p;
        std::vector<float> pv_trk_eta;
        std::vector<float> pv_trk_phi;

        std::vector<int> pv_trk_nTrackerLayers;
        std::vector<int> pv_trk_nPixelBarrelLayers;
        std::vector<int> pv_trk_nPixelEndcapLayers;
        std::vector<int> pv_trk_nStripLayers;

        std::vector<int> pv_trk_nValid;
        std::vector<float> pv_trk_fValid;
        std::vector<int> pv_trk_nValidTracker;
        std::vector<int> pv_trk_nValidPixelBarrel;
        std::vector<int> pv_trk_nValidPixelEndcap;
        std::vector<int> pv_trk_nValidStrip;

        std::vector<int> pv_trk_nMissed;
        std::vector<int> pv_trk_nMissedOut;
        std::vector<int> pv_trk_nMissedIn;
        std::vector<int> pv_trk_nMissedTrackerOut;
        std::vector<int> pv_trk_nMissedTrackerIn;
        std::vector<int> pv_trk_nMissedPixelBarrelOut;
        std::vector<int> pv_trk_nMissedPixelBarrelIn;
        std::vector<int> pv_trk_nMissedPixelEndcapOut;
        std::vector<int> pv_trk_nMissedPixelEndcapIn;

        std::vector<bool> pv_trk_hasPixelBarrelLayer1;
        std::vector<bool> pv_trk_hasPixelEndcapLayer1;
        std::vector<bool> pv_trk_hasPixelBarrelLayer2;
        std::vector<bool> pv_trk_hasPixelEndcapLayer2;
        std::vector<bool> pv_trk_hasPixelBarrelLayer3;
        std::vector<bool> pv_trk_hasPixelEndcapLayer3;
        std::vector<bool> pv_trk_hasPixelBarrelLayer4;
        std::vector<bool> pv_trk_hasPixelEndcapLayer4;

        std::vector<int> pv_trk_quality;
        std::vector<float> pv_trk_normalizedChi2;
        std::vector<int> pv_trk_ndof;
        std::vector<int> pv_trk_charge;
        std::vector<float> pv_trk_qoverp;
        std::vector<float> pv_trk_qoverpError;
        std::vector<float> pv_trk_theta;
        std::vector<float> pv_trk_thetaError;
        std::vector<float> pv_trk_lambda;
        std::vector<float> pv_trk_lambdaError;
        std::vector<float> pv_trk_ptError;
        std::vector<float> pv_trk_etaError;
        std::vector<float> pv_trk_phiError;

        std::vector<float> pv_trk_d0;
        std::vector<float> pv_trk_dz;
        std::vector<float> pv_trk_d0_pv;
        std::vector<float> pv_trk_dz_pv;
        std::vector<float> pv_trk_d0_bs;
        std::vector<float> pv_trk_d0_bs_zpca;
        std::vector<float> pv_trk_d0_bs_zpv;
        std::vector<float> pv_trk_dz_bs;
        std::vector<float> pv_trk_d0Err;
        std::vector<float> pv_trk_dzErr;


        TrackCollection initPVTkCollection;
        for( std::vector<reco::TransientTrack>::const_iterator it = vtxTracks.begin(); it != vtxTracks.end(); it++ )
        {
            reco::Track trk = (*it).track();
            initPVTkCollection.push_back(trk);
        }	

        int iTrk = 0;
        for( std::vector<reco::TransientTrack>::const_iterator it = vtxTracks.begin(); it != vtxTracks.end(); it++ )
        {	     
            reco::Track trk = (*it).track();

            // Since TrackBaseRefs are not preserved after vertex refitting, do a pt-based track matching
            TrackCollection::const_iterator itt = find_if(tracks.begin(), tracks.end(), TrackEqualReco(trk));

            if( itt != tracks.end() ) pv_trk_idx.push_back( itt - tracks.begin() );
            else pv_trk_idx.push_back( -1 );

            // Remove the track from the PV track collection
            TrackCollection newPVTkCollection;
            newPVTkCollection.assign(initPVTkCollection.begin(), initPVTkCollection.begin()+iTrk);
            newPVTkCollection.insert(newPVTkCollection.end(), initPVTkCollection.begin()+iTrk+1, initPVTkCollection.end());

            // Split vertex tracks in two groups (unbiased)
            reco::TrackCollection vtxTkCollection1;
            reco::TrackCollection vtxTkCollection2;

            float SumTrackPt_p1 = 0;
            float SumTrackPt2_p1 = 0;

            float SumTrackPt_p2 = 0;
            float SumTrackPt2_p2 = 0;

            float pv_fracHighPurity_p1 = 0;
            float pv_fracHighPurity_p2 = 0;

            for( std::vector<reco::Track>::const_iterator itv = newPVTkCollection.begin(); itv != newPVTkCollection.end(); itv++ )
            {
                reco::Track pvtrk = (*itv);

                if( rnd->Rndm() > 0.5 )
                {
                    vtxTkCollection1.push_back(pvtrk);
                    SumTrackPt_p1 += pvtrk.pt();
                    SumTrackPt2_p1 += pvtrk.pt()*pvtrk.pt();
                    pv_fracHighPurity_p1 += pvtrk.quality(reco::TrackBase::highPurity);
                }	
                else
                {	     
                    vtxTkCollection2.push_back(pvtrk);
                    SumTrackPt_p2 += pvtrk.pt();
                    SumTrackPt2_p2 += pvtrk.pt()*pvtrk.pt();
                    pv_fracHighPurity_p2 += pvtrk.quality(reco::TrackBase::highPurity);
                }
            }	     

            // Remake vertices
            vector<TransientVertex> pvst = revertex->makeVertices(newPVTkCollection, *pvbeamspot, iSetup);

            vector<TransientVertex> pvst1 = revertex->makeVertices(vtxTkCollection1, *pvbeamspot, iSetup);
            vector<TransientVertex> pvst2 = revertex->makeVertices(vtxTkCollection2, *pvbeamspot, iSetup);

            pv_trk_pvN.push_back( pvst.size() );
            pv_trk_pv1N.push_back( pvst1.size() );
            pv_trk_pv2N.push_back( pvst2.size() );

            if( !pvst1.empty() && !pvst2.empty() )
            {
                reco::Vertex vtx1 = reco::Vertex(pvst1.front());
                reco::Vertex vtx2 = reco::Vertex(pvst2.front());

                pv_trk_pvunbiased_IsValid_p1.push_back( vtx1.isValid() );
                pv_trk_pvunbiased_IsValid_p2.push_back( vtx2.isValid() );

                pv_trk_pvunbiased_IsFake_p1.push_back( vtx1.isFake() );
                pv_trk_pvunbiased_IsFake_p2.push_back( vtx2.isFake() );

                pv_trk_pvunbiased_NTracks_p1.push_back( vtxTkCollection1.size() );
                pv_trk_pvunbiased_NTracks_p2.push_back( vtxTkCollection2.size() );

                pv_trk_pvunbiased_SumTrackPt_p1.push_back( SumTrackPt_p1 );
                pv_trk_pvunbiased_SumTrackPt_p2.push_back( SumTrackPt_p2 );

                pv_trk_pvunbiased_SumTrackPt2_p1.push_back( SumTrackPt2_p1 );	     
                pv_trk_pvunbiased_SumTrackPt2_p2.push_back( SumTrackPt2_p2 );

                pv_trk_pvunbiased_fracHighPurity_p1.push_back( pv_fracHighPurity_p1 );
                pv_trk_pvunbiased_fracHighPurity_p2.push_back( pv_fracHighPurity_p2 );

                pv_trk_pvunbiased_chi2_p1.push_back( vtx1.chi2() );
                pv_trk_pvunbiased_chi2_p2.push_back( vtx2.chi2() );

                pv_trk_pvunbiased_ndof_p1.push_back( vtx1.ndof() );
                pv_trk_pvunbiased_ndof_p2.push_back( vtx2.ndof() );

                pv_trk_pvunbiased_x_p1.push_back( vtx1.x()*micron );
                pv_trk_pvunbiased_y_p1.push_back( vtx1.y()*micron );
                pv_trk_pvunbiased_z_p1.push_back( vtx1.z()*micron );
                pv_trk_pvunbiased_xError_p1.push_back( vtx1.xError()*micron );
                pv_trk_pvunbiased_yError_p1.push_back( vtx1.yError()*micron );
                pv_trk_pvunbiased_zError_p1.push_back( vtx1.zError()*micron );

                pv_trk_pvunbiased_x_p2.push_back( vtx2.x()*micron );
                pv_trk_pvunbiased_y_p2.push_back( vtx2.y()*micron );
                pv_trk_pvunbiased_z_p2.push_back( vtx2.z()*micron );
                pv_trk_pvunbiased_xError_p2.push_back( vtx2.xError()*micron );
                pv_trk_pvunbiased_yError_p2.push_back( vtx2.yError()*micron );
                pv_trk_pvunbiased_zError_p2.push_back( vtx2.zError()*micron );

                Track::Point vtxPositionUnbiased1 = Track::Point(vtx1.position().x(), vtx1.position().y(), vtx1.position().z());
                Track::Point vtxPositionUnbiased2 = Track::Point(vtx2.position().x(), vtx2.position().y(), vtx2.position().z());

                pv_trk_d0_pvunbiased_p1.push_back( trk.dxy(vtxPositionUnbiased1) * micron );
                pv_trk_dz_pvunbiased_p1.push_back( trk.dz(vtxPositionUnbiased1) * micron );
                pv_trk_d0_bs_zpvunbiased_p1.push_back( trk.dxy(pvbeamspot->position(vtxPositionUnbiased1.z())) * micron );

                pv_trk_d0_pvunbiased_p2.push_back( trk.dxy(vtxPositionUnbiased2) * micron );
                pv_trk_dz_pvunbiased_p2.push_back( trk.dz(vtxPositionUnbiased2) * micron );
                pv_trk_d0_bs_zpvunbiased_p2.push_back( trk.dxy(pvbeamspot->position(vtxPositionUnbiased2.z())) * micron );
            }	     
            else
            {
                pv_trk_pvunbiased_IsValid_p1.push_back( 0 );
                pv_trk_pvunbiased_IsValid_p2.push_back( 0 );

                pv_trk_pvunbiased_IsFake_p1.push_back( 0 );
                pv_trk_pvunbiased_IsFake_p2.push_back( 0 );

                pv_trk_pvunbiased_NTracks_p1.push_back( 0 );
                pv_trk_pvunbiased_NTracks_p2.push_back( 0 );

                pv_trk_pvunbiased_SumTrackPt_p1.push_back( null );
                pv_trk_pvunbiased_SumTrackPt_p2.push_back( null );

                pv_trk_pvunbiased_SumTrackPt2_p1.push_back( null );
                pv_trk_pvunbiased_SumTrackPt2_p2.push_back( null );

                pv_trk_pvunbiased_fracHighPurity_p1.push_back( null );
                pv_trk_pvunbiased_fracHighPurity_p2.push_back( null );

                pv_trk_pvunbiased_chi2_p1.push_back( null );
                pv_trk_pvunbiased_chi2_p2.push_back( null );

                pv_trk_pvunbiased_ndof_p1.push_back( null );
                pv_trk_pvunbiased_ndof_p2.push_back( null );

                pv_trk_pvunbiased_x_p1.push_back( null );
                pv_trk_pvunbiased_y_p1.push_back( null );
                pv_trk_pvunbiased_z_p1.push_back( null );
                pv_trk_pvunbiased_xError_p1.push_back( null );
                pv_trk_pvunbiased_yError_p1.push_back( null );
                pv_trk_pvunbiased_zError_p1.push_back( null );

                pv_trk_pvunbiased_x_p2.push_back( null );
                pv_trk_pvunbiased_y_p2.push_back( null );
                pv_trk_pvunbiased_z_p2.push_back( null );
                pv_trk_pvunbiased_xError_p2.push_back( null );
                pv_trk_pvunbiased_yError_p2.push_back( null );
                pv_trk_pvunbiased_zError_p2.push_back( null );

                pv_trk_d0_pvunbiased_p1.push_back( null );
                pv_trk_dz_pvunbiased_p1.push_back( null );
                pv_trk_d0_bs_zpvunbiased_p1.push_back( null );

                pv_trk_d0_pvunbiased_p2.push_back( null );
                pv_trk_dz_pvunbiased_p2.push_back( null );
                pv_trk_d0_bs_zpvunbiased_p2.push_back( null );
            }

            if( !pvst.empty() )
            {
                reco::Vertex vtxt = reco::Vertex(pvst.front());

                float unbiasedSumTrackPt = 0.;
                float unbiasedSumTrackPt2 = 0.;
                float unbiasedFracHighPurity = 0.;

                for( TrackCollection::const_iterator itt = newPVTkCollection.begin(); itt != newPVTkCollection.end(); itt++ )
                {
                    unbiasedSumTrackPt += (*itt).pt();
                    unbiasedSumTrackPt2 += (*itt).pt()*(*itt).pt();
                    unbiasedFracHighPurity += (*itt).quality(reco::TrackBase::highPurity);
                }		  
                int nTracksUnbiased = vtxt.tracksSize();
                if( nTracksUnbiased ) unbiasedFracHighPurity /= float(nTracksUnbiased);

                pv_trk_pvunbiased_IsValid.push_back( vtxt.isValid() );
                pv_trk_pvunbiased_IsFake.push_back( vtxt.isFake() );		  
                pv_trk_pvunbiased_NTracks.push_back( newPVTkCollection.size() );
                pv_trk_pvunbiased_SumTrackPt.push_back( unbiasedSumTrackPt );
                pv_trk_pvunbiased_SumTrackPt2.push_back( unbiasedSumTrackPt2 );
                pv_trk_pvunbiased_fracHighPurity.push_back( unbiasedFracHighPurity );
                pv_trk_pvunbiased_chi2.push_back( vtxt.chi2() );		  
                pv_trk_pvunbiased_ndof.push_back( vtxt.ndof() );		  
                pv_trk_pvunbiased_x.push_back( vtxt.x()*micron );
                pv_trk_pvunbiased_y.push_back( vtxt.y()*micron );
                pv_trk_pvunbiased_z.push_back( vtxt.z()*micron );
                pv_trk_pvunbiased_xError.push_back( vtxt.xError()*micron );
                pv_trk_pvunbiased_yError.push_back( vtxt.yError()*micron );
                pv_trk_pvunbiased_zError.push_back( vtxt.zError()*micron );

                Track::Point vtxPositionUnbiased = Track::Point(vtxt.position().x(), vtxt.position().y(), vtxt.position().z());

                pv_trk_d0_pvunbiased.push_back( trk.dxy(vtxPositionUnbiased) * micron );
                pv_trk_dz_pvunbiased.push_back( trk.dz(vtxPositionUnbiased) * micron );
                pv_trk_d0_bs_zpvunbiased.push_back( trk.dxy(pvbeamspot->position(vtxPositionUnbiased.z())) * micron );
            }
            else
            {
                pv_trk_pvunbiased_IsValid.push_back( 0 );
                pv_trk_pvunbiased_IsFake.push_back( 0 );
                pv_trk_pvunbiased_NTracks.push_back( null );
                pv_trk_pvunbiased_SumTrackPt.push_back( null );
                pv_trk_pvunbiased_SumTrackPt2.push_back( null );
                pv_trk_pvunbiased_fracHighPurity.push_back( null );
                pv_trk_pvunbiased_chi2.push_back( null );		  
                pv_trk_pvunbiased_ndof.push_back( null );
                pv_trk_pvunbiased_x.push_back( null );
                pv_trk_pvunbiased_y.push_back( null );
                pv_trk_pvunbiased_z.push_back( null );
                pv_trk_pvunbiased_xError.push_back( null );
                pv_trk_pvunbiased_yError.push_back( null );
                pv_trk_pvunbiased_zError.push_back( null );

                pv_trk_d0_pvunbiased.push_back( null );
                pv_trk_dz_pvunbiased.push_back( null );
                pv_trk_d0_bs_zpvunbiased.push_back( null );
            }	     

            pv_SumTrackPt += trk.pt();
            pv_SumTrackPt2 += trk.pt()*trk.pt();
            pv_fracHighPurity += trk.quality(reco::TrackBase::highPurity);

            if( pvs[ipv].hasTrackWeight() ) pv_trackWeight.push_back( pvs[ipv].trackWeight(*it) );
            else pv_trackWeight.push_back( null );

            pv_trk_isHighPurity.push_back( trk.quality(reco::TrackBase::highPurity) );
            pv_trk_algo.push_back( trk.algo() );
            pv_trk_originalAlgo.push_back( trk.originalAlgo() );

            pv_trk_pt.push_back( trk.pt() );
            pv_trk_px.push_back( trk.px() );
            pv_trk_py.push_back( trk.py() );
            pv_trk_pz.push_back( trk.pz() );
            pv_trk_p.push_back( trk.p() );
            pv_trk_eta.push_back( trk.eta() );
            pv_trk_phi.push_back( trk.phi() );

            pv_trk_nTrackerLayers.push_back( trk.hitPattern().trackerLayersWithMeasurement() );
            pv_trk_nPixelBarrelLayers.push_back( trk.hitPattern().pixelBarrelLayersWithMeasurement() );
            pv_trk_nPixelEndcapLayers.push_back( trk.hitPattern().pixelEndcapLayersWithMeasurement() );
            pv_trk_nStripLayers.push_back( trk.hitPattern().stripLayersWithMeasurement() );

            pv_trk_nValid.push_back( trk.numberOfValidHits() );
            pv_trk_fValid.push_back( trk.validFraction() );
            pv_trk_nValidTracker.push_back( trk.hitPattern().numberOfValidTrackerHits() );
            pv_trk_nValidPixelBarrel.push_back( trk.hitPattern().numberOfValidPixelBarrelHits() );
            pv_trk_nValidPixelEndcap.push_back( trk.hitPattern().numberOfValidPixelEndcapHits() );
            pv_trk_nValidStrip.push_back( trk.hitPattern().numberOfValidStripHits() );

            pv_trk_nMissed.push_back( trk.numberOfLostHits() );
            pv_trk_nMissedOut.push_back( trk.hitPattern().numberOfLostHits(HitPattern::MISSING_OUTER_HITS) );
            pv_trk_nMissedIn.push_back( trk.hitPattern().numberOfLostHits(HitPattern::MISSING_INNER_HITS) );
            pv_trk_nMissedTrackerOut.push_back( trk.hitPattern().numberOfLostTrackerHits(HitPattern::MISSING_OUTER_HITS) );
            pv_trk_nMissedTrackerIn.push_back( trk.hitPattern().numberOfLostTrackerHits(HitPattern::MISSING_INNER_HITS) );
            pv_trk_nMissedPixelBarrelOut.push_back( trk.hitPattern().numberOfLostPixelBarrelHits(HitPattern::MISSING_OUTER_HITS) );
            pv_trk_nMissedPixelBarrelIn.push_back( trk.hitPattern().numberOfLostPixelBarrelHits(HitPattern::MISSING_INNER_HITS) );
            pv_trk_nMissedPixelEndcapOut.push_back( trk.hitPattern().numberOfLostPixelEndcapHits(HitPattern::MISSING_OUTER_HITS) );
            pv_trk_nMissedPixelEndcapIn.push_back( trk.hitPattern().numberOfLostPixelEndcapHits(HitPattern::MISSING_INNER_HITS) );

            pv_trk_hasPixelBarrelLayer1.push_back( trk.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelBarrel, 1) );
            pv_trk_hasPixelEndcapLayer1.push_back( trk.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelEndcap, 1) );
            pv_trk_hasPixelBarrelLayer2.push_back( trk.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelBarrel, 2) );
            pv_trk_hasPixelEndcapLayer2.push_back( trk.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelEndcap, 2) );
            pv_trk_hasPixelBarrelLayer3.push_back( trk.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelBarrel, 3) );
            pv_trk_hasPixelEndcapLayer3.push_back( trk.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelEndcap, 3) );
            pv_trk_hasPixelBarrelLayer4.push_back( trk.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelBarrel, 4) );
            pv_trk_hasPixelEndcapLayer4.push_back( trk.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelEndcap, 4) );

            pv_trk_quality.push_back( trk.qualityMask() );
            pv_trk_normalizedChi2.push_back( trk.normalizedChi2() );
            pv_trk_ndof.push_back( trk.ndof() );
            pv_trk_charge.push_back( trk.charge() );
            pv_trk_qoverp.push_back( trk.qoverp() );
            pv_trk_qoverpError.push_back( trk.qoverpError() );
            pv_trk_theta.push_back( trk.theta() );
            pv_trk_thetaError.push_back( trk.thetaError() );
            pv_trk_lambda.push_back( trk.lambda() );
            pv_trk_lambdaError.push_back( trk.lambdaError() );
            pv_trk_ptError.push_back( trk.ptError() );
            pv_trk_etaError.push_back( trk.etaError() );
            pv_trk_phiError.push_back( trk.phiError() );

            pv_trk_d0.push_back( trk.dxy() * micron );
            pv_trk_dz.push_back( trk.dz() * micron );
            pv_trk_d0_pv.push_back( trk.dxy(vtxPosition) * micron );
            pv_trk_dz_pv.push_back( trk.dz(vtxPosition) * micron );
            pv_trk_d0_bs.push_back( trk.dxy(pvbeamspot->position()) * micron );
            pv_trk_d0_bs_zpca.push_back( trk.dxy(*pvbeamspot) * micron );
            pv_trk_d0_bs_zpv.push_back( trk.dxy(pvbeamspot->position(vtxPosition.z())) * micron );
            pv_trk_dz_bs.push_back( trk.dz(pvbeamspot->position()) * micron );
            pv_trk_d0Err.push_back( trk.d0Error() * micron );
            pv_trk_dzErr.push_back( trk.dzError() * micron );

            iTrk++;
        }
        if( nTracks ) pv_fracHighPurity /= float(nTracks);

        ftree->pv_IsValid.push_back( pvr[ipv].isValid() );
        ftree->pv_IsFake.push_back( pvr[ipv].isFake() );
        ftree->pv_NTracks.push_back( nTracks );
        ftree->pv_SumTrackPt.push_back( pv_SumTrackPt );
        ftree->pv_SumTrackPt2.push_back( pv_SumTrackPt2 );
        ftree->pv_fracHighPurity.push_back( pv_fracHighPurity );
        ftree->pv_chi2.push_back( pvr[ipv].chi2() );
        ftree->pv_ndof.push_back( pvr[ipv].ndof() );
        ftree->pv_x.push_back( pvr[ipv].x()*micron );
        ftree->pv_y.push_back( pvr[ipv].y()*micron );
        ftree->pv_z.push_back( pvr[ipv].z()*micron );
        ftree->pv_xError.push_back( pvr[ipv].xError()*micron );
        ftree->pv_yError.push_back( pvr[ipv].yError()*micron );
        ftree->pv_zError.push_back( pvr[ipv].zError()*micron );

        ftree->pv_trk_weight.push_back( pv_trackWeight );
        ftree->pv_trk_isHighPurity.push_back( pv_trk_isHighPurity );
        ftree->pv_trk_algo.push_back( pv_trk_algo );
        ftree->pv_trk_originalAlgo.push_back( pv_trk_originalAlgo );

        ftree->pv_trk_idx.push_back( pv_trk_idx );

        ftree->pv_trk_pvN.push_back( pv_trk_pvN );
        ftree->pv_trk_pv1N.push_back( pv_trk_pv1N );
        ftree->pv_trk_pv2N.push_back( pv_trk_pv2N );

        ftree->pv_trk_pvunbiased_IsValid.push_back( pv_trk_pvunbiased_IsValid );
        ftree->pv_trk_pvunbiased_IsFake.push_back( pv_trk_pvunbiased_IsFake );
        ftree->pv_trk_pvunbiased_NTracks.push_back( pv_trk_pvunbiased_NTracks );
        ftree->pv_trk_pvunbiased_SumTrackPt.push_back( pv_trk_pvunbiased_SumTrackPt );
        ftree->pv_trk_pvunbiased_SumTrackPt2.push_back( pv_trk_pvunbiased_SumTrackPt2 );
        ftree->pv_trk_pvunbiased_fracHighPurity.push_back( pv_trk_pvunbiased_fracHighPurity );
        ftree->pv_trk_pvunbiased_chi2.push_back( pv_trk_pvunbiased_chi2 );
        ftree->pv_trk_pvunbiased_ndof.push_back( pv_trk_pvunbiased_ndof );
        ftree->pv_trk_pvunbiased_x.push_back( pv_trk_pvunbiased_x );
        ftree->pv_trk_pvunbiased_y.push_back( pv_trk_pvunbiased_y );
        ftree->pv_trk_pvunbiased_z.push_back( pv_trk_pvunbiased_z );
        ftree->pv_trk_pvunbiased_xError.push_back( pv_trk_pvunbiased_xError );
        ftree->pv_trk_pvunbiased_yError.push_back( pv_trk_pvunbiased_yError );
        ftree->pv_trk_pvunbiased_zError.push_back( pv_trk_pvunbiased_zError );

        ftree->pv_trk_d0_pvunbiased.push_back( pv_trk_d0_pvunbiased );
        ftree->pv_trk_dz_pvunbiased.push_back( pv_trk_dz_pvunbiased );
        ftree->pv_trk_d0_bs_zpvunbiased.push_back( pv_trk_d0_bs_zpvunbiased );

        ftree->pv_trk_pvunbiased_IsValid_p1.push_back( pv_trk_pvunbiased_IsValid_p1 );
        ftree->pv_trk_pvunbiased_IsFake_p1.push_back( pv_trk_pvunbiased_IsFake_p1 );
        ftree->pv_trk_pvunbiased_NTracks_p1.push_back( pv_trk_pvunbiased_NTracks_p1 );
        ftree->pv_trk_pvunbiased_SumTrackPt_p1.push_back( pv_trk_pvunbiased_SumTrackPt_p1 );
        ftree->pv_trk_pvunbiased_SumTrackPt2_p1.push_back( pv_trk_pvunbiased_SumTrackPt2_p1 );
        ftree->pv_trk_pvunbiased_fracHighPurity_p1.push_back( pv_trk_pvunbiased_fracHighPurity_p1 );
        ftree->pv_trk_pvunbiased_chi2_p1.push_back( pv_trk_pvunbiased_chi2_p1 );
        ftree->pv_trk_pvunbiased_ndof_p1.push_back( pv_trk_pvunbiased_ndof_p1 );
        ftree->pv_trk_pvunbiased_x_p1.push_back( pv_trk_pvunbiased_x_p1 );
        ftree->pv_trk_pvunbiased_y_p1.push_back( pv_trk_pvunbiased_y_p1 );
        ftree->pv_trk_pvunbiased_z_p1.push_back( pv_trk_pvunbiased_z_p1 );
        ftree->pv_trk_pvunbiased_xError_p1.push_back( pv_trk_pvunbiased_xError_p1 );
        ftree->pv_trk_pvunbiased_yError_p1.push_back( pv_trk_pvunbiased_yError_p1 );
        ftree->pv_trk_pvunbiased_zError_p1.push_back( pv_trk_pvunbiased_zError_p1 );

        ftree->pv_trk_d0_pvunbiased_p1.push_back( pv_trk_d0_pvunbiased_p1 );
        ftree->pv_trk_dz_pvunbiased_p1.push_back( pv_trk_dz_pvunbiased_p1 );
        ftree->pv_trk_d0_bs_zpvunbiased_p1.push_back( pv_trk_d0_bs_zpvunbiased_p1 );

        ftree->pv_trk_pvunbiased_IsValid_p2.push_back( pv_trk_pvunbiased_IsValid_p2 );
        ftree->pv_trk_pvunbiased_IsFake_p2.push_back( pv_trk_pvunbiased_IsFake_p2 );
        ftree->pv_trk_pvunbiased_NTracks_p2.push_back( pv_trk_pvunbiased_NTracks_p2 );
        ftree->pv_trk_pvunbiased_SumTrackPt_p2.push_back( pv_trk_pvunbiased_SumTrackPt_p2 );
        ftree->pv_trk_pvunbiased_SumTrackPt2_p2.push_back( pv_trk_pvunbiased_SumTrackPt2_p2 );
        ftree->pv_trk_pvunbiased_fracHighPurity_p2.push_back( pv_trk_pvunbiased_fracHighPurity_p2 );
        ftree->pv_trk_pvunbiased_chi2_p2.push_back( pv_trk_pvunbiased_chi2_p2 );
        ftree->pv_trk_pvunbiased_ndof_p2.push_back( pv_trk_pvunbiased_ndof_p2 );
        ftree->pv_trk_pvunbiased_x_p2.push_back( pv_trk_pvunbiased_x_p2 );
        ftree->pv_trk_pvunbiased_y_p2.push_back( pv_trk_pvunbiased_y_p2 );
        ftree->pv_trk_pvunbiased_z_p2.push_back( pv_trk_pvunbiased_z_p2 );
        ftree->pv_trk_pvunbiased_xError_p2.push_back( pv_trk_pvunbiased_xError_p2 );
        ftree->pv_trk_pvunbiased_yError_p2.push_back( pv_trk_pvunbiased_yError_p2 );
        ftree->pv_trk_pvunbiased_zError_p2.push_back( pv_trk_pvunbiased_zError_p2 );

        ftree->pv_trk_d0_pvunbiased_p2.push_back( pv_trk_d0_pvunbiased_p2 );
        ftree->pv_trk_dz_pvunbiased_p2.push_back( pv_trk_dz_pvunbiased_p2 );
        ftree->pv_trk_d0_bs_zpvunbiased_p2.push_back( pv_trk_d0_bs_zpvunbiased_p2 );

        ftree->pv_trk_pt.push_back( pv_trk_pt );
        ftree->pv_trk_px.push_back( pv_trk_px );
        ftree->pv_trk_py.push_back( pv_trk_py );
        ftree->pv_trk_pz.push_back( pv_trk_pz );
        ftree->pv_trk_p.push_back( pv_trk_p );
        ftree->pv_trk_eta.push_back( pv_trk_eta );
        ftree->pv_trk_phi.push_back( pv_trk_phi );

        ftree->pv_trk_nTrackerLayers.push_back( pv_trk_nTrackerLayers );
        ftree->pv_trk_nPixelBarrelLayers.push_back( pv_trk_nPixelBarrelLayers );
        ftree->pv_trk_nPixelEndcapLayers.push_back( pv_trk_nPixelEndcapLayers );
        ftree->pv_trk_nStripLayers.push_back( pv_trk_nStripLayers );

        ftree->pv_trk_nValid.push_back( pv_trk_nValid );
        ftree->pv_trk_fValid.push_back( pv_trk_fValid );
        ftree->pv_trk_nValidTracker.push_back( pv_trk_nValidTracker );
        ftree->pv_trk_nValidPixelBarrel.push_back( pv_trk_nValidPixelBarrel );
        ftree->pv_trk_nValidPixelEndcap.push_back( pv_trk_nValidPixelEndcap );
        ftree->pv_trk_nValidStrip.push_back( pv_trk_nValidStrip );

        ftree->pv_trk_nMissed.push_back( pv_trk_nMissed );
        ftree->pv_trk_nMissedOut.push_back( pv_trk_nMissedOut );
        ftree->pv_trk_nMissedIn.push_back( pv_trk_nMissedIn );
        ftree->pv_trk_nMissedTrackerOut.push_back( pv_trk_nMissedTrackerOut );
        ftree->pv_trk_nMissedTrackerIn.push_back( pv_trk_nMissedTrackerIn );
        ftree->pv_trk_nMissedPixelBarrelOut.push_back( pv_trk_nMissedPixelBarrelOut );
        ftree->pv_trk_nMissedPixelBarrelIn.push_back( pv_trk_nMissedPixelBarrelIn );
        ftree->pv_trk_nMissedPixelEndcapOut.push_back( pv_trk_nMissedPixelEndcapOut );
        ftree->pv_trk_nMissedPixelEndcapIn.push_back( pv_trk_nMissedPixelEndcapIn );

        ftree->pv_trk_hasPixelBarrelLayer1.push_back( pv_trk_hasPixelBarrelLayer1 );
        ftree->pv_trk_hasPixelEndcapLayer1.push_back( pv_trk_hasPixelEndcapLayer1 );
        ftree->pv_trk_hasPixelBarrelLayer2.push_back( pv_trk_hasPixelBarrelLayer2 );
        ftree->pv_trk_hasPixelEndcapLayer2.push_back( pv_trk_hasPixelEndcapLayer2 );
        ftree->pv_trk_hasPixelBarrelLayer3.push_back( pv_trk_hasPixelBarrelLayer3 );
        ftree->pv_trk_hasPixelEndcapLayer3.push_back( pv_trk_hasPixelEndcapLayer3 );
        ftree->pv_trk_hasPixelBarrelLayer4.push_back( pv_trk_hasPixelBarrelLayer4 );
        ftree->pv_trk_hasPixelEndcapLayer4.push_back( pv_trk_hasPixelEndcapLayer4 );

        ftree->pv_trk_quality.push_back( pv_trk_quality );
        ftree->pv_trk_normalizedChi2.push_back( pv_trk_normalizedChi2 );
        ftree->pv_trk_ndof.push_back( pv_trk_ndof );
        ftree->pv_trk_charge.push_back( pv_trk_charge );
        ftree->pv_trk_qoverp.push_back( pv_trk_qoverp );
        ftree->pv_trk_qoverpError.push_back( pv_trk_qoverpError );
        ftree->pv_trk_theta.push_back( pv_trk_theta );
        ftree->pv_trk_thetaError.push_back( pv_trk_thetaError );
        ftree->pv_trk_lambda.push_back( pv_trk_lambda );
        ftree->pv_trk_lambdaError.push_back( pv_trk_lambdaError );
        ftree->pv_trk_ptError.push_back( pv_trk_ptError );
        ftree->pv_trk_etaError.push_back( pv_trk_etaError );
        ftree->pv_trk_phiError.push_back( pv_trk_phiError );

        ftree->pv_trk_d0.push_back( pv_trk_d0 );
        ftree->pv_trk_dz.push_back( pv_trk_dz );
        ftree->pv_trk_d0_pv.push_back( pv_trk_d0_pv );
        ftree->pv_trk_dz_pv.push_back( pv_trk_dz_pv );
        ftree->pv_trk_d0_bs.push_back( pv_trk_d0_bs );
        ftree->pv_trk_d0_bs_zpca.push_back( pv_trk_d0_bs_zpca );
        ftree->pv_trk_d0_bs_zpv.push_back( pv_trk_d0_bs_zpv );
        ftree->pv_trk_dz_bs.push_back( pv_trk_dz_bs );
        ftree->pv_trk_d0Err.push_back( pv_trk_d0Err );
        ftree->pv_trk_dzErr.push_back( pv_trk_dzErr );
    }

    // Vertex split method
    for( unsigned int ipv=0;ipv<pvr.size();ipv++ )
    {	     	   
        std::vector<reco::TransientTrack> vtxTracks = pvs[ipv].originalTracks();
        stable_sort(vtxTracks.begin(), vtxTracks.end(), sortPt);

        int nTracks = pvr[ipv].tracksSize();

        reco::TrackCollection vtxTkCollection1;
        reco::TrackCollection vtxTkCollection2;

        std::vector<int> vtxTkIdx1;
        std::vector<int> vtxTkIdx2;

        float SumTrackPt_p1 = 0;
        float SumTrackPt2_p1 = 0;

        float SumTrackPt_p2 = 0;
        float SumTrackPt2_p2 = 0;

        float pv_fracHighPurity_p1 = 0;
        float pv_fracHighPurity_p2 = 0;

        int iTrk = 0;

        for( std::vector<reco::TransientTrack>::const_iterator it = vtxTracks.begin(); it != vtxTracks.end(); it++ )
        {
            reco::Track trk = (*it).track();

            if( rnd->Rndm() > 0.5 )
            {
                vtxTkCollection1.push_back(trk);
                SumTrackPt_p1 += trk.pt();
                SumTrackPt2_p1 += trk.pt()*trk.pt();
                pv_fracHighPurity_p1 += trk.quality(reco::TrackBase::highPurity);
                vtxTkIdx1.push_back(iTrk);
            }	
            else
            {	     
                vtxTkCollection2.push_back(trk);
                SumTrackPt_p2 += trk.pt();
                SumTrackPt2_p2 += trk.pt()*trk.pt();
                pv_fracHighPurity_p2 += trk.quality(reco::TrackBase::highPurity);
                vtxTkIdx2.push_back(iTrk);
            }

            iTrk++;
        }
        if( nTracks ) 
        {
            pv_fracHighPurity_p1 /= float(nTracks);
            pv_fracHighPurity_p2 /= float(nTracks);
        }	

        vector<TransientVertex> pvs1 = revertex->makeVertices(vtxTkCollection1, *pvbeamspot, iSetup);
        vector<TransientVertex> pvs2 = revertex->makeVertices(vtxTkCollection2, *pvbeamspot, iSetup);

        if( !pvs1.empty() && !pvs2.empty() )
        {
            reco::Vertex vtx1 = reco::Vertex(pvs1.front());
            reco::Vertex vtx2 = reco::Vertex(pvs2.front());

            ftree->pv_IsValid_p1.push_back( vtx1.isValid() );
            ftree->pv_IsValid_p2.push_back( vtx2.isValid() );

            ftree->pv_IsFake_p1.push_back( vtx1.isFake() );
            ftree->pv_IsFake_p2.push_back( vtx2.isFake() );

            ftree->pv_NTracks_p1.push_back( vtxTkCollection1.size() );
            ftree->pv_NTracks_p2.push_back( vtxTkCollection2.size() );

            ftree->pv_SumTrackPt_p1.push_back( SumTrackPt_p1 );
            ftree->pv_SumTrackPt_p2.push_back( SumTrackPt_p2 );

            ftree->pv_SumTrackPt2_p1.push_back( SumTrackPt2_p1 );	     
            ftree->pv_SumTrackPt2_p2.push_back( SumTrackPt2_p2 );

            ftree->pv_fracHighPurity_p1.push_back( pv_fracHighPurity_p1 );
            ftree->pv_fracHighPurity_p2.push_back( pv_fracHighPurity_p2 );

            ftree->pv_vtxTkIdx_p1.push_back( vtxTkIdx1 );
            ftree->pv_vtxTkIdx_p2.push_back( vtxTkIdx2 );

            ftree->pv_chi2_p1.push_back( vtx1.chi2() );
            ftree->pv_chi2_p2.push_back( vtx2.chi2() );

            ftree->pv_ndof_p1.push_back( vtx1.ndof() );
            ftree->pv_ndof_p2.push_back( vtx2.ndof() );

            ftree->pv_x_p1.push_back( vtx1.x()*micron );
            ftree->pv_y_p1.push_back( vtx1.y()*micron );
            ftree->pv_z_p1.push_back( vtx1.z()*micron );
            ftree->pv_xError_p1.push_back( vtx1.xError()*micron );
            ftree->pv_yError_p1.push_back( vtx1.yError()*micron );
            ftree->pv_zError_p1.push_back( vtx1.zError()*micron );

            ftree->pv_x_p2.push_back( vtx2.x()*micron );
            ftree->pv_y_p2.push_back( vtx2.y()*micron );
            ftree->pv_z_p2.push_back( vtx2.z()*micron );
            ftree->pv_xError_p2.push_back( vtx2.xError()*micron );
            ftree->pv_yError_p2.push_back( vtx2.yError()*micron );
            ftree->pv_zError_p2.push_back( vtx2.zError()*micron );
        }
        else
        {
            ftree->pv_IsValid_p1.push_back( 0 );
            ftree->pv_IsValid_p2.push_back( 0 );

            ftree->pv_IsFake_p1.push_back( 0 );
            ftree->pv_IsFake_p2.push_back( 0 );

            ftree->pv_NTracks_p1.push_back( 0 );
            ftree->pv_NTracks_p2.push_back( 0 );

            ftree->pv_SumTrackPt_p1.push_back( null );
            ftree->pv_SumTrackPt_p2.push_back( null );

            ftree->pv_SumTrackPt2_p1.push_back( null );
            ftree->pv_SumTrackPt2_p2.push_back( null );

            ftree->pv_fracHighPurity_p1.push_back( null );
            ftree->pv_fracHighPurity_p2.push_back( null );

            ftree->pv_vtxTkIdx_p1.push_back( vtxTkIdx1 );
            ftree->pv_vtxTkIdx_p2.push_back( vtxTkIdx2 );

            ftree->pv_chi2_p1.push_back( null );
            ftree->pv_chi2_p2.push_back( null );

            ftree->pv_ndof_p1.push_back( null );
            ftree->pv_ndof_p2.push_back( null );

            ftree->pv_x_p1.push_back( null );
            ftree->pv_y_p1.push_back( null );
            ftree->pv_z_p1.push_back( null );
            ftree->pv_xError_p1.push_back( null );
            ftree->pv_yError_p1.push_back( null );
            ftree->pv_zError_p1.push_back( null );

            ftree->pv_x_p2.push_back( null );
            ftree->pv_y_p2.push_back( null );
            ftree->pv_z_p2.push_back( null );
            ftree->pv_xError_p2.push_back( null );
            ftree->pv_yError_p2.push_back( null );
            ftree->pv_zError_p2.push_back( null );
        }	
    }   

    // PFJets
    unsigned int nFPJets = pfJets->size();
    ftree->pfjet_n = nFPJets;

    for( unsigned int ij=0;ij<nFPJets;ij++ )
    {
        pat::Jet jet = pfJets->at(ij);

        ftree->pfjet_pt.push_back( jet.pt() );
        ftree->pfjet_eta.push_back( jet.eta() );
        ftree->pfjet_phi.push_back( jet.phi() );
        ftree->pfjet_E.push_back( jet.energy() );
    }   

    /*	reco::TransientTrack tranitk = (*theB).build(*itk);

        GlobalPoint pvPos(vtx.position().x(), vtx.position().y(), vtx.position().z());
        GlobalPoint bsPos(pvbeamspot->position().x(), pvbeamspot->position().y(), pvbeamspot->position().z());
        GlobalVector dir(itk->px(), itk->py(), itk->pz());

        reco::Vertex bspv(pvbeamspot->position(), pvbeamspot->rotatedCovariance3D());

        float d0_perigee_pv = -tranitk.trajectoryStateClosestToPoint(pvPos).perigeeParameters().transverseImpactParameter() * micron;
        float d0_perigee_bs = -tranitk.trajectoryStateClosestToPoint(bsPos).perigeeParameters().transverseImpactParameter() * micron;
        float dz_perigee_pv = tranitk.trajectoryStateClosestToPoint(pvPos).perigeeParameters().longitudinalImpactParameter() * micron;
        float dz_perigee_bs = tranitk.trajectoryStateClosestToPoint(bsPos).perigeeParameters().longitudinalImpactParameter() * micron;

        float d0_IPTools_pv = IPTools::signedTransverseImpactParameter(tranitk,dir,vtx).second.value() * micron;
        float d0Err_IPTools_pv = IPTools::signedTransverseImpactParameter(tranitk,dir,vtx).second.error() * micron;
        float d0Sign_IPTools_pv = IPTools::signedTransverseImpactParameter(tranitk,dir,vtx).second.significance();
        float d0_IPTools_bs = IPTools::signedTransverseImpactParameter(tranitk,dir,bspv).second.value() * micron;
        float d0Err_IPTools_bs = IPTools::signedTransverseImpactParameter(tranitk,dir,bspv).second.error() * micron;
        float d0Sign_IPTools_bs = IPTools::signedTransverseImpactParameter(tranitk,dir,bspv).second.significance();

        float ip3d_IPTools_pv = IPTools::signedImpactParameter3D(tranitk,dir,vtx).second.value() * micron;
        float ip3dErr_IPTools_pv = IPTools::signedImpactParameter3D(tranitk,dir,vtx).second.error() * micron;
        float ip3dSign_IPTools_pv = IPTools::signedImpactParameter3D(tranitk,dir,vtx).second.significance();
        float ip3d_IPTools_bs = IPTools::signedImpactParameter3D(tranitk,dir,bspv).second.value() * micron;
        float ip3dErr_IPTools_bs = IPTools::signedImpactParameter3D(tranitk,dir,bspv).second.error() * micron;
        float ip3dSign_IPTools_bs = IPTools::signedImpactParameter3D(tranitk,dir,bspv).second.significance();

        float ip3dl_IPTools_pv = IPTools::linearizedSignedImpactParameter3D(tranitk,dir,vtx).second.value() * micron;
        float ip3dlErr_IPTools_pv = IPTools::linearizedSignedImpactParameter3D(tranitk,dir,vtx).second.error() * micron;
        float ip3dlSign_IPTools_pv = IPTools::linearizedSignedImpactParameter3D(tranitk,dir,vtx).second.significance();
        float ip3dl_IPTools_bs = IPTools::linearizedSignedImpactParameter3D(tranitk,dir,bspv).second.value() * micron;
        float ip3dlErr_IPTools_bs = IPTools::linearizedSignedImpactParameter3D(tranitk,dir,bspv).second.error() * micron;
        float ip3dlSign_IPTools_bs = IPTools::linearizedSignedImpactParameter3D(tranitk,dir,bspv).second.significance();*/

    // Tracks from PFJets

    /*	float drMinPF = 10E+10;
        int iJetPF = -1;
        int iTrackMinPF = -1;

        for( unsigned int ij=0;ij<pfJets->size();ij++ )
        {
        pat::Jet jet = pfJets->at(ij);
        reco::TrackRefVector trks = jet.associatedTracks();

        for( unsigned int it=0;it<trks.size();it++ )
        {
        for( unsigned int ipv=0;ipv<pvs.size();ipv++ )
        {
        std::vector<reco::TransientTrack> vtxTracks = pvs[ipv].originalTracks();

        for( std::vector<reco::TransientTrack>::const_iterator itt=vtxTracks.begin();itt!=vtxTracks.end();itt++ )
        {
        reco::Track trk = (*itt).track();

    //		       edm::RefVector<TrackCollection>::const_iterator itt = find_if(trks.begin(), trks.end(), TrackEqualRef(trk));
    //		       if( itt != trks.end() )
    //			 {
    //			    std::cout << "found" << std::endl;
    //			 }
    }
    }
    }*/
    /*	     edm::RefVector<TrackCollection>::const_iterator itt = find_if(trks.begin(), trks.end(), TrackEqualRef(trkRef));
             if( itt != trks.end() )
             {
             size_t pos = itt - trks.begin();

             iJetPF = ij;

             for( unsigned int it=0;it<trks.size();it++ )
             {
             if( it == pos ) continue;

             float dr = getDeltaR(trks[pos]->eta(), trks[pos]->phi(), trks[it]->eta(), trks[it]->phi());
             if( dr < drMinPF )
             {
             drMinPF = dr;
             iTrackMinPF = it;
             }
             }

             break;
             }
             }

             if( iJetPF >= 0 )
             {
             pat::Jet jet = pfJets->at(iJetPF);

             ftree->trk_pfjet_found.push_back( true );

             ftree->trk_pfjet_pt.push_back( jet.pt() );
             ftree->trk_pfjet_eta.push_back( jet.eta() );
             ftree->trk_pfjet_phi.push_back( jet.phi() );
             ftree->trk_pfjet_nTracks.push_back( jet.associatedTracks().size() );
             }
             else
             {
             ftree->trk_pfjet_found.push_back( false );

             ftree->trk_pfjet_pt.push_back( null );
             ftree->trk_pfjet_eta.push_back( null );
             ftree->trk_pfjet_phi.push_back( null );
             ftree->trk_pfjet_nTracks.push_back( null );
             }	

             if( iTrackMinPF >= 0 )
             {		  
             reco::TrackRef trkCl = pfJets->at(iJetPF).associatedTracks().at(iTrackMinPF);

             ftree->trk_pfjetTrk_found.push_back( true );

             ftree->trk_pfjetTrk_deltaR.push_back( drMinPF );

             ftree->trk_pfjetTrk_pt.push_back( trkCl->pt() );
             ftree->trk_pfjetTrk_px.push_back( trkCl->px() );
             ftree->trk_pfjetTrk_py.push_back( trkCl->py() );
             ftree->trk_pfjetTrk_pz.push_back( trkCl->pz() );
             ftree->trk_pfjetTrk_p.push_back( trkCl->p() );
             ftree->trk_pfjetTrk_eta.push_back( trkCl->eta() );
             ftree->trk_pfjetTrk_phi.push_back( trkCl->phi() );

             ftree->trk_pfjetTrk_nTrackerLayers.push_back( trkCl->hitPattern().trackerLayersWithMeasurement() );
             ftree->trk_pfjetTrk_nPixelBarrelLayers.push_back( trkCl->hitPattern().pixelBarrelLayersWithMeasurement() );
             ftree->trk_pfjetTrk_nPixelEndcapLayers.push_back( trkCl->hitPattern().pixelEndcapLayersWithMeasurement() );
             ftree->trk_pfjetTrk_nStripLayers.push_back( trkCl->hitPattern().stripLayersWithMeasurement() );

             ftree->trk_pfjetTrk_nValid.push_back( trkCl->numberOfValidHits() );
             ftree->trk_pfjetTrk_fValid.push_back( trkCl->validFraction() );
             ftree->trk_pfjetTrk_nValidTracker.push_back( trkCl->hitPattern().numberOfValidTrackerHits() );
             ftree->trk_pfjetTrk_nValidPixelBarrel.push_back( trkCl->hitPattern().numberOfValidPixelBarrelHits() );
             ftree->trk_pfjetTrk_nValidPixelEndcap.push_back( trkCl->hitPattern().numberOfValidPixelEndcapHits() );
             ftree->trk_pfjetTrk_nValidStrip.push_back( trkCl->hitPattern().numberOfValidStripHits() );

    ftree->trk_pfjetTrk_nMissed.push_back( trkCl->numberOfLostHits() );
    ftree->trk_pfjetTrk_nMissedOut.push_back( trkCl->hitPattern().numberOfLostHits(HitPattern::MISSING_OUTER_HITS) );
    ftree->trk_pfjetTrk_nMissedIn.push_back( trkCl->hitPattern().numberOfLostHits(HitPattern::MISSING_INNER_HITS) );
    ftree->trk_pfjetTrk_nMissedTrackerOut.push_back( trkCl->hitPattern().numberOfLostTrackerHits(HitPattern::MISSING_OUTER_HITS) );
    ftree->trk_pfjetTrk_nMissedTrackerIn.push_back( trkCl->hitPattern().numberOfLostTrackerHits(HitPattern::MISSING_INNER_HITS) );
    ftree->trk_pfjetTrk_nMissedPixelBarrelOut.push_back( trkCl->hitPattern().numberOfLostPixelBarrelHits(HitPattern::MISSING_OUTER_HITS) );
    ftree->trk_pfjetTrk_nMissedPixelBarrelIn.push_back( trkCl->hitPattern().numberOfLostPixelBarrelHits(HitPattern::MISSING_INNER_HITS) );
    ftree->trk_pfjetTrk_nMissedPixelEndcapOut.push_back( trkCl->hitPattern().numberOfLostPixelEndcapHits(HitPattern::MISSING_OUTER_HITS) );
    ftree->trk_pfjetTrk_nMissedPixelEndcapIn.push_back( trkCl->hitPattern().numberOfLostPixelEndcapHits(HitPattern::MISSING_INNER_HITS) );

    ftree->trk_pfjetTrk_hasPixelBarrelLayer1.push_back( trkCl->hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelBarrel, 1) );
    ftree->trk_pfjetTrk_hasPixelEndcapLayer1.push_back( trkCl->hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelEndcap, 1) );
    ftree->trk_pfjetTrk_hasPixelBarrelLayer2.push_back( trkCl->hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelBarrel, 2) );
    ftree->trk_pfjetTrk_hasPixelEndcapLayer2.push_back( trkCl->hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelEndcap, 2) );
    ftree->trk_pfjetTrk_hasPixelBarrelLayer3.push_back( trkCl->hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelBarrel, 3) );
    ftree->trk_pfjetTrk_hasPixelEndcapLayer3.push_back( trkCl->hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelEndcap, 3) );
    ftree->trk_pfjetTrk_hasPixelBarrelLayer4.push_back( trkCl->hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelBarrel, 4) );
    ftree->trk_pfjetTrk_hasPixelEndcapLayer4.push_back( trkCl->hitPattern().hasValidHitInPixelLayer(PixelSubdetector::SubDetector::PixelEndcap, 4) );

    ftree->trk_pfjetTrk_quality.push_back( trkCl->qualityMask() );
    ftree->trk_pfjetTrk_normalizedChi2.push_back( trkCl->normalizedChi2() );
    ftree->trk_pfjetTrk_ndof.push_back( trkCl->ndof() );
    ftree->trk_pfjetTrk_charge.push_back( trkCl->charge() );
    ftree->trk_pfjetTrk_qoverp.push_back( trkCl->qoverp() );
    ftree->trk_pfjetTrk_qoverpError.push_back( trkCl->qoverpError() );
    ftree->trk_pfjetTrk_theta.push_back( trkCl->theta() );
    ftree->trk_pfjetTrk_thetaError.push_back( trkCl->thetaError() );
    ftree->trk_pfjetTrk_lambda.push_back( trkCl->lambda() );
    ftree->trk_pfjetTrk_lambdaError.push_back( trkCl->lambdaError() );
    ftree->trk_pfjetTrk_ptError.push_back( trkCl->ptError() );
    ftree->trk_pfjetTrk_etaError.push_back( trkCl->etaError() );
    ftree->trk_pfjetTrk_phiError.push_back( trkCl->phiError() );

    ftree->trk_pfjetTrk_d0.push_back( trkCl->dxy() * micron );
    ftree->trk_pfjetTrk_dz.push_back( trkCl->dz() * micron );
    ftree->trk_pfjetTrk_d0_pv.push_back( trkCl->dxy(vtxPosition) * micron );
    ftree->trk_pfjetTrk_dz_pv.push_back( trkCl->dz(vtxPosition) * micron );
    ftree->trk_pfjetTrk_d0_bs.push_back( trkCl->dxy(pvbeamspot->position()) * micron );
    ftree->trk_pfjetTrk_d0_bs_zpca.push_back( trkCl->dxy(*pvbeamspot) * micron );
    ftree->trk_pfjetTrk_d0_bs_zpv.push_back( trkCl->dxy(pvbeamspot->position(vtx.z())) * micron );
    ftree->trk_pfjetTrk_dz_bs.push_back( trkCl->dz(pvbeamspot->position()) * micron );
    ftree->trk_pfjetTrk_d0Err.push_back( trkCl->d0Error() * micron );
    ftree->trk_pfjetTrk_dzErr.push_back( trkCl->dzError() * micron );
    ftree->trk_pfjetTrk_d0_pv_NoRefit.push_back( trkCl->dxy(vtxH->front().position()) * micron );
    ftree->trk_pfjetTrk_dz_pv_NoRefit.push_back( trkCl->dz(vtxH->front().position()) * micron );
}
else
{
    ftree->trk_pfjetTrk_found.push_back( false );

    ftree->trk_pfjetTrk_deltaR.push_back( null );

    ftree->trk_pfjetTrk_pt.push_back( null );
    ftree->trk_pfjetTrk_px.push_back( null );
    ftree->trk_pfjetTrk_py.push_back( null );
    ftree->trk_pfjetTrk_pz.push_back( null );
    ftree->trk_pfjetTrk_p.push_back( null );
    ftree->trk_pfjetTrk_eta.push_back( null );
    ftree->trk_pfjetTrk_phi.push_back( null );

    ftree->trk_pfjetTrk_nTrackerLayers.push_back( null );
    ftree->trk_pfjetTrk_nPixelBarrelLayers.push_back( null );
    ftree->trk_pfjetTrk_nPixelEndcapLayers.push_back( null );
    ftree->trk_pfjetTrk_nStripLayers.push_back( null );

    ftree->trk_pfjetTrk_nValid.push_back( null );
    ftree->trk_pfjetTrk_fValid.push_back( null );
    ftree->trk_pfjetTrk_nValidTracker.push_back( null );
    ftree->trk_pfjetTrk_nValidPixelBarrel.push_back( null );
    ftree->trk_pfjetTrk_nValidPixelEndcap.push_back( null );
    ftree->trk_pfjetTrk_nValidStrip.push_back( null );

    ftree->trk_pfjetTrk_nMissed.push_back( null );
    ftree->trk_pfjetTrk_nMissedOut.push_back( null );
    ftree->trk_pfjetTrk_nMissedIn.push_back( null );
    ftree->trk_pfjetTrk_nMissedTrackerOut.push_back( null );
    ftree->trk_pfjetTrk_nMissedTrackerIn.push_back( null );
    ftree->trk_pfjetTrk_nMissedPixelBarrelOut.push_back( null );
    ftree->trk_pfjetTrk_nMissedPixelBarrelIn.push_back( null );
    ftree->trk_pfjetTrk_nMissedPixelEndcapOut.push_back( null );
    ftree->trk_pfjetTrk_nMissedPixelEndcapIn.push_back( null );

    ftree->trk_pfjetTrk_hasPixelBarrelLayer1.push_back( false );
    ftree->trk_pfjetTrk_hasPixelEndcapLayer1.push_back( false );
    ftree->trk_pfjetTrk_hasPixelBarrelLayer2.push_back( false );
    ftree->trk_pfjetTrk_hasPixelEndcapLayer2.push_back( false );
    ftree->trk_pfjetTrk_hasPixelBarrelLayer3.push_back( false );
    ftree->trk_pfjetTrk_hasPixelEndcapLayer3.push_back( false );
    ftree->trk_pfjetTrk_hasPixelBarrelLayer4.push_back( false );
    ftree->trk_pfjetTrk_hasPixelEndcapLayer4.push_back( false );

    ftree->trk_pfjetTrk_quality.push_back( null );
    ftree->trk_pfjetTrk_normalizedChi2.push_back( null );
    ftree->trk_pfjetTrk_ndof.push_back( null );
    ftree->trk_pfjetTrk_charge.push_back( null );
    ftree->trk_pfjetTrk_qoverp.push_back( null );
    ftree->trk_pfjetTrk_qoverpError.push_back( null );
    ftree->trk_pfjetTrk_theta.push_back( null );
    ftree->trk_pfjetTrk_thetaError.push_back( null );
    ftree->trk_pfjetTrk_lambda.push_back( null );
    ftree->trk_pfjetTrk_lambdaError.push_back( null );
    ftree->trk_pfjetTrk_ptError.push_back( null );
    ftree->trk_pfjetTrk_etaError.push_back( null );
    ftree->trk_pfjetTrk_phiError.push_back( null );

    ftree->trk_pfjetTrk_d0.push_back( null );
    ftree->trk_pfjetTrk_dz.push_back( null );
    ftree->trk_pfjetTrk_d0_pv.push_back( null );
    ftree->trk_pfjetTrk_dz_pv.push_back( null );
    ftree->trk_pfjetTrk_d0_bs.push_back( null );
    ftree->trk_pfjetTrk_d0_bs_zpca.push_back( null );
    ftree->trk_pfjetTrk_d0_bs_zpv.push_back( null );
    ftree->trk_pfjetTrk_dz_bs.push_back( null );
    ftree->trk_pfjetTrk_d0Err.push_back( null );
    ftree->trk_pfjetTrk_dzErr.push_back( null );
    ftree->trk_pfjetTrk_d0_pv_NoRefit.push_back( null );
    ftree->trk_pfjetTrk_dz_pv_NoRefit.push_back( null );
}*/
//   }

ftree->tree->Fill();
}   

void Residuals::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{
    bool changed = true;
    if( !hltConfig_.init(iRun, iSetup, "HLT", changed) )
        std::cout << "Warning, didn't find HLTConfigProvider with label "
            << "HLT" << " in run " << iRun.run() << std::endl;

    if( !hltPrescale_.init(iRun, iSetup, "HLT", changed) )
        std::cout << "Warning, didn't find HLTPrescaleProvider with label "
            << "HLT" << " in run " << iRun.run() << std::endl;
}

void Residuals::endRun() 
{
}

bool Residuals::trackSelection(const reco::Track& track) const 
{   
    using namespace reco;

    if( track.pt() < tkMinPt ) return false;
    //   if( track.hitPattern().trackerLayersWithMeasurement() < tkMinXLayers ) return false;
    //   if( track.trackerExpectedHitsOuter().numberOfLostHits() > tkMaxMissedOuterLayers ) return false;
    //   if( track.trackerExpectedHitsInner().numberOfLostHits() > tkMaxMissedInnerLayers ) return false;   
    if( ! track.quality(reco::TrackBase::highPurity) ) return false;
    //   if( ! (track.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::PixelBarrel, 1) ||
    //	  track.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::PixelEndcap, 1)) ) return false;

    return true;
}

bool Residuals::vertexSelection(const reco::Vertex& vertex) const
{
    if( vertex.tracksSize()>vtxTracksSizeMax || vertex.tracksSize()<vtxTracksSizeMin ) return false;
    //   if( vertex.xError() < vtxErrorXMin || vertex.xError() > vtxErrorXMax ) return false;
    //   if( vertex.yError() < vtxErrorYMin || vertex.yError() > vtxErrorYMax ) return false;
    //   if( vertex.zError() < vtxErrorZMin || vertex.zError() > vtxErrorZMax ) return false;

    return true;
}

float Residuals::getDeltaR(float eta1, float phi1, float eta2, float phi2)
{      
    float DeltaPhi = fabs(phi2 - phi1);
    if (DeltaPhi > 3.141593 ) DeltaPhi -= 2.*3.141593;
    return sqrt( (eta2-eta1)*(eta2-eta1) + DeltaPhi*DeltaPhi );
}

void Residuals::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Residuals);
