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

const TString figdir = "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias/pv_res/"+sampletype;
const int nbins = 100;

void draw_gr(TGraph * gr, float height, float ground, TString given_text, TString xlabel, TString ylabel, TString figpath) {

    lumi_sqrtS = "13.6 TeV, 2022 " + all_era;

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas_setup(canvas);
    canvas->SetBottomMargin(0.15);
    canvas->SetRightMargin(0.05);
    canvas->SetLogy(0);
    canvas->SetFillColor(0);
    canvas->SetFrameFillColor(0);

    gr->SetMarkerStyle(20);
    gr->SetMarkerColor(kBlack);
    gr->Draw("AP");
    gr->GetXaxis()->SetTitle(xlabel);
    gr->GetYaxis()->SetTitle(ylabel);
    gr->GetXaxis()->SetTitleSize(0.035);
    gr->GetXaxis()->SetTitleOffset(1.5);
    write_text(0.55, 0.85, given_text);
    gr->SetMaximum(height*1.3);
    gr->SetMinimum(0.0);
    gr->GetYaxis()->SetNdivisions(810);
    CMS_lumi(canvas);
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath+".png");

    /* gr->SetMaximum(height*100.); */
    /* gr->SetMinimum(ground/100.); */
    /* canvas->SetLogy(1); */
    /* canvas->Update(); */
    /* canvas->RedrawAxis(); */
    /* canvas->SaveAs(figpath + "_log.png"); */
}

int draw_pv_res(){

    setTDRStyle();

    float data_reso_pullx[nbins];
    float data_reso_pully[nbins];
    float data_reso_pullz[nbins];
    float data_reso_pvx[nbins];
    float data_reso_pvy[nbins];
    float data_reso_pvz[nbins];

    float sumpt2_sqrt[nbins];

    for(int i=0; i<nbins; i++){
        nlohmann::json data_results;
        std::ifstream data_infile(Form("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/ZeroBias/pv_res/data/fit_%d.json",i));
        data_infile >> data_results;

        sumpt2_sqrt[i] = data_results["sumpt2_sqrt"];
        data_reso_pullx[i] = data_results["reso_pullx"];
        data_reso_pully[i] = data_results["reso_pully"];
        data_reso_pullz[i] = data_results["reso_pullz"];
        data_reso_pvx[i] = data_results["reso_pvx"];
        data_reso_pvy[i] = data_results["reso_pvy"];
        data_reso_pvz[i] = data_results["reso_pvz"];
    }

    TGraph * data_gr_pvx = new TGraph(nbins, sumpt2_sqrt, data_reso_pvx);
    TGraph * data_gr_pvy = new TGraph(nbins, sumpt2_sqrt, data_reso_pvy);
    TGraph * data_gr_pvz = new TGraph(nbins, sumpt2_sqrt, data_reso_pvz);
    TGraph * data_gr_pullx = new TGraph(nbins, sumpt2_sqrt, data_reso_pullx);
    TGraph * data_gr_pully = new TGraph(nbins, sumpt2_sqrt, data_reso_pully);
    TGraph * data_gr_pullz = new TGraph(nbins, sumpt2_sqrt, data_reso_pullz);

    float data_height_pvx = *std::max_element(data_reso_pvx, data_reso_pvx+nbins);
    float data_height_pvy = *std::max_element(data_reso_pvy, data_reso_pvy+nbins);
    float data_height_pvz = *std::max_element(data_reso_pvz, data_reso_pvz+nbins);
    float data_floor_pvx = *std::min_element(data_reso_pvx, data_reso_pvx+nbins);
    float data_floor_pvy = *std::min_element(data_reso_pvy, data_reso_pvy+nbins);
    float data_floor_pvz = *std::min_element(data_reso_pvz, data_reso_pvz+nbins);

    draw_gr(data_gr_pvx, data_height_pvx, data_floor_pvx, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in x [#mum]", figdir + "/pvx");
    draw_gr(data_gr_pvy, data_height_pvy, data_floor_pvy, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in y [#mum]", figdir + "/pvy");
    draw_gr(data_gr_pvz, data_height_pvz, data_floor_pvz, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV resolution in z [#mum]", figdir + "/pvz");
    draw_gr(data_gr_pullx, 1.5, 0.5, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in x [#mum]", figdir + "/pullx");
    draw_gr(data_gr_pully, 1.5, 0.5, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in y [#mum]", figdir + "/pully");
    draw_gr(data_gr_pullz, 1.5, 0.5, datatype_text, "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "PV pull in z [#mum]", figdir + "/pullz");

    return 0;
}
