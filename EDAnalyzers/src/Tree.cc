#include "TrackingAnalysis/EDAnalyzers/interface/Tree.h"

ResTree::ResTree(TTree *_tree)
{
    tree = _tree;
}

void ResTree::Init()
{
    ev_run = null;
    ev_id = null;
    ev_lumi = null;
    ev_bunchCrossing = null;
    ev_orbitNumber = null;
    ev_time = null;
    ev_rho = null;
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
    bs_emittanceX = null;
    bs_emittanceY = null;
    bs_betaStar = null;

    pv_IsValid.clear();
    pv_IsFake.clear();
    pv_NTracks.clear();
    pv_SumTrackPt.clear();
    pv_SumTrackPt2.clear();
    pv_fracHighPurity.clear();
    pv_chi2.clear();
    pv_ndof.clear();
    pv_x.clear();
    pv_y.clear();
    pv_z.clear();
    pv_xError.clear();
    pv_yError.clear();
    pv_zError.clear();

    pv_trk_weight.clear();
    pv_trk_isHighPurity.clear();
    pv_trk_algo.clear();
    pv_trk_originalAlgo.clear();
    pv_trk_idx.clear();
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

    pv_IsValid_p1.clear();
    pv_IsFake_p1.clear();
    pv_NTracks_p1.clear();
    pv_SumTrackPt_p1.clear();
    pv_SumTrackPt2_p1.clear();
    pv_fracHighPurity_p1.clear();
    pv_chi2_p1.clear();
    pv_ndof_p1.clear();
    pv_x_p1.clear();
    pv_y_p1.clear();
    pv_z_p1.clear();
    pv_xError_p1.clear();
    pv_yError_p1.clear();
    pv_zError_p1.clear();

    pv_IsValid_p2.clear();
    pv_IsFake_p2.clear();
    pv_NTracks_p2.clear();
    pv_SumTrackPt_p2.clear();
    pv_SumTrackPt2_p2.clear();
    pv_fracHighPurity_p2.clear();
    pv_chi2_p2.clear();
    pv_ndof_p2.clear();
    pv_x_p2.clear();
    pv_y_p2.clear();
    pv_z_p2.clear();
    pv_xError_p2.clear();
    pv_yError_p2.clear();
    pv_zError_p2.clear();
}

