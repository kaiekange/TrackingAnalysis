#include <memory>
#include <vector>
#include <map>
#include <tuple>
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

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

#include "FWCore/Utilities/interface/ESGetToken.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

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

    Bool_t vertexSelection(const reco::Vertex &vertex) const;

    edm::EDGetTokenT<reco::VertexCollection> primvtxToken_;
    edm::EDGetTokenT<reco::BeamSpot> beamspotToken_;
    edm::EDGetTokenT<edm::TriggerResults> triggerToken_;
    edm::EDGetTokenT<pat::PackedCandidateCollection> PFCandToken_;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>> PileupToken_;

    // --- vertex selection variables
    UInt_t vtxTracksSizeMin;
    UInt_t vtxTracksSizeMax;

    std::unique_ptr<VertexReProducer> revertex;

    Double_t micron = 10000;

    Bool_t runOnData;
    Int_t eventScale;
    Int_t eventModulo;
    std::string SampleType;

    std::unique_ptr<TRandom3> rnd;

    const edm::Service<TFileService> fs;
    ResTree *ftree;
    TTree *InfoTree;

    ULong_t nEventsProcessed_;
    ULong_t nEventsScaled_;
    ULong_t nEventsTriggered_;
};

Residuals::Residuals(const edm::ParameterSet &pset)
{
    edm::InputTag PFCandTag_("packedPFCandidates");
    PFCandToken_ = consumes<pat::PackedCandidateCollection>(PFCandTag_);

    edm::InputTag primvtxTag_("offlineSlimmedPrimaryVertices");
    primvtxToken_ = consumes<reco::VertexCollection>(primvtxTag_);

    edm::InputTag BeamspotTag_("offlineBeamSpot");
    beamspotToken_ = consumes<reco::BeamSpot>(BeamspotTag_);

    edm::InputTag triggerTag_("TriggerResults", "", "HLT");
    triggerToken_ = consumes<edm::TriggerResults>(triggerTag_);

    edm::InputTag PileupTag_("slimmedAddPileupInfo");
    PileupToken_ = consumes<std::vector<PileupSummaryInfo>>(PileupTag_);

    vtxTracksSizeMin = pset.getParameter<Int_t>("VtxTracksSizeMin");
    vtxTracksSizeMax = pset.getParameter<Int_t>("VtxTracksSizeMax");

    edm::ConsumesCollector c{consumesCollector()};
    revertex = std::make_unique<VertexReProducer>(pset, c);

    runOnData = pset.getParameter<Bool_t>("RunOnData");
    eventScale = pset.getParameter<Int_t>("EventScale");
    eventModulo = pset.getParameter<Int_t>("EventModulo");
    SampleType = pset.getParameter<std::string>("SampleType");

    rnd = std::make_unique<TRandom3>();
}

Residuals::~Residuals() {}

