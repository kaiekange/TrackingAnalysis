#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include <limits>
#include <cmath>

#include "TTree.h"
#include "TRandom3.h"
#include "Compression.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

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

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
// #include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/Common/interface/Association.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include <TrackingTools/TrajectoryState/interface/PerigeeConversions.h>
#include <TrackingTools/TrajectoryState/interface/TrajectoryStateClosestToPoint.h>
#include <TrackingTools/PatternTools/interface/TSCPBuilderNoMaterial.h>

#include "TrackingAnalysis/EDAnalyzers/interface/VertexReProducer.h"
#include "TrackingAnalysis/EDAnalyzers/interface/Tree.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include <CommonTools/UtilAlgos/interface/TFileService.h>

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

namespace
{
    Bool_t sortPt(const reco::TransientTrack &t1,
                  const reco::TransientTrack &t2)
    {
        return t1.track().pt() > t2.track().pt();
    }
}

class Residuals : public edm::one::EDAnalyzer<>
{

public:
    explicit Residuals(const edm::ParameterSet &pset);
    ~Residuals();

    static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event &, const edm::EventSetup &) override;
    virtual void endJob() override;

    Bool_t trackSelection(const reco::Track &track) const;
    Bool_t vertexSelection(const reco::Vertex &vertex) const;

    class TrackEqual
    {
    public:
        TrackEqual(const edm::Ptr<reco::Track> &t) : track_(t) {}

        Bool_t operator()(const edm::Ptr<reco::Track> &t) const
        {
            return t->pt() == track_->pt();
        }

    private:
        const edm::Ptr<reco::Track> &track_;
    };

    class TrackEqualReco
    {
    public:
        TrackEqualReco(const reco::Track &t) : track_(t) {}

        Bool_t operator()(const reco::Track &t) const
        {
            return t.pt() == track_.pt();
        }

    private:
        const reco::Track &track_;
    };

    class TrackEqualRef
    {
    public:
        TrackEqualRef(const reco::TrackRef &t) : track_(t) {}

        Bool_t operator()(const reco::TrackRef &t) const
        {
            return t->pt() == track_->pt();
        }

    private:
        const reco::TrackRef &track_;
    };

    // class TrackEqualGen
    // {
    // public:
    //     explicit TrackEqualGen(reco::Track const &trk) : trk_(&trk) {}

    //     bool operator()(reco::GenParticle const &gp) const
    //     {
    //         if (gp.charge() != trk_->charge())
    //             return false;
    //         if ((abs(gp.pt() - trk_->pt()) / trk_->pt()) > 0.3)
    //             return false;
    //         if (reco::deltaR(gp.eta(), gp.phi(), trk_->eta(), trk_->phi()) > 0.1)
    //             return false;
    //         else
    //             return true;
    //     }

    // private:
    //     reco::Track const *trk_;
    // };

    class TrackEqualGen
    {
    public:
        explicit TrackEqualGen(reco::Track const &trk) : trk_(&trk) {}

        bool operator()(pat::PackedGenParticle const &gp) const
        {
            if (gp.charge() != trk_->charge())
                return false;
            if ((abs(gp.pt() - trk_->pt()) / trk_->pt()) > 0.3)
                return false;
            if (reco::deltaR(gp.eta(), gp.phi(), trk_->eta(), trk_->phi()) > 0.1)
                return false;
            else
                return true;
        }

    private:
        reco::Track const *trk_;
    };

    class TrackEqualPF
    {
    public:
        explicit TrackEqualPF(reco::Track const &trk) : trk_(&trk) {}

        bool operator()(pat::PackedCandidate const &pf) const
        {
            if (!pf.hasTrackDetails())
                return false;
            reco::Track pftrack = pf.pseudoTrack();
            if (pf.charge() != trk_->charge())
                return false;
            if (abs(pftrack.pt() - trk_->pt()) > 1e-6)
                return false;
            if (abs(pftrack.eta() - trk_->eta()) > 1e-6)
                return false;
            if (abs(pftrack.phi() - trk_->phi()) > 1e-6)
                return false;
            else
                return true;
        }

    private:
        reco::Track const *trk_;
    };

    class VertexEqual
    {
    public:
        VertexEqual(const reco::Vertex::Point &p) : p_(p) {}

        Bool_t operator()(const reco::Vertex::Point &p) const
        {
            return (p.x() == p_.x() && p.y() == p_.y() && p.z() == p_.z());
        }

    private:
        const reco::Vertex::Point &p_;
    };

    edm::EDGetTokenT<reco::VertexCollection> primvtxToken_;
    edm::EDGetTokenT<pat::PackedCandidateCollection> tracksToken_;
    edm::EDGetTokenT<reco::BeamSpot> beamspotToken_;
    edm::EDGetTokenT<edm::TriggerResults> triggerToken_;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>> PileupToken_;
    // edm::EDGetTokenT<reco::GenParticleCollection> GenPartToken_;
    edm::EDGetTokenT<edm::Association<reco::GenParticleCollection>> associationToken_;
    edm::EDGetTokenT<pat::PackedGenParticleCollection> GenPartToken_;

    // --- track selection variables
    Double_t tkMinPt;
    Int_t tkMinXLayers, tkMaxMissedOuterLayers, tkMaxMissedInnerLayers;

    // --- vertex selection variables
    UInt_t vtxTracksSizeMin;
    UInt_t vtxTracksSizeMax;
    //   Double_t vtxErrorXMin,vtxErrorXMax;
    //   Double_t vtxErrorYMin,vtxErrorYMax;
    //   Double_t vtxErrorZMin,vtxErrorZMax;

    std::string beamSpotConfig;

    VertexReProducer *revertex;

    Double_t micron = 10000;

    Int_t eventScale;
    Int_t eventModulo;
    std::string SampleType;

    TRandom3 *rnd;

    const edm::Service<TFileService> fs;
    ResTree *ftree;
    TTree *InfoTree;

    ULong_t nEventsProcessed_;
    ULong_t nEventsScaled_;
    ULong_t nEventsTriggered_;
};

