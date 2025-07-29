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

const TString figdir = "./figures/JetHT/ip_res_analysis/";
const TString jsondir = "./json/JetHT/ip_res/";
const int nbins = 100;

int draw_ip_res_JetHT(){

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, 2022BC";

    float eta[nbins];
    float phi[nbins];
    float pt[nbins];

    float reso_d0_pv_pt_loeta[nbins];
    float reso_dz_pv_pt_loeta[nbins];
    float reso_d0_bs_pt_loeta[nbins];
    float reso_dz_bs_pt_loeta[nbins];
    float reso_d0_pv_pt_hieta[nbins];
    float reso_dz_pv_pt_hieta[nbins];
    float reso_d0_bs_pt_hieta[nbins];
    float reso_dz_bs_pt_hieta[nbins];

    float reso_d0_pv_eta_lopt[nbins];
    float reso_dz_pv_eta_lopt[nbins];
    float reso_d0_bs_eta_lopt[nbins];
    float reso_dz_bs_eta_lopt[nbins];
    float reso_d0_pv_eta_hipt[nbins];
    float reso_dz_pv_eta_hipt[nbins];
    float reso_d0_bs_eta_hipt[nbins];
    float reso_dz_bs_eta_hipt[nbins];
    float reso_d0_pv_eta_ulpt[nbins];
    float reso_dz_pv_eta_ulpt[nbins];
    float reso_d0_bs_eta_ulpt[nbins];
    float reso_dz_bs_eta_ulpt[nbins];

    float reso_d0_pv_phi_lopt[nbins];
    float reso_dz_pv_phi_lopt[nbins];
    float reso_d0_bs_phi_lopt[nbins];
    float reso_dz_bs_phi_lopt[nbins];
    float reso_d0_pv_phi_hipt[nbins];
    float reso_dz_pv_phi_hipt[nbins];
    float reso_d0_bs_phi_hipt[nbins];
    float reso_dz_bs_phi_hipt[nbins];
    float reso_d0_pv_phi_ulpt[nbins];
    float reso_dz_pv_phi_ulpt[nbins];
    float reso_d0_bs_phi_ulpt[nbins];
    float reso_dz_bs_phi_ulpt[nbins];

    for(int i=0; i<nbins; i++){
        nlohmann::json results;
        std::ifstream infile(jsondir + Form("fit_%d.json",i));
        infile >> results;

        eta[i] = results["eta"];
        phi[i] = results["phi"];
        pt[i] = results["pt"];
        reso_d0_pv_pt_loeta[i] = results["reso_d0_pv_pt_loeta"];
        reso_dz_pv_pt_loeta[i] = results["reso_dz_pv_pt_loeta"];
        reso_d0_bs_pt_loeta[i] = results["reso_d0_bs_pt_loeta"];
        reso_dz_bs_pt_loeta[i] = results["reso_dz_bs_pt_loeta"];
        reso_d0_pv_pt_hieta[i] = results["reso_d0_pv_pt_hieta"];
        reso_dz_pv_pt_hieta[i] = results["reso_dz_pv_pt_hieta"];
        reso_d0_bs_pt_hieta[i] = results["reso_d0_bs_pt_hieta"];
        reso_dz_bs_pt_hieta[i] = results["reso_dz_bs_pt_hieta"];

        reso_d0_pv_eta_lopt[i] = results["reso_d0_pv_eta_lopt"];
        reso_dz_pv_eta_lopt[i] = results["reso_dz_pv_eta_lopt"];
        reso_d0_bs_eta_lopt[i] = results["reso_d0_bs_eta_lopt"];
        reso_dz_bs_eta_lopt[i] = results["reso_dz_bs_eta_lopt"];
        reso_d0_pv_eta_hipt[i] = results["reso_d0_pv_eta_hipt"];
        reso_dz_pv_eta_hipt[i] = results["reso_dz_pv_eta_hipt"];
        reso_d0_bs_eta_hipt[i] = results["reso_d0_bs_eta_hipt"];
        reso_dz_bs_eta_hipt[i] = results["reso_dz_bs_eta_hipt"];
        reso_d0_pv_eta_ulpt[i] = results["reso_d0_pv_eta_ulpt"];
        reso_dz_pv_eta_ulpt[i] = results["reso_dz_pv_eta_ulpt"];
        reso_d0_bs_eta_ulpt[i] = results["reso_d0_bs_eta_ulpt"];
        reso_dz_bs_eta_ulpt[i] = results["reso_dz_bs_eta_ulpt"];

        reso_d0_pv_phi_lopt[i] = results["reso_d0_pv_phi_lopt"];
        reso_dz_pv_phi_lopt[i] = results["reso_dz_pv_phi_lopt"];
        reso_d0_bs_phi_lopt[i] = results["reso_d0_bs_phi_lopt"];
        reso_dz_bs_phi_lopt[i] = results["reso_dz_bs_phi_lopt"];
        reso_d0_pv_phi_hipt[i] = results["reso_d0_pv_phi_hipt"];
        reso_dz_pv_phi_hipt[i] = results["reso_dz_pv_phi_hipt"];
        reso_d0_bs_phi_hipt[i] = results["reso_d0_bs_phi_hipt"];
        reso_dz_bs_phi_hipt[i] = results["reso_dz_bs_phi_hipt"];
        reso_d0_pv_phi_ulpt[i] = results["reso_d0_pv_phi_ulpt"];
        reso_dz_pv_phi_ulpt[i] = results["reso_dz_pv_phi_ulpt"];
        reso_d0_bs_phi_ulpt[i] = results["reso_d0_bs_phi_ulpt"];
        reso_dz_bs_phi_ulpt[i] = results["reso_dz_bs_phi_ulpt"];
    }

    TGraph * gr_d0_pv_pt_loeta = new TGraph(nbins, pt, reso_d0_pv_pt_loeta);
    TGraph * gr_dz_pv_pt_loeta = new TGraph(nbins, pt, reso_dz_pv_pt_loeta);
    TGraph * gr_d0_bs_pt_loeta = new TGraph(nbins, pt, reso_d0_bs_pt_loeta);
    TGraph * gr_dz_bs_pt_loeta = new TGraph(nbins, pt, reso_dz_bs_pt_loeta);
    TGraph * gr_d0_pv_pt_hieta = new TGraph(nbins, pt, reso_d0_pv_pt_hieta);
    TGraph * gr_dz_pv_pt_hieta = new TGraph(nbins, pt, reso_dz_pv_pt_hieta);
    TGraph * gr_d0_bs_pt_hieta = new TGraph(nbins, pt, reso_d0_bs_pt_hieta);
    TGraph * gr_dz_bs_pt_hieta = new TGraph(nbins, pt, reso_dz_bs_pt_hieta);
    
    TGraph * gr_d0_pv_eta_lopt = new TGraph(nbins, eta, reso_d0_pv_eta_lopt);
    TGraph * gr_dz_pv_eta_lopt = new TGraph(nbins, eta, reso_dz_pv_eta_lopt);
    TGraph * gr_d0_bs_eta_lopt = new TGraph(nbins, eta, reso_d0_bs_eta_lopt);
    TGraph * gr_dz_bs_eta_lopt = new TGraph(nbins, eta, reso_dz_bs_eta_lopt);
    TGraph * gr_d0_pv_eta_hipt = new TGraph(nbins, eta, reso_d0_pv_eta_hipt);
    TGraph * gr_dz_pv_eta_hipt = new TGraph(nbins, eta, reso_dz_pv_eta_hipt);
    TGraph * gr_d0_bs_eta_hipt = new TGraph(nbins, eta, reso_d0_bs_eta_hipt);
    TGraph * gr_dz_bs_eta_hipt = new TGraph(nbins, eta, reso_dz_bs_eta_hipt);
    TGraph * gr_d0_pv_eta_ulpt = new TGraph(nbins, eta, reso_d0_pv_eta_ulpt);
    TGraph * gr_dz_pv_eta_ulpt = new TGraph(nbins, eta, reso_dz_pv_eta_ulpt);
    TGraph * gr_d0_bs_eta_ulpt = new TGraph(nbins, eta, reso_d0_bs_eta_ulpt);
    TGraph * gr_dz_bs_eta_ulpt = new TGraph(nbins, eta, reso_dz_bs_eta_ulpt);

    TGraph * gr_d0_pv_phi_lopt = new TGraph(nbins, phi, reso_d0_pv_phi_lopt);
    TGraph * gr_dz_pv_phi_lopt = new TGraph(nbins, phi, reso_dz_pv_phi_lopt);
    TGraph * gr_d0_bs_phi_lopt = new TGraph(nbins, phi, reso_d0_bs_phi_lopt);
    TGraph * gr_dz_bs_phi_lopt = new TGraph(nbins, phi, reso_dz_bs_phi_lopt);
    TGraph * gr_d0_pv_phi_hipt = new TGraph(nbins, phi, reso_d0_pv_phi_hipt);
    TGraph * gr_dz_pv_phi_hipt = new TGraph(nbins, phi, reso_dz_pv_phi_hipt);
    TGraph * gr_d0_bs_phi_hipt = new TGraph(nbins, phi, reso_d0_bs_phi_hipt);
    TGraph * gr_dz_bs_phi_hipt = new TGraph(nbins, phi, reso_dz_bs_phi_hipt);
    TGraph * gr_d0_pv_phi_ulpt = new TGraph(nbins, phi, reso_d0_pv_phi_ulpt);
    TGraph * gr_dz_pv_phi_ulpt = new TGraph(nbins, phi, reso_dz_pv_phi_ulpt);
    TGraph * gr_d0_bs_phi_ulpt = new TGraph(nbins, phi, reso_d0_bs_phi_ulpt);
    TGraph * gr_dz_bs_phi_ulpt = new TGraph(nbins, phi, reso_dz_bs_phi_ulpt);

    float height_d0_pv_pt_loeta = *std::max_element(reso_d0_pv_pt_loeta, reso_d0_pv_pt_loeta+nbins);
    float height_dz_pv_pt_loeta = *std::max_element(reso_dz_pv_pt_loeta, reso_dz_pv_pt_loeta+nbins);
    float height_d0_bs_pt_loeta = *std::max_element(reso_d0_bs_pt_loeta, reso_d0_bs_pt_loeta+nbins);
    float height_dz_bs_pt_loeta = *std::max_element(reso_dz_bs_pt_loeta, reso_dz_bs_pt_loeta+nbins);
    float height_d0_pv_pt_hieta = *std::max_element(reso_d0_pv_pt_hieta, reso_d0_pv_pt_hieta+nbins);
    float height_dz_pv_pt_hieta = *std::max_element(reso_dz_pv_pt_hieta, reso_dz_pv_pt_hieta+nbins);
    float height_d0_bs_pt_hieta = *std::max_element(reso_d0_bs_pt_hieta, reso_d0_bs_pt_hieta+nbins);
    float height_dz_bs_pt_hieta = *std::max_element(reso_dz_bs_pt_hieta, reso_dz_bs_pt_hieta+nbins);

    float height_d0_pv_eta_lopt = *std::max_element(reso_d0_pv_eta_lopt, reso_d0_pv_eta_lopt+nbins);
    float height_dz_pv_eta_lopt = *std::max_element(reso_dz_pv_eta_lopt, reso_dz_pv_eta_lopt+nbins);
    float height_d0_bs_eta_lopt = *std::max_element(reso_d0_bs_eta_lopt, reso_d0_bs_eta_lopt+nbins);
    float height_dz_bs_eta_lopt = *std::max_element(reso_dz_bs_eta_lopt, reso_dz_bs_eta_lopt+nbins);
    float height_d0_pv_eta_hipt = *std::max_element(reso_d0_pv_eta_hipt, reso_d0_pv_eta_hipt+nbins);
    float height_dz_pv_eta_hipt = *std::max_element(reso_dz_pv_eta_hipt, reso_dz_pv_eta_hipt+nbins);
    float height_d0_bs_eta_hipt = *std::max_element(reso_d0_bs_eta_hipt, reso_d0_bs_eta_hipt+nbins);
    float height_dz_bs_eta_hipt = *std::max_element(reso_dz_bs_eta_hipt, reso_dz_bs_eta_hipt+nbins);
    float height_d0_pv_eta_ulpt = *std::max_element(reso_d0_pv_eta_ulpt, reso_d0_pv_eta_ulpt+nbins);
    float height_dz_pv_eta_ulpt = *std::max_element(reso_dz_pv_eta_ulpt, reso_dz_pv_eta_ulpt+nbins);
    float height_d0_bs_eta_ulpt = *std::max_element(reso_d0_bs_eta_ulpt, reso_d0_bs_eta_ulpt+nbins);
    float height_dz_bs_eta_ulpt = *std::max_element(reso_dz_bs_eta_ulpt, reso_dz_bs_eta_ulpt+nbins);

    float height_d0_pv_phi_lopt = *std::max_element(reso_d0_pv_phi_lopt, reso_d0_pv_phi_lopt+nbins);
    float height_dz_pv_phi_lopt = *std::max_element(reso_dz_pv_phi_lopt, reso_dz_pv_phi_lopt+nbins);
    float height_d0_bs_phi_lopt = *std::max_element(reso_d0_bs_phi_lopt, reso_d0_bs_phi_lopt+nbins);
    float height_dz_bs_phi_lopt = *std::max_element(reso_dz_bs_phi_lopt, reso_dz_bs_phi_lopt+nbins);
    float height_d0_pv_phi_hipt = *std::max_element(reso_d0_pv_phi_hipt, reso_d0_pv_phi_hipt+nbins);
    float height_dz_pv_phi_hipt = *std::max_element(reso_dz_pv_phi_hipt, reso_dz_pv_phi_hipt+nbins);
    float height_d0_bs_phi_hipt = *std::max_element(reso_d0_bs_phi_hipt, reso_d0_bs_phi_hipt+nbins);
    float height_dz_bs_phi_hipt = *std::max_element(reso_dz_bs_phi_hipt, reso_dz_bs_phi_hipt+nbins);
    float height_d0_pv_phi_ulpt = *std::max_element(reso_d0_pv_phi_ulpt, reso_d0_pv_phi_ulpt+nbins);
    float height_dz_pv_phi_ulpt = *std::max_element(reso_dz_pv_phi_ulpt, reso_dz_pv_phi_ulpt+nbins);
    float height_d0_bs_phi_ulpt = *std::max_element(reso_d0_bs_phi_ulpt, reso_d0_bs_phi_ulpt+nbins);
    float height_dz_bs_phi_ulpt = *std::max_element(reso_dz_bs_phi_ulpt, reso_dz_bs_phi_ulpt+nbins);

    float floor_d0_pv_pt_loeta = *std::min_element(reso_d0_pv_pt_loeta, reso_d0_pv_pt_loeta+nbins);
    float floor_dz_pv_pt_loeta = *std::min_element(reso_dz_pv_pt_loeta, reso_dz_pv_pt_loeta+nbins);
    float floor_d0_bs_pt_loeta = *std::min_element(reso_d0_bs_pt_loeta, reso_d0_bs_pt_loeta+nbins);
    float floor_dz_bs_pt_loeta = *std::min_element(reso_dz_bs_pt_loeta, reso_dz_bs_pt_loeta+nbins);
    float floor_d0_pv_pt_hieta = *std::min_element(reso_d0_pv_pt_hieta, reso_d0_pv_pt_hieta+nbins);
    float floor_dz_pv_pt_hieta = *std::min_element(reso_dz_pv_pt_hieta, reso_dz_pv_pt_hieta+nbins);
    float floor_d0_bs_pt_hieta = *std::min_element(reso_d0_bs_pt_hieta, reso_d0_bs_pt_hieta+nbins);
    float floor_dz_bs_pt_hieta = *std::min_element(reso_dz_bs_pt_hieta, reso_dz_bs_pt_hieta+nbins);

    float floor_d0_pv_eta_lopt = *std::min_element(reso_d0_pv_eta_lopt, reso_d0_pv_eta_lopt+nbins);
    float floor_dz_pv_eta_lopt = *std::min_element(reso_dz_pv_eta_lopt, reso_dz_pv_eta_lopt+nbins);
    float floor_d0_bs_eta_lopt = *std::min_element(reso_d0_bs_eta_lopt, reso_d0_bs_eta_lopt+nbins);
    float floor_dz_bs_eta_lopt = *std::min_element(reso_dz_bs_eta_lopt, reso_dz_bs_eta_lopt+nbins);
    float floor_d0_pv_eta_hipt = *std::min_element(reso_d0_pv_eta_hipt, reso_d0_pv_eta_hipt+nbins);
    float floor_dz_pv_eta_hipt = *std::min_element(reso_dz_pv_eta_hipt, reso_dz_pv_eta_hipt+nbins);
    float floor_d0_bs_eta_hipt = *std::min_element(reso_d0_bs_eta_hipt, reso_d0_bs_eta_hipt+nbins);
    float floor_dz_bs_eta_hipt = *std::min_element(reso_dz_bs_eta_hipt, reso_dz_bs_eta_hipt+nbins);
    float floor_d0_pv_eta_ulpt = *std::min_element(reso_d0_pv_eta_ulpt, reso_d0_pv_eta_ulpt+nbins);
    float floor_dz_pv_eta_ulpt = *std::min_element(reso_dz_pv_eta_ulpt, reso_dz_pv_eta_ulpt+nbins);
    float floor_d0_bs_eta_ulpt = *std::min_element(reso_d0_bs_eta_ulpt, reso_d0_bs_eta_ulpt+nbins);
    float floor_dz_bs_eta_ulpt = *std::min_element(reso_dz_bs_eta_ulpt, reso_dz_bs_eta_ulpt+nbins);

    float floor_d0_pv_phi_lopt = *std::min_element(reso_d0_pv_phi_lopt, reso_d0_pv_phi_lopt+nbins);
    float floor_dz_pv_phi_lopt = *std::min_element(reso_dz_pv_phi_lopt, reso_dz_pv_phi_lopt+nbins);
    float floor_d0_bs_phi_lopt = *std::min_element(reso_d0_bs_phi_lopt, reso_d0_bs_phi_lopt+nbins);
    float floor_dz_bs_phi_lopt = *std::min_element(reso_dz_bs_phi_lopt, reso_dz_bs_phi_lopt+nbins);
    float floor_d0_pv_phi_hipt = *std::min_element(reso_d0_pv_phi_hipt, reso_d0_pv_phi_hipt+nbins);
    float floor_dz_pv_phi_hipt = *std::min_element(reso_dz_pv_phi_hipt, reso_dz_pv_phi_hipt+nbins);
    float floor_d0_bs_phi_hipt = *std::min_element(reso_d0_bs_phi_hipt, reso_d0_bs_phi_hipt+nbins);
    float floor_dz_bs_phi_hipt = *std::min_element(reso_dz_bs_phi_hipt, reso_dz_bs_phi_hipt+nbins);
    float floor_d0_pv_phi_ulpt = *std::min_element(reso_d0_pv_phi_ulpt, reso_d0_pv_phi_ulpt+nbins);
    float floor_dz_pv_phi_ulpt = *std::min_element(reso_dz_pv_phi_ulpt, reso_dz_pv_phi_ulpt+nbins);
    float floor_d0_bs_phi_ulpt = *std::min_element(reso_d0_bs_phi_ulpt, reso_d0_bs_phi_ulpt+nbins);
    float floor_dz_bs_phi_ulpt = *std::min_element(reso_dz_bs_phi_ulpt, reso_dz_bs_phi_ulpt+nbins);
    
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
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
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

    TCanvas *c_d0_eta = new TCanvas("c_d0_eta", "", 800, 600);
    canvas_setup(c_d0_eta);
    gr_d0_pv_eta_ulpt->SetMarkerStyle(4);
    gr_d0_pv_eta_ulpt->SetMarkerColor(kGreen);
    gr_d0_pv_eta_hipt->SetMarkerStyle(4);
    gr_d0_pv_eta_hipt->SetMarkerColor(kBlack);
    gr_d0_pv_eta_lopt->SetMarkerStyle(4);
    gr_d0_pv_eta_lopt->SetMarkerColor(kBlue);
    gr_d0_bs_eta_ulpt->SetMarkerStyle(26);
    gr_d0_bs_eta_ulpt->SetMarkerColor(kGreen);
    gr_d0_bs_eta_hipt->SetMarkerStyle(26);
    gr_d0_bs_eta_hipt->SetMarkerColor(kBlack);
    gr_d0_bs_eta_lopt->SetMarkerStyle(26);
    gr_d0_bs_eta_lopt->SetMarkerColor(kBlue);
    gr_d0_pv_eta_hipt->Draw("AP");
    gr_d0_pv_eta_ulpt->Draw("P SAME");
    gr_d0_pv_eta_lopt->Draw("P SAME");
    gr_d0_bs_eta_ulpt->Draw("P SAME");
    gr_d0_bs_eta_hipt->Draw("P SAME");
    gr_d0_bs_eta_lopt->Draw("P SAME");
    gr_d0_pv_eta_hipt->GetXaxis()->SetTitle("Track #it{#eta}");
    gr_d0_pv_eta_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]");
    TLegend *leg_d0_eta = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_d0_eta->AddEntry(gr_d0_pv_eta_ulpt, "IPPV 5.0<#it{p_{T}}<15.0 GeV", "p");
    leg_d0_eta->AddEntry(gr_d0_pv_eta_hipt, "IPPV 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_d0_eta->AddEntry(gr_d0_pv_eta_lopt, "IPPV 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_eta->AddEntry(gr_d0_bs_eta_ulpt, "IPBS 5.0<#it{p_{T}}<15.0 GeV", "p");
    leg_d0_eta->AddEntry(gr_d0_bs_eta_hipt, "IPBS 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_d0_eta->AddEntry(gr_d0_bs_eta_lopt, "IPBS 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_eta->SetTextFont(42);
    leg_d0_eta->SetTextColor(kBlack);
    leg_d0_eta->SetBorderSize(0);
    leg_d0_eta->SetFillColor(0);
    leg_d0_eta->SetLineWidth(0);
    leg_d0_eta->SetTextSize(0.04);
    leg_d0_eta->Draw();
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_d0_eta);

    float height_d0_eta = std::max(height_d0_pv_eta_hipt, height_d0_pv_eta_lopt);
    height_d0_eta = std::max(height_d0_eta, height_d0_bs_eta_hipt);
    height_d0_eta = std::max(height_d0_eta, height_d0_bs_eta_lopt);
    height_d0_eta = std::max(height_d0_eta, height_d0_pv_eta_ulpt);
    height_d0_eta = std::max(height_d0_eta, height_d0_bs_eta_ulpt);
    float floor_d0_eta = std::min(floor_d0_pv_eta_hipt, floor_d0_pv_eta_lopt);
    floor_d0_eta = std::min(floor_d0_eta, floor_d0_bs_eta_hipt);
    floor_d0_eta = std::min(floor_d0_eta, floor_d0_bs_eta_lopt);
    floor_d0_eta = std::min(floor_d0_eta, floor_d0_pv_eta_ulpt);
    floor_d0_eta = std::min(floor_d0_eta, floor_d0_bs_eta_ulpt);

    gr_d0_pv_eta_hipt->SetMaximum(height_d0_eta*1.5);
    gr_d0_pv_eta_hipt->SetMinimum(0);
    c_d0_eta->Update();
    c_d0_eta->RedrawAxis();
    c_d0_eta->SaveAs(figdir + "d0_eta.png");


    TCanvas *c_d0_phi = new TCanvas("c_d0_phi", "", 800, 600);
    canvas_setup(c_d0_phi);
    gr_d0_pv_phi_ulpt->SetMarkerStyle(4);
    gr_d0_pv_phi_ulpt->SetMarkerColor(kGreen);
    gr_d0_pv_phi_hipt->SetMarkerStyle(4);
    gr_d0_pv_phi_hipt->SetMarkerColor(kBlack);
    gr_d0_pv_phi_lopt->SetMarkerStyle(4);
    gr_d0_pv_phi_lopt->SetMarkerColor(kBlue);
    gr_d0_bs_phi_ulpt->SetMarkerStyle(26);
    gr_d0_bs_phi_ulpt->SetMarkerColor(kGreen);
    gr_d0_bs_phi_hipt->SetMarkerStyle(26);
    gr_d0_bs_phi_hipt->SetMarkerColor(kBlack);
    gr_d0_bs_phi_lopt->SetMarkerStyle(26);
    gr_d0_bs_phi_lopt->SetMarkerColor(kBlue);
    gr_d0_pv_phi_hipt->Draw("AP");
    gr_d0_pv_phi_ulpt->Draw("P SAME");
    gr_d0_pv_phi_lopt->Draw("P SAME");
    gr_d0_bs_phi_ulpt->Draw("P SAME");
    gr_d0_bs_phi_hipt->Draw("P SAME");
    gr_d0_bs_phi_lopt->Draw("P SAME");
    gr_d0_pv_phi_hipt->GetXaxis()->SetTitle("Track #it{#phi}");
    gr_d0_pv_phi_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{xy}} [#mum]");
    TLegend *leg_d0_phi = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_d0_phi->AddEntry(gr_d0_pv_phi_ulpt, "IPPV 5.0<#it{p_{T}}<15.0 GeV", "p");
    leg_d0_phi->AddEntry(gr_d0_pv_phi_hipt, "IPPV 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_d0_phi->AddEntry(gr_d0_pv_phi_lopt, "IPPV 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_phi->AddEntry(gr_d0_bs_phi_ulpt, "IPBS 5.0<#it{p_{T}}<15.0 GeV", "p");
    leg_d0_phi->AddEntry(gr_d0_bs_phi_hipt, "IPBS 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_d0_phi->AddEntry(gr_d0_bs_phi_lopt, "IPBS 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_d0_phi->SetTextFont(42);
    leg_d0_phi->SetTextColor(kBlack);
    leg_d0_phi->SetBorderSize(0);
    leg_d0_phi->SetFillColor(0);
    leg_d0_phi->SetLineWidth(0);
    leg_d0_phi->SetTextSize(0.04);
    leg_d0_phi->Draw();
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_d0_phi);

    float height_d0_phi = std::max(height_d0_pv_phi_hipt, height_d0_pv_phi_lopt);
    height_d0_phi = std::max(height_d0_phi, height_d0_bs_phi_hipt);
    height_d0_phi = std::max(height_d0_phi, height_d0_bs_phi_lopt);
    height_d0_phi = std::max(height_d0_phi, height_d0_pv_phi_ulpt);
    height_d0_phi = std::max(height_d0_phi, height_d0_bs_phi_ulpt);
    float floor_d0_phi = std::min(floor_d0_pv_phi_hipt, floor_d0_pv_phi_lopt);
    floor_d0_phi = std::min(floor_d0_phi, floor_d0_bs_phi_hipt);
    floor_d0_phi = std::min(floor_d0_phi, floor_d0_bs_phi_lopt);
    floor_d0_phi = std::min(floor_d0_phi, floor_d0_pv_phi_ulpt);
    floor_d0_phi = std::min(floor_d0_phi, floor_d0_bs_phi_ulpt);

    gr_d0_pv_phi_hipt->SetMaximum(height_d0_phi*1.5);
    gr_d0_pv_phi_hipt->SetMinimum(0);
    c_d0_phi->Update();
    c_d0_phi->RedrawAxis();
    c_d0_phi->SaveAs(figdir + "d0_phi.png");

    TCanvas *c_dz_pt = new TCanvas("c_dz_pt", "", 800, 600);
    canvas_setup(c_dz_pt);
    gr_dz_pv_pt_hieta->SetMarkerStyle(4);
    gr_dz_pv_pt_hieta->SetMarkerColor(kBlack);
    gr_dz_pv_pt_loeta->SetMarkerStyle(4);
    gr_dz_pv_pt_loeta->SetMarkerColor(kBlue);
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
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_dz_pt);

    float height_dz_pt = std::max(height_dz_pv_pt_hieta, height_dz_pv_pt_loeta);
    float floor_dz_pt = std::min(floor_dz_pv_pt_hieta, floor_dz_pv_pt_loeta);

    gr_dz_pv_pt_hieta->SetMaximum(height_dz_pt*1.5);
    gr_dz_pv_pt_hieta->SetMinimum(0);
    c_dz_pt->Update();
    c_dz_pt->RedrawAxis();
    c_dz_pt->SaveAs(figdir + "dz_pt.png");

    TCanvas *c_dz_eta = new TCanvas("c_dz_eta", "", 800, 600);
    canvas_setup(c_dz_eta);
    gr_dz_pv_eta_ulpt->SetMarkerStyle(4);
    gr_dz_pv_eta_ulpt->SetMarkerColor(kGreen);
    gr_dz_pv_eta_hipt->SetMarkerStyle(4);
    gr_dz_pv_eta_hipt->SetMarkerColor(kBlack);
    gr_dz_pv_eta_lopt->SetMarkerStyle(4);
    gr_dz_pv_eta_lopt->SetMarkerColor(kBlue);
    gr_dz_pv_eta_hipt->Draw("AP");
    gr_dz_pv_eta_lopt->Draw("P SAME");
    gr_dz_pv_eta_ulpt->Draw("P SAME");
    gr_dz_pv_eta_hipt->GetXaxis()->SetTitle("Track #it{#eta}");
    gr_dz_pv_eta_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [#mum]");
    TLegend *leg_dz_eta = new TLegend(0.55, 0.7, 0.75, 0.85);
    leg_dz_eta->AddEntry(gr_dz_pv_eta_ulpt, "IPPV 5.0<#it{p_{T}}<15.0 GeV", "p");
    leg_dz_eta->AddEntry(gr_dz_pv_eta_hipt, "IPPV 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_dz_eta->AddEntry(gr_dz_pv_eta_lopt, "IPPV 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_dz_eta->SetTextFont(42);
    leg_dz_eta->SetTextColor(kBlack);
    leg_dz_eta->SetBorderSize(0);
    leg_dz_eta->SetFillColor(0);
    leg_dz_eta->SetLineWidth(0);
    leg_dz_eta->SetTextSize(0.04);
    leg_dz_eta->Draw();
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_dz_eta);

    float height_dz_eta = std::max(height_dz_pv_eta_hipt, height_dz_pv_eta_lopt);
    height_dz_eta = std::max(height_dz_eta, height_dz_pv_eta_ulpt);
    float floor_dz_eta = std::min(floor_dz_pv_eta_hipt, floor_dz_pv_eta_lopt);
    floor_dz_eta = std::min(floor_dz_eta, floor_dz_pv_eta_ulpt);

    gr_dz_pv_eta_hipt->SetMaximum(height_dz_eta*1.5);
    gr_dz_pv_eta_hipt->SetMinimum(0);
    c_dz_eta->Update();
    c_dz_eta->RedrawAxis();
    c_dz_eta->SaveAs(figdir + "dz_eta.png");


    TCanvas *c_dz_phi = new TCanvas("c_dz_phi", "", 800, 600);
    canvas_setup(c_dz_phi);
    gr_dz_pv_phi_ulpt->SetMarkerStyle(4);
    gr_dz_pv_phi_ulpt->SetMarkerColor(kGreen);
    gr_dz_pv_phi_hipt->SetMarkerStyle(4);
    gr_dz_pv_phi_hipt->SetMarkerColor(kBlack);
    gr_dz_pv_phi_lopt->SetMarkerStyle(4);
    gr_dz_pv_phi_lopt->SetMarkerColor(kBlue);
    gr_dz_pv_phi_hipt->Draw("AP");
    gr_dz_pv_phi_lopt->Draw("P SAME");
    gr_dz_pv_phi_ulpt->Draw("P SAME");
    gr_dz_pv_phi_hipt->GetXaxis()->SetTitle("Track #it{#phi}");
    gr_dz_pv_phi_hipt->GetYaxis()->SetTitle("Track IP resolution #it{d_{z}} [#mum]");
    TLegend *leg_dz_phi = new TLegend(0.55, 0.7, 0.75, 0.85);
    leg_dz_phi->AddEntry(gr_dz_pv_phi_ulpt, "IPPV 5.0<#it{p_{T}}<15.0 GeV", "p");
    leg_dz_phi->AddEntry(gr_dz_pv_phi_hipt, "IPPV 1.0<#it{p_{T}}<5.0 GeV", "p");
    leg_dz_phi->AddEntry(gr_dz_pv_phi_lopt, "IPPV 0.1<#it{p_{T}}<1.0 GeV", "p");
    leg_dz_phi->SetTextFont(42);
    leg_dz_phi->SetTextColor(kBlack);
    leg_dz_phi->SetBorderSize(0);
    leg_dz_phi->SetFillColor(0);
    leg_dz_phi->SetLineWidth(0);
    leg_dz_phi->SetTextSize(0.04);
    leg_dz_phi->Draw();
    write_text(0.55, 0.88, "High-q^{2} multi-jet events");
    CMS_lumi(c_dz_phi);

    float height_dz_phi = std::max(height_dz_pv_phi_hipt, height_dz_pv_phi_lopt);
    height_dz_phi = std::max(height_dz_phi, height_dz_pv_phi_ulpt);
    float floor_dz_phi = std::min(floor_dz_pv_phi_hipt, floor_dz_pv_phi_lopt);
    floor_dz_phi = std::min(floor_dz_phi, floor_dz_pv_phi_ulpt);

    gr_dz_pv_phi_hipt->SetMaximum(height_dz_phi*1.5);
    gr_dz_pv_phi_hipt->SetMinimum(0);
    c_dz_phi->Update();
    c_dz_phi->RedrawAxis();
    c_dz_phi->SaveAs(figdir + "dz_phi.png");

    return 0;
}
