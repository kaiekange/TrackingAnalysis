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
    /* edm::EDGetTokenT<reco::TrackCollection> tracksToken_; */
    edm::EDGetTokenT<edm::View<pat::PackedCandidate>> tracksToken_;
    edm::EDGetTokenT<reco::BeamSpot> beamspotToken_;
    edm::EDGetTokenT<Double_t> rhoToken_;
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

    Bool_t runOnData;
    Int_t eventScale;
    Int_t eventModulo;
    Int_t trackScale;

    TRandom3 *rnd;

    const edm::Service<TFileService> fs;
    ResTree *ftree;

    ULong_t nEventsProcessed_;
    ULong_t nEventsScaled_;
};

Residuals::Residuals(const edm::ParameterSet &pset) : magFieldToken_(esConsumes<MagneticField, IdealMagneticFieldRecord>())
{
    edm::InputTag TrackCollectionTag_ = pset.getParameter<edm::InputTag>("TrackLabel");
    tracksToken_ = consumes<edm::View<pat::PackedCandidate>>(TrackCollectionTag_);

    edm::InputTag VertexCollectionTag_ = pset.getParameter<edm::InputTag>("VertexLabel");
    primvtxToken_ = consumes<reco::VertexCollection>(VertexCollectionTag_);

    edm::InputTag BeamspotTag_ = edm::InputTag("offlineBeamSpot");
    beamspotToken_ = consumes<reco::BeamSpot>(BeamspotTag_);

    edm::InputTag RhoTag_ = pset.getParameter<edm::InputTag>("RhoLabel");
    rhoToken_ = consumes<Double_t>(RhoTag_);

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
    //   vtxErrorXMin     = pset.getParameter<Double_t>("VtxErrorXMin");
    //   vtxErrorXMax     = pset.getParameter<Double_t>("VtxErrorXMax");
    //   vtxErrorYMin     = pset.getParameter<Double_t>("VtxErrorYMin");
    //   vtxErrorYMax     = pset.getParameter<Double_t>("VtxErrorYMax");
    //   vtxErrorZMin     = pset.getParameter<Double_t>("VtxErrorZMin");
    //   vtxErrorZMax     = pset.getParameter<Double_t>("VtxErrorZMax");

    edm::ConsumesCollector c{consumesCollector()};
    revertex = new VertexReProducer(pset, c);

    runOnData = pset.getParameter<Bool_t>("RunOnData");
    eventScale = pset.getParameter<Int_t>("EventScale");
    eventModulo = pset.getParameter<Int_t>("EventModulo");
    trackScale = pset.getParameter<Int_t>("TrackScale");

    rnd = new TRandom3();
}

Residuals::~Residuals()
{
    if (revertex)
        delete revertex;
    delete rnd;
    delete ftree;
}

void Residuals::beginJob()
{
    TFile &f = fs->file();
    f.SetCompressionAlgorithm(ROOT::kZSTD);
    f.SetCompressionLevel(5);
    ftree = new ResTree(fs->make<TTree>("tree", "tree"));
    ftree->CreateBranches(runOnData);

    nEventsProcessed_ = 0;
    nEventsScaled_ = 0;
}

