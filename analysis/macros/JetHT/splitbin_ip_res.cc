#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "TFile.h"
#include "TTree.h"

int splitbin_ip_res(TString era) {

    ROOT::EnableImplicitMT();

    if(gSystem->AccessPathName("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_" + era)) gSystem->MakeDirectory("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_"+era);
    TFile *datafile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_data_"+era+".root");
    TTree *datatree = (TTree*)datafile->Get("mytree");

    std::vector<float> * mypv_trk_pt = nullptr;
    std::vector<float> * mypv_trk_eta = nullptr;
    std::vector<float> * mypv_trk_phi = nullptr;
    datatree->SetBranchAddress("pv_trk_pt", &mypv_trk_pt);
    datatree->SetBranchAddress("pv_trk_eta", &mypv_trk_eta);
    datatree->SetBranchAddress("pv_trk_phi", &mypv_trk_phi);

    std::vector<float> pv_trk_pt_vec;
    std::vector<float> pv_trk_eta_vec;
    std::vector<float> pv_trk_phi_vec;

    Long64_t nentries = datatree->GetEntries();
    for (Long64_t i=0; i<nentries; ++i) {
        datatree->GetEntry(i);
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
        for(Long64_t j=0; j<mypv_trk_phi->size(); ++j){
            if ( (mypv_trk_phi->at(j)) >= -3.1416 && (mypv_trk_phi->at(j) <= 3.1416) ){
                pv_trk_phi_vec.push_back(mypv_trk_phi->at(j));
            }
        }
    }
    
    datafile->Close();
    delete datafile;

    int nbins = 500;
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

    std::sort(pv_trk_phi_vec.begin(), pv_trk_phi_vec.end());
    std::vector<float> pv_trk_phi_binedges;
    pv_trk_phi_binedges.reserve(nbins + 1);
    pv_trk_phi_binedges.push_back(-3.1416f);
    for (int i=1; i<nbins; ++i) {
        size_t idx = static_cast<size_t>(i * (pv_trk_phi_vec.size() - 1) / nbins);
        pv_trk_phi_binedges.push_back(pv_trk_phi_vec[idx]);
    }
    pv_trk_phi_binedges.push_back(3.1416f);

    nlohmann::json splitparams;
    splitparams["pv_trk_pt"] = pv_trk_pt_binedges;
    splitparams["pv_trk_eta"] = pv_trk_eta_binedges;
    splitparams["pv_trk_phi"] = pv_trk_phi_binedges;

    std::ofstream outfile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_"+era+"/binning_ip_res.json");
    outfile << splitparams.dump(4);
    outfile.close();
    std::cout << "Binning saved to binning.json\n";


    return 0;
}