Residuals::Residuals(const edm::ParameterSet &pset)
{
    edm::InputTag TrackCollectionTag_ = pset.getParameter<edm::InputTag>("TrackLabel");
    tracksToken_ = consumes<pat::PackedCandidateCollection>(TrackCollectionTag_);

    edm::InputTag VertexCollectionTag_ = pset.getParameter<edm::InputTag>("VertexLabel");
    primvtxToken_ = consumes<reco::VertexCollection>(VertexCollectionTag_);

    edm::InputTag BeamspotTag_ = edm::InputTag("offlineBeamSpot");
    beamspotToken_ = consumes<reco::BeamSpot>(BeamspotTag_);

    edm::InputTag TriggerBitsTag_ = pset.getParameter<edm::InputTag>("TriggerResultsLabel");
    triggerToken_ = consumes<edm::TriggerResults>(TriggerBitsTag_);

    edm::InputTag PileupTag("slimmedAddPileupInfo");
    PileupToken_ = consumes<std::vector<PileupSummaryInfo>>(PileupTag);

    // edm::InputTag GenPartTag("prunedGenParticles");
    // GenPartToken_ = consumes<reco::GenParticleCollection>(GenPartTag);

    edm::InputTag GenPartTag("packedGenParticles");
    GenPartToken_ = consumes<pat::PackedGenParticleCollection>(GenPartTag);

    edm::InputTag assotiationTag_("packedPFCandidateToGenAssociation");
    associationToken_ = consumes<edm::Association<reco::GenParticleCollection>>(assotiationTag_);

    beamSpotConfig = pset.getParameter<std::string>("BeamSpotConfig");

    tkMinPt = pset.getParameter<Double_t>("TkMinPt");
    tkMinXLayers = pset.getParameter<Int_t>("TkMinXLayers");
    tkMaxMissedOuterLayers = pset.getParameter<Int_t>("TkMaxMissedOuterLayers");
    tkMaxMissedInnerLayers = pset.getParameter<Int_t>("TkMaxMissedInnerLayers");

    vtxTracksSizeMin = pset.getParameter<Int_t>("VtxTracksSizeMin");
    vtxTracksSizeMax = pset.getParameter<Int_t>("VtxTracksSizeMax");
    // vtxErrorXMin     = pset.getParameter<Double_t>("VtxErrorXMin");
    // vtxErrorXMax     = pset.getParameter<Double_t>("VtxErrorXMax");
    // vtxErrorYMin     = pset.getParameter<Double_t>("VtxErrorYMin");
    // vtxErrorYMax     = pset.getParameter<Double_t>("VtxErrorYMax");
    // vtxErrorZMin     = pset.getParameter<Double_t>("VtxErrorZMin");
    // vtxErrorZMax     = pset.getParameter<Double_t>("VtxErrorZMax");

    edm::ConsumesCollector c{consumesCollector()};
    revertex = new VertexReProducer(pset, c);

    eventScale = pset.getParameter<Int_t>("EventScale");
    eventModulo = pset.getParameter<Int_t>("EventModulo");
    SampleType = pset.getParameter<std::string>("SampleType");

    rnd = new TRandom3();
}

Residuals::~Residuals() {}

void Residuals::beginJob()
{
    TFile &f = fs->file();
    f.SetCompressionAlgorithm(ROOT::kZSTD);
    f.SetCompressionLevel(5);
    ftree = new ResTree(fs->make<TTree>("tree", "tree"));
    ftree->CreateBranches();

    InfoTree = fs->make<TTree>("RunInfo", "RunInfo");
    InfoTree->Branch("nEventsProcessed_", &nEventsProcessed_);
    InfoTree->Branch("nEventsScaled_", &nEventsScaled_);
    InfoTree->Branch("nEventsTriggered_", &nEventsTriggered_);

    nEventsProcessed_ = 0;
    nEventsScaled_ = 0;
    nEventsTriggered_ = 0;
}

void Residuals::analyze(const edm::Event &iEvent, const edm::EventSetup &iSetup)
{
    nEventsProcessed_++;

    if ((nEventsProcessed_ - 1) % eventScale != static_cast<ULong_t>(eventModulo) && eventScale > 0)
        return;

    nEventsScaled_++;

    ftree->Init();

    edm::Handle<edm::TriggerResults> triggerHandle;
    iEvent.getByToken(triggerToken_, triggerHandle);
    const edm::TriggerNames &names = iEvent.triggerNames(*triggerHandle);
    for (UInt_t i = 0; i < names.size(); i++)
    {
        TString trigName = TString(names.triggerName(i));

        Bool_t pass = (triggerHandle->accept(i) ? true : false);

        if (trigName.Contains("HLT_ZeroBias_v"))
            ftree->trig_ZeroBias_pass = pass;

        else if (trigName.Contains("HLT_PFHT180_v"))
            ftree->trig_PFHT180_pass = pass;
        else if (trigName.Contains("HLT_PFHT250_v"))
            ftree->trig_PFHT250_pass = pass;
        else if (trigName.Contains("HLT_PFHT370_v"))
            ftree->trig_PFHT370_pass = pass;
        else if (trigName.Contains("HLT_PFHT430_v"))
            ftree->trig_PFHT430_pass = pass;
        else if (trigName.Contains("HLT_PFHT510_v"))
            ftree->trig_PFHT510_pass = pass;
        else if (trigName.Contains("HLT_PFHT590_v"))
            ftree->trig_PFHT590_pass = pass;
        else if (trigName.Contains("HLT_PFHT680_v"))
            ftree->trig_PFHT680_pass = pass;
        else if (trigName.Contains("HLT_PFHT780_v"))
            ftree->trig_PFHT780_pass = pass;
        else if (trigName.Contains("HLT_PFHT890_v"))
            ftree->trig_PFHT890_pass = pass;
        else if (trigName.Contains("HLT_PFHT1050_v"))
            ftree->trig_PFHT1050_pass = pass;
    }

    Bool_t trigger_pass = false;

    if (SampleType == "ZeroBias")
        trigger_pass = ftree->trig_ZeroBias_pass;
    else if (SampleType == "JetHT")
        trigger_pass = (ftree->trig_PFHT180_pass) || (ftree->trig_PFHT250_pass) || (ftree->trig_PFHT370_pass) || (ftree->trig_PFHT430_pass) || (ftree->trig_PFHT510_pass) || (ftree->trig_PFHT590_pass) || (ftree->trig_PFHT680_pass) || (ftree->trig_PFHT780_pass) || (ftree->trig_PFHT890_pass) || (ftree->trig_PFHT1050_pass);

    if (!trigger_pass)
        return;
    nEventsTriggered_++;

    // edm::Handle<std::vector<PileupSummaryInfo>> PileupInfo;
    // iEvent.getByToken(PileupToken_, PileupInfo);

    // for (std::vector<PileupSummaryInfo>::const_iterator iPU = PileupInfo->begin(); iPU != PileupInfo->end(); iPU++)
    // {
    //     Int_t BX = iPU->getBunchCrossing();
    //     if (BX == 0)
    //     {
    //         ftree->NumTrueInts = iPU->getTrueNumInteractions();
    //         ftree->NumPUInts = iPU->getPU_NumInteractions();
    //     }
    // }

    edm::Handle<reco::VertexCollection> primvtxHandle;
    iEvent.getByToken(primvtxToken_, primvtxHandle);

    edm::Handle<reco::BeamSpot> beamspotHandle;
    iEvent.getByToken(beamspotToken_, beamspotHandle);

    if (!primvtxHandle.isValid())
        return;
    if (primvtxHandle->size() == 0)
        return;

    edm::Handle<edm::Association<reco::GenParticleCollection>> associationHandle;
    iEvent.getByToken(associationToken_, associationHandle);

    edm::Handle<pat::PackedCandidateCollection> tracksHandle;
    iEvent.getByToken(tracksToken_, tracksHandle);
    reco::TrackCollection tracks;
    for (const pat::PackedCandidate &pf : *tracksHandle)
    {
        if (!pf.hasTrackDetails())
            continue;
        tracks.push_back(pf.pseudoTrack());
    }

    Int_t hasGen = 0;
    for (size_t i = 0; i < tracksHandle->size(); ++i)
    {
        const auto &pf = (*tracksHandle)[i];

        // int pvAssoc = pf.fromPV(); // 0=NoPV, 1=PVLoose, 2=PVTight, 3=PVUsedInFit
        // if (pvAssoc > 0)
        // {
        //     fromPV++;

        edm::Ref<pat::PackedCandidateCollection> pfRef(tracksHandle, i);
        reco::GenParticleRef genRef = (*associationHandle)[pfRef];

        if (genRef.isNonnull())
            hasGen++;
    }

    float rate = float(hasGen) / float(tracksHandle->size());

    std::vector<TransientVertex> refitted_tPVs = revertex->makeVertices(tracks, *beamspotHandle, iSetup);

    if (refitted_tPVs.empty())
        return;

    TransientVertex refitted_tPV_front = refitted_tPVs.front();
    reco::Vertex refitted_PV_front = reco::Vertex(refitted_tPVs.front());

    if (!vertexSelection(refitted_PV_front))
        return;

    ftree->ev_nPV = refitted_tPVs.size();

    std::vector<reco::TransientTrack> vtxTracks = refitted_tPV_front.originalTracks();

    edm::Handle<pat::PackedGenParticleCollection> GenPartHandle;
    iEvent.getByToken(GenPartToken_, GenPartHandle);

    // std::cout << "Size of the packedGenParticle: " << GenPartHandle->size() << std::endl;

    // for(const pat::PackedGenParticle & gp : *GenPartHandle)
    // {
    //     std::cout << "pdgId: " << gp.pdgId() << std::endl;
    // }

    edm::LogPrint("Residuals") << "\nAll PFCandidate tracks size = " << tracks.size() << ", " << rate * 100 << "\% have Gen match, refitted PV tracks size = " << vtxTracks.size() << ", PackedGenParticle size = " << GenPartHandle->size();

    stable_sort(vtxTracks.begin(), vtxTracks.end(), sortPt);

    Int_t nTracks = refitted_PV_front.tracksSize();

    reco::TrackCollection initPVTkCollection;

    Int_t newhasGen = 0;
    TString foundPF = "no";
    for (const reco::TransientTrack &tt : vtxTracks)
    {
        reco::Track trk = tt.track();
        initPVTkCollection.push_back(trk);
        pat::PackedCandidateCollection::const_iterator itt = find_if(tracksHandle->begin(), tracksHandle->end(), TrackEqualPF(trk));
        if (itt == tracksHandle->end())
            continue;
        foundPF = "yes";
        size_t iidx = itt - tracksHandle->begin();
        edm::Ref<pat::PackedCandidateCollection> newpfRef(tracksHandle, iidx);
        reco::GenParticleRef newgenRef = (*associationHandle)[newpfRef];

        if (newgenRef.isNonnull())
            newhasGen++;
    }
    edm::LogPrint("Residuals") << "Refitted PV tracks size = " << vtxTracks.size() << ", found the original PFCandidate?: " << foundPF << ", PF with a match = " << newhasGen << ", ratio: " << Float_t(newhasGen) / GenPartHandle->size() * 100 << "\%\n";
}

