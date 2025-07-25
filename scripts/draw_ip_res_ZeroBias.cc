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

#include "CMSplots/tdrStyle.c"
#include "CMSplots/CMS_lumi.c"
#include "CMSplots/draw_funcs.c"
/* #include "input_list.cc" */

const TString figdir = "./figures/ZeroBias/ip_res_analysis/";
const TString jsondir = "./json/ZeroBias/ip_res/";
const int nbins = 100;

int draw_ip_res_ZeroBias(){

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, 2022BCDEFG";

    float eta[nbins];
    float phi[nbins];
    float pt[nbins];
    float reso_d0_bs_eta_hipt[nbins];
    float reso_d0_bs_eta_lopt[nbins];
    float reso_d0_bs_phi_hipt[nbins];
    float reso_d0_bs_phi_lopt[nbins];
    float reso_d0_bs_pt_hieta[nbins];
    float reso_d0_bs_pt_loeta[nbins];
    float reso_d0_pv_eta_hipt[nbins];
    float reso_d0_pv_eta_lopt[nbins];
    float reso_d0_pv_phi_hipt[nbins];
    float reso_d0_pv_phi_lopt[nbins];
    float reso_d0_pv_pt_hieta[nbins];
    float reso_d0_pv_pt_loeta[nbins];
    float reso_dz_bs_eta_hipt[nbins];
    float reso_dz_bs_eta_lopt[nbins];
    float reso_dz_bs_phi_hipt[nbins];
    float reso_dz_bs_phi_lopt[nbins];
    float reso_dz_bs_pt_hieta[nbins];
    float reso_dz_bs_pt_loeta[nbins];
    float reso_dz_pv_eta_hipt[nbins];
    float reso_dz_pv_eta_lopt[nbins];
    float reso_dz_pv_phi_hipt[nbins];
    float reso_dz_pv_phi_lopt[nbins];
    float reso_dz_pv_pt_hieta[nbins];
    float reso_dz_pv_pt_loeta[nbins];
    float resoerr_d0_bs_eta_hipt[nbins];
    float resoerr_d0_bs_eta_lopt[nbins];
    float resoerr_d0_bs_phi_hipt[nbins];
    float resoerr_d0_bs_phi_lopt[nbins];
    float resoerr_d0_bs_pt_hieta[nbins];
    float resoerr_d0_bs_pt_loeta[nbins];
    float resoerr_d0_pv_eta_hipt[nbins];
    float resoerr_d0_pv_eta_lopt[nbins];
    float resoerr_d0_pv_phi_hipt[nbins];
    float resoerr_d0_pv_phi_lopt[nbins];
    float resoerr_d0_pv_pt_hieta[nbins];
    float resoerr_d0_pv_pt_loeta[nbins];
    float resoerr_dz_bs_eta_hipt[nbins];
    float resoerr_dz_bs_eta_lopt[nbins];
    float resoerr_dz_bs_phi_hipt[nbins];
    float resoerr_dz_bs_phi_lopt[nbins];
    float resoerr_dz_bs_pt_hieta[nbins];
    float resoerr_dz_bs_pt_loeta[nbins];
    float resoerr_dz_pv_eta_hipt[nbins];
    float resoerr_dz_pv_eta_lopt[nbins];
    float resoerr_dz_pv_phi_hipt[nbins];
    float resoerr_dz_pv_phi_lopt[nbins];
    float resoerr_dz_pv_pt_hieta[nbins];
    float resoerr_dz_pv_pt_loeta[nbins];

    for(int i=0; i<nbins; i++){
        nlohmann::json results;
        std::ifstream infile(jsondir + Form("fit_%d.json",i));
        infile >> results;

        eta[i] = results["eta"];
        phi[i] = results["phi"];
        pt[i] = results["pt"];
        reso_d0_bs_eta_hipt[i] = results["reso_d0_bs_eta_hipt"];
        reso_d0_bs_eta_lopt[i] = results["reso_d0_bs_eta_lopt"];
        reso_d0_bs_phi_hipt[i] = results["reso_d0_bs_phi_hipt"];
        reso_d0_bs_phi_lopt[i] = results["reso_d0_bs_phi_lopt"];
        reso_d0_bs_pt_hieta[i] = results["reso_d0_bs_pt_hieta"];
        reso_d0_bs_pt_loeta[i] = results["reso_d0_bs_pt_loeta"];
        reso_d0_pv_eta_hipt[i] = results["reso_d0_pv_eta_hipt"];
        reso_d0_pv_eta_lopt[i] = results["reso_d0_pv_eta_lopt"];
        reso_d0_pv_phi_hipt[i] = results["reso_d0_pv_phi_hipt"];
        reso_d0_pv_phi_lopt[i] = results["reso_d0_pv_phi_lopt"];
        reso_d0_pv_pt_hieta[i] = results["reso_d0_pv_pt_hieta"];
        reso_d0_pv_pt_loeta[i] = results["reso_d0_pv_pt_loeta"];
        reso_dz_bs_eta_hipt[i] = results["reso_dz_bs_eta_hipt"];
        reso_dz_bs_eta_lopt[i] = results["reso_dz_bs_eta_lopt"];
        reso_dz_bs_phi_hipt[i] = results["reso_dz_bs_phi_hipt"];
        reso_dz_bs_phi_lopt[i] = results["reso_dz_bs_phi_lopt"];
        reso_dz_bs_pt_hieta[i] = results["reso_dz_bs_pt_hieta"];
        reso_dz_bs_pt_loeta[i] = results["reso_dz_bs_pt_loeta"];
        reso_dz_pv_eta_hipt[i] = results["reso_dz_pv_eta_hipt"];
        reso_dz_pv_eta_lopt[i] = results["reso_dz_pv_eta_lopt"];
        reso_dz_pv_phi_hipt[i] = results["reso_dz_pv_phi_hipt"];
        reso_dz_pv_phi_lopt[i] = results["reso_dz_pv_phi_lopt"];
        reso_dz_pv_pt_hieta[i] = results["reso_dz_pv_pt_hieta"];
        reso_dz_pv_pt_loeta[i] = results["reso_dz_pv_pt_loeta"];
        resoerr_d0_bs_eta_hipt[i] = results["resoerr_d0_bs_eta_hipt"];
        resoerr_d0_bs_eta_lopt[i] = results["resoerr_d0_bs_eta_lopt"];
        resoerr_d0_bs_phi_hipt[i] = results["resoerr_d0_bs_phi_hipt"];
        resoerr_d0_bs_phi_lopt[i] = results["resoerr_d0_bs_phi_lopt"];
        resoerr_d0_bs_pt_hieta[i] = results["resoerr_d0_bs_pt_hieta"];
        resoerr_d0_bs_pt_loeta[i] = results["resoerr_d0_bs_pt_loeta"];
        resoerr_d0_pv_eta_hipt[i] = results["resoerr_d0_pv_eta_hipt"];
        resoerr_d0_pv_eta_lopt[i] = results["resoerr_d0_pv_eta_lopt"];
        resoerr_d0_pv_phi_hipt[i] = results["resoerr_d0_pv_phi_hipt"];
        resoerr_d0_pv_phi_lopt[i] = results["resoerr_d0_pv_phi_lopt"];
        resoerr_d0_pv_pt_hieta[i] = results["resoerr_d0_pv_pt_hieta"];
        resoerr_d0_pv_pt_loeta[i] = results["resoerr_d0_pv_pt_loeta"];
        resoerr_dz_bs_eta_hipt[i] = results["resoerr_dz_bs_eta_hipt"];
        resoerr_dz_bs_eta_lopt[i] = results["resoerr_dz_bs_eta_lopt"];
        resoerr_dz_bs_phi_hipt[i] = results["resoerr_dz_bs_phi_hipt"];
        resoerr_dz_bs_phi_lopt[i] = results["resoerr_dz_bs_phi_lopt"];
        resoerr_dz_bs_pt_hieta[i] = results["resoerr_dz_bs_pt_hieta"];
        resoerr_dz_bs_pt_loeta[i] = results["resoerr_dz_bs_pt_loeta"];
        resoerr_dz_pv_eta_hipt[i] = results["resoerr_dz_pv_eta_hipt"];
        resoerr_dz_pv_eta_lopt[i] = results["resoerr_dz_pv_eta_lopt"];
        resoerr_dz_pv_phi_hipt[i] = results["resoerr_dz_pv_phi_hipt"];
        resoerr_dz_pv_phi_lopt[i] = results["resoerr_dz_pv_phi_lopt"];
        resoerr_dz_pv_pt_hieta[i] = results["resoerr_dz_pv_pt_hieta"];
        resoerr_dz_pv_pt_loeta[i] = results["resoerr_dz_pv_pt_loeta"];
    }

    float arr_zeros[nbins] = {0};

    TGraph * gr_d0_pv_pt_hieta = new TGraph(nbins, pt, reso_d0_pv_pt_hieta);
    TGraph * gr_d0_pv_pt_loeta = new TGraph(nbins, pt, reso_d0_pv_pt_loeta);
    TGraph * gr_d0_bs_pt_hieta = new TGraph(nbins, pt, reso_d0_bs_pt_hieta);
    TGraph * gr_d0_bs_pt_loeta = new TGraph(nbins, pt, reso_d0_bs_pt_loeta);
    TGraph * gr_dz_pv_pt_hieta = new TGraph(nbins, pt, reso_dz_pv_pt_hieta);
    TGraph * gr_dz_pv_pt_loeta = new TGraph(nbins, pt, reso_dz_pv_pt_loeta);
    TGraph * gr_dz_bs_pt_hieta = new TGraph(nbins, pt, reso_dz_bs_pt_hieta);
    TGraph * gr_dz_bs_pt_loeta = new TGraph(nbins, pt, reso_dz_bs_pt_loeta);
    
    TGraph * gr_d0_pv_eta_hipt = new TGraph(nbins, eta, reso_d0_pv_eta_hipt);
    TGraph * gr_d0_pv_eta_lopt = new TGraph(nbins, eta, reso_d0_pv_eta_lopt);
    TGraph * gr_d0_bs_eta_hipt = new TGraph(nbins, eta, reso_d0_bs_eta_hipt);
    TGraph * gr_d0_bs_eta_lopt = new TGraph(nbins, eta, reso_d0_bs_eta_lopt);
    TGraph * gr_dz_pv_eta_hipt = new TGraph(nbins, eta, reso_dz_pv_eta_hipt);
    TGraph * gr_dz_pv_eta_lopt = new TGraph(nbins, eta, reso_dz_pv_eta_lopt);
    TGraph * gr_dz_bs_eta_hipt = new TGraph(nbins, eta, reso_dz_bs_eta_hipt);
    TGraph * gr_dz_bs_eta_lopt = new TGraph(nbins, eta, reso_dz_bs_eta_lopt);

    TGraph * gr_d0_pv_phi_hipt = new TGraph(nbins, phi, reso_d0_pv_phi_hipt);
    TGraph * gr_d0_pv_phi_lopt = new TGraph(nbins, phi, reso_d0_pv_phi_lopt);
    TGraph * gr_d0_bs_phi_hipt = new TGraph(nbins, phi, reso_d0_bs_phi_hipt);
    TGraph * gr_d0_bs_phi_lopt = new TGraph(nbins, phi, reso_d0_bs_phi_lopt);
    TGraph * gr_dz_pv_phi_hipt = new TGraph(nbins, phi, reso_dz_pv_phi_hipt);
    TGraph * gr_dz_pv_phi_lopt = new TGraph(nbins, phi, reso_dz_pv_phi_lopt);
    TGraph * gr_dz_bs_phi_hipt = new TGraph(nbins, phi, reso_dz_bs_phi_hipt);
    TGraph * gr_dz_bs_phi_lopt = new TGraph(nbins, phi, reso_dz_bs_phi_lopt);
 
    /* TGraphErrors * gr_d0_pv_pt_hieta = new TGraphErrors(nbins, pt, reso_d0_pv_pt_hieta, arr_zeros, resoerr_d0_pv_pt_hieta); */
    /* TGraphErrors * gr_d0_pv_pt_loeta = new TGraphErrors(nbins, pt, reso_d0_pv_pt_loeta, arr_zeros, resoerr_d0_pv_pt_loeta); */
    /* TGraphErrors * gr_d0_bs_pt_hieta = new TGraphErrors(nbins, pt, reso_d0_bs_pt_hieta, arr_zeros, resoerr_d0_bs_pt_hieta); */
    /* TGraphErrors * gr_d0_bs_pt_loeta = new TGraphErrors(nbins, pt, reso_d0_bs_pt_loeta, arr_zeros, resoerr_d0_bs_pt_loeta); */
    /* TGraphErrors * gr_dz_pv_pt_hieta = new TGraphErrors(nbins, pt, reso_dz_pv_pt_hieta, arr_zeros, resoerr_dz_pv_pt_hieta); */
    /* TGraphErrors * gr_dz_pv_pt_loeta = new TGraphErrors(nbins, pt, reso_dz_pv_pt_loeta, arr_zeros, resoerr_dz_pv_pt_loeta); */
    /* TGraphErrors * gr_dz_bs_pt_hieta = new TGraphErrors(nbins, pt, reso_dz_bs_pt_hieta, arr_zeros, resoerr_dz_bs_pt_hieta); */
    /* TGraphErrors * gr_dz_bs_pt_loeta = new TGraphErrors(nbins, pt, reso_dz_bs_pt_loeta, arr_zeros, resoerr_dz_bs_pt_loeta); */
    
    /* TGraphErrors * gr_d0_pv_eta_hipt = new TGraphErrors(nbins, eta, reso_d0_pv_eta_hipt, arr_zeros, resoerr_d0_pv_eta_hipt); */
    /* TGraphErrors * gr_d0_pv_eta_lopt = new TGraphErrors(nbins, eta, reso_d0_pv_eta_lopt, arr_zeros, resoerr_d0_pv_eta_lopt); */
    /* TGraphErrors * gr_d0_bs_eta_hipt = new TGraphErrors(nbins, eta, reso_d0_bs_eta_hipt, arr_zeros, resoerr_d0_bs_eta_hipt); */
    /* TGraphErrors * gr_d0_bs_eta_lopt = new TGraphErrors(nbins, eta, reso_d0_bs_eta_lopt, arr_zeros, resoerr_d0_bs_eta_lopt); */
    /* TGraphErrors * gr_dz_pv_eta_hipt = new TGraphErrors(nbins, eta, reso_dz_pv_eta_hipt, arr_zeros, resoerr_dz_pv_eta_hipt); */
    /* TGraphErrors * gr_dz_pv_eta_lopt = new TGraphErrors(nbins, eta, reso_dz_pv_eta_lopt, arr_zeros, resoerr_dz_pv_eta_lopt); */
    /* TGraphErrors * gr_dz_bs_eta_hipt = new TGraphErrors(nbins, eta, reso_dz_bs_eta_hipt, arr_zeros, resoerr_dz_bs_eta_hipt); */
    /* TGraphErrors * gr_dz_bs_eta_lopt = new TGraphErrors(nbins, eta, reso_dz_bs_eta_lopt, arr_zeros, resoerr_dz_bs_eta_lopt); */

    /* TGraphErrors * gr_d0_pv_phi_hipt = new TGraphErrors(nbins, phi, reso_d0_pv_phi_hipt, arr_zeros, resoerr_d0_pv_phi_hipt); */
    /* TGraphErrors * gr_d0_pv_phi_lopt = new TGraphErrors(nbins, phi, reso_d0_pv_phi_lopt, arr_zeros, resoerr_d0_pv_phi_lopt); */
    /* TGraphErrors * gr_d0_bs_phi_hipt = new TGraphErrors(nbins, phi, reso_d0_bs_phi_hipt, arr_zeros, resoerr_d0_bs_phi_hipt); */
    /* TGraphErrors * gr_d0_bs_phi_lopt = new TGraphErrors(nbins, phi, reso_d0_bs_phi_lopt, arr_zeros, resoerr_d0_bs_phi_lopt); */
    /* TGraphErrors * gr_dz_pv_phi_hipt = new TGraphErrors(nbins, phi, reso_dz_pv_phi_hipt, arr_zeros, resoerr_dz_pv_phi_hipt); */
    /* TGraphErrors * gr_dz_pv_phi_lopt = new TGraphErrors(nbins, phi, reso_dz_pv_phi_lopt, arr_zeros, resoerr_dz_pv_phi_lopt); */
    /* TGraphErrors * gr_dz_bs_phi_hipt = new TGraphErrors(nbins, phi, reso_dz_bs_phi_hipt, arr_zeros, resoerr_dz_bs_phi_hipt); */
    /* TGraphErrors * gr_dz_bs_phi_lopt = new TGraphErrors(nbins, phi, reso_dz_bs_phi_lopt, arr_zeros, resoerr_dz_bs_phi_lopt); */
 
    float height_d0_bs_eta_hipt = *std::max_element(reso_d0_bs_eta_hipt, reso_d0_bs_eta_hipt+nbins);
    float height_d0_bs_eta_lopt = *std::max_element(reso_d0_bs_eta_lopt, reso_d0_bs_eta_lopt+nbins);
    float height_d0_bs_phi_hipt = *std::max_element(reso_d0_bs_phi_hipt, reso_d0_bs_phi_hipt+nbins);
    float height_d0_bs_phi_lopt = *std::max_element(reso_d0_bs_phi_lopt, reso_d0_bs_phi_lopt+nbins);
    float height_d0_bs_pt_hieta = *std::max_element(reso_d0_bs_pt_hieta, reso_d0_bs_pt_hieta+nbins);
    float height_d0_bs_pt_loeta = *std::max_element(reso_d0_bs_pt_loeta, reso_d0_bs_pt_loeta+nbins);
    float height_d0_pv_eta_hipt = *std::max_element(reso_d0_pv_eta_hipt, reso_d0_pv_eta_hipt+nbins);
    float height_d0_pv_eta_lopt = *std::max_element(reso_d0_pv_eta_lopt, reso_d0_pv_eta_lopt+nbins);
    float height_d0_pv_phi_hipt = *std::max_element(reso_d0_pv_phi_hipt, reso_d0_pv_phi_hipt+nbins);
    float height_d0_pv_phi_lopt = *std::max_element(reso_d0_pv_phi_lopt, reso_d0_pv_phi_lopt+nbins);
    float height_d0_pv_pt_hieta = *std::max_element(reso_d0_pv_pt_hieta, reso_d0_pv_pt_hieta+nbins);
    float height_d0_pv_pt_loeta = *std::max_element(reso_d0_pv_pt_loeta, reso_d0_pv_pt_loeta+nbins);
    float height_dz_bs_eta_hipt = *std::max_element(reso_dz_bs_eta_hipt, reso_dz_bs_eta_hipt+nbins);
    float height_dz_bs_eta_lopt = *std::max_element(reso_dz_bs_eta_lopt, reso_dz_bs_eta_lopt+nbins);
    float height_dz_bs_phi_hipt = *std::max_element(reso_dz_bs_phi_hipt, reso_dz_bs_phi_hipt+nbins);
    float height_dz_bs_phi_lopt = *std::max_element(reso_dz_bs_phi_lopt, reso_dz_bs_phi_lopt+nbins);
    float height_dz_bs_pt_hieta = *std::max_element(reso_dz_bs_pt_hieta, reso_dz_bs_pt_hieta+nbins);
    float height_dz_bs_pt_loeta = *std::max_element(reso_dz_bs_pt_loeta, reso_dz_bs_pt_loeta+nbins);
    float height_dz_pv_eta_hipt = *std::max_element(reso_dz_pv_eta_hipt, reso_dz_pv_eta_hipt+nbins);
    float height_dz_pv_eta_lopt = *std::max_element(reso_dz_pv_eta_lopt, reso_dz_pv_eta_lopt+nbins);
    float height_dz_pv_phi_hipt = *std::max_element(reso_dz_pv_phi_hipt, reso_dz_pv_phi_hipt+nbins);
    float height_dz_pv_phi_lopt = *std::max_element(reso_dz_pv_phi_lopt, reso_dz_pv_phi_lopt+nbins);
    float height_dz_pv_pt_hieta = *std::max_element(reso_dz_pv_pt_hieta, reso_dz_pv_pt_hieta+nbins);
    float height_dz_pv_pt_loeta = *std::max_element(reso_dz_pv_pt_loeta, reso_dz_pv_pt_loeta+nbins);

    float floor_d0_bs_eta_hipt = *std::min_element(reso_d0_bs_eta_hipt, reso_d0_bs_eta_hipt+nbins);
    float floor_d0_bs_eta_lopt = *std::min_element(reso_d0_bs_eta_lopt, reso_d0_bs_eta_lopt+nbins);
    float floor_d0_bs_phi_hipt = *std::min_element(reso_d0_bs_phi_hipt, reso_d0_bs_phi_hipt+nbins);
    float floor_d0_bs_phi_lopt = *std::min_element(reso_d0_bs_phi_lopt, reso_d0_bs_phi_lopt+nbins);
    float floor_d0_bs_pt_hieta = *std::min_element(reso_d0_bs_pt_hieta, reso_d0_bs_pt_hieta+nbins);
    float floor_d0_bs_pt_loeta = *std::min_element(reso_d0_bs_pt_loeta, reso_d0_bs_pt_loeta+nbins);
    float floor_d0_pv_eta_hipt = *std::min_element(reso_d0_pv_eta_hipt, reso_d0_pv_eta_hipt+nbins);
    float floor_d0_pv_eta_lopt = *std::min_element(reso_d0_pv_eta_lopt, reso_d0_pv_eta_lopt+nbins);
    float floor_d0_pv_phi_hipt = *std::min_element(reso_d0_pv_phi_hipt, reso_d0_pv_phi_hipt+nbins);
    float floor_d0_pv_phi_lopt = *std::min_element(reso_d0_pv_phi_lopt, reso_d0_pv_phi_lopt+nbins);
    float floor_d0_pv_pt_hieta = *std::min_element(reso_d0_pv_pt_hieta, reso_d0_pv_pt_hieta+nbins);
    float floor_d0_pv_pt_loeta = *std::min_element(reso_d0_pv_pt_loeta, reso_d0_pv_pt_loeta+nbins);
    float floor_dz_bs_eta_hipt = *std::min_element(reso_dz_bs_eta_hipt, reso_dz_bs_eta_hipt+nbins);
    float floor_dz_bs_eta_lopt = *std::min_element(reso_dz_bs_eta_lopt, reso_dz_bs_eta_lopt+nbins);
    float floor_dz_bs_phi_hipt = *std::min_element(reso_dz_bs_phi_hipt, reso_dz_bs_phi_hipt+nbins);
    float floor_dz_bs_phi_lopt = *std::min_element(reso_dz_bs_phi_lopt, reso_dz_bs_phi_lopt+nbins);
    float floor_dz_bs_pt_hieta = *std::min_element(reso_dz_bs_pt_hieta, reso_dz_bs_pt_hieta+nbins);
    float floor_dz_bs_pt_loeta = *std::min_element(reso_dz_bs_pt_loeta, reso_dz_bs_pt_loeta+nbins);
    float floor_dz_pv_eta_hipt = *std::min_element(reso_dz_pv_eta_hipt, reso_dz_pv_eta_hipt+nbins);
    float floor_dz_pv_eta_lopt = *std::min_element(reso_dz_pv_eta_lopt, reso_dz_pv_eta_lopt+nbins);
    float floor_dz_pv_phi_hipt = *std::min_element(reso_dz_pv_phi_hipt, reso_dz_pv_phi_hipt+nbins);
    float floor_dz_pv_phi_lopt = *std::min_element(reso_dz_pv_phi_lopt, reso_dz_pv_phi_lopt+nbins);
    float floor_dz_pv_pt_hieta = *std::min_element(reso_dz_pv_pt_hieta, reso_dz_pv_pt_hieta+nbins);
    float floor_dz_pv_pt_loeta = *std::min_element(reso_dz_pv_pt_loeta, reso_dz_pv_pt_loeta+nbins);

    TCanvas *c_d0_pt = new TCanvas("c_d0_pt", "", 800, 600);
    canvas_setup(c_d0_pt);
    gr_d0_pv_pt_hieta->SetMarkerStyle(4);
    gr_d0_pv_pt_hieta->SetMarkerColor(kBlack);
    gr_d0_pv_pt_loeta->SetMarkerStyle(4);
    gr_d0_pv_pt_loeta->SetMarkerColor(kBlue);
    gr_d0_bs_pt_hieta->SetMarkerStyle(26);
    gr_d0_bs_pt_hieta->SetMarkerColor(kBlack);
    gr_d0_bs_pt_loeta->SetMarkerStyle(26);
    gr_d0_bs_pt_loeta->SetMarkerColor(kBlue);
    gr_d0_pv_pt_hieta->Draw("AP");
    gr_d0_pv_pt_loeta->Draw("P SAME");
    gr_d0_bs_pt_hieta->Draw("P SAME");
    gr_d0_bs_pt_loeta->Draw("P SAME");
    gr_d0_pv_pt_hieta->GetXaxis()->SetTitle("Track #it{p_{T}} [GeV]");
    gr_d0_pv_pt_hieta->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]");
    TLegend *leg_d0_pt = new TLegend(0.55, 0.7, 0.75, 0.85);
    leg_d0_pt->AddEntry(gr_d0_pv_pt_hieta, "IPPV |#it{#eta}|<3.0", "p");
    leg_d0_pt->AddEntry(gr_d0_pv_pt_loeta, "IPPV |#it{#eta}|<1.4", "p");
    leg_d0_pt->AddEntry(gr_d0_bs_pt_hieta, "IPBS |#it{#eta}|<3.0", "p");
    leg_d0_pt->AddEntry(gr_d0_bs_pt_loeta, "IPBS |#it{#eta}|<1.4", "p");
    leg_d0_pt->SetTextFont(42);
    leg_d0_pt->SetTextColor(kBlack);
    leg_d0_pt->SetBorderSize(0);
    leg_d0_pt->SetFillColor(0);
    leg_d0_pt->SetLineWidth(0);
    leg_d0_pt->SetTextSize(0.04);
    leg_d0_pt->Draw();
    write_text(0.55, 0.88, "Unbiased collision events");
    CMS_lumi(c_d0_pt);

    float height_d0_pt = std::max(height_d0_pv_pt_hieta, height_d0_pv_pt_loeta);
    height_d0_pt = std::max(height_d0_pt, height_d0_bs_pt_hieta);
    height_d0_pt = std::max(height_d0_pt, height_d0_bs_pt_loeta);
    float floor_d0_pt = std::min(floor_d0_pv_pt_hieta, floor_d0_pv_pt_loeta);
    floor_d0_pt = std::min(floor_d0_pt, floor_d0_bs_pt_hieta);
    floor_d0_pt = std::min(floor_d0_pt, floor_d0_bs_pt_loeta);

    gr_d0_pv_pt_hieta->SetMaximum(height_d0_pt*1.5);
    gr_d0_pv_pt_hieta->SetMinimum(0);
    c_d0_pt->Update();
    c_d0_pt->RedrawAxis();
    c_d0_pt->SaveAs(figdir + "d0_pt.png");

    /* gr_d0_pv_pt_hieta->SetMaximum(height_d0_pt*100); */
    /* gr_d0_pv_pt_hieta->SetMinimum(floor_d0_pt/100); */
    /* c_d0_pt->SetLogy(); */
    /* c_d0_pt->Update(); */
    /* c_d0_pt->RedrawAxis(); */
    /* c_d0_pt->SaveAs(figdir + "d0_pt_log.png"); */

    TCanvas *c_dz_pt = new TCanvas("c_dz_pt", "", 800, 600);
    canvas_setup(c_dz_pt);
    gr_dz_pv_pt_hieta->SetMarkerStyle(4);
    gr_dz_pv_pt_hieta->SetMarkerColor(kBlack);
    gr_dz_pv_pt_loeta->SetMarkerStyle(4);
    gr_dz_pv_pt_loeta->SetMarkerColor(kBlue);
    gr_dz_bs_pt_hieta->SetMarkerStyle(26);
    gr_dz_bs_pt_hieta->SetMarkerColor(kBlack);
    gr_dz_bs_pt_loeta->SetMarkerStyle(26);
    gr_dz_bs_pt_loeta->SetMarkerColor(kBlue);
    gr_dz_pv_pt_hieta->Draw("AP");
    gr_dz_pv_pt_loeta->Draw("P SAME");
    gr_dz_pv_pt_hieta->GetXaxis()->SetTitle("Track #it{p_{T}} [GeV]");
    gr_dz_pv_pt_hieta->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [mm]");
    TLegend *leg_dz_pt = new TLegend(0.55, 0.7, 0.75, 0.85);
    leg_dz_pt->AddEntry(gr_dz_pv_pt_hieta, "IPPV |#it{#eta}|<3.0", "p");
    leg_dz_pt->AddEntry(gr_dz_pv_pt_loeta, "IPPV |#it{#eta}|<1.4", "p");
    leg_dz_pt->SetTextFont(42);
    leg_dz_pt->SetTextColor(kBlack);
    leg_dz_pt->SetBorderSize(0);
    leg_dz_pt->SetFillColor(0);
    leg_dz_pt->SetLineWidth(0);
    leg_dz_pt->SetTextSize(0.04);
    leg_dz_pt->Draw();
    write_text(0.55, 0.88, "Unbiased collision events");
    CMS_lumi(c_dz_pt);

    float height_dz_pt = std::max(height_dz_pv_pt_hieta, height_dz_pv_pt_loeta);
    float floor_dz_pt = std::min(floor_dz_pv_pt_hieta, floor_dz_pv_pt_loeta);

    gr_dz_pv_pt_hieta->SetMaximum(height_dz_pt*1.5);
    gr_dz_pv_pt_hieta->SetMinimum(0);
    c_dz_pt->Update();
    c_dz_pt->RedrawAxis();
    c_dz_pt->SaveAs(figdir + "dz_pt.png");

    /* gr_dz_pv_pt_hieta->SetMaximum(height_dz_pt*100); */
    /* gr_dz_pv_pt_hieta->SetMinimum(floor_dz_pt/100); */
    /* c_dz_pt->SetLogy(); */
    /* c_dz_pt->Update(); */
    /* c_dz_pt->RedrawAxis(); */
    /* c_dz_pt->SaveAs(figdir + "dz_pt_log.png"); */



    TCanvas *c_d0_eta = new TCanvas("c_d0_eta", "", 800, 600);
    canvas_setup(c_d0_eta);
    gr_d0_pv_eta_hipt->SetMarkerStyle(4);
    gr_d0_pv_eta_hipt->SetMarkerColor(kBlack);
    gr_d0_pv_eta_lopt->SetMarkerStyle(4);
    gr_d0_pv_eta_lopt->SetMarkerColor(kBlue);
    gr_d0_bs_eta_hipt->SetMarkerStyle(26);
    gr_d0_bs_eta_hipt->SetMarkerColor(kBlack);
    gr_d0_bs_eta_lopt->SetMarkerStyle(26);
    gr_d0_bs_eta_lopt->SetMarkerColor(kBlue);
    gr_d0_pv_eta_hipt->Draw("AP");
    gr_d0_pv_eta_lopt->Draw("P SAME");
    gr_d0_bs_eta_hipt->Draw("P SAME");
    gr_d0_bs_eta_lopt->Draw("P SAME");
    gr_d0_pv_eta_hipt->GetXaxis()->SetTitle("Track #it{#eta}");
    gr_d0_pv_eta_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]");
    TLegend *leg_d0_eta = new TLegend(0.55, 0.7, 0.75, 0.85);
    leg_d0_eta->AddEntry(gr_d0_pv_eta_hipt, "IPPV 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_d0_eta->AddEntry(gr_d0_pv_eta_lopt, "IPPV 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_eta->AddEntry(gr_d0_bs_eta_hipt, "IPBS 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_d0_eta->AddEntry(gr_d0_bs_eta_lopt, "IPBS 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_eta->SetTextFont(42);
    leg_d0_eta->SetTextColor(kBlack);
    leg_d0_eta->SetBorderSize(0);
    leg_d0_eta->SetFillColor(0);
    leg_d0_eta->SetLineWidth(0);
    leg_d0_eta->SetTextSize(0.04);
    leg_d0_eta->Draw();
    write_text(0.55, 0.88, "Unbiased collision events");
    CMS_lumi(c_d0_eta);

    float height_d0_eta = std::max(height_d0_pv_eta_hipt, height_d0_pv_eta_lopt);
    height_d0_eta = std::max(height_d0_eta, height_d0_bs_eta_hipt);
    height_d0_eta = std::max(height_d0_eta, height_d0_bs_eta_lopt);
    float floor_d0_eta = std::min(floor_d0_pv_eta_hipt, floor_d0_pv_eta_lopt);
    floor_d0_eta = std::min(floor_d0_eta, floor_d0_bs_eta_hipt);
    floor_d0_eta = std::min(floor_d0_eta, floor_d0_bs_eta_lopt);

    gr_d0_pv_eta_hipt->SetMaximum(height_d0_eta*1.5);
    gr_d0_pv_eta_hipt->SetMinimum(0);
    c_d0_eta->Update();
    c_d0_eta->RedrawAxis();
    c_d0_eta->SaveAs(figdir + "d0_eta.png");

    /* gr_d0_pv_eta_hipt->SetMaximum(height_d0_eta*100); */
    /* gr_d0_pv_eta_hipt->SetMinimum(floor_d0_eta/100); */
    /* c_d0_eta->SetLogy(); */
    /* c_d0_eta->Update(); */
    /* c_d0_eta->RedrawAxis(); */
    /* c_d0_eta->SaveAs(figdir + "d0_eta_log.png"); */


    TCanvas *c_dz_eta = new TCanvas("c_dz_eta", "", 800, 600);
    canvas_setup(c_dz_eta);
    gr_dz_pv_eta_hipt->SetMarkerStyle(4);
    gr_dz_pv_eta_hipt->SetMarkerColor(kBlack);
    gr_dz_pv_eta_lopt->SetMarkerStyle(4);
    gr_dz_pv_eta_lopt->SetMarkerColor(kBlue);
    gr_dz_pv_eta_hipt->Draw("AP");
    gr_dz_pv_eta_lopt->Draw("P SAME");
    gr_dz_pv_eta_hipt->GetXaxis()->SetTitle("Track #it{#eta}");
    gr_dz_pv_eta_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [#mum]");
    TLegend *leg_dz_eta = new TLegend(0.55, 0.7, 0.75, 0.85);
    leg_dz_eta->AddEntry(gr_dz_pv_eta_hipt, "IPPV 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_dz_eta->AddEntry(gr_dz_pv_eta_lopt, "IPPV 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_dz_eta->SetTextFont(42);
    leg_dz_eta->SetTextColor(kBlack);
    leg_dz_eta->SetBorderSize(0);
    leg_dz_eta->SetFillColor(0);
    leg_dz_eta->SetLineWidth(0);
    leg_dz_eta->SetTextSize(0.04);
    leg_dz_eta->Draw();
    write_text(0.55, 0.88, "Unbiased collision events");
    CMS_lumi(c_dz_eta);

    float height_dz_eta = std::max(height_dz_pv_eta_hipt, height_dz_pv_eta_lopt);
    float floor_dz_eta = std::min(floor_dz_pv_eta_hipt, floor_dz_pv_eta_lopt);

    gr_dz_pv_eta_hipt->SetMaximum(height_dz_eta*1.5);
    gr_dz_pv_eta_hipt->SetMinimum(0);
    c_dz_eta->Update();
    c_dz_eta->RedrawAxis();
    c_dz_eta->SaveAs(figdir + "dz_eta.png");

    /* gr_dz_pv_eta_hipt->SetMaximum(height_dz_eta*100); */
    /* gr_dz_pv_eta_hipt->SetMinimum(floor_dz_eta/100); */
    /* c_dz_eta->SetLogy(); */
    /* c_dz_eta->Update(); */
    /* c_dz_eta->RedrawAxis(); */
    /* c_dz_eta->SaveAs(figdir + "dz_eta_log.png"); */



    TCanvas *c_d0_phi = new TCanvas("c_d0_phi", "", 800, 600);
    canvas_setup(c_d0_phi);
    gr_d0_pv_phi_hipt->SetMarkerStyle(4);
    gr_d0_pv_phi_hipt->SetMarkerColor(kBlack);
    gr_d0_pv_phi_lopt->SetMarkerStyle(4);
    gr_d0_pv_phi_lopt->SetMarkerColor(kBlue);
    gr_d0_bs_phi_hipt->SetMarkerStyle(26);
    gr_d0_bs_phi_hipt->SetMarkerColor(kBlack);
    gr_d0_bs_phi_lopt->SetMarkerStyle(26);
    gr_d0_bs_phi_lopt->SetMarkerColor(kBlue);
    gr_d0_pv_phi_hipt->Draw("AP");
    gr_d0_pv_phi_lopt->Draw("P SAME");
    gr_d0_bs_phi_lopt->Draw("P SAME");
    gr_d0_bs_phi_lopt->Draw("P SAME");
    gr_d0_pv_phi_hipt->GetXaxis()->SetTitle("Track #it{#phi}");
    gr_d0_pv_phi_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]");
    TLegend *leg_d0_phi = new TLegend(0.55, 0.7, 0.75, 0.85);
    leg_d0_phi->AddEntry(gr_d0_pv_phi_hipt, "IPPV 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_d0_phi->AddEntry(gr_d0_pv_phi_lopt, "IPPV 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_phi->AddEntry(gr_d0_bs_phi_hipt, "IPBS 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_d0_phi->AddEntry(gr_d0_bs_phi_lopt, "IPBS 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_phi->SetTextFont(42);
    leg_d0_phi->SetTextColor(kBlack);
    leg_d0_phi->SetBorderSize(0);
    leg_d0_phi->SetFillColor(0);
    leg_d0_phi->SetLineWidth(0);
    leg_d0_phi->SetTextSize(0.04);
    leg_d0_phi->Draw();
    write_text(0.55, 0.88, "Unbiased collision events");
    CMS_lumi(c_d0_phi);

    float height_d0_phi = std::max(height_d0_pv_phi_hipt, height_d0_pv_phi_lopt);
    height_d0_phi = std::max(height_d0_phi, height_d0_bs_phi_hipt);
    height_d0_phi = std::max(height_d0_phi, height_d0_bs_phi_lopt);
    float floor_d0_phi = std::min(floor_d0_pv_phi_hipt, floor_d0_pv_phi_lopt);
    floor_d0_phi = std::min(floor_d0_phi, floor_d0_bs_phi_hipt);
    floor_d0_phi = std::min(floor_d0_phi, floor_d0_bs_phi_lopt);

    gr_d0_pv_phi_hipt->SetMaximum(height_d0_phi*1.5);
    gr_d0_pv_phi_hipt->SetMinimum(0);
    c_d0_phi->Update();
    c_d0_phi->RedrawAxis();
    c_d0_phi->SaveAs(figdir + "d0_phi.png");

    /* gr_d0_pv_phi_hipt->SetMaximum(height_d0_phi*100); */
    /* gr_d0_pv_phi_hipt->SetMinimum(floor_d0_phi/100); */
    /* c_d0_phi->SetLogy(); */
    /* c_d0_phi->Update(); */
    /* c_d0_phi->RedrawAxis(); */
    /* c_d0_phi->SaveAs(figdir + "d0_phi_log.png"); */


    TCanvas *c_dz_phi = new TCanvas("c_dz_phi", "", 800, 600);
    canvas_setup(c_dz_phi);
    gr_dz_pv_phi_hipt->SetMarkerStyle(4);
    gr_dz_pv_phi_hipt->SetMarkerColor(kBlack);
    gr_dz_pv_phi_lopt->SetMarkerStyle(4);
    gr_dz_pv_phi_lopt->SetMarkerColor(kBlue);
    gr_dz_pv_phi_hipt->Draw("AP");
    gr_dz_pv_phi_lopt->Draw("P SAME");
    gr_dz_pv_phi_hipt->GetXaxis()->SetTitle("Track #it{#phi}");
    gr_dz_pv_phi_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [#mum]");
    TLegend *leg_dz_phi = new TLegend(0.55, 0.7, 0.75, 0.85);
    leg_dz_phi->AddEntry(gr_dz_pv_phi_hipt, "IPPV 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_dz_phi->AddEntry(gr_dz_pv_phi_lopt, "IPPV 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_dz_phi->SetTextFont(42);
    leg_dz_phi->SetTextColor(kBlack);
    leg_dz_phi->SetBorderSize(0);
    leg_dz_phi->SetFillColor(0);
    leg_dz_phi->SetLineWidth(0);
    leg_dz_phi->SetTextSize(0.04);
    leg_dz_phi->Draw();
    write_text(0.55, 0.88, "Unbiased collision events");
    CMS_lumi(c_dz_phi);

    float height_dz_phi = std::max(height_dz_pv_phi_hipt, height_dz_pv_phi_lopt);
    float floor_dz_phi = std::min(floor_dz_pv_phi_hipt, floor_dz_pv_phi_lopt);

    gr_dz_pv_phi_hipt->SetMaximum(height_dz_phi*1.5);
    gr_dz_pv_phi_hipt->SetMinimum(0);
    c_dz_phi->Update();
    c_dz_phi->RedrawAxis();
    c_dz_phi->SaveAs(figdir + "dz_phi.png");

    /* gr_dz_pv_phi_hipt->SetMaximum(height_dz_phi*100); */
    /* gr_dz_pv_phi_hipt->SetMinimum(floor_dz_phi/100); */
    /* c_dz_phi->SetLogy(); */
    /* c_dz_phi->Update(); */
    /* c_dz_phi->RedrawAxis(); */
    /* c_dz_phi->SaveAs(figdir + "dz_phi_log.png"); */

    return 0;
}
