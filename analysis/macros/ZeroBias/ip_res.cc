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

int ip_res(int iera, int idx) {

    TString eras[] = {"preEE", "postEE"};

    TString era = eras[iera];

    TString figdir = "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias_"+era+"/ip_res/";

    TFile *datafile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/all_skimmed_2022_data_"+era+".root");
    TTree *datatree = (TTree*)datafile->Get("mytree");
    TFile *mcfile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/all_skimmed_2022_mc_"+era+"_corr.root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    std::ifstream infile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/ZeroBias_"+era+"/binning.json");
    nlohmann::json binning;
    infile >> binning;

    std::vector<float> pv_trk_pt_edges = binning["pv_trk_pt"].get<std::vector<float>>();
    std::vector<float> pv_trk_eta_edges = binning["pv_trk_eta"].get<std::vector<float>>();
    std::vector<float> pv_trk_phi_edges = binning["pv_trk_phi"].get<std::vector<float>>();

    TString ptcut_title = Form("%.3f<#it{p_{T}}<%.3f GeV", pv_trk_pt_edges[idx], pv_trk_pt_edges[idx+1]);
    TString etacut_title = Form("%.2f<#it{#eta}<%.2f", pv_trk_eta_edges[idx], pv_trk_eta_edges[idx+1]);
    TString phicut_title = Form("%.2f<#it{#phi}<%.2f", pv_trk_phi_edges[idx], pv_trk_phi_edges[idx+1]);
    TCut ptcut = Form("pv_trk_pt > %f && pv_trk_pt < %f", pv_trk_pt_edges[idx], pv_trk_pt_edges[idx+1]);
    TCut etacut = Form("pv_trk_eta > %f && pv_trk_eta < %f", pv_trk_eta_edges[idx], pv_trk_eta_edges[idx+1]);
    TCut phicut = Form("pv_trk_phi > %f && pv_trk_phi < %f", pv_trk_phi_edges[idx], pv_trk_phi_edges[idx+1]);

    TH1F *h_d0_pt_loeta_tmp = new TH1F("h_d0_pt_loeta_tmp", "", 200, -2000, 2000);
    TH1F *h_dz_pt_loeta_tmp = new TH1F("h_dz_pt_loeta_tmp", "", 200, -2000, 2000);
    TH1F *h_d0_pt_hieta_tmp = new TH1F("h_d0_pt_hieta_tmp", "", 200, -3000, 3000);
    TH1F *h_dz_pt_hieta_tmp = new TH1F("h_dz_pt_hieta_tmp", "", 200, -8000, 8000);

    TH1F *h_d0_eta_lopt_tmp = new TH1F("h_d0_eta_lopt_tmp", "", 200, -2000, 2000);
    TH1F *h_dz_eta_lopt_tmp = new TH1F("h_dz_eta_lopt_tmp", "", 200, -10000, 10000);
    TH1F *h_d0_eta_hipt_tmp = new TH1F("h_d0_eta_hipt_tmp", "", 200, -1000, 1000);
    TH1F *h_dz_eta_hipt_tmp = new TH1F("h_dz_eta_hipt_tmp", "", 200, -5000, 5000);

    TH1F *h_d0_phi_lopt_tmp = new TH1F("h_d0_phi_lopt_tmp", "", 200, -1500, 1500);
    TH1F *h_dz_phi_lopt_tmp = new TH1F("h_dz_phi_lopt_tmp", "", 200, -3000, 3000);
    TH1F *h_d0_phi_hipt_tmp = new TH1F("h_d0_phi_hipt_tmp", "", 200, -500, 500);
    TH1F *h_dz_phi_hipt_tmp = new TH1F("h_dz_phi_hipt_tmp", "", 200, -1000, 1000);

    datatree->Project("h_d0_pt_loeta_tmp", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    datatree->Project("h_dz_pt_loeta_tmp", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    datatree->Project("h_d0_pt_hieta_tmp", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");
    datatree->Project("h_dz_pt_hieta_tmp", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");

    datatree->Project("h_d0_eta_lopt_tmp", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_dz_eta_lopt_tmp", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_d0_eta_hipt_tmp", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_dz_eta_hipt_tmp", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");

    datatree->Project("h_d0_phi_lopt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_dz_phi_lopt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_d0_phi_hipt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_dz_phi_hipt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");

    float d0_pt_loeta_mean = h_d0_pt_loeta_tmp->GetMean();
    float dz_pt_loeta_mean = h_dz_pt_loeta_tmp->GetMean();
    float d0_pt_hieta_mean = h_d0_pt_hieta_tmp->GetMean();
    float dz_pt_hieta_mean = h_dz_pt_hieta_tmp->GetMean();

    float d0_eta_lopt_mean = h_d0_eta_lopt_tmp->GetMean();
    float dz_eta_lopt_mean = h_dz_eta_lopt_tmp->GetMean();
    float d0_eta_hipt_mean = h_d0_eta_hipt_tmp->GetMean();
    float dz_eta_hipt_mean = h_dz_eta_hipt_tmp->GetMean();

    float d0_phi_lopt_mean = h_d0_phi_lopt_tmp->GetMean();
    float dz_phi_lopt_mean = h_dz_phi_lopt_tmp->GetMean();
    float d0_phi_hipt_mean = h_d0_phi_hipt_tmp->GetMean();
    float dz_phi_hipt_mean = h_dz_phi_hipt_tmp->GetMean();

    float d0_pt_loeta_stddev = h_d0_pt_loeta_tmp->GetStdDev();
    float dz_pt_loeta_stddev = h_dz_pt_loeta_tmp->GetStdDev();
    float d0_pt_hieta_stddev = h_d0_pt_hieta_tmp->GetStdDev();
    float dz_pt_hieta_stddev = h_dz_pt_hieta_tmp->GetStdDev();

    float d0_eta_lopt_stddev = h_d0_eta_lopt_tmp->GetStdDev();
    float dz_eta_lopt_stddev = h_dz_eta_lopt_tmp->GetStdDev();
    float d0_eta_hipt_stddev = h_d0_eta_hipt_tmp->GetStdDev();
    float dz_eta_hipt_stddev = h_dz_eta_hipt_tmp->GetStdDev();

    float d0_phi_lopt_stddev = h_d0_phi_lopt_tmp->GetStdDev();
    float dz_phi_lopt_stddev = h_dz_phi_lopt_tmp->GetStdDev();
    float d0_phi_hipt_stddev = h_d0_phi_hipt_tmp->GetStdDev();
    float dz_phi_hipt_stddev = h_dz_phi_hipt_tmp->GetStdDev();

    delete h_d0_pt_loeta_tmp;
    delete h_dz_pt_loeta_tmp;
    delete h_d0_pt_hieta_tmp;
    delete h_dz_pt_hieta_tmp;

    delete h_d0_eta_lopt_tmp;
    delete h_dz_eta_lopt_tmp;
    delete h_d0_eta_hipt_tmp;
    delete h_dz_eta_hipt_tmp;

    delete h_d0_phi_lopt_tmp;
    delete h_dz_phi_lopt_tmp;
    delete h_d0_phi_hipt_tmp;
    delete h_dz_phi_hipt_tmp;

    TH1F *h_data_d0_pt_loeta = new TH1F("h_data_d0_pt_loeta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<1.4};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_pt_loeta_mean-8*d0_pt_loeta_stddev, d0_pt_loeta_mean+8*d0_pt_loeta_stddev);
    TH1F *h_data_dz_pt_loeta = new TH1F("h_data_dz_pt_loeta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<1.4};Track IP #it{d_{z}} [mm];# tracks", 500, dz_pt_loeta_mean-8*dz_pt_loeta_stddev, dz_pt_loeta_mean+8*dz_pt_loeta_stddev);
    TH1F *h_data_d0_pt_hieta = new TH1F("h_data_d0_pt_hieta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<3.0};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_pt_hieta_mean-8*d0_pt_hieta_stddev, d0_pt_hieta_mean+8*d0_pt_hieta_stddev);
    TH1F *h_data_dz_pt_hieta = new TH1F("h_data_dz_pt_hieta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<3.0};Track IP #it{d_{z}} [mm];# tracks", 500, dz_pt_hieta_mean-8*dz_pt_hieta_stddev, dz_pt_hieta_mean+8*dz_pt_hieta_stddev);

    TH1F *h_data_d0_eta_lopt = new TH1F("h_data_d0_eta_lopt", "#splitline{"+etacut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_eta_lopt_mean-8*d0_eta_lopt_stddev, d0_eta_lopt_mean+8*d0_eta_lopt_stddev);
    TH1F *h_data_dz_eta_lopt = new TH1F("h_data_dz_eta_lopt", "#splitline{"+etacut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} [mm];# tracks", 500, dz_eta_lopt_mean-8*dz_eta_lopt_stddev, dz_eta_lopt_mean+8*dz_eta_lopt_stddev);
    TH1F *h_data_d0_eta_hipt = new TH1F("h_data_d0_eta_hipt", "#splitline{"+etacut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_eta_hipt_mean-8*d0_eta_hipt_stddev, d0_eta_hipt_mean+8*d0_eta_hipt_stddev);
    TH1F *h_data_dz_eta_hipt = new TH1F("h_data_dz_eta_hipt", "#splitline{"+etacut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{z}} [mm];# tracks", 500, dz_eta_hipt_mean-8*dz_eta_hipt_stddev, dz_eta_hipt_mean+8*dz_eta_hipt_stddev);

    TH1F *h_data_d0_phi_lopt = new TH1F("h_data_d0_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_phi_lopt_mean-8*d0_phi_lopt_stddev, d0_phi_lopt_mean+8*d0_phi_lopt_stddev);
    TH1F *h_data_dz_phi_lopt = new TH1F("h_data_dz_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} [mm];# tracks", 500, dz_phi_lopt_mean-8*dz_phi_lopt_stddev, dz_phi_lopt_mean+8*dz_phi_lopt_stddev);
    TH1F *h_data_d0_phi_hipt = new TH1F("h_data_d0_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_phi_hipt_mean-8*d0_phi_hipt_stddev, d0_phi_hipt_mean+8*d0_phi_hipt_stddev);
    TH1F *h_data_dz_phi_hipt = new TH1F("h_data_dz_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{z}} [mm];# tracks", 500, dz_phi_hipt_mean-8*dz_phi_hipt_stddev, dz_phi_hipt_mean+8*dz_phi_hipt_stddev);

    datatree->Project("h_data_d0_pt_loeta", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    datatree->Project("h_data_dz_pt_loeta", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    datatree->Project("h_data_d0_pt_hieta", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");
    datatree->Project("h_data_dz_pt_hieta", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");

    datatree->Project("h_data_d0_eta_lopt", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_data_dz_eta_lopt", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_data_d0_eta_hipt", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_data_dz_eta_hipt", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");

    datatree->Project("h_data_d0_phi_lopt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_data_dz_phi_lopt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_data_d0_phi_hipt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_data_dz_phi_hipt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");

    TH1F *h_mc_d0_pt_loeta = new TH1F("h_mc_d0_pt_loeta", "", 500, d0_pt_loeta_mean-8*d0_pt_loeta_stddev, d0_pt_loeta_mean+8*d0_pt_loeta_stddev);
    TH1F *h_mc_dz_pt_loeta = new TH1F("h_mc_dz_pt_loeta", "", 500, dz_pt_loeta_mean-8*dz_pt_loeta_stddev, dz_pt_loeta_mean+8*dz_pt_loeta_stddev);
    TH1F *h_mc_d0_pt_hieta = new TH1F("h_mc_d0_pt_hieta", "", 500, d0_pt_hieta_mean-8*d0_pt_hieta_stddev, d0_pt_hieta_mean+8*d0_pt_hieta_stddev);
    TH1F *h_mc_dz_pt_hieta = new TH1F("h_mc_dz_pt_hieta", "", 500, dz_pt_hieta_mean-8*dz_pt_hieta_stddev, dz_pt_hieta_mean+8*dz_pt_hieta_stddev);

    TH1F *h_mc_d0_eta_lopt = new TH1F("h_mc_d0_eta_lopt", "", 500, d0_eta_lopt_mean-8*d0_eta_lopt_stddev, d0_eta_lopt_mean+8*d0_eta_lopt_stddev);
    TH1F *h_mc_dz_eta_lopt = new TH1F("h_mc_dz_eta_lopt", "", 500, dz_eta_lopt_mean-8*dz_eta_lopt_stddev, dz_eta_lopt_mean+8*dz_eta_lopt_stddev);
    TH1F *h_mc_d0_eta_hipt = new TH1F("h_mc_d0_eta_hipt", "", 500, d0_eta_hipt_mean-8*d0_eta_hipt_stddev, d0_eta_hipt_mean+8*d0_eta_hipt_stddev);
    TH1F *h_mc_dz_eta_hipt = new TH1F("h_mc_dz_eta_hipt", "", 500, dz_eta_hipt_mean-8*dz_eta_hipt_stddev, dz_eta_hipt_mean+8*dz_eta_hipt_stddev);

    TH1F *h_mc_d0_phi_lopt = new TH1F("h_mc_d0_phi_lopt", "", 500, d0_phi_lopt_mean-8*d0_phi_lopt_stddev, d0_phi_lopt_mean+8*d0_phi_lopt_stddev);
    TH1F *h_mc_dz_phi_lopt = new TH1F("h_mc_dz_phi_lopt", "", 500, dz_phi_lopt_mean-8*dz_phi_lopt_stddev, dz_phi_lopt_mean+8*dz_phi_lopt_stddev);
    TH1F *h_mc_d0_phi_hipt = new TH1F("h_mc_d0_phi_hipt", "", 500, d0_phi_hipt_mean-8*d0_phi_hipt_stddev, d0_phi_hipt_mean+8*d0_phi_hipt_stddev);
    TH1F *h_mc_dz_phi_hipt = new TH1F("h_mc_dz_phi_hipt", "", 500, dz_phi_hipt_mean-8*dz_phi_hipt_stddev, dz_phi_hipt_mean+8*dz_phi_hipt_stddev);

    mctree->Project("h_mc_d0_pt_loeta", "pv_trk_d0_pvunbiased", TString("PU_factor*(") + (ptcut+"abs(pv_trk_eta) < 1.4").GetTitle() + ")");
    mctree->Project("h_mc_dz_pt_loeta", "pv_trk_dz_pvunbiased", TString("PU_factor*(") + (ptcut+"abs(pv_trk_eta) < 1.4").GetTitle() + ")");
    mctree->Project("h_mc_d0_pt_hieta", "pv_trk_d0_pvunbiased", TString("PU_factor*(") + (ptcut+"abs(pv_trk_eta) < 3").GetTitle() + ")");
    mctree->Project("h_mc_dz_pt_hieta", "pv_trk_dz_pvunbiased", TString("PU_factor*(") + (ptcut+"abs(pv_trk_eta) < 3").GetTitle() + ")");

    mctree->Project("h_mc_d0_eta_lopt", "pv_trk_d0_pvunbiased", TString("PU_factor*(") + (etacut+"pv_trk_pt>0.1 && pv_trk_pt<1").GetTitle() + ")");
    mctree->Project("h_mc_dz_eta_lopt", "pv_trk_dz_pvunbiased", TString("PU_factor*(") + (etacut+"pv_trk_pt>0.1 && pv_trk_pt<1").GetTitle() + ")");
    mctree->Project("h_mc_d0_eta_hipt", "pv_trk_d0_pvunbiased", TString("PU_factor*(") + (etacut+"pv_trk_pt>1 && pv_trk_pt<3").GetTitle() + ")");
    mctree->Project("h_mc_dz_eta_hipt", "pv_trk_dz_pvunbiased", TString("PU_factor*(") + (etacut+"pv_trk_pt>1 && pv_trk_pt<3").GetTitle() + ")");

    mctree->Project("h_mc_d0_phi_lopt", "pv_trk_d0_pvunbiased", TString("PU_factor*(") + (phicut+"pv_trk_pt>0.1 && pv_trk_pt<1").GetTitle() + ")");
    mctree->Project("h_mc_dz_phi_lopt", "pv_trk_dz_pvunbiased", TString("PU_factor*(") + (phicut+"pv_trk_pt>0.1 && pv_trk_pt<1").GetTitle() + ")");
    mctree->Project("h_mc_d0_phi_hipt", "pv_trk_d0_pvunbiased", TString("PU_factor*(") + (phicut+"pv_trk_pt>1 && pv_trk_pt<3").GetTitle() + ")");
    mctree->Project("h_mc_dz_phi_hipt", "pv_trk_dz_pvunbiased", TString("PU_factor*(") + (phicut+"pv_trk_pt>1 && pv_trk_pt<3").GetTitle() + ")");


    auto result_reso_d0_pt_loeta = fit_compare(h_data_d0_pt_loeta, h_mc_d0_pt_loeta, era, figdir+Form("ippv_xy_fit/pt_loeta_%d", idx), 0.1);
    auto result_reso_dz_pt_loeta = fit_compare(h_data_dz_pt_loeta, h_mc_dz_pt_loeta, era, figdir+Form("ippv_z_fit/pt_loeta_%d", idx), 0.1);
    auto result_reso_d0_pt_hieta = fit_compare(h_data_d0_pt_hieta, h_mc_d0_pt_hieta, era, figdir+Form("ippv_xy_fit/pt_hieta_%d", idx), 0.1);
    auto result_reso_dz_pt_hieta = fit_compare(h_data_dz_pt_hieta, h_mc_dz_pt_hieta, era, figdir+Form("ippv_z_fit/pt_hieta_%d", idx), 0.1);

    auto result_reso_d0_eta_lopt = fit_compare(h_data_d0_eta_lopt, h_mc_d0_eta_lopt, era, figdir+Form("ippv_xy_fit/eta_lopt_%d", idx), 0.1);
    auto result_reso_dz_eta_lopt = fit_compare(h_data_dz_eta_lopt, h_mc_dz_eta_lopt, era, figdir+Form("ippv_z_fit/eta_lopt_%d", idx), 0.1);
    auto result_reso_d0_eta_hipt = fit_compare(h_data_d0_eta_hipt, h_mc_d0_eta_hipt, era, figdir+Form("ippv_xy_fit/eta_hipt_%d", idx), 0.1);
    auto result_reso_dz_eta_hipt = fit_compare(h_data_dz_eta_hipt, h_mc_dz_eta_hipt, era, figdir+Form("ippv_z_fit/eta_hipt_%d", idx), 0.1);

    auto result_reso_d0_phi_lopt = fit_compare(h_data_d0_phi_lopt, h_mc_d0_phi_lopt, era, figdir+Form("ippv_xy_fit/phi_lopt_%d", idx), 0.1);
    auto result_reso_dz_phi_lopt = fit_compare(h_data_dz_phi_lopt, h_mc_dz_phi_lopt, era, figdir+Form("ippv_z_fit/phi_lopt_%d", idx), 0.1);
    auto result_reso_d0_phi_hipt = fit_compare(h_data_d0_phi_hipt, h_mc_d0_phi_hipt, era, figdir+Form("ippv_xy_fit/phi_hipt_%d", idx), 0.1);
    auto result_reso_dz_phi_hipt = fit_compare(h_data_dz_phi_hipt, h_mc_dz_phi_hipt, era, figdir+Form("ippv_z_fit/phi_hipt_%d", idx), 0.1);


    nlohmann::json resojson;
    resojson["pt"] = (pv_trk_pt_edges[idx] + pv_trk_pt_edges[idx+1])/2;
    resojson["eta"] = (pv_trk_eta_edges[idx] + pv_trk_eta_edges[idx+1])/2;
    resojson["phi"] = (pv_trk_phi_edges[idx] + pv_trk_phi_edges[idx+1])/2;

    resojson["reso_data_d0_pt_loeta"] = result_reso_d0_pt_loeta.first;
    resojson["reso_data_dz_pt_loeta"] = result_reso_dz_pt_loeta.first;
    resojson["reso_data_d0_pt_hieta"] = result_reso_d0_pt_hieta.first;
    resojson["reso_data_dz_pt_hieta"] = result_reso_dz_pt_hieta.first;

    resojson["reso_data_d0_eta_lopt"] = result_reso_d0_eta_lopt.first;
    resojson["reso_data_dz_eta_lopt"] = result_reso_dz_eta_lopt.first;
    resojson["reso_data_d0_eta_hipt"] = result_reso_d0_eta_hipt.first;
    resojson["reso_data_dz_eta_hipt"] = result_reso_dz_eta_hipt.first;

    resojson["reso_data_d0_phi_lopt"] = result_reso_d0_phi_lopt.first;
    resojson["reso_data_dz_phi_lopt"] = result_reso_dz_phi_lopt.first;
    resojson["reso_data_d0_phi_hipt"] = result_reso_d0_phi_hipt.first;
    resojson["reso_data_dz_phi_hipt"] = result_reso_dz_phi_hipt.first;

    resojson["reso_mc_d0_pt_loeta"] = result_reso_d0_pt_loeta.second;
    resojson["reso_mc_dz_pt_loeta"] = result_reso_dz_pt_loeta.second;
    resojson["reso_mc_d0_pt_hieta"] = result_reso_d0_pt_hieta.second;
    resojson["reso_mc_dz_pt_hieta"] = result_reso_dz_pt_hieta.second;

    resojson["reso_mc_d0_eta_lopt"] = result_reso_d0_eta_lopt.second;
    resojson["reso_mc_dz_eta_lopt"] = result_reso_dz_eta_lopt.second;
    resojson["reso_mc_d0_eta_hipt"] = result_reso_d0_eta_hipt.second;
    resojson["reso_mc_dz_eta_hipt"] = result_reso_dz_eta_hipt.second;

    resojson["reso_mc_d0_phi_lopt"] = result_reso_d0_phi_lopt.second;
    resojson["reso_mc_dz_phi_lopt"] = result_reso_dz_phi_lopt.second;
    resojson["reso_mc_d0_phi_hipt"] = result_reso_d0_phi_hipt.second;
    resojson["reso_mc_dz_phi_hipt"] = result_reso_dz_phi_hipt.second;

    std::ofstream outFile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/ZeroBias_"+era+Form("/ip_res/fit_%d.json",idx));
    outFile << resojson.dump(4);
    outFile.close();

    return 0;
}
