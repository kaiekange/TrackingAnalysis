#include "TrackingAnalysis/EDAnalyzers/interface/Tree.h"

ResTree::ResTree(TTree *_tree)
{
    tree = _tree;
}

void ResTree::Init()
{
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
    trig_PFHT350_pass = false;

    NumTrueInts = null;
    NumPUInts = null;

    pv_IsValid = false;
    pv_IsFake = false;
    pv_NTracks = null;
    pv_SumTrackPt = null;
    pv_SumTrackPt2 = null;
    pv_fracHighPurity = null;
    pv_chi2 = null;
    pv_ndof = null;
    pv_x = null;
    pv_y = null;
    pv_z = null;
    pv_xError = null;
    pv_yError = null;
    pv_zError = null;

    pv_trk_weight.clear();
    pv_trk_isHighPurity.clear();
    pv_trk_algo.clear();
    pv_trk_originalAlgo.clear();
    pv_trk_pvN.clear();

    pv_trk_pvunbiased_IsValid.clear();
    pv_trk_pvunbiased_IsFake.clear();
    pv_trk_pvunbiased_NTracks.clear();
    pv_trk_pvunbiased_SumTrackPt.clear();
    pv_trk_pvunbiased_SumTrackPt2.clear();
    pv_trk_pvunbiased_fracHighPurity.clear();
    pv_trk_pvunbiased_chi2.clear();
    pv_trk_pvunbiased_ndof.clear();
    pv_trk_pvunbiased_x.clear();
    pv_trk_pvunbiased_y.clear();
    pv_trk_pvunbiased_z.clear();
    pv_trk_pvunbiased_xError.clear();
    pv_trk_pvunbiased_yError.clear();
    pv_trk_pvunbiased_zError.clear();

    pv_trk_d0_pvunbiased.clear();
    pv_trk_dz_pvunbiased.clear();
    pv_trk_d0_bs_zpvunbiased.clear();

    pv_trk_pt.clear();
    pv_trk_px.clear();
    pv_trk_py.clear();
    pv_trk_pz.clear();
    pv_trk_p.clear();
    pv_trk_eta.clear();
    pv_trk_phi.clear();

    pv_trk_d0.clear();
    pv_trk_dz.clear();
    pv_trk_d0_pv.clear();
    pv_trk_dz_pv.clear();
    pv_trk_d0_bs.clear();
    pv_trk_d0_bs_zpca.clear();
    pv_trk_d0_bs_zpv.clear();
    pv_trk_dz_bs.clear();
    pv_trk_d0Err.clear();
    pv_trk_dzErr.clear();

    pv_IsValid_p1 = false;
    pv_IsFake_p1 = false;
    pv_NTracks_p1 = null;
    pv_SumTrackPt_p1 = null;
    pv_SumTrackPt2_p1 = null;
    pv_fracHighPurity_p1 = null;
    pv_chi2_p1 = null;
    pv_ndof_p1 = null;
    pv_x_p1 = null;
    pv_y_p1 = null;
    pv_z_p1 = null;
    pv_xError_p1 = null;
    pv_yError_p1 = null;
    pv_zError_p1 = null;

    pv_IsValid_p2 = false;
    pv_IsFake_p2 = false;
    pv_NTracks_p2 = null;
    pv_SumTrackPt_p2 = null;
    pv_SumTrackPt2_p2 = null;
    pv_fracHighPurity_p2 = null;
    pv_chi2_p2 = null;
    pv_ndof_p2 = null;
    pv_x_p2 = null;
    pv_y_p2 = null;
    pv_z_p2 = null;
    pv_xError_p2 = null;
    pv_yError_p2 = null;
    pv_zError_p2 = null;
}

