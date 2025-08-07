#include <vector>
#include <iostream>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooAddition.h>
#include <RooFitResult.h>
#include <RooCBShape.h>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"
#include "input_list.cc"
#include "../../functions/compare_gr.cc"

const TString figdir = "../../figures/"+datatype+"/pv_res/compare/";
const TString jsondir = "../../json/"+datatype+"/pv_res/compare/";
const int nbins = 20;

int draw_pv_res(){

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, 2022";

    float data_reso_pullx[nbins];
    float data_reso_pully[nbins];
    float data_reso_pullz[nbins];
    float data_reso_pvx[nbins];
    float data_reso_pvy[nbins];
    float data_reso_pvz[nbins];
    
    float mc_reso_pullx[nbins];
    float mc_reso_pully[nbins];
    float mc_reso_pullz[nbins];
    float mc_reso_pvx[nbins];
    float mc_reso_pvy[nbins];
    float mc_reso_pvz[nbins];
    
    float div_reso_pullx[nbins];
    float div_reso_pully[nbins];
    float div_reso_pullz[nbins];
    float div_reso_pvx[nbins];
    float div_reso_pvy[nbins];
    float div_reso_pvz[nbins];

    float sumpt2_sqrt[nbins];

    for(int i=0; i<nbins; i++){
        nlohmann::json results;
        std::ifstream infile(jsondir + Form("/fit_%d.json",i));
        infile >> results;

        sumpt2_sqrt[i] = results["sumpt2_sqrt"];
        data_reso_pullx[i] = results["reso_pullx_data"];
        data_reso_pully[i] = results["reso_pully_data"];
        data_reso_pullz[i] = results["reso_pullz_data"];
        data_reso_pvx[i] = results["reso_pvx_data"];
        data_reso_pvy[i] = results["reso_pvy_data"];
        data_reso_pvz[i] = results["reso_pvz_data"];
        mc_reso_pullx[i] = results["reso_pullx_mc"];
        mc_reso_pully[i] = results["reso_pully_mc"];
        mc_reso_pullz[i] = results["reso_pullz_mc"];
        mc_reso_pvx[i] = results["reso_pvx_mc"];
        mc_reso_pvy[i] = results["reso_pvy_mc"];
        mc_reso_pvz[i] = results["reso_pvz_mc"];
        div_reso_pullx[i] = data_reso_pullx[i] / mc_reso_pullx[i];
        div_reso_pully[i] = data_reso_pully[i] / mc_reso_pully[i];
        div_reso_pullz[i] = data_reso_pullz[i] / mc_reso_pullz[i];
        div_reso_pvx[i] = data_reso_pvx[i] / mc_reso_pvx[i];
        div_reso_pvy[i] = data_reso_pvy[i] / mc_reso_pvy[i];
        div_reso_pvz[i] = data_reso_pvz[i] / mc_reso_pvz[i];
    }

    TGraph * data_gr_pvx = new TGraph(nbins, sumpt2_sqrt, data_reso_pvx);
    TGraph * data_gr_pvy = new TGraph(nbins, sumpt2_sqrt, data_reso_pvy);
    TGraph * data_gr_pvz = new TGraph(nbins, sumpt2_sqrt, data_reso_pvz);
    TGraph * data_gr_pullx = new TGraph(nbins, sumpt2_sqrt, data_reso_pullx);
    TGraph * data_gr_pully = new TGraph(nbins, sumpt2_sqrt, data_reso_pully);
    TGraph * data_gr_pullz = new TGraph(nbins, sumpt2_sqrt, data_reso_pullz);

    TGraph * mc_gr_pvx = new TGraph(nbins, sumpt2_sqrt, mc_reso_pvx);
    TGraph * mc_gr_pvy = new TGraph(nbins, sumpt2_sqrt, mc_reso_pvy);
    TGraph * mc_gr_pvz = new TGraph(nbins, sumpt2_sqrt, mc_reso_pvz);
    TGraph * mc_gr_pullx = new TGraph(nbins, sumpt2_sqrt, mc_reso_pullx);
    TGraph * mc_gr_pully = new TGraph(nbins, sumpt2_sqrt, mc_reso_pully);
    TGraph * mc_gr_pullz = new TGraph(nbins, sumpt2_sqrt, mc_reso_pullz);

    TGraph * div_gr_pvx = new TGraph(nbins, sumpt2_sqrt, div_reso_pvx);
    TGraph * div_gr_pvy = new TGraph(nbins, sumpt2_sqrt, div_reso_pvy);
    TGraph * div_gr_pvz = new TGraph(nbins, sumpt2_sqrt, div_reso_pvz);
    TGraph * div_gr_pullx = new TGraph(nbins, sumpt2_sqrt, div_reso_pullx);
    TGraph * div_gr_pully = new TGraph(nbins, sumpt2_sqrt, div_reso_pully);
    TGraph * div_gr_pullz = new TGraph(nbins, sumpt2_sqrt, div_reso_pullz);

    float data_height_pvx = *std::max_element(data_reso_pvx, data_reso_pvx+nbins);
    float data_height_pvy = *std::max_element(data_reso_pvy, data_reso_pvy+nbins);
    float data_height_pvz = *std::max_element(data_reso_pvz, data_reso_pvz+nbins);
    float data_floor_pvx = *std::min_element(data_reso_pvx, data_reso_pvx+nbins);
    float data_floor_pvy = *std::min_element(data_reso_pvy, data_reso_pvy+nbins);
    float data_floor_pvz = *std::min_element(data_reso_pvz, data_reso_pvz+nbins);

    float mc_height_pvx = *std::max_element(mc_reso_pvx, mc_reso_pvx+nbins);
    float mc_height_pvy = *std::max_element(mc_reso_pvy, mc_reso_pvy+nbins);
    float mc_height_pvz = *std::max_element(mc_reso_pvz, mc_reso_pvz+nbins);
    float mc_floor_pvx = *std::min_element(mc_reso_pvx, mc_reso_pvx+nbins);
    float mc_floor_pvy = *std::min_element(mc_reso_pvy, mc_reso_pvy+nbins);
    float mc_floor_pvz = *std::min_element(mc_reso_pvz, mc_reso_pvz+nbins);

    float height_pvx = std::max(data_height_pvx, mc_height_pvx);
    float height_pvy = std::max(data_height_pvy, mc_height_pvy);
    float height_pvz = std::max(data_height_pvz, mc_height_pvz);
    float floor_pvx = std::min(data_floor_pvx, mc_floor_pvx);
    float floor_pvy = std::min(data_floor_pvy, mc_floor_pvy);
    float floor_pvz = std::min(data_floor_pvz, mc_floor_pvz);

    compare_gr(data_gr_pvx, mc_gr_pvx, div_gr_pvx, height_pvx, floor_pvx, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.8, 1.2, "Data", "Simulation", datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in x [#mum]", figdir + "pvx");
    compare_gr(data_gr_pvy, mc_gr_pvy, div_gr_pvy, height_pvy, floor_pvy, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.8, 1.2, "Data", "Simulation", datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in y [#mum]", figdir + "pvy");
    compare_gr(data_gr_pvz, mc_gr_pvz, div_gr_pvz, height_pvz, floor_pvz, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.8, 1.2, "Data", "Simulation", datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in z [#mum]", figdir + "pvz");

    compare_gr(data_gr_pullx, mc_gr_pullx, div_gr_pullx, 1.5, 0.5, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.8, 1.2, "Data", "Simulation", datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in x", figdir + "pullx");
    compare_gr(data_gr_pully, mc_gr_pully, div_gr_pully, 1.5, 0.5, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.8, 1.2, "Data", "Simulation", datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in y", figdir + "pully");
    compare_gr(data_gr_pullz, mc_gr_pullz, div_gr_pullz, 1.5, 0.5, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], 0.8, 1.2, "Data", "Simulation", datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in z", figdir + "pullz");
    
    return 0;
}
