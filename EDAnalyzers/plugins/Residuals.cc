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

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

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

    edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magFieldToken_;
    edm::EDGetTokenT<reco::VertexCollection> primvtxToken_;
    edm::EDGetTokenT<edm::View<pat::PackedCandidate>> tracksToken_;
    edm::EDGetTokenT<reco::BeamSpot> beamspotToken_;
    edm::EDGetTokenT<edm::TriggerResults> triggerToken_;
    edm::EDGetTokenT<std::vector<PileupSummaryInfo>> PileupToken;

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

    Bool_t runOnData;
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

Residuals::Residuals(const edm::ParameterSet &pset) : magFieldToken_(esConsumes<MagneticField, IdealMagneticFieldRecord>())
{
    edm::InputTag TrackCollectionTag_ = pset.getParameter<edm::InputTag>("TrackLabel");
    tracksToken_ = consumes<edm::View<pat::PackedCandidate>>(TrackCollectionTag_);

    edm::InputTag VertexCollectionTag_ = pset.getParameter<edm::InputTag>("VertexLabel");
    primvtxToken_ = consumes<reco::VertexCollection>(VertexCollectionTag_);

    edm::InputTag BeamspotTag_ = edm::InputTag("offlineBeamSpot");
    beamspotToken_ = consumes<reco::BeamSpot>(BeamspotTag_);

    edm::InputTag TriggerBitsTag_ = pset.getParameter<edm::InputTag>("TriggerResultsLabel");
    triggerToken_ = consumes<edm::TriggerResults>(TriggerBitsTag_);

    edm::InputTag PileupTag("slimmedAddPileupInfo");
    PileupToken = consumes<std::vector<PileupSummaryInfo>>(PileupTag);

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

    runOnData = pset.getParameter<Bool_t>("RunOnData");
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


    if (!runOnData)
    {
        edm::Handle<std::vector<PileupSummaryInfo>> PileupInfo;
        iEvent.getByToken(PileupToken, PileupInfo);

        for (std::vector<PileupSummaryInfo>::const_iterator iPU = PileupInfo->begin(); iPU != PileupInfo->end(); iPU++)
        {
            Int_t BX = iPU->getBunchCrossing();
            if (BX == 0)
            {
                ftree->NumTrueInts = iPU->getTrueNumInteractions();
                ftree->NumPUInts = iPU->getPU_NumInteractions();
            }
        }
    }

    edm::Handle<reco::VertexCollection> primvtxHandle;
    iEvent.getByToken(primvtxToken_, primvtxHandle);

    edm::Handle<reco::BeamSpot> beamspotHandle;
    iEvent.getByToken(beamspotToken_, beamspotHandle);

    if (!primvtxHandle.isValid())
        return;
    if (primvtxHandle->size() == 0)
        return;

    edm::Handle<edm::View<pat::PackedCandidate>> tracksHandle;
    iEvent.getByToken(tracksToken_, tracksHandle);
    edm::View<pat::PackedCandidate> tracksPacked = (*tracksHandle.product());
    reco::TrackCollection tracks;
    for (size_t it = 0; it < tracksPacked.size(); it++)
    {
        const pat::PackedCandidate &trkPacked = tracksPacked[it];
        if (!trkPacked.hasTrackDetails())
            continue;
        tracks.push_back(trkPacked.pseudoTrack());
    }

    std::vector<TransientVertex> refitted_tPVs = revertex->makeVertices(tracks, *beamspotHandle, iSetup);

    edm::LogPrint("Residuals") << "Primary vertices = " << primvtxHandle->size() << ", refitted vertices = " << refitted_tPVs.size() << ", tracks = " << tracks.size();
    if (refitted_tPVs.empty())
        return;

    TransientVertex refitted_tPV_front = refitted_tPVs.front();
    reco::Vertex refitted_PV_front = reco::Vertex(refitted_tPVs.front());

    if (!vertexSelection(refitted_PV_front))
        return;

    edm::ESHandle<MagneticField> magField = iSetup.getHandle(magFieldToken_);

    ftree->ev_nPV = refitted_tPVs.size();

    std::vector<reco::TransientTrack> vtxTracks = refitted_tPV_front.originalTracks();
    stable_sort(vtxTracks.begin(), vtxTracks.end(), sortPt);

    Int_t nTracks = refitted_PV_front.tracksSize();

    reco::Track::Point vtxPosition = reco::Track::Point(refitted_PV_front.position().x(), refitted_PV_front.position().y(), refitted_PV_front.position().z());

    Float_t pv_SumTrackPt = 0.;
    Float_t pv_SumTrackPt2 = 0.;
    Float_t pv_fracHighPurity = 0.;

    reco::TrackCollection initPVTkCollection;
    for (std::vector<reco::TransientTrack>::const_iterator it = vtxTracks.begin(); it != vtxTracks.end(); it++)
    {
        reco::Track trk = (*it).track();
        initPVTkCollection.push_back(trk);
    }

    Int_t iTrk = 0;
    for (std::vector<reco::TransientTrack>::const_iterator it = vtxTracks.begin(); it != vtxTracks.end(); it++)
    {
        reco::Track trk = (*it).track();

        pv_SumTrackPt += trk.pt();
        pv_SumTrackPt2 += trk.pt() * trk.pt();
        pv_fracHighPurity += trk.quality(reco::TrackBase::highPurity);

        if (refitted_tPV_front.hasTrackWeight())
            ftree->pv_trk_weight.push_back(refitted_tPV_front.trackWeight(*it));
        else
            ftree->pv_trk_weight.push_back(null);

        ftree->pv_trk_isHighPurity.push_back(trk.quality(reco::TrackBase::highPurity));
        ftree->pv_trk_algo.push_back(trk.algo());
        ftree->pv_trk_originalAlgo.push_back(trk.originalAlgo());

        ftree->pv_trk_pt.push_back(trk.pt());
        ftree->pv_trk_px.push_back(trk.px());
        ftree->pv_trk_py.push_back(trk.py());
        ftree->pv_trk_pz.push_back(trk.pz());
        ftree->pv_trk_p.push_back(trk.p());
        ftree->pv_trk_eta.push_back(trk.eta());
        ftree->pv_trk_phi.push_back(trk.phi());

        ftree->pv_trk_d0.push_back(trk.dxy() * micron);
        ftree->pv_trk_dz.push_back(trk.dz() * micron);
        ftree->pv_trk_d0_pv.push_back(trk.dxy(vtxPosition) * micron);
        ftree->pv_trk_dz_pv.push_back(trk.dz(vtxPosition) * micron);
        ftree->pv_trk_d0_bs.push_back(trk.dxy(beamspotHandle->position()) * micron);
        ftree->pv_trk_d0_bs_zpca.push_back(trk.dxy(*beamspotHandle) * micron);
        ftree->pv_trk_d0_bs_zpv.push_back(trk.dxy(beamspotHandle->position(vtxPosition.z())) * micron);
        ftree->pv_trk_dz_bs.push_back(trk.dz(beamspotHandle->position()) * micron);
        ftree->pv_trk_d0Err.push_back(trk.d0Error() * micron);
        ftree->pv_trk_dzErr.push_back(trk.dzError() * micron);

        // Remove the track from the PV track collection
        reco::TrackCollection newPVTkCollection;
        newPVTkCollection.assign(initPVTkCollection.begin(), initPVTkCollection.begin() + iTrk);
        newPVTkCollection.insert(newPVTkCollection.end(), initPVTkCollection.begin() + iTrk + 1, initPVTkCollection.end());

        std::vector<TransientVertex> refitted_tPVs_unbiased = revertex->makeVertices(newPVTkCollection, *beamspotHandle, iSetup);

        ftree->pv_trk_pvN.push_back(refitted_tPVs_unbiased.size());

        if (!refitted_tPVs_unbiased.empty())
        {
            reco::Vertex refitted_PV_front_unbiased = reco::Vertex(refitted_tPVs_unbiased.front());

            Float_t unbiasedSumTrackPt = 0.;
            Float_t unbiasedSumTrackPt2 = 0.;
            Float_t unbiasedFracHighPurity = 0.;

            for (reco::TrackCollection::const_iterator itt = newPVTkCollection.begin(); itt != newPVTkCollection.end(); itt++)
            {
                unbiasedSumTrackPt += (*itt).pt();
                unbiasedSumTrackPt2 += (*itt).pt() * (*itt).pt();
                unbiasedFracHighPurity += (*itt).quality(reco::TrackBase::highPurity);
            }
            Int_t nTracksUnbiased = refitted_PV_front_unbiased.tracksSize();
            if (nTracksUnbiased)
                unbiasedFracHighPurity /= Float_t(nTracksUnbiased);

            ftree->pv_trk_pvunbiased_IsValid.push_back(refitted_PV_front_unbiased.isValid());
            ftree->pv_trk_pvunbiased_IsFake.push_back(refitted_PV_front_unbiased.isFake());
            ftree->pv_trk_pvunbiased_NTracks.push_back(nTracksUnbiased);
            ftree->pv_trk_pvunbiased_SumTrackPt.push_back(unbiasedSumTrackPt);
            ftree->pv_trk_pvunbiased_SumTrackPt2.push_back(unbiasedSumTrackPt2);
            ftree->pv_trk_pvunbiased_fracHighPurity.push_back(unbiasedFracHighPurity);
            ftree->pv_trk_pvunbiased_chi2.push_back(refitted_PV_front_unbiased.chi2());
            ftree->pv_trk_pvunbiased_ndof.push_back(refitted_PV_front_unbiased.ndof());
            ftree->pv_trk_pvunbiased_x.push_back(refitted_PV_front_unbiased.x() * micron);
            ftree->pv_trk_pvunbiased_y.push_back(refitted_PV_front_unbiased.y() * micron);
            ftree->pv_trk_pvunbiased_z.push_back(refitted_PV_front_unbiased.z() * micron);
            ftree->pv_trk_pvunbiased_xError.push_back(refitted_PV_front_unbiased.xError() * micron);
            ftree->pv_trk_pvunbiased_yError.push_back(refitted_PV_front_unbiased.yError() * micron);
            ftree->pv_trk_pvunbiased_zError.push_back(refitted_PV_front_unbiased.zError() * micron);

            reco::Track::Point vtxPositionUnbiased = reco::Track::Point(refitted_PV_front_unbiased.position().x(), refitted_PV_front_unbiased.position().y(), refitted_PV_front_unbiased.position().z());

            ftree->pv_trk_d0_pvunbiased.push_back(trk.dxy(vtxPositionUnbiased) * micron);
            ftree->pv_trk_dz_pvunbiased.push_back(trk.dz(vtxPositionUnbiased) * micron);
            ftree->pv_trk_d0_bs_zpvunbiased.push_back(trk.dxy(beamspotHandle->position(vtxPositionUnbiased.z())) * micron);
        }
        else
        {
            ftree->pv_trk_pvunbiased_IsValid.push_back(false);
            ftree->pv_trk_pvunbiased_IsFake.push_back(true);
            ftree->pv_trk_pvunbiased_NTracks.push_back(null);
            ftree->pv_trk_pvunbiased_SumTrackPt.push_back(null);
            ftree->pv_trk_pvunbiased_SumTrackPt2.push_back(null);
            ftree->pv_trk_pvunbiased_fracHighPurity.push_back(null);
            ftree->pv_trk_pvunbiased_chi2.push_back(null);
            ftree->pv_trk_pvunbiased_ndof.push_back(null);
            ftree->pv_trk_pvunbiased_x.push_back(null);
            ftree->pv_trk_pvunbiased_y.push_back(null);
            ftree->pv_trk_pvunbiased_z.push_back(null);
            ftree->pv_trk_pvunbiased_xError.push_back(null);
            ftree->pv_trk_pvunbiased_yError.push_back(null);
            ftree->pv_trk_pvunbiased_zError.push_back(null);

            ftree->pv_trk_d0_pvunbiased.push_back(null);
            ftree->pv_trk_dz_pvunbiased.push_back(null);
            ftree->pv_trk_d0_bs_zpvunbiased.push_back(null);
        }

        iTrk++;
    }

    if (nTracks)
        pv_fracHighPurity /= Float_t(nTracks);

    ftree->pv_IsValid = refitted_PV_front.isValid();
    ftree->pv_IsFake = refitted_PV_front.isFake();
    ftree->pv_NTracks = nTracks;
    ftree->pv_SumTrackPt = pv_SumTrackPt;
    ftree->pv_SumTrackPt2 = pv_SumTrackPt2;
    ftree->pv_fracHighPurity = pv_fracHighPurity;
    ftree->pv_chi2 = refitted_PV_front.chi2();
    ftree->pv_ndof = refitted_PV_front.ndof();
    ftree->pv_x = refitted_PV_front.x() * micron;
    ftree->pv_y = refitted_PV_front.y() * micron;
    ftree->pv_z = refitted_PV_front.z() * micron;
    ftree->pv_xError = refitted_PV_front.xError() * micron;
    ftree->pv_yError = refitted_PV_front.yError() * micron;
    ftree->pv_zError = refitted_PV_front.zError() * micron;

    reco::TrackCollection vtxTkCollection1;
    reco::TrackCollection vtxTkCollection2;

    Float_t SumTrackPt_p1 = 0;
    Float_t SumTrackPt2_p1 = 0;
    Float_t pv_fracHighPurity_p1 = 0;

    Float_t SumTrackPt_p2 = 0;
    Float_t SumTrackPt2_p2 = 0;
    Float_t pv_fracHighPurity_p2 = 0;

    for (std::vector<reco::TransientTrack>::const_iterator it = vtxTracks.begin(); it != vtxTracks.end(); it++)
    {
        reco::Track trk = (*it).track();

        if (rnd->Rndm() > 0.5)
        {
            vtxTkCollection1.push_back(trk);
            SumTrackPt_p1 += trk.pt();
            SumTrackPt2_p1 += trk.pt() * trk.pt();
            pv_fracHighPurity_p1 += trk.quality(reco::TrackBase::highPurity);
        }
        else
        {
            vtxTkCollection2.push_back(trk);
            SumTrackPt_p2 += trk.pt();
            SumTrackPt2_p2 += trk.pt() * trk.pt();
            pv_fracHighPurity_p2 += trk.quality(reco::TrackBase::highPurity);
        }
    }

    if (nTracks)
    {
        pv_fracHighPurity_p1 /= Float_t(nTracks);
        pv_fracHighPurity_p2 /= Float_t(nTracks);
    }

    std::vector<TransientVertex> refitted_tPVs1 = revertex->makeVertices(vtxTkCollection1, *beamspotHandle, iSetup);
    std::vector<TransientVertex> refitted_tPVs2 = revertex->makeVertices(vtxTkCollection2, *beamspotHandle, iSetup);

    if (!refitted_tPVs1.empty() && !refitted_tPVs2.empty())
    {
        reco::Vertex vtx1 = reco::Vertex(refitted_tPVs1.front());
        ftree->pv_IsValid_p1 = vtx1.isValid();
        ftree->pv_IsFake_p1 = vtx1.isFake();
        ftree->pv_NTracks_p1 = vtxTkCollection1.size();
        ftree->pv_SumTrackPt_p1 = SumTrackPt_p1;
        ftree->pv_SumTrackPt2_p1 = SumTrackPt2_p1;
        ftree->pv_fracHighPurity_p1 = pv_fracHighPurity_p1;
        ftree->pv_chi2_p1 = vtx1.chi2();
        ftree->pv_ndof_p1 = vtx1.ndof();
        ftree->pv_x_p1 = vtx1.x() * micron;
        ftree->pv_y_p1 = vtx1.y() * micron;
        ftree->pv_z_p1 = vtx1.z() * micron;
        ftree->pv_xError_p1 = vtx1.xError() * micron;
        ftree->pv_yError_p1 = vtx1.yError() * micron;
        ftree->pv_zError_p1 = vtx1.zError() * micron;

        reco::Vertex vtx2 = reco::Vertex(refitted_tPVs2.front());
        ftree->pv_IsValid_p2 = vtx2.isValid();
        ftree->pv_IsFake_p2 = vtx2.isFake();
        ftree->pv_NTracks_p2 = vtxTkCollection2.size();
        ftree->pv_SumTrackPt_p2 = SumTrackPt_p2;
        ftree->pv_SumTrackPt2_p2 = SumTrackPt2_p2;
        ftree->pv_fracHighPurity_p2 = pv_fracHighPurity_p2;
        ftree->pv_chi2_p2 = vtx2.chi2();
        ftree->pv_ndof_p2 = vtx2.ndof();
        ftree->pv_x_p2 = vtx2.x() * micron;
        ftree->pv_y_p2 = vtx2.y() * micron;
        ftree->pv_z_p2 = vtx2.z() * micron;
        ftree->pv_xError_p2 = vtx2.xError() * micron;
        ftree->pv_yError_p2 = vtx2.yError() * micron;
        ftree->pv_zError_p2 = vtx2.zError() * micron;
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