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
#include "input_list.cc"

const TString figdir = "./figures/"+datatype+"/pv_res_analysis/";
const TString jsondir = "./json/"+datatype+"/pv_res/";
const int nbins = 50;

int draw_pv_res(){

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, 2022"+all_era;

    float reso_pullx[nbins];
    float reso_pully[nbins];
    float reso_pullz[nbins];
    float reso_pvx[nbins];
    float reso_pvy[nbins];
    float reso_pvz[nbins];
    float resoerr_pullx[nbins];
    float resoerr_pully[nbins];
    float resoerr_pullz[nbins];
    float resoerr_pvx[nbins];
    float resoerr_pvy[nbins];
    float resoerr_pvz[nbins];
    float sumpt2_sqrt[nbins];

    for(int i=0; i<nbins; i++){
        nlohmann::json results;
        std::ifstream infile(jsondir + Form("fit_%d.json",i));
        infile >> results;

        reso_pullx[i] = results["reso_pullx"];
        reso_pully[i] = results["reso_pully"];
        reso_pullz[i] = results["reso_pullz"];
        reso_pvx[i] = results["reso_pvx"];
        reso_pvy[i] = results["reso_pvy"];
        reso_pvz[i] = results["reso_pvz"];
        reso_pvz[i] = reso_pvz[i]*1000;
        resoerr_pullx[i] = results["resoerr_pullx"];
        resoerr_pully[i] = results["resoerr_pully"];
        resoerr_pullz[i] = results["resoerr_pullz"];
        resoerr_pvx[i] = results["resoerr_pvx"];
        resoerr_pvy[i] = results["resoerr_pvy"];
        resoerr_pvz[i] = results["resoerr_pvz"];
        resoerr_pvz[i] = resoerr_pvz[i]*1000;
        sumpt2_sqrt[i] = results["sumpt2_sqrt"];
    }

    float arr_zeros[nbins] = {0};

    /* TGraphErrors * gr_pvx = new TGraphErrors(nbins, sumpt2_sqrt, reso_pvx, arr_zeros, resoerr_pvx); */
    /* TGraphErrors * gr_pvy = new TGraphErrors(nbins, sumpt2_sqrt, reso_pvy, arr_zeros, resoerr_pvy); */
    /* TGraphErrors * gr_pvz = new TGraphErrors(nbins, sumpt2_sqrt, reso_pvz, arr_zeros, resoerr_pvz); */
    /* TGraphErrors * gr_pullx = new TGraphErrors(nbins, sumpt2_sqrt, reso_pullx, arr_zeros, resoerr_pullx); */
    /* TGraphErrors * gr_pully = new TGraphErrors(nbins, sumpt2_sqrt, reso_pully, arr_zeros, resoerr_pully); */
    /* TGraphErrors * gr_pullz = new TGraphErrors(nbins, sumpt2_sqrt, reso_pullz, arr_zeros, resoerr_pullz); */

    TGraph * gr_pvx = new TGraph(nbins, sumpt2_sqrt, reso_pvx);
    TGraph * gr_pvy = new TGraph(nbins, sumpt2_sqrt, reso_pvy);
    TGraph * gr_pvz = new TGraph(nbins, sumpt2_sqrt, reso_pvz);
    TGraph * gr_pullx = new TGraph(nbins, sumpt2_sqrt, reso_pullx);
    TGraph * gr_pully = new TGraph(nbins, sumpt2_sqrt, reso_pully);
    TGraph * gr_pullz = new TGraph(nbins, sumpt2_sqrt, reso_pullz);

    float height_pvx = *std::max_element(reso_pvx, reso_pvx+nbins);
    float height_pvy = *std::max_element(reso_pvy, reso_pvy+nbins);
    float height_pvz = *std::max_element(reso_pvz, reso_pvz+nbins);
    float floor_pvx = *std::min_element(reso_pvx, reso_pvx+nbins);
    float floor_pvy = *std::min_element(reso_pvy, reso_pvy+nbins);
    float floor_pvz = *std::min_element(reso_pvz, reso_pvz+nbins);
    
    TCanvas *c_pvx = new TCanvas("c_pvx", "", 800, 600);
    canvas_setup(c_pvx);
    gr_pvx->SetMarkerStyle(20);
    gr_pvx->SetMarkerColor(kBlack);
    gr_pvx->Draw("AP");
    gr_pvx->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr_pvx->GetYaxis()->SetTitle("PV resolution in x [#mum]");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pvx);
    gr_pvx->SetMaximum(height_pvx*1.3);
    gr_pvx->SetMinimum(0);
    c_pvx->Update();
    c_pvx->RedrawAxis();
    c_pvx->SaveAs(figdir + "pvx.png");
    /* gr_pvx->SetMaximum(height_pvx*100); */
    /* gr_pvx->SetMinimum(floor_pvx/100); */
    /* c_pvx->SetLogy(); */
    /* c_pvx->Update(); */
    /* c_pvx->RedrawAxis(); */
    /* c_pvx->SaveAs(figdir + "pvx_log.png"); */

    TCanvas *c_pvy = new TCanvas("c_pvy", "", 800, 600);
    canvas_setup(c_pvy);
    gr_pvy->SetMarkerStyle(20);
    gr_pvy->SetMarkerColor(kBlack);
    gr_pvy->Draw("AP");
    gr_pvy->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr_pvy->GetYaxis()->SetTitle("PV resolution in y [#mum]");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pvy);
    gr_pvy->SetMaximum(height_pvy*1.3);
    gr_pvy->SetMinimum(0);
    c_pvy->Update();
    c_pvy->RedrawAxis();
    c_pvy->SaveAs(figdir + "pvy.png");
    /* gr_pvy->SetMaximum(height_pvy*100); */
    /* gr_pvy->SetMinimum(floor_pvy/100); */
    /* c_pvy->SetLogy(); */
    /* c_pvy->Update(); */
    /* c_pvy->RedrawAxis(); */
    /* c_pvy->SaveAs(figdir + "pvy_log.png"); */

    TCanvas *c_pvz = new TCanvas("c_pvz", "", 800, 600);
    canvas_setup(c_pvz);
    gr_pvz->SetMarkerStyle(20);
    gr_pvz->SetMarkerColor(kBlack);
    gr_pvz->Draw("AP");
    gr_pvz->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr_pvz->GetYaxis()->SetTitle("PV resolution in z [#mum]");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pvz);
    gr_pvz->SetMaximum(height_pvz*1.3);
    gr_pvz->SetMinimum(0);
    c_pvz->Update();
    c_pvz->RedrawAxis();
    c_pvz->SaveAs(figdir + "pvz.png");
    /* gr_pvz->SetMaximum(height_pvz*100); */
    /* gr_pvz->SetMinimum(floor_pvz/100); */
    /* c_pvz->SetLogy(); */
    /* c_pvz->Update(); */
    /* c_pvz->RedrawAxis(); */
    /* c_pvz->SaveAs(figdir + "pvz_log.png"); */

    TCanvas *c_pullx = new TCanvas("c_pullx", "", 800, 600);
    canvas_setup(c_pullx);
    gr_pullx->SetMarkerStyle(20);
    gr_pullx->SetMarkerColor(kBlack);
    gr_pullx->Draw("AP");
    gr_pullx->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr_pullx->GetYaxis()->SetTitle("PV pull in x");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pullx);
    gr_pullx->SetMaximum(1.5);
    gr_pullx->SetMinimum(0.5);
    c_pullx->Update();
    c_pullx->RedrawAxis();
    c_pullx->SaveAs(figdir + "pullx.png");

    TCanvas *c_pully = new TCanvas("c_pully", "", 800, 600);
    canvas_setup(c_pully);
    gr_pully->SetMarkerStyle(20);
    gr_pully->SetMarkerColor(kBlack);
    gr_pully->Draw("AP");
    gr_pully->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr_pully->GetYaxis()->SetTitle("PV pull in y");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pully);
    gr_pully->SetMaximum(1.5);
    gr_pully->SetMinimum(0.5);
    c_pully->Update();
    c_pully->RedrawAxis();
    c_pully->SaveAs(figdir + "pully.png");

    TCanvas *c_pullz = new TCanvas("c_pullz", "", 800, 600);
    canvas_setup(c_pullz);
    gr_pullz->SetMarkerStyle(20);
    gr_pullz->SetMarkerColor(kBlack);
    gr_pullz->Draw("AP");
    gr_pullz->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr_pullz->GetYaxis()->SetTitle("PV pull in z");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c_pullz);
    gr_pullz->SetMaximum(1.5);
    gr_pullz->SetMinimum(0.5);
    c_pullz->Update();
    c_pullz->RedrawAxis();
    c_pullz->SaveAs(figdir + "pullz.png");

    return 0;
}
