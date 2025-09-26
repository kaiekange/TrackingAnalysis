#include <iostream>
#include <TString.h>
#include <ROOT/RDataFrame.hxx>

int haddmc(){

    ROOT::RDataFrame DF_600to800_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_600to800_preEE/skimmed_*.root");
    DF_600to800_preEE.Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_preEE.root");

    ROOT::RDataFrame DF_600to800_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_600to800_postEE/skimmed_*.root");
    DF_600to800_postEE.Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_postEE.root");
    
    return 0;
}
