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

const TString datatype_text = "Unbiased collision events";
const int nbins = 500;

int draw_ip_res(TString era){

    TString figdir = "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias_"+era+"/ip_res/";

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, 2022 " + era;

    float reso_data_d0_pt_loeta[nbins];
    float reso_data_dz_pt_loeta[nbins];
    float reso_data_d0_pt_hieta[nbins];
    float reso_data_dz_pt_hieta[nbins];

    float reso_data_d0_eta_lopt[nbins];
    float reso_data_dz_eta_lopt[nbins];
    float reso_data_d0_eta_hipt[nbins];
    float reso_data_dz_eta_hipt[nbins];

    float reso_data_d0_phi_lopt[nbins];
    float reso_data_dz_phi_lopt[nbins];
    float reso_data_d0_phi_hipt[nbins];
    float reso_data_dz_phi_hipt[nbins];

    float reso_mc_d0_pt_loeta[nbins];
    float reso_mc_dz_pt_loeta[nbins];
    float reso_mc_d0_pt_hieta[nbins];
    float reso_mc_dz_pt_hieta[nbins];

    float reso_mc_d0_eta_lopt[nbins];
    float reso_mc_dz_eta_lopt[nbins];
    float reso_mc_d0_eta_hipt[nbins];
    float reso_mc_dz_eta_hipt[nbins];

    float reso_mc_d0_phi_lopt[nbins];
    float reso_mc_dz_phi_lopt[nbins];
    float reso_mc_d0_phi_hipt[nbins];
    float reso_mc_dz_phi_hipt[nbins];

    float reso_div_d0_pt_loeta[nbins];
    float reso_div_dz_pt_loeta[nbins];
    float reso_div_d0_pt_hieta[nbins];
    float reso_div_dz_pt_hieta[nbins];

    float reso_div_d0_eta_lopt[nbins];
    float reso_div_dz_eta_lopt[nbins];
    float reso_div_d0_eta_hipt[nbins];
    float reso_div_dz_eta_hipt[nbins];

    float reso_div_d0_phi_lopt[nbins];
    float reso_div_dz_phi_lopt[nbins];
    float reso_div_d0_phi_hipt[nbins];
    float reso_div_dz_phi_hipt[nbins];

    float eta[nbins];
    float phi[nbins];
    float pt[nbins];

    for(int i=0; i<nbins; i++){
        nlohmann::json results;
        std::ifstream infile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/ZeroBias_"+era+Form("/ip_res/fit_%d.json",i));
        infile >> results;

        eta[i] = results["eta"];
        phi[i] = results["phi"];
        pt[i] = results["pt"];

        reso_data_d0_pt_loeta[i] = results["reso_data_d0_pt_loeta"];
        reso_data_dz_pt_loeta[i] = results["reso_data_dz_pt_loeta"];
        reso_data_d0_pt_hieta[i] = results["reso_data_d0_pt_hieta"];
        reso_data_dz_pt_hieta[i] = results["reso_data_dz_pt_hieta"];

        reso_data_d0_eta_lopt[i] = results["reso_data_d0_eta_lopt"];
        reso_data_dz_eta_lopt[i] = results["reso_data_dz_eta_lopt"];
        reso_data_d0_eta_hipt[i] = results["reso_data_d0_eta_hipt"];
        reso_data_dz_eta_hipt[i] = results["reso_data_dz_eta_hipt"];

        reso_data_d0_phi_lopt[i] = results["reso_data_d0_phi_lopt"];
        reso_data_dz_phi_lopt[i] = results["reso_data_dz_phi_lopt"];
        reso_data_d0_phi_hipt[i] = results["reso_data_d0_phi_hipt"];
        reso_data_dz_phi_hipt[i] = results["reso_data_dz_phi_hipt"];

        reso_mc_d0_pt_loeta[i] = results["reso_mc_d0_pt_loeta"];
        reso_mc_dz_pt_loeta[i] = results["reso_mc_dz_pt_loeta"];
        reso_mc_d0_pt_hieta[i] = results["reso_mc_d0_pt_hieta"];
        reso_mc_dz_pt_hieta[i] = results["reso_mc_dz_pt_hieta"];

        reso_mc_d0_eta_lopt[i] = results["reso_mc_d0_eta_lopt"];
        reso_mc_dz_eta_lopt[i] = results["reso_mc_dz_eta_lopt"];
        reso_mc_d0_eta_hipt[i] = results["reso_mc_d0_eta_hipt"];
        reso_mc_dz_eta_hipt[i] = results["reso_mc_dz_eta_hipt"];

        reso_mc_d0_phi_lopt[i] = results["reso_mc_d0_phi_lopt"];
        reso_mc_dz_phi_lopt[i] = results["reso_mc_dz_phi_lopt"];
        reso_mc_d0_phi_hipt[i] = results["reso_mc_d0_phi_hipt"];
        reso_mc_dz_phi_hipt[i] = results["reso_mc_dz_phi_hipt"];

        reso_div_d0_pt_loeta[i] = reso_data_d0_pt_loeta[i] / reso_mc_d0_pt_loeta[i];
        reso_div_dz_pt_loeta[i] = reso_data_dz_pt_loeta[i] / reso_mc_dz_pt_loeta[i];
        reso_div_d0_pt_hieta[i] = reso_data_d0_pt_hieta[i] / reso_mc_d0_pt_hieta[i];
        reso_div_dz_pt_hieta[i] = reso_data_dz_pt_hieta[i] / reso_mc_dz_pt_hieta[i];

        reso_div_d0_eta_lopt[i] = reso_data_d0_eta_lopt[i] / reso_mc_d0_eta_lopt[i];
        reso_div_dz_eta_lopt[i] = reso_data_dz_eta_lopt[i] / reso_mc_dz_eta_lopt[i];
        reso_div_d0_eta_hipt[i] = reso_data_d0_eta_hipt[i] / reso_mc_d0_eta_hipt[i];
        reso_div_dz_eta_hipt[i] = reso_data_dz_eta_hipt[i] / reso_mc_dz_eta_hipt[i];

        reso_div_d0_phi_lopt[i] = reso_data_d0_phi_lopt[i] / reso_mc_d0_phi_lopt[i];
        reso_div_dz_phi_lopt[i] = reso_data_dz_phi_lopt[i] / reso_mc_dz_phi_lopt[i];
        reso_div_d0_phi_hipt[i] = reso_data_d0_phi_hipt[i] / reso_mc_d0_phi_hipt[i];
        reso_div_dz_phi_hipt[i] = reso_data_dz_phi_hipt[i] / reso_mc_dz_phi_hipt[i];

    }

    TGraph * gr_data_d0_pt_loeta = new TGraph(nbins, pt, reso_data_d0_pt_loeta);
    TGraph * gr_data_dz_pt_loeta = new TGraph(nbins, pt, reso_data_dz_pt_loeta);
    TGraph * gr_data_d0_pt_hieta = new TGraph(nbins, pt, reso_data_d0_pt_hieta);
    TGraph * gr_data_dz_pt_hieta = new TGraph(nbins, pt, reso_data_dz_pt_hieta);
    
    TGraph * gr_data_d0_eta_lopt = new TGraph(nbins, eta, reso_data_d0_eta_lopt);
    TGraph * gr_data_dz_eta_lopt = new TGraph(nbins, eta, reso_data_dz_eta_lopt);
    TGraph * gr_data_d0_eta_hipt = new TGraph(nbins, eta, reso_data_d0_eta_hipt);
    TGraph * gr_data_dz_eta_hipt = new TGraph(nbins, eta, reso_data_dz_eta_hipt);

    TGraph * gr_data_d0_phi_lopt = new TGraph(nbins, phi, reso_data_d0_phi_lopt);
    TGraph * gr_data_dz_phi_lopt = new TGraph(nbins, phi, reso_data_dz_phi_lopt);
    TGraph * gr_data_d0_phi_hipt = new TGraph(nbins, phi, reso_data_d0_phi_hipt);
    TGraph * gr_data_dz_phi_hipt = new TGraph(nbins, phi, reso_data_dz_phi_hipt);
    
    TGraph * gr_mc_d0_pt_loeta = new TGraph(nbins, pt, reso_mc_d0_pt_loeta);
    TGraph * gr_mc_dz_pt_loeta = new TGraph(nbins, pt, reso_mc_dz_pt_loeta);
    TGraph * gr_mc_d0_pt_hieta = new TGraph(nbins, pt, reso_mc_d0_pt_hieta);
    TGraph * gr_mc_dz_pt_hieta = new TGraph(nbins, pt, reso_mc_dz_pt_hieta);
    
    TGraph * gr_mc_d0_eta_lopt = new TGraph(nbins, eta, reso_mc_d0_eta_lopt);
    TGraph * gr_mc_dz_eta_lopt = new TGraph(nbins, eta, reso_mc_dz_eta_lopt);
    TGraph * gr_mc_d0_eta_hipt = new TGraph(nbins, eta, reso_mc_d0_eta_hipt);
    TGraph * gr_mc_dz_eta_hipt = new TGraph(nbins, eta, reso_mc_dz_eta_hipt);

    TGraph * gr_mc_d0_phi_lopt = new TGraph(nbins, phi, reso_mc_d0_phi_lopt);
    TGraph * gr_mc_dz_phi_lopt = new TGraph(nbins, phi, reso_mc_dz_phi_lopt);
    TGraph * gr_mc_d0_phi_hipt = new TGraph(nbins, phi, reso_mc_d0_phi_hipt);
    TGraph * gr_mc_dz_phi_hipt = new TGraph(nbins, phi, reso_mc_dz_phi_hipt);
    
    TGraph * gr_div_d0_pt_loeta = new TGraph(nbins, pt, reso_div_d0_pt_loeta);
    TGraph * gr_div_dz_pt_loeta = new TGraph(nbins, pt, reso_div_dz_pt_loeta);
    TGraph * gr_div_d0_pt_hieta = new TGraph(nbins, pt, reso_div_d0_pt_hieta);
    TGraph * gr_div_dz_pt_hieta = new TGraph(nbins, pt, reso_div_dz_pt_hieta);
    
    TGraph * gr_div_d0_eta_lopt = new TGraph(nbins, eta, reso_div_d0_eta_lopt);
    TGraph * gr_div_dz_eta_lopt = new TGraph(nbins, eta, reso_div_dz_eta_lopt);
    TGraph * gr_div_d0_eta_hipt = new TGraph(nbins, eta, reso_div_d0_eta_hipt);
    TGraph * gr_div_dz_eta_hipt = new TGraph(nbins, eta, reso_div_dz_eta_hipt);

    TGraph * gr_div_d0_phi_lopt = new TGraph(nbins, phi, reso_div_d0_phi_lopt);
    TGraph * gr_div_dz_phi_lopt = new TGraph(nbins, phi, reso_div_dz_phi_lopt);
    TGraph * gr_div_d0_phi_hipt = new TGraph(nbins, phi, reso_div_d0_phi_hipt);
    TGraph * gr_div_dz_phi_hipt = new TGraph(nbins, phi, reso_div_dz_phi_hipt);

    float height_data_d0_pt_loeta = *std::max_element(reso_data_d0_pt_loeta, reso_data_d0_pt_loeta+nbins);
    float height_data_dz_pt_loeta = *std::max_element(reso_data_dz_pt_loeta, reso_data_dz_pt_loeta+nbins);
    float height_data_d0_pt_hieta = *std::max_element(reso_data_d0_pt_hieta, reso_data_d0_pt_hieta+nbins);
    float height_data_dz_pt_hieta = *std::max_element(reso_data_dz_pt_hieta, reso_data_dz_pt_hieta+nbins);
    float height_data_d0_eta_lopt = *std::max_element(reso_data_d0_eta_lopt, reso_data_d0_eta_lopt+nbins);
    float height_data_dz_eta_lopt = *std::max_element(reso_data_dz_eta_lopt, reso_data_dz_eta_lopt+nbins);
    float height_data_d0_eta_hipt = *std::max_element(reso_data_d0_eta_hipt, reso_data_d0_eta_hipt+nbins);
    float height_data_dz_eta_hipt = *std::max_element(reso_data_dz_eta_hipt, reso_data_dz_eta_hipt+nbins);
    float height_data_d0_phi_lopt = *std::max_element(reso_data_d0_phi_lopt, reso_data_d0_phi_lopt+nbins);
    float height_data_dz_phi_lopt = *std::max_element(reso_data_dz_phi_lopt, reso_data_dz_phi_lopt+nbins);
    float height_data_d0_phi_hipt = *std::max_element(reso_data_d0_phi_hipt, reso_data_d0_phi_hipt+nbins);
    float height_data_dz_phi_hipt = *std::max_element(reso_data_dz_phi_hipt, reso_data_dz_phi_hipt+nbins);

    float height_mc_d0_pt_loeta = *std::max_element(reso_mc_d0_pt_loeta, reso_mc_d0_pt_loeta+nbins);
    float height_mc_dz_pt_loeta = *std::max_element(reso_mc_dz_pt_loeta, reso_mc_dz_pt_loeta+nbins);
    float height_mc_d0_pt_hieta = *std::max_element(reso_mc_d0_pt_hieta, reso_mc_d0_pt_hieta+nbins);
    float height_mc_dz_pt_hieta = *std::max_element(reso_mc_dz_pt_hieta, reso_mc_dz_pt_hieta+nbins);
    float height_mc_d0_eta_lopt = *std::max_element(reso_mc_d0_eta_lopt, reso_mc_d0_eta_lopt+nbins);
    float height_mc_dz_eta_lopt = *std::max_element(reso_mc_dz_eta_lopt, reso_mc_dz_eta_lopt+nbins);
    float height_mc_d0_eta_hipt = *std::max_element(reso_mc_d0_eta_hipt, reso_mc_d0_eta_hipt+nbins);
    float height_mc_dz_eta_hipt = *std::max_element(reso_mc_dz_eta_hipt, reso_mc_dz_eta_hipt+nbins);
    float height_mc_d0_phi_lopt = *std::max_element(reso_mc_d0_phi_lopt, reso_mc_d0_phi_lopt+nbins);
    float height_mc_dz_phi_lopt = *std::max_element(reso_mc_dz_phi_lopt, reso_mc_dz_phi_lopt+nbins);
    float height_mc_d0_phi_hipt = *std::max_element(reso_mc_d0_phi_hipt, reso_mc_d0_phi_hipt+nbins);
    float height_mc_dz_phi_hipt = *std::max_element(reso_mc_dz_phi_hipt, reso_mc_dz_phi_hipt+nbins);
    
    float height_d0_pt_loeta = std::max(height_data_d0_pt_loeta, height_mc_d0_pt_loeta);
    float height_dz_pt_loeta = std::max(height_data_dz_pt_loeta, height_mc_dz_pt_loeta);
    float height_d0_pt_hieta = std::max(height_data_d0_pt_hieta, height_mc_d0_pt_hieta);
    float height_dz_pt_hieta = std::max(height_data_dz_pt_hieta, height_mc_dz_pt_hieta);
    float height_d0_eta_lopt = std::max(height_data_d0_eta_lopt, height_mc_d0_eta_lopt);
    float height_dz_eta_lopt = std::max(height_data_dz_eta_lopt, height_mc_dz_eta_lopt);
    float height_d0_eta_hipt = std::max(height_data_d0_eta_hipt, height_mc_d0_eta_hipt);
    float height_dz_eta_hipt = std::max(height_data_dz_eta_hipt, height_mc_dz_eta_hipt);
    float height_d0_phi_lopt = std::max(height_data_d0_phi_lopt, height_mc_d0_phi_lopt);
    float height_dz_phi_lopt = std::max(height_data_dz_phi_lopt, height_mc_dz_phi_lopt);
    float height_d0_phi_hipt = std::max(height_data_d0_phi_hipt, height_mc_d0_phi_hipt);
    float height_dz_phi_hipt = std::max(height_data_dz_phi_hipt, height_mc_dz_phi_hipt);


    float floor_data_d0_pt_loeta = *std::min_element(reso_data_d0_pt_loeta, reso_data_d0_pt_loeta+nbins);
    float floor_data_dz_pt_loeta = *std::min_element(reso_data_dz_pt_loeta, reso_data_dz_pt_loeta+nbins);
    float floor_data_d0_pt_hieta = *std::min_element(reso_data_d0_pt_hieta, reso_data_d0_pt_hieta+nbins);
    float floor_data_dz_pt_hieta = *std::min_element(reso_data_dz_pt_hieta, reso_data_dz_pt_hieta+nbins);
    float floor_data_d0_eta_lopt = *std::min_element(reso_data_d0_eta_lopt, reso_data_d0_eta_lopt+nbins);
    float floor_data_dz_eta_lopt = *std::min_element(reso_data_dz_eta_lopt, reso_data_dz_eta_lopt+nbins);
    float floor_data_d0_eta_hipt = *std::min_element(reso_data_d0_eta_hipt, reso_data_d0_eta_hipt+nbins);
    float floor_data_dz_eta_hipt = *std::min_element(reso_data_dz_eta_hipt, reso_data_dz_eta_hipt+nbins);
    float floor_data_d0_phi_lopt = *std::min_element(reso_data_d0_phi_lopt, reso_data_d0_phi_lopt+nbins);
    float floor_data_dz_phi_lopt = *std::min_element(reso_data_dz_phi_lopt, reso_data_dz_phi_lopt+nbins);
    float floor_data_d0_phi_hipt = *std::min_element(reso_data_d0_phi_hipt, reso_data_d0_phi_hipt+nbins);
    float floor_data_dz_phi_hipt = *std::min_element(reso_data_dz_phi_hipt, reso_data_dz_phi_hipt+nbins);

    float floor_mc_d0_pt_loeta = *std::min_element(reso_mc_d0_pt_loeta, reso_mc_d0_pt_loeta+nbins);
    float floor_mc_dz_pt_loeta = *std::min_element(reso_mc_dz_pt_loeta, reso_mc_dz_pt_loeta+nbins);
    float floor_mc_d0_pt_hieta = *std::min_element(reso_mc_d0_pt_hieta, reso_mc_d0_pt_hieta+nbins);
    float floor_mc_dz_pt_hieta = *std::min_element(reso_mc_dz_pt_hieta, reso_mc_dz_pt_hieta+nbins);
    float floor_mc_d0_eta_lopt = *std::min_element(reso_mc_d0_eta_lopt, reso_mc_d0_eta_lopt+nbins);
    float floor_mc_dz_eta_lopt = *std::min_element(reso_mc_dz_eta_lopt, reso_mc_dz_eta_lopt+nbins);
    float floor_mc_d0_eta_hipt = *std::min_element(reso_mc_d0_eta_hipt, reso_mc_d0_eta_hipt+nbins);
    float floor_mc_dz_eta_hipt = *std::min_element(reso_mc_dz_eta_hipt, reso_mc_dz_eta_hipt+nbins);
    float floor_mc_d0_phi_lopt = *std::min_element(reso_mc_d0_phi_lopt, reso_mc_d0_phi_lopt+nbins);
    float floor_mc_dz_phi_lopt = *std::min_element(reso_mc_dz_phi_lopt, reso_mc_dz_phi_lopt+nbins);
    float floor_mc_d0_phi_hipt = *std::min_element(reso_mc_d0_phi_hipt, reso_mc_d0_phi_hipt+nbins);
    float floor_mc_dz_phi_hipt = *std::min_element(reso_mc_dz_phi_hipt, reso_mc_dz_phi_hipt+nbins);
    
    float floor_d0_pt_loeta = std::min(floor_data_d0_pt_loeta, floor_mc_d0_pt_loeta);
    float floor_dz_pt_loeta = std::min(floor_data_dz_pt_loeta, floor_mc_dz_pt_loeta);
    float floor_d0_pt_hieta = std::min(floor_data_d0_pt_hieta, floor_mc_d0_pt_hieta);
    float floor_dz_pt_hieta = std::min(floor_data_dz_pt_hieta, floor_mc_dz_pt_hieta);
    float floor_d0_eta_lopt = std::min(floor_data_d0_eta_lopt, floor_mc_d0_eta_lopt);
    float floor_dz_eta_lopt = std::min(floor_data_dz_eta_lopt, floor_mc_dz_eta_lopt);
    float floor_d0_eta_hipt = std::min(floor_data_d0_eta_hipt, floor_mc_d0_eta_hipt);
    float floor_dz_eta_hipt = std::min(floor_data_dz_eta_hipt, floor_mc_dz_eta_hipt);
    float floor_d0_phi_lopt = std::min(floor_data_d0_phi_lopt, floor_mc_d0_phi_lopt);
    float floor_dz_phi_lopt = std::min(floor_data_dz_phi_lopt, floor_mc_dz_phi_lopt);
    float floor_d0_phi_hipt = std::min(floor_data_d0_phi_hipt, floor_mc_d0_phi_hipt);
    float floor_dz_phi_hipt = std::min(floor_data_dz_phi_hipt, floor_mc_dz_phi_hipt);

    compare_gr(gr_data_d0_pt_hieta, gr_mc_d0_pt_hieta, gr_div_d0_pt_hieta, height_d0_pt_hieta, floor_d0_pt_hieta, pt[0], pt[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_hieta"); 
    compare_gr(gr_data_d0_pt_loeta, gr_mc_d0_pt_loeta, gr_div_d0_pt_loeta, height_d0_pt_loeta, floor_d0_pt_loeta, pt[0], pt[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{|#it{#eta}|<1.4}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_loeta"); 

    compare_gr(gr_data_d0_eta_lopt, gr_mc_d0_eta_lopt, gr_div_d0_eta_lopt, height_d0_eta_lopt, floor_d0_eta_lopt, eta[0], eta[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_lopt"); 
    compare_gr(gr_data_d0_eta_hipt, gr_mc_d0_eta_hipt, gr_div_d0_eta_hipt, height_d0_eta_hipt, floor_d0_eta_hipt, eta[0], eta[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_hipt"); 

    compare_gr(gr_data_d0_phi_lopt, gr_mc_d0_phi_lopt, gr_div_d0_phi_lopt, height_d0_phi_lopt, floor_d0_phi_lopt, phi[0], phi[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_lopt"); 
    compare_gr(gr_data_d0_phi_hipt, gr_mc_d0_phi_hipt, gr_div_d0_phi_hipt, height_d0_phi_hipt, floor_d0_phi_hipt, phi[0], phi[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_hipt"); 

    compare_gr(gr_data_dz_pt_hieta, gr_mc_dz_pt_hieta, gr_div_dz_pt_hieta, height_dz_pt_hieta, floor_dz_pt_hieta, pt[0], pt[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_hieta"); 
    compare_gr(gr_data_dz_pt_loeta, gr_mc_dz_pt_loeta, gr_div_dz_pt_loeta, height_dz_pt_loeta, floor_dz_pt_loeta, pt[0], pt[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{|#it{#eta}|<1.4}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_loeta"); 

    compare_gr(gr_data_dz_eta_lopt, gr_mc_dz_eta_lopt, gr_div_dz_eta_lopt, height_dz_eta_lopt, floor_dz_eta_lopt, eta[0], eta[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_lopt"); 
    compare_gr(gr_data_dz_eta_hipt, gr_mc_dz_eta_hipt, gr_div_dz_eta_hipt, height_dz_eta_hipt, floor_dz_eta_hipt, eta[0], eta[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_hipt"); 

    compare_gr(gr_data_dz_phi_lopt, gr_mc_dz_phi_lopt, gr_div_dz_phi_lopt, height_dz_phi_lopt, floor_dz_phi_lopt, phi[0], phi[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_lopt"); 
    compare_gr(gr_data_dz_phi_hipt, gr_mc_dz_phi_hipt, gr_div_dz_phi_hipt, height_dz_phi_hipt, floor_dz_phi_hipt, phi[0], phi[nbins-1], 0.8, 1.2, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_hipt"); 

/*     TCanvas *c_d0_pt = new TCanvas("c_d0_pt", "", 800, 600); */
/*     canvas_setup(c_d0_pt); */
/*     gr_d0_pt_hieta->SetMarkerStyle(20); */
/*     gr_d0_pt_hieta->SetMarkerColor(kBlack); */
/*     gr_d0_pt_loeta->SetMarkerStyle(20); */
/*     gr_d0_pt_loeta->SetMarkerColor(kBlue); */
/*     gr_d0_pt_hieta->Draw("AP"); */
/*     gr_d0_pt_loeta->Draw("P SAME"); */
/*     gr_d0_pt_hieta->GetXaxis()->SetTitle("Track #it{p_{T}} [GeV]"); */
/*     gr_d0_pt_hieta->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]"); */
/*     TLegend *leg_d0_pt = new TLegend(0.55, 0.7, 0.75, 0.85); */
/*     leg_d0_pt->AddEntry(gr_d0_pt_hieta, "|#it{#eta}|<3.0", "p"); */
/*     leg_d0_pt->AddEntry(gr_d0_pt_loeta, "|#it{#eta}|<1.4", "p"); */
/*     leg_d0_pt->SetTextFont(42); */
/*     leg_d0_pt->SetTextColor(kBlack); */
/*     leg_d0_pt->SetBorderSize(0); */
/*     leg_d0_pt->SetFillColor(0); */
/*     leg_d0_pt->SetLineWidth(0); */
/*     leg_d0_pt->SetTextSize(0.04); */
/*     leg_d0_pt->Draw(); */
/*     write_text(0.55, 0.88, "High-q^{2} multi-jet events"); */
/*     CMS_lumi(c_d0_pt); */

/*     float height_d0_pt = std::max(height_d0_pt_hieta, height_d0_pt_loeta); */
/*     float floor_d0_pt = std::min(floor_d0_pt_hieta, floor_d0_pt_loeta); */

/*     gr_d0_pt_hieta->SetMaximum(height_d0_pt*1.5); */
/*     gr_d0_pt_hieta->SetMinimum(0); */
/*     c_d0_pt->Update(); */
/*     c_d0_pt->RedrawAxis(); */
/*     c_d0_pt->SaveAs(figdir + "/d0_pt.png"); */
/*     gr_d0_pt_hieta->SetMaximum(height_d0_pt*100); */
/*     gr_d0_pt_hieta->SetMinimum(floor_d0_pt/100); */
/*     c_d0_pt->SetLogy(); */
/*     c_d0_pt->Update(); */
/*     c_d0_pt->RedrawAxis(); */
/*     c_d0_pt->SaveAs(figdir + "/d0_pt_log.png"); */

/*     TCanvas *c_d0_eta = new TCanvas("c_d0_eta", "", 800, 600); */
/*     canvas_setup(c_d0_eta); */
/*     gr_d0_eta_hipt->SetMarkerStyle(20); */
/*     gr_d0_eta_hipt->SetMarkerColor(kBlack); */
/*     gr_d0_eta_lopt->SetMarkerStyle(20); */
/*     gr_d0_eta_lopt->SetMarkerColor(kBlue); */
/*     gr_d0_eta_hipt->Draw("AP"); */
/*     gr_d0_eta_lopt->Draw("P SAME"); */
/*     gr_d0_eta_hipt->GetXaxis()->SetTitle("Track #it{#eta}"); */
/*     gr_d0_eta_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]"); */
/*     TLegend *leg_d0_eta = new TLegend(0.55, 0.65, 0.75, 0.85); */
/*     leg_d0_eta->AddEntry(gr_d0_eta_hipt, "1.0<#it{p_{T}}<3.0 GeV", "p"); */
/*     leg_d0_eta->AddEntry(gr_d0_eta_lopt, "0.1<#it{p_{T}}<1.0 GeV", "p"); */
/*     leg_d0_eta->SetTextFont(42); */
/*     leg_d0_eta->SetTextColor(kBlack); */
/*     leg_d0_eta->SetBorderSize(0); */
/*     leg_d0_eta->SetFillColor(0); */
/*     leg_d0_eta->SetLineWidth(0); */
/*     leg_d0_eta->SetTextSize(0.04); */
/*     leg_d0_eta->Draw(); */
/*     write_text(0.55, 0.88, "High-q^{2} multi-jet events"); */
/*     CMS_lumi(c_d0_eta); */

/*     float height_d0_eta = std::max(height_d0_eta_hipt, height_d0_eta_lopt); */
/*     float floor_d0_eta = std::min(floor_d0_eta_hipt, floor_d0_eta_lopt); */

/*     gr_d0_eta_hipt->SetMaximum(height_d0_eta*1.5); */
/*     gr_d0_eta_hipt->SetMinimum(0); */
/*     c_d0_eta->Update(); */
/*     c_d0_eta->RedrawAxis(); */
/*     c_d0_eta->SaveAs(figdir + "/d0_eta.png"); */
/*     gr_d0_eta_hipt->SetMaximum(height_d0_eta*100); */
/*     gr_d0_eta_hipt->SetMinimum(floor_d0_eta/100); */
/*     c_d0_eta->SetLogy(); */
/*     c_d0_eta->Update(); */
/*     c_d0_eta->RedrawAxis(); */
/*     c_d0_eta->SaveAs(figdir + "/d0_eta_log.png"); */


/*     TCanvas *c_d0_phi = new TCanvas("c_d0_phi", "", 800, 600); */
/*     canvas_setup(c_d0_phi); */
/*     gr_d0_phi_hipt->SetMarkerStyle(20); */
/*     gr_d0_phi_hipt->SetMarkerColor(kBlack); */
/*     gr_d0_phi_lopt->SetMarkerStyle(20); */
/*     gr_d0_phi_lopt->SetMarkerColor(kBlue); */
/*     gr_d0_phi_hipt->Draw("AP"); */
/*     gr_d0_phi_lopt->Draw("P SAME"); */
/*     gr_d0_phi_hipt->GetXaxis()->SetTitle("Track #it{#phi}"); */
/*     gr_d0_phi_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]"); */
/*     TLegend *leg_d0_phi = new TLegend(0.55, 0.65, 0.75, 0.85); */
/*     leg_d0_phi->AddEntry(gr_d0_phi_hipt, "1.0<#it{p_{T}}<3.0 GeV", "p"); */
/*     leg_d0_phi->AddEntry(gr_d0_phi_lopt, "0.1<#it{p_{T}}<1.0 GeV", "p"); */
/*     leg_d0_phi->SetTextFont(42); */
/*     leg_d0_phi->SetTextColor(kBlack); */
/*     leg_d0_phi->SetBorderSize(0); */
/*     leg_d0_phi->SetFillColor(0); */
/*     leg_d0_phi->SetLineWidth(0); */
/*     leg_d0_phi->SetTextSize(0.04); */
/*     leg_d0_phi->Draw(); */
/*     write_text(0.55, 0.88, "High-q^{2} multi-jet events"); */
/*     CMS_lumi(c_d0_phi); */

/*     float height_d0_phi = std::max(height_d0_phi_hipt, height_d0_phi_lopt); */
/*     float floor_d0_phi = std::min(floor_d0_phi_hipt, floor_d0_phi_lopt); */

/*     gr_d0_phi_hipt->SetMaximum(height_d0_phi*1.5); */
/*     gr_d0_phi_hipt->SetMinimum(0); */
/*     c_d0_phi->Update(); */
/*     c_d0_phi->RedrawAxis(); */
/*     c_d0_phi->SaveAs(figdir + "/d0_phi.png"); */
/*     gr_d0_phi_hipt->SetMaximum(height_d0_phi*100); */
/*     gr_d0_phi_hipt->SetMinimum(floor_d0_phi/100); */
/*     c_d0_phi->SetLogy(); */
/*     c_d0_phi->Update(); */
/*     c_d0_phi->RedrawAxis(); */
/*     c_d0_phi->SaveAs(figdir + "/d0_phi_log.png"); */



/*     TCanvas *c_dz_pt = new TCanvas("c_dz_pt", "", 800, 600); */
/*     canvas_setup(c_dz_pt); */
/*     gr_dz_pt_hieta->SetMarkerStyle(20); */
/*     gr_dz_pt_hieta->SetMarkerColor(kBlack); */
/*     gr_dz_pt_loeta->SetMarkerStyle(20); */
/*     gr_dz_pt_loeta->SetMarkerColor(kBlue); */
/*     gr_dz_pt_hieta->Draw("AP"); */
/*     gr_dz_pt_loeta->Draw("P SAME"); */
/*     gr_dz_pt_hieta->GetXaxis()->SetTitle("Track #it{p_{T}} [GeV]"); */
/*     gr_dz_pt_hieta->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [#mum]"); */
/*     TLegend *leg_dz_pt = new TLegend(0.55, 0.7, 0.75, 0.85); */
/*     leg_dz_pt->AddEntry(gr_dz_pt_hieta, "|#it{#eta}|<3.0", "p"); */
/*     leg_dz_pt->AddEntry(gr_dz_pt_loeta, "|#it{#eta}|<1.4", "p"); */
/*     leg_dz_pt->SetTextFont(42); */
/*     leg_dz_pt->SetTextColor(kBlack); */
/*     leg_dz_pt->SetBorderSize(0); */
/*     leg_dz_pt->SetFillColor(0); */
/*     leg_dz_pt->SetLineWidth(0); */
/*     leg_dz_pt->SetTextSize(0.04); */
/*     leg_dz_pt->Draw(); */
/*     write_text(0.55, 0.88, "High-q^{2} multi-jet events"); */
/*     CMS_lumi(c_dz_pt); */

/*     float height_dz_pt = std::max(height_dz_pt_hieta, height_dz_pt_loeta); */
/*     float floor_dz_pt = std::min(floor_dz_pt_hieta, floor_dz_pt_loeta); */

/*     gr_dz_pt_hieta->SetMaximum(height_dz_pt*1.5); */
/*     gr_dz_pt_hieta->SetMinimum(0); */
/*     c_dz_pt->Update(); */
/*     c_dz_pt->RedrawAxis(); */
/*     c_dz_pt->SaveAs(figdir + "/dz_pt.png"); */
/*     gr_dz_pt_hieta->SetMaximum(height_dz_pt*100); */
/*     gr_dz_pt_hieta->SetMinimum(floor_dz_pt/100); */
/*     c_dz_pt->SetLogy(); */
/*     c_dz_pt->Update(); */
/*     c_dz_pt->RedrawAxis(); */
/*     c_dz_pt->SaveAs(figdir + "/dz_pt_log.png"); */

/*     TCanvas *c_dz_eta = new TCanvas("c_dz_eta", "", 800, 600); */
/*     canvas_setup(c_dz_eta); */
/*     gr_dz_eta_hipt->SetMarkerStyle(20); */
/*     gr_dz_eta_hipt->SetMarkerColor(kBlack); */
/*     gr_dz_eta_lopt->SetMarkerStyle(20); */
/*     gr_dz_eta_lopt->SetMarkerColor(kBlue); */
/*     gr_dz_eta_hipt->Draw("AP"); */
/*     gr_dz_eta_lopt->Draw("P SAME"); */
/*     gr_dz_eta_hipt->GetXaxis()->SetTitle("Track #it{#eta}"); */
/*     gr_dz_eta_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [#mum]"); */
/*     TLegend *leg_dz_eta = new TLegend(0.55, 0.65, 0.75, 0.85); */
/*     leg_dz_eta->AddEntry(gr_dz_eta_hipt, "1.0<#it{p_{T}}<3.0 GeV", "p"); */
/*     leg_dz_eta->AddEntry(gr_dz_eta_lopt, "0.1<#it{p_{T}}<1.0 GeV", "p"); */
/*     leg_dz_eta->SetTextFont(42); */
/*     leg_dz_eta->SetTextColor(kBlack); */
/*     leg_dz_eta->SetBorderSize(0); */
/*     leg_dz_eta->SetFillColor(0); */
/*     leg_dz_eta->SetLineWidth(0); */
/*     leg_dz_eta->SetTextSize(0.04); */
/*     leg_dz_eta->Draw(); */
/*     write_text(0.55, 0.88, "High-q^{2} multi-jet events"); */
/*     CMS_lumi(c_dz_eta); */

/*     float height_dz_eta = std::max(height_dz_eta_hipt, height_dz_eta_lopt); */
/*     float floor_dz_eta = std::min(floor_dz_eta_hipt, floor_dz_eta_lopt); */

/*     gr_dz_eta_hipt->SetMaximum(height_dz_eta*1.5); */
/*     gr_dz_eta_hipt->SetMinimum(0); */
/*     c_dz_eta->Update(); */
/*     c_dz_eta->RedrawAxis(); */
/*     c_dz_eta->SaveAs(figdir + "/dz_eta.png"); */
/*     gr_dz_eta_hipt->SetMaximum(height_dz_eta*100); */
/*     gr_dz_eta_hipt->SetMinimum(floor_dz_eta/100); */
/*     c_dz_eta->SetLogy(); */
/*     c_dz_eta->Update(); */
/*     c_dz_eta->RedrawAxis(); */
/*     c_dz_eta->SaveAs(figdir + "/dz_eta_log.png"); */


/*     TCanvas *c_dz_phi = new TCanvas("c_dz_phi", "", 800, 600); */
/*     canvas_setup(c_dz_phi); */
/*     gr_dz_phi_hipt->SetMarkerStyle(20); */
/*     gr_dz_phi_hipt->SetMarkerColor(kBlack); */
/*     gr_dz_phi_lopt->SetMarkerStyle(20); */
/*     gr_dz_phi_lopt->SetMarkerColor(kBlue); */
/*     gr_dz_phi_hipt->Draw("AP"); */
/*     gr_dz_phi_lopt->Draw("P SAME"); */
/*     gr_dz_phi_hipt->GetXaxis()->SetTitle("Track #it{#phi}"); */
/*     gr_dz_phi_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [#mum]"); */
/*     TLegend *leg_dz_phi = new TLegend(0.55, 0.65, 0.75, 0.85); */
/*     leg_dz_phi->AddEntry(gr_dz_phi_hipt, "1.0<#it{p_{T}}<3.0 GeV", "p"); */
/*     leg_dz_phi->AddEntry(gr_dz_phi_lopt, "0.1<#it{p_{T}}<1.0 GeV", "p"); */
/*     leg_dz_phi->SetTextFont(42); */
/*     leg_dz_phi->SetTextColor(kBlack); */
/*     leg_dz_phi->SetBorderSize(0); */
/*     leg_dz_phi->SetFillColor(0); */
/*     leg_dz_phi->SetLineWidth(0); */
/*     leg_dz_phi->SetTextSize(0.04); */
/*     leg_dz_phi->Draw(); */
/*     write_text(0.55, 0.88, "High-q^{2} multi-jet events"); */
/*     CMS_lumi(c_dz_phi); */

/*     float height_dz_phi = std::max(height_dz_phi_hipt, height_dz_phi_lopt); */
/*     float floor_dz_phi = std::min(floor_dz_phi_hipt, floor_dz_phi_lopt); */

/*     gr_dz_phi_hipt->SetMaximum(height_dz_phi*1.5); */
/*     gr_dz_phi_hipt->SetMinimum(0); */
/*     c_dz_phi->Update(); */
/*     c_dz_phi->RedrawAxis(); */
/*     c_dz_phi->SaveAs(figdir + "/dz_phi.png"); */
/*     gr_dz_phi_hipt->SetMaximum(height_dz_phi*100); */
/*     gr_dz_phi_hipt->SetMinimum(floor_dz_phi/100); */
/*     c_dz_phi->SetLogy(); */
/*     c_dz_phi->Update(); */
/*     c_dz_phi->RedrawAxis(); */
/*     c_dz_phi->SaveAs(figdir + "/dz_phi_log.png"); */



    return 0;
}
