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

const TString figdir = "../../figures/"+datatype+"/ip_res/compare/";

int ip_res(int idx) {

    setTDRStyle();

    TFile *datafile = TFile::Open("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/tuples/JetHT_data2022.root");
    TTree *datatree = (TTree*)datafile->Get("mytree");
    TFile *mcfile = TFile::Open("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/tuples/JetHT_mc2022.root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    float low_edge = -3.14 + 0.02*idx;
    float high_edge = -3.12 + 0.02*idx;

    TString phicut_title = Form("%.2f<#it{#phi}<%.2f", low_edge, high_edge);
    TCut phicut = Form("pv_trk_phi > %f && pv_trk_phi < %f", low_edge, high_edge);

    TH1F *h_d0_phi_lopt_tmp = new TH1F("h_d0_phi_lopt_tmp", "", 200, -2000, 2000);
    TH1F *h_dz_phi_lopt_tmp = new TH1F("h_dz_phi_lopt_tmp", "", 200, -8000, 8000);
    TH1F *h_d0_phi_hipt_tmp = new TH1F("h_d0_phi_hipt_tmp", "", 200, -800, 800);
    TH1F *h_dz_phi_hipt_tmp = new TH1F("h_dz_phi_hipt_tmp", "", 200, -3000, 3000);
    TH1F *h_d0_phi_ulpt_tmp = new TH1F("h_d0_phi_ulpt_tmp", "", 200, -300, 300);
    TH1F *h_dz_phi_ulpt_tmp = new TH1F("h_dz_phi_ulpt_tmp", "", 200, -1500, 1500);

    datatree->Project("h_d0_phi_lopt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_dz_phi_lopt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_d0_phi_hipt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_dz_phi_hipt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_d0_phi_ulpt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");
    datatree->Project("h_dz_phi_ulpt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");

    float d0_phi_lopt_mean = h_d0_phi_lopt_tmp->GetMean();
    float dz_phi_lopt_mean = h_dz_phi_lopt_tmp->GetMean();
    float d0_phi_hipt_mean = h_d0_phi_hipt_tmp->GetMean();
    float dz_phi_hipt_mean = h_dz_phi_hipt_tmp->GetMean();
    float d0_phi_ulpt_mean = h_d0_phi_ulpt_tmp->GetMean();
    float dz_phi_ulpt_mean = h_dz_phi_ulpt_tmp->GetMean();

    float d0_phi_lopt_stddev = h_d0_phi_lopt_tmp->GetStdDev();
    float dz_phi_lopt_stddev = h_dz_phi_lopt_tmp->GetStdDev();
    float d0_phi_hipt_stddev = h_d0_phi_hipt_tmp->GetStdDev();
    float dz_phi_hipt_stddev = h_dz_phi_hipt_tmp->GetStdDev();
    float d0_phi_ulpt_stddev = h_d0_phi_ulpt_tmp->GetStdDev();
    float dz_phi_ulpt_stddev = h_dz_phi_ulpt_tmp->GetStdDev();

    delete h_d0_phi_lopt_tmp;
    delete h_dz_phi_lopt_tmp;
    delete h_d0_phi_hipt_tmp;
    delete h_dz_phi_hipt_tmp;
    delete h_d0_phi_ulpt_tmp;
    delete h_dz_phi_ulpt_tmp;

    TH1F *h_data_d0_phi_lopt = new TH1F("h_data_d0_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_phi_lopt_mean-8*d0_phi_lopt_stddev, d0_phi_lopt_mean+8*d0_phi_lopt_stddev);
    TH1F *h_data_dz_phi_lopt = new TH1F("h_data_dz_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_phi_lopt_mean-8*dz_phi_lopt_stddev, dz_phi_lopt_mean+8*dz_phi_lopt_stddev);
    TH1F *h_data_d0_phi_hipt = new TH1F("h_data_d0_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_phi_hipt_mean-8*d0_phi_hipt_stddev, d0_phi_hipt_mean+8*d0_phi_hipt_stddev);
    TH1F *h_data_dz_phi_hipt = new TH1F("h_data_dz_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_phi_hipt_mean-8*dz_phi_hipt_stddev, dz_phi_hipt_mean+8*dz_phi_hipt_stddev);
    TH1F *h_data_d0_phi_ulpt = new TH1F("h_data_d0_phi_ulpt", "#splitline{"+phicut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_phi_ulpt_mean-8*d0_phi_ulpt_stddev, d0_phi_ulpt_mean+8*d0_phi_ulpt_stddev);
    TH1F *h_data_dz_phi_ulpt = new TH1F("h_data_dz_phi_ulpt", "#splitline{"+phicut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_phi_ulpt_mean-8*dz_phi_ulpt_stddev, dz_phi_ulpt_mean+8*dz_phi_ulpt_stddev);
    
    datatree->Project("h_data_d0_phi_lopt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_data_dz_phi_lopt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_data_d0_phi_hipt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_data_dz_phi_hipt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_data_d0_phi_ulpt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");
    datatree->Project("h_data_dz_phi_ulpt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");

    TH1F *h_mc_d0_phi_lopt = new TH1F("h_mc_d0_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_phi_lopt_mean-8*d0_phi_lopt_stddev, d0_phi_lopt_mean+8*d0_phi_lopt_stddev);
    TH1F *h_mc_dz_phi_lopt = new TH1F("h_mc_dz_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_phi_lopt_mean-8*dz_phi_lopt_stddev, dz_phi_lopt_mean+8*dz_phi_lopt_stddev);
    TH1F *h_mc_d0_phi_hipt = new TH1F("h_mc_d0_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_phi_hipt_mean-8*d0_phi_hipt_stddev, d0_phi_hipt_mean+8*d0_phi_hipt_stddev);
    TH1F *h_mc_dz_phi_hipt = new TH1F("h_mc_dz_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_phi_hipt_mean-8*dz_phi_hipt_stddev, dz_phi_hipt_mean+8*dz_phi_hipt_stddev);
    TH1F *h_mc_d0_phi_ulpt = new TH1F("h_mc_d0_phi_ulpt", "#splitline{"+phicut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_phi_ulpt_mean-8*d0_phi_ulpt_stddev, d0_phi_ulpt_mean+8*d0_phi_ulpt_stddev);
    TH1F *h_mc_dz_phi_ulpt = new TH1F("h_mc_dz_phi_ulpt", "#splitline{"+phicut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_phi_ulpt_mean-8*dz_phi_ulpt_stddev, dz_phi_ulpt_mean+8*dz_phi_ulpt_stddev);

    mctree->Project("h_mc_d0_phi_lopt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    mctree->Project("h_mc_dz_phi_lopt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    mctree->Project("h_mc_d0_phi_hipt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    mctree->Project("h_mc_dz_phi_hipt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    mctree->Project("h_mc_d0_phi_ulpt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");
    mctree->Project("h_mc_dz_phi_ulpt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");

    auto result_d0_phi_lopt = fit_compare(h_data_d0_phi_lopt, h_mc_d0_phi_lopt, figdir+Form("ippv_xy_fit/phi_lopt_%d", idx), 0.1);
    auto result_dz_phi_lopt = fit_compare(h_data_dz_phi_lopt, h_mc_dz_phi_lopt, figdir+Form("ippv_z_fit/phi_lopt_%d", idx), 0.1);
    auto result_d0_phi_hipt = fit_compare(h_data_d0_phi_hipt, h_mc_d0_phi_hipt, figdir+Form("ippv_xy_fit/phi_hipt_%d", idx), 0.1);
    auto result_dz_phi_hipt = fit_compare(h_data_dz_phi_hipt, h_mc_dz_phi_hipt, figdir+Form("ippv_z_fit/phi_hipt_%d", idx), 0.1);
    auto result_d0_phi_ulpt = fit_compare(h_data_d0_phi_ulpt, h_mc_d0_phi_ulpt, figdir+Form("ippv_xy_fit/phi_ulpt_%d", idx), 0.1);
    auto result_dz_phi_ulpt = fit_compare(h_data_dz_phi_ulpt, h_mc_dz_phi_ulpt, figdir+Form("ippv_z_fit/phi_ulpt_%d", idx), 0.1);

    nlohmann::json resojson;
    resojson["phi"] = -3.13 + 0.02*idx;

    resojson["reso_data_d0_phi_lopt"] = result_d0_phi_lopt.first;
    resojson["reso_data_dz_phi_lopt"] = result_dz_phi_lopt.first;
    resojson["reso_data_d0_phi_hipt"] = result_d0_phi_hipt.first;
    resojson["reso_data_dz_phi_hipt"] = result_dz_phi_hipt.first;
    resojson["reso_data_d0_phi_ulpt"] = result_d0_phi_ulpt.first;
    resojson["reso_data_dz_phi_ulpt"] = result_dz_phi_ulpt.first;

    resojson["reso_mc_d0_phi_lopt"] = result_d0_phi_lopt.second;
    resojson["reso_mc_dz_phi_lopt"] = result_dz_phi_lopt.second;
    resojson["reso_mc_d0_phi_hipt"] = result_d0_phi_hipt.second;
    resojson["reso_mc_dz_phi_hipt"] = result_dz_phi_hipt.second;
    resojson["reso_mc_d0_phi_ulpt"] = result_d0_phi_ulpt.second;
    resojson["reso_mc_dz_phi_ulpt"] = result_dz_phi_ulpt.second;

    std::ofstream outFile("../../json/"+datatype+Form("/ip_res/compare/phi_fit_%d.json",idx));
    outFile << resojson.dump(4);
    outFile.close();

    return 0;
}