void ResTree::CreateBranches()
{
    tree->Branch("ev_nPV", &ev_nPV);

    tree->Branch("trig_ZeroBias_pass", &trig_ZeroBias_pass);
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
    tree->Branch("trig_PFHT350_pass", &trig_PFHT350_pass);

    tree->Branch("NumTrueInts", &NumTrueInts);
    tree->Branch("NumPUInts", &NumPUInts);

    tree->Branch("pv_IsValid", &pv_IsValid);
    tree->Branch("pv_IsFake", &pv_IsFake);
    tree->Branch("pv_NTracks", &pv_NTracks);
    tree->Branch("pv_SumTrackPt", &pv_SumTrackPt);
    tree->Branch("pv_SumTrackPt2", &pv_SumTrackPt2);
    tree->Branch("pv_fracHighPurity", &pv_fracHighPurity);
    tree->Branch("pv_chi2", &pv_chi2);
    tree->Branch("pv_ndof", &pv_ndof);
    tree->Branch("pv_x", &pv_x);
    tree->Branch("pv_y", &pv_y);
    tree->Branch("pv_z", &pv_z);
    tree->Branch("pv_xError", &pv_xError);
    tree->Branch("pv_yError", &pv_yError);
    tree->Branch("pv_zError", &pv_zError);

    tree->Branch("pv_trk_weight", &pv_trk_weight);
    tree->Branch("pv_trk_isHighPurity", &pv_trk_isHighPurity);
    tree->Branch("pv_trk_algo", &pv_trk_algo);
    tree->Branch("pv_trk_originalAlgo", &pv_trk_originalAlgo);
    tree->Branch("pv_trk_pvN", &pv_trk_pvN);

    tree->Branch("pv_trk_pvunbiased_IsValid", &pv_trk_pvunbiased_IsValid);
    tree->Branch("pv_trk_pvunbiased_IsFake", &pv_trk_pvunbiased_IsFake);
    tree->Branch("pv_trk_pvunbiased_NTracks", &pv_trk_pvunbiased_NTracks);
    tree->Branch("pv_trk_pvunbiased_SumTrackPt", &pv_trk_pvunbiased_SumTrackPt);
    tree->Branch("pv_trk_pvunbiased_SumTrackPt2", &pv_trk_pvunbiased_SumTrackPt2);
    tree->Branch("pv_trk_pvunbiased_fracHighPurity", &pv_trk_pvunbiased_fracHighPurity);
    tree->Branch("pv_trk_pvunbiased_chi2", &pv_trk_pvunbiased_chi2);
    tree->Branch("pv_trk_pvunbiased_ndof", &pv_trk_pvunbiased_ndof);
    tree->Branch("pv_trk_pvunbiased_x", &pv_trk_pvunbiased_x);
    tree->Branch("pv_trk_pvunbiased_y", &pv_trk_pvunbiased_y);
    tree->Branch("pv_trk_pvunbiased_z", &pv_trk_pvunbiased_z);
    tree->Branch("pv_trk_pvunbiased_xError", &pv_trk_pvunbiased_xError);
    tree->Branch("pv_trk_pvunbiased_yError", &pv_trk_pvunbiased_yError);
    tree->Branch("pv_trk_pvunbiased_zError", &pv_trk_pvunbiased_zError);

    tree->Branch("pv_trk_d0_pvunbiased", &pv_trk_d0_pvunbiased);
    tree->Branch("pv_trk_dz_pvunbiased", &pv_trk_dz_pvunbiased);
    tree->Branch("pv_trk_d0_bs_zpvunbiased", &pv_trk_d0_bs_zpvunbiased);

    tree->Branch("pv_trk_pt", &pv_trk_pt);
    tree->Branch("pv_trk_px", &pv_trk_px);
    tree->Branch("pv_trk_py", &pv_trk_py);
    tree->Branch("pv_trk_pz", &pv_trk_pz);
    tree->Branch("pv_trk_p", &pv_trk_p);
    tree->Branch("pv_trk_eta", &pv_trk_eta);
    tree->Branch("pv_trk_phi", &pv_trk_phi);

    tree->Branch("pv_trk_d0", &pv_trk_d0);
    tree->Branch("pv_trk_dz", &pv_trk_dz);
    tree->Branch("pv_trk_d0_pv", &pv_trk_d0_pv);
    tree->Branch("pv_trk_dz_pv", &pv_trk_dz_pv);
    tree->Branch("pv_trk_d0_bs", &pv_trk_d0_bs);
    tree->Branch("pv_trk_d0_bs_zpca", &pv_trk_d0_bs_zpca);
    tree->Branch("pv_trk_d0_bs_zpv", &pv_trk_d0_bs_zpv);
    tree->Branch("pv_trk_dz_bs", &pv_trk_dz_bs);
    tree->Branch("pv_trk_d0Err", &pv_trk_d0Err);
    tree->Branch("pv_trk_dzErr", &pv_trk_dzErr);

    tree->Branch("pv_IsValid_p1", &pv_IsValid_p1);
    tree->Branch("pv_IsFake_p1", &pv_IsFake_p1);
    tree->Branch("pv_NTracks_p1", &pv_NTracks_p1);
    tree->Branch("pv_SumTrackPt_p1", &pv_SumTrackPt_p1);
    tree->Branch("pv_SumTrackPt2_p1", &pv_SumTrackPt2_p1);
    tree->Branch("pv_fracHighPurity_p1", &pv_fracHighPurity_p1);
    tree->Branch("pv_chi2_p1", &pv_chi2_p1);
    tree->Branch("pv_ndof_p1", &pv_ndof_p1);
    tree->Branch("pv_x_p1", &pv_x_p1);
    tree->Branch("pv_y_p1", &pv_y_p1);
    tree->Branch("pv_z_p1", &pv_z_p1);
    tree->Branch("pv_xError_p1", &pv_xError_p1);
    tree->Branch("pv_yError_p1", &pv_yError_p1);
    tree->Branch("pv_zError_p1", &pv_zError_p1);

    tree->Branch("pv_IsValid_p2", &pv_IsValid_p2);
    tree->Branch("pv_IsFake_p2", &pv_IsFake_p2);
    tree->Branch("pv_NTracks_p2", &pv_NTracks_p2);
    tree->Branch("pv_SumTrackPt_p2", &pv_SumTrackPt_p2);
    tree->Branch("pv_SumTrackPt2_p2", &pv_SumTrackPt2_p2);
    tree->Branch("pv_fracHighPurity_p2", &pv_fracHighPurity_p2);
    tree->Branch("pv_chi2_p2", &pv_chi2_p2);
    tree->Branch("pv_ndof_p2", &pv_ndof_p2);
    tree->Branch("pv_x_p2", &pv_x_p2);
    tree->Branch("pv_y_p2", &pv_y_p2);
    tree->Branch("pv_z_p2", &pv_z_p2);
    tree->Branch("pv_xError_p2", &pv_xError_p2);
    tree->Branch("pv_yError_p2", &pv_yError_p2);
    tree->Branch("pv_zError_p2", &pv_zError_p2);
}
