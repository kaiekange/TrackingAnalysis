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

const TString figdir = "../../figures/"+datatype+"/pv_res_analysis/";
const TString jsondir = "../../json/"+datatype+"/pv_res/";
const int nbins = 100;

int draw_pv_res(){

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, 2022"+all_era;

    float reso_pullx[nbins];
    float reso_pully[nbins];
    float reso_pullz[nbins];
    float reso_pvx[nbins];
    float reso_pvy[nbins];
    float reso_pvz[nbins];
    float reso2_pullx[nbins];
    float reso2_pully[nbins];
    float reso2_pullz[nbins];
    float reso2_pvx[nbins];
    float reso2_pvy[nbins];
    float reso2_pvz[nbins];
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
        reso2_pullx[i] = results["reso2_pullx"];
        reso2_pully[i] = results["reso2_pully"];
        reso2_pullz[i] = results["reso2_pullz"];
        reso2_pvx[i] = results["reso2_pvx"];
        reso2_pvy[i] = results["reso2_pvy"];
        reso2_pvz[i] = results["reso2_pvz"];
        sumpt2_sqrt[i] = results["sumpt2_sqrt"];
    }

    float arr_zeros[nbins] = {0};

    TGraph * gr_pvx = new TGraph(nbins, sumpt2_sqrt, reso_pvx);
    TGraph * gr_pvy = new TGraph(nbins, sumpt2_sqrt, reso_pvy);
    TGraph * gr_pvz = new TGraph(nbins, sumpt2_sqrt, reso_pvz);
    TGraph * gr_pullx = new TGraph(nbins, sumpt2_sqrt, reso_pullx);
    TGraph * gr_pully = new TGraph(nbins, sumpt2_sqrt, reso_pully);
    TGraph * gr_pullz = new TGraph(nbins, sumpt2_sqrt, reso_pullz);
    TGraph * gr2_pvx = new TGraph(nbins, sumpt2_sqrt, reso2_pvx);
    TGraph * gr2_pvy = new TGraph(nbins, sumpt2_sqrt, reso2_pvy);
    TGraph * gr2_pvz = new TGraph(nbins, sumpt2_sqrt, reso2_pvz);
    TGraph * gr2_pullx = new TGraph(nbins, sumpt2_sqrt, reso2_pullx);
    TGraph * gr2_pully = new TGraph(nbins, sumpt2_sqrt, reso2_pully);
    TGraph * gr2_pullz = new TGraph(nbins, sumpt2_sqrt, reso2_pullz);

    float height_pvx = *std::max_element(reso_pvx, reso_pvx+nbins);
    float height_pvy = *std::max_element(reso_pvy, reso_pvy+nbins);
    float height_pvz = *std::max_element(reso_pvz, reso_pvz+nbins);
    float floor_pvx = *std::min_element(reso_pvx, reso_pvx+nbins);
    float floor_pvy = *std::min_element(reso_pvy, reso_pvy+nbins);
    float floor_pvz = *std::min_element(reso_pvz, reso_pvz+nbins);
    float height2_pvx = *std::max_element(reso2_pvx, reso2_pvx+nbins);
    float height2_pvy = *std::max_element(reso2_pvy, reso2_pvy+nbins);
    float height2_pvz = *std::max_element(reso2_pvz, reso2_pvz+nbins);
    float floor2_pvx = *std::min_element(reso2_pvx, reso2_pvx+nbins);
    float floor2_pvy = *std::min_element(reso2_pvy, reso2_pvy+nbins);
    float floor2_pvz = *std::min_element(reso2_pvz, reso2_pvz+nbins);
    
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

    TCanvas *c2_pvx = new TCanvas("c2_pvx", "", 800, 600);
    canvas_setup(c2_pvx);
    gr2_pvx->SetMarkerStyle(20);
    gr2_pvx->SetMarkerColor(kBlack);
    gr2_pvx->Draw("AP");
    gr2_pvx->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr2_pvx->GetYaxis()->SetTitle("PV resolution in x [#mum]");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c2_pvx);
    gr2_pvx->SetMaximum(height2_pvx*1.3);
    gr2_pvx->SetMinimum(0);
    c2_pvx->Update();
    c2_pvx->RedrawAxis();
    c2_pvx->SaveAs(figdir + "pvx2.png");
    /* gr2_pvx->SetMaximum(height2_pvx*100); */
    /* gr2_pvx->SetMinimum(floor_pvx/100); */
    /* c2_pvx->SetLogy(); */
    /* c2_pvx->Update(); */
    /* c2_pvx->RedrawAxis(); */
    /* c2_pvx->SaveAs(figdir + "pvx_log2.png"); */

    TCanvas *c2_pvy = new TCanvas("c2_pvy", "", 800, 600);
    canvas_setup(c2_pvy);
    gr2_pvy->SetMarkerStyle(20);
    gr2_pvy->SetMarkerColor(kBlack);
    gr2_pvy->Draw("AP");
    gr2_pvy->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr2_pvy->GetYaxis()->SetTitle("PV resolution in y [#mum]");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c2_pvy);
    gr2_pvy->SetMaximum(height2_pvy*1.3);
    gr2_pvy->SetMinimum(0);
    c2_pvy->Update();
    c2_pvy->RedrawAxis();
    c2_pvy->SaveAs(figdir + "pvy2.png");
    /* gr2_pvy->SetMaximum(height2_pvy*100); */
    /* gr2_pvy->SetMinimum(floor_pvy/100); */
    /* c2_pvy->SetLogy(); */
    /* c2_pvy->Update(); */
    /* c2_pvy->RedrawAxis(); */
    /* c2_pvy->SaveAs(figdir + "pvy_log2.png"); */

    TCanvas *c2_pvz = new TCanvas("c2_pvz", "", 800, 600);
    canvas_setup(c2_pvz);
    gr2_pvz->SetMarkerStyle(20);
    gr2_pvz->SetMarkerColor(kBlack);
    gr2_pvz->Draw("AP");
    gr2_pvz->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr2_pvz->GetYaxis()->SetTitle("PV resolution in z [#mum]");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c2_pvz);
    gr2_pvz->SetMaximum(height2_pvz*1.3);
    gr2_pvz->SetMinimum(0);
    c2_pvz->Update();
    c2_pvz->RedrawAxis();
    c2_pvz->SaveAs(figdir + "pvz2.png");
    /* gr2_pvz->SetMaximum(height2_pvz*100); */
    /* gr2_pvz->SetMinimum(floor_pvz/100); */
    /* c2_pvz->SetLogy(); */
    /* c2_pvz->Update(); */
    /* c2_pvz->RedrawAxis(); */
    /* c2_pvz->SaveAs(figdir + "pvz_log2.png"); */

    TCanvas *c2_pullx = new TCanvas("c2_pullx", "", 800, 600);
    canvas_setup(c2_pullx);
    gr2_pullx->SetMarkerStyle(20);
    gr2_pullx->SetMarkerColor(kBlack);
    gr2_pullx->Draw("AP");
    gr2_pullx->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr2_pullx->GetYaxis()->SetTitle("PV pull in x");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c2_pullx);
    gr2_pullx->SetMaximum(1.5);
    gr2_pullx->SetMinimum(0.5);
    c2_pullx->Update();
    c2_pullx->RedrawAxis();
    c2_pullx->SaveAs(figdir + "pullx2.png");

    TCanvas *c2_pully = new TCanvas("c2_pully", "", 800, 600);
    canvas_setup(c2_pully);
    gr2_pully->SetMarkerStyle(20);
    gr2_pully->SetMarkerColor(kBlack);
    gr2_pully->Draw("AP");
    gr2_pully->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr2_pully->GetYaxis()->SetTitle("PV pull in y");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c2_pully);
    gr2_pully->SetMaximum(1.5);
    gr2_pully->SetMinimum(0.5);
    c2_pully->Update();
    c2_pully->RedrawAxis();
    c2_pully->SaveAs(figdir + "pully2.png");

    TCanvas *c2_pullz = new TCanvas("c2_pullz", "", 800, 600);
    canvas_setup(c2_pullz);
    gr2_pullz->SetMarkerStyle(20);
    gr2_pullz->SetMarkerColor(kBlack);
    gr2_pullz->Draw("AP");
    gr2_pullz->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}}^{2}} [GeV]");
    gr2_pullz->GetYaxis()->SetTitle("PV pull in z");
    write_text(0.55, 0.88, datatype_text);
    CMS_lumi(c2_pullz);
    gr2_pullz->SetMaximum(1.5);
    gr2_pullz->SetMinimum(0.5);
    c2_pullz->Update();
    c2_pullz->RedrawAxis();
    c2_pullz->SaveAs(figdir + "pullz2.png");
    return 0;
}
