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

const TString figdir = "../../figures/"+datatype+"/pv_res/";
const TString jsondir = "../../json/"+datatype+"/pv_res/";
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
    float sumpt2_sqrt[nbins];

    for(int i=0; i<nbins; i++){
        nlohmann::json data_results;
        std::ifstream data_infile(jsondir + Form("/data/fit_%d.json",i));
        data_infile >> data_results;

        nlohmann::json mc_results;
        std::ifstream mc_infile(jsondir + Form("/mc/fit_%d.json",i));
        mc_infile >> mc_results;

        sumpt2_sqrt[i] = data_results["sumpt2_sqrt"];
        data_reso_pullx[i] = data_results["reso_pullx"];
        data_reso_pully[i] = data_results["reso_pully"];
        data_reso_pullz[i] = data_results["reso_pullz"];
        data_reso_pvx[i] = data_results["reso_pvx"];
        data_reso_pvy[i] = data_results["reso_pvy"];
        data_reso_pvz[i] = data_results["reso_pvz"];
        mc_reso_pullx[i] = mc_results["reso_pullx"];
        mc_reso_pully[i] = mc_results["reso_pully"];
        mc_reso_pullz[i] = mc_results["reso_pullz"];
        mc_reso_pvx[i] = mc_results["reso_pvx"];
        mc_reso_pvy[i] = mc_results["reso_pvy"];
        mc_reso_pvz[i] = mc_results["reso_pvz"];
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

    TCanvas *c_pvx = new TCanvas("c_pvx", "", 800, 600);
    canvas_setup(c_pvx);
    data_gr_pvx->SetMarkerStyle(20);
    data_gr_pvx->SetMarkerColor(kBlack);
    mc_gr_pvx->SetMarkerStyle(20);
    mc_gr_pvx->SetMarkerColor(kRed);
    data_gr_pvx->Draw("AP");
    mc_gr_pvx->Draw("P SAME");
    data_gr_pvx->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    data_gr_pvx->GetYaxis()->SetTitle("PV resolution in x [#mum]");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pvx);
    data_gr_pvx->SetMaximum(height_pvx*1.3);
    data_gr_pvx->SetMinimum(0);
    data_gr_pvx->GetXaxis()->SetNdivisions(810);
    data_gr_pvx->GetYaxis()->SetNdivisions(810);
    data_gr_pvx->GetXaxis()->SetTitleSize(0.035);
    data_gr_pvx->GetXaxis()->SetTitleOffset(1.5);
    TLegend * leg_pvx = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_pvx->AddEntry(data_gr_pvx, "Data", "p");
    leg_pvx->AddEntry(mc_gr_pvx, "Simulation", "p");
    leg_pvx->SetTextFont(42);
    leg_pvx->SetTextColor(kBlack);
    leg_pvx->SetBorderSize(0);
    leg_pvx->SetFillColor(0);
    leg_pvx->SetLineWidth(0);
    leg_pvx->SetTextSize(0.04);
    leg_pvx->Draw();
    c_pvx->Update();
    c_pvx->RedrawAxis();
    c_pvx->SaveAs(figdir + "pvx.png");
    /* data_gr_pvx->SetMaximum(height_pvx*100); */
    /* data_gr_pvx->SetMinimum(floor_pvx/100); */
    /* c_pvx->SetLogy(); */
    /* c_pvx->Update(); */
    /* c_pvx->RedrawAxis(); */
    /* c_pvx->SaveAs(figdir + "pvx_log.png"); */

    TCanvas *c_pvy = new TCanvas("c_pvy", "", 800, 600);
    canvas_setup(c_pvy);
    data_gr_pvy->SetMarkerStyle(20);
    data_gr_pvy->SetMarkerColor(kBlack);
    mc_gr_pvy->SetMarkerStyle(20);
    mc_gr_pvy->SetMarkerColor(kRed);
    data_gr_pvy->Draw("AP");
    mc_gr_pvy->Draw("P SAME");
    data_gr_pvy->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    data_gr_pvy->GetYaxis()->SetTitle("PV resolution in y [#mum]");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pvy);
    data_gr_pvy->SetMaximum(height_pvy*1.3);
    data_gr_pvy->SetMinimum(0);
    data_gr_pvy->GetXaxis()->SetNdivisions(810);
    data_gr_pvy->GetYaxis()->SetNdivisions(810);
    data_gr_pvy->GetXaxis()->SetTitleSize(0.035);
    data_gr_pvy->GetXaxis()->SetTitleOffset(1.5);
    TLegend * leg_pvy = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_pvy->AddEntry(data_gr_pvy, "Data", "p");
    leg_pvy->AddEntry(mc_gr_pvy, "Simulation", "p");
    leg_pvy->SetTextFont(42);
    leg_pvy->SetTextColor(kBlack);
    leg_pvy->SetBorderSize(0);
    leg_pvy->SetFillColor(0);
    leg_pvy->SetLineWidth(0);
    leg_pvy->SetTextSize(0.04);
    leg_pvy->Draw();
    c_pvy->Update();
    c_pvy->RedrawAxis();
    c_pvy->SaveAs(figdir + "pvy.png");
    /* data_gr_pvy->SetMaximum(height_pvy*100); */
    /* data_gr_pvy->SetMinimum(floor_pvy/100); */
    /* c_pvy->SetLogy(); */
    /* c_pvy->Update(); */
    /* c_pvy->RedrawAxis(); */
    /* c_pvy->SaveAs(figdir + "pvy_log.png"); */

    TCanvas *c_pvz = new TCanvas("c_pvz", "", 800, 600);
    canvas_setup(c_pvz);
    data_gr_pvz->SetMarkerStyle(20);
    data_gr_pvz->SetMarkerColor(kBlack);
    mc_gr_pvz->SetMarkerStyle(20);
    mc_gr_pvz->SetMarkerColor(kRed);
    data_gr_pvz->Draw("AP");
    mc_gr_pvz->Draw("P SAME");
    data_gr_pvz->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    data_gr_pvz->GetYaxis()->SetTitle("PV resolution in z [#mum]");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pvz);
    data_gr_pvz->SetMaximum(height_pvz*1.3);
    data_gr_pvz->SetMinimum(0);
    data_gr_pvz->GetXaxis()->SetNdivisions(810);
    data_gr_pvz->GetYaxis()->SetNdivisions(810);
    data_gr_pvz->GetXaxis()->SetTitleSize(0.035);
    data_gr_pvz->GetXaxis()->SetTitleOffset(1.5);
    TLegend * leg_pvz = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_pvz->AddEntry(data_gr_pvz, "Data", "p");
    leg_pvz->AddEntry(mc_gr_pvz, "Simulation", "p");
    leg_pvz->SetTextFont(42);
    leg_pvz->SetTextColor(kBlack);
    leg_pvz->SetBorderSize(0);
    leg_pvz->SetFillColor(0);
    leg_pvz->SetLineWidth(0);
    leg_pvz->SetTextSize(0.04);
    leg_pvz->Draw();
    c_pvz->Update();
    c_pvz->RedrawAxis();
    c_pvz->SaveAs(figdir + "pvz.png");
    /* data_gr_pvz->SetMaximum(height_pvz*100); */
    /* data_gr_pvz->SetMinimum(floor_pvz/100); */
    /* c_pvz->SetLogy(); */
    /* c_pvz->Update(); */
    /* c_pvz->RedrawAxis(); */
    /* c_pvz->SaveAs(figdir + "pvz_log.png"); */

    TCanvas *c_pullx = new TCanvas("c_pullx", "", 800, 600);
    canvas_setup(c_pullx);
    data_gr_pullx->SetMarkerStyle(20);
    data_gr_pullx->SetMarkerColor(kBlack);
    mc_gr_pullx->SetMarkerStyle(20);
    mc_gr_pullx->SetMarkerColor(kRed);
    data_gr_pullx->Draw("AP");
    mc_gr_pullx->Draw("P SAME");
    data_gr_pullx->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    data_gr_pullx->GetYaxis()->SetTitle("PV pull in x");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pullx);
    data_gr_pullx->SetMaximum(1.5);
    data_gr_pullx->SetMinimum(0.5);
    data_gr_pullx->GetXaxis()->SetNdivisions(810);
    data_gr_pullx->GetYaxis()->SetNdivisions(810);
    data_gr_pullx->GetXaxis()->SetTitleSize(0.035);
    data_gr_pullx->GetXaxis()->SetTitleOffset(1.5);
    TLegend * leg_pullx = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_pullx->AddEntry(data_gr_pullx, "Data", "p");
    leg_pullx->AddEntry(mc_gr_pullx, "Simulation", "p");
    leg_pullx->SetTextFont(42);
    leg_pullx->SetTextColor(kBlack);
    leg_pullx->SetBorderSize(0);
    leg_pullx->SetFillColor(0);
    leg_pullx->SetLineWidth(0);
    leg_pullx->SetTextSize(0.04);
    leg_pullx->Draw();
    c_pullx->Update();
    c_pullx->RedrawAxis();
    c_pullx->SaveAs(figdir + "pullx.png");

    TCanvas *c_pully = new TCanvas("c_pully", "", 800, 600);
    canvas_setup(c_pully);
    data_gr_pully->SetMarkerStyle(20);
    data_gr_pully->SetMarkerColor(kBlack);
    mc_gr_pully->SetMarkerStyle(20);
    mc_gr_pully->SetMarkerColor(kRed);
    data_gr_pully->Draw("AP");
    mc_gr_pully->Draw("P SAME");
    data_gr_pully->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    data_gr_pully->GetYaxis()->SetTitle("PV pull in y");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pully);
    data_gr_pully->SetMaximum(1.5);
    data_gr_pully->SetMinimum(0.5);
    data_gr_pully->GetXaxis()->SetNdivisions(810);
    data_gr_pully->GetYaxis()->SetNdivisions(810);
    data_gr_pully->GetXaxis()->SetTitleSize(0.035);
    data_gr_pully->GetXaxis()->SetTitleOffset(1.5);
    TLegend * leg_pully = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_pully->AddEntry(data_gr_pully, "Data", "p");
    leg_pully->AddEntry(mc_gr_pully, "Simulation", "p");
    leg_pully->SetTextFont(42);
    leg_pully->SetTextColor(kBlack);
    leg_pully->SetBorderSize(0);
    leg_pully->SetFillColor(0);
    leg_pully->SetLineWidth(0);
    leg_pully->SetTextSize(0.04);
    leg_pully->Draw();
    c_pully->Update();
    c_pully->RedrawAxis();
    c_pully->SaveAs(figdir + "pully.png");

    TCanvas *c_pullz = new TCanvas("c_pullz", "", 800, 600);
    canvas_setup(c_pullz);
    data_gr_pullz->SetMarkerStyle(20);
    data_gr_pullz->SetMarkerColor(kBlack);
    mc_gr_pullz->SetMarkerStyle(20);
    mc_gr_pullz->SetMarkerColor(kRed);
    data_gr_pullz->Draw("AP");
    mc_gr_pullz->Draw("P SAME");
    data_gr_pullz->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    data_gr_pullz->GetYaxis()->SetTitle("PV pull in z");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pullz);
    data_gr_pullz->SetMaximum(1.5);
    data_gr_pullz->SetMinimum(0.5);
    data_gr_pullz->GetXaxis()->SetNdivisions(810);
    data_gr_pullz->GetYaxis()->SetNdivisions(810);
    data_gr_pullz->GetXaxis()->SetTitleSize(0.035);
    data_gr_pullz->GetXaxis()->SetTitleOffset(1.5);
    TLegend * leg_pullz = new TLegend(0.55, 0.65, 0.75, 0.85);
    leg_pullz->AddEntry(data_gr_pullz, "Data", "p");
    leg_pullz->AddEntry(mc_gr_pullz, "Simulation", "p");
    leg_pullz->SetTextFont(42);
    leg_pullz->SetTextColor(kBlack);
    leg_pullz->SetBorderSize(0);
    leg_pullz->SetFillColor(0);
    leg_pullz->SetLineWidth(0);
    leg_pullz->SetTextSize(0.04);
    leg_pullz->Draw();
    c_pullz->Update();
    c_pullz->RedrawAxis();
    c_pullz->SaveAs(figdir + "pullz.png");

    return 0;
}
