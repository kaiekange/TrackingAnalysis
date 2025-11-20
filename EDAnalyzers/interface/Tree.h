#ifndef TREE_H
#define TREE_H

#include <TTree.h>
#include <string>
#include <iostream>
#include <vector>

#define null -777

class ResTree
{

public:
    ResTree(TTree *_tree);
    ~ResTree() {};

    TTree *tree;

    void Init();
    void CreateBranches(Bool_t runOnData);

    Int_t ev_run;
    Int_t ev_id;
    Int_t ev_lumi;
    Int_t ev_bunchCrossing;
    Int_t ev_orbitNumber;
    ULong_t ev_time;
    Float_t ev_rho;
    Int_t ev_nPV;

    Bool_t trig_ZeroBias_pass;
    Bool_t trig_PFHT180_pass;
    Bool_t trig_PFHT250_pass;
    Bool_t trig_PFHT370_pass;
    Bool_t trig_PFHT430_pass;
    Bool_t trig_PFHT510_pass;
    Bool_t trig_PFHT590_pass;
    Bool_t trig_PFHT680_pass;
    Bool_t trig_PFHT780_pass;
    Bool_t trig_PFHT890_pass;
    Bool_t trig_PFHT1050_pass;
    Bool_t trig_PFHT350_pass;

    Int_t NumTrueInts;
    Int_t NumPUInts;

    Int_t bs_type;
    Float_t bs_x0;
    Float_t bs_y0;
    Float_t bs_z0;
    Float_t bs_x_zpv;
    Float_t bs_y_zpv;
    Float_t bs_sigmaZ;
    Float_t bs_dxdz;
    Float_t bs_dydz;
    Float_t bs_BeamWidthX;
    Float_t bs_BeamWidthY;
    Float_t bs_x0Error;
    Float_t bs_y0Error;
    Float_t bs_z0Error;
    Float_t bs_sigmaZ0Error;
    Float_t bs_dxdzError;
    Float_t bs_dydzError;
    Float_t bs_BeamWidthXError;
    Float_t bs_BeamWidthYError;
    Float_t bs_emittanceX;
    Float_t bs_emittanceY;
    Float_t bs_betaStar;

    std::vector<Bool_t> pv_IsValid;
    std::vector<Bool_t> pv_IsFake;
    std::vector<Int_t> pv_NTracks;
    std::vector<Float_t> pv_SumTrackPt;
    std::vector<Float_t> pv_SumTrackPt2;
    std::vector<Float_t> pv_fracHighPurity;
    std::vector<Float_t> pv_chi2;
    std::vector<Int_t> pv_ndof;
    std::vector<Float_t> pv_x;
    std::vector<Float_t> pv_y;
    std::vector<Float_t> pv_z;
    std::vector<Float_t> pv_xError;
    std::vector<Float_t> pv_yError;
    std::vector<Float_t> pv_zError;

    std::vector<std::vector<Float_t>> pv_trk_weight;
    std::vector<std::vector<Bool_t>> pv_trk_isHighPurity;
    std::vector<std::vector<Int_t>> pv_trk_algo;
    std::vector<std::vector<Int_t>> pv_trk_originalAlgo;
    std::vector<std::vector<Int_t>> pv_trk_idx;
    std::vector<std::vector<Int_t>> pv_trk_pvN;

    std::vector<std::vector<Bool_t>> pv_trk_pvunbiased_IsValid;
    std::vector<std::vector<Bool_t>> pv_trk_pvunbiased_IsFake;
    std::vector<std::vector<Int_t>> pv_trk_pvunbiased_NTracks;
    std::vector<std::vector<Float_t>> pv_trk_pvunbiased_SumTrackPt;
    std::vector<std::vector<Float_t>> pv_trk_pvunbiased_SumTrackPt2;
    std::vector<std::vector<Float_t>> pv_trk_pvunbiased_fracHighPurity;
    std::vector<std::vector<Float_t>> pv_trk_pvunbiased_chi2;
    std::vector<std::vector<Int_t>> pv_trk_pvunbiased_ndof;
    std::vector<std::vector<Float_t>> pv_trk_pvunbiased_x;
    std::vector<std::vector<Float_t>> pv_trk_pvunbiased_y;
    std::vector<std::vector<Float_t>> pv_trk_pvunbiased_z;
    std::vector<std::vector<Float_t>> pv_trk_pvunbiased_xError;
    std::vector<std::vector<Float_t>> pv_trk_pvunbiased_yError;
    std::vector<std::vector<Float_t>> pv_trk_pvunbiased_zError;

    std::vector<std::vector<Float_t>> pv_trk_d0_pvunbiased;
    std::vector<std::vector<Float_t>> pv_trk_dz_pvunbiased;
    std::vector<std::vector<Float_t>> pv_trk_d0_bs_zpvunbiased;

    std::vector<std::vector<Float_t>> pv_trk_pt;
    std::vector<std::vector<Float_t>> pv_trk_px;
    std::vector<std::vector<Float_t>> pv_trk_py;
    std::vector<std::vector<Float_t>> pv_trk_pz;
    std::vector<std::vector<Float_t>> pv_trk_p;
    std::vector<std::vector<Float_t>> pv_trk_eta;
    std::vector<std::vector<Float_t>> pv_trk_phi;

    std::vector<std::vector<Float_t>> pv_trk_d0;
    std::vector<std::vector<Float_t>> pv_trk_dz;
    std::vector<std::vector<Float_t>> pv_trk_d0_pv;
    std::vector<std::vector<Float_t>> pv_trk_dz_pv;
    std::vector<std::vector<Float_t>> pv_trk_d0_bs;
    std::vector<std::vector<Float_t>> pv_trk_d0_bs_zpca;
    std::vector<std::vector<Float_t>> pv_trk_d0_bs_zpv;
    std::vector<std::vector<Float_t>> pv_trk_dz_bs;
    std::vector<std::vector<Float_t>> pv_trk_d0Err;
    std::vector<std::vector<Float_t>> pv_trk_dzErr;

    std::vector<Bool_t> pv_IsValid_p1;
    std::vector<Bool_t> pv_IsFake_p1;
    std::vector<Int_t> pv_NTracks_p1;
    std::vector<Float_t> pv_SumTrackPt_p1;
    std::vector<Float_t> pv_SumTrackPt2_p1;
    std::vector<Float_t> pv_fracHighPurity_p1;
    std::vector<Float_t> pv_chi2_p1;
    std::vector<Int_t> pv_ndof_p1;
    std::vector<Float_t> pv_x_p1;
    std::vector<Float_t> pv_y_p1;
    std::vector<Float_t> pv_z_p1;
    std::vector<Float_t> pv_xError_p1;
    std::vector<Float_t> pv_yError_p1;
    std::vector<Float_t> pv_zError_p1;

    std::vector<Bool_t> pv_IsValid_p2;
    std::vector<Bool_t> pv_IsFake_p2;
    std::vector<Int_t> pv_NTracks_p2;
    std::vector<Float_t> pv_SumTrackPt_p2;
    std::vector<Float_t> pv_SumTrackPt2_p2;
    std::vector<Float_t> pv_fracHighPurity_p2;
    std::vector<Float_t> pv_chi2_p2;
    std::vector<Int_t> pv_ndof_p2;
    std::vector<Float_t> pv_x_p2;
    std::vector<Float_t> pv_y_p2;
    std::vector<Float_t> pv_z_p2;
    std::vector<Float_t> pv_xError_p2;
    std::vector<Float_t> pv_yError_p2;
    std::vector<Float_t> pv_zError_p2;
};

#endif