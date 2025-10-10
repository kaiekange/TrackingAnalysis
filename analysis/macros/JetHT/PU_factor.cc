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

    ROOT::EnableImplicitMT();

    /* ROOT::RDataFrame dataDF("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_data_"+era+".root"); */
    /* auto dataDF_trig = dataDF.Filter("trig_PFHT1050_pass || trig_PFHT890_pass || trig_PFHT780_pass || trig_PFHT680_pass || trig_PFHT590_pass || trig_PFHT510_pass || trig_PFHT430_pass || trig_PFHT370_pass || trig_PFHT250_pass || trig_PFHT180_pass"); */
    ROOT::RDataFrame dataDF_trig("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_data_"+era+"_corr.root");
    
    ROOT::RDataFrame mcDF("mytree", "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/2022_mc_XsecPSmask/all_skimmed_2022_mc_*_"+era+".root");

    auto h_data_auto = dataDF_trig.Histo1D({"h_data_auto", "", nbins, 0.5, nbins+0.5}, "ev_nPV");
    TH1D * h_data = h_data_auto.GetPtr();
    h_data->Scale(1./h_data->Integral());

    auto h_mc_auto = mcDF.Define("totweight", "xsecweight * PSweight * cell_mask").Histo1D({"h_mc_auto", "", nbins, 0.5, nbins+0.5}, "ev_nPV", "totweight");
    TH1D * h_mc = h_mc_auto.GetPtr();
    h_mc->Scale(1./h_mc->Integral());

    TH1D * h_ratio = (TH1D*) h_data->Clone("h_ratio");
    h_ratio->Divide(h_mc);

    std::vector<double> mcPU_fac;
    for(int i=0; i<nbins; i++){
        mcPU_fac.push_back(h_ratio->GetBinContent(i+1));
    }

    /* dataDF_trig.Snapshot("mytree" ,"/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_data_"+era+"_corr.root"); */

    mcDF.Define("PU_factor", [&mcPU_fac](int nPV) {
            return mcPU_fac[nPV-1];
            }, {"ev_nPV"}).Snapshot("mytree" ,"/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_"+era+"_corrmask.root");

    return 0;
}
