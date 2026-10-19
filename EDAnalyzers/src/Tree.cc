#include "TrackingAnalysis/EDAnalyzers/interface/Tree.h"

ResTree::ResTree(TTree *_tree)
{
    tree = _tree;
}

void ResTree::Init()
{
    ev_run = null;
    ev_lumi = null;
    ev_id = null;
    ev_nPV = null;

    trig_ZeroBias_pass = false;
    trig_PFHT180_pass = false;
    trig_PFHT250_pass = false;
    trig_PFHT370_pass = false;
    trig_PFHT430_pass = false;
    trig_PFHT510_pass = false;
    trig_PFHT590_pass = false;
    trig_PFHT680_pass = false;
    trig_PFHT780_pass = false;
    trig_PFHT890_pass = false;
    trig_PFHT1050_pass = false;

    NumTrueInts = null;

    bs_type = null;
    bs_x0 = null;
    bs_y0 = null;
    bs_z0 = null;
    bs_x_zpv = null;
    bs_y_zpv = null;
    bs_sigmaZ = null;
    bs_dxdz = null;
    bs_dydz = null;
    bs_BeamWidthX = null;
    bs_BeamWidthY = null;
    bs_x0Error = null;
    bs_y0Error = null;
    bs_z0Error = null;
    bs_sigmaZ0Error = null;
    bs_dxdzError = null;
    bs_dydzError = null;
    bs_BeamWidthXError = null;
    bs_BeamWidthYError = null;

    pv_IsValid = false;
    pv_IsFake = false;
    pv_NTracks = null;
    pv_SumTrackPt = null;
    pv_SumTrackPt2 = null;
    pv_chi2 = null;
    pv_ndof = null;
    pv_x = null;
    pv_y = null;
    pv_z = null;
    pv_xError = null;
    pv_yError = null;
    pv_zError = null;

    pv_trk_pvAssociationQuality.clear();
    pv_trk_isHighPurity.clear();
    pv_trk_charge.clear();
    pv_trk_nTrackerLayers.clear();
    pv_trk_nPixelBarrelLayers.clear();
    pv_trk_nPixelEndcapLayers.clear();
    pv_trk_nValidPixelHits.clear();
    pv_trk_nMissedIn.clear();
    pv_trk_hasPixelBarrelLayer1.clear();
    pv_trk_hasPixelEndcapLayer1.clear();

    pv_trk_pvunbiased_IsValid.clear();
    pv_trk_pvunbiased_IsFake.clear();
    pv_trk_pvunbiased_SumTrackPt2.clear();
    pv_trk_pvunbiased_xError.clear();
    pv_trk_pvunbiased_yError.clear();
    pv_trk_pvunbiased_zError.clear();
    pv_trk_pvunbiased_covXY.clear();
    pv_trk_pvunbiased_covXZ.clear();
    pv_trk_pvunbiased_covYZ.clear();

    pv_trk_d0_pvunbiased.clear();
    pv_trk_dz_pvunbiased.clear();

    pv_trk_pt.clear();
    pv_trk_eta.clear();
    pv_trk_phi.clear();

    pv_trk_d0Err.clear();
    pv_trk_dzErr.clear();

    pv_IsValid_p1 = false;
    pv_IsFake_p1 = false;
    pv_SumTrackPt2_p1 = null;
    pv_x_p1 = null;
    pv_y_p1 = null;
    pv_z_p1 = null;
    pv_xError_p1 = null;
    pv_yError_p1 = null;
    pv_zError_p1 = null;
    pv_covXY_p1 = null;
    pv_covXZ_p1 = null;
    pv_covYZ_p1 = null;

    pv_IsValid_p2 = false;
    pv_IsFake_p2 = false;
    pv_SumTrackPt2_p2 = null;
    pv_x_p2 = null;
    pv_y_p2 = null;
    pv_z_p2 = null;
    pv_xError_p2 = null;
    pv_yError_p2 = null;
    pv_zError_p2 = null;
    pv_covXY_p2 = null;
    pv_covXZ_p2 = null;
    pv_covYZ_p2 = null;
}

