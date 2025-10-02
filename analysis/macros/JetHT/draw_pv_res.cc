#include <vector>
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TGraph.h>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"
#include "../../functions/compare_gr.cc"

const TString datatype_text = "High-q^{2} multi-jet events";
const int nbins = 20;

int draw_pv_res(TString era){

    TString figdir = "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_"+era+"/pv_res/";

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, 2022 " + era;

    float reso_data_pullx[nbins];
    float reso_data_pully[nbins];
    float reso_data_pullz[nbins];
    float reso_data_pvx[nbins];
    float reso_data_pvy[nbins];
    float reso_data_pvz[nbins];
    
    float reso_mc_pullx[nbins];
    float reso_mc_pully[nbins];
    float reso_mc_pullz[nbins];
    float reso_mc_pvx[nbins];
    float reso_mc_pvy[nbins];
    float reso_mc_pvz[nbins];
    
    float reso_div_pullx[nbins];
    float reso_div_pully[nbins];
    float reso_div_pullz[nbins];
    float reso_div_pvx[nbins];
    float reso_div_pvy[nbins];
    float reso_div_pvz[nbins];

    float sumpt2_sqrt[nbins];

    for(int i=0; i<nbins; i++){
        nlohmann::json results;
        std::ifstream infile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_"+era+Form("/pv_res/fit_%d.json",i));
        infile >> results;

        sumpt2_sqrt[i] = results["sumpt2_sqrt"];
        reso_data_pullx[i] = results["reso_data_pullx"];
        reso_data_pully[i] = results["reso_data_pully"];
        reso_data_pullz[i] = results["reso_data_pullz"];
        reso_data_pvx[i] = results["reso_data_pvx"];
        reso_data_pvy[i] = results["reso_data_pvy"];
        reso_data_pvz[i] = results["reso_data_pvz"];
        reso_mc_pullx[i] = results["reso_mc_pullx"];
        reso_mc_pully[i] = results["reso_mc_pully"];
        reso_mc_pullz[i] = results["reso_mc_pullz"];
        reso_mc_pvx[i] = results["reso_mc_pvx"];
        reso_mc_pvy[i] = results["reso_mc_pvy"];
        reso_mc_pvz[i] = results["reso_mc_pvz"];
        reso_div_pullx[i] = reso_data_pullx[i] / reso_mc_pullx[i];
        reso_div_pully[i] = reso_data_pully[i] / reso_mc_pully[i];
        reso_div_pullz[i] = reso_data_pullz[i] / reso_mc_pullz[i];
        reso_div_pvx[i] = reso_data_pvx[i] / reso_mc_pvx[i];
        reso_div_pvy[i] = reso_data_pvy[i] / reso_mc_pvy[i];
        reso_div_pvz[i] = reso_data_pvz[i] / reso_mc_pvz[i];
    }

    TGraph * gr_data_pvx = new TGraph(nbins, sumpt2_sqrt, reso_data_pvx);
    TGraph * gr_data_pvy = new TGraph(nbins, sumpt2_sqrt, reso_data_pvy);
    TGraph * gr_data_pvz = new TGraph(nbins, sumpt2_sqrt, reso_data_pvz);
    TGraph * gr_data_pullx = new TGraph(nbins, sumpt2_sqrt, reso_data_pullx);
    TGraph * gr_data_pully = new TGraph(nbins, sumpt2_sqrt, reso_data_pully);
    TGraph * gr_data_pullz = new TGraph(nbins, sumpt2_sqrt, reso_data_pullz);

    TGraph * gr_mc_pvx = new TGraph(nbins, sumpt2_sqrt, reso_mc_pvx);
    TGraph * gr_mc_pvy = new TGraph(nbins, sumpt2_sqrt, reso_mc_pvy);
    TGraph * gr_mc_pvz = new TGraph(nbins, sumpt2_sqrt, reso_mc_pvz);
    TGraph * gr_mc_pullx = new TGraph(nbins, sumpt2_sqrt, reso_mc_pullx);
    TGraph * gr_mc_pully = new TGraph(nbins, sumpt2_sqrt, reso_mc_pully);
    TGraph * gr_mc_pullz = new TGraph(nbins, sumpt2_sqrt, reso_mc_pullz);

    TGraph * gr_div_pvx = new TGraph(nbins, sumpt2_sqrt, reso_div_pvx);
    TGraph * gr_div_pvy = new TGraph(nbins, sumpt2_sqrt, reso_div_pvy);
    TGraph * gr_div_pvz = new TGraph(nbins, sumpt2_sqrt, reso_div_pvz);
    TGraph * gr_div_pullx = new TGraph(nbins, sumpt2_sqrt, reso_div_pullx);
    TGraph * gr_div_pully = new TGraph(nbins, sumpt2_sqrt, reso_div_pully);
    TGraph * gr_div_pullz = new TGraph(nbins, sumpt2_sqrt, reso_div_pullz);

    float height_data_pvx = *std::max_element(reso_data_pvx, reso_data_pvx+nbins);
    float height_data_pvy = *std::max_element(reso_data_pvy, reso_data_pvy+nbins);
    float height_data_pvz = *std::max_element(reso_data_pvz, reso_data_pvz+nbins);
    float floor_data_pvx = *std::min_element(reso_data_pvx, reso_data_pvx+nbins);
    float floor_data_pvy = *std::min_element(reso_data_pvy, reso_data_pvy+nbins);
    float floor_data_pvz = *std::min_element(reso_data_pvz, reso_data_pvz+nbins);

    float height_mc_pvx = *std::max_element(reso_mc_pvx, reso_mc_pvx+nbins);
    float height_mc_pvy = *std::max_element(reso_mc_pvy, reso_mc_pvy+nbins);
    float height_mc_pvz = *std::max_element(reso_mc_pvz, reso_mc_pvz+nbins);
    float floor_mc_pvx = *std::min_element(reso_mc_pvx, reso_mc_pvx+nbins);
    float floor_mc_pvy = *std::min_element(reso_mc_pvy, reso_mc_pvy+nbins);
    float floor_mc_pvz = *std::min_element(reso_mc_pvz, reso_mc_pvz+nbins);

    float height_pvx = std::max(height_data_pvx, height_mc_pvx);
    float height_pvy = std::max(height_data_pvy, height_mc_pvy);
    float height_pvz = std::max(height_data_pvz, height_mc_pvz);
    float floor_pvx = std::min(floor_data_pvx, floor_mc_pvx);
    float floor_pvy = std::min(floor_data_pvy, floor_mc_pvy);
    float floor_pvz = std::min(floor_data_pvz, floor_mc_pvz);

    compare_gr(gr_data_pvx, gr_mc_pvx, gr_div_pvx, height_pvx, floor_pvx, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.9, 1.5, "Data", "Simulation", era, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in x [#mum]", figdir + "pvx");
    compare_gr(gr_data_pvy, gr_mc_pvy, gr_div_pvy, height_pvy, floor_pvy, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.9, 1.5, "Data", "Simulation", era, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in y [#mum]", figdir + "pvy");
    compare_gr(gr_data_pvz, gr_mc_pvz, gr_div_pvz, height_pvz, floor_pvz, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.9, 1.5, "Data", "Simulation", era, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in z [#mum]", figdir + "pvz");

    compare_gr(gr_data_pullx, gr_mc_pullx, gr_div_pullx, 1.5, 0.5, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.8, 1.2, "Data", "Simulation", era, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in x", figdir + "pullx");
    compare_gr(gr_data_pully, gr_mc_pully, gr_div_pully, 1.5, 0.5, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.8, 1.2, "Data", "Simulation", era, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in y", figdir + "pully");
    compare_gr(gr_data_pullz, gr_mc_pullz, gr_div_pullz, 1.5, 0.5, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.8, 1.2, "Data", "Simulation", era, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in z", figdir + "pullz");
    
    return 0;
}
