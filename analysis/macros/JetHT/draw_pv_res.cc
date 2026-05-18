#include <vector>
#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TGraph.h>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "../../functions/compare_gr.cc"

const TString datatype_text = "High-q^{2} multi-jet events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";
const Int_t nbins = 50;

TGraph* make_graph(Int_t n, const Float_t *x, const Float_t *y) {
    std::vector<Float_t> vx, vy;
    for (Int_t i = 0; i < n; ++i)
        if (y[i] > 0.f) { vx.push_back(x[i]); vy.push_back(y[i]); }
    return new TGraph((Int_t)vx.size(), vx.data(), vy.data());
}

Float_t valid_max(const Float_t *first, const Float_t *last) {
    Float_t m = 0.f;
    for (const Float_t *p = first; p != last; ++p) if (*p > m) m = *p;
    return m;
}

Int_t draw_pv_res(TString period){

    TString figdir = storage_dir + "/figures/" + period + "/pv_res/";

    setTDRStyle();
    TString period_title = period;
    period_title.ReplaceAll("_", " ");
    lumi_sqrtS = "13.6 TeV, " + period_title;

    Float_t reso_data_pullx[nbins];
    Float_t reso_data_pully[nbins];
    Float_t reso_data_pullz[nbins];
    Float_t reso_data_pvx[nbins];
    Float_t reso_data_pvy[nbins];
    Float_t reso_data_pvz[nbins];
    
    Float_t reso_mc_pullx[nbins];
    Float_t reso_mc_pully[nbins];
    Float_t reso_mc_pullz[nbins];
    Float_t reso_mc_pvx[nbins];
    Float_t reso_mc_pvy[nbins];
    Float_t reso_mc_pvz[nbins];
    
    Float_t reso_div_pullx[nbins];
    Float_t reso_div_pully[nbins];
    Float_t reso_div_pullz[nbins];
    Float_t reso_div_pvx[nbins];
    Float_t reso_div_pvy[nbins];
    Float_t reso_div_pvz[nbins];

    Float_t sumpt2_sqrt[nbins];

    for(Int_t i=0; i<nbins; i++){
        nlohmann::json results;

        std::ifstream infile(storage_dir + "/json/" + period + Form("/pv_res/fit_%d.json",i));
        infile >> results;

        sumpt2_sqrt[i] = results["pv_SumTrackPt2_sqrt"];
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
        reso_div_pullx[i] = (reso_mc_pullx[i] > 0.f) ? reso_data_pullx[i] / reso_mc_pullx[i] : 0.f;
        reso_div_pully[i] = (reso_mc_pully[i] > 0.f) ? reso_data_pully[i] / reso_mc_pully[i] : 0.f;
        reso_div_pullz[i] = (reso_mc_pullz[i] > 0.f) ? reso_data_pullz[i] / reso_mc_pullz[i] : 0.f;
        reso_div_pvx[i]   = (reso_mc_pvx[i]   > 0.f) ? reso_data_pvx[i]   / reso_mc_pvx[i]   : 0.f;
        reso_div_pvy[i]   = (reso_mc_pvy[i]   > 0.f) ? reso_data_pvy[i]   / reso_mc_pvy[i]   : 0.f;
        reso_div_pvz[i]   = (reso_mc_pvz[i]   > 0.f) ? reso_data_pvz[i]   / reso_mc_pvz[i]   : 0.f;
    }

    TGraph * gr_data_pvx = make_graph(nbins, sumpt2_sqrt, reso_data_pvx);
    TGraph * gr_data_pvy = make_graph(nbins, sumpt2_sqrt, reso_data_pvy);
    TGraph * gr_data_pvz = make_graph(nbins, sumpt2_sqrt, reso_data_pvz);
    TGraph * gr_data_pullx = make_graph(nbins, sumpt2_sqrt, reso_data_pullx);
    TGraph * gr_data_pully = make_graph(nbins, sumpt2_sqrt, reso_data_pully);
    TGraph * gr_data_pullz = make_graph(nbins, sumpt2_sqrt, reso_data_pullz);

    TGraph * gr_mc_pvx = make_graph(nbins, sumpt2_sqrt, reso_mc_pvx);
    TGraph * gr_mc_pvy = make_graph(nbins, sumpt2_sqrt, reso_mc_pvy);
    TGraph * gr_mc_pvz = make_graph(nbins, sumpt2_sqrt, reso_mc_pvz);
    TGraph * gr_mc_pullx = make_graph(nbins, sumpt2_sqrt, reso_mc_pullx);
    TGraph * gr_mc_pully = make_graph(nbins, sumpt2_sqrt, reso_mc_pully);
    TGraph * gr_mc_pullz = make_graph(nbins, sumpt2_sqrt, reso_mc_pullz);

    TGraph * gr_div_pvx = make_graph(nbins, sumpt2_sqrt, reso_div_pvx);
    TGraph * gr_div_pvy = make_graph(nbins, sumpt2_sqrt, reso_div_pvy);
    TGraph * gr_div_pvz = make_graph(nbins, sumpt2_sqrt, reso_div_pvz);
    TGraph * gr_div_pullx = make_graph(nbins, sumpt2_sqrt, reso_div_pullx);
    TGraph * gr_div_pully = make_graph(nbins, sumpt2_sqrt, reso_div_pully);
    TGraph * gr_div_pullz = make_graph(nbins, sumpt2_sqrt, reso_div_pullz);

    Float_t height_data_pvx = valid_max(reso_data_pvx, reso_data_pvx+nbins);
    Float_t height_data_pvy = valid_max(reso_data_pvy, reso_data_pvy+nbins);
    Float_t height_data_pvz = valid_max(reso_data_pvz, reso_data_pvz+nbins);

    Float_t height_mc_pvx = valid_max(reso_mc_pvx, reso_mc_pvx+nbins);
    Float_t height_mc_pvy = valid_max(reso_mc_pvy, reso_mc_pvy+nbins);
    Float_t height_mc_pvz = valid_max(reso_mc_pvz, reso_mc_pvz+nbins);

    Float_t floor_data_pvx = *std::min_element(reso_data_pvx, reso_data_pvx+nbins);
    Float_t floor_data_pvy = *std::min_element(reso_data_pvy, reso_data_pvy+nbins);
    Float_t floor_data_pvz = *std::min_element(reso_data_pvz, reso_data_pvz+nbins);

    Float_t floor_mc_pvx = *std::min_element(reso_mc_pvx, reso_mc_pvx+nbins);
    Float_t floor_mc_pvy = *std::min_element(reso_mc_pvy, reso_mc_pvy+nbins);
    Float_t floor_mc_pvz = *std::min_element(reso_mc_pvz, reso_mc_pvz+nbins);

    Float_t height_pvx = std::max(height_data_pvx, height_mc_pvx);
    Float_t height_pvy = std::max(height_data_pvy, height_mc_pvy);
    Float_t height_pvz = std::max(height_data_pvz, height_mc_pvz);

    Float_t floor_pvx = std::min(floor_data_pvx, floor_mc_pvx);
    Float_t floor_pvy = std::min(floor_data_pvy, floor_mc_pvy);
    Float_t floor_pvz = std::min(floor_data_pvz, floor_mc_pvz);

    Float_t height_div_pvx = valid_max(reso_div_pvx, reso_div_pvx+nbins);
    Float_t height_div_pvy = valid_max(reso_div_pvy, reso_div_pvy+nbins);
    Float_t height_div_pvz = valid_max(reso_div_pvz, reso_div_pvz+nbins);

    Float_t floor_div_pvx = *std::min_element(reso_div_pvx, reso_div_pvx+nbins);
    Float_t floor_div_pvy = *std::min_element(reso_div_pvy, reso_div_pvy+nbins);
    Float_t floor_div_pvz = *std::min_element(reso_div_pvz, reso_div_pvz+nbins);

    Float_t height_div_pullx = valid_max(reso_div_pullx, reso_div_pullx+nbins);
    Float_t height_div_pully = valid_max(reso_div_pully, reso_div_pully+nbins);
    Float_t height_div_pullz = valid_max(reso_div_pullz, reso_div_pullz+nbins);

    Float_t floor_div_pullx = *std::min_element(reso_div_pullx, reso_div_pullx+nbins);
    Float_t floor_div_pully = *std::min_element(reso_div_pully, reso_div_pully+nbins);
    Float_t floor_div_pullz = *std::min_element(reso_div_pullz, reso_div_pullz+nbins);

    compare_gr(gr_data_pvx, gr_mc_pvx, gr_div_pvx, height_pvx, floor_pvx, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], height_div_pvx, floor_div_pvx, period, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in x [#mum]", figdir + "pvx");
    compare_gr(gr_data_pvy, gr_mc_pvy, gr_div_pvy, height_pvy, floor_pvy, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], height_div_pvy, floor_div_pvy, period, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in y [#mum]", figdir + "pvy");
    compare_gr(gr_data_pvz, gr_mc_pvz, gr_div_pvz, height_pvz, floor_pvz, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], height_div_pvz, floor_div_pvz, period, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in z [#mum]", figdir + "pvz");

    compare_gr(gr_data_pullx, gr_mc_pullx, gr_div_pullx, 1.5, 0.5, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], height_div_pullx, floor_div_pullx, period, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in x", figdir + "pullx");
    compare_gr(gr_data_pully, gr_mc_pully, gr_div_pully, 1.5, 0.5, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], height_div_pully, floor_div_pully, period, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in y", figdir + "pully");
    compare_gr(gr_data_pullz, gr_mc_pullz, gr_div_pullz, 1.5, 0.5, sumpt2_sqrt[0], sumpt2_sqrt[nbins-1], height_div_pullz, floor_div_pullz, period, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in z", figdir + "pullz");
    
    return 0;
}