void ResTree::CreateBranches(Bool_t runOnData)
{
    tree->Branch("ev_run", &ev_run, "ev_run/I");
    tree->Branch("ev_id", &ev_id, "ev_id/I");
    tree->Branch("ev_lumi", &ev_lumi, "ev_lumi/I");
    tree->Branch("ev_bunchCrossing", &ev_bunchCrossing, "ev_bunchCrossing/I");
    tree->Branch("ev_orbitNumber", &ev_orbitNumber, "ev_orbitNumber/I");
    tree->Branch("ev_time", &ev_time, "ev_time/I");
    tree->Branch("ev_rho", &ev_rho, "ev_rho/F");
    tree->Branch("ev_nPV", &ev_nPV, "ev_nPV/I");

    tree->Branch("trig_ZeroBias_pass", &trig_ZeroBias_pass, "trig_ZeroBias_pass/O");
    tree->Branch("trig_PFHT180_pass", &trig_PFHT180_pass, "trig_PFHT180_pass/O");
    tree->Branch("trig_PFHT250_pass", &trig_PFHT250_pass, "trig_PFHT250_pass/O");
    tree->Branch("trig_PFHT370_pass", &trig_PFHT370_pass, "trig_PFHT370_pass/O");
    tree->Branch("trig_PFHT430_pass", &trig_PFHT430_pass, "trig_PFHT430_pass/O");
    tree->Branch("trig_PFHT510_pass", &trig_PFHT510_pass, "trig_PFHT510_pass/O");
    tree->Branch("trig_PFHT590_pass", &trig_PFHT590_pass, "trig_PFHT590_pass/O");
    tree->Branch("trig_PFHT680_pass", &trig_PFHT680_pass, "trig_PFHT680_pass/O");
    tree->Branch("trig_PFHT780_pass", &trig_PFHT780_pass, "trig_PFHT780_pass/O");
    tree->Branch("trig_PFHT890_pass", &trig_PFHT890_pass, "trig_PFHT890_pass/O");
    tree->Branch("trig_PFHT1050_pass", &trig_PFHT1050_pass, "trig_PFHT1050_pass/O");
    tree->Branch("trig_PFHT350_pass", &trig_PFHT350_pass, "trig_PFHT350_pass/O");

    if (!runOnData)
    {
        tree->Branch("NumTrueInts", &NumTrueInts, "NumTrueInts/I");
        tree->Branch("NumPUInts", &NumTrueInts, "NumPUInts/I");
    }

    tree->Branch("bs_type", &bs_type, "bs_type/I");
    tree->Branch("bs_x0", &bs_x0, "bs_x0/F");
    tree->Branch("bs_y0", &bs_y0, "bs_y0/F");
    tree->Branch("bs_z0", &bs_z0, "bs_z0/F");
    tree->Branch("bs_x_zpv", &bs_x_zpv, "bs_x_zpv/F");
    tree->Branch("bs_y_zpv", &bs_y_zpv, "bs_y_zpv/F");
    tree->Branch("bs_sigmaZ", &bs_sigmaZ, "bs_sigmaZ/F");
    tree->Branch("bs_dxdz", &bs_dxdz, "bs_dxdz/F");
    tree->Branch("bs_dydz", &bs_dydz, "bs_dydz/F");
    tree->Branch("bs_BeamWidthX", &bs_BeamWidthX, "bs_BeamWidthX/F");
    tree->Branch("bs_BeamWidthY", &bs_BeamWidthY, "bs_BeamWidthY/F");
    tree->Branch("bs_x0Error", &bs_x0Error, "bs_x0Error/F");
    tree->Branch("bs_y0Error", &bs_y0Error, "bs_y0Error/F");
    tree->Branch("bs_z0Error", &bs_z0Error, "bs_z0Error/F");
    tree->Branch("bs_sigmaZ0Error", &bs_sigmaZ0Error, "bs_sigmaZ0Error/F");
    tree->Branch("bs_dxdzError", &bs_dxdzError, "bs_dxdzError/F");
    tree->Branch("bs_dydzError", &bs_dydzError, "bs_dydzError/F");
    tree->Branch("bs_BeamWidthXError", &bs_BeamWidthXError, "bs_BeamWidthXError/F");
    tree->Branch("bs_BeamWidthYError", &bs_BeamWidthYError, "bs_BeamWidthYError/F");
    tree->Branch("bs_emittanceX", &bs_emittanceX, "bs_emittanceX/F");
    tree->Branch("bs_emittanceY", &bs_emittanceY, "bs_emittanceY/F");
    tree->Branch("bs_betaStar", &bs_betaStar, "bs_betaStar/F");

    tree->Branch("pv_IsValid", "std::vector<Bool_t>", &pv_IsValid);
    tree->Branch("pv_IsFake", "std::vector<Bool_t>", &pv_IsFake);
    tree->Branch("pv_NTracks", "std::vector<Int_t>", &pv_NTracks);
    tree->Branch("pv_SumTrackPt", "std::vector<Float_t>", &pv_SumTrackPt);
    tree->Branch("pv_SumTrackPt2", "std::vector<Float_t>", &pv_SumTrackPt2);
    tree->Branch("pv_fracHighPurity", "std::vector<Float_t>", &pv_fracHighPurity);
    tree->Branch("pv_chi2", "std::vector<Float_t>", &pv_chi2);
    tree->Branch("pv_ndof", "std::vector<Int_t>", &pv_ndof);
    tree->Branch("pv_x", "std::vector<Float_t>", &pv_x);
    tree->Branch("pv_y", "std::vector<Float_t>", &pv_y);
    tree->Branch("pv_z", "std::vector<Float_t>", &pv_z);
    tree->Branch("pv_xError", "std::vector<Float_t>", &pv_xError);
    tree->Branch("pv_yError", "std::vector<Float_t>", &pv_yError);
    tree->Branch("pv_zError", "std::vector<Float_t>", &pv_zError);

    tree->Branch("pv_trk_weight", "std::vector<std::vector<Float_t>>", &pv_trk_weight);
    tree->Branch("pv_trk_isHighPurity", "std::vector<std::vector<Bool_t>>", &pv_trk_isHighPurity);
    tree->Branch("pv_trk_algo", "std::vector<std::vector<Int_t>>", &pv_trk_algo);
    tree->Branch("pv_trk_originalAlgo", "std::vector<std::vector<Int_t>>", &pv_trk_originalAlgo);
    tree->Branch("pv_trk_idx", "std::vector<std::vector<Int_t>>", &pv_trk_idx);
    tree->Branch("pv_trk_pvN", "std::vector<std::vector<Int_t>>", &pv_trk_pvN);

    tree->Branch("pv_trk_pvunbiased_IsValid", "std::vector<std::vector<Bool_t>>", &pv_trk_pvunbiased_IsValid);
    tree->Branch("pv_trk_pvunbiased_IsFake", "std::vector<std::vector<Bool_t>>", &pv_trk_pvunbiased_IsFake);
    tree->Branch("pv_trk_pvunbiased_NTracks", "std::vector<std::vector<Int_t>>", &pv_trk_pvunbiased_NTracks);
    tree->Branch("pv_trk_pvunbiased_SumTrackPt", "std::vector<std::vector<Float_t>>", &pv_trk_pvunbiased_SumTrackPt);
    tree->Branch("pv_trk_pvunbiased_SumTrackPt2", "std::vector<std::vector<Float_t>>", &pv_trk_pvunbiased_SumTrackPt2);
    tree->Branch("pv_trk_pvunbiased_fracHighPurity", "std::vector<std::vector<Float_t>>", &pv_trk_pvunbiased_fracHighPurity);
    tree->Branch("pv_trk_pvunbiased_chi2", "std::vector<std::vector<Float_t>>", &pv_trk_pvunbiased_chi2);
    tree->Branch("pv_trk_pvunbiased_ndof", "std::vector<std::vector<Int_t>>", &pv_trk_pvunbiased_ndof);
    tree->Branch("pv_trk_pvunbiased_x", "std::vector<std::vector<Float_t>>", &pv_trk_pvunbiased_x);
    tree->Branch("pv_trk_pvunbiased_y", "std::vector<std::vector<Float_t>>", &pv_trk_pvunbiased_y);
    tree->Branch("pv_trk_pvunbiased_z", "std::vector<std::vector<Float_t>>", &pv_trk_pvunbiased_z);
    tree->Branch("pv_trk_pvunbiased_xError", "std::vector<std::vector<Float_t>>", &pv_trk_pvunbiased_xError);
    tree->Branch("pv_trk_pvunbiased_yError", "std::vector<std::vector<Float_t>>", &pv_trk_pvunbiased_yError);
    tree->Branch("pv_trk_pvunbiased_zError", "std::vector<std::vector<Float_t>>", &pv_trk_pvunbiased_zError);

    tree->Branch("pv_trk_d0_pvunbiased", "std::vector<std::vector<Float_t>>", &pv_trk_d0_pvunbiased);
    tree->Branch("pv_trk_dz_pvunbiased", "std::vector<std::vector<Float_t>>", &pv_trk_dz_pvunbiased);
    tree->Branch("pv_trk_d0_bs_zpvunbiased", "std::vector<std::vector<Float_t>>", &pv_trk_d0_bs_zpvunbiased);

    tree->Branch("pv_trk_pt", "std::vector<std::vector<Float_t>>", &pv_trk_pt);
    tree->Branch("pv_trk_px", "std::vector<std::vector<Float_t>>", &pv_trk_px);
    tree->Branch("pv_trk_py", "std::vector<std::vector<Float_t>>", &pv_trk_py);
    tree->Branch("pv_trk_pz", "std::vector<std::vector<Float_t>>", &pv_trk_pz);
    tree->Branch("pv_trk_p", "std::vector<std::vector<Float_t>>", &pv_trk_p);
    tree->Branch("pv_trk_eta", "std::vector<std::vector<Float_t>>", &pv_trk_eta);
    tree->Branch("pv_trk_phi", "std::vector<std::vector<Float_t>>", &pv_trk_phi);

    tree->Branch("pv_trk_d0", "std::vector<std::vector<Float_t>>", &pv_trk_d0);
    tree->Branch("pv_trk_dz", "std::vector<std::vector<Float_t>>", &pv_trk_dz);
    tree->Branch("pv_trk_d0_pv", "std::vector<std::vector<Float_t>>", &pv_trk_d0_pv);
    tree->Branch("pv_trk_dz_pv", "std::vector<std::vector<Float_t>>", &pv_trk_dz_pv);
    tree->Branch("pv_trk_d0_bs", "std::vector<std::vector<Float_t>>", &pv_trk_d0_bs);
    tree->Branch("pv_trk_d0_bs_zpca", "std::vector<std::vector<Float_t>>", &pv_trk_d0_bs_zpca);
    tree->Branch("pv_trk_d0_bs_zpv", "std::vector<std::vector<Float_t>>", &pv_trk_d0_bs_zpv);
    tree->Branch("pv_trk_dz_bs", "std::vector<std::vector<Float_t>>", &pv_trk_dz_bs);
    tree->Branch("pv_trk_d0Err", "std::vector<std::vector<Float_t>>", &pv_trk_d0Err);
    tree->Branch("pv_trk_dzErr", "std::vector<std::vector<Float_t>>", &pv_trk_dzErr);

    tree->Branch("pv_IsValid_p1", "std::vector<Bool_t>", &pv_IsValid_p1);
    tree->Branch("pv_IsFake_p1", "std::vector<Bool_t>", &pv_IsFake_p1);
    tree->Branch("pv_NTracks_p1", "std::vector<Int_t>", &pv_NTracks_p1);
    tree->Branch("pv_SumTrackPt_p1", "std::vector<Float_t>", &pv_SumTrackPt_p1);
    tree->Branch("pv_SumTrackPt2_p1", "std::vector<Float_t>", &pv_SumTrackPt2_p1);
    tree->Branch("pv_fracHighPurity_p1", "std::vector<Float_t>", &pv_fracHighPurity_p1);
    tree->Branch("pv_chi2_p1", "std::vector<Float_t>", &pv_chi2_p1);
    tree->Branch("pv_ndof_p1", "std::vector<Int_t>", &pv_ndof_p1);
    tree->Branch("pv_x_p1", "std::vector<Float_t>", &pv_x_p1);
    tree->Branch("pv_y_p1", "std::vector<Float_t>", &pv_y_p1);
    tree->Branch("pv_z_p1", "std::vector<Float_t>", &pv_z_p1);
    tree->Branch("pv_xError_p1", "std::vector<Float_t>", &pv_xError_p1);
    tree->Branch("pv_yError_p1", "std::vector<Float_t>", &pv_yError_p1);
    tree->Branch("pv_zError_p1", "std::vector<Float_t>", &pv_zError_p1);

    tree->Branch("pv_IsValid_p2", "std::vector<Bool_t>", &pv_IsValid_p2);
    tree->Branch("pv_IsFake_p2", "std::vector<Bool_t>", &pv_IsFake_p2);
    tree->Branch("pv_NTracks_p2", "std::vector<Int_t>", &pv_NTracks_p2);
    tree->Branch("pv_SumTrackPt_p2", "std::vector<Float_t>", &pv_SumTrackPt_p2);
    tree->Branch("pv_SumTrackPt2_p2", "std::vector<Float_t>", &pv_SumTrackPt2_p2);
    tree->Branch("pv_fracHighPurity_p2", "std::vector<Float_t>", &pv_fracHighPurity_p2);
    tree->Branch("pv_chi2_p2", "std::vector<Float_t>", &pv_chi2_p2);
    tree->Branch("pv_ndof_p2", "std::vector<Int_t>", &pv_ndof_p2);
    tree->Branch("pv_x_p2", "std::vector<Float_t>", &pv_x_p2);
    tree->Branch("pv_y_p2", "std::vector<Float_t>", &pv_y_p2);
    tree->Branch("pv_z_p2", "std::vector<Float_t>", &pv_z_p2);
    tree->Branch("pv_xError_p2", "std::vector<Float_t>", &pv_xError_p2);
    tree->Branch("pv_yError_p2", "std::vector<Float_t>", &pv_yError_p2);
    tree->Branch("pv_zError_p2", "std::vector<Float_t>", &pv_zError_p2);
}
