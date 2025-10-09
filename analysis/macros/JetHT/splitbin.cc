#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "TFile.h"
#include "TTree.h"

int splitbin(TString era) {

    if(gSystem->AccessPathName("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_" + era)) gSystem->MakeDirectory("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_"+era);
    TFile *myfile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_"+era+"_corr.root");

    TTree *mytree = (TTree*)myfile->Get("mytree");

    TH1F *h_pv_SumTrackPt = new TH1F("h_pv_SumTrackPt", "", 500, 30, 400);
    /* TH1F *h_pv_trk_pt = new TH1F("h_pv_trk_pt", "", 500, 0.1, 15); */
    /* TH1F *h_pv_trk_eta = new TH1F("h_pv_trk_eta", "", 500, -3, 3); */
    /* TH1F *h_pv_trk_phi = new TH1F("h_pv_trk_phi", "", 500, -3.1416, 3.1416); */

    mytree->Project("h_pv_SumTrackPt", "sqrt(pv_SumTrackPt2)", "xsecweight * PSweight * PU_factor");
    /* mytree->Project("h_pv_trk_pt", "pv_trk_pt", "xsecweight * PSweight * PU_factor"); */
    /* mytree->Project("h_pv_trk_eta", "pv_trk_pt", "xsecweight * PSweight * PU_factor"); */
    /* mytree->Project("h_pv_trk_phi", "pv_trk_phi", "xsecweight * PSweight * PU_factor"); */
    
    double p30[31];
    for (int i=0; i<31; i++) p30[i] = i / 30.0;
    /* double p100[101]; */
    /* for (int i=0; i<101; i++) p100[i] = i / 100.0; */
    
    double q_pv_SumTrackPt[31];
    h_pv_SumTrackPt->GetQuantiles(31, q_pv_SumTrackPt, p30);
    /* double q_pv_trk_pt[101]; */
    /* h_pv_trk_pt->GetQuantiles(101, q_pv_trk_pt, p100); */
    /* double q_pv_trk_eta[101]; */
    /* h_pv_trk_eta->GetQuantiles(101, q_pv_trk_eta, p100); */
    /* double q_pv_trk_phi[101]; */
    /* h_pv_trk_phi->GetQuantiles(101, q_pv_trk_phi, p100); */

    nlohmann::json splitparams;
    splitparams["pv_SumTrackPt2_sqrt"] = std::vector<double>(q_pv_SumTrackPt, q_pv_SumTrackPt + 31);
    /* splitparams["pv_trk_pt"] = std::vector<double>(q_pv_trk_pt, q_pv_trk_pt + 101); */
    /* splitparams["pv_trk_eta"] = std::vector<double>(q_pv_trk_eta, q_pv_trk_eta + 101); */
    /* splitparams["pv_trk_phi"] = std::vector<double>(q_pv_trk_phi, q_pv_trk_phi + 101); */

    std::ofstream outfile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_"+era+"/binning.json");
    outfile << splitparams.dump(4);
    outfile.close();
    std::cout << "Binning saved to binning.json\n";

    myfile->Close();
    delete myfile;

    return 0;
}
