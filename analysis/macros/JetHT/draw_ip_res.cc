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

const TString figdir = "../../figures/"+datatype+"/ip_res/";
const TString jsondir = "../../json/"+datatype+"/ip_res/";
const int nbins = 100;

int draw_ip_res(){

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, 2022";

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

    float div_reso_d0_pt_loeta[nbins];
    float div_reso_dz_pt_loeta[nbins];
    float div_reso_d0_pt_hieta[nbins];
    float div_reso_dz_pt_hieta[nbins];

    float div_reso_d0_eta_lopt[nbins];
    float div_reso_dz_eta_lopt[nbins];
    float div_reso_d0_eta_hipt[nbins];
    float div_reso_dz_eta_hipt[nbins];
    float div_reso_d0_eta_ulpt[nbins];
    float div_reso_dz_eta_ulpt[nbins];

    float div_reso_d0_phi_lopt[nbins];
    float div_reso_dz_phi_lopt[nbins];
    float div_reso_d0_phi_hipt[nbins];
    float div_reso_dz_phi_hipt[nbins];
    float div_reso_d0_phi_ulpt[nbins];
    float div_reso_dz_phi_ulpt[nbins];

    for(int i=0; i<nbins; i++){
        nlohmann::json results;
        std::ifstream infile(jsondir + Form("/compare/fit_%d.json",i));
        infile >> results;

        eta[i] = results["eta"];
        phi[i] = results["phi"];
        pt[i] = results["pt"];

        data_reso_d0_pt_loeta[i] = results["reso_d0_pt_loeta_data"];
        data_reso_dz_pt_loeta[i] = results["reso_dz_pt_loeta_data"];
        data_reso_d0_pt_hieta[i] = results["reso_d0_pt_hieta_data"];
        data_reso_dz_pt_hieta[i] = results["reso_dz_pt_hieta_data"];

        data_reso_d0_eta_lopt[i] = results["reso_d0_eta_lopt_data"];
        data_reso_dz_eta_lopt[i] = results["reso_dz_eta_lopt_data"];
        data_reso_d0_eta_hipt[i] = results["reso_d0_eta_hipt_data"];
        data_reso_dz_eta_hipt[i] = results["reso_dz_eta_hipt_data"];
        data_reso_d0_eta_ulpt[i] = results["reso_d0_eta_ulpt_data"];
        data_reso_dz_eta_ulpt[i] = results["reso_dz_eta_ulpt_data"];

        data_reso_d0_phi_lopt[i] = results["reso_d0_phi_lopt_data"];
        data_reso_dz_phi_lopt[i] = results["reso_dz_phi_lopt_data"];
        data_reso_d0_phi_hipt[i] = results["reso_d0_phi_hipt_data"];
        data_reso_dz_phi_hipt[i] = results["reso_dz_phi_hipt_data"];
        data_reso_d0_phi_ulpt[i] = results["reso_d0_phi_ulpt_data"];
        data_reso_dz_phi_ulpt[i] = results["reso_dz_phi_ulpt_data"];

        mc_reso_d0_pt_loeta[i] = results["reso_d0_pt_loeta_mc"];
        mc_reso_dz_pt_loeta[i] = results["reso_dz_pt_loeta_mc"];
        mc_reso_d0_pt_hieta[i] = results["reso_d0_pt_hieta_mc"];
        mc_reso_dz_pt_hieta[i] = results["reso_dz_pt_hieta_mc"];

        mc_reso_d0_eta_lopt[i] = results["reso_d0_eta_lopt_mc"];
        mc_reso_dz_eta_lopt[i] = results["reso_dz_eta_lopt_mc"];
        mc_reso_d0_eta_hipt[i] = results["reso_d0_eta_hipt_mc"];
        mc_reso_dz_eta_hipt[i] = results["reso_dz_eta_hipt_mc"];
        mc_reso_d0_eta_ulpt[i] = results["reso_d0_eta_ulpt_mc"];
        mc_reso_dz_eta_ulpt[i] = results["reso_dz_eta_ulpt_mc"];

        mc_reso_d0_phi_lopt[i] = results["reso_d0_phi_lopt_mc"];
        mc_reso_dz_phi_lopt[i] = results["reso_dz_phi_lopt_mc"];
        mc_reso_d0_phi_hipt[i] = results["reso_d0_phi_hipt_mc"];
        mc_reso_dz_phi_hipt[i] = results["reso_dz_phi_hipt_mc"];
        mc_reso_d0_phi_ulpt[i] = results["reso_d0_phi_ulpt_mc"];
        mc_reso_dz_phi_ulpt[i] = results["reso_dz_phi_ulpt_mc"];

        div_reso_d0_pt_loeta[i] = data_reso_d0_pt_loeta[i] / mc_reso_d0_pt_loeta[i];
        div_reso_dz_pt_loeta[i] = data_reso_dz_pt_loeta[i] / mc_reso_dz_pt_loeta[i];
        div_reso_d0_pt_hieta[i] = data_reso_d0_pt_hieta[i] / mc_reso_d0_pt_hieta[i];
        div_reso_dz_pt_hieta[i] = data_reso_dz_pt_hieta[i] / mc_reso_dz_pt_hieta[i];

        div_reso_d0_eta_lopt[i] = data_reso_d0_eta_lopt[i] / mc_reso_d0_eta_lopt[i];
        div_reso_dz_eta_lopt[i] = data_reso_dz_eta_lopt[i] / mc_reso_dz_eta_lopt[i];
        div_reso_d0_eta_hipt[i] = data_reso_d0_eta_hipt[i] / mc_reso_d0_eta_hipt[i];
        div_reso_dz_eta_hipt[i] = data_reso_dz_eta_hipt[i] / mc_reso_dz_eta_hipt[i];
        div_reso_d0_eta_ulpt[i] = data_reso_d0_eta_ulpt[i] / mc_reso_d0_eta_ulpt[i];
        div_reso_dz_eta_ulpt[i] = data_reso_dz_eta_ulpt[i] / mc_reso_dz_eta_ulpt[i];

        div_reso_d0_phi_lopt[i] = data_reso_d0_phi_lopt[i] / mc_reso_d0_phi_lopt[i];
        div_reso_dz_phi_lopt[i] = data_reso_dz_phi_lopt[i] / mc_reso_dz_phi_lopt[i];
        div_reso_d0_phi_hipt[i] = data_reso_d0_phi_hipt[i] / mc_reso_d0_phi_hipt[i];
        div_reso_dz_phi_hipt[i] = data_reso_dz_phi_hipt[i] / mc_reso_dz_phi_hipt[i];
        div_reso_d0_phi_ulpt[i] = data_reso_d0_phi_ulpt[i] / mc_reso_d0_phi_ulpt[i];
        div_reso_dz_phi_ulpt[i] = data_reso_dz_phi_ulpt[i] / mc_reso_dz_phi_ulpt[i];
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
    
    TGraph * div_gr_d0_pt_loeta = new TGraph(nbins, pt, div_reso_d0_pt_loeta);
    TGraph * div_gr_dz_pt_loeta = new TGraph(nbins, pt, div_reso_dz_pt_loeta);
    TGraph * div_gr_d0_pt_hieta = new TGraph(nbins, pt, div_reso_d0_pt_hieta);
    TGraph * div_gr_dz_pt_hieta = new TGraph(nbins, pt, div_reso_dz_pt_hieta);
    
    TGraph * div_gr_d0_eta_lopt = new TGraph(nbins, eta, div_reso_d0_eta_lopt);
    TGraph * div_gr_dz_eta_lopt = new TGraph(nbins, eta, div_reso_dz_eta_lopt);
    TGraph * div_gr_d0_eta_hipt = new TGraph(nbins, eta, div_reso_d0_eta_hipt);
    TGraph * div_gr_dz_eta_hipt = new TGraph(nbins, eta, div_reso_dz_eta_hipt);
    TGraph * div_gr_d0_eta_ulpt = new TGraph(nbins, eta, div_reso_d0_eta_ulpt);
    TGraph * div_gr_dz_eta_ulpt = new TGraph(nbins, eta, div_reso_dz_eta_ulpt);

    TGraph * div_gr_d0_phi_lopt = new TGraph(nbins, phi, div_reso_d0_phi_lopt);
    TGraph * div_gr_dz_phi_lopt = new TGraph(nbins, phi, div_reso_dz_phi_lopt);
    TGraph * div_gr_d0_phi_hipt = new TGraph(nbins, phi, div_reso_d0_phi_hipt);
    TGraph * div_gr_dz_phi_hipt = new TGraph(nbins, phi, div_reso_dz_phi_hipt);
    TGraph * div_gr_d0_phi_ulpt = new TGraph(nbins, phi, div_reso_d0_phi_ulpt);
    TGraph * div_gr_dz_phi_ulpt = new TGraph(nbins, phi, div_reso_dz_phi_ulpt);
    
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

    compare_gr(data_gr_d0_pt_hieta, mc_gr_d0_pt_hieta, div_gr_d0_pt_hieta, height_d0_pt_hieta, floor_d0_pt_hieta, pt[0], pt[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_hieta");
    compare_gr(data_gr_d0_pt_loeta, mc_gr_d0_pt_loeta, div_gr_d0_pt_loeta, height_d0_pt_loeta, floor_d0_pt_loeta, pt[0], pt[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{|#it{#eta}|<1.4}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_loeta");

    compare_gr(data_gr_d0_eta_lopt, mc_gr_d0_eta_lopt, div_gr_d0_eta_lopt, height_d0_eta_lopt, floor_d0_eta_lopt, eta[0], eta[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_lopt");
    compare_gr(data_gr_d0_eta_hipt, mc_gr_d0_eta_hipt, div_gr_d0_eta_hipt, height_d0_eta_hipt, floor_d0_eta_hipt, eta[0], eta[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_hipt");
    compare_gr(data_gr_d0_eta_ulpt, mc_gr_d0_eta_ulpt, div_gr_d0_eta_ulpt, height_d0_eta_ulpt, floor_d0_eta_ulpt, eta[0], eta[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{3.0<#it{p_{T}}<10.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_ulpt");

    compare_gr(data_gr_d0_phi_lopt, mc_gr_d0_phi_lopt, div_gr_d0_phi_lopt, height_d0_phi_lopt, floor_d0_phi_lopt, phi[0], phi[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_lopt");
    compare_gr(data_gr_d0_phi_hipt, mc_gr_d0_phi_hipt, div_gr_d0_phi_hipt, height_d0_phi_hipt, floor_d0_phi_hipt, phi[0], phi[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_hipt");
    compare_gr(data_gr_d0_phi_ulpt, mc_gr_d0_phi_ulpt, div_gr_d0_phi_ulpt, height_d0_phi_ulpt, floor_d0_phi_ulpt, phi[0], phi[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{3.0<#it{p_{T}}<10.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_ulpt");
    

    compare_gr(data_gr_dz_pt_hieta, mc_gr_dz_pt_hieta, div_gr_dz_pt_hieta, height_dz_pt_hieta, floor_dz_pt_hieta, pt[0], pt[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_hieta");
    compare_gr(data_gr_dz_pt_loeta, mc_gr_dz_pt_loeta, div_gr_dz_pt_loeta, height_dz_pt_loeta, floor_dz_pt_loeta, pt[0], pt[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{|#it{#eta}|<1.4}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_loeta");

    compare_gr(data_gr_dz_eta_lopt, mc_gr_dz_eta_lopt, div_gr_dz_eta_lopt, height_dz_eta_lopt, floor_dz_eta_lopt, eta[0], eta[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_lopt");
    compare_gr(data_gr_dz_eta_hipt, mc_gr_dz_eta_hipt, div_gr_dz_eta_hipt, height_dz_eta_hipt, floor_dz_eta_hipt, eta[0], eta[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_hipt");
    compare_gr(data_gr_dz_eta_ulpt, mc_gr_dz_eta_ulpt, div_gr_dz_eta_ulpt, height_dz_eta_ulpt, floor_dz_eta_ulpt, eta[0], eta[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{3.0<#it{p_{T}}<10.0 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_ulpt");

    compare_gr(data_gr_dz_phi_lopt, mc_gr_dz_phi_lopt, div_gr_dz_phi_lopt, height_dz_phi_lopt, floor_dz_phi_lopt, phi[0], phi[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_lopt");
    compare_gr(data_gr_dz_phi_hipt, mc_gr_dz_phi_hipt, div_gr_dz_phi_hipt, height_dz_phi_hipt, floor_dz_phi_hipt, phi[0], phi[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{1.0<#it{p_{T}}<3.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_hipt");
    compare_gr(data_gr_dz_phi_ulpt, mc_gr_dz_phi_ulpt, div_gr_dz_phi_ulpt, height_dz_phi_ulpt, floor_dz_phi_ulpt, phi[0], phi[nbins-1], 0.8, 1.2, "Data", "Simulation", "#splitline{"+datatype_text+"}{3.0<#it{p_{T}}<10.0 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_ulpt");

    return 0;
}