void Residuals::beginJob()
{
    TFile &f = fs->file();
    f.SetCompressionAlgorithm(ROOT::kZSTD);
    f.SetCompressionLevel(5);
    ftree = new ResTree(fs->make<TTree>("tree", "tree"));
    ftree->CreateBranches(runOnData);

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

    if (eventScale > 0 && (nEventsProcessed_ - 1) % eventScale != static_cast<ULong_t>(eventModulo))
        return;

    nEventsScaled_++;

    ftree->Init();

    ftree->ev_run  = iEvent.id().run();
    ftree->ev_lumi = iEvent.id().luminosityBlock();
    ftree->ev_id   = iEvent.id().event();

    //==================================================//
    //--------------------------------------------------//
    // Triggers
    //--------------------------------------------------//
    //==================================================//
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

    //==================================================//
    //--------------------------------------------------//
    // Pileup
    //--------------------------------------------------//
    //==================================================//
    if (!runOnData)
    {
        edm::Handle<std::vector<PileupSummaryInfo>> PileupInfo;
        iEvent.getByToken(PileupToken_, PileupInfo);

        for (const PileupSummaryInfo &iPU : *PileupInfo)
        {
            Int_t BX = iPU.getBunchCrossing();
            if (BX == 0)
            {
                ftree->NumTrueInts = iPU.getTrueNumInteractions();
            }
        }
    }

    //==================================================//
    //--------------------------------------------------//
    // primary vertex and beamspot
    //--------------------------------------------------//
    //==================================================//
    edm::Handle<reco::VertexCollection> primvtxHandle;
    iEvent.getByToken(primvtxToken_, primvtxHandle);

    edm::Handle<reco::BeamSpot> beamspotHandle;
    iEvent.getByToken(beamspotToken_, beamspotHandle);

    if (!primvtxHandle.isValid())
        return;
    if (primvtxHandle->size() == 0)
        return;

    //==================================================//
    //--------------------------------------------------//
    // Resolution
    //--------------------------------------------------//
    //==================================================//

    // select tracks
    edm::Handle<pat::PackedCandidateCollection> PFCandHandle;
    iEvent.getByToken(PFCandToken_, PFCandHandle);

    // pvAssociationQuality of the PF candidates, keyed by track parameters (first candidate wins)
    std::map<std::tuple<Int_t, Double_t, Double_t, Double_t>, Int_t> pvAssocQuality;
    reco::TrackCollection tracks;
    for (const pat::PackedCandidate &pf : *PFCandHandle)
    {
        if (!pf.hasTrackDetails())
            continue;
        const reco::Track &pseudoTrack = pf.pseudoTrack();
        pvAssocQuality.emplace(std::make_tuple(pf.charge(), pseudoTrack.pt(), pseudoTrack.eta(), pseudoTrack.phi()), pf.pvAssociationQuality());
        if (pf.pvAssociationQuality() < 2)
            continue;
        tracks.push_back(pseudoTrack);
    }

    // PV refitting
    std::vector<TransientVertex> refitted_tPVs = revertex->makeVertices(tracks, *beamspotHandle, iSetup);

    if (refitted_tPVs.empty())
        return;

    TransientVertex refitted_tPV_front = refitted_tPVs.front();
    reco::Vertex refitted_PV_front = reco::Vertex(refitted_tPVs.front());

    if (!vertexSelection(refitted_PV_front))
        return;

    ftree->ev_nPV = refitted_tPVs.size();

    // Sort tracks by pt; these transient tracks (beam spot already set) are reused for all refits below
    std::vector<reco::TransientTrack> vtxTracks = refitted_tPV_front.originalTracks();
    stable_sort(vtxTracks.begin(), vtxTracks.end(), sortPt);

    Int_t nTracks = refitted_PV_front.tracksSize();

    //--------------------------------------------------//
    // Beam spot
    //--------------------------------------------------//
    const reco::BeamSpot &bs = *beamspotHandle;
    ftree->bs_type = bs.type();
    ftree->bs_x0 = bs.x0() * micron;
    ftree->bs_y0 = bs.y0() * micron;
    ftree->bs_z0 = bs.z0() * micron;
    ftree->bs_x_zpv = bs.x(refitted_PV_front.z()) * micron;
    ftree->bs_y_zpv = bs.y(refitted_PV_front.z()) * micron;
    ftree->bs_sigmaZ = bs.sigmaZ() * micron;
    ftree->bs_dxdz = bs.dxdz();
    ftree->bs_dydz = bs.dydz();
    ftree->bs_BeamWidthX = bs.BeamWidthX() * micron;
    ftree->bs_BeamWidthY = bs.BeamWidthY() * micron;
    ftree->bs_x0Error = bs.x0Error() * micron;
    ftree->bs_y0Error = bs.y0Error() * micron;
    ftree->bs_z0Error = bs.z0Error() * micron;
    ftree->bs_sigmaZ0Error = bs.sigmaZ0Error() * micron;
    ftree->bs_dxdzError = bs.dxdzError();
    ftree->bs_dydzError = bs.dydzError();
    ftree->bs_BeamWidthXError = bs.BeamWidthXError() * micron;
    ftree->bs_BeamWidthYError = bs.BeamWidthYError() * micron;

    //--------------------------------------------------//
    // IP resolution
    //--------------------------------------------------//
    Float_t pv_SumTrackPt = 0.;
    Float_t pv_SumTrackPt2 = 0.;

    for (size_t iTrk = 0; iTrk < vtxTracks.size(); iTrk++)
    {
        const reco::Track &trk = vtxTracks[iTrk].track();

        auto quality = pvAssocQuality.find(std::make_tuple(trk.charge(), trk.pt(), trk.eta(), trk.phi()));
        ftree->pv_trk_pvAssociationQuality.push_back(quality != pvAssocQuality.end() ? quality->second : null);

        pv_SumTrackPt += trk.pt();
        pv_SumTrackPt2 += trk.pt() * trk.pt();

        const reco::HitPattern &hits = trk.hitPattern();
        ftree->pv_trk_isHighPurity.push_back(trk.quality(reco::TrackBase::highPurity));
        ftree->pv_trk_charge.push_back(trk.charge());
        ftree->pv_trk_nTrackerLayers.push_back(hits.trackerLayersWithMeasurement());
        ftree->pv_trk_nPixelBarrelLayers.push_back(hits.pixelBarrelLayersWithMeasurement());
        ftree->pv_trk_nPixelEndcapLayers.push_back(hits.pixelEndcapLayersWithMeasurement());
        ftree->pv_trk_nValidPixelHits.push_back(hits.numberOfValidPixelHits());
        ftree->pv_trk_nMissedIn.push_back(hits.numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS));
        ftree->pv_trk_hasPixelBarrelLayer1.push_back(hits.hasValidHitInPixelLayer(PixelSubdetector::PixelBarrel, 1));
        ftree->pv_trk_hasPixelEndcapLayer1.push_back(hits.hasValidHitInPixelLayer(PixelSubdetector::PixelEndcap, 1));

        ftree->pv_trk_pt.push_back(trk.pt());
        ftree->pv_trk_eta.push_back(trk.eta());
        ftree->pv_trk_phi.push_back(trk.phi());

        ftree->pv_trk_d0Err.push_back(trk.d0Error() * micron);
        ftree->pv_trk_dzErr.push_back(trk.dzError() * micron);

        // Refit the PV without this track
        std::vector<reco::TransientTrack> unbiasedTracks;
        unbiasedTracks.reserve(vtxTracks.size() - 1);
        unbiasedTracks.insert(unbiasedTracks.end(), vtxTracks.begin(), vtxTracks.begin() + iTrk);
        unbiasedTracks.insert(unbiasedTracks.end(), vtxTracks.begin() + iTrk + 1, vtxTracks.end());

        std::vector<TransientVertex> refitted_tPVs_unbiased = revertex->makeVertices(unbiasedTracks, *beamspotHandle);

        if (!refitted_tPVs_unbiased.empty())
        {
            reco::Vertex refitted_PV_front_unbiased = reco::Vertex(refitted_tPVs_unbiased.front());

            Float_t unbiasedSumTrackPt2 = 0.;
            for (const reco::TransientTrack &tt : unbiasedTracks)
                unbiasedSumTrackPt2 += tt.track().pt() * tt.track().pt();

            ftree->pv_trk_pvunbiased_IsValid.push_back(refitted_PV_front_unbiased.isValid());
            ftree->pv_trk_pvunbiased_IsFake.push_back(refitted_PV_front_unbiased.isFake());
            ftree->pv_trk_pvunbiased_SumTrackPt2.push_back(unbiasedSumTrackPt2);
            ftree->pv_trk_pvunbiased_xError.push_back(refitted_PV_front_unbiased.xError() * micron);
            ftree->pv_trk_pvunbiased_yError.push_back(refitted_PV_front_unbiased.yError() * micron);
            ftree->pv_trk_pvunbiased_zError.push_back(refitted_PV_front_unbiased.zError() * micron);
            ftree->pv_trk_pvunbiased_covXY.push_back(refitted_PV_front_unbiased.covariance(0, 1) * micron * micron);
            ftree->pv_trk_pvunbiased_covXZ.push_back(refitted_PV_front_unbiased.covariance(0, 2) * micron * micron);
            ftree->pv_trk_pvunbiased_covYZ.push_back(refitted_PV_front_unbiased.covariance(1, 2) * micron * micron);

            reco::Track::Point vtxPositionUnbiased = reco::Track::Point(refitted_PV_front_unbiased.position().x(), refitted_PV_front_unbiased.position().y(), refitted_PV_front_unbiased.position().z());

            ftree->pv_trk_d0_pvunbiased.push_back(trk.dxy(vtxPositionUnbiased) * micron);
            ftree->pv_trk_dz_pvunbiased.push_back(trk.dz(vtxPositionUnbiased) * micron);
        }
        else
        {
            ftree->pv_trk_pvunbiased_IsValid.push_back(false);
            ftree->pv_trk_pvunbiased_IsFake.push_back(true);
            ftree->pv_trk_pvunbiased_SumTrackPt2.push_back(null);
            ftree->pv_trk_pvunbiased_xError.push_back(null);
            ftree->pv_trk_pvunbiased_yError.push_back(null);
            ftree->pv_trk_pvunbiased_zError.push_back(null);
            ftree->pv_trk_pvunbiased_covXY.push_back(null);
            ftree->pv_trk_pvunbiased_covXZ.push_back(null);
            ftree->pv_trk_pvunbiased_covYZ.push_back(null);

            ftree->pv_trk_d0_pvunbiased.push_back(null);
            ftree->pv_trk_dz_pvunbiased.push_back(null);
        }
    }

    ftree->pv_IsValid = refitted_PV_front.isValid();
    ftree->pv_IsFake = refitted_PV_front.isFake();
    ftree->pv_NTracks = nTracks;
    ftree->pv_SumTrackPt = pv_SumTrackPt;
    ftree->pv_SumTrackPt2 = pv_SumTrackPt2;
    ftree->pv_chi2 = refitted_PV_front.chi2();
    ftree->pv_ndof = refitted_PV_front.ndof();
    ftree->pv_x = refitted_PV_front.x() * micron;
    ftree->pv_y = refitted_PV_front.y() * micron;
    ftree->pv_z = refitted_PV_front.z() * micron;
    ftree->pv_xError = refitted_PV_front.xError() * micron;
    ftree->pv_yError = refitted_PV_front.yError() * micron;
    ftree->pv_zError = refitted_PV_front.zError() * micron;

    //--------------------------------------------------//
    // PV resolution
    //--------------------------------------------------//

    // split PV tracks; seed from the event id so the split does not depend on how events are grouped into jobs
    ULong64_t seed = (ULong64_t(iEvent.id().run()) << 32) ^ (ULong64_t(iEvent.id().luminosityBlock()) << 16) ^ iEvent.id().event();
    seed ^= seed >> 33;
    seed *= 0xff51afd7ed558ccdULL;
    seed ^= seed >> 33;
    seed *= 0xc4ceb9fe1a85ec53ULL;
    seed ^= seed >> 33;
    rnd->SetSeed(UInt_t(seed) ? UInt_t(seed) : 1);  // TRandom3 uses 32 bits; seed 0 would mean a time-based seed

    std::vector<reco::TransientTrack> vtxTracks1;
    std::vector<reco::TransientTrack> vtxTracks2;

    Float_t SumTrackPt2_p1 = 0;
    Float_t SumTrackPt2_p2 = 0;

    for (const reco::TransientTrack &tt : vtxTracks)
    {
        const reco::Track &trk = tt.track();

        if (rnd->Rndm() > 0.5)
        {
            vtxTracks1.push_back(tt);
            SumTrackPt2_p1 += trk.pt() * trk.pt();
        }
        else
        {
            vtxTracks2.push_back(tt);
            SumTrackPt2_p2 += trk.pt() * trk.pt();
        }
    }

    std::vector<TransientVertex> refitted_tPVs1 = revertex->makeVertices(vtxTracks1, *beamspotHandle);
    std::vector<TransientVertex> refitted_tPVs2 = revertex->makeVertices(vtxTracks2, *beamspotHandle);

    if (!refitted_tPVs1.empty() && !refitted_tPVs2.empty())
    {
        reco::Vertex vtx1 = reco::Vertex(refitted_tPVs1.front());
        ftree->pv_IsValid_p1 = vtx1.isValid();
        ftree->pv_IsFake_p1 = vtx1.isFake();
        ftree->pv_SumTrackPt2_p1 = SumTrackPt2_p1;
        ftree->pv_x_p1 = vtx1.x() * micron;
        ftree->pv_y_p1 = vtx1.y() * micron;
        ftree->pv_z_p1 = vtx1.z() * micron;
        ftree->pv_xError_p1 = vtx1.xError() * micron;
        ftree->pv_yError_p1 = vtx1.yError() * micron;
        ftree->pv_zError_p1 = vtx1.zError() * micron;
        ftree->pv_covXY_p1 = vtx1.covariance(0, 1) * micron * micron;
        ftree->pv_covXZ_p1 = vtx1.covariance(0, 2) * micron * micron;
        ftree->pv_covYZ_p1 = vtx1.covariance(1, 2) * micron * micron;

        reco::Vertex vtx2 = reco::Vertex(refitted_tPVs2.front());
        ftree->pv_IsValid_p2 = vtx2.isValid();
        ftree->pv_IsFake_p2 = vtx2.isFake();
        ftree->pv_SumTrackPt2_p2 = SumTrackPt2_p2;
        ftree->pv_x_p2 = vtx2.x() * micron;
        ftree->pv_y_p2 = vtx2.y() * micron;
        ftree->pv_z_p2 = vtx2.z() * micron;
        ftree->pv_xError_p2 = vtx2.xError() * micron;
        ftree->pv_yError_p2 = vtx2.yError() * micron;
        ftree->pv_zError_p2 = vtx2.zError() * micron;
        ftree->pv_covXY_p2 = vtx2.covariance(0, 1) * micron * micron;
        ftree->pv_covXZ_p2 = vtx2.covariance(0, 2) * micron * micron;
        ftree->pv_covYZ_p2 = vtx2.covariance(1, 2) * micron * micron;
    }

    ftree->tree->Fill();
}

void Residuals::endJob()
{
    edm::LogInfo("Residuals") << "Processed all events.";
    edm::LogPrint("Residuals") << "Total number of events before event scale: " << nEventsProcessed_;
    edm::LogPrint("Residuals") << "Total number of events after event scale: " << nEventsScaled_;
    edm::LogPrint("Residuals") << "Total number of triggered events: " << nEventsTriggered_;

    InfoTree->Fill();
}

Bool_t Residuals::vertexSelection(const reco::Vertex &vertex) const
{
    if (vertex.tracksSize() > vtxTracksSizeMax || vertex.tracksSize() < vtxTracksSizeMin)
        return false;

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