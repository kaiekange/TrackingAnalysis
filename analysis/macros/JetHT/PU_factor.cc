#include <vector>
#include <iostream>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>
#include <algorithm>
#include <nlohmann/json.hpp>

const int nbins = 90;

int PU_factor(TString era){

    TFile *datafile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_data_"+era+".root");
    TTree *datatree = (TTree*)datafile->Get("mytree");
    TFile *mcfile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_"+era+"_PS.root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    int mcev_nPV;
    mctree->SetBranchAddress("ev_nPV", &mcev_nPV);

    TH1F * h_data = new TH1F("h_data", "", nbins, 0.5, nbins+0.5);
    TH1F * h_mc = new TH1F("h_mc", "", nbins, 0.5, nbins+0.5);

    datatree->Project("h_data", "ev_nPV");
    mctree->Project("h_mc", "ev_nPV", "xsecweight * PSweight");

    h_data->Scale(1./h_data->Integral());
    h_mc->Scale(1./h_mc->Integral());

    TH1F * h_ratio = (TH1F*)h_data->Clone("h_ratio");
    h_ratio->Divide(h_mc);
    
    std::vector<float> mcPU_fac;

    for(int i=0; i<nbins; i++){
        mcPU_fac.push_back(h_ratio->GetBinContent(i+1));
    }

    TFile *mcoutfile = new TFile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_"+era+"_corr.root", "RECREATE");
    TTree *mcouttree = mctree->CloneTree(0);
    float mcfactor;
    mcouttree->Branch("PU_factor", &mcfactor);
    for(int i=0; i<mctree->GetEntries(); i++){
        mctree->GetEntry(i);
        mcfactor = mcPU_fac[mcev_nPV-1];
        mcouttree->Fill();
    }
    mcouttree->Write();
	delete mcouttree;
	mcoutfile->Close();

    return 0;
}
