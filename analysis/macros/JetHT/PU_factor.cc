#include <vector>
#include <iostream>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "input_list.cc"

int PU_factor(){

    TFile *datafile = TFile::Open("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/tuples/JetHT_data2022.root");
    TTree *datatree = (TTree*)datafile->Get("mytree");
    TFile *mcfile = TFile::Open("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/tuples/JetHT_mc2022.root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    int ev_nPV;
    mctree->SetBranchAddress("ev_nPV", &ev_nPV);

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

        factor = PU_fac[ev_nPV];

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
