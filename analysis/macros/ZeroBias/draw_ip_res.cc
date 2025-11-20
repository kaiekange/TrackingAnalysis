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

const TString datatype_text = "Unbiased collision events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/ZeroBias";
const Int_t nbins = 500;

Int_t draw_ip_res(TString period){

    TString figdir = storage_dir + "/figures/" + period + "/ip_res/";

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, " + period;

    Float_t reso_data_d0_pt_loeta[nbins];
    Float_t reso_data_dz_pt_loeta[nbins];
    Float_t reso_data_d0_pt_hieta[nbins];
    Float_t reso_data_dz_pt_hieta[nbins];
    Float_t reso_data_d0_pt_uleta[nbins];
    Float_t reso_data_dz_pt_uleta[nbins];
    Float_t reso_data_d0_pt_alleta[nbins];
    Float_t reso_data_dz_pt_alleta[nbins];

    Float_t reso_data_d0_eta_lopt[nbins];
    Float_t reso_data_dz_eta_lopt[nbins];
    Float_t reso_data_d0_eta_hipt[nbins];
    Float_t reso_data_dz_eta_hipt[nbins];
    Float_t reso_data_d0_eta_ulpt[nbins];
    Float_t reso_data_dz_eta_ulpt[nbins];
    Float_t reso_data_d0_eta_allpt[nbins];
    Float_t reso_data_dz_eta_allpt[nbins];

    Float_t reso_data_d0_phi_lopt[nbins];
    Float_t reso_data_dz_phi_lopt[nbins];
    Float_t reso_data_d0_phi_hipt[nbins];
    Float_t reso_data_dz_phi_hipt[nbins];
    Float_t reso_data_d0_phi_ulpt[nbins];
    Float_t reso_data_dz_phi_ulpt[nbins];
    Float_t reso_data_d0_phi_allpt[nbins];
    Float_t reso_data_dz_phi_allpt[nbins];

    Float_t reso_mc_d0_pt_loeta[nbins];
    Float_t reso_mc_dz_pt_loeta[nbins];
    Float_t reso_mc_d0_pt_hieta[nbins];
    Float_t reso_mc_dz_pt_hieta[nbins];
    Float_t reso_mc_d0_pt_uleta[nbins];
    Float_t reso_mc_dz_pt_uleta[nbins];
    Float_t reso_mc_d0_pt_alleta[nbins];
    Float_t reso_mc_dz_pt_alleta[nbins];

    Float_t reso_mc_d0_eta_lopt[nbins];
    Float_t reso_mc_dz_eta_lopt[nbins];
    Float_t reso_mc_d0_eta_hipt[nbins];
    Float_t reso_mc_dz_eta_hipt[nbins];
    Float_t reso_mc_d0_eta_ulpt[nbins];
    Float_t reso_mc_dz_eta_ulpt[nbins];
    Float_t reso_mc_d0_eta_allpt[nbins];
    Float_t reso_mc_dz_eta_allpt[nbins];

    Float_t reso_mc_d0_phi_lopt[nbins];
    Float_t reso_mc_dz_phi_lopt[nbins];
    Float_t reso_mc_d0_phi_hipt[nbins];
    Float_t reso_mc_dz_phi_hipt[nbins];
    Float_t reso_mc_d0_phi_ulpt[nbins];
    Float_t reso_mc_dz_phi_ulpt[nbins];
    Float_t reso_mc_d0_phi_allpt[nbins];
    Float_t reso_mc_dz_phi_allpt[nbins];

    Float_t reso_div_d0_pt_loeta[nbins];
    Float_t reso_div_dz_pt_loeta[nbins];
    Float_t reso_div_d0_pt_hieta[nbins];
    Float_t reso_div_dz_pt_hieta[nbins];
    Float_t reso_div_d0_pt_uleta[nbins];
    Float_t reso_div_dz_pt_uleta[nbins];
    Float_t reso_div_d0_pt_alleta[nbins];
    Float_t reso_div_dz_pt_alleta[nbins];

    Float_t reso_div_d0_eta_lopt[nbins];
    Float_t reso_div_dz_eta_lopt[nbins];
    Float_t reso_div_d0_eta_hipt[nbins];
    Float_t reso_div_dz_eta_hipt[nbins];
    Float_t reso_div_d0_eta_ulpt[nbins];
    Float_t reso_div_dz_eta_ulpt[nbins];
    Float_t reso_div_d0_eta_allpt[nbins];
    Float_t reso_div_dz_eta_allpt[nbins];

    Float_t reso_div_d0_phi_lopt[nbins];
    Float_t reso_div_dz_phi_lopt[nbins];
    Float_t reso_div_d0_phi_hipt[nbins];
    Float_t reso_div_dz_phi_hipt[nbins];
    Float_t reso_div_d0_phi_ulpt[nbins];
    Float_t reso_div_dz_phi_ulpt[nbins];
    Float_t reso_div_d0_phi_allpt[nbins];
    Float_t reso_div_dz_phi_allpt[nbins];
    

    Float_t eta[nbins];
    Float_t phi[nbins];
    Float_t pt[nbins];

    for(Int_t i=0; i<nbins; i++){
        nlohmann::json results;
        std::ifstream infile(storage_dir + "/json/" + period + Form("/ip_res/fit_%d.json",i));
        infile >> results;

        eta[i] = results["eta"];
        phi[i] = results["phi"];
        pt[i] = results["pt"];

        reso_data_d0_pt_loeta[i] = results["reso_data_d0_pt_loeta"];
        reso_data_dz_pt_loeta[i] = results["reso_data_dz_pt_loeta"];
        reso_data_d0_pt_hieta[i] = results["reso_data_d0_pt_hieta"];
        reso_data_dz_pt_hieta[i] = results["reso_data_dz_pt_hieta"];
        reso_data_d0_pt_uleta[i] = results["reso_data_d0_pt_uleta"];
        reso_data_dz_pt_uleta[i] = results["reso_data_dz_pt_uleta"];
        reso_data_d0_pt_alleta[i] = results["reso_data_d0_pt_alleta"];
        reso_data_dz_pt_alleta[i] = results["reso_data_dz_pt_alleta"];

        reso_data_d0_eta_lopt[i] = results["reso_data_d0_eta_lopt"];
        reso_data_dz_eta_lopt[i] = results["reso_data_dz_eta_lopt"];
        reso_data_d0_eta_hipt[i] = results["reso_data_d0_eta_hipt"];
        reso_data_dz_eta_hipt[i] = results["reso_data_dz_eta_hipt"];
        reso_data_d0_eta_ulpt[i] = results["reso_data_d0_eta_ulpt"];
        reso_data_dz_eta_ulpt[i] = results["reso_data_dz_eta_ulpt"];
        reso_data_d0_eta_allpt[i] = results["reso_data_d0_eta_allpt"];
        reso_data_dz_eta_allpt[i] = results["reso_data_dz_eta_allpt"];

        reso_data_d0_phi_lopt[i] = results["reso_data_d0_phi_lopt"];
        reso_data_dz_phi_lopt[i] = results["reso_data_dz_phi_lopt"];
        reso_data_d0_phi_hipt[i] = results["reso_data_d0_phi_hipt"];
        reso_data_dz_phi_hipt[i] = results["reso_data_dz_phi_hipt"];
        reso_data_d0_phi_ulpt[i] = results["reso_data_d0_phi_ulpt"];
        reso_data_dz_phi_ulpt[i] = results["reso_data_dz_phi_ulpt"];
        reso_data_d0_phi_allpt[i] = results["reso_data_d0_phi_allpt"];
        reso_data_dz_phi_allpt[i] = results["reso_data_dz_phi_allpt"];

        reso_mc_d0_pt_loeta[i] = results["reso_mc_d0_pt_loeta"];
        reso_mc_dz_pt_loeta[i] = results["reso_mc_dz_pt_loeta"];
        reso_mc_d0_pt_hieta[i] = results["reso_mc_d0_pt_hieta"];
        reso_mc_dz_pt_hieta[i] = results["reso_mc_dz_pt_hieta"];
        reso_mc_d0_pt_uleta[i] = results["reso_mc_d0_pt_uleta"];
        reso_mc_dz_pt_uleta[i] = results["reso_mc_dz_pt_uleta"];
        reso_mc_d0_pt_alleta[i] = results["reso_mc_d0_pt_alleta"];
        reso_mc_dz_pt_alleta[i] = results["reso_mc_dz_pt_alleta"];

        reso_mc_d0_eta_lopt[i] = results["reso_mc_d0_eta_lopt"];
        reso_mc_dz_eta_lopt[i] = results["reso_mc_dz_eta_lopt"];
        reso_mc_d0_eta_hipt[i] = results["reso_mc_d0_eta_hipt"];
        reso_mc_dz_eta_hipt[i] = results["reso_mc_dz_eta_hipt"];
        reso_mc_d0_eta_ulpt[i] = results["reso_mc_d0_eta_ulpt"];
        reso_mc_dz_eta_ulpt[i] = results["reso_mc_dz_eta_ulpt"];
        reso_mc_d0_eta_allpt[i] = results["reso_mc_d0_eta_allpt"];
        reso_mc_dz_eta_allpt[i] = results["reso_mc_dz_eta_allpt"];

        reso_mc_d0_phi_lopt[i] = results["reso_mc_d0_phi_lopt"];
        reso_mc_dz_phi_lopt[i] = results["reso_mc_dz_phi_lopt"];
        reso_mc_d0_phi_hipt[i] = results["reso_mc_d0_phi_hipt"];
        reso_mc_dz_phi_hipt[i] = results["reso_mc_dz_phi_hipt"];
        reso_mc_d0_phi_ulpt[i] = results["reso_mc_d0_phi_ulpt"];
        reso_mc_dz_phi_ulpt[i] = results["reso_mc_dz_phi_ulpt"];
        reso_mc_d0_phi_allpt[i] = results["reso_mc_d0_phi_allpt"];
        reso_mc_dz_phi_allpt[i] = results["reso_mc_dz_phi_allpt"];

        reso_div_d0_pt_loeta[i] = reso_data_d0_pt_loeta[i] / reso_mc_d0_pt_loeta[i];
        reso_div_dz_pt_loeta[i] = reso_data_dz_pt_loeta[i] / reso_mc_dz_pt_loeta[i];
        reso_div_d0_pt_hieta[i] = reso_data_d0_pt_hieta[i] / reso_mc_d0_pt_hieta[i];
        reso_div_dz_pt_hieta[i] = reso_data_dz_pt_hieta[i] / reso_mc_dz_pt_hieta[i];
        reso_div_d0_pt_uleta[i] = reso_data_d0_pt_uleta[i] / reso_mc_d0_pt_uleta[i];
        reso_div_dz_pt_uleta[i] = reso_data_dz_pt_uleta[i] / reso_mc_dz_pt_uleta[i];
        reso_div_d0_pt_alleta[i] = reso_data_d0_pt_alleta[i] / reso_mc_d0_pt_alleta[i];
        reso_div_dz_pt_alleta[i] = reso_data_dz_pt_alleta[i] / reso_mc_dz_pt_alleta[i];

        reso_div_d0_eta_lopt[i] = reso_data_d0_eta_lopt[i] / reso_mc_d0_eta_lopt[i];
        reso_div_dz_eta_lopt[i] = reso_data_dz_eta_lopt[i] / reso_mc_dz_eta_lopt[i];
        reso_div_d0_eta_hipt[i] = reso_data_d0_eta_hipt[i] / reso_mc_d0_eta_hipt[i];
        reso_div_dz_eta_hipt[i] = reso_data_dz_eta_hipt[i] / reso_mc_dz_eta_hipt[i];
        reso_div_d0_eta_ulpt[i] = reso_data_d0_eta_ulpt[i] / reso_mc_d0_eta_ulpt[i];
        reso_div_dz_eta_ulpt[i] = reso_data_dz_eta_ulpt[i] / reso_mc_dz_eta_ulpt[i];
        reso_div_d0_eta_allpt[i] = reso_data_d0_eta_allpt[i] / reso_mc_d0_eta_allpt[i];
        reso_div_dz_eta_allpt[i] = reso_data_dz_eta_allpt[i] / reso_mc_dz_eta_allpt[i];

        reso_div_d0_phi_lopt[i] = reso_data_d0_phi_lopt[i] / reso_mc_d0_phi_lopt[i];
        reso_div_dz_phi_lopt[i] = reso_data_dz_phi_lopt[i] / reso_mc_dz_phi_lopt[i];
        reso_div_d0_phi_hipt[i] = reso_data_d0_phi_hipt[i] / reso_mc_d0_phi_hipt[i];
        reso_div_dz_phi_hipt[i] = reso_data_dz_phi_hipt[i] / reso_mc_dz_phi_hipt[i];
        reso_div_d0_phi_ulpt[i] = reso_data_d0_phi_ulpt[i] / reso_mc_d0_phi_ulpt[i];
        reso_div_dz_phi_ulpt[i] = reso_data_dz_phi_ulpt[i] / reso_mc_dz_phi_ulpt[i];
        reso_div_d0_phi_allpt[i] = reso_data_d0_phi_allpt[i] / reso_mc_d0_phi_allpt[i];
        reso_div_dz_phi_allpt[i] = reso_data_dz_phi_allpt[i] / reso_mc_dz_phi_allpt[i];

    }

    TGraph * gr_data_d0_pt_loeta = new TGraph(nbins, pt, reso_data_d0_pt_loeta);
    TGraph * gr_data_dz_pt_loeta = new TGraph(nbins, pt, reso_data_dz_pt_loeta);
    TGraph * gr_data_d0_pt_hieta = new TGraph(nbins, pt, reso_data_d0_pt_hieta);
    TGraph * gr_data_dz_pt_hieta = new TGraph(nbins, pt, reso_data_dz_pt_hieta);
    TGraph * gr_data_d0_pt_uleta = new TGraph(nbins, pt, reso_data_d0_pt_uleta);
    TGraph * gr_data_dz_pt_uleta = new TGraph(nbins, pt, reso_data_dz_pt_uleta);
    TGraph * gr_data_d0_pt_alleta = new TGraph(nbins, pt, reso_data_d0_pt_alleta);
    TGraph * gr_data_dz_pt_alleta = new TGraph(nbins, pt, reso_data_dz_pt_alleta);

    TGraph * gr_data_d0_eta_lopt = new TGraph(nbins, eta, reso_data_d0_eta_lopt);
    TGraph * gr_data_dz_eta_lopt = new TGraph(nbins, eta, reso_data_dz_eta_lopt);
    TGraph * gr_data_d0_eta_hipt = new TGraph(nbins, eta, reso_data_d0_eta_hipt);
    TGraph * gr_data_dz_eta_hipt = new TGraph(nbins, eta, reso_data_dz_eta_hipt);
    TGraph * gr_data_d0_eta_ulpt = new TGraph(nbins, eta, reso_data_d0_eta_ulpt);
    TGraph * gr_data_dz_eta_ulpt = new TGraph(nbins, eta, reso_data_dz_eta_ulpt);
    TGraph * gr_data_d0_eta_allpt = new TGraph(nbins, eta, reso_data_d0_eta_allpt);
    TGraph * gr_data_dz_eta_allpt = new TGraph(nbins, eta, reso_data_dz_eta_allpt);

    TGraph * gr_data_d0_phi_lopt = new TGraph(nbins, phi, reso_data_d0_phi_lopt);
    TGraph * gr_data_dz_phi_lopt = new TGraph(nbins, phi, reso_data_dz_phi_lopt);
    TGraph * gr_data_d0_phi_hipt = new TGraph(nbins, phi, reso_data_d0_phi_hipt);
    TGraph * gr_data_dz_phi_hipt = new TGraph(nbins, phi, reso_data_dz_phi_hipt);
    TGraph * gr_data_d0_phi_ulpt = new TGraph(nbins, phi, reso_data_d0_phi_ulpt);
    TGraph * gr_data_dz_phi_ulpt = new TGraph(nbins, phi, reso_data_dz_phi_ulpt);
    TGraph * gr_data_d0_phi_allpt = new TGraph(nbins, phi, reso_data_d0_phi_allpt);
    TGraph * gr_data_dz_phi_allpt = new TGraph(nbins, phi, reso_data_dz_phi_allpt);


    TGraph * gr_mc_d0_pt_loeta = new TGraph(nbins, pt, reso_mc_d0_pt_loeta);
    TGraph * gr_mc_dz_pt_loeta = new TGraph(nbins, pt, reso_mc_dz_pt_loeta);
    TGraph * gr_mc_d0_pt_hieta = new TGraph(nbins, pt, reso_mc_d0_pt_hieta);
    TGraph * gr_mc_dz_pt_hieta = new TGraph(nbins, pt, reso_mc_dz_pt_hieta);
    TGraph * gr_mc_d0_pt_uleta = new TGraph(nbins, pt, reso_mc_d0_pt_uleta);
    TGraph * gr_mc_dz_pt_uleta = new TGraph(nbins, pt, reso_mc_dz_pt_uleta);
    TGraph * gr_mc_d0_pt_alleta = new TGraph(nbins, pt, reso_mc_d0_pt_alleta);
    TGraph * gr_mc_dz_pt_alleta = new TGraph(nbins, pt, reso_mc_dz_pt_alleta);

    TGraph * gr_mc_d0_eta_lopt = new TGraph(nbins, eta, reso_mc_d0_eta_lopt);
    TGraph * gr_mc_dz_eta_lopt = new TGraph(nbins, eta, reso_mc_dz_eta_lopt);
    TGraph * gr_mc_d0_eta_hipt = new TGraph(nbins, eta, reso_mc_d0_eta_hipt);
    TGraph * gr_mc_dz_eta_hipt = new TGraph(nbins, eta, reso_mc_dz_eta_hipt);
    TGraph * gr_mc_d0_eta_ulpt = new TGraph(nbins, eta, reso_mc_d0_eta_ulpt);
    TGraph * gr_mc_dz_eta_ulpt = new TGraph(nbins, eta, reso_mc_dz_eta_ulpt);
    TGraph * gr_mc_d0_eta_allpt = new TGraph(nbins, eta, reso_mc_d0_eta_allpt);
    TGraph * gr_mc_dz_eta_allpt = new TGraph(nbins, eta, reso_mc_dz_eta_allpt);

    TGraph * gr_mc_d0_phi_lopt = new TGraph(nbins, phi, reso_mc_d0_phi_lopt);
    TGraph * gr_mc_dz_phi_lopt = new TGraph(nbins, phi, reso_mc_dz_phi_lopt);
    TGraph * gr_mc_d0_phi_hipt = new TGraph(nbins, phi, reso_mc_d0_phi_hipt);
    TGraph * gr_mc_dz_phi_hipt = new TGraph(nbins, phi, reso_mc_dz_phi_hipt);
    TGraph * gr_mc_d0_phi_ulpt = new TGraph(nbins, phi, reso_mc_d0_phi_ulpt);
    TGraph * gr_mc_dz_phi_ulpt = new TGraph(nbins, phi, reso_mc_dz_phi_ulpt);
    TGraph * gr_mc_d0_phi_allpt = new TGraph(nbins, phi, reso_mc_d0_phi_allpt);
    TGraph * gr_mc_dz_phi_allpt = new TGraph(nbins, phi, reso_mc_dz_phi_allpt);


    TGraph * gr_div_d0_pt_loeta = new TGraph(nbins, pt, reso_div_d0_pt_loeta);
    TGraph * gr_div_dz_pt_loeta = new TGraph(nbins, pt, reso_div_dz_pt_loeta);
    TGraph * gr_div_d0_pt_hieta = new TGraph(nbins, pt, reso_div_d0_pt_hieta);
    TGraph * gr_div_dz_pt_hieta = new TGraph(nbins, pt, reso_div_dz_pt_hieta);
    TGraph * gr_div_d0_pt_uleta = new TGraph(nbins, pt, reso_div_d0_pt_uleta);
    TGraph * gr_div_dz_pt_uleta = new TGraph(nbins, pt, reso_div_dz_pt_uleta);
    TGraph * gr_div_d0_pt_alleta = new TGraph(nbins, pt, reso_div_d0_pt_alleta);
    TGraph * gr_div_dz_pt_alleta = new TGraph(nbins, pt, reso_div_dz_pt_alleta);

    TGraph * gr_div_d0_eta_lopt = new TGraph(nbins, eta, reso_div_d0_eta_lopt);
    TGraph * gr_div_dz_eta_lopt = new TGraph(nbins, eta, reso_div_dz_eta_lopt);
    TGraph * gr_div_d0_eta_hipt = new TGraph(nbins, eta, reso_div_d0_eta_hipt);
    TGraph * gr_div_dz_eta_hipt = new TGraph(nbins, eta, reso_div_dz_eta_hipt);
    TGraph * gr_div_d0_eta_ulpt = new TGraph(nbins, eta, reso_div_d0_eta_ulpt);
    TGraph * gr_div_dz_eta_ulpt = new TGraph(nbins, eta, reso_div_dz_eta_ulpt);
    TGraph * gr_div_d0_eta_allpt = new TGraph(nbins, eta, reso_div_d0_eta_allpt);
    TGraph * gr_div_dz_eta_allpt = new TGraph(nbins, eta, reso_div_dz_eta_allpt);

    TGraph * gr_div_d0_phi_lopt = new TGraph(nbins, phi, reso_div_d0_phi_lopt);
    TGraph * gr_div_dz_phi_lopt = new TGraph(nbins, phi, reso_div_dz_phi_lopt);
    TGraph * gr_div_d0_phi_hipt = new TGraph(nbins, phi, reso_div_d0_phi_hipt);
    TGraph * gr_div_dz_phi_hipt = new TGraph(nbins, phi, reso_div_dz_phi_hipt);
    TGraph * gr_div_d0_phi_ulpt = new TGraph(nbins, phi, reso_div_d0_phi_ulpt);
    TGraph * gr_div_dz_phi_ulpt = new TGraph(nbins, phi, reso_div_dz_phi_ulpt);
    TGraph * gr_div_d0_phi_allpt = new TGraph(nbins, phi, reso_div_d0_phi_allpt);
    TGraph * gr_div_dz_phi_allpt = new TGraph(nbins, phi, reso_div_dz_phi_allpt);

    

    Float_t height_data_d0_pt_loeta = *std::max_element(reso_data_d0_pt_loeta, reso_data_d0_pt_loeta+nbins);
    Float_t height_data_dz_pt_loeta = *std::max_element(reso_data_dz_pt_loeta, reso_data_dz_pt_loeta+nbins);
    Float_t height_data_d0_pt_hieta = *std::max_element(reso_data_d0_pt_hieta, reso_data_d0_pt_hieta+nbins);
    Float_t height_data_dz_pt_hieta = *std::max_element(reso_data_dz_pt_hieta, reso_data_dz_pt_hieta+nbins);
    Float_t height_data_d0_pt_uleta = *std::max_element(reso_data_d0_pt_uleta, reso_data_d0_pt_uleta+nbins);
    Float_t height_data_dz_pt_uleta = *std::max_element(reso_data_dz_pt_uleta, reso_data_dz_pt_uleta+nbins);
    Float_t height_data_d0_pt_alleta = *std::max_element(reso_data_d0_pt_alleta, reso_data_d0_pt_alleta+nbins);
    Float_t height_data_dz_pt_alleta = *std::max_element(reso_data_dz_pt_alleta, reso_data_dz_pt_alleta+nbins);

    Float_t height_data_d0_eta_lopt = *std::max_element(reso_data_d0_eta_lopt, reso_data_d0_eta_lopt+nbins);
    Float_t height_data_dz_eta_lopt = *std::max_element(reso_data_dz_eta_lopt, reso_data_dz_eta_lopt+nbins);
    Float_t height_data_d0_eta_hipt = *std::max_element(reso_data_d0_eta_hipt, reso_data_d0_eta_hipt+nbins);
    Float_t height_data_dz_eta_hipt = *std::max_element(reso_data_dz_eta_hipt, reso_data_dz_eta_hipt+nbins);
    Float_t height_data_d0_eta_ulpt = *std::max_element(reso_data_d0_eta_ulpt, reso_data_d0_eta_ulpt+nbins);
    Float_t height_data_dz_eta_ulpt = *std::max_element(reso_data_dz_eta_ulpt, reso_data_dz_eta_ulpt+nbins);
    Float_t height_data_d0_eta_allpt = *std::max_element(reso_data_d0_eta_allpt, reso_data_d0_eta_allpt+nbins);
    Float_t height_data_dz_eta_allpt = *std::max_element(reso_data_dz_eta_allpt, reso_data_dz_eta_allpt+nbins);

    Float_t height_data_d0_phi_lopt = *std::max_element(reso_data_d0_phi_lopt, reso_data_d0_phi_lopt+nbins);
    Float_t height_data_dz_phi_lopt = *std::max_element(reso_data_dz_phi_lopt, reso_data_dz_phi_lopt+nbins);
    Float_t height_data_d0_phi_hipt = *std::max_element(reso_data_d0_phi_hipt, reso_data_d0_phi_hipt+nbins);
    Float_t height_data_dz_phi_hipt = *std::max_element(reso_data_dz_phi_hipt, reso_data_dz_phi_hipt+nbins);
    Float_t height_data_d0_phi_ulpt = *std::max_element(reso_data_d0_phi_ulpt, reso_data_d0_phi_ulpt+nbins);
    Float_t height_data_dz_phi_ulpt = *std::max_element(reso_data_dz_phi_ulpt, reso_data_dz_phi_ulpt+nbins);
    Float_t height_data_d0_phi_allpt = *std::max_element(reso_data_d0_phi_allpt, reso_data_d0_phi_allpt+nbins);
    Float_t height_data_dz_phi_allpt = *std::max_element(reso_data_dz_phi_allpt, reso_data_dz_phi_allpt+nbins);

    Float_t height_mc_d0_pt_loeta = *std::max_element(reso_mc_d0_pt_loeta, reso_mc_d0_pt_loeta+nbins);
    Float_t height_mc_dz_pt_loeta = *std::max_element(reso_mc_dz_pt_loeta, reso_mc_dz_pt_loeta+nbins);
    Float_t height_mc_d0_pt_hieta = *std::max_element(reso_mc_d0_pt_hieta, reso_mc_d0_pt_hieta+nbins);
    Float_t height_mc_dz_pt_hieta = *std::max_element(reso_mc_dz_pt_hieta, reso_mc_dz_pt_hieta+nbins);
    Float_t height_mc_d0_pt_uleta = *std::max_element(reso_mc_d0_pt_uleta, reso_mc_d0_pt_uleta+nbins);
    Float_t height_mc_dz_pt_uleta = *std::max_element(reso_mc_dz_pt_uleta, reso_mc_dz_pt_uleta+nbins);
    Float_t height_mc_d0_pt_alleta = *std::max_element(reso_mc_d0_pt_alleta, reso_mc_d0_pt_alleta+nbins);
    Float_t height_mc_dz_pt_alleta = *std::max_element(reso_mc_dz_pt_alleta, reso_mc_dz_pt_alleta+nbins);

    Float_t height_mc_d0_eta_lopt = *std::max_element(reso_mc_d0_eta_lopt, reso_mc_d0_eta_lopt+nbins);
    Float_t height_mc_dz_eta_lopt = *std::max_element(reso_mc_dz_eta_lopt, reso_mc_dz_eta_lopt+nbins);
    Float_t height_mc_d0_eta_hipt = *std::max_element(reso_mc_d0_eta_hipt, reso_mc_d0_eta_hipt+nbins);
    Float_t height_mc_dz_eta_hipt = *std::max_element(reso_mc_dz_eta_hipt, reso_mc_dz_eta_hipt+nbins);
    Float_t height_mc_d0_eta_ulpt = *std::max_element(reso_mc_d0_eta_ulpt, reso_mc_d0_eta_ulpt+nbins);
    Float_t height_mc_dz_eta_ulpt = *std::max_element(reso_mc_dz_eta_ulpt, reso_mc_dz_eta_ulpt+nbins);
    Float_t height_mc_d0_eta_allpt = *std::max_element(reso_mc_d0_eta_allpt, reso_mc_d0_eta_allpt+nbins);
    Float_t height_mc_dz_eta_allpt = *std::max_element(reso_mc_dz_eta_allpt, reso_mc_dz_eta_allpt+nbins);

    Float_t height_mc_d0_phi_lopt = *std::max_element(reso_mc_d0_phi_lopt, reso_mc_d0_phi_lopt+nbins);
    Float_t height_mc_dz_phi_lopt = *std::max_element(reso_mc_dz_phi_lopt, reso_mc_dz_phi_lopt+nbins);
    Float_t height_mc_d0_phi_hipt = *std::max_element(reso_mc_d0_phi_hipt, reso_mc_d0_phi_hipt+nbins);
    Float_t height_mc_dz_phi_hipt = *std::max_element(reso_mc_dz_phi_hipt, reso_mc_dz_phi_hipt+nbins);
    Float_t height_mc_d0_phi_ulpt = *std::max_element(reso_mc_d0_phi_ulpt, reso_mc_d0_phi_ulpt+nbins);
    Float_t height_mc_dz_phi_ulpt = *std::max_element(reso_mc_dz_phi_ulpt, reso_mc_dz_phi_ulpt+nbins);
    Float_t height_mc_d0_phi_allpt = *std::max_element(reso_mc_d0_phi_allpt, reso_mc_d0_phi_allpt+nbins);
    Float_t height_mc_dz_phi_allpt = *std::max_element(reso_mc_dz_phi_allpt, reso_mc_dz_phi_allpt+nbins);

    Float_t height_d0_pt_loeta = std::max(height_data_d0_pt_loeta, height_mc_d0_pt_loeta);
    Float_t height_dz_pt_loeta = std::max(height_data_dz_pt_loeta, height_mc_dz_pt_loeta);
    Float_t height_d0_pt_hieta = std::max(height_data_d0_pt_hieta, height_mc_d0_pt_hieta);
    Float_t height_dz_pt_hieta = std::max(height_data_dz_pt_hieta, height_mc_dz_pt_hieta);
    Float_t height_d0_pt_uleta = std::max(height_data_d0_pt_uleta, height_mc_d0_pt_uleta);
    Float_t height_dz_pt_uleta = std::max(height_data_dz_pt_uleta, height_mc_dz_pt_uleta);
    Float_t height_d0_pt_alleta = std::max(height_data_d0_pt_alleta, height_mc_d0_pt_alleta);
    Float_t height_dz_pt_alleta = std::max(height_data_dz_pt_alleta, height_mc_dz_pt_alleta);

    Float_t height_d0_eta_lopt = std::max(height_data_d0_eta_lopt, height_mc_d0_eta_lopt);
    Float_t height_dz_eta_lopt = std::max(height_data_dz_eta_lopt, height_mc_dz_eta_lopt);
    Float_t height_d0_eta_hipt = std::max(height_data_d0_eta_hipt, height_mc_d0_eta_hipt);
    Float_t height_dz_eta_hipt = std::max(height_data_dz_eta_hipt, height_mc_dz_eta_hipt);
    Float_t height_d0_eta_ulpt = std::max(height_data_d0_eta_ulpt, height_mc_d0_eta_ulpt);
    Float_t height_dz_eta_ulpt = std::max(height_data_dz_eta_ulpt, height_mc_dz_eta_ulpt);
    Float_t height_d0_eta_allpt = std::max(height_data_d0_eta_allpt, height_mc_d0_eta_allpt);
    Float_t height_dz_eta_allpt = std::max(height_data_dz_eta_allpt, height_mc_dz_eta_allpt);

    Float_t height_d0_phi_lopt = std::max(height_data_d0_phi_lopt, height_mc_d0_phi_lopt);
    Float_t height_dz_phi_lopt = std::max(height_data_dz_phi_lopt, height_mc_dz_phi_lopt);
    Float_t height_d0_phi_hipt = std::max(height_data_d0_phi_hipt, height_mc_d0_phi_hipt);
    Float_t height_dz_phi_hipt = std::max(height_data_dz_phi_hipt, height_mc_dz_phi_hipt);
    Float_t height_d0_phi_ulpt = std::max(height_data_d0_phi_ulpt, height_mc_d0_phi_ulpt);
    Float_t height_dz_phi_ulpt = std::max(height_data_dz_phi_ulpt, height_mc_dz_phi_ulpt);
    Float_t height_d0_phi_allpt = std::max(height_data_d0_phi_allpt, height_mc_d0_phi_allpt);
    Float_t height_dz_phi_allpt = std::max(height_data_dz_phi_allpt, height_mc_dz_phi_allpt);

    
    Float_t floor_data_d0_pt_loeta = *std::min_element(reso_data_d0_pt_loeta, reso_data_d0_pt_loeta+nbins);
    Float_t floor_data_dz_pt_loeta = *std::min_element(reso_data_dz_pt_loeta, reso_data_dz_pt_loeta+nbins);
    Float_t floor_data_d0_pt_hieta = *std::min_element(reso_data_d0_pt_hieta, reso_data_d0_pt_hieta+nbins);
    Float_t floor_data_dz_pt_hieta = *std::min_element(reso_data_dz_pt_hieta, reso_data_dz_pt_hieta+nbins);
    Float_t floor_data_d0_pt_uleta = *std::min_element(reso_data_d0_pt_uleta, reso_data_d0_pt_uleta+nbins);
    Float_t floor_data_dz_pt_uleta = *std::min_element(reso_data_dz_pt_uleta, reso_data_dz_pt_uleta+nbins);
    Float_t floor_data_d0_pt_alleta = *std::min_element(reso_data_d0_pt_alleta, reso_data_d0_pt_alleta+nbins);
    Float_t floor_data_dz_pt_alleta = *std::min_element(reso_data_dz_pt_alleta, reso_data_dz_pt_alleta+nbins);

    Float_t floor_data_d0_eta_lopt = *std::min_element(reso_data_d0_eta_lopt, reso_data_d0_eta_lopt+nbins);
    Float_t floor_data_dz_eta_lopt = *std::min_element(reso_data_dz_eta_lopt, reso_data_dz_eta_lopt+nbins);
    Float_t floor_data_d0_eta_hipt = *std::min_element(reso_data_d0_eta_hipt, reso_data_d0_eta_hipt+nbins);
    Float_t floor_data_dz_eta_hipt = *std::min_element(reso_data_dz_eta_hipt, reso_data_dz_eta_hipt+nbins);
    Float_t floor_data_d0_eta_ulpt = *std::min_element(reso_data_d0_eta_ulpt, reso_data_d0_eta_ulpt+nbins);
    Float_t floor_data_dz_eta_ulpt = *std::min_element(reso_data_dz_eta_ulpt, reso_data_dz_eta_ulpt+nbins);
    Float_t floor_data_d0_eta_allpt = *std::min_element(reso_data_d0_eta_allpt, reso_data_d0_eta_allpt+nbins);
    Float_t floor_data_dz_eta_allpt = *std::min_element(reso_data_dz_eta_allpt, reso_data_dz_eta_allpt+nbins);

    Float_t floor_data_d0_phi_lopt = *std::min_element(reso_data_d0_phi_lopt, reso_data_d0_phi_lopt+nbins);
    Float_t floor_data_dz_phi_lopt = *std::min_element(reso_data_dz_phi_lopt, reso_data_dz_phi_lopt+nbins);
    Float_t floor_data_d0_phi_hipt = *std::min_element(reso_data_d0_phi_hipt, reso_data_d0_phi_hipt+nbins);
    Float_t floor_data_dz_phi_hipt = *std::min_element(reso_data_dz_phi_hipt, reso_data_dz_phi_hipt+nbins);
    Float_t floor_data_d0_phi_ulpt = *std::min_element(reso_data_d0_phi_ulpt, reso_data_d0_phi_ulpt+nbins);
    Float_t floor_data_dz_phi_ulpt = *std::min_element(reso_data_dz_phi_ulpt, reso_data_dz_phi_ulpt+nbins);
    Float_t floor_data_d0_phi_allpt = *std::min_element(reso_data_d0_phi_allpt, reso_data_d0_phi_allpt+nbins);
    Float_t floor_data_dz_phi_allpt = *std::min_element(reso_data_dz_phi_allpt, reso_data_dz_phi_allpt+nbins);

    Float_t floor_mc_d0_pt_loeta = *std::min_element(reso_mc_d0_pt_loeta, reso_mc_d0_pt_loeta+nbins);
    Float_t floor_mc_dz_pt_loeta = *std::min_element(reso_mc_dz_pt_loeta, reso_mc_dz_pt_loeta+nbins);
    Float_t floor_mc_d0_pt_hieta = *std::min_element(reso_mc_d0_pt_hieta, reso_mc_d0_pt_hieta+nbins);
    Float_t floor_mc_dz_pt_hieta = *std::min_element(reso_mc_dz_pt_hieta, reso_mc_dz_pt_hieta+nbins);
    Float_t floor_mc_d0_pt_uleta = *std::min_element(reso_mc_d0_pt_uleta, reso_mc_d0_pt_uleta+nbins);
    Float_t floor_mc_dz_pt_uleta = *std::min_element(reso_mc_dz_pt_uleta, reso_mc_dz_pt_uleta+nbins);
    Float_t floor_mc_d0_pt_alleta = *std::min_element(reso_mc_d0_pt_alleta, reso_mc_d0_pt_alleta+nbins);
    Float_t floor_mc_dz_pt_alleta = *std::min_element(reso_mc_dz_pt_alleta, reso_mc_dz_pt_alleta+nbins);

    Float_t floor_mc_d0_eta_lopt = *std::min_element(reso_mc_d0_eta_lopt, reso_mc_d0_eta_lopt+nbins);
    Float_t floor_mc_dz_eta_lopt = *std::min_element(reso_mc_dz_eta_lopt, reso_mc_dz_eta_lopt+nbins);
    Float_t floor_mc_d0_eta_hipt = *std::min_element(reso_mc_d0_eta_hipt, reso_mc_d0_eta_hipt+nbins);
    Float_t floor_mc_dz_eta_hipt = *std::min_element(reso_mc_dz_eta_hipt, reso_mc_dz_eta_hipt+nbins);
    Float_t floor_mc_d0_eta_ulpt = *std::min_element(reso_mc_d0_eta_ulpt, reso_mc_d0_eta_ulpt+nbins);
    Float_t floor_mc_dz_eta_ulpt = *std::min_element(reso_mc_dz_eta_ulpt, reso_mc_dz_eta_ulpt+nbins);
    Float_t floor_mc_d0_eta_allpt = *std::min_element(reso_mc_d0_eta_allpt, reso_mc_d0_eta_allpt+nbins);
    Float_t floor_mc_dz_eta_allpt = *std::min_element(reso_mc_dz_eta_allpt, reso_mc_dz_eta_allpt+nbins);

    Float_t floor_mc_d0_phi_lopt = *std::min_element(reso_mc_d0_phi_lopt, reso_mc_d0_phi_lopt+nbins);
    Float_t floor_mc_dz_phi_lopt = *std::min_element(reso_mc_dz_phi_lopt, reso_mc_dz_phi_lopt+nbins);
    Float_t floor_mc_d0_phi_hipt = *std::min_element(reso_mc_d0_phi_hipt, reso_mc_d0_phi_hipt+nbins);
    Float_t floor_mc_dz_phi_hipt = *std::min_element(reso_mc_dz_phi_hipt, reso_mc_dz_phi_hipt+nbins);
    Float_t floor_mc_d0_phi_ulpt = *std::min_element(reso_mc_d0_phi_ulpt, reso_mc_d0_phi_ulpt+nbins);
    Float_t floor_mc_dz_phi_ulpt = *std::min_element(reso_mc_dz_phi_ulpt, reso_mc_dz_phi_ulpt+nbins);
    Float_t floor_mc_d0_phi_allpt = *std::min_element(reso_mc_d0_phi_allpt, reso_mc_d0_phi_allpt+nbins);
    Float_t floor_mc_dz_phi_allpt = *std::min_element(reso_mc_dz_phi_allpt, reso_mc_dz_phi_allpt+nbins);

    Float_t floor_d0_pt_loeta = std::min(floor_data_d0_pt_loeta, floor_mc_d0_pt_loeta);
    Float_t floor_dz_pt_loeta = std::min(floor_data_dz_pt_loeta, floor_mc_dz_pt_loeta);
    Float_t floor_d0_pt_hieta = std::min(floor_data_d0_pt_hieta, floor_mc_d0_pt_hieta);
    Float_t floor_dz_pt_hieta = std::min(floor_data_dz_pt_hieta, floor_mc_dz_pt_hieta);
    Float_t floor_d0_pt_uleta = std::min(floor_data_d0_pt_uleta, floor_mc_d0_pt_uleta);
    Float_t floor_dz_pt_uleta = std::min(floor_data_dz_pt_uleta, floor_mc_dz_pt_uleta);
    Float_t floor_d0_pt_alleta = std::min(floor_data_d0_pt_alleta, floor_mc_d0_pt_alleta);
    Float_t floor_dz_pt_alleta = std::min(floor_data_dz_pt_alleta, floor_mc_dz_pt_alleta);

    Float_t floor_d0_eta_lopt = std::min(floor_data_d0_eta_lopt, floor_mc_d0_eta_lopt);
    Float_t floor_dz_eta_lopt = std::min(floor_data_dz_eta_lopt, floor_mc_dz_eta_lopt);
    Float_t floor_d0_eta_hipt = std::min(floor_data_d0_eta_hipt, floor_mc_d0_eta_hipt);
    Float_t floor_dz_eta_hipt = std::min(floor_data_dz_eta_hipt, floor_mc_dz_eta_hipt);
    Float_t floor_d0_eta_ulpt = std::min(floor_data_d0_eta_ulpt, floor_mc_d0_eta_ulpt);
    Float_t floor_dz_eta_ulpt = std::min(floor_data_dz_eta_ulpt, floor_mc_dz_eta_ulpt);
    Float_t floor_d0_eta_allpt = std::min(floor_data_d0_eta_allpt, floor_mc_d0_eta_allpt);
    Float_t floor_dz_eta_allpt = std::min(floor_data_dz_eta_allpt, floor_mc_dz_eta_allpt);

    Float_t floor_d0_phi_lopt = std::min(floor_data_d0_phi_lopt, floor_mc_d0_phi_lopt);
    Float_t floor_dz_phi_lopt = std::min(floor_data_dz_phi_lopt, floor_mc_dz_phi_lopt);
    Float_t floor_d0_phi_hipt = std::min(floor_data_d0_phi_hipt, floor_mc_d0_phi_hipt);
    Float_t floor_dz_phi_hipt = std::min(floor_data_dz_phi_hipt, floor_mc_dz_phi_hipt);
    Float_t floor_d0_phi_ulpt = std::min(floor_data_d0_phi_ulpt, floor_mc_d0_phi_ulpt);
    Float_t floor_dz_phi_ulpt = std::min(floor_data_dz_phi_ulpt, floor_mc_dz_phi_ulpt);
    Float_t floor_d0_phi_allpt = std::min(floor_data_d0_phi_allpt, floor_mc_d0_phi_allpt);
    Float_t floor_dz_phi_allpt = std::min(floor_data_dz_phi_allpt, floor_mc_dz_phi_allpt);


    Float_t height_div_d0_pt_loeta = *std::max_element(reso_div_d0_pt_loeta, reso_div_d0_pt_loeta+nbins);
    Float_t height_div_dz_pt_loeta = *std::max_element(reso_div_dz_pt_loeta, reso_div_dz_pt_loeta+nbins);
    Float_t height_div_d0_pt_hieta = *std::max_element(reso_div_d0_pt_hieta, reso_div_d0_pt_hieta+nbins);
    Float_t height_div_dz_pt_hieta = *std::max_element(reso_div_dz_pt_hieta, reso_div_dz_pt_hieta+nbins);
    Float_t height_div_d0_pt_uleta = *std::max_element(reso_div_d0_pt_uleta, reso_div_d0_pt_uleta+nbins);
    Float_t height_div_dz_pt_uleta = *std::max_element(reso_div_dz_pt_uleta, reso_div_dz_pt_uleta+nbins);
    Float_t height_div_d0_pt_alleta = *std::max_element(reso_div_d0_pt_alleta, reso_div_d0_pt_alleta+nbins);
    Float_t height_div_dz_pt_alleta = *std::max_element(reso_div_dz_pt_alleta, reso_div_dz_pt_alleta+nbins);

    Float_t height_div_d0_eta_lopt = *std::max_element(reso_div_d0_eta_lopt, reso_div_d0_eta_lopt+nbins);
    Float_t height_div_dz_eta_lopt = *std::max_element(reso_div_dz_eta_lopt, reso_div_dz_eta_lopt+nbins);
    Float_t height_div_d0_eta_hipt = *std::max_element(reso_div_d0_eta_hipt, reso_div_d0_eta_hipt+nbins);
    Float_t height_div_dz_eta_hipt = *std::max_element(reso_div_dz_eta_hipt, reso_div_dz_eta_hipt+nbins);
    Float_t height_div_d0_eta_ulpt = *std::max_element(reso_div_d0_eta_ulpt, reso_div_d0_eta_ulpt+nbins);
    Float_t height_div_dz_eta_ulpt = *std::max_element(reso_div_dz_eta_ulpt, reso_div_dz_eta_ulpt+nbins);
    Float_t height_div_d0_eta_allpt = *std::max_element(reso_div_d0_eta_allpt, reso_div_d0_eta_allpt+nbins);
    Float_t height_div_dz_eta_allpt = *std::max_element(reso_div_dz_eta_allpt, reso_div_dz_eta_allpt+nbins);

    Float_t height_div_d0_phi_lopt = *std::max_element(reso_div_d0_phi_lopt, reso_div_d0_phi_lopt+nbins);
    Float_t height_div_dz_phi_lopt = *std::max_element(reso_div_dz_phi_lopt, reso_div_dz_phi_lopt+nbins);
    Float_t height_div_d0_phi_hipt = *std::max_element(reso_div_d0_phi_hipt, reso_div_d0_phi_hipt+nbins);
    Float_t height_div_dz_phi_hipt = *std::max_element(reso_div_dz_phi_hipt, reso_div_dz_phi_hipt+nbins);
    Float_t height_div_d0_phi_ulpt = *std::max_element(reso_div_d0_phi_ulpt, reso_div_d0_phi_ulpt+nbins);
    Float_t height_div_dz_phi_ulpt = *std::max_element(reso_div_dz_phi_ulpt, reso_div_dz_phi_ulpt+nbins);
    Float_t height_div_d0_phi_allpt = *std::max_element(reso_div_d0_phi_allpt, reso_div_d0_phi_allpt+nbins);
    Float_t height_div_dz_phi_allpt = *std::max_element(reso_div_dz_phi_allpt, reso_div_dz_phi_allpt+nbins);

    Float_t floor_div_d0_pt_loeta = *std::min_element(reso_div_d0_pt_loeta, reso_div_d0_pt_loeta+nbins);
    Float_t floor_div_dz_pt_loeta = *std::min_element(reso_div_dz_pt_loeta, reso_div_dz_pt_loeta+nbins);
    Float_t floor_div_d0_pt_hieta = *std::min_element(reso_div_d0_pt_hieta, reso_div_d0_pt_hieta+nbins);
    Float_t floor_div_dz_pt_hieta = *std::min_element(reso_div_dz_pt_hieta, reso_div_dz_pt_hieta+nbins);
    Float_t floor_div_d0_pt_uleta = *std::min_element(reso_div_d0_pt_uleta, reso_div_d0_pt_uleta+nbins);
    Float_t floor_div_dz_pt_uleta = *std::min_element(reso_div_dz_pt_uleta, reso_div_dz_pt_uleta+nbins);
    Float_t floor_div_d0_pt_alleta = *std::min_element(reso_div_d0_pt_alleta, reso_div_d0_pt_alleta+nbins);
    Float_t floor_div_dz_pt_alleta = *std::min_element(reso_div_dz_pt_alleta, reso_div_dz_pt_alleta+nbins);

    Float_t floor_div_d0_eta_lopt = *std::min_element(reso_div_d0_eta_lopt, reso_div_d0_eta_lopt+nbins);
    Float_t floor_div_dz_eta_lopt = *std::min_element(reso_div_dz_eta_lopt, reso_div_dz_eta_lopt+nbins);
    Float_t floor_div_d0_eta_hipt = *std::min_element(reso_div_d0_eta_hipt, reso_div_d0_eta_hipt+nbins);
    Float_t floor_div_dz_eta_hipt = *std::min_element(reso_div_dz_eta_hipt, reso_div_dz_eta_hipt+nbins);
    Float_t floor_div_d0_eta_ulpt = *std::min_element(reso_div_d0_eta_ulpt, reso_div_d0_eta_ulpt+nbins);
    Float_t floor_div_dz_eta_ulpt = *std::min_element(reso_div_dz_eta_ulpt, reso_div_dz_eta_ulpt+nbins);
    Float_t floor_div_d0_eta_allpt = *std::min_element(reso_div_d0_eta_allpt, reso_div_d0_eta_allpt+nbins);
    Float_t floor_div_dz_eta_allpt = *std::min_element(reso_div_dz_eta_allpt, reso_div_dz_eta_allpt+nbins);

    Float_t floor_div_d0_phi_lopt = *std::min_element(reso_div_d0_phi_lopt, reso_div_d0_phi_lopt+nbins);
    Float_t floor_div_dz_phi_lopt = *std::min_element(reso_div_dz_phi_lopt, reso_div_dz_phi_lopt+nbins);
    Float_t floor_div_d0_phi_hipt = *std::min_element(reso_div_d0_phi_hipt, reso_div_d0_phi_hipt+nbins);
    Float_t floor_div_dz_phi_hipt = *std::min_element(reso_div_dz_phi_hipt, reso_div_dz_phi_hipt+nbins);
    Float_t floor_div_d0_phi_ulpt = *std::min_element(reso_div_d0_phi_ulpt, reso_div_d0_phi_ulpt+nbins);
    Float_t floor_div_dz_phi_ulpt = *std::min_element(reso_div_dz_phi_ulpt, reso_div_dz_phi_ulpt+nbins);
    Float_t floor_div_d0_phi_allpt = *std::min_element(reso_div_d0_phi_allpt, reso_div_d0_phi_allpt+nbins);
    Float_t floor_div_dz_phi_allpt = *std::min_element(reso_div_dz_phi_allpt, reso_div_dz_phi_allpt+nbins);

    compare_gr(gr_data_d0_pt_loeta, gr_mc_d0_pt_loeta, gr_div_d0_pt_loeta, height_d0_pt_loeta, floor_d0_pt_loeta, pt[0], pt[nbins-1], height_div_d0_pt_loeta, floor_div_d0_pt_loeta, period, "#splitline{"+datatype_text+"}{|#it{#eta}|<1.3}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_loeta");
    compare_gr(gr_data_d0_pt_hieta, gr_mc_d0_pt_hieta, gr_div_d0_pt_hieta, height_d0_pt_hieta, floor_d0_pt_hieta, pt[0], pt[nbins-1], height_div_d0_pt_hieta, floor_div_d0_pt_hieta, period, "#splitline{"+datatype_text+"}{1.3<|#it{#eta}|<2.5}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_hieta");
    compare_gr(gr_data_d0_pt_uleta, gr_mc_d0_pt_uleta, gr_div_d0_pt_uleta, height_d0_pt_uleta, floor_d0_pt_uleta, pt[0], pt[nbins-1], height_div_d0_pt_uleta, floor_div_d0_pt_uleta, period, "#splitline{"+datatype_text+"}{2.5<|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_uleta");
    compare_gr(gr_data_d0_pt_alleta, gr_mc_d0_pt_alleta, gr_div_d0_pt_alleta, height_d0_pt_alleta, floor_d0_pt_alleta, pt[0], pt[nbins-1], height_div_d0_pt_alleta, floor_div_d0_pt_alleta, period, "#splitline{"+datatype_text+"}{|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_alleta");

    compare_gr(gr_data_d0_eta_lopt, gr_mc_d0_eta_lopt, gr_div_d0_eta_lopt, height_d0_eta_lopt, floor_d0_eta_lopt, eta[0], eta[nbins-1], height_div_d0_eta_lopt, floor_div_d0_eta_lopt, period, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_lopt");
    compare_gr(gr_data_d0_eta_hipt, gr_mc_d0_eta_hipt, gr_div_d0_eta_hipt, height_d0_eta_hipt, floor_d0_eta_hipt, eta[0], eta[nbins-1], height_div_d0_eta_hipt, floor_div_d0_eta_hipt, period, "#splitline{"+datatype_text+"}{1<#it{p_{T}}<3 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_hipt");
    compare_gr(gr_data_d0_eta_ulpt, gr_mc_d0_eta_ulpt, gr_div_d0_eta_ulpt, height_d0_eta_ulpt, floor_d0_eta_ulpt, eta[0], eta[nbins-1], height_div_d0_eta_ulpt, floor_div_d0_eta_ulpt, period, "#splitline{"+datatype_text+"}{3<#it{p_{T}}<10 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_ulpt");
    compare_gr(gr_data_d0_eta_allpt, gr_mc_d0_eta_allpt, gr_div_d0_eta_allpt, height_d0_eta_allpt, floor_d0_eta_allpt, eta[0], eta[nbins-1], height_div_d0_eta_allpt, floor_div_d0_eta_allpt, period, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<10 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_allpt");

    compare_gr(gr_data_d0_phi_lopt, gr_mc_d0_phi_lopt, gr_div_d0_phi_lopt, height_d0_phi_lopt, floor_d0_phi_lopt, phi[0], phi[nbins-1], height_div_d0_phi_lopt, floor_div_d0_phi_lopt, period, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_lopt");
    compare_gr(gr_data_d0_phi_hipt, gr_mc_d0_phi_hipt, gr_div_d0_phi_hipt, height_d0_phi_hipt, floor_d0_phi_hipt, phi[0], phi[nbins-1], height_div_d0_phi_hipt, floor_div_d0_phi_hipt, period, "#splitline{"+datatype_text+"}{1<#it{p_{T}}<3 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_hipt");
    compare_gr(gr_data_d0_phi_ulpt, gr_mc_d0_phi_ulpt, gr_div_d0_phi_ulpt, height_d0_phi_ulpt, floor_d0_phi_ulpt, phi[0], phi[nbins-1], height_div_d0_phi_ulpt, floor_div_d0_phi_ulpt, period, "#splitline{"+datatype_text+"}{3<#it{p_{T}}<10 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_ulpt");
    compare_gr(gr_data_d0_phi_allpt, gr_mc_d0_phi_allpt, gr_div_d0_phi_allpt, height_d0_phi_allpt, floor_d0_phi_allpt, phi[0], phi[nbins-1], height_div_d0_phi_allpt, floor_div_d0_phi_allpt, period, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<10 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_allpt");

    compare_gr(gr_data_dz_pt_loeta, gr_mc_dz_pt_loeta, gr_div_dz_pt_loeta, height_dz_pt_loeta, floor_dz_pt_loeta, pt[0], pt[nbins-1], height_div_dz_pt_loeta, floor_div_dz_pt_loeta, period, "#splitline{"+datatype_text+"}{|#it{#eta}|<1.3}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_loeta");
    compare_gr(gr_data_dz_pt_hieta, gr_mc_dz_pt_hieta, gr_div_dz_pt_hieta, height_dz_pt_hieta, floor_dz_pt_hieta, pt[0], pt[nbins-1], height_div_dz_pt_hieta, floor_div_dz_pt_hieta, period, "#splitline{"+datatype_text+"}{1.3<|#it{#eta}|<2.5}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_hieta");
    compare_gr(gr_data_dz_pt_uleta, gr_mc_dz_pt_uleta, gr_div_dz_pt_uleta, height_dz_pt_uleta, floor_dz_pt_uleta, pt[0], pt[nbins-1], height_div_dz_pt_uleta, floor_div_dz_pt_uleta, period, "#splitline{"+datatype_text+"}{2.5<|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_uleta");
    compare_gr(gr_data_dz_pt_alleta, gr_mc_dz_pt_alleta, gr_div_dz_pt_alleta, height_dz_pt_alleta, floor_dz_pt_alleta, pt[0], pt[nbins-1], height_div_dz_pt_alleta, floor_div_dz_pt_alleta, period, "#splitline{"+datatype_text+"}{|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_alleta");

    compare_gr(gr_data_dz_eta_lopt, gr_mc_dz_eta_lopt, gr_div_dz_eta_lopt, height_dz_eta_lopt, floor_dz_eta_lopt, eta[0], eta[nbins-1], height_div_dz_eta_lopt, floor_div_dz_eta_lopt, period, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_lopt");
    compare_gr(gr_data_dz_eta_hipt, gr_mc_dz_eta_hipt, gr_div_dz_eta_hipt, height_dz_eta_hipt, floor_dz_eta_hipt, eta[0], eta[nbins-1], height_div_dz_eta_hipt, floor_div_dz_eta_hipt, period, "#splitline{"+datatype_text+"}{1<#it{p_{T}}<3 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_hipt");
    compare_gr(gr_data_dz_eta_ulpt, gr_mc_dz_eta_ulpt, gr_div_dz_eta_ulpt, height_dz_eta_ulpt, floor_dz_eta_ulpt, eta[0], eta[nbins-1], height_div_dz_eta_ulpt, floor_div_dz_eta_ulpt, period, "#splitline{"+datatype_text+"}{3<#it{p_{T}}<10 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_ulpt");
    compare_gr(gr_data_dz_eta_allpt, gr_mc_dz_eta_allpt, gr_div_dz_eta_allpt, height_dz_eta_allpt, floor_dz_eta_allpt, eta[0], eta[nbins-1], height_div_dz_eta_allpt, floor_div_dz_eta_allpt, period, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<10 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_allpt");

    compare_gr(gr_data_dz_phi_lopt, gr_mc_dz_phi_lopt, gr_div_dz_phi_lopt, height_dz_phi_lopt, floor_dz_phi_lopt, phi[0], phi[nbins-1], height_div_dz_phi_lopt, floor_div_dz_phi_lopt, period, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<1 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_lopt");
    compare_gr(gr_data_dz_phi_hipt, gr_mc_dz_phi_hipt, gr_div_dz_phi_hipt, height_dz_phi_hipt, floor_dz_phi_hipt, phi[0], phi[nbins-1], height_div_dz_phi_hipt, floor_div_dz_phi_hipt, period, "#splitline{"+datatype_text+"}{1<#it{p_{T}}<3 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_hipt");
    compare_gr(gr_data_dz_phi_ulpt, gr_mc_dz_phi_ulpt, gr_div_dz_phi_ulpt, height_dz_phi_ulpt, floor_dz_phi_ulpt, phi[0], phi[nbins-1], height_div_dz_phi_ulpt, floor_div_dz_phi_ulpt, period, "#splitline{"+datatype_text+"}{3<#it{p_{T}}<10 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_ulpt");
    compare_gr(gr_data_dz_phi_allpt, gr_mc_dz_phi_allpt, gr_div_dz_phi_allpt, height_dz_phi_allpt, floor_dz_phi_allpt, phi[0], phi[nbins-1], height_div_dz_phi_allpt, floor_div_dz_phi_allpt, period, "#splitline{"+datatype_text+"}{0.1<#it{p_{T}}<10 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_allpt");

    return 0;
}
