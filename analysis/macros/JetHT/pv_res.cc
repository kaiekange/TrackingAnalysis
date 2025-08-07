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
#include "../../functions/fit_compare.cc"

const TString figdir = "../../figures/"+datatype+"/pv_res/compare/";
const int nbins = 20;

int pv_res(int idx) {

    setTDRStyle();

    TFile *datafile = TFile::Open("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/tuples/JetHT_data2022.root");
    TTree *datatree = (TTree*)datafile->Get("mytree");
    TFile *mcfile = TFile::Open("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/tuples/JetHT_mc2022.root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    std::ifstream infile("../../json/"+datatype+"/binning.json");
    nlohmann::json binning;
    infile >> binning;

    std::vector<float> pv_SumTrackPt2_sqrt_edges = binning["pv_SumTrackPt2_sqrt"].get<std::vector<float>>();

    TString ptcut_title = Form("%.2f<#sqrt{#sum#it{p_{T}}^{2}}<%.2f GeV", pv_SumTrackPt2_sqrt_edges[idx], pv_SumTrackPt2_sqrt_edges[idx+1]);
    TCut ptcut = Form("sqrt(pv_SumTrackPt2) > %f && sqrt(pv_SumTrackPt2) < %f", pv_SumTrackPt2_sqrt_edges[idx], pv_SumTrackPt2_sqrt_edges[idx+1]);
    TCut xnull_cut = "pv_x_p1 != -777 && pv_x_p2 != -777";
    TCut ynull_cut = "pv_y_p1 != -777 && pv_y_p2 != -777";
    TCut znull_cut = "pv_z_p1 != -777 && pv_z_p2 != -777";

    TH1F * h_diff_x_tmp = new TH1F("h_diff_x_tmp", "", 200, -100, 100); 
    TH1F * h_diff_y_tmp = new TH1F("h_diff_y_tmp", "", 200, -100, 100); 
    TH1F * h_diff_z_tmp = new TH1F("h_diff_z_tmp", "", 200, -200, 200); 
    TH1F * h_pull_x_tmp = new TH1F("h_pull_x_tmp", "", 200, -10, 10); 
    TH1F * h_pull_y_tmp = new TH1F("h_pull_y_tmp", "", 200, -10, 10); 
    TH1F * h_pull_z_tmp = new TH1F("h_pull_z_tmp", "", 200, -10, 10); 

    datatree->Project("h_diff_x_tmp", "(pv_x_p1 - pv_x_p2)/sqrt(2)", ptcut+xnull_cut);
    datatree->Project("h_diff_y_tmp", "(pv_y_p1 - pv_y_p2)/sqrt(2)", ptcut+ynull_cut);
    datatree->Project("h_diff_z_tmp", "(pv_z_p1 - pv_z_p2)/sqrt(2)", ptcut+znull_cut);
    datatree->Project("h_pull_x_tmp", "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))", ptcut+xnull_cut);
    datatree->Project("h_pull_y_tmp", "(pv_y_p1 - pv_y_p2)/sqrt(pow(pv_yError_p1,2)+pow(pv_yError_p2,2))", ptcut+ynull_cut);
    datatree->Project("h_pull_z_tmp", "(pv_z_p1 - pv_z_p2)/sqrt(pow(pv_zError_p1,2)+pow(pv_zError_p2,2))", ptcut+znull_cut);

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

    TH1F *h_diff_x_data = new TH1F("h_diff_x_data", ptcut_title+";(#it{x}_{1}-#it{x}_{2})/#sqrt{2} [#mum];# PV", 200, diff_x_mean-8*diff_x_stddev, diff_x_mean+8*diff_x_stddev);
    TH1F *h_diff_y_data = new TH1F("h_diff_y_data", ptcut_title+";(#it{y}_{1}-#it{y}_{2})/#sqrt{2} [#mum];# PV", 200, diff_y_mean-8*diff_y_stddev, diff_y_mean+8*diff_y_stddev);
    TH1F *h_diff_z_data = new TH1F("h_diff_z_data", ptcut_title+";(#it{z}_{1}-#it{z}_{2})/#sqrt{2} [mm];# PV", 200, diff_z_mean-8*diff_z_stddev, diff_z_mean+8*diff_z_stddev);
    TH1F *h_pull_x_data = new TH1F("h_pull_x_data", ptcut_title+";(#it{x}_{1}-#it{x}_{2})/#sqrt{#Delta#it{x}_{1}^{2}+#Delta#it{x}_{2}^{2}};# PV", 200, pull_x_mean-8*pull_x_stddev, pull_x_mean+8*pull_x_stddev);
    TH1F *h_pull_y_data = new TH1F("h_pull_y_data", ptcut_title+";(#it{y}_{1}-#it{y}_{2})/#sqrt{#Delta#it{y}_{1}^{2}+#Delta#it{y}_{2}^{2}};# PV", 200, pull_y_mean-8*pull_y_stddev, pull_y_mean+8*pull_y_stddev);
    TH1F *h_pull_z_data = new TH1F("h_pull_z_data", ptcut_title+";(#it{z}_{1}-#it{z}_{2})/#sqrt{#Delta#it{z}_{1}^{2}+#Delta#it{z}_{2}^{2}};# PV", 200, pull_z_mean-8*pull_z_stddev, pull_z_mean+8*pull_z_stddev);

    datatree->Project("h_diff_x_data", "(pv_x_p1 - pv_x_p2)/sqrt(2)", ptcut+xnull_cut);
    datatree->Project("h_diff_y_data", "(pv_y_p1 - pv_y_p2)/sqrt(2)", ptcut+ynull_cut);
    datatree->Project("h_diff_z_data", "(pv_z_p1 - pv_z_p2)/sqrt(2)", ptcut+znull_cut);
    datatree->Project("h_pull_x_data", "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))", ptcut+xnull_cut);
    datatree->Project("h_pull_y_data", "(pv_y_p1 - pv_y_p2)/sqrt(pow(pv_yError_p1,2)+pow(pv_yError_p2,2))", ptcut+ynull_cut);
    datatree->Project("h_pull_z_data", "(pv_z_p1 - pv_z_p2)/sqrt(pow(pv_zError_p1,2)+pow(pv_zError_p2,2))", ptcut+znull_cut);

    TH1F *h_diff_x_mc = new TH1F("h_diff_x_mc", ptcut_title+";(#it{x}_{1}-#it{x}_{2})/#sqrt{2} [#mum];# PV", 200, diff_x_mean-8*diff_x_stddev, diff_x_mean+8*diff_x_stddev);
    TH1F *h_diff_y_mc = new TH1F("h_diff_y_mc", ptcut_title+";(#it{y}_{1}-#it{y}_{2})/#sqrt{2} [#mum];# PV", 200, diff_y_mean-8*diff_y_stddev, diff_y_mean+8*diff_y_stddev);
    TH1F *h_diff_z_mc = new TH1F("h_diff_z_mc", ptcut_title+";(#it{z}_{1}-#it{z}_{2})/#sqrt{2} [mm];# PV", 200, diff_z_mean-8*diff_z_stddev, diff_z_mean+8*diff_z_stddev);
    TH1F *h_pull_x_mc = new TH1F("h_pull_x_mc", ptcut_title+";(#it{x}_{1}-#it{x}_{2})/#sqrt{#Delta#it{x}_{1}^{2}+#Delta#it{x}_{2}^{2}};# PV", 200, pull_x_mean-8*pull_x_stddev, pull_x_mean+8*pull_x_stddev);
    TH1F *h_pull_y_mc = new TH1F("h_pull_y_mc", ptcut_title+";(#it{y}_{1}-#it{y}_{2})/#sqrt{#Delta#it{y}_{1}^{2}+#Delta#it{y}_{2}^{2}};# PV", 200, pull_y_mean-8*pull_y_stddev, pull_y_mean+8*pull_y_stddev);
    TH1F *h_pull_z_mc = new TH1F("h_pull_z_mc", ptcut_title+";(#it{z}_{1}-#it{z}_{2})/#sqrt{#Delta#it{z}_{1}^{2}+#Delta#it{z}_{2}^{2}};# PV", 200, pull_z_mean-8*pull_z_stddev, pull_z_mean+8*pull_z_stddev);

    mctree->Project("h_diff_x_mc", "(pv_x_p1 - pv_x_p2)/sqrt(2)", ptcut+xnull_cut);
    mctree->Project("h_diff_y_mc", "(pv_y_p1 - pv_y_p2)/sqrt(2)", ptcut+ynull_cut);
    mctree->Project("h_diff_z_mc", "(pv_z_p1 - pv_z_p2)/sqrt(2)", ptcut+znull_cut);
    mctree->Project("h_pull_x_mc", "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))", ptcut+xnull_cut);
    mctree->Project("h_pull_y_mc", "(pv_y_p1 - pv_y_p2)/sqrt(pow(pv_yError_p1,2)+pow(pv_yError_p2,2))", ptcut+ynull_cut);
    mctree->Project("h_pull_z_mc", "(pv_z_p1 - pv_z_p2)/sqrt(pow(pv_zError_p1,2)+pow(pv_zError_p2,2))", ptcut+znull_cut);

    auto result_reso_pvx = fit_compare(h_diff_x_data, h_diff_x_mc, figdir+Form("pvx_fit/pt_%d", idx), 0.01);
    auto result_reso_pvy = fit_compare(h_diff_y_data, h_diff_y_mc, figdir+Form("pvy_fit/pt_%d", idx), 0.01);
    auto result_reso_pvz = fit_compare(h_diff_z_data, h_diff_z_mc, figdir+Form("pvz_fit/pt_%d", idx), 0.01);

    auto result_reso_pullx = fit_compare(h_pull_x_data, h_pull_x_mc, figdir+Form("pullx_fit/pt_%d", idx), 0.01);
    auto result_reso_pully = fit_compare(h_pull_y_data, h_pull_y_mc, figdir+Form("pully_fit/pt_%d", idx), 0.01);
    auto result_reso_pullz = fit_compare(h_pull_z_data, h_pull_z_mc, figdir+Form("pullz_fit/pt_%d", idx), 0.01);

    nlohmann::json resojson;
    resojson["sumpt2_sqrt"] = (pv_SumTrackPt2_sqrt_edges[idx] + pv_SumTrackPt2_sqrt_edges[idx+1])/2;

    resojson["reso_pvx_data"] = result_reso_pvx.first;
    resojson["reso_pvy_data"] = result_reso_pvy.first;
    resojson["reso_pvz_data"] = result_reso_pvz.first;
    resojson["reso_pullx_data"] = result_reso_pullx.first;
    resojson["reso_pully_data"] = result_reso_pully.first;
    resojson["reso_pullz_data"] = result_reso_pullz.first;

    resojson["reso_pvx_mc"] = result_reso_pvx.second;
    resojson["reso_pvy_mc"] = result_reso_pvy.second;
    resojson["reso_pvz_mc"] = result_reso_pvz.second;
    resojson["reso_pullx_mc"] = result_reso_pullx.second;
    resojson["reso_pully_mc"] = result_reso_pully.second;
    resojson["reso_pullz_mc"] = result_reso_pullz.second;


    std::ofstream outFile("../../json/"+datatype+Form("/pv_res/compare/fit_%d.json",idx));
    outFile << resojson.dump(4);
    outFile.close();

    return 0;
}