void ResTree::CreateBranches(Bool_t runOnData)
{
    tree->Branch("ev_run", &ev_run);
    tree->Branch("ev_lumi", &ev_lumi);
    tree->Branch("ev_id", &ev_id);
    tree->Branch("ev_nPV", &ev_nPV);

    tree->Branch("trig_PFHT180_pass", &trig_PFHT180_pass);
    tree->Branch("trig_PFHT250_pass", &trig_PFHT250_pass);
    tree->Branch("trig_PFHT370_pass", &trig_PFHT370_pass);
    tree->Branch("trig_PFHT430_pass", &trig_PFHT430_pass);
    tree->Branch("trig_PFHT510_pass", &trig_PFHT510_pass);
    tree->Branch("trig_PFHT590_pass", &trig_PFHT590_pass);
    tree->Branch("trig_PFHT680_pass", &trig_PFHT680_pass);
    tree->Branch("trig_PFHT780_pass", &trig_PFHT780_pass);
    tree->Branch("trig_PFHT890_pass", &trig_PFHT890_pass);
    tree->Branch("trig_PFHT1050_pass", &trig_PFHT1050_pass);

    if (!runOnData)
        tree->Branch("NumTrueInts", &NumTrueInts);

    tree->Branch("bs_type", &bs_type);
    tree->Branch("bs_x0", &bs_x0);
    tree->Branch("bs_y0", &bs_y0);
    tree->Branch("bs_z0", &bs_z0);
    tree->Branch("bs_x_zpv", &bs_x_zpv);
    tree->Branch("bs_y_zpv", &bs_y_zpv);
    tree->Branch("bs_sigmaZ", &bs_sigmaZ);
    tree->Branch("bs_dxdz", &bs_dxdz);
    tree->Branch("bs_dydz", &bs_dydz);
    tree->Branch("bs_BeamWidthX", &bs_BeamWidthX);
    tree->Branch("bs_BeamWidthY", &bs_BeamWidthY);
    tree->Branch("bs_x0Error", &bs_x0Error);
    tree->Branch("bs_y0Error", &bs_y0Error);
    tree->Branch("bs_z0Error", &bs_z0Error);
    tree->Branch("bs_sigmaZ0Error", &bs_sigmaZ0Error);
    tree->Branch("bs_dxdzError", &bs_dxdzError);
    tree->Branch("bs_dydzError", &bs_dydzError);
    tree->Branch("bs_BeamWidthXError", &bs_BeamWidthXError);
    tree->Branch("bs_BeamWidthYError", &bs_BeamWidthYError);

    tree->Branch("pv_IsValid", &pv_IsValid);
    tree->Branch("pv_IsFake", &pv_IsFake);
    tree->Branch("pv_NTracks", &pv_NTracks);
    tree->Branch("pv_SumTrackPt", &pv_SumTrackPt);
    tree->Branch("pv_SumTrackPt2", &pv_SumTrackPt2);
    tree->Branch("pv_chi2", &pv_chi2);
    tree->Branch("pv_ndof", &pv_ndof);
    tree->Branch("pv_x", &pv_x);
    tree->Branch("pv_y", &pv_y);
    tree->Branch("pv_z", &pv_z);
    tree->Branch("pv_xError", &pv_xError);
    tree->Branch("pv_yError", &pv_yError);
    tree->Branch("pv_zError", &pv_zError);

    tree->Branch("pv_trk_pvAssociationQuality", &pv_trk_pvAssociationQuality);
    tree->Branch("pv_trk_isHighPurity", &pv_trk_isHighPurity);
    tree->Branch("pv_trk_charge", &pv_trk_charge);
    tree->Branch("pv_trk_nTrackerLayers", &pv_trk_nTrackerLayers);
    tree->Branch("pv_trk_nPixelBarrelLayers", &pv_trk_nPixelBarrelLayers);
    tree->Branch("pv_trk_nPixelEndcapLayers", &pv_trk_nPixelEndcapLayers);
    tree->Branch("pv_trk_nValidPixelHits", &pv_trk_nValidPixelHits);
    tree->Branch("pv_trk_nMissedIn", &pv_trk_nMissedIn);
    tree->Branch("pv_trk_hasPixelBarrelLayer1", &pv_trk_hasPixelBarrelLayer1);
    tree->Branch("pv_trk_hasPixelEndcapLayer1", &pv_trk_hasPixelEndcapLayer1);

    tree->Branch("pv_trk_pvunbiased_IsValid", &pv_trk_pvunbiased_IsValid);
    tree->Branch("pv_trk_pvunbiased_IsFake", &pv_trk_pvunbiased_IsFake);
    tree->Branch("pv_trk_pvunbiased_SumTrackPt2", &pv_trk_pvunbiased_SumTrackPt2);
    tree->Branch("pv_trk_pvunbiased_xError", &pv_trk_pvunbiased_xError);
    tree->Branch("pv_trk_pvunbiased_yError", &pv_trk_pvunbiased_yError);
    tree->Branch("pv_trk_pvunbiased_zError", &pv_trk_pvunbiased_zError);
    tree->Branch("pv_trk_pvunbiased_covXY", &pv_trk_pvunbiased_covXY);
    tree->Branch("pv_trk_pvunbiased_covXZ", &pv_trk_pvunbiased_covXZ);
    tree->Branch("pv_trk_pvunbiased_covYZ", &pv_trk_pvunbiased_covYZ);

    tree->Branch("pv_trk_d0_pvunbiased", &pv_trk_d0_pvunbiased);
    tree->Branch("pv_trk_dz_pvunbiased", &pv_trk_dz_pvunbiased);

    tree->Branch("pv_trk_pt", &pv_trk_pt);
    tree->Branch("pv_trk_eta", &pv_trk_eta);
    tree->Branch("pv_trk_phi", &pv_trk_phi);

    tree->Branch("pv_trk_d0Err", &pv_trk_d0Err);
    tree->Branch("pv_trk_dzErr", &pv_trk_dzErr);

    tree->Branch("pv_IsValid_p1", &pv_IsValid_p1);
    tree->Branch("pv_IsFake_p1", &pv_IsFake_p1);
    tree->Branch("pv_SumTrackPt2_p1", &pv_SumTrackPt2_p1);
    tree->Branch("pv_x_p1", &pv_x_p1);
    tree->Branch("pv_y_p1", &pv_y_p1);
    tree->Branch("pv_z_p1", &pv_z_p1);
    tree->Branch("pv_xError_p1", &pv_xError_p1);
    tree->Branch("pv_yError_p1", &pv_yError_p1);
    tree->Branch("pv_zError_p1", &pv_zError_p1);
    tree->Branch("pv_covXY_p1", &pv_covXY_p1);
    tree->Branch("pv_covXZ_p1", &pv_covXZ_p1);
    tree->Branch("pv_covYZ_p1", &pv_covYZ_p1);

    tree->Branch("pv_IsValid_p2", &pv_IsValid_p2);
    tree->Branch("pv_IsFake_p2", &pv_IsFake_p2);
    tree->Branch("pv_SumTrackPt2_p2", &pv_SumTrackPt2_p2);
    tree->Branch("pv_x_p2", &pv_x_p2);
    tree->Branch("pv_y_p2", &pv_y_p2);
    tree->Branch("pv_z_p2", &pv_z_p2);
    tree->Branch("pv_xError_p2", &pv_xError_p2);
    tree->Branch("pv_yError_p2", &pv_yError_p2);
    tree->Branch("pv_zError_p2", &pv_zError_p2);
    tree->Branch("pv_covXY_p2", &pv_covXY_p2);
    tree->Branch("pv_covXZ_p2", &pv_covXZ_p2);
    tree->Branch("pv_covYZ_p2", &pv_covYZ_p2);
}
