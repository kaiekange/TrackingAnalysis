#include <vector>
#include <iostream>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "input_list.cc"

const double weight_preEE[] = {1, 0.221839575, 0.027052134, 0.012157346, 0.000675689, 5.52185E-05, 1.96163E-06, 5.98198E-07, 3.13786E-07, 4.68717E-08, 1.42529E-08, 3.32981E-09, 1.02914E-10};
const double weight_postEE[] = {1, 0.099155235, 0.040371105, 0.010722512, 0.000368968, 3.43437E-05, 7.28824E-06, 2.2931E-06, 1.26887E-06, 2.1858E-07, 5.41467E-08, 1.10481E-08, 5.69236E-10};

int xsecweight(){

    TFile * outfile = new TFile("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/tuples/JetHT_mc2022_redo.root", "RECREATE");
    TTree *outtree = mctree->CloneTree(0);

    float factor;
    outtree->Branch("PU_factor", &factor);

    TH1F * h_data = new TH1F("h_data", "", 92, 0.5, 92.5);
    TH1F * h_mc = new TH1F("h_mc", "", 92, 0.5, 92.5);

    datatree->Project("h_data", "ev_nPV");
    mctree->Project("h_mc", "ev_nPV");

    TH1F * h_ratio = (TH1F*)h_data->Clone("h_ratio");
    h_ratio->Divide(h_mc);
    
    std::vector<float> PU_fac;

    for(int i=0; i<92; i++){
        PU_fac.push_back(h_ratio->GetBinContent(i+1));
    }

    int nentries = mctree->GetEntries();

    for(int i=0; i<nentries; i++){
        mctree->GetEntry(i);

        factor = PU_fac[ev_nPV-1];

        outtree->Fill();
    }

    outtree->Write();
	delete outtree;
	outfile->Close();

    /* nlohmann::json splitparams; */
    /* splitparams["PU_factor"] = PU_fac; */

    /* std::ofstream outfile("../../json/JetHT/PU_factor.json"); */
    /* outfile << splitparams.dump(4); */
    /* outfile.close(); */

    return 0;
}
