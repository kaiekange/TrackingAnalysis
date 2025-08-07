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

int write_ip_res(){

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

        std::cout << data_reso_dz_pt_loeta[i] - mc_reso_dz_pt_loeta[i] << std::endl;
    }

    return 0;
}
