#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "TFile.h"
#include "TTree.h"

int splitbin() {

    if(gSystem->AccessPathName("../../json")) gSystem->MakeDirectory("../../json");
    if(gSystem->AccessPathName("../../json/JetHT")) gSystem->MakeDirectory("../../json/JetHT");
    TFile *myfile = TFile::Open("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/tuples/JetHT_data2022.root");

    TTree *mytree = (TTree*)myfile->Get("mytree");

    float mypv_SumTrackPt2;
    std::vector<float> * mypv_trk_pt = nullptr; 
    std::vector<float> * mypv_trk_eta = nullptr; 
    mytree->SetBranchAddress("pv_SumTrackPt2", &mypv_SumTrackPt2);
    mytree->SetBranchAddress("pv_trk_pt", &mypv_trk_pt);
    mytree->SetBranchAddress("pv_trk_eta", &mypv_trk_eta);

    std::vector<float> pv_SumTrackPt_vec;
    std::vector<float> pv_trk_pt_vec;
    std::vector<float> pv_trk_eta_vec;

    Long64_t nentries = mytree->GetEntries();

    for (Long64_t i=0; i<nentries; ++i) {
        mytree->GetEntry(i);

        if ( (std::sqrt(mypv_SumTrackPt2)) >= 5 && (std::sqrt(mypv_SumTrackPt2) <= 300) ){
            pv_SumTrackPt_vec.push_back(std::sqrt(mypv_SumTrackPt2));
        }

        for(Long64_t j=0; j<mypv_trk_pt->size(); ++j){
            if ( (mypv_trk_pt->at(j)) >= 0.1 && (mypv_trk_pt->at(j) <= 15) ){
                pv_trk_pt_vec.push_back(mypv_trk_pt->at(j));
            }
        }

        for(Long64_t j=0; j<mypv_trk_eta->size(); ++j){
            if ( (mypv_trk_eta->at(j)) >= -3 && (mypv_trk_eta->at(j) <= 3) ){
                pv_trk_eta_vec.push_back(mypv_trk_eta->at(j));
            }
        }
    }

    int nbins = 20;
    std::sort(pv_SumTrackPt_vec.begin(), pv_SumTrackPt_vec.end());
    std::vector<float> pv_SumTrackPt_binedges;
    pv_SumTrackPt_binedges.reserve(nbins + 1);
    pv_SumTrackPt_binedges.push_back(5.0f);
    for (int i=1; i<nbins; ++i) {
        size_t idx = static_cast<size_t>(i * (pv_SumTrackPt_vec.size() - 1) / nbins);
        pv_SumTrackPt_binedges.push_back(pv_SumTrackPt_vec[idx]);
    }
    pv_SumTrackPt_binedges.push_back(300.0f);

    nbins = 100;
    std::sort(pv_trk_pt_vec.begin(), pv_trk_pt_vec.end());
    std::vector<float> pv_trk_pt_binedges;
    pv_trk_pt_binedges.reserve(nbins + 1);
    pv_trk_pt_binedges.push_back(0.1f);
    for (int i=1; i<nbins; ++i) {
        size_t idx = static_cast<size_t>(i * (pv_trk_pt_vec.size() - 1) / nbins);
        pv_trk_pt_binedges.push_back(pv_trk_pt_vec[idx]);
    }
    pv_trk_pt_binedges.push_back(15.0f);

    std::sort(pv_trk_eta_vec.begin(), pv_trk_eta_vec.end());
    std::vector<float> pv_trk_eta_binedges;
    pv_trk_eta_binedges.reserve(nbins + 1);
    pv_trk_eta_binedges.push_back(-3.0f);
    for (int i=1; i<nbins; ++i) {
        size_t idx = static_cast<size_t>(i * (pv_trk_eta_vec.size() - 1) / nbins);
        pv_trk_eta_binedges.push_back(pv_trk_eta_vec[idx]);
    }
    pv_trk_eta_binedges.push_back(3.0f);

    nlohmann::json splitparams;
    splitparams["pv_SumTrackPt2_sqrt"] = pv_SumTrackPt_binedges;
    splitparams["pv_trk_pt"] = pv_trk_pt_binedges;
    splitparams["pv_trk_eta"] = pv_trk_eta_binedges;

    std::ofstream outfile("../../json/JetHT/binning.json");
    outfile << splitparams.dump(4);
    outfile.close();
    std::cout << "Binning saved to binning.json\n";

    myfile->Close();
    delete myfile;

    return 0;
}
