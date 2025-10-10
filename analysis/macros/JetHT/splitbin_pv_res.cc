#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "TFile.h"
#include "TTree.h"

const int nbins = 50;

int splitbin_pv_res(TString era) {

    ROOT::EnableImplicitMT();

    if(gSystem->AccessPathName("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_" + era)) gSystem->MakeDirectory("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_"+era);
    TFile *mcfile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_"+era+"_corr.root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    TH1F *h_pv_SumTrackPt = new TH1F("h_pv_SumTrackPt", "", 500, 30, 400);
    mctree->Project("h_pv_SumTrackPt", "sqrt(pv_SumTrackPt2)", "xsecweight * PSweight * cell_mask * PU_factor");
    double perc[nbins+1];
    for (int i=0; i<nbins+1; i++) perc[i] = i / double(nbins);
    double q_pv_SumTrackPt[nbins+1];
    h_pv_SumTrackPt->GetQuantiles(nbins+1, q_pv_SumTrackPt, perc);

    nlohmann::json splitparams;
    splitparams["pv_SumTrackPt2_sqrt"] = std::vector<double>(q_pv_SumTrackPt, q_pv_SumTrackPt + nbins+1);

    std::ofstream outfile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_"+era+"/binning_pv_res.json");
    outfile << splitparams.dump(4);
    outfile.close();
    std::cout << "Binning saved to binning.json\n";
    
    mcfile->Close();
    delete mcfile;


    return 0;
}
