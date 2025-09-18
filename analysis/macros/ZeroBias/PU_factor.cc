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

    int dataev_nPV;
    datatree->SetBranchAddress("ev_nPV", &dataev_nPV);
    int mcev_nPV;
    mctree->SetBranchAddress("ev_nPV", &mcev_nPV);

    TH1F * h_data = new TH1F("h_data", "", nbins, 0.5, nbins+0.5);
    TH1F * h_mc = new TH1F("h_mc", "", nbins, 0.5, nbins+0.5);

    datatree->Project("h_data", "ev_nPV");
    mctree->Project("h_mc", "ev_nPV");

    h_data->Scale(1./h_data->Integral());
    h_mc->Scale(1./h_mc->Integral());

    TH1F * h_ratio = (TH1F*)h_data->Clone("h_ratio");
    h_ratio->Divide(h_mc);
    
    std::vector<float> dataPU_fac;
    std::vector<float> mcPU_fac;

    for(int i=0; i<5; i++){
        if(era.Contains("preEE")){ 
            dataPU_fac.push_back(0);
            mcPU_fac.push_back(0);
        }
        else if(era.Contains("postEE")){
            dataPU_fac.push_back(1);
            mcPU_fac.push_back(h_ratio->GetBinContent(i+1));
        }
    }

    for(int i=5; i<nbins; i++){
        dataPU_fac.push_back(1);
        mcPU_fac.push_back(h_ratio->GetBinContent(i+1));
    }

    TFile *dataoutfile = new TFile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/all_skimmed_2022_data_"+era+"_corr.root", "RECREATE");
    TTree *dataouttree = datatree->CloneTree(0);
    float datafactor;
    dataouttree->Branch("PU_factor", &datafactor);
    for(int i=0; i<datatree->GetEntries(); i++){
        datatree->GetEntry(i);
        datafactor = dataPU_fac[dataev_nPV-1];
        dataouttree->Fill();
    }
    dataouttree->Write();
	delete dataouttree;
	dataoutfile->Close();

    TFile *mcoutfile = new TFile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/all_skimmed_2022_mc_"+era+"_corr.root", "RECREATE");
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
