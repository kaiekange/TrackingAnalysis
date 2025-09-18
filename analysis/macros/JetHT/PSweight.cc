#include <iostream>
#include <TTree.h>
#include <TString.h>

const double weight_preEE[] = {6.79408E-05, 1.17731E-05, 1.65481E-05, 2.84464E-05, 6.17066E-05, 0.000138763, 0.000994453, 0.006185925, 0.087915573, 1};
const double weight_postEE[] = {9.25952E-05, 7.52549E-05, 0.000160302, 0.000189268, 0.000201043, 0.000334513, 0.001776837, 0.008781148, 0.109464725, 1};

int PSweight(TString era){

    TFile *infile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_"+era+"_xsec.root");
    TTree *intree = (TTree*)infile->Get("mytree");

    bool trig_PFHT1050_pass;
    bool trig_PFHT890_pass;
    bool trig_PFHT780_pass;
    bool trig_PFHT680_pass;
    bool trig_PFHT590_pass;
    bool trig_PFHT510_pass;
    bool trig_PFHT430_pass;
    bool trig_PFHT370_pass;
    bool trig_PFHT250_pass;
    bool trig_PFHT180_pass;
    intree->SetBranchAddress("trig_PFHT1050_pass", &trig_PFHT1050_pass);
    intree->SetBranchAddress("trig_PFHT890_pass", &trig_PFHT890_pass);
    intree->SetBranchAddress("trig_PFHT780_pass", &trig_PFHT780_pass);
    intree->SetBranchAddress("trig_PFHT680_pass", &trig_PFHT680_pass);
    intree->SetBranchAddress("trig_PFHT590_pass", &trig_PFHT590_pass);
    intree->SetBranchAddress("trig_PFHT510_pass", &trig_PFHT510_pass);
    intree->SetBranchAddress("trig_PFHT430_pass", &trig_PFHT430_pass);
    intree->SetBranchAddress("trig_PFHT370_pass", &trig_PFHT370_pass);
    intree->SetBranchAddress("trig_PFHT250_pass", &trig_PFHT250_pass);
    intree->SetBranchAddress("trig_PFHT180_pass", &trig_PFHT180_pass);

    TFile *outfile = new TFile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_"+era+"_PS.root", "RECREATE");
    TTree *outtree = intree->CloneTree(0);

    float PSweight;
    outtree->Branch("PSweight", &PSweight);

    const double* myweight = nullptr;
    if(era.Contains("preEE")) myweight = weight_preEE;
    else if(era.Contains("postEE")) myweight = weight_postEE;

    int nentries = intree->GetEntries();

    for(int i=0; i<nentries; i++){
        intree->GetEntry(i);
        if((!trig_PFHT1050_pass) && (!trig_PFHT890_pass) && (!trig_PFHT780_pass) && (!trig_PFHT680_pass) && (!trig_PFHT590_pass) && (!trig_PFHT510_pass) && (!trig_PFHT430_pass) && (!trig_PFHT370_pass) && (!trig_PFHT250_pass) && (!trig_PFHT180_pass)) continue; 

        if(trig_PFHT1050_pass) PSweight = myweight[0];
        else if(trig_PFHT890_pass) PSweight = myweight[1];
        else if(trig_PFHT780_pass) PSweight = myweight[2];
        else if(trig_PFHT680_pass) PSweight = myweight[3];
        else if(trig_PFHT590_pass) PSweight = myweight[4];
        else if(trig_PFHT510_pass) PSweight = myweight[5];
        else if(trig_PFHT430_pass) PSweight = myweight[6];
        else if(trig_PFHT370_pass) PSweight = myweight[7];
        else if(trig_PFHT250_pass) PSweight = myweight[8];
        else if(trig_PFHT180_pass) PSweight = myweight[9];
        else PSweight = 0;

        outtree->Fill();
    }

    outtree->Write();
    delete outtree;
    outfile->Close();

    return 0;
}
