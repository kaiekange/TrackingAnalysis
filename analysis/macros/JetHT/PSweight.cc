#include <iostream>
#include <TTree.h>
#include <TString.h>

const double weight_preEE[] = {
    35317389.8287109,
    1619937.38973069,
    491789.493013329,
    126703.898944331,
    33011.5278127328,
    8680.56547125817,
    1005.49000424629,
    161.649821030966,
    11.3732049797148,
    1
};

const double weight_postEE[] = {
    25156407.3397797,
    1112202.78668804,
    338933.742668619,
    86444.5820867605,
    22385.0317691743,
    5893.01382483904,
    693.246875343215,
    113.90150050473,
    9.13497706877434,
    1
}; 

int PSweight(TString era, TString PTrange){

    TFile *infile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_xsec/all_skimmed_2022_mc_"+PTrange+"_"+era+"_xsec.root");
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

    TFile *outfile = new TFile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_PS/all_skimmed_2022_mc_"+PTrange+"_"+era+"_PS.root", "RECREATE");
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

        outtree->Fill();
    }

    outtree->Write();
    delete outtree;
    outfile->Close();

    return 0;
}