void Residuals::analyze(const edm::Event &iEvent, const edm::EventSetup &iSetup)
{
    nEventsProcessed_++;

    if ((nEventsProcessed_ - 1) % eventScale != static_cast<ULong_t>(eventModulo) && eventScale > 0)
        return;

    nEventsScaled_++;

    using namespace edm;
    using namespace reco;
    using namespace std;

    ftree->Init();

    Handle<edm::View<pat::PackedCandidate>> tracksHandle;
    iEvent.getByToken(tracksToken_, tracksHandle);

    edm::View<pat::PackedCandidate> tracksPacked = (*tracksHandle.product());

    TrackCollection tracks;

    for (size_t it = 0; it < tracksPacked.size(); it++)
    {
        const pat::PackedCandidate &trkPacked = tracksPacked[it];
        if (!trkPacked.hasTrackDetails())
            continue;
        tracks.push_back(trkPacked.pseudoTrack());
    }

    Handle<VertexCollection> primvtxHandle;
    iEvent.getByToken(primvtxToken_, primvtxHandle);

    Handle<BeamSpot> beamspotHandle;
    iEvent.getByToken(beamspotToken_, beamspotHandle);

    if (!primvtxHandle.isValid())
        return;
    if (primvtxHandle->size() == 0)
        return;

    // refit primary vertices and put it in a new handle (note: TrackBaseRefs are different)
    vector<TransientVertex> pvs = revertex->makeVertices(tracks, *beamspotHandle, iSetup);
    std::cout << "Primary vertices = " << primvtxHandle->size() << ", refitted vertices = " << pvs.size() << ", tracks = " << tracks.size() << std::endl;
    if (pvs.empty())
        return;

    TransientVertex tvtx = pvs.front();
    reco::Vertex vtx = reco::Vertex(pvs.front());

    if (!vertexSelection(vtx))
        return;

    edm::ESHandle<MagneticField> magField = iSetup.getHandle(magFieldToken_);

    Handle<Double_t> rhoHandle;
    iEvent.getByToken(rhoToken_, rhoHandle);

    ftree->ev_run = iEvent.id().run();
    ftree->ev_id = iEvent.id().event();
    ftree->ev_lumi = iEvent.id().luminosityBlock();
    ftree->ev_bunchCrossing = iEvent.bunchCrossing();
    ftree->ev_orbitNumber = iEvent.orbitNumber();
    ftree->ev_time = iEvent.time().unixTime();
    ftree->ev_rho = *rhoHandle;
    ftree->ev_nPV = pvs.size();

    Handle<TriggerResults> triggerHandle;
    iEvent.getByToken(triggerToken_, triggerHandle);
    const TriggerNames &names = iEvent.triggerNames(*triggerHandle);
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

    // Pileup info
    if (!runOnData)
    {
        edm::Handle<std::vector<PileupSummaryInfo>> PileupInfo;
        iEvent.getByToken(PileupToken, PileupInfo);

        for(std::vector<PileupSummaryInfo>::const_iterator iPU = PileupInfo->begin(); iPU != PileupInfo->end(); iPU++){
            Int_t BX = iPU->getBunchCrossing();
            if(BX == 0){
                ftree->NumTrueInts = iPU->getTrueNumInteractions();
                ftree->NumPUInts = iPU->getPU_NumInteractions();
            }
        }
    }

    Double_t micron = 10000;

    ftree->bs_type = beamspotHandle->type();
    ftree->bs_x0 = beamspotHandle->x0();
    ftree->bs_y0 = beamspotHandle->y0();
    ftree->bs_z0 = beamspotHandle->z0();
    ftree->bs_x_zpv = beamspotHandle->x(vtx.z());
    ftree->bs_y_zpv = beamspotHandle->y(vtx.z());
    ftree->bs_sigmaZ = beamspotHandle->sigmaZ();
    ftree->bs_dxdz = beamspotHandle->dxdz();
    ftree->bs_dydz = beamspotHandle->dydz();
    ftree->bs_BeamWidthX = beamspotHandle->BeamWidthX();
    ftree->bs_BeamWidthY = beamspotHandle->BeamWidthY();
    ftree->bs_x0Error = beamspotHandle->x0Error();
    ftree->bs_y0Error = beamspotHandle->y0Error();
    ftree->bs_z0Error = beamspotHandle->z0Error();
    ftree->bs_sigmaZ0Error = beamspotHandle->sigmaZ0Error();
    ftree->bs_dxdzError = beamspotHandle->dxdzError();
    ftree->bs_dydzError = beamspotHandle->dydzError();
    ftree->bs_BeamWidthXError = beamspotHandle->BeamWidthXError();
    ftree->bs_BeamWidthYError = beamspotHandle->BeamWidthYError();
    ftree->bs_emittanceX = beamspotHandle->emittanceX();
    ftree->bs_emittanceY = beamspotHandle->emittanceY();
    ftree->bs_betaStar = beamspotHandle->betaStar();

    std::vector<reco::TransientTrack> vtxTracks = tvtx.originalTracks();
    stable_sort(vtxTracks.begin(), vtxTracks.end(), sortPt);

    Int_t nTracks = vtx.tracksSize();

    Track::Point vtxPosition = Track::Point(vtx.position().x(), vtx.position().y(), vtx.position().z());

    Float_t pv_SumTrackPt = 0.;
    Float_t pv_SumTrackPt2 = 0.;
    Float_t pv_fracHighPurity = 0.;

    std::vector<Float_t> pv_trackWeight;
    std::vector<Int_t> pv_trk_algo;
    std::vector<Int_t> pv_trk_originalAlgo;
    std::vector<Bool_t> pv_trk_isHighPurity;
    std::vector<Int_t> pv_trk_idx;
    std::vector<Int_t> pv_trk_pvN;

    std::vector<Bool_t> pv_trk_pvunbiased_IsValid;
    std::vector<Bool_t> pv_trk_pvunbiased_IsFake;
    std::vector<Int_t> pv_trk_pvunbiased_NTracks;
    std::vector<Float_t> pv_trk_pvunbiased_SumTrackPt;
    std::vector<Float_t> pv_trk_pvunbiased_SumTrackPt2;
    std::vector<Float_t> pv_trk_pvunbiased_fracHighPurity;
    std::vector<Float_t> pv_trk_pvunbiased_chi2;
    std::vector<Int_t> pv_trk_pvunbiased_ndof;
    std::vector<Float_t> pv_trk_pvunbiased_x;
    std::vector<Float_t> pv_trk_pvunbiased_y;
    std::vector<Float_t> pv_trk_pvunbiased_z;
    std::vector<Float_t> pv_trk_pvunbiased_xError;
    std::vector<Float_t> pv_trk_pvunbiased_yError;
    std::vector<Float_t> pv_trk_pvunbiased_zError;

    std::vector<Float_t> pv_trk_d0_pvunbiased;
    std::vector<Float_t> pv_trk_dz_pvunbiased;
    std::vector<Float_t> pv_trk_d0_bs_zpvunbiased;

    std::vector<Float_t> pv_trk_pt;
    std::vector<Float_t> pv_trk_px;
    std::vector<Float_t> pv_trk_py;
    std::vector<Float_t> pv_trk_pz;
    std::vector<Float_t> pv_trk_p;
    std::vector<Float_t> pv_trk_eta;
    std::vector<Float_t> pv_trk_phi;

    std::vector<Float_t> pv_trk_d0;
    std::vector<Float_t> pv_trk_dz;
    std::vector<Float_t> pv_trk_d0_pv;
    std::vector<Float_t> pv_trk_dz_pv;
    std::vector<Float_t> pv_trk_d0_bs;
    std::vector<Float_t> pv_trk_d0_bs_zpca;
    std::vector<Float_t> pv_trk_d0_bs_zpv;
    std::vector<Float_t> pv_trk_dz_bs;
    std::vector<Float_t> pv_trk_d0Err;
    std::vector<Float_t> pv_trk_dzErr;

    TrackCollection initPVTkCollection;
    for (std::vector<reco::TransientTrack>::const_iterator it = vtxTracks.begin(); it != vtxTracks.end(); it++)
    {
        reco::Track trk = (*it).track();
        initPVTkCollection.push_back(trk);
    }

    Int_t iTrk = 0;
    for (std::vector<reco::TransientTrack>::const_iterator it = vtxTracks.begin(); it != vtxTracks.end(); it++)
    {
        reco::Track trk = (*it).track();

        TrackCollection::const_iterator itt = find_if(tracks.begin(), tracks.end(), TrackEqualReco(trk));

        if (itt != tracks.end())
            pv_trk_idx.push_back(itt - tracks.begin());
        else
            pv_trk_idx.push_back(-1);

        // Remove the track from the PV track collection
        TrackCollection newPVTkCollection;
        newPVTkCollection.assign(initPVTkCollection.begin(), initPVTkCollection.begin() + iTrk);
        newPVTkCollection.insert(newPVTkCollection.end(), initPVTkCollection.begin() + iTrk + 1, initPVTkCollection.end());

        vector<TransientVertex> pvst = revertex->makeVertices(newPVTkCollection, *beamspotHandle, iSetup);

        pv_trk_pvN.push_back(pvst.size());

        if (!pvst.empty())
        {
            reco::Vertex vtxt = reco::Vertex(pvst.front());

            Float_t unbiasedSumTrackPt = 0.;
            Float_t unbiasedSumTrackPt2 = 0.;
            Float_t unbiasedFracHighPurity = 0.;

            for (TrackCollection::const_iterator itt = newPVTkCollection.begin(); itt != newPVTkCollection.end(); itt++)
            {
                unbiasedSumTrackPt += (*itt).pt();
                unbiasedSumTrackPt2 += (*itt).pt() * (*itt).pt();
                unbiasedFracHighPurity += (*itt).quality(reco::TrackBase::highPurity);
            }
            Int_t nTracksUnbiased = vtxt.tracksSize();
            if (nTracksUnbiased)
                unbiasedFracHighPurity /= Float_t(nTracksUnbiased);

            pv_trk_pvunbiased_IsValid.push_back(vtxt.isValid());
            pv_trk_pvunbiased_IsFake.push_back(vtxt.isFake());
            pv_trk_pvunbiased_NTracks.push_back(newPVTkCollection.size());
            pv_trk_pvunbiased_SumTrackPt.push_back(unbiasedSumTrackPt);
            pv_trk_pvunbiased_SumTrackPt2.push_back(unbiasedSumTrackPt2);
            pv_trk_pvunbiased_fracHighPurity.push_back(unbiasedFracHighPurity);
            pv_trk_pvunbiased_chi2.push_back(vtxt.chi2());
            pv_trk_pvunbiased_ndof.push_back(vtxt.ndof());
            pv_trk_pvunbiased_x.push_back(vtxt.x() * micron);
            pv_trk_pvunbiased_y.push_back(vtxt.y() * micron);
            pv_trk_pvunbiased_z.push_back(vtxt.z() * micron);
            pv_trk_pvunbiased_xError.push_back(vtxt.xError() * micron);
            pv_trk_pvunbiased_yError.push_back(vtxt.yError() * micron);
            pv_trk_pvunbiased_zError.push_back(vtxt.zError() * micron);

            Track::Point vtxPositionUnbiased = Track::Point(vtxt.position().x(), vtxt.position().y(), vtxt.position().z());

            pv_trk_d0_pvunbiased.push_back(trk.dxy(vtxPositionUnbiased) * micron);
            pv_trk_dz_pvunbiased.push_back(trk.dz(vtxPositionUnbiased) * micron);
            pv_trk_d0_bs_zpvunbiased.push_back(trk.dxy(beamspotHandle->position(vtxPositionUnbiased.z())) * micron);
        }
        else
        {
            pv_trk_pvunbiased_IsValid.push_back(false);
            pv_trk_pvunbiased_IsFake.push_back(true);
            pv_trk_pvunbiased_NTracks.push_back(null);
            pv_trk_pvunbiased_SumTrackPt.push_back(null);
            pv_trk_pvunbiased_SumTrackPt2.push_back(null);
            pv_trk_pvunbiased_fracHighPurity.push_back(null);
            pv_trk_pvunbiased_chi2.push_back(null);
            pv_trk_pvunbiased_ndof.push_back(null);
            pv_trk_pvunbiased_x.push_back(null);
            pv_trk_pvunbiased_y.push_back(null);
            pv_trk_pvunbiased_z.push_back(null);
            pv_trk_pvunbiased_xError.push_back(null);
            pv_trk_pvunbiased_yError.push_back(null);
            pv_trk_pvunbiased_zError.push_back(null);

            pv_trk_d0_pvunbiased.push_back(null);
            pv_trk_dz_pvunbiased.push_back(null);
            pv_trk_d0_bs_zpvunbiased.push_back(null);
        }

        pv_SumTrackPt += trk.pt();
        pv_SumTrackPt2 += trk.pt() * trk.pt();
        pv_fracHighPurity += trk.quality(reco::TrackBase::highPurity);

        if (tvtx.hasTrackWeight())
            pv_trackWeight.push_back(tvtx.trackWeight(*it));
        else
            pv_trackWeight.push_back(null);

        pv_trk_isHighPurity.push_back(trk.quality(reco::TrackBase::highPurity));
        pv_trk_algo.push_back(trk.algo());
        pv_trk_originalAlgo.push_back(trk.originalAlgo());

        pv_trk_pt.push_back(trk.pt());
        pv_trk_px.push_back(trk.px());
        pv_trk_py.push_back(trk.py());
        pv_trk_pz.push_back(trk.pz());
        pv_trk_p.push_back(trk.p());
        pv_trk_eta.push_back(trk.eta());
        pv_trk_phi.push_back(trk.phi());

        pv_trk_d0.push_back(trk.dxy() * micron);
        pv_trk_dz.push_back(trk.dz() * micron);
        pv_trk_d0_pv.push_back(trk.dxy(vtxPosition) * micron);
        pv_trk_dz_pv.push_back(trk.dz(vtxPosition) * micron);
        pv_trk_d0_bs.push_back(trk.dxy(beamspotHandle->position()) * micron);
        pv_trk_d0_bs_zpca.push_back(trk.dxy(*beamspotHandle) * micron);
        pv_trk_d0_bs_zpv.push_back(trk.dxy(beamspotHandle->position(vtxPosition.z())) * micron);
        pv_trk_dz_bs.push_back(trk.dz(beamspotHandle->position()) * micron);
        pv_trk_d0Err.push_back(trk.d0Error() * micron);
        pv_trk_dzErr.push_back(trk.dzError() * micron);

        iTrk++;
    }

    if (nTracks)
        pv_fracHighPurity /= Float_t(nTracks);

    ftree->pv_IsValid.push_back(vtx.isValid());
    ftree->pv_IsFake.push_back(vtx.isFake());
    ftree->pv_NTracks.push_back(nTracks);
    ftree->pv_SumTrackPt.push_back(pv_SumTrackPt);
    ftree->pv_SumTrackPt2.push_back(pv_SumTrackPt2);
    ftree->pv_fracHighPurity.push_back(pv_fracHighPurity);
    ftree->pv_chi2.push_back(vtx.chi2());
    ftree->pv_ndof.push_back(vtx.ndof());
    ftree->pv_x.push_back(vtx.x() * micron);
    ftree->pv_y.push_back(vtx.y() * micron);
    ftree->pv_z.push_back(vtx.z() * micron);
    ftree->pv_xError.push_back(vtx.xError() * micron);
    ftree->pv_yError.push_back(vtx.yError() * micron);
    ftree->pv_zError.push_back(vtx.zError() * micron);

    ftree->pv_trk_weight.push_back(pv_trackWeight);
    ftree->pv_trk_isHighPurity.push_back(pv_trk_isHighPurity);
    ftree->pv_trk_algo.push_back(pv_trk_algo);
    ftree->pv_trk_originalAlgo.push_back(pv_trk_originalAlgo);

    ftree->pv_trk_idx.push_back(pv_trk_idx);
    ftree->pv_trk_pvN.push_back(pv_trk_pvN);

    ftree->pv_trk_pvunbiased_IsValid.push_back(pv_trk_pvunbiased_IsValid);
    ftree->pv_trk_pvunbiased_IsFake.push_back(pv_trk_pvunbiased_IsFake);
    ftree->pv_trk_pvunbiased_NTracks.push_back(pv_trk_pvunbiased_NTracks);
    ftree->pv_trk_pvunbiased_SumTrackPt.push_back(pv_trk_pvunbiased_SumTrackPt);
    ftree->pv_trk_pvunbiased_SumTrackPt2.push_back(pv_trk_pvunbiased_SumTrackPt2);
    ftree->pv_trk_pvunbiased_fracHighPurity.push_back(pv_trk_pvunbiased_fracHighPurity);
    ftree->pv_trk_pvunbiased_chi2.push_back(pv_trk_pvunbiased_chi2);
    ftree->pv_trk_pvunbiased_ndof.push_back(pv_trk_pvunbiased_ndof);
    ftree->pv_trk_pvunbiased_x.push_back(pv_trk_pvunbiased_x);
    ftree->pv_trk_pvunbiased_y.push_back(pv_trk_pvunbiased_y);
    ftree->pv_trk_pvunbiased_z.push_back(pv_trk_pvunbiased_z);
    ftree->pv_trk_pvunbiased_xError.push_back(pv_trk_pvunbiased_xError);
    ftree->pv_trk_pvunbiased_yError.push_back(pv_trk_pvunbiased_yError);
    ftree->pv_trk_pvunbiased_zError.push_back(pv_trk_pvunbiased_zError);

    ftree->pv_trk_d0_pvunbiased.push_back(pv_trk_d0_pvunbiased);
    ftree->pv_trk_dz_pvunbiased.push_back(pv_trk_dz_pvunbiased);
    ftree->pv_trk_d0_bs_zpvunbiased.push_back(pv_trk_d0_bs_zpvunbiased);

    ftree->pv_trk_pt.push_back(pv_trk_pt);
    ftree->pv_trk_px.push_back(pv_trk_px);
    ftree->pv_trk_py.push_back(pv_trk_py);
    ftree->pv_trk_pz.push_back(pv_trk_pz);
    ftree->pv_trk_p.push_back(pv_trk_p);
    ftree->pv_trk_eta.push_back(pv_trk_eta);
    ftree->pv_trk_phi.push_back(pv_trk_phi);

    ftree->pv_trk_d0.push_back(pv_trk_d0);
    ftree->pv_trk_dz.push_back(pv_trk_dz);
    ftree->pv_trk_d0_pv.push_back(pv_trk_d0_pv);
    ftree->pv_trk_dz_pv.push_back(pv_trk_dz_pv);
    ftree->pv_trk_d0_bs.push_back(pv_trk_d0_bs);
    ftree->pv_trk_d0_bs_zpca.push_back(pv_trk_d0_bs_zpca);
    ftree->pv_trk_d0_bs_zpv.push_back(pv_trk_d0_bs_zpv);
    ftree->pv_trk_dz_bs.push_back(pv_trk_dz_bs);
    ftree->pv_trk_d0Err.push_back(pv_trk_d0Err);
    ftree->pv_trk_dzErr.push_back(pv_trk_dzErr);

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

    vector<TransientVertex> pvs1 = revertex->makeVertices(vtxTkCollection1, *beamspotHandle, iSetup);
    vector<TransientVertex> pvs2 = revertex->makeVertices(vtxTkCollection2, *beamspotHandle, iSetup);

    if (!pvs1.empty() && !pvs2.empty())
    {
        reco::Vertex vtx1 = reco::Vertex(pvs1.front());
        ftree->pv_IsValid_p1.push_back(vtx1.isValid());
        ftree->pv_IsFake_p1.push_back(vtx1.isFake());
        ftree->pv_NTracks_p1.push_back(vtxTkCollection1.size());
        ftree->pv_SumTrackPt_p1.push_back(SumTrackPt_p1);
        ftree->pv_SumTrackPt2_p1.push_back(SumTrackPt2_p1);
        ftree->pv_fracHighPurity_p1.push_back(pv_fracHighPurity_p1);
        ftree->pv_chi2_p1.push_back(vtx1.chi2());
        ftree->pv_ndof_p1.push_back(vtx1.ndof());
        ftree->pv_x_p1.push_back(vtx1.x() * micron);
        ftree->pv_y_p1.push_back(vtx1.y() * micron);
        ftree->pv_z_p1.push_back(vtx1.z() * micron);
        ftree->pv_xError_p1.push_back(vtx1.xError() * micron);
        ftree->pv_yError_p1.push_back(vtx1.yError() * micron);
        ftree->pv_zError_p1.push_back(vtx1.zError() * micron);

        reco::Vertex vtx2 = reco::Vertex(pvs2.front());
        ftree->pv_IsValid_p2.push_back(vtx2.isValid());
        ftree->pv_IsFake_p2.push_back(vtx2.isFake());
        ftree->pv_NTracks_p2.push_back(vtxTkCollection2.size());
        ftree->pv_SumTrackPt_p2.push_back(SumTrackPt_p2);
        ftree->pv_SumTrackPt2_p2.push_back(SumTrackPt2_p2);
        ftree->pv_fracHighPurity_p2.push_back(pv_fracHighPurity_p2);
        ftree->pv_chi2_p2.push_back(vtx2.chi2());
        ftree->pv_ndof_p2.push_back(vtx2.ndof());
        ftree->pv_x_p2.push_back(vtx2.x() * micron);
        ftree->pv_y_p2.push_back(vtx2.y() * micron);
        ftree->pv_z_p2.push_back(vtx2.z() * micron);
        ftree->pv_xError_p2.push_back(vtx2.xError() * micron);
        ftree->pv_yError_p2.push_back(vtx2.yError() * micron);
        ftree->pv_zError_p2.push_back(vtx2.zError() * micron);
    }

    ftree->tree->Fill();
}

void Residuals::endJob()
{
    edm::LogInfo("Residuals") << "Processed all events.";
    edm::LogPrint("Residuals") << "Total number of events before event scale: " << nEventsProcessed_;
    edm::LogPrint("Residuals") << "Total number of events after event scale: " << nEventsScaled_;
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