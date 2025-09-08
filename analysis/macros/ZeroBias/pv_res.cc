#include <vector>
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>
#include <algorithm>
#include <nlohmann/json.hpp>

const TString datatype_text = "Unbiased collision events";

#include "../../functions/fit_compare.cc"

int pv_res(int iera, int idx) {

    TString eras[] = {"preEE", "postEE"};

    TString era = eras[iera];

    TString figdir = "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias_"+era+"/pv_res/";

    TFile *datafile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/all_skimmed_2022_data_"+era+".root");
    TTree *datatree = (TTree*)datafile->Get("mytree");
    TFile *mcfile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/all_skimmed_2022_mc_"+era+"_corr.root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    std::ifstream infile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/ZeroBias_"+era+"/binning.json");
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

    TH1F *h_data_diff_x = new TH1F("h_data_diff_x", ptcut_title+";(#it{x}_{1}-#it{x}_{2})/#sqrt{2} [#mum];# PV", 500, diff_x_mean-8*diff_x_stddev, diff_x_mean+8*diff_x_stddev);
    TH1F *h_data_diff_y = new TH1F("h_data_diff_y", ptcut_title+";(#it{y}_{1}-#it{y}_{2})/#sqrt{2} [#mum];# PV", 500, diff_y_mean-8*diff_y_stddev, diff_y_mean+8*diff_y_stddev);
    TH1F *h_data_diff_z = new TH1F("h_data_diff_z", ptcut_title+";(#it{z}_{1}-#it{z}_{2})/#sqrt{2} [mm];# PV", 500, diff_z_mean-8*diff_z_stddev, diff_z_mean+8*diff_z_stddev);
    TH1F *h_data_pull_x = new TH1F("h_data_pull_x", ptcut_title+";(#it{x}_{1}-#it{x}_{2})/#sqrt{#Delta#it{x}_{1}^{2}+#Delta#it{x}_{2}^{2}};# PV", 500, pull_x_mean-8*pull_x_stddev, pull_x_mean+8*pull_x_stddev);
    TH1F *h_data_pull_y = new TH1F("h_data_pull_y", ptcut_title+";(#it{y}_{1}-#it{y}_{2})/#sqrt{#Delta#it{y}_{1}^{2}+#Delta#it{y}_{2}^{2}};# PV", 500, pull_y_mean-8*pull_y_stddev, pull_y_mean+8*pull_y_stddev);
    TH1F *h_data_pull_z = new TH1F("h_data_pull_z", ptcut_title+";(#it{z}_{1}-#it{z}_{2})/#sqrt{#Delta#it{z}_{1}^{2}+#Delta#it{z}_{2}^{2}};# PV", 500, pull_z_mean-8*pull_z_stddev, pull_z_mean+8*pull_z_stddev);

    datatree->Project("h_data_diff_x", "(pv_x_p1 - pv_x_p2)/sqrt(2)", ptcut+xnull_cut);
    datatree->Project("h_data_diff_y", "(pv_y_p1 - pv_y_p2)/sqrt(2)", ptcut+ynull_cut);
    datatree->Project("h_data_diff_z", "(pv_z_p1 - pv_z_p2)/sqrt(2)", ptcut+znull_cut);
    datatree->Project("h_data_pull_x", "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))", ptcut+xnull_cut);
    datatree->Project("h_data_pull_y", "(pv_y_p1 - pv_y_p2)/sqrt(pow(pv_yError_p1,2)+pow(pv_yError_p2,2))", ptcut+ynull_cut);
    datatree->Project("h_data_pull_z", "(pv_z_p1 - pv_z_p2)/sqrt(pow(pv_zError_p1,2)+pow(pv_zError_p2,2))", ptcut+znull_cut);

    TH1F *h_mc_diff_x = new TH1F("h_mc_diff_x", "", 500, diff_x_mean-8*diff_x_stddev, diff_x_mean+8*diff_x_stddev);
    TH1F *h_mc_diff_y = new TH1F("h_mc_diff_y", "", 500, diff_y_mean-8*diff_y_stddev, diff_y_mean+8*diff_y_stddev);
    TH1F *h_mc_diff_z = new TH1F("h_mc_diff_z", "", 500, diff_z_mean-8*diff_z_stddev, diff_z_mean+8*diff_z_stddev);
    TH1F *h_mc_pull_x = new TH1F("h_mc_pull_x", "", 500, pull_x_mean-8*pull_x_stddev, pull_x_mean+8*pull_x_stddev);
    TH1F *h_mc_pull_y = new TH1F("h_mc_pull_y", "", 500, pull_y_mean-8*pull_y_stddev, pull_y_mean+8*pull_y_stddev);
    TH1F *h_mc_pull_z = new TH1F("h_mc_pull_z", "", 500, pull_z_mean-8*pull_z_stddev, pull_z_mean+8*pull_z_stddev);

    mctree->Project("h_mc_diff_x", "(pv_x_p1 - pv_x_p2)/sqrt(2)", TString("PU_factor*(") + (ptcut+xnull_cut).GetTitle() + ")");
    mctree->Project("h_mc_diff_y", "(pv_y_p1 - pv_y_p2)/sqrt(2)", TString("PU_factor*(") + (ptcut+ynull_cut).GetTitle() + ")");
    mctree->Project("h_mc_diff_z", "(pv_z_p1 - pv_z_p2)/sqrt(2)", TString("PU_factor*(") + (ptcut+znull_cut).GetTitle() + ")");
    mctree->Project("h_mc_pull_x", "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))", TString("PU_factor*(") + (ptcut+xnull_cut).GetTitle() + ")");
    mctree->Project("h_mc_pull_y", "(pv_y_p1 - pv_y_p2)/sqrt(pow(pv_yError_p1,2)+pow(pv_yError_p2,2))", TString("PU_factor*(") + (ptcut+ynull_cut).GetTitle() + ")");
    mctree->Project("h_mc_pull_z", "(pv_z_p1 - pv_z_p2)/sqrt(pow(pv_zError_p1,2)+pow(pv_zError_p2,2))", TString("PU_factor*(") + (ptcut+znull_cut).GetTitle() + ")");


    auto result_reso_pvx = fit_compare(h_data_diff_x, h_mc_diff_x, era, figdir+Form("pvx_fit/pt_%d", idx), 0.01);
    auto result_reso_pvy = fit_compare(h_data_diff_y, h_mc_diff_y, era, figdir+Form("pvy_fit/pt_%d", idx), 0.01);
    auto result_reso_pvz = fit_compare(h_data_diff_z, h_mc_diff_z, era, figdir+Form("pvz_fit/pt_%d", idx), 0.01);

    auto result_reso_pullx = fit_compare(h_data_pull_x, h_mc_pull_x, era, figdir+Form("pullx_fit/pt_%d", idx), 0.01);
    auto result_reso_pully = fit_compare(h_data_pull_y, h_mc_pull_y, era, figdir+Form("pully_fit/pt_%d", idx), 0.01);
    auto result_reso_pullz = fit_compare(h_data_pull_z, h_mc_pull_z, era, figdir+Form("pullz_fit/pt_%d", idx), 0.01);

    nlohmann::json resojson;

    resojson["sumpt2_sqrt"] = (pv_SumTrackPt2_sqrt_edges[idx] + pv_SumTrackPt2_sqrt_edges[idx+1])/2;

    resojson["reso_data_pvx"] = result_reso_pvx.first;
    resojson["reso_data_pvy"] = result_reso_pvy.first;
    resojson["reso_data_pvz"] = result_reso_pvz.first;

    resojson["reso_data_pullx"] = result_reso_pullx.first;
    resojson["reso_data_pully"] = result_reso_pully.first;
    resojson["reso_data_pullz"] = result_reso_pullz.first;

    resojson["reso_mc_pvx"] = result_reso_pvx.second;
    resojson["reso_mc_pvy"] = result_reso_pvy.second;
    resojson["reso_mc_pvz"] = result_reso_pvz.second;

    resojson["reso_mc_pullx"] = result_reso_pullx.second;
    resojson["reso_mc_pully"] = result_reso_pully.second;
    resojson["reso_mc_pullz"] = result_reso_pullz.second;

    std::ofstream outFile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/ZeroBias_"+era+Form("/pv_res/fit_%d.json",idx));
    outFile << resojson.dump(4);
    outFile.close();

    return 0;
}
