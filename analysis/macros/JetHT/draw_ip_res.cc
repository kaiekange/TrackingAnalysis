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
/* const int nbins = 500; */
const int nbins = 499;

int draw_ip_res(TString era){

    /* TString figdir = "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_"+era+"/ip_res/"; */
    TString figdir = "/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/macros/JetHT/figures/JetHT_"+era+"/ip_res/";

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, 2022 " + era;

    float reso_data_d0_pt_loeta[nbins];
    float reso_data_dz_pt_loeta[nbins];
    float reso_data_d0_pt_hieta[nbins];
    float reso_data_dz_pt_hieta[nbins];
    float reso_data_d0_pt_eta[nbins];
    float reso_data_dz_pt_eta[nbins];

    float reso_data_d0_eta_lopt[nbins];
    float reso_data_dz_eta_lopt[nbins];
    float reso_data_d0_eta_hipt[nbins];
    float reso_data_dz_eta_hipt[nbins];
    float reso_data_d0_eta_ulpt[nbins];
    float reso_data_dz_eta_ulpt[nbins];

    float reso_data_d0_phi_lopt[nbins];
    float reso_data_dz_phi_lopt[nbins];
    float reso_data_d0_phi_hipt[nbins];
    float reso_data_dz_phi_hipt[nbins];
    float reso_data_d0_phi_ulpt[nbins];
    float reso_data_dz_phi_ulpt[nbins];

    float reso_mc_d0_pt_loeta[nbins];
    float reso_mc_dz_pt_loeta[nbins];
    float reso_mc_d0_pt_hieta[nbins];
    float reso_mc_dz_pt_hieta[nbins];
    float reso_mc_d0_pt_eta[nbins];
    float reso_mc_dz_pt_eta[nbins];

    float reso_mc_d0_eta_lopt[nbins];
    float reso_mc_dz_eta_lopt[nbins];
    float reso_mc_d0_eta_hipt[nbins];
    float reso_mc_dz_eta_hipt[nbins];
    float reso_mc_d0_eta_ulpt[nbins];
    float reso_mc_dz_eta_ulpt[nbins];

    float reso_mc_d0_phi_lopt[nbins];
    float reso_mc_dz_phi_lopt[nbins];
    float reso_mc_d0_phi_hipt[nbins];
    float reso_mc_dz_phi_hipt[nbins];
    float reso_mc_d0_phi_ulpt[nbins];
    float reso_mc_dz_phi_ulpt[nbins];

    float reso_div_d0_pt_loeta[nbins];
    float reso_div_dz_pt_loeta[nbins];
    float reso_div_d0_pt_hieta[nbins];
    float reso_div_dz_pt_hieta[nbins];
    float reso_div_d0_pt_eta[nbins];
    float reso_div_dz_pt_eta[nbins];

    float reso_div_d0_eta_lopt[nbins];
    float reso_div_dz_eta_lopt[nbins];
    float reso_div_d0_eta_hipt[nbins];
    float reso_div_dz_eta_hipt[nbins];
    float reso_div_d0_eta_ulpt[nbins];
    float reso_div_dz_eta_ulpt[nbins];

    float reso_div_d0_phi_lopt[nbins];
    float reso_div_dz_phi_lopt[nbins];
    float reso_div_d0_phi_hipt[nbins];
    float reso_div_dz_phi_hipt[nbins];
    float reso_div_d0_phi_ulpt[nbins];
    float reso_div_dz_phi_ulpt[nbins];

    float eta[nbins];
    float phi[nbins];
    float pt[nbins];

    /* for(int i=0; i<nbins; i++){ */
    int i=0;
    for(int idx=0; idx<500; idx++){
        if(idx==383) continue;
        nlohmann::json results;
        /* std::ifstream infile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_"+era+Form("/ip_res/fit_%d.json",i)); */
        std::ifstream infile("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/macros/JetHT/json/JetHT_"+era+Form("/ip_res/fit_%d.json",idx));
        infile >> results;

        eta[i] = results["eta"];
        phi[i] = results["phi"];
        pt[i] = results["pt"];

        reso_data_d0_pt_loeta[i] = results["reso_data_d0_pt_loeta"];
        reso_data_dz_pt_loeta[i] = results["reso_data_dz_pt_loeta"];
        reso_data_d0_pt_hieta[i] = results["reso_data_d0_pt_hieta"];
        reso_data_dz_pt_hieta[i] = results["reso_data_dz_pt_hieta"];
        reso_data_d0_pt_eta[i] = results["reso_data_d0_pt_eta"];
        reso_data_dz_pt_eta[i] = results["reso_data_dz_pt_eta"];

        reso_data_d0_eta_lopt[i] = results["reso_data_d0_eta_lopt"];
        reso_data_dz_eta_lopt[i] = results["reso_data_dz_eta_lopt"];
        reso_data_d0_eta_hipt[i] = results["reso_data_d0_eta_hipt"];
        reso_data_dz_eta_hipt[i] = results["reso_data_dz_eta_hipt"];
        reso_data_d0_eta_ulpt[i] = results["reso_data_d0_eta_ulpt"];
        reso_data_dz_eta_ulpt[i] = results["reso_data_dz_eta_ulpt"];

        reso_data_d0_phi_lopt[i] = results["reso_data_d0_phi_lopt"];
        reso_data_dz_phi_lopt[i] = results["reso_data_dz_phi_lopt"];
        reso_data_d0_phi_hipt[i] = results["reso_data_d0_phi_hipt"];
        reso_data_dz_phi_hipt[i] = results["reso_data_dz_phi_hipt"];
        reso_data_d0_phi_ulpt[i] = results["reso_data_d0_phi_ulpt"];
        reso_data_dz_phi_ulpt[i] = results["reso_data_dz_phi_ulpt"];

        reso_mc_d0_pt_loeta[i] = results["reso_mc_d0_pt_loeta"];
        reso_mc_dz_pt_loeta[i] = results["reso_mc_dz_pt_loeta"];
        reso_mc_d0_pt_hieta[i] = results["reso_mc_d0_pt_hieta"];
        reso_mc_dz_pt_hieta[i] = results["reso_mc_dz_pt_hieta"];
        reso_mc_d0_pt_eta[i] = results["reso_mc_d0_pt_eta"];
        reso_mc_dz_pt_eta[i] = results["reso_mc_dz_pt_eta"];

        reso_mc_d0_eta_lopt[i] = results["reso_mc_d0_eta_lopt"];
        reso_mc_dz_eta_lopt[i] = results["reso_mc_dz_eta_lopt"];
        reso_mc_d0_eta_hipt[i] = results["reso_mc_d0_eta_hipt"];
        reso_mc_dz_eta_hipt[i] = results["reso_mc_dz_eta_hipt"];
        reso_mc_d0_eta_ulpt[i] = results["reso_mc_d0_eta_ulpt"];
        reso_mc_dz_eta_ulpt[i] = results["reso_mc_dz_eta_ulpt"];

        reso_mc_d0_phi_lopt[i] = results["reso_mc_d0_phi_lopt"];
        reso_mc_dz_phi_lopt[i] = results["reso_mc_dz_phi_lopt"];
        reso_mc_d0_phi_hipt[i] = results["reso_mc_d0_phi_hipt"];
        reso_mc_dz_phi_hipt[i] = results["reso_mc_dz_phi_hipt"];
        reso_mc_d0_phi_ulpt[i] = results["reso_mc_d0_phi_ulpt"];
        reso_mc_dz_phi_ulpt[i] = results["reso_mc_dz_phi_ulpt"];

        reso_div_d0_pt_loeta[i] = reso_data_d0_pt_loeta[i] / reso_mc_d0_pt_loeta[i];
        reso_div_dz_pt_loeta[i] = reso_data_dz_pt_loeta[i] / reso_mc_dz_pt_loeta[i];
        reso_div_d0_pt_hieta[i] = reso_data_d0_pt_hieta[i] / reso_mc_d0_pt_hieta[i];
        reso_div_dz_pt_hieta[i] = reso_data_dz_pt_hieta[i] / reso_mc_dz_pt_hieta[i];
        reso_div_d0_pt_eta[i] = reso_data_d0_pt_eta[i] / reso_mc_d0_pt_eta[i];
        reso_div_dz_pt_eta[i] = reso_data_dz_pt_eta[i] / reso_mc_dz_pt_eta[i];

        reso_div_d0_eta_lopt[i] = reso_data_d0_eta_lopt[i] / reso_mc_d0_eta_lopt[i];
        reso_div_dz_eta_lopt[i] = reso_data_dz_eta_lopt[i] / reso_mc_dz_eta_lopt[i];
        reso_div_d0_eta_hipt[i] = reso_data_d0_eta_hipt[i] / reso_mc_d0_eta_hipt[i];
        reso_div_dz_eta_hipt[i] = reso_data_dz_eta_hipt[i] / reso_mc_dz_eta_hipt[i];
        reso_div_d0_eta_ulpt[i] = reso_data_d0_eta_ulpt[i] / reso_mc_d0_eta_ulpt[i];
        reso_div_dz_eta_ulpt[i] = reso_data_dz_eta_ulpt[i] / reso_mc_dz_eta_ulpt[i];

        reso_div_d0_phi_lopt[i] = reso_data_d0_phi_lopt[i] / reso_mc_d0_phi_lopt[i];
        reso_div_dz_phi_lopt[i] = reso_data_dz_phi_lopt[i] / reso_mc_dz_phi_lopt[i];
        reso_div_d0_phi_hipt[i] = reso_data_d0_phi_hipt[i] / reso_mc_d0_phi_hipt[i];
        reso_div_dz_phi_hipt[i] = reso_data_dz_phi_hipt[i] / reso_mc_dz_phi_hipt[i];
        reso_div_d0_phi_ulpt[i] = reso_data_d0_phi_ulpt[i] / reso_mc_d0_phi_ulpt[i];
        reso_div_dz_phi_ulpt[i] = reso_data_dz_phi_ulpt[i] / reso_mc_dz_phi_ulpt[i];

        i++;
    }

    TGraph * gr_data_d0_pt_loeta = new TGraph(nbins, pt, reso_data_d0_pt_loeta);
    TGraph * gr_data_dz_pt_loeta = new TGraph(nbins, pt, reso_data_dz_pt_loeta);
    TGraph * gr_data_d0_pt_hieta = new TGraph(nbins, pt, reso_data_d0_pt_hieta);
    TGraph * gr_data_dz_pt_hieta = new TGraph(nbins, pt, reso_data_dz_pt_hieta);
    TGraph * gr_data_d0_pt_eta = new TGraph(nbins, pt, reso_data_d0_pt_eta);
    TGraph * gr_data_dz_pt_eta = new TGraph(nbins, pt, reso_data_dz_pt_eta);

    TGraph * gr_data_d0_eta_lopt = new TGraph(nbins, eta, reso_data_d0_eta_lopt);
    TGraph * gr_data_dz_eta_lopt = new TGraph(nbins, eta, reso_data_dz_eta_lopt);
    TGraph * gr_data_d0_eta_hipt = new TGraph(nbins, eta, reso_data_d0_eta_hipt);
    TGraph * gr_data_dz_eta_hipt = new TGraph(nbins, eta, reso_data_dz_eta_hipt);
    TGraph * gr_data_d0_eta_ulpt = new TGraph(nbins, eta, reso_data_d0_eta_ulpt);
    TGraph * gr_data_dz_eta_ulpt = new TGraph(nbins, eta, reso_data_dz_eta_ulpt);

    TGraph * gr_data_d0_phi_lopt = new TGraph(nbins, phi, reso_data_d0_phi_lopt);
    TGraph * gr_data_dz_phi_lopt = new TGraph(nbins, phi, reso_data_dz_phi_lopt);
    TGraph * gr_data_d0_phi_hipt = new TGraph(nbins, phi, reso_data_d0_phi_hipt);
    TGraph * gr_data_dz_phi_hipt = new TGraph(nbins, phi, reso_data_dz_phi_hipt);
    TGraph * gr_data_d0_phi_ulpt = new TGraph(nbins, phi, reso_data_d0_phi_ulpt);
    TGraph * gr_data_dz_phi_ulpt = new TGraph(nbins, phi, reso_data_dz_phi_ulpt);

    TGraph * gr_mc_d0_pt_loeta = new TGraph(nbins, pt, reso_mc_d0_pt_loeta);
    TGraph * gr_mc_dz_pt_loeta = new TGraph(nbins, pt, reso_mc_dz_pt_loeta);
    TGraph * gr_mc_d0_pt_hieta = new TGraph(nbins, pt, reso_mc_d0_pt_hieta);
    TGraph * gr_mc_dz_pt_hieta = new TGraph(nbins, pt, reso_mc_dz_pt_hieta);
    TGraph * gr_mc_d0_pt_eta = new TGraph(nbins, pt, reso_mc_d0_pt_eta);
    TGraph * gr_mc_dz_pt_eta = new TGraph(nbins, pt, reso_mc_dz_pt_eta);

    TGraph * gr_mc_d0_eta_lopt = new TGraph(nbins, eta, reso_mc_d0_eta_lopt);
    TGraph * gr_mc_dz_eta_lopt = new TGraph(nbins, eta, reso_mc_dz_eta_lopt);
    TGraph * gr_mc_d0_eta_hipt = new TGraph(nbins, eta, reso_mc_d0_eta_hipt);
    TGraph * gr_mc_dz_eta_hipt = new TGraph(nbins, eta, reso_mc_dz_eta_hipt);
    TGraph * gr_mc_d0_eta_ulpt = new TGraph(nbins, eta, reso_mc_d0_eta_ulpt);
    TGraph * gr_mc_dz_eta_ulpt = new TGraph(nbins, eta, reso_mc_dz_eta_ulpt);

    TGraph * gr_mc_d0_phi_lopt = new TGraph(nbins, phi, reso_mc_d0_phi_lopt);
    TGraph * gr_mc_dz_phi_lopt = new TGraph(nbins, phi, reso_mc_dz_phi_lopt);
    TGraph * gr_mc_d0_phi_hipt = new TGraph(nbins, phi, reso_mc_d0_phi_hipt);
    TGraph * gr_mc_dz_phi_hipt = new TGraph(nbins, phi, reso_mc_dz_phi_hipt);
    TGraph * gr_mc_d0_phi_ulpt = new TGraph(nbins, phi, reso_mc_d0_phi_ulpt);
    TGraph * gr_mc_dz_phi_ulpt = new TGraph(nbins, phi, reso_mc_dz_phi_ulpt);

    TGraph * gr_div_d0_pt_loeta = new TGraph(nbins, pt, reso_div_d0_pt_loeta);
    TGraph * gr_div_dz_pt_loeta = new TGraph(nbins, pt, reso_div_dz_pt_loeta);
    TGraph * gr_div_d0_pt_hieta = new TGraph(nbins, pt, reso_div_d0_pt_hieta);
    TGraph * gr_div_dz_pt_hieta = new TGraph(nbins, pt, reso_div_dz_pt_hieta);
    TGraph * gr_div_d0_pt_eta = new TGraph(nbins, pt, reso_div_d0_pt_eta);
    TGraph * gr_div_dz_pt_eta = new TGraph(nbins, pt, reso_div_dz_pt_eta);

    TGraph * gr_div_d0_eta_lopt = new TGraph(nbins, eta, reso_div_d0_eta_lopt);
    TGraph * gr_div_dz_eta_lopt = new TGraph(nbins, eta, reso_div_dz_eta_lopt);
    TGraph * gr_div_d0_eta_hipt = new TGraph(nbins, eta, reso_div_d0_eta_hipt);
    TGraph * gr_div_dz_eta_hipt = new TGraph(nbins, eta, reso_div_dz_eta_hipt);
    TGraph * gr_div_d0_eta_ulpt = new TGraph(nbins, eta, reso_div_d0_eta_ulpt);
    TGraph * gr_div_dz_eta_ulpt = new TGraph(nbins, eta, reso_div_dz_eta_ulpt);

    TGraph * gr_div_d0_phi_lopt = new TGraph(nbins, phi, reso_div_d0_phi_lopt);
    TGraph * gr_div_dz_phi_lopt = new TGraph(nbins, phi, reso_div_dz_phi_lopt);
    TGraph * gr_div_d0_phi_hipt = new TGraph(nbins, phi, reso_div_d0_phi_hipt);
    TGraph * gr_div_dz_phi_hipt = new TGraph(nbins, phi, reso_div_dz_phi_hipt);
    TGraph * gr_div_d0_phi_ulpt = new TGraph(nbins, phi, reso_div_d0_phi_ulpt);
    TGraph * gr_div_dz_phi_ulpt = new TGraph(nbins, phi, reso_div_dz_phi_ulpt);

    float height_data_d0_pt_loeta = *std::max_element(reso_data_d0_pt_loeta, reso_data_d0_pt_loeta+nbins);
    float height_data_dz_pt_loeta = *std::max_element(reso_data_dz_pt_loeta, reso_data_dz_pt_loeta+nbins);
    float height_data_d0_pt_hieta = *std::max_element(reso_data_d0_pt_hieta, reso_data_d0_pt_hieta+nbins);
    float height_data_dz_pt_hieta = *std::max_element(reso_data_dz_pt_hieta, reso_data_dz_pt_hieta+nbins);
    float height_data_d0_pt_eta = *std::max_element(reso_data_d0_pt_eta, reso_data_d0_pt_eta+nbins);
    float height_data_dz_pt_eta = *std::max_element(reso_data_dz_pt_eta, reso_data_dz_pt_eta+nbins);
    float height_data_d0_eta_lopt = *std::max_element(reso_data_d0_eta_lopt, reso_data_d0_eta_lopt+nbins);
    float height_data_dz_eta_lopt = *std::max_element(reso_data_dz_eta_lopt, reso_data_dz_eta_lopt+nbins);
    float height_data_d0_eta_hipt = *std::max_element(reso_data_d0_eta_hipt, reso_data_d0_eta_hipt+nbins);
    float height_data_dz_eta_hipt = *std::max_element(reso_data_dz_eta_hipt, reso_data_dz_eta_hipt+nbins);
    float height_data_d0_eta_ulpt = *std::max_element(reso_data_d0_eta_ulpt, reso_data_d0_eta_ulpt+nbins);
    float height_data_dz_eta_ulpt = *std::max_element(reso_data_dz_eta_ulpt, reso_data_dz_eta_ulpt+nbins);
    float height_data_d0_phi_lopt = *std::max_element(reso_data_d0_phi_lopt, reso_data_d0_phi_lopt+nbins);
    float height_data_dz_phi_lopt = *std::max_element(reso_data_dz_phi_lopt, reso_data_dz_phi_lopt+nbins);
    float height_data_d0_phi_hipt = *std::max_element(reso_data_d0_phi_hipt, reso_data_d0_phi_hipt+nbins);
    float height_data_dz_phi_hipt = *std::max_element(reso_data_dz_phi_hipt, reso_data_dz_phi_hipt+nbins);
    float height_data_d0_phi_ulpt = *std::max_element(reso_data_d0_phi_ulpt, reso_data_d0_phi_ulpt+nbins);
    float height_data_dz_phi_ulpt = *std::max_element(reso_data_dz_phi_ulpt, reso_data_dz_phi_ulpt+nbins);

    float height_mc_d0_pt_loeta = *std::max_element(reso_mc_d0_pt_loeta, reso_mc_d0_pt_loeta+nbins);
    float height_mc_dz_pt_loeta = *std::max_element(reso_mc_dz_pt_loeta, reso_mc_dz_pt_loeta+nbins);
    float height_mc_d0_pt_hieta = *std::max_element(reso_mc_d0_pt_hieta, reso_mc_d0_pt_hieta+nbins);
    float height_mc_dz_pt_hieta = *std::max_element(reso_mc_dz_pt_hieta, reso_mc_dz_pt_hieta+nbins);
    float height_mc_d0_pt_eta = *std::max_element(reso_mc_d0_pt_eta, reso_mc_d0_pt_eta+nbins);
    float height_mc_dz_pt_eta = *std::max_element(reso_mc_dz_pt_eta, reso_mc_dz_pt_eta+nbins);
    float height_mc_d0_eta_lopt = *std::max_element(reso_mc_d0_eta_lopt, reso_mc_d0_eta_lopt+nbins);
    float height_mc_dz_eta_lopt = *std::max_element(reso_mc_dz_eta_lopt, reso_mc_dz_eta_lopt+nbins);
    float height_mc_d0_eta_hipt = *std::max_element(reso_mc_d0_eta_hipt, reso_mc_d0_eta_hipt+nbins);
    float height_mc_dz_eta_hipt = *std::max_element(reso_mc_dz_eta_hipt, reso_mc_dz_eta_hipt+nbins);
    float height_mc_d0_eta_ulpt = *std::max_element(reso_mc_d0_eta_ulpt, reso_mc_d0_eta_ulpt+nbins);
    float height_mc_dz_eta_ulpt = *std::max_element(reso_mc_dz_eta_ulpt, reso_mc_dz_eta_ulpt+nbins);
    float height_mc_d0_phi_lopt = *std::max_element(reso_mc_d0_phi_lopt, reso_mc_d0_phi_lopt+nbins);
    float height_mc_dz_phi_lopt = *std::max_element(reso_mc_dz_phi_lopt, reso_mc_dz_phi_lopt+nbins);
    float height_mc_d0_phi_hipt = *std::max_element(reso_mc_d0_phi_hipt, reso_mc_d0_phi_hipt+nbins);
    float height_mc_dz_phi_hipt = *std::max_element(reso_mc_dz_phi_hipt, reso_mc_dz_phi_hipt+nbins);
    float height_mc_d0_phi_ulpt = *std::max_element(reso_mc_d0_phi_ulpt, reso_mc_d0_phi_ulpt+nbins);
    float height_mc_dz_phi_ulpt = *std::max_element(reso_mc_dz_phi_ulpt, reso_mc_dz_phi_ulpt+nbins);

    float height_d0_pt_loeta = std::max(height_data_d0_pt_loeta, height_mc_d0_pt_loeta);
    float height_dz_pt_loeta = std::max(height_data_dz_pt_loeta, height_mc_dz_pt_loeta);
    float height_d0_pt_hieta = std::max(height_data_d0_pt_hieta, height_mc_d0_pt_hieta);
    float height_dz_pt_hieta = std::max(height_data_dz_pt_hieta, height_mc_dz_pt_hieta);
    float height_d0_pt_eta = std::max(height_data_d0_pt_eta, height_mc_d0_pt_eta);
    float height_dz_pt_eta = std::max(height_data_dz_pt_eta, height_mc_dz_pt_eta);
    float height_d0_eta_lopt = std::max(height_data_d0_eta_lopt, height_mc_d0_eta_lopt);
    float height_dz_eta_lopt = std::max(height_data_dz_eta_lopt, height_mc_dz_eta_lopt);
    float height_d0_eta_hipt = std::max(height_data_d0_eta_hipt, height_mc_d0_eta_hipt);
    float height_dz_eta_hipt = std::max(height_data_dz_eta_hipt, height_mc_dz_eta_hipt);
    float height_d0_eta_ulpt = std::max(height_data_d0_eta_ulpt, height_mc_d0_eta_ulpt);
    float height_dz_eta_ulpt = std::max(height_data_dz_eta_ulpt, height_mc_dz_eta_ulpt);
    float height_d0_phi_lopt = std::max(height_data_d0_phi_lopt, height_mc_d0_phi_lopt);
    float height_dz_phi_lopt = std::max(height_data_dz_phi_lopt, height_mc_dz_phi_lopt);
    float height_d0_phi_hipt = std::max(height_data_d0_phi_hipt, height_mc_d0_phi_hipt);
    float height_dz_phi_hipt = std::max(height_data_dz_phi_hipt, height_mc_dz_phi_hipt);
    float height_d0_phi_ulpt = std::max(height_data_d0_phi_ulpt, height_mc_d0_phi_ulpt);
    float height_dz_phi_ulpt = std::max(height_data_dz_phi_ulpt, height_mc_dz_phi_ulpt);

    float height_div_d0_pt_loeta = *std::max_element(reso_div_d0_pt_loeta, reso_div_d0_pt_loeta+nbins);
    float height_div_dz_pt_loeta = *std::max_element(reso_div_dz_pt_loeta, reso_div_dz_pt_loeta+nbins);
    float height_div_d0_pt_hieta = *std::max_element(reso_div_d0_pt_hieta, reso_div_d0_pt_hieta+nbins);
    float height_div_dz_pt_hieta = *std::max_element(reso_div_dz_pt_hieta, reso_div_dz_pt_hieta+nbins);
    float height_div_d0_pt_eta = *std::max_element(reso_div_d0_pt_eta, reso_div_d0_pt_eta+nbins);
    float height_div_dz_pt_eta = *std::max_element(reso_div_dz_pt_eta, reso_div_dz_pt_eta+nbins);
    float height_div_d0_eta_lopt = *std::max_element(reso_div_d0_eta_lopt, reso_div_d0_eta_lopt+nbins);
    float height_div_dz_eta_lopt = *std::max_element(reso_div_dz_eta_lopt, reso_div_dz_eta_lopt+nbins);
    float height_div_d0_eta_hipt = *std::max_element(reso_div_d0_eta_hipt, reso_div_d0_eta_hipt+nbins);
    float height_div_dz_eta_hipt = *std::max_element(reso_div_dz_eta_hipt, reso_div_dz_eta_hipt+nbins);
    float height_div_d0_eta_ulpt = *std::max_element(reso_div_d0_eta_ulpt, reso_div_d0_eta_ulpt+nbins);
    float height_div_dz_eta_ulpt = *std::max_element(reso_div_dz_eta_ulpt, reso_div_dz_eta_ulpt+nbins);
    float height_div_d0_phi_lopt = *std::max_element(reso_div_d0_phi_lopt, reso_div_d0_phi_lopt+nbins);
    float height_div_dz_phi_lopt = *std::max_element(reso_div_dz_phi_lopt, reso_div_dz_phi_lopt+nbins);
    float height_div_d0_phi_hipt = *std::max_element(reso_div_d0_phi_hipt, reso_div_d0_phi_hipt+nbins);
    float height_div_dz_phi_hipt = *std::max_element(reso_div_dz_phi_hipt, reso_div_dz_phi_hipt+nbins);
    float height_div_d0_phi_ulpt = *std::max_element(reso_div_d0_phi_ulpt, reso_div_d0_phi_ulpt+nbins);
    float height_div_dz_phi_ulpt = *std::max_element(reso_div_dz_phi_ulpt, reso_div_dz_phi_ulpt+nbins);


    float floor_data_d0_pt_loeta = *std::min_element(reso_data_d0_pt_loeta, reso_data_d0_pt_loeta+nbins);
    float floor_data_dz_pt_loeta = *std::min_element(reso_data_dz_pt_loeta, reso_data_dz_pt_loeta+nbins);
    float floor_data_d0_pt_hieta = *std::min_element(reso_data_d0_pt_hieta, reso_data_d0_pt_hieta+nbins);
    float floor_data_dz_pt_hieta = *std::min_element(reso_data_dz_pt_hieta, reso_data_dz_pt_hieta+nbins);
    float floor_data_d0_pt_eta = *std::min_element(reso_data_d0_pt_eta, reso_data_d0_pt_eta+nbins);
    float floor_data_dz_pt_eta = *std::min_element(reso_data_dz_pt_eta, reso_data_dz_pt_eta+nbins);
    float floor_data_d0_eta_lopt = *std::min_element(reso_data_d0_eta_lopt, reso_data_d0_eta_lopt+nbins);
    float floor_data_dz_eta_lopt = *std::min_element(reso_data_dz_eta_lopt, reso_data_dz_eta_lopt+nbins);
    float floor_data_d0_eta_hipt = *std::min_element(reso_data_d0_eta_hipt, reso_data_d0_eta_hipt+nbins);
    float floor_data_dz_eta_hipt = *std::min_element(reso_data_dz_eta_hipt, reso_data_dz_eta_hipt+nbins);
    float floor_data_d0_eta_ulpt = *std::min_element(reso_data_d0_eta_ulpt, reso_data_d0_eta_ulpt+nbins);
    float floor_data_dz_eta_ulpt = *std::min_element(reso_data_dz_eta_ulpt, reso_data_dz_eta_ulpt+nbins);
    float floor_data_d0_phi_lopt = *std::min_element(reso_data_d0_phi_lopt, reso_data_d0_phi_lopt+nbins);
    float floor_data_dz_phi_lopt = *std::min_element(reso_data_dz_phi_lopt, reso_data_dz_phi_lopt+nbins);
    float floor_data_d0_phi_hipt = *std::min_element(reso_data_d0_phi_hipt, reso_data_d0_phi_hipt+nbins);
    float floor_data_dz_phi_hipt = *std::min_element(reso_data_dz_phi_hipt, reso_data_dz_phi_hipt+nbins);
    float floor_data_d0_phi_ulpt = *std::min_element(reso_data_d0_phi_ulpt, reso_data_d0_phi_ulpt+nbins);
    float floor_data_dz_phi_ulpt = *std::min_element(reso_data_dz_phi_ulpt, reso_data_dz_phi_ulpt+nbins);

    float floor_mc_d0_pt_loeta = *std::min_element(reso_mc_d0_pt_loeta, reso_mc_d0_pt_loeta+nbins);
    float floor_mc_dz_pt_loeta = *std::min_element(reso_mc_dz_pt_loeta, reso_mc_dz_pt_loeta+nbins);
    float floor_mc_d0_pt_hieta = *std::min_element(reso_mc_d0_pt_hieta, reso_mc_d0_pt_hieta+nbins);
    float floor_mc_dz_pt_hieta = *std::min_element(reso_mc_dz_pt_hieta, reso_mc_dz_pt_hieta+nbins);
    float floor_mc_d0_pt_eta = *std::min_element(reso_mc_d0_pt_eta, reso_mc_d0_pt_eta+nbins);
    float floor_mc_dz_pt_eta = *std::min_element(reso_mc_dz_pt_eta, reso_mc_dz_pt_eta+nbins);
    float floor_mc_d0_eta_lopt = *std::min_element(reso_mc_d0_eta_lopt, reso_mc_d0_eta_lopt+nbins);
    float floor_mc_dz_eta_lopt = *std::min_element(reso_mc_dz_eta_lopt, reso_mc_dz_eta_lopt+nbins);
    float floor_mc_d0_eta_hipt = *std::min_element(reso_mc_d0_eta_hipt, reso_mc_d0_eta_hipt+nbins);
    float floor_mc_dz_eta_hipt = *std::min_element(reso_mc_dz_eta_hipt, reso_mc_dz_eta_hipt+nbins);
    float floor_mc_d0_eta_ulpt = *std::min_element(reso_mc_d0_eta_ulpt, reso_mc_d0_eta_ulpt+nbins);
    float floor_mc_dz_eta_ulpt = *std::min_element(reso_mc_dz_eta_ulpt, reso_mc_dz_eta_ulpt+nbins);
    float floor_mc_d0_phi_lopt = *std::min_element(reso_mc_d0_phi_lopt, reso_mc_d0_phi_lopt+nbins);
    float floor_mc_dz_phi_lopt = *std::min_element(reso_mc_dz_phi_lopt, reso_mc_dz_phi_lopt+nbins);
    float floor_mc_d0_phi_hipt = *std::min_element(reso_mc_d0_phi_hipt, reso_mc_d0_phi_hipt+nbins);
    float floor_mc_dz_phi_hipt = *std::min_element(reso_mc_dz_phi_hipt, reso_mc_dz_phi_hipt+nbins);
    float floor_mc_d0_phi_ulpt = *std::min_element(reso_mc_d0_phi_ulpt, reso_mc_d0_phi_ulpt+nbins);
    float floor_mc_dz_phi_ulpt = *std::min_element(reso_mc_dz_phi_ulpt, reso_mc_dz_phi_ulpt+nbins);

    float floor_d0_pt_loeta = std::min(floor_data_d0_pt_loeta, floor_mc_d0_pt_loeta);
    float floor_dz_pt_loeta = std::min(floor_data_dz_pt_loeta, floor_mc_dz_pt_loeta);
    float floor_d0_pt_hieta = std::min(floor_data_d0_pt_hieta, floor_mc_d0_pt_hieta);
    float floor_dz_pt_hieta = std::min(floor_data_dz_pt_hieta, floor_mc_dz_pt_hieta);
    float floor_d0_pt_eta = std::min(floor_data_d0_pt_eta, floor_mc_d0_pt_eta);
    float floor_dz_pt_eta = std::min(floor_data_dz_pt_eta, floor_mc_dz_pt_eta);
    float floor_d0_eta_lopt = std::min(floor_data_d0_eta_lopt, floor_mc_d0_eta_lopt);
    float floor_dz_eta_lopt = std::min(floor_data_dz_eta_lopt, floor_mc_dz_eta_lopt);
    float floor_d0_eta_hipt = std::min(floor_data_d0_eta_hipt, floor_mc_d0_eta_hipt);
    float floor_dz_eta_hipt = std::min(floor_data_dz_eta_hipt, floor_mc_dz_eta_hipt);
    float floor_d0_eta_ulpt = std::min(floor_data_d0_eta_ulpt, floor_mc_d0_eta_ulpt);
    float floor_dz_eta_ulpt = std::min(floor_data_dz_eta_ulpt, floor_mc_dz_eta_ulpt);
    float floor_d0_phi_lopt = std::min(floor_data_d0_phi_lopt, floor_mc_d0_phi_lopt);
    float floor_dz_phi_lopt = std::min(floor_data_dz_phi_lopt, floor_mc_dz_phi_lopt);
    float floor_d0_phi_hipt = std::min(floor_data_d0_phi_hipt, floor_mc_d0_phi_hipt);
    float floor_dz_phi_hipt = std::min(floor_data_dz_phi_hipt, floor_mc_dz_phi_hipt);
    float floor_d0_phi_ulpt = std::min(floor_data_d0_phi_ulpt, floor_mc_d0_phi_ulpt);
    float floor_dz_phi_ulpt = std::min(floor_data_dz_phi_ulpt, floor_mc_dz_phi_ulpt);

    float floor_div_d0_pt_loeta = *std::min_element(reso_div_d0_pt_loeta, reso_div_d0_pt_loeta+nbins);
    float floor_div_dz_pt_loeta = *std::min_element(reso_div_dz_pt_loeta, reso_div_dz_pt_loeta+nbins);
    float floor_div_d0_pt_hieta = *std::min_element(reso_div_d0_pt_hieta, reso_div_d0_pt_hieta+nbins);
    float floor_div_dz_pt_hieta = *std::min_element(reso_div_dz_pt_hieta, reso_div_dz_pt_hieta+nbins);
    float floor_div_d0_pt_eta = *std::min_element(reso_div_d0_pt_eta, reso_div_d0_pt_eta+nbins);
    float floor_div_dz_pt_eta = *std::min_element(reso_div_dz_pt_eta, reso_div_dz_pt_eta+nbins);
    float floor_div_d0_eta_lopt = *std::min_element(reso_div_d0_eta_lopt, reso_div_d0_eta_lopt+nbins);
    float floor_div_dz_eta_lopt = *std::min_element(reso_div_dz_eta_lopt, reso_div_dz_eta_lopt+nbins);
    float floor_div_d0_eta_hipt = *std::min_element(reso_div_d0_eta_hipt, reso_div_d0_eta_hipt+nbins);
    float floor_div_dz_eta_hipt = *std::min_element(reso_div_dz_eta_hipt, reso_div_dz_eta_hipt+nbins);
    float floor_div_d0_eta_ulpt = *std::min_element(reso_div_d0_eta_ulpt, reso_div_d0_eta_ulpt+nbins);
    float floor_div_dz_eta_ulpt = *std::min_element(reso_div_dz_eta_ulpt, reso_div_dz_eta_ulpt+nbins);
    float floor_div_d0_phi_lopt = *std::min_element(reso_div_d0_phi_lopt, reso_div_d0_phi_lopt+nbins);
    float floor_div_dz_phi_lopt = *std::min_element(reso_div_dz_phi_lopt, reso_div_dz_phi_lopt+nbins);
    float floor_div_d0_phi_hipt = *std::min_element(reso_div_d0_phi_hipt, reso_div_d0_phi_hipt+nbins);
    float floor_div_dz_phi_hipt = *std::min_element(reso_div_dz_phi_hipt, reso_div_dz_phi_hipt+nbins);
    float floor_div_d0_phi_ulpt = *std::min_element(reso_div_d0_phi_ulpt, reso_div_d0_phi_ulpt+nbins);
    float floor_div_dz_phi_ulpt = *std::min_element(reso_div_dz_phi_ulpt, reso_div_dz_phi_ulpt+nbins);

    compare_gr(gr_data_d0_pt_hieta, gr_mc_d0_pt_hieta, gr_div_d0_pt_hieta, height_d0_pt_hieta, floor_d0_pt_hieta, pt[0], pt[nbins-1], 0.8*floor_div_d0_pt_hieta, 1.2*height_div_d0_pt_hieta, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_hieta");
    compare_gr(gr_data_d0_pt_loeta, gr_mc_d0_pt_loeta, gr_div_d0_pt_loeta, height_d0_pt_loeta, floor_d0_pt_loeta, pt[0], pt[nbins-1], 0.8*floor_div_d0_pt_loeta, 1.2*height_div_d0_pt_loeta, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{|#it{#eta}|<1.4}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_loeta");
    compare_gr(gr_data_d0_pt_eta, gr_mc_d0_pt_eta, gr_div_d0_pt_eta, height_d0_pt_eta, floor_d0_pt_eta, pt[0], pt[nbins-1], 0.8*floor_div_d0_pt_eta, 1.2*height_div_d0_pt_eta, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{1.4<|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_eta");

    compare_gr(gr_data_d0_eta_lopt, gr_mc_d0_eta_lopt, gr_div_d0_eta_lopt, height_d0_eta_lopt, floor_d0_eta_lopt, eta[0], eta[nbins-1], 0.8*floor_div_d0_eta_lopt, 1.2*height_div_d0_eta_lopt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_lopt");
    compare_gr(gr_data_d0_eta_hipt, gr_mc_d0_eta_hipt, gr_div_d0_eta_hipt, height_d0_eta_hipt, floor_d0_eta_hipt, eta[0], eta[nbins-1], 0.8*floor_div_d0_eta_hipt, 1.2*height_div_d0_eta_hipt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_hipt");
    compare_gr(gr_data_d0_eta_ulpt, gr_mc_d0_eta_ulpt, gr_div_d0_eta_ulpt, height_d0_eta_ulpt, floor_d0_eta_ulpt, eta[0], eta[nbins-1], 0.8*floor_div_d0_eta_ulpt, 1.2*height_div_d0_eta_ulpt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{3.0<#it{p_{T}}<10.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_ulpt");

    compare_gr(gr_data_d0_phi_lopt, gr_mc_d0_phi_lopt, gr_div_d0_phi_lopt, height_d0_phi_lopt, floor_d0_phi_lopt, phi[0], phi[nbins-1], 0.8*floor_div_d0_phi_lopt, 1.2*height_div_d0_phi_lopt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_lopt");
    compare_gr(gr_data_d0_phi_hipt, gr_mc_d0_phi_hipt, gr_div_d0_phi_hipt, height_d0_phi_hipt, floor_d0_phi_hipt, phi[0], phi[nbins-1], 0.8*floor_div_d0_phi_hipt, 1.2*height_div_d0_phi_hipt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_hipt");
    compare_gr(gr_data_d0_phi_ulpt, gr_mc_d0_phi_ulpt, gr_div_d0_phi_ulpt, height_d0_phi_ulpt, floor_d0_phi_ulpt, phi[0], phi[nbins-1], 0.8*floor_div_d0_phi_ulpt, 1.2*height_div_d0_phi_ulpt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{3.0<#it{p_{T}}<10.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_ulpt");


    compare_gr(gr_data_dz_pt_hieta, gr_mc_dz_pt_hieta, gr_div_dz_pt_hieta, height_dz_pt_hieta, floor_dz_pt_hieta, pt[0], pt[nbins-1], 0.8*floor_div_dz_pt_hieta, 1.2*height_div_dz_pt_hieta, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_hieta");
    compare_gr(gr_data_dz_pt_loeta, gr_mc_dz_pt_loeta, gr_div_dz_pt_loeta, height_dz_pt_loeta, floor_dz_pt_loeta, pt[0], pt[nbins-1], 0.8*floor_div_dz_pt_loeta, 1.2*height_div_dz_pt_loeta, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{|#it{#eta}|<1.4}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_loeta");
    compare_gr(gr_data_dz_pt_eta, gr_mc_dz_pt_eta, gr_div_dz_pt_eta, height_dz_pt_eta, floor_dz_pt_eta, pt[0], pt[nbins-1], 0.8*floor_div_dz_pt_eta, 1.2*height_div_dz_pt_eta, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{1.4<|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_eta");

    compare_gr(gr_data_dz_eta_lopt, gr_mc_dz_eta_lopt, gr_div_dz_eta_lopt, height_dz_eta_lopt, floor_dz_eta_lopt, eta[0], eta[nbins-1], 0.8*floor_div_dz_eta_lopt, 1.2*height_div_dz_eta_lopt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_lopt");
    compare_gr(gr_data_dz_eta_hipt, gr_mc_dz_eta_hipt, gr_div_dz_eta_hipt, height_dz_eta_hipt, floor_dz_eta_hipt, eta[0], eta[nbins-1], 0.8*floor_div_dz_eta_hipt, 1.2*height_div_dz_eta_hipt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_hipt");
    compare_gr(gr_data_dz_eta_ulpt, gr_mc_dz_eta_ulpt, gr_div_dz_eta_ulpt, height_dz_eta_ulpt, floor_dz_eta_ulpt, eta[0], eta[nbins-1], 0.8*floor_div_dz_eta_ulpt, 1.2*height_div_dz_eta_ulpt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{3.0<#it{p_{T}}<10.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_ulpt");

    compare_gr(gr_data_dz_phi_lopt, gr_mc_dz_phi_lopt, gr_div_dz_phi_lopt, height_dz_phi_lopt, floor_dz_phi_lopt, phi[0], phi[nbins-1], 0.8*floor_div_dz_phi_lopt, 1.2*height_div_dz_phi_lopt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_lopt");
    compare_gr(gr_data_dz_phi_hipt, gr_mc_dz_phi_hipt, gr_div_dz_phi_hipt, height_dz_phi_hipt, floor_dz_phi_hipt, phi[0], phi[nbins-1], 0.8*floor_div_dz_phi_hipt, 1.2*height_div_dz_phi_hipt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_hipt");
    compare_gr(gr_data_dz_phi_ulpt, gr_mc_dz_phi_ulpt, gr_div_dz_phi_ulpt, height_dz_phi_ulpt, floor_dz_phi_ulpt, phi[0], phi[nbins-1], 0.8*floor_div_dz_phi_ulpt, 1.2*height_div_dz_phi_ulpt, "Data", "Simulation", era, "#splitline{"+datatype_text+"}{3.0<#it{p_{T}}<10.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_ulpt");

    return 0;
}
