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
#include "../../functions/fit_res.cc"

const TString figdir = "../../figures/"+datatype+"/pv_res/"+sampletype+"/";
const int nbins = 100;

int pv_res_analysis(int idx) {

    setTDRStyle();

    std::cout << anaroot << std::endl;
    TFile *myfile = TFile::Open(anaroot);
    TTree *mytree = (TTree*)myfile->Get("mytree");

    std::ifstream infile("../../json/"+datatype+"/binning.json");
    nlohmann::json binning;
    infile >> binning;

    std::vector<float> pv_SumTrackPt2_sqrt_edges = binning["pv_SumTrackPt2_sqrt"].get<std::vector<float>>();

    TString ptcut_title = Form("%.2f<#sqrt{#sum#it{p_{T}}^{2}}<%.2f GeV", pv_SumTrackPt2_sqrt_edges[idx], pv_SumTrackPt2_sqrt_edges[idx+1]);
    TCut ptcut = Form("sqrt(pv_SumTrackPt2) > %f && sqrt(pv_SumTrackPt2) < %f", pv_SumTrackPt2_sqrt_edges[idx], pv_SumTrackPt2_sqrt_edges[idx+1]);
    TCut xnull_cut = "pv_x_p1 != -777 && pv_x_p2 != -777";
    TCut ynull_cut = "pv_y_p1 != -777 && pv_y_p2 != -777";
    TCut znull_cut = "pv_z_p1 != -777 && pv_z_p2 != -777";

    TH1F * h_diff_x_tmp = new TH1F("h_diff_x_tmp", "", 200, -300, 300); 
    TH1F * h_diff_y_tmp = new TH1F("h_diff_y_tmp", "", 200, -300, 300); 
    TH1F * h_diff_z_tmp = new TH1F("h_diff_z_tmp", "", 200, -300, 300); 
    TH1F * h_pull_x_tmp = new TH1F("h_pull_x_tmp", "", 200, -10, 10); 
    TH1F * h_pull_y_tmp = new TH1F("h_pull_y_tmp", "", 200, -10, 10); 
    TH1F * h_pull_z_tmp = new TH1F("h_pull_z_tmp", "", 200, -10, 10); 

    mytree->Project("h_diff_x_tmp", "(pv_x_p1 - pv_x_p2)/sqrt(2)", ptcut+xnull_cut);
    mytree->Project("h_diff_y_tmp", "(pv_y_p1 - pv_y_p2)/sqrt(2)", ptcut+ynull_cut);
    mytree->Project("h_diff_z_tmp", "(pv_z_p1 - pv_z_p2)/sqrt(2)", ptcut+znull_cut);
    mytree->Project("h_pull_x_tmp", "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))", ptcut+xnull_cut);
    mytree->Project("h_pull_y_tmp", "(pv_y_p1 - pv_y_p2)/sqrt(pow(pv_yError_p1,2)+pow(pv_yError_p2,2))", ptcut+ynull_cut);
    mytree->Project("h_pull_z_tmp", "(pv_z_p1 - pv_z_p2)/sqrt(pow(pv_zError_p1,2)+pow(pv_zError_p2,2))", ptcut+znull_cut);

    float diff_x_mean = h_diff_x_tmp->GetMean();
    float diff_y_mean = h_diff_y_tmp->GetMean();
    float diff_z_mean = h_diff_z_tmp->GetMean();
    float pull_x_mean = h_pull_x_tmp->GetMean();
    float pull_y_mean = h_pull_y_tmp->GetMean();
    float pull_z_mean = h_pull_z_tmp->GetMean();

    float diff_x_stddev = h_diff_x_tmp->GetStdDev();
    float diff_y_stddev = h_diff_y_tmp->GetStdDev();
    float diff_z_stddev = h_diff_z_tmp->GetStdDev();
    float pull_x_stddev = h_pull_x_tmp->GetStdDev();
    float pull_y_stddev = h_pull_y_tmp->GetStdDev();
    float pull_z_stddev = h_pull_z_tmp->GetStdDev();

    TH1F *h_diff_x = new TH1F("h_diff_x", ptcut_title+";(#it{x}_{1}-#it{x}_{2})/#sqrt{2} [#mum];# PV", 500, diff_x_mean-8*diff_x_stddev, diff_x_mean+8*diff_x_stddev);
    TH1F *h_diff_y = new TH1F("h_diff_y", ptcut_title+";(#it{y}_{1}-#it{y}_{2})/#sqrt{2} [#mum];# PV", 500, diff_y_mean-8*diff_y_stddev, diff_y_mean+8*diff_y_stddev);
    TH1F *h_diff_z = new TH1F("h_diff_z", ptcut_title+";(#it{z}_{1}-#it{z}_{2})/#sqrt{2} [mm];# PV", 500, diff_z_mean-8*diff_z_stddev, diff_z_mean+8*diff_z_stddev);
    TH1F *h_pull_x = new TH1F("h_pull_x", ptcut_title+";(#it{x}_{1}-#it{x}_{2})/#sqrt{#Delta#it{x}_{1}^{2}+#Delta#it{x}_{2}^{2}};# PV", 500, pull_x_mean-8*pull_x_stddev, pull_x_mean+8*pull_x_stddev);
    TH1F *h_pull_y = new TH1F("h_pull_y", ptcut_title+";(#it{y}_{1}-#it{y}_{2})/#sqrt{#Delta#it{y}_{1}^{2}+#Delta#it{y}_{2}^{2}};# PV", 500, pull_y_mean-8*pull_y_stddev, pull_y_mean+8*pull_y_stddev);
    TH1F *h_pull_z = new TH1F("h_pull_z", ptcut_title+";(#it{z}_{1}-#it{z}_{2})/#sqrt{#Delta#it{z}_{1}^{2}+#Delta#it{z}_{2}^{2}};# PV", 500, pull_z_mean-8*pull_z_stddev, pull_z_mean+8*pull_z_stddev);

    mytree->Project("h_diff_x", "(pv_x_p1 - pv_x_p2)/sqrt(2)", ptcut+xnull_cut);
    mytree->Project("h_diff_y", "(pv_y_p1 - pv_y_p2)/sqrt(2)", ptcut+ynull_cut);
    mytree->Project("h_diff_z", "(pv_z_p1 - pv_z_p2)/sqrt(2)", ptcut+znull_cut);
    mytree->Project("h_pull_x", "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))", ptcut+xnull_cut);
    mytree->Project("h_pull_y", "(pv_y_p1 - pv_y_p2)/sqrt(pow(pv_yError_p1,2)+pow(pv_yError_p2,2))", ptcut+ynull_cut);
    mytree->Project("h_pull_z", "(pv_z_p1 - pv_z_p2)/sqrt(pow(pv_zError_p1,2)+pow(pv_zError_p2,2))", ptcut+znull_cut);

    auto result_reso_pvx = fit_draw(h_diff_x, figdir+Form("pvx_fit/pt_%d", idx), 0.01);
    auto result_reso_pvy = fit_draw(h_diff_y, figdir+Form("pvy_fit/pt_%d", idx), 0.01);
    auto result_reso_pvz = fit_draw(h_diff_z, figdir+Form("pvz_fit/pt_%d", idx), 0.01);

    auto result_reso_pullx = fit_draw(h_pull_x, figdir+Form("pullx_fit/pt_%d", idx), 0.01);
    auto result_reso_pully = fit_draw(h_pull_y, figdir+Form("pully_fit/pt_%d", idx), 0.01);
    auto result_reso_pullz = fit_draw(h_pull_z, figdir+Form("pullz_fit/pt_%d", idx), 0.01);

    nlohmann::json resojson;
    resojson["sumpt2_sqrt"] = (pv_SumTrackPt2_sqrt_edges[idx] + pv_SumTrackPt2_sqrt_edges[idx+1])/2;

    resojson["reso_pvx"] = result_reso_pvx.first;
    resojson["reso_pvy"] = result_reso_pvy.first;
    resojson["reso_pvz"] = result_reso_pvz.first;
    resojson["reso_pullx"] = result_reso_pullx.first;
    resojson["reso_pully"] = result_reso_pully.first;
    resojson["reso_pullz"] = result_reso_pullz.first;

    resojson["reso2_pvx"] = result_reso_pvx.second;
    resojson["reso2_pvy"] = result_reso_pvy.second;
    resojson["reso2_pvz"] = result_reso_pvz.second;
    resojson["reso2_pullx"] = result_reso_pullx.second;
    resojson["reso2_pully"] = result_reso_pully.second;
    resojson["reso2_pullz"] = result_reso_pullz.second;

    std::ofstream outFile("../../json/"+datatype+"/pv_res/"+sampletype+Form("/fit_%d.json",idx));
    outFile << resojson.dump(4);
    outFile.close();

    return 0;
}