void Residuals::endJob() {}

Bool_t Residuals::trackSelection(const reco::Track &track) const
{
    using namespace reco;

    if (track.pt() < tkMinPt)
        return false;
    //   if( track.hitPattern().trackerLayersWithMeasurement() < tkMinXLayers ) return false;
    //   if( track.trackerExpectedHitsOuter().numberOfLostHits() > tkMaxMissedOuterLayers ) return false;
    //   if( track.trackerExpectedHitsInner().numberOfLostHits() > tkMaxMissedInnerLayers ) return false;
    if (!track.quality(reco::TrackBase::highPurity))
        return false;
    //   if( ! (track.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::PixelBarrel, 1) ||
    //	  track.hitPattern().hasValidHitInPixelLayer(PixelSubdetector::PixelEndcap, 1)) ) return false;

    return true;
}

Bool_t Residuals::vertexSelection(const reco::Vertex &vertex) const
{
    if (vertex.tracksSize() > vtxTracksSizeMax || vertex.tracksSize() < vtxTracksSizeMin)
        return false;
    //   if( vertex.xError() < vtxErrorXMin || vertex.xError() > vtxErrorXMax ) return false;
    //   if( vertex.yError() < vtxErrorYMin || vertex.yError() > vtxErrorYMax ) return false;
    //   if( vertex.zError() < vtxErrorZMin || vertex.zError() > vtxErrorZMax ) return false;

    return true;
}

void Residuals::fillDescriptions(edm::ConfigurationDescriptions &descriptions)
{
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

// define this as a plug-in
DEFINE_FWK_MODULE(Residuals);