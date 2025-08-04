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

const TString figdir = "../../figures/"+datatype+"/ip_res/";
const TString jsondir = "../../json/"+datatype+"/ip_res/";
const int nbins = 100;

int draw_ip_res(){

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, 2022"+all_era;

    float eta[nbins];
    float phi[nbins];
    float pt[nbins];

    float data_reso_d0_pt_loeta[nbins];
    float data_reso_dz_pt_loeta[nbins];
    float data_reso_d0_pt_hieta[nbins];
    float data_reso_dz_pt_hieta[nbins];

    float data_reso_d0_eta_lopt[nbins];
    float data_reso_dz_eta_lopt[nbins];
    float data_reso_d0_eta_hipt[nbins];
    float data_reso_dz_eta_hipt[nbins];
    float data_reso_d0_eta_ulpt[nbins];
    float data_reso_dz_eta_ulpt[nbins];

    float data_reso_d0_phi_lopt[nbins];
    float data_reso_dz_phi_lopt[nbins];
    float data_reso_d0_phi_hipt[nbins];
    float data_reso_dz_phi_hipt[nbins];
    float data_reso_d0_phi_ulpt[nbins];
    float data_reso_dz_phi_ulpt[nbins];

    float mc_reso_d0_pt_loeta[nbins];
    float mc_reso_dz_pt_loeta[nbins];
    float mc_reso_d0_pt_hieta[nbins];
    float mc_reso_dz_pt_hieta[nbins];

    float mc_reso_d0_eta_lopt[nbins];
    float mc_reso_dz_eta_lopt[nbins];
    float mc_reso_d0_eta_hipt[nbins];
    float mc_reso_dz_eta_hipt[nbins];
    float mc_reso_d0_eta_ulpt[nbins];
    float mc_reso_dz_eta_ulpt[nbins];

    float mc_reso_d0_phi_lopt[nbins];
    float mc_reso_dz_phi_lopt[nbins];
    float mc_reso_d0_phi_hipt[nbins];
    float mc_reso_dz_phi_hipt[nbins];
    float mc_reso_d0_phi_ulpt[nbins];
    float mc_reso_dz_phi_ulpt[nbins];

    for(int i=0; i<nbins; i++){
        nlohmann::json data_results;
        std::ifstream data_infile(jsondir + Form("/data/fit_%d.json",i));
        data_infile >> data_results;

        nlohmann::json mc_results;
        std::ifstream mc_infile(jsondir + Form("/mc/fit_%d.json",i));
        mc_infile >> mc_results;

        eta[i] = data_results["eta"];
        phi[i] = data_results["phi"];
        pt[i] = data_results["pt"];

        data_reso_d0_pt_loeta[i] = data_results["reso_d0_pt_loeta"];
        data_reso_dz_pt_loeta[i] = data_results["reso_dz_pt_loeta"];
        data_reso_d0_pt_hieta[i] = data_results["reso_d0_pt_hieta"];
        data_reso_dz_pt_hieta[i] = data_results["reso_dz_pt_hieta"];

        data_reso_d0_eta_lopt[i] = data_results["reso_d0_eta_lopt"];
        data_reso_dz_eta_lopt[i] = data_results["reso_dz_eta_lopt"];
        data_reso_d0_eta_hipt[i] = data_results["reso_d0_eta_hipt"];
        data_reso_dz_eta_hipt[i] = data_results["reso_dz_eta_hipt"];
        data_reso_d0_eta_ulpt[i] = data_results["reso_d0_eta_ulpt"];
        data_reso_dz_eta_ulpt[i] = data_results["reso_dz_eta_ulpt"];

        data_reso_d0_phi_lopt[i] = data_results["reso_d0_phi_lopt"];
        data_reso_dz_phi_lopt[i] = data_results["reso_dz_phi_lopt"];
        data_reso_d0_phi_hipt[i] = data_results["reso_d0_phi_hipt"];
        data_reso_dz_phi_hipt[i] = data_results["reso_dz_phi_hipt"];
        data_reso_d0_phi_ulpt[i] = data_results["reso_d0_phi_ulpt"];
        data_reso_dz_phi_ulpt[i] = data_results["reso_dz_phi_ulpt"];

        mc_reso_d0_pt_loeta[i] = mc_results["reso_d0_pt_loeta"];
        mc_reso_dz_pt_loeta[i] = mc_results["reso_dz_pt_loeta"];
        mc_reso_d0_pt_hieta[i] = mc_results["reso_d0_pt_hieta"];
        mc_reso_dz_pt_hieta[i] = mc_results["reso_dz_pt_hieta"];

        mc_reso_d0_eta_lopt[i] = mc_results["reso_d0_eta_lopt"];
        mc_reso_dz_eta_lopt[i] = mc_results["reso_dz_eta_lopt"];
        mc_reso_d0_eta_hipt[i] = mc_results["reso_d0_eta_hipt"];
        mc_reso_dz_eta_hipt[i] = mc_results["reso_dz_eta_hipt"];
        mc_reso_d0_eta_ulpt[i] = mc_results["reso_d0_eta_ulpt"];
        mc_reso_dz_eta_ulpt[i] = mc_results["reso_dz_eta_ulpt"];

        mc_reso_d0_phi_lopt[i] = mc_results["reso_d0_phi_lopt"];
        mc_reso_dz_phi_lopt[i] = mc_results["reso_dz_phi_lopt"];
        mc_reso_d0_phi_hipt[i] = mc_results["reso_d0_phi_hipt"];
        mc_reso_dz_phi_hipt[i] = mc_results["reso_dz_phi_hipt"];
        mc_reso_d0_phi_ulpt[i] = mc_results["reso_d0_phi_ulpt"];
        mc_reso_dz_phi_ulpt[i] = mc_results["reso_dz_phi_ulpt"];
    }

    TGraph * data_gr_d0_pt_loeta = new TGraph(nbins, pt, data_reso_d0_pt_loeta);
    TGraph * data_gr_dz_pt_loeta = new TGraph(nbins, pt, data_reso_dz_pt_loeta);
    TGraph * data_gr_d0_pt_hieta = new TGraph(nbins, pt, data_reso_d0_pt_hieta);
    TGraph * data_gr_dz_pt_hieta = new TGraph(nbins, pt, data_reso_dz_pt_hieta);
    
    TGraph * data_gr_d0_eta_lopt = new TGraph(nbins, eta, data_reso_d0_eta_lopt);
    TGraph * data_gr_dz_eta_lopt = new TGraph(nbins, eta, data_reso_dz_eta_lopt);
    TGraph * data_gr_d0_eta_hipt = new TGraph(nbins, eta, data_reso_d0_eta_hipt);
    TGraph * data_gr_dz_eta_hipt = new TGraph(nbins, eta, data_reso_dz_eta_hipt);
    TGraph * data_gr_d0_eta_ulpt = new TGraph(nbins, eta, data_reso_d0_eta_ulpt);
    TGraph * data_gr_dz_eta_ulpt = new TGraph(nbins, eta, data_reso_dz_eta_ulpt);

    TGraph * data_gr_d0_phi_lopt = new TGraph(nbins, phi, data_reso_d0_phi_lopt);
    TGraph * data_gr_dz_phi_lopt = new TGraph(nbins, phi, data_reso_dz_phi_lopt);
    TGraph * data_gr_d0_phi_hipt = new TGraph(nbins, phi, data_reso_d0_phi_hipt);
    TGraph * data_gr_dz_phi_hipt = new TGraph(nbins, phi, data_reso_dz_phi_hipt);
    TGraph * data_gr_d0_phi_ulpt = new TGraph(nbins, phi, data_reso_d0_phi_ulpt);
    TGraph * data_gr_dz_phi_ulpt = new TGraph(nbins, phi, data_reso_dz_phi_ulpt);
    
    TGraph * mc_gr_d0_pt_loeta = new TGraph(nbins, pt, mc_reso_d0_pt_loeta);
    TGraph * mc_gr_dz_pt_loeta = new TGraph(nbins, pt, mc_reso_dz_pt_loeta);
    TGraph * mc_gr_d0_pt_hieta = new TGraph(nbins, pt, mc_reso_d0_pt_hieta);
    TGraph * mc_gr_dz_pt_hieta = new TGraph(nbins, pt, mc_reso_dz_pt_hieta);
    
    TGraph * mc_gr_d0_eta_lopt = new TGraph(nbins, eta, mc_reso_d0_eta_lopt);
    TGraph * mc_gr_dz_eta_lopt = new TGraph(nbins, eta, mc_reso_dz_eta_lopt);
    TGraph * mc_gr_d0_eta_hipt = new TGraph(nbins, eta, mc_reso_d0_eta_hipt);
    TGraph * mc_gr_dz_eta_hipt = new TGraph(nbins, eta, mc_reso_dz_eta_hipt);
    TGraph * mc_gr_d0_eta_ulpt = new TGraph(nbins, eta, mc_reso_d0_eta_ulpt);
    TGraph * mc_gr_dz_eta_ulpt = new TGraph(nbins, eta, mc_reso_dz_eta_ulpt);

    TGraph * mc_gr_d0_phi_lopt = new TGraph(nbins, phi, mc_reso_d0_phi_lopt);
    TGraph * mc_gr_dz_phi_lopt = new TGraph(nbins, phi, mc_reso_dz_phi_lopt);
    TGraph * mc_gr_d0_phi_hipt = new TGraph(nbins, phi, mc_reso_d0_phi_hipt);
    TGraph * mc_gr_dz_phi_hipt = new TGraph(nbins, phi, mc_reso_dz_phi_hipt);
    TGraph * mc_gr_d0_phi_ulpt = new TGraph(nbins, phi, mc_reso_d0_phi_ulpt);
    TGraph * mc_gr_dz_phi_ulpt = new TGraph(nbins, phi, mc_reso_dz_phi_ulpt);
    
    float data_height_d0_pt_loeta = *std::max_element(data_reso_d0_pt_loeta, data_reso_d0_pt_loeta+nbins);
    float data_height_dz_pt_loeta = *std::max_element(data_reso_dz_pt_loeta, data_reso_dz_pt_loeta+nbins);
    float data_height_d0_pt_hieta = *std::max_element(data_reso_d0_pt_hieta, data_reso_d0_pt_hieta+nbins);
    float data_height_dz_pt_hieta = *std::max_element(data_reso_dz_pt_hieta, data_reso_dz_pt_hieta+nbins);

    float data_height_d0_eta_lopt = *std::max_element(data_reso_d0_eta_lopt, data_reso_d0_eta_lopt+nbins);
    float data_height_dz_eta_lopt = *std::max_element(data_reso_dz_eta_lopt, data_reso_dz_eta_lopt+nbins);
    float data_height_d0_eta_hipt = *std::max_element(data_reso_d0_eta_hipt, data_reso_d0_eta_hipt+nbins);
    float data_height_dz_eta_hipt = *std::max_element(data_reso_dz_eta_hipt, data_reso_dz_eta_hipt+nbins);
    float data_height_d0_eta_ulpt = *std::max_element(data_reso_d0_eta_ulpt, data_reso_d0_eta_ulpt+nbins);
    float data_height_dz_eta_ulpt = *std::max_element(data_reso_dz_eta_ulpt, data_reso_dz_eta_ulpt+nbins);

    float data_height_d0_phi_lopt = *std::max_element(data_reso_d0_phi_lopt, data_reso_d0_phi_lopt+nbins);
    float data_height_dz_phi_lopt = *std::max_element(data_reso_dz_phi_lopt, data_reso_dz_phi_lopt+nbins);
    float data_height_d0_phi_hipt = *std::max_element(data_reso_d0_phi_hipt, data_reso_d0_phi_hipt+nbins);
    float data_height_dz_phi_hipt = *std::max_element(data_reso_dz_phi_hipt, data_reso_dz_phi_hipt+nbins);
    float data_height_d0_phi_ulpt = *std::max_element(data_reso_d0_phi_ulpt, data_reso_d0_phi_ulpt+nbins);
    float data_height_dz_phi_ulpt = *std::max_element(data_reso_dz_phi_ulpt, data_reso_dz_phi_ulpt+nbins);
    
    float mc_height_d0_pt_loeta = *std::max_element(mc_reso_d0_pt_loeta, mc_reso_d0_pt_loeta+nbins);
    float mc_height_dz_pt_loeta = *std::max_element(mc_reso_dz_pt_loeta, mc_reso_dz_pt_loeta+nbins);
    float mc_height_d0_pt_hieta = *std::max_element(mc_reso_d0_pt_hieta, mc_reso_d0_pt_hieta+nbins);
    float mc_height_dz_pt_hieta = *std::max_element(mc_reso_dz_pt_hieta, mc_reso_dz_pt_hieta+nbins);

    float mc_height_d0_eta_lopt = *std::max_element(mc_reso_d0_eta_lopt, mc_reso_d0_eta_lopt+nbins);
    float mc_height_dz_eta_lopt = *std::max_element(mc_reso_dz_eta_lopt, mc_reso_dz_eta_lopt+nbins);
    float mc_height_d0_eta_hipt = *std::max_element(mc_reso_d0_eta_hipt, mc_reso_d0_eta_hipt+nbins);
    float mc_height_dz_eta_hipt = *std::max_element(mc_reso_dz_eta_hipt, mc_reso_dz_eta_hipt+nbins);
    float mc_height_d0_eta_ulpt = *std::max_element(mc_reso_d0_eta_ulpt, mc_reso_d0_eta_ulpt+nbins);
    float mc_height_dz_eta_ulpt = *std::max_element(mc_reso_dz_eta_ulpt, mc_reso_dz_eta_ulpt+nbins);

    float mc_height_d0_phi_lopt = *std::max_element(mc_reso_d0_phi_lopt, mc_reso_d0_phi_lopt+nbins);
    float mc_height_dz_phi_lopt = *std::max_element(mc_reso_dz_phi_lopt, mc_reso_dz_phi_lopt+nbins);
    float mc_height_d0_phi_hipt = *std::max_element(mc_reso_d0_phi_hipt, mc_reso_d0_phi_hipt+nbins);
    float mc_height_dz_phi_hipt = *std::max_element(mc_reso_dz_phi_hipt, mc_reso_dz_phi_hipt+nbins);
    float mc_height_d0_phi_ulpt = *std::max_element(mc_reso_d0_phi_ulpt, mc_reso_d0_phi_ulpt+nbins);
    float mc_height_dz_phi_ulpt = *std::max_element(mc_reso_dz_phi_ulpt, mc_reso_dz_phi_ulpt+nbins);

    float data_floor_d0_pt_loeta = *std::min_element(data_reso_d0_pt_loeta, data_reso_d0_pt_loeta+nbins);
    float data_floor_dz_pt_loeta = *std::min_element(data_reso_dz_pt_loeta, data_reso_dz_pt_loeta+nbins);
    float data_floor_d0_pt_hieta = *std::min_element(data_reso_d0_pt_hieta, data_reso_d0_pt_hieta+nbins);
    float data_floor_dz_pt_hieta = *std::min_element(data_reso_dz_pt_hieta, data_reso_dz_pt_hieta+nbins);

    float data_floor_d0_eta_lopt = *std::min_element(data_reso_d0_eta_lopt, data_reso_d0_eta_lopt+nbins);
    float data_floor_dz_eta_lopt = *std::min_element(data_reso_dz_eta_lopt, data_reso_dz_eta_lopt+nbins);
    float data_floor_d0_eta_hipt = *std::min_element(data_reso_d0_eta_hipt, data_reso_d0_eta_hipt+nbins);
    float data_floor_dz_eta_hipt = *std::min_element(data_reso_dz_eta_hipt, data_reso_dz_eta_hipt+nbins);
    float data_floor_d0_eta_ulpt = *std::min_element(data_reso_d0_eta_ulpt, data_reso_d0_eta_ulpt+nbins);
    float data_floor_dz_eta_ulpt = *std::min_element(data_reso_dz_eta_ulpt, data_reso_dz_eta_ulpt+nbins);

    float data_floor_d0_phi_lopt = *std::min_element(data_reso_d0_phi_lopt, data_reso_d0_phi_lopt+nbins);
    float data_floor_dz_phi_lopt = *std::min_element(data_reso_dz_phi_lopt, data_reso_dz_phi_lopt+nbins);
    float data_floor_d0_phi_hipt = *std::min_element(data_reso_d0_phi_hipt, data_reso_d0_phi_hipt+nbins);
    float data_floor_dz_phi_hipt = *std::min_element(data_reso_dz_phi_hipt, data_reso_dz_phi_hipt+nbins);
    float data_floor_d0_phi_ulpt = *std::min_element(data_reso_d0_phi_ulpt, data_reso_d0_phi_ulpt+nbins);
    float data_floor_dz_phi_ulpt = *std::min_element(data_reso_dz_phi_ulpt, data_reso_dz_phi_ulpt+nbins);

    float mc_floor_d0_pt_loeta = *std::min_element(mc_reso_d0_pt_loeta, mc_reso_d0_pt_loeta+nbins);
    float mc_floor_dz_pt_loeta = *std::min_element(mc_reso_dz_pt_loeta, mc_reso_dz_pt_loeta+nbins);
    float mc_floor_d0_pt_hieta = *std::min_element(mc_reso_d0_pt_hieta, mc_reso_d0_pt_hieta+nbins);
    float mc_floor_dz_pt_hieta = *std::min_element(mc_reso_dz_pt_hieta, mc_reso_dz_pt_hieta+nbins);

    float mc_floor_d0_eta_lopt = *std::min_element(mc_reso_d0_eta_lopt, mc_reso_d0_eta_lopt+nbins);
    float mc_floor_dz_eta_lopt = *std::min_element(mc_reso_dz_eta_lopt, mc_reso_dz_eta_lopt+nbins);
    float mc_floor_d0_eta_hipt = *std::min_element(mc_reso_d0_eta_hipt, mc_reso_d0_eta_hipt+nbins);
    float mc_floor_dz_eta_hipt = *std::min_element(mc_reso_dz_eta_hipt, mc_reso_dz_eta_hipt+nbins);
    float mc_floor_d0_eta_ulpt = *std::min_element(mc_reso_d0_eta_ulpt, mc_reso_d0_eta_ulpt+nbins);
    float mc_floor_dz_eta_ulpt = *std::min_element(mc_reso_dz_eta_ulpt, mc_reso_dz_eta_ulpt+nbins);

    float mc_floor_d0_phi_lopt = *std::min_element(mc_reso_d0_phi_lopt, mc_reso_d0_phi_lopt+nbins);
    float mc_floor_dz_phi_lopt = *std::min_element(mc_reso_dz_phi_lopt, mc_reso_dz_phi_lopt+nbins);
    float mc_floor_d0_phi_hipt = *std::min_element(mc_reso_d0_phi_hipt, mc_reso_d0_phi_hipt+nbins);
    float mc_floor_dz_phi_hipt = *std::min_element(mc_reso_dz_phi_hipt, mc_reso_dz_phi_hipt+nbins);
    float mc_floor_d0_phi_ulpt = *std::min_element(mc_reso_d0_phi_ulpt, mc_reso_d0_phi_ulpt+nbins);
    float mc_floor_dz_phi_ulpt = *std::min_element(mc_reso_dz_phi_ulpt, mc_reso_dz_phi_ulpt+nbins);

    float height_d0_pt_loeta = std::max(data_height_d0_pt_loeta, mc_height_d0_pt_loeta);
    float height_dz_pt_loeta = std::max(data_height_dz_pt_loeta, mc_height_dz_pt_loeta);
    float height_d0_pt_hieta = std::max(data_height_d0_pt_hieta, mc_height_d0_pt_hieta);
    float height_dz_pt_hieta = std::max(data_height_dz_pt_hieta, mc_height_dz_pt_hieta);

    float height_d0_eta_lopt = std::max(data_height_d0_eta_lopt, mc_height_d0_eta_lopt);
    float height_dz_eta_lopt = std::max(data_height_dz_eta_lopt, mc_height_dz_eta_lopt);
    float height_d0_eta_hipt = std::max(data_height_d0_eta_hipt, mc_height_d0_eta_hipt);
    float height_dz_eta_hipt = std::max(data_height_dz_eta_hipt, mc_height_dz_eta_hipt);
    float height_d0_eta_ulpt = std::max(data_height_d0_eta_ulpt, mc_height_d0_eta_ulpt);
    float height_dz_eta_ulpt = std::max(data_height_dz_eta_ulpt, mc_height_dz_eta_ulpt);

    float height_d0_phi_lopt = std::max(data_height_d0_phi_lopt, mc_height_d0_phi_lopt);
    float height_dz_phi_lopt = std::max(data_height_dz_phi_lopt, mc_height_dz_phi_lopt);
    float height_d0_phi_hipt = std::max(data_height_d0_phi_hipt, mc_height_d0_phi_hipt);
    float height_dz_phi_hipt = std::max(data_height_dz_phi_hipt, mc_height_dz_phi_hipt);
    float height_d0_phi_ulpt = std::max(data_height_d0_phi_ulpt, mc_height_d0_phi_ulpt);
    float height_dz_phi_ulpt = std::max(data_height_dz_phi_ulpt, mc_height_dz_phi_ulpt);

    float floor_d0_pt_loeta = std::min(data_floor_d0_pt_loeta, mc_floor_d0_pt_loeta);
    float floor_dz_pt_loeta = std::min(data_floor_dz_pt_loeta, mc_floor_dz_pt_loeta);
    float floor_d0_pt_hieta = std::min(data_floor_d0_pt_hieta, mc_floor_d0_pt_hieta);
    float floor_dz_pt_hieta = std::min(data_floor_dz_pt_hieta, mc_floor_dz_pt_hieta);

    float floor_d0_eta_lopt = std::min(data_floor_d0_eta_lopt, mc_floor_d0_eta_lopt);
    float floor_dz_eta_lopt = std::min(data_floor_dz_eta_lopt, mc_floor_dz_eta_lopt);
    float floor_d0_eta_hipt = std::min(data_floor_d0_eta_hipt, mc_floor_d0_eta_hipt);
    float floor_dz_eta_hipt = std::min(data_floor_dz_eta_hipt, mc_floor_dz_eta_hipt);
    float floor_d0_eta_ulpt = std::min(data_floor_d0_eta_ulpt, mc_floor_d0_eta_ulpt);
    float floor_dz_eta_ulpt = std::min(data_floor_dz_eta_ulpt, mc_floor_dz_eta_ulpt);

    float floor_d0_phi_lopt = std::min(data_floor_d0_phi_lopt, mc_floor_d0_phi_lopt);
    float floor_dz_phi_lopt = std::min(data_floor_dz_phi_lopt, mc_floor_dz_phi_lopt);
    float floor_d0_phi_hipt = std::min(data_floor_d0_phi_hipt, mc_floor_d0_phi_hipt);
    float floor_dz_phi_hipt = std::min(data_floor_dz_phi_hipt, mc_floor_dz_phi_hipt);
    float floor_d0_phi_ulpt = std::min(data_floor_d0_phi_ulpt, mc_floor_d0_phi_ulpt);
    float floor_dz_phi_ulpt = std::min(data_floor_dz_phi_ulpt, mc_floor_dz_phi_ulpt);

    TCanvas *c_d0_pt = new TCanvas("c_d0_pt", "", 800, 600);
    canvas_setup(c_d0_pt);
    data_gr_d0_pt_hieta->SetMarkerStyle(20);
    data_gr_d0_pt_hieta->SetMarkerColor(kBlack);
    data_gr_d0_pt_loeta->SetMarkerStyle(20);
    data_gr_d0_pt_loeta->SetMarkerColor(kBlue);
    mc_gr_d0_pt_hieta->SetMarkerStyle(4);
    mc_gr_d0_pt_hieta->SetMarkerColor(kBlack);
    mc_gr_d0_pt_loeta->SetMarkerStyle(4);
    mc_gr_d0_pt_loeta->SetMarkerColor(kBlue);
    data_gr_d0_pt_hieta->Draw("AP");
    data_gr_d0_pt_loeta->Draw("P SAME");
    mc_gr_d0_pt_hieta->Draw("P SAME");
    mc_gr_d0_pt_loeta->Draw("P SAME");
    data_gr_d0_pt_hieta->GetXaxis()->SetTitle("Track #it{p_{T}} [GeV]");
    data_gr_d0_pt_hieta->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]");
    TLegend *leg_d0_pt = new TLegend(0.55, 0.7, 0.75, 0.85);
    leg_d0_pt->AddEntry(data_gr_d0_pt_hieta, "Data |#it{#eta}|<3.0", "p");
    leg_d0_pt->AddEntry(data_gr_d0_pt_loeta, "Data |#it{#eta}|<1.4", "p");
    leg_d0_pt->AddEntry(mc_gr_d0_pt_hieta, "Simulation |#it{#eta}|<3.0", "p");
    leg_d0_pt->AddEntry(mc_gr_d0_pt_loeta, "Simulation |#it{#eta}|<1.4", "p");
    leg_d0_pt->SetTextFont(42);
    leg_d0_pt->SetTextColor(kBlack);
    leg_d0_pt->SetBorderSize(0);
    leg_d0_pt->SetFillColor(0);
    leg_d0_pt->SetLineWidth(0);
    leg_d0_pt->SetTextSize(0.04);
    leg_d0_pt->Draw();
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_d0_pt);

    float height_d0_pt = std::max(height_d0_pt_hieta, height_d0_pt_loeta);
    float floor_d0_pt = std::min(floor_d0_pt_hieta, floor_d0_pt_loeta);

    data_gr_d0_pt_hieta->SetMaximum(height_d0_pt*1.5);
    data_gr_d0_pt_hieta->SetMinimum(0);
    c_d0_pt->Update();
    c_d0_pt->RedrawAxis();
    c_d0_pt->SaveAs(figdir + "d0_pt.png");
    data_gr_d0_pt_hieta->SetMaximum(height_d0_pt*100);
    data_gr_d0_pt_hieta->SetMinimum(floor_d0_pt/100);
    c_d0_pt->SetLogy();
    c_d0_pt->Update();
    c_d0_pt->RedrawAxis();
    c_d0_pt->SaveAs(figdir + "d0_pt_log.png");

    TCanvas *c_d0_eta = new TCanvas("c_d0_eta", "", 800, 600);
    canvas_setup(c_d0_eta);
    data_gr_d0_eta_ulpt->SetMarkerStyle(20);
    data_gr_d0_eta_ulpt->SetMarkerColor(kGreen);
    data_gr_d0_eta_hipt->SetMarkerStyle(20);
    data_gr_d0_eta_hipt->SetMarkerColor(kBlack);
    data_gr_d0_eta_lopt->SetMarkerStyle(20);
    data_gr_d0_eta_lopt->SetMarkerColor(kBlue);
    mc_gr_d0_eta_ulpt->SetMarkerStyle(4);
    mc_gr_d0_eta_ulpt->SetMarkerColor(kGreen);
    mc_gr_d0_eta_hipt->SetMarkerStyle(4);
    mc_gr_d0_eta_hipt->SetMarkerColor(kBlack);
    mc_gr_d0_eta_lopt->SetMarkerStyle(4);
    mc_gr_d0_eta_lopt->SetMarkerColor(kBlue);
    data_gr_d0_eta_hipt->Draw("AP");
    data_gr_d0_eta_ulpt->Draw("P SAME");
    data_gr_d0_eta_lopt->Draw("P SAME");
    mc_gr_d0_eta_hipt->Draw("P SAME");
    mc_gr_d0_eta_ulpt->Draw("P SAME");
    mc_gr_d0_eta_lopt->Draw("P SAME");
    data_gr_d0_eta_hipt->GetXaxis()->SetTitle("Track #it{#eta}");
    data_gr_d0_eta_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]");
    TLegend *leg_d0_eta = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_d0_eta->AddEntry(data_gr_d0_eta_ulpt, "Data 3.0<#it{p_{T}}<10.0 GeV", "p");
    leg_d0_eta->AddEntry(data_gr_d0_eta_hipt, "Data 1.0<#it{p_{T}}<3.0 GeV", "p");
    leg_d0_eta->AddEntry(data_gr_d0_eta_lopt, "Data 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_eta->AddEntry(mc_gr_d0_eta_ulpt, "Simulation 3.0<#it{p_{T}}<10.0 GeV", "p");
    leg_d0_eta->AddEntry(mc_gr_d0_eta_hipt, "Simulation 1.0<#it{p_{T}}<3.0 GeV", "p");
    leg_d0_eta->AddEntry(mc_gr_d0_eta_lopt, "Simulation 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_eta->SetTextFont(42);
    leg_d0_eta->SetTextColor(kBlack);
    leg_d0_eta->SetBorderSize(0);
    leg_d0_eta->SetFillColor(0);
    leg_d0_eta->SetLineWidth(0);
    leg_d0_eta->SetTextSize(0.04);
    leg_d0_eta->Draw();
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_d0_eta);

    float height_d0_eta = std::max(height_d0_eta_hipt, height_d0_eta_lopt);
    height_d0_eta = std::max(height_d0_eta, height_d0_eta_ulpt);
    float floor_d0_eta = std::min(floor_d0_eta_hipt, floor_d0_eta_lopt);
    floor_d0_eta = std::min(floor_d0_eta, floor_d0_eta_ulpt);

    data_gr_d0_eta_hipt->SetMaximum(height_d0_eta*1.5);
    data_gr_d0_eta_hipt->SetMinimum(0);
    c_d0_eta->Update();
    c_d0_eta->RedrawAxis();
    c_d0_eta->SaveAs(figdir + "d0_eta.png");
    data_gr_d0_eta_hipt->SetMaximum(height_d0_eta*100);
    data_gr_d0_eta_hipt->SetMinimum(floor_d0_eta/100);
    c_d0_eta->SetLogy();
    c_d0_eta->Update();
    c_d0_eta->RedrawAxis();
    c_d0_eta->SaveAs(figdir + "d0_eta_log.png");


    TCanvas *c_d0_phi = new TCanvas("c_d0_phi", "", 800, 600);
    canvas_setup(c_d0_phi);
    data_gr_d0_phi_ulpt->SetMarkerStyle(20);
    data_gr_d0_phi_ulpt->SetMarkerColor(kGreen);
    data_gr_d0_phi_hipt->SetMarkerStyle(20);
    data_gr_d0_phi_hipt->SetMarkerColor(kBlack);
    data_gr_d0_phi_lopt->SetMarkerStyle(20);
    data_gr_d0_phi_lopt->SetMarkerColor(kBlue);
    mc_gr_d0_phi_ulpt->SetMarkerStyle(4);
    mc_gr_d0_phi_ulpt->SetMarkerColor(kGreen);
    mc_gr_d0_phi_hipt->SetMarkerStyle(4);
    mc_gr_d0_phi_hipt->SetMarkerColor(kBlack);
    mc_gr_d0_phi_lopt->SetMarkerStyle(4);
    mc_gr_d0_phi_lopt->SetMarkerColor(kBlue);
    data_gr_d0_phi_hipt->Draw("AP");
    data_gr_d0_phi_ulpt->Draw("P SAME");
    data_gr_d0_phi_lopt->Draw("P SAME");
    mc_gr_d0_phi_hipt->Draw("P SAME");
    mc_gr_d0_phi_ulpt->Draw("P SAME");
    mc_gr_d0_phi_lopt->Draw("P SAME");
    data_gr_d0_phi_hipt->GetXaxis()->SetTitle("Track #it{#phi}");
    data_gr_d0_phi_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]");
    TLegend *leg_d0_phi = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_d0_phi->AddEntry(data_gr_d0_phi_ulpt, "Data 3.0<#it{p_{T}}<10.0 GeV", "p");
    leg_d0_phi->AddEntry(data_gr_d0_phi_hipt, "Data 1.0<#it{p_{T}}<3.0 GeV", "p");
    leg_d0_phi->AddEntry(data_gr_d0_phi_lopt, "Data 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_phi->AddEntry(mc_gr_d0_phi_ulpt, "Simulation 3.0<#it{p_{T}}<10.0 GeV", "p");
    leg_d0_phi->AddEntry(mc_gr_d0_phi_hipt, "Simulation 1.0<#it{p_{T}}<3.0 GeV", "p");
    leg_d0_phi->AddEntry(mc_gr_d0_phi_lopt, "Simulation 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_phi->SetTextFont(42);
    leg_d0_phi->SetTextColor(kBlack);
    leg_d0_phi->SetBorderSize(0);
    leg_d0_phi->SetFillColor(0);
    leg_d0_phi->SetLineWidth(0);
    leg_d0_phi->SetTextSize(0.04);
    leg_d0_phi->Draw();
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_d0_phi);

    float height_d0_phi = std::max(height_d0_phi_hipt, height_d0_phi_lopt);
    height_d0_phi = std::max(height_d0_phi, height_d0_phi_ulpt);
    float floor_d0_phi = std::min(floor_d0_phi_hipt, floor_d0_phi_lopt);
    floor_d0_phi = std::min(floor_d0_phi, floor_d0_phi_ulpt);

    data_gr_d0_phi_hipt->SetMaximum(height_d0_phi*1.5);
    data_gr_d0_phi_hipt->SetMinimum(0);
    c_d0_phi->Update();
    c_d0_phi->RedrawAxis();
    c_d0_phi->SaveAs(figdir + "d0_phi.png");
    data_gr_d0_phi_hipt->SetMaximum(height_d0_phi*100);
    data_gr_d0_phi_hipt->SetMinimum(floor_d0_phi/100);
    c_d0_phi->SetLogy();
    c_d0_phi->Update();
    c_d0_phi->RedrawAxis();
    c_d0_phi->SaveAs(figdir + "d0_phi_log.png");

    TCanvas *c_dz_pt = new TCanvas("c_dz_pt", "", 800, 600);
    canvas_setup(c_dz_pt);
    data_gr_dz_pt_hieta->SetMarkerStyle(20);
    data_gr_dz_pt_hieta->SetMarkerColor(kBlack);
    data_gr_dz_pt_loeta->SetMarkerStyle(20);
    data_gr_dz_pt_loeta->SetMarkerColor(kBlue);
    mc_gr_dz_pt_hieta->SetMarkerStyle(4);
    mc_gr_dz_pt_hieta->SetMarkerColor(kBlack);
    mc_gr_dz_pt_loeta->SetMarkerStyle(4);
    mc_gr_dz_pt_loeta->SetMarkerColor(kBlue);
    data_gr_dz_pt_hieta->Draw("AP");
    data_gr_dz_pt_loeta->Draw("P SAME");
    mc_gr_dz_pt_hieta->Draw("P SAME");
    mc_gr_dz_pt_loeta->Draw("P SAME");
    data_gr_dz_pt_hieta->GetXaxis()->SetTitle("Track #it{p_{T}} [GeV]");
    data_gr_dz_pt_hieta->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [#mum]");
    TLegend *leg_dz_pt = new TLegend(0.55, 0.7, 0.75, 0.85);
    leg_dz_pt->AddEntry(data_gr_dz_pt_hieta, "Data |#it{#eta}|<3.0", "p");
    leg_dz_pt->AddEntry(data_gr_dz_pt_loeta, "Data |#it{#eta}|<1.4", "p");
    leg_dz_pt->AddEntry(mc_gr_dz_pt_hieta, "Simulation |#it{#eta}|<3.0", "p");
    leg_dz_pt->AddEntry(mc_gr_dz_pt_loeta, "Simulation |#it{#eta}|<1.4", "p");
    leg_dz_pt->SetTextFont(42);
    leg_dz_pt->SetTextColor(kBlack);
    leg_dz_pt->SetBorderSize(0);
    leg_dz_pt->SetFillColor(0);
    leg_dz_pt->SetLineWidth(0);
    leg_dz_pt->SetTextSize(0.04);
    leg_dz_pt->Draw();
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_dz_pt);

    float height_dz_pt = std::max(height_dz_pt_hieta, height_dz_pt_loeta);
    float floor_dz_pt = std::min(floor_dz_pt_hieta, floor_dz_pt_loeta);

    data_gr_dz_pt_hieta->SetMaximum(height_dz_pt*1.5);
    data_gr_dz_pt_hieta->SetMinimum(0);
    c_dz_pt->Update();
    c_dz_pt->RedrawAxis();
    c_dz_pt->SaveAs(figdir + "dz_pt.png");
    data_gr_dz_pt_hieta->SetMaximum(height_dz_pt*100);
    data_gr_dz_pt_hieta->SetMinimum(floor_dz_pt/100);
    c_dz_pt->SetLogy();
    c_dz_pt->Update();
    c_dz_pt->RedrawAxis();
    c_dz_pt->SaveAs(figdir + "dz_pt_log.png");

    TCanvas *c_dz_eta = new TCanvas("c_dz_eta", "", 800, 600);
    canvas_setup(c_dz_eta);
    data_gr_dz_eta_ulpt->SetMarkerStyle(20);
    data_gr_dz_eta_ulpt->SetMarkerColor(kGreen);
    data_gr_dz_eta_hipt->SetMarkerStyle(20);
    data_gr_dz_eta_hipt->SetMarkerColor(kBlack);
    data_gr_dz_eta_lopt->SetMarkerStyle(20);
    data_gr_dz_eta_lopt->SetMarkerColor(kBlue);
    mc_gr_dz_eta_ulpt->SetMarkerStyle(4);
    mc_gr_dz_eta_ulpt->SetMarkerColor(kGreen);
    mc_gr_dz_eta_hipt->SetMarkerStyle(4);
    mc_gr_dz_eta_hipt->SetMarkerColor(kBlack);
    mc_gr_dz_eta_lopt->SetMarkerStyle(4);
    mc_gr_dz_eta_lopt->SetMarkerColor(kBlue);
    data_gr_dz_eta_hipt->Draw("AP");
    data_gr_dz_eta_ulpt->Draw("P SAME");
    data_gr_dz_eta_lopt->Draw("P SAME");
    mc_gr_dz_eta_hipt->Draw("P SAME");
    mc_gr_dz_eta_ulpt->Draw("P SAME");
    mc_gr_dz_eta_lopt->Draw("P SAME");
    data_gr_dz_eta_hipt->GetXaxis()->SetTitle("Track #it{#eta}");
    data_gr_dz_eta_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [#mum]");
    TLegend *leg_dz_eta = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_dz_eta->AddEntry(data_gr_dz_eta_ulpt, "Data 3.0<#it{p_{T}}<10.0 GeV", "p");
    leg_dz_eta->AddEntry(data_gr_dz_eta_hipt, "Data 1.0<#it{p_{T}}<3.0 GeV", "p");
    leg_dz_eta->AddEntry(data_gr_dz_eta_lopt, "Data 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_dz_eta->AddEntry(mc_gr_dz_eta_ulpt, "Simulation 3.0<#it{p_{T}}<10.0 GeV", "p");
    leg_dz_eta->AddEntry(mc_gr_dz_eta_hipt, "Simulation 1.0<#it{p_{T}}<3.0 GeV", "p");
    leg_dz_eta->AddEntry(mc_gr_dz_eta_lopt, "Simulation 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_dz_eta->SetTextFont(42);
    leg_dz_eta->SetTextColor(kBlack);
    leg_dz_eta->SetBorderSize(0);
    leg_dz_eta->SetFillColor(0);
    leg_dz_eta->SetLineWidth(0);
    leg_dz_eta->SetTextSize(0.04);
    leg_dz_eta->Draw();
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_dz_eta);

    float height_dz_eta = std::max(height_dz_eta_hipt, height_dz_eta_lopt);
    height_dz_eta = std::max(height_dz_eta, height_dz_eta_ulpt);
    float floor_dz_eta = std::min(floor_dz_eta_hipt, floor_dz_eta_lopt);
    floor_dz_eta = std::min(floor_dz_eta, floor_dz_eta_ulpt);

    data_gr_dz_eta_hipt->SetMaximum(height_dz_eta*1.5);
    data_gr_dz_eta_hipt->SetMinimum(0);
    c_dz_eta->Update();
    c_dz_eta->RedrawAxis();
    c_dz_eta->SaveAs(figdir + "dz_eta.png");
    data_gr_dz_eta_hipt->SetMaximum(height_dz_eta*100);
    data_gr_dz_eta_hipt->SetMinimum(floor_dz_eta/100);
    c_dz_eta->SetLogy();
    c_dz_eta->Update();
    c_dz_eta->RedrawAxis();
    c_dz_eta->SaveAs(figdir + "dz_eta_log.png");


    TCanvas *c_dz_phi = new TCanvas("c_dz_phi", "", 800, 600);
    canvas_setup(c_dz_phi);
    data_gr_dz_phi_ulpt->SetMarkerStyle(20);
    data_gr_dz_phi_ulpt->SetMarkerColor(kGreen);
    data_gr_dz_phi_hipt->SetMarkerStyle(20);
    data_gr_dz_phi_hipt->SetMarkerColor(kBlack);
    data_gr_dz_phi_lopt->SetMarkerStyle(20);
    data_gr_dz_phi_lopt->SetMarkerColor(kBlue);
    mc_gr_dz_phi_ulpt->SetMarkerStyle(4);
    mc_gr_dz_phi_ulpt->SetMarkerColor(kGreen);
    mc_gr_dz_phi_hipt->SetMarkerStyle(4);
    mc_gr_dz_phi_hipt->SetMarkerColor(kBlack);
    mc_gr_dz_phi_lopt->SetMarkerStyle(4);
    mc_gr_dz_phi_lopt->SetMarkerColor(kBlue);
    data_gr_dz_phi_hipt->Draw("AP");
    data_gr_dz_phi_ulpt->Draw("P SAME");
    data_gr_dz_phi_lopt->Draw("P SAME");
    mc_gr_dz_phi_hipt->Draw("P SAME");
    mc_gr_dz_phi_ulpt->Draw("P SAME");
    mc_gr_dz_phi_lopt->Draw("P SAME");
    data_gr_dz_phi_hipt->GetXaxis()->SetTitle("Track #it{#phi}");
    data_gr_dz_phi_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [#mum]");
    TLegend *leg_dz_phi = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_dz_phi->AddEntry(data_gr_dz_phi_ulpt, "Data 3.0<#it{p_{T}}<10.0 GeV", "p");
    leg_dz_phi->AddEntry(data_gr_dz_phi_hipt, "Data 1.0<#it{p_{T}}<3.0 GeV", "p");
    leg_dz_phi->AddEntry(data_gr_dz_phi_lopt, "Data 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_dz_phi->AddEntry(mc_gr_dz_phi_ulpt, "Simulation 3.0<#it{p_{T}}<10.0 GeV", "p");
    leg_dz_phi->AddEntry(mc_gr_dz_phi_hipt, "Simulation 1.0<#it{p_{T}}<3.0 GeV", "p");
    leg_dz_phi->AddEntry(mc_gr_dz_phi_lopt, "Simulation 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_dz_phi->SetTextFont(42);
    leg_dz_phi->SetTextColor(kBlack);
    leg_dz_phi->SetBorderSize(0);
    leg_dz_phi->SetFillColor(0);
    leg_dz_phi->SetLineWidth(0);
    leg_dz_phi->SetTextSize(0.04);
    leg_dz_phi->Draw();
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_dz_phi);

    float height_dz_phi = std::max(height_dz_phi_hipt, height_dz_phi_lopt);
    height_dz_phi = std::max(height_dz_phi, height_dz_phi_ulpt);
    float floor_dz_phi = std::min(floor_dz_phi_hipt, floor_dz_phi_lopt);
    floor_dz_phi = std::min(floor_dz_phi, floor_dz_phi_ulpt);

    data_gr_dz_phi_hipt->SetMaximum(height_dz_phi*1.5);
    data_gr_dz_phi_hipt->SetMinimum(0);
    c_dz_phi->Update();
    c_dz_phi->RedrawAxis();
    c_dz_phi->SaveAs(figdir + "dz_phi.png");
    data_gr_dz_phi_hipt->SetMaximum(height_dz_phi*100);
    data_gr_dz_phi_hipt->SetMinimum(floor_dz_phi/100);
    c_dz_phi->SetLogy();
    c_dz_phi->Update();
    c_dz_phi->RedrawAxis();
    c_dz_phi->SaveAs(figdir + "dz_phi_log.png");

    return 0;
}
