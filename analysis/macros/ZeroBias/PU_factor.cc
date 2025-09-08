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

    TFile *datafile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/all_skimmed_2022_data_"+era+".root");
    TTree *datatree = (TTree*)datafile->Get("mytree");
    TFile *mcfile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/all_skimmed_2022_mc_"+era+".root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    int ev_nPV;
    mctree->SetBranchAddress("ev_nPV", &ev_nPV);

    TFile * outfile = new TFile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/all_skimmed_2022_mc_"+era+"_corr.root", "RECREATE");
    TTree *outtree = mctree->CloneTree(0);

    float factor;
    outtree->Branch("PU_factor", &factor);

    TH1F * h_data = new TH1F("h_data", "", nbins, 0.5, nbins+0.5);
    TH1F * h_mc = new TH1F("h_mc", "", nbins, 0.5, nbins+0.5);

    h_data->Scale(1./h_data->Integral());
    h_mc->Scale(1./h_mc->Integral());

    datatree->Project("h_data", "ev_nPV");
    mctree->Project("h_mc", "ev_nPV");

    TH1F * h_ratio = (TH1F*)h_data->Clone("h_ratio");
    h_ratio->Divide(h_mc);
    
    std::vector<float> PU_fac;

    for(int i=0; i<nbins; i++){
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

    return 0;
}
