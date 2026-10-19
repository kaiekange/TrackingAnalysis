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
    Int_t ev_lumi;
    Int_t ev_id;
    Int_t ev_nPV;

    Bool_t trig_ZeroBias_pass;  // used for the event selection only, not stored
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

    Int_t NumTrueInts;

    // beam spot (lengths in micron)
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

    // split vertices (positions in micron, covariances in micron^2)
    Bool_t pv_IsValid_p1;
    Bool_t pv_IsFake_p1;
    Float_t pv_SumTrackPt2_p1;
    Float_t pv_x_p1;
    Float_t pv_y_p1;
    Float_t pv_z_p1;
    Float_t pv_xError_p1;
    Float_t pv_yError_p1;
    Float_t pv_zError_p1;
    Float_t pv_covXY_p1;
    Float_t pv_covXZ_p1;
    Float_t pv_covYZ_p1;

    Bool_t pv_IsValid_p2;
    Bool_t pv_IsFake_p2;
    Float_t pv_SumTrackPt2_p2;
    Float_t pv_x_p2;
    Float_t pv_y_p2;
    Float_t pv_z_p2;
    Float_t pv_xError_p2;
    Float_t pv_yError_p2;
    Float_t pv_zError_p2;
    Float_t pv_covXY_p2;
    Float_t pv_covXZ_p2;
    Float_t pv_covYZ_p2;

    // refitted PV
    Bool_t pv_IsValid;
    Bool_t pv_IsFake;
    Int_t pv_NTracks;
    Float_t pv_SumTrackPt;
    Float_t pv_SumTrackPt2;
    Float_t pv_chi2;
    Int_t pv_ndof;
    Float_t pv_x;
    Float_t pv_y;
    Float_t pv_z;
    Float_t pv_xError;
    Float_t pv_yError;
    Float_t pv_zError;

    // PV tracks
    std::vector<Int_t> pv_trk_pvAssociationQuality;
    std::vector<Bool_t> pv_trk_isHighPurity;
    std::vector<Int_t> pv_trk_charge;
    std::vector<Int_t> pv_trk_nTrackerLayers;
    std::vector<Int_t> pv_trk_nPixelBarrelLayers;
    std::vector<Int_t> pv_trk_nPixelEndcapLayers;
    std::vector<Int_t> pv_trk_nValidPixelHits;
    std::vector<Int_t> pv_trk_nMissedIn;
    std::vector<Bool_t> pv_trk_hasPixelBarrelLayer1;
    std::vector<Bool_t> pv_trk_hasPixelEndcapLayer1;

    // PV refitted without the track (positions in micron, covariances in micron^2)
    std::vector<Bool_t> pv_trk_pvunbiased_IsValid;
    std::vector<Bool_t> pv_trk_pvunbiased_IsFake;
    std::vector<Float_t> pv_trk_pvunbiased_SumTrackPt2;
    std::vector<Float_t> pv_trk_pvunbiased_xError;
    std::vector<Float_t> pv_trk_pvunbiased_yError;
    std::vector<Float_t> pv_trk_pvunbiased_zError;
    std::vector<Float_t> pv_trk_pvunbiased_covXY;
    std::vector<Float_t> pv_trk_pvunbiased_covXZ;
    std::vector<Float_t> pv_trk_pvunbiased_covYZ;

    std::vector<Float_t> pv_trk_d0_pvunbiased;
    std::vector<Float_t> pv_trk_dz_pvunbiased;

    std::vector<Float_t> pv_trk_pt;
    std::vector<Float_t> pv_trk_eta;
    std::vector<Float_t> pv_trk_phi;

    std::vector<Float_t> pv_trk_d0Err;
    std::vector<Float_t> pv_trk_dzErr;
};

#endif
