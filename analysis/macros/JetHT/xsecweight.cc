#include <iostream>
#include <TTree.h>
#include <TString.h>
#include <ROOT/RDataFrame.hxx>

const double weight_preEE[] = {1, 0.221839575, 0.027052134, 0.012157346, 0.000675689, 5.52185E-05, 1.96163E-06, 5.98198E-07, 3.13786E-07, 4.68717E-08, 1.42529E-08, 3.32981E-09, 1.02914E-10};
const double weight_postEE[] = {1, 0.099155235, 0.040371105, 0.010722512, 0.000368968, 3.43437E-05, 7.28824E-06, 2.2931E-06, 1.26887E-06, 2.1858E-07, 5.41467E-08, 1.10481E-08, 5.69236E-10};

int xsecweight(){

    ROOT::RDataFrame DF_50to80_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_50to80_preEE/skimmed_*.root");
    DF_50to80_preEE.Define("xsecweight", [](){ return weight_preEE[0]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_50to80_preEE_xsec.root");
    ROOT::RDataFrame DF_80to120_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_80to120_preEE/skimmed_*.root");
    DF_80to120_preEE.Define("xsecweight", [](){ return weight_preEE[1]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_80to120_preEE_xsec.root");
    ROOT::RDataFrame DF_120to170_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_120to170_preEE/skimmed_*.root");
    DF_120to170_preEE.Define("xsecweight", [](){ return weight_preEE[2]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_120to170_preEE_xsec.root");
    ROOT::RDataFrame DF_170to300_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_170to300_preEE/skimmed_*.root");
    DF_170to300_preEE.Define("xsecweight", [](){ return weight_preEE[3]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_170to300_preEE_xsec.root");
    ROOT::RDataFrame DF_300to470_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_300to470_preEE/skimmed_*.root");
    DF_300to470_preEE.Define("xsecweight", [](){ return weight_preEE[4]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_300to470_preEE_xsec.root");
    ROOT::RDataFrame DF_470to600_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_470to600_preEE/skimmed_*.root");
    DF_470to600_preEE.Define("xsecweight", [](){ return weight_preEE[5]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_470to600_preEE_xsec.root");
    ROOT::RDataFrame DF_600to800_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_600to800_preEE/skimmed_*.root");
    DF_600to800_preEE.Define("xsecweight", [](){ return weight_preEE[6]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_600to800_preEE_xsec.root");
    ROOT::RDataFrame DF_800to1000_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_800to1000_preEE/skimmed_*.root");
    DF_800to1000_preEE.Define("xsecweight", [](){ return weight_preEE[7]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_800to1000_preEE_xsec.root");
    ROOT::RDataFrame DF_1000to1400_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1000to1400_preEE/skimmed_*.root");
    DF_1000to1400_preEE.Define("xsecweight", [](){ return weight_preEE[8]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1000to1400_preEE_xsec.root");
    ROOT::RDataFrame DF_1400to1800_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1400to1800_preEE/skimmed_*.root");
    DF_1400to1800_preEE.Define("xsecweight", [](){ return weight_preEE[9]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1400to1800_preEE_xsec.root");
    ROOT::RDataFrame DF_1800to2400_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1800to2400_preEE/skimmed_*.root");
    DF_1800to2400_preEE.Define("xsecweight", [](){ return weight_preEE[10]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1800to2400_preEE_xsec.root");
    ROOT::RDataFrame DF_2400to3200_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_2400to3200_preEE/skimmed_*.root");
    DF_2400to3200_preEE.Define("xsecweight", [](){ return weight_preEE[11]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_2400to3200_preEE_xsec.root");
    ROOT::RDataFrame DF_3200_preEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_3200_preEE/skimmed_*.root");
    DF_3200_preEE.Define("xsecweight", [](){ return weight_preEE[12]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_3200_preEE_xsec.root");

    ROOT::RDataFrame DF_50to80_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_50to80_postEE/skimmed_*.root");
    DF_50to80_postEE.Define("xsecweight", [](){ return weight_postEE[0]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_50to80_postEE_xsec.root");
    ROOT::RDataFrame DF_80to120_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_80to120_postEE/skimmed_*.root");
    DF_80to120_postEE.Define("xsecweight", [](){ return weight_postEE[1]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_80to120_postEE_xsec.root");
    ROOT::RDataFrame DF_120to170_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_120to170_postEE/skimmed_*.root");
    DF_120to170_postEE.Define("xsecweight", [](){ return weight_postEE[2]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_120to170_postEE_xsec.root");
    ROOT::RDataFrame DF_170to300_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_170to300_postEE/skimmed_*.root");
    DF_170to300_postEE.Define("xsecweight", [](){ return weight_postEE[3]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_170to300_postEE_xsec.root");
    ROOT::RDataFrame DF_300to470_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_300to470_postEE/skimmed_*.root");
    DF_300to470_postEE.Define("xsecweight", [](){ return weight_postEE[4]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_300to470_postEE_xsec.root");
    ROOT::RDataFrame DF_470to600_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_470to600_postEE/skimmed_*.root");
    DF_470to600_postEE.Define("xsecweight", [](){ return weight_postEE[5]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_470to600_postEE_xsec.root");
    ROOT::RDataFrame DF_600to800_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_600to800_postEE/skimmed_*.root");
    DF_600to800_postEE.Define("xsecweight", [](){ return weight_postEE[6]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_600to800_postEE_xsec.root");
    ROOT::RDataFrame DF_800to1000_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_800to1000_postEE/skimmed_*.root");
    DF_800to1000_postEE.Define("xsecweight", [](){ return weight_postEE[7]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_800to1000_postEE_xsec.root");
    ROOT::RDataFrame DF_1000to1400_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1000to1400_postEE/skimmed_*.root");
    DF_1000to1400_postEE.Define("xsecweight", [](){ return weight_postEE[8]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1000to1400_postEE_xsec.root");
    ROOT::RDataFrame DF_1400to1800_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1400to1800_postEE/skimmed_*.root");
    DF_1400to1800_postEE.Define("xsecweight", [](){ return weight_postEE[9]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1400to1800_postEE_xsec.root");
    ROOT::RDataFrame DF_1800to2400_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1800to2400_postEE/skimmed_*.root");
    DF_1800to2400_postEE.Define("xsecweight", [](){ return weight_postEE[10]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_1800to2400_postEE_xsec.root");
    ROOT::RDataFrame DF_2400to3200_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_2400to3200_postEE/skimmed_*.root");
    DF_2400to3200_postEE.Define("xsecweight", [](){ return weight_postEE[11]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_2400to3200_postEE_xsec.root");
    ROOT::RDataFrame DF_3200_postEE("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_3200_postEE/skimmed_*.root");
    DF_3200_postEE.Define("xsecweight", [](){ return weight_postEE[12]; }).Snapshot("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_3200_postEE_xsec.root");
    
    return 0;
}
