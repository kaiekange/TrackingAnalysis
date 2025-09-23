#include <iostream>
#include <TTree.h>
#include <TString.h>

const double weight_preEE[] = {14718.68741, 84939.43179, 60429.87469, 35153.78198, 16205.72735, 7206.506941, 1005.578246, 161.6573003, 11.37454914, 1};
    
const double weight_postEE[] = {10799.6928, 13288.17665, 6238.220023, 5283.509487, 4974.061986, 2989.423651, 562.7977765, 113.8803247, 9.135362991, 1};
    

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
