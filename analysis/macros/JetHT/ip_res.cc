#include <vector>
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>
#include <algorithm>
#include <nlohmann/json.hpp>

const TString datatype_text = "High-q^{2} multi-jet events";

#include "../../functions/fit_res.cc"

int ip_res(int iera, int idx) {

    TString eras[] = {"preEE", "postEE"};

    TString era = eras[iera];

    TString figdir = "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_"+era+"/ip_res/";

    TFile *datafile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_data_"+era+"_corr.root");
    TTree *datatree = (TTree*)datafile->Get("mytree");
    TFile *mcfile = TFile::Open("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_"+era+"_corr.root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    std::ifstream infile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_"+era+"/binning.json");
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

    TH1F *h_d0_pt_loeta_tmp = new TH1F("h_d0_pt_loeta_tmp", "", 200, -1500, 1500);
    TH1F *h_dz_pt_loeta_tmp = new TH1F("h_dz_pt_loeta_tmp", "", 200, -2000, 2000);
    TH1F *h_d0_pt_hieta_tmp = new TH1F("h_d0_pt_hieta_tmp", "", 200, -3000, 3000);
    TH1F *h_dz_pt_hieta_tmp = new TH1F("h_dz_pt_hieta_tmp", "", 200, -8000, 8000);
    TH1F *h_d0_pt_eta_tmp = new TH1F("h_d0_pt_eta_tmp", "", 200, -3000, 3000);
    TH1F *h_dz_pt_eta_tmp = new TH1F("h_dz_pt_eta_tmp", "", 200, -8000, 8000);

    TH1F *h_d0_eta_lopt_tmp = new TH1F("h_d0_eta_lopt_tmp", "", 200, -2000, 2000);
    TH1F *h_dz_eta_lopt_tmp = new TH1F("h_dz_eta_lopt_tmp", "", 200, -8000, 8000);
    TH1F *h_d0_eta_hipt_tmp = new TH1F("h_d0_eta_hipt_tmp", "", 200, -800, 800);
    TH1F *h_dz_eta_hipt_tmp = new TH1F("h_dz_eta_hipt_tmp", "", 200, -3000, 3000);
    TH1F *h_d0_eta_ulpt_tmp = new TH1F("h_d0_eta_ulpt_tmp", "", 200, -300, 300);
    TH1F *h_dz_eta_ulpt_tmp = new TH1F("h_dz_eta_ulpt_tmp", "", 200, -1500, 1500);

    TH1F *h_d0_phi_lopt_tmp = new TH1F("h_d0_phi_lopt_tmp", "", 200, -2000, 2000);
    TH1F *h_dz_phi_lopt_tmp = new TH1F("h_dz_phi_lopt_tmp", "", 200, -8000, 8000);
    TH1F *h_d0_phi_hipt_tmp = new TH1F("h_d0_phi_hipt_tmp", "", 200, -800, 800);
    TH1F *h_dz_phi_hipt_tmp = new TH1F("h_dz_phi_hipt_tmp", "", 200, -3000, 3000);
    TH1F *h_d0_phi_ulpt_tmp = new TH1F("h_d0_phi_ulpt_tmp", "", 200, -300, 300);
    TH1F *h_dz_phi_ulpt_tmp = new TH1F("h_dz_phi_ulpt_tmp", "", 200, -1500, 1500);

    datatree->Project("h_d0_pt_loeta_tmp", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    datatree->Project("h_dz_pt_loeta_tmp", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    datatree->Project("h_d0_pt_g_tmp", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) > 1.4"+"abs(pv_trk_eta) < 3");
    datatree->Project("h_dz_pt_g_tmp", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) > 1.4"+"abs(pv_trk_eta) < 3");
    datatree->Project("h_d0_pt_eta_tmp", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");
    datatree->Project("h_dz_pt_eta_tmp", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");

    datatree->Project("h_d0_eta_lopt_tmp", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_dz_eta_lopt_tmp", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_d0_eta_hipt_tmp", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_dz_eta_hipt_tmp", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_d0_eta_ulpt_tmp", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>3 && pv_trk_pt<10");
    datatree->Project("h_dz_eta_ulpt_tmp", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>3 && pv_trk_pt<10");

    datatree->Project("h_d0_phi_lopt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_dz_phi_lopt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_d0_phi_hipt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_dz_phi_hipt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_d0_phi_ulpt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");
    datatree->Project("h_dz_phi_ulpt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");

    float d0_pt_loeta_mean = h_d0_pt_loeta_tmp->GetMean();
    float dz_pt_loeta_mean = h_dz_pt_loeta_tmp->GetMean();
    float d0_pt_hieta_mean = h_d0_pt_hieta_tmp->GetMean();
    float dz_pt_hieta_mean = h_dz_pt_hieta_tmp->GetMean();
    float d0_pt_eta_mean = h_d0_pt_eta_tmp->GetMean();
    float dz_pt_eta_mean = h_dz_pt_eta_tmp->GetMean();

    float d0_eta_lopt_mean = h_d0_eta_lopt_tmp->GetMean();
    float dz_eta_lopt_mean = h_dz_eta_lopt_tmp->GetMean();
    float d0_eta_hipt_mean = h_d0_eta_hipt_tmp->GetMean();
    float dz_eta_hipt_mean = h_dz_eta_hipt_tmp->GetMean();
    float d0_eta_ulpt_mean = h_d0_eta_ulpt_tmp->GetMean();
    float dz_eta_ulpt_mean = h_dz_eta_ulpt_tmp->GetMean();

    float d0_phi_lopt_mean = h_d0_phi_lopt_tmp->GetMean();
    float dz_phi_lopt_mean = h_dz_phi_lopt_tmp->GetMean();
    float d0_phi_hipt_mean = h_d0_phi_hipt_tmp->GetMean();
    float dz_phi_hipt_mean = h_dz_phi_hipt_tmp->GetMean();
    float d0_phi_ulpt_mean = h_d0_phi_ulpt_tmp->GetMean();
    float dz_phi_ulpt_mean = h_dz_phi_ulpt_tmp->GetMean();

    float d0_pt_loeta_stddev = h_d0_pt_loeta_tmp->GetStdDev();
    float dz_pt_loeta_stddev = h_dz_pt_loeta_tmp->GetStdDev();
    float d0_pt_hieta_stddev = h_d0_pt_hieta_tmp->GetStdDev();
    float dz_pt_hieta_stddev = h_dz_pt_hieta_tmp->GetStdDev();
    float d0_pt_eta_stddev = h_d0_pt_eta_tmp->GetStdDev();
    float dz_pt_eta_stddev = h_dz_pt_eta_tmp->GetStdDev();

    float d0_eta_lopt_stddev = h_d0_eta_lopt_tmp->GetStdDev();
    float dz_eta_lopt_stddev = h_dz_eta_lopt_tmp->GetStdDev();
    float d0_eta_hipt_stddev = h_d0_eta_hipt_tmp->GetStdDev();
    float dz_eta_hipt_stddev = h_dz_eta_hipt_tmp->GetStdDev();
    float d0_eta_ulpt_stddev = h_d0_eta_ulpt_tmp->GetStdDev();
    float dz_eta_ulpt_stddev = h_dz_eta_ulpt_tmp->GetStdDev();

    float d0_phi_lopt_stddev = h_d0_phi_lopt_tmp->GetStdDev();
    float dz_phi_lopt_stddev = h_dz_phi_lopt_tmp->GetStdDev();
    float d0_phi_hipt_stddev = h_d0_phi_hipt_tmp->GetStdDev();
    float dz_phi_hipt_stddev = h_dz_phi_hipt_tmp->GetStdDev();
    float d0_phi_ulpt_stddev = h_d0_phi_ulpt_tmp->GetStdDev();
    float dz_phi_ulpt_stddev = h_dz_phi_ulpt_tmp->GetStdDev();

    delete h_d0_pt_loeta_tmp;
    delete h_dz_pt_loeta_tmp;
    delete h_d0_pt_hieta_tmp;
    delete h_dz_pt_hieta_tmp;
    delete h_d0_pt_eta_tmp;
    delete h_dz_pt_eta_tmp;

    delete h_d0_eta_lopt_tmp;
    delete h_dz_eta_lopt_tmp;
    delete h_d0_eta_hipt_tmp;
    delete h_dz_eta_hipt_tmp;
    delete h_d0_eta_ulpt_tmp;
    delete h_dz_eta_ulpt_tmp;

    delete h_d0_phi_lopt_tmp;
    delete h_dz_phi_lopt_tmp;
    delete h_d0_phi_hipt_tmp;
    delete h_dz_phi_hipt_tmp;
    delete h_d0_phi_ulpt_tmp;
    delete h_dz_phi_ulpt_tmp;

    TH1F *h_data_d0_pt_loeta = new TH1F("h_data_d0_pt_loeta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<1.4};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_pt_loeta_mean-8*d0_pt_loeta_stddev, d0_pt_loeta_mean+8*d0_pt_loeta_stddev);
    TH1F *h_data_dz_pt_loeta = new TH1F("h_data_dz_pt_loeta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<1.4};Track IP #it{d_{z}} [mm];# tracks", 500, dz_pt_loeta_mean-8*dz_pt_loeta_stddev, dz_pt_loeta_mean+8*dz_pt_loeta_stddev);
    TH1F *h_data_d0_pt_hieta = new TH1F("h_data_d0_pt_hieta", "#splitline{"+ptcut_title+"}{1.4<|#it{#hieta}|<3.0};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_pt_hieta_mean-8*d0_pt_hieta_stddev, d0_pt_hieta_mean+8*d0_pt_hieta_stddev);
    TH1F *h_data_dz_pt_hieta = new TH1F("h_data_dz_pt_hieta", "#splitline{"+ptcut_title+"}{1.4<|#it{#hieta}|<3.0};Track IP #it{d_{z}} [mm];# tracks", 500, dz_pt_hieta_mean-8*dz_pt_hieta_stddev, dz_pt_hieta_mean+8*dz_pt_hieta_stddev);
    TH1F *h_data_d0_pt_eta = new TH1F("h_data_d0_pt_eta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<3.0};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_pt_eta_mean-8*d0_pt_eta_stddev, d0_pt_eta_mean+8*d0_pt_eta_stddev);
    TH1F *h_data_dz_pt_eta = new TH1F("h_data_dz_pt_eta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<3.0};Track IP #it{d_{z}} [mm];# tracks", 500, dz_pt_eta_mean-8*dz_pt_eta_stddev, dz_pt_eta_mean+8*dz_pt_eta_stddev);

    TH1F *h_data_d0_eta_lopt = new TH1F("h_data_d0_eta_lopt", "#splitline{"+etacut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_eta_lopt_mean-8*d0_eta_lopt_stddev, d0_eta_lopt_mean+8*d0_eta_lopt_stddev);
    TH1F *h_data_dz_eta_lopt = new TH1F("h_data_dz_eta_lopt", "#splitline{"+etacut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} [mm];# tracks", 500, dz_eta_lopt_mean-8*dz_eta_lopt_stddev, dz_eta_lopt_mean+8*dz_eta_lopt_stddev);
    TH1F *h_data_d0_eta_hipt = new TH1F("h_data_d0_eta_hipt", "#splitline{"+etacut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_eta_hipt_mean-8*d0_eta_hipt_stddev, d0_eta_hipt_mean+8*d0_eta_hipt_stddev);
    TH1F *h_data_dz_eta_hipt = new TH1F("h_data_dz_eta_hipt", "#splitline{"+etacut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{z}} [mm];# tracks", 500, dz_eta_hipt_mean-8*dz_eta_hipt_stddev, dz_eta_hipt_mean+8*dz_eta_hipt_stddev);
    TH1F *h_data_d0_eta_ulpt = new TH1F("h_data_d0_eta_ulpt", "#splitline{"+etacut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_eta_ulpt_mean-8*d0_eta_ulpt_stddev, d0_eta_ulpt_mean+8*d0_eta_ulpt_stddev);
    TH1F *h_data_dz_eta_ulpt = new TH1F("h_data_dz_eta_ulpt", "#splitline{"+etacut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{z}} [mm];# tracks", 500, dz_eta_ulpt_mean-8*dz_eta_ulpt_stddev, dz_eta_ulpt_mean+8*dz_eta_ulpt_stddev);

    TH1F *h_data_d0_phi_lopt = new TH1F("h_data_d0_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_phi_lopt_mean-8*d0_phi_lopt_stddev, d0_phi_lopt_mean+8*d0_phi_lopt_stddev);
    TH1F *h_data_dz_phi_lopt = new TH1F("h_data_dz_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} [mm];# tracks", 500, dz_phi_lopt_mean-8*dz_phi_lopt_stddev, dz_phi_lopt_mean+8*dz_phi_lopt_stddev);
    TH1F *h_data_d0_phi_hipt = new TH1F("h_data_d0_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_phi_hipt_mean-8*d0_phi_hipt_stddev, d0_phi_hipt_mean+8*d0_phi_hipt_stddev);
    TH1F *h_data_dz_phi_hipt = new TH1F("h_data_dz_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{z}} [mm];# tracks", 500, dz_phi_hipt_mean-8*dz_phi_hipt_stddev, dz_phi_hipt_mean+8*dz_phi_hipt_stddev);
    TH1F *h_data_d0_phi_ulpt = new TH1F("h_data_d0_phi_ulpt", "#splitline{"+phicut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 500, d0_phi_ulpt_mean-8*d0_phi_ulpt_stddev, d0_phi_ulpt_mean+8*d0_phi_ulpt_stddev);
    TH1F *h_data_dz_phi_ulpt = new TH1F("h_data_dz_phi_ulpt", "#splitline{"+phicut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{z}} [mm];# tracks", 500, dz_phi_ulpt_mean-8*dz_phi_ulpt_stddev, dz_phi_ulpt_mean+8*dz_phi_ulpt_stddev);

    datatree->Project("h_data_d0_pt_loeta", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    datatree->Project("h_data_dz_pt_loeta", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    datatree->Project("h_data_d0_pt_hieta", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_hieta) > 1.4"+"abs(pv_trk_hieta) < 3");
    datatree->Project("h_data_dz_pt_hieta", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_hieta) > 1.4"+"abs(pv_trk_hieta) < 3");
    datatree->Project("h_data_d0_pt_eta", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");
    datatree->Project("h_data_dz_pt_eta", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");

    datatree->Project("h_data_d0_eta_lopt", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_data_dz_eta_lopt", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_data_d0_eta_hipt", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_data_dz_eta_hipt", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_data_d0_eta_ulpt", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>3 && pv_trk_pt<10");
    datatree->Project("h_data_dz_eta_ulpt", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>3 && pv_trk_pt<10");

    datatree->Project("h_data_d0_phi_lopt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_data_dz_phi_lopt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    datatree->Project("h_data_d0_phi_hipt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_data_dz_phi_hipt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    datatree->Project("h_data_d0_phi_ulpt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");
    datatree->Project("h_data_dz_phi_ulpt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");

    TH1F *h_mc_d0_pt_loeta = new TH1F("h_mc_d0_pt_loeta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<1.4};Track IP #it{d_{xy}} [#mum];# tracks", 100, d0_pt_loeta_mean-8*d0_pt_loeta_stddev, d0_pt_loeta_mean+8*d0_pt_loeta_stddev);
    TH1F *h_mc_dz_pt_loeta = new TH1F("h_mc_dz_pt_loeta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<1.4};Track IP #it{d_{z}} [mm];# tracks", 100, dz_pt_loeta_mean-8*dz_pt_loeta_stddev, dz_pt_loeta_mean+8*dz_pt_loeta_stddev);
    TH1F *h_mc_d0_pt_hieta = new TH1F("h_mc_d0_pt_hieta", "#splitline{"+ptcut_title+"}{1.4<|#it{#hieta}|<3.0};Track IP #it{d_{xy}} [#mum];# tracks", 100, d0_pt_hieta_mean-8*d0_pt_hieta_stddev, d0_pt_hieta_mean+8*d0_pt_hieta_stddev);
    TH1F *h_mc_dz_pt_hieta = new TH1F("h_mc_dz_pt_hieta", "#splitline{"+ptcut_title+"}{1.4<|#it{#hieta}|<3.0};Track IP #it{d_{z}} [mm];# tracks", 100, dz_pt_hieta_mean-8*dz_pt_hieta_stddev, dz_pt_hieta_mean+8*dz_pt_hieta_stddev);
    TH1F *h_mc_d0_pt_eta = new TH1F("h_mc_d0_pt_eta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<3.0};Track IP #it{d_{xy}} [#mum];# tracks", 100, d0_pt_eta_mean-8*d0_pt_eta_stddev, d0_pt_eta_mean+8*d0_pt_eta_stddev);
    TH1F *h_mc_dz_pt_eta = new TH1F("h_mc_dz_pt_eta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<3.0};Track IP #it{d_{z}} [mm];# tracks", 100, dz_pt_eta_mean-8*dz_pt_eta_stddev, dz_pt_eta_mean+8*dz_pt_eta_stddev);

    TH1F *h_mc_d0_eta_lopt = new TH1F("h_mc_d0_eta_lopt", "#splitline{"+etacut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 100, d0_eta_lopt_mean-8*d0_eta_lopt_stddev, d0_eta_lopt_mean+8*d0_eta_lopt_stddev);
    TH1F *h_mc_dz_eta_lopt = new TH1F("h_mc_dz_eta_lopt", "#splitline{"+etacut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} [mm];# tracks", 100, dz_eta_lopt_mean-8*dz_eta_lopt_stddev, dz_eta_lopt_mean+8*dz_eta_lopt_stddev);
    TH1F *h_mc_d0_eta_hipt = new TH1F("h_mc_d0_eta_hipt", "#splitline{"+etacut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 100, d0_eta_hipt_mean-8*d0_eta_hipt_stddev, d0_eta_hipt_mean+8*d0_eta_hipt_stddev);
    TH1F *h_mc_dz_eta_hipt = new TH1F("h_mc_dz_eta_hipt", "#splitline{"+etacut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{z}} [mm];# tracks", 100, dz_eta_hipt_mean-8*dz_eta_hipt_stddev, dz_eta_hipt_mean+8*dz_eta_hipt_stddev);
    TH1F *h_mc_d0_eta_ulpt = new TH1F("h_mc_d0_eta_ulpt", "#splitline{"+etacut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 100, d0_eta_ulpt_mean-8*d0_eta_ulpt_stddev, d0_eta_ulpt_mean+8*d0_eta_ulpt_stddev);
    TH1F *h_mc_dz_eta_ulpt = new TH1F("h_mc_dz_eta_ulpt", "#splitline{"+etacut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{z}} [mm];# tracks", 100, dz_eta_ulpt_mean-8*dz_eta_ulpt_stddev, dz_eta_ulpt_mean+8*dz_eta_ulpt_stddev);

    TH1F *h_mc_d0_phi_lopt = new TH1F("h_mc_d0_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 100, d0_phi_lopt_mean-8*d0_phi_lopt_stddev, d0_phi_lopt_mean+8*d0_phi_lopt_stddev);
    TH1F *h_mc_dz_phi_lopt = new TH1F("h_mc_dz_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} [mm];# tracks", 100, dz_phi_lopt_mean-8*dz_phi_lopt_stddev, dz_phi_lopt_mean+8*dz_phi_lopt_stddev);
    TH1F *h_mc_d0_phi_hipt = new TH1F("h_mc_d0_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 100, d0_phi_hipt_mean-8*d0_phi_hipt_stddev, d0_phi_hipt_mean+8*d0_phi_hipt_stddev);
    TH1F *h_mc_dz_phi_hipt = new TH1F("h_mc_dz_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{z}} [mm];# tracks", 100, dz_phi_hipt_mean-8*dz_phi_hipt_stddev, dz_phi_hipt_mean+8*dz_phi_hipt_stddev);
    TH1F *h_mc_d0_phi_ulpt = new TH1F("h_mc_d0_phi_ulpt", "#splitline{"+phicut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{xy}} [#mum];# tracks", 100, d0_phi_ulpt_mean-8*d0_phi_ulpt_stddev, d0_phi_ulpt_mean+8*d0_phi_ulpt_stddev);
    TH1F *h_mc_dz_phi_ulpt = new TH1F("h_mc_dz_phi_ulpt", "#splitline{"+phicut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{z}} [mm];# tracks", 100, dz_phi_ulpt_mean-8*dz_phi_ulpt_stddev, dz_phi_ulpt_mean+8*dz_phi_ulpt_stddev);

    mctree->Project("h_mc_d0_pt_loeta", "pv_trk_d0_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (ptcut+"abs(pv_trk_eta) < 1.4").GetTitle() + ")");
    mctree->Project("h_mc_dz_pt_loeta", "pv_trk_dz_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (ptcut+"abs(pv_trk_eta) < 1.4").GetTitle() + ")");
    mctree->Project("h_mc_d0_pt_hieta", "pv_trk_d0_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (ptcut+"abs(pv_trk_hieta) > 1.4"+"abs(pv_trk_hieta) < 3").GetTitle() + ")");
    mctree->Project("h_mc_dz_pt_hieta", "pv_trk_dz_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (ptcut+"abs(pv_trk_hieta) > 1.4"+"abs(pv_trk_hieta) < 3").GetTitle() + ")");
    mctree->Project("h_mc_d0_pt_eta", "pv_trk_d0_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (ptcut+"abs(pv_trk_eta) < 3").GetTitle() + ")");
    mctree->Project("h_mc_dz_pt_eta", "pv_trk_dz_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (ptcut+"abs(pv_trk_eta) < 3").GetTitle() + ")");

    mctree->Project("h_mc_d0_eta_lopt", "pv_trk_d0_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (etacut+"pv_trk_pt > 0.1 && pv_trk_pt <1").GetTitle() + ")");
    mctree->Project("h_mc_dz_eta_lopt", "pv_trk_dz_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (etacut+"pv_trk_pt > 0.1 && pv_trk_pt < 1").GetTitle() + ")");
    mctree->Project("h_mc_d0_eta_hipt", "pv_trk_d0_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (etacut+"pv_trk_pt > 1 && pv_trk_pt < 3").GetTitle() + ")");
    mctree->Project("h_mc_dz_eta_hipt", "pv_trk_dz_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (etacut+"pv_trk_pt > 1 && pv_trk_pt < 3").GetTitle() + ")");
    mctree->Project("h_mc_d0_eta_ulpt", "pv_trk_d0_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (etacut+"pv_trk_pt > 3 && pv_trk_pt < 10").GetTitle() + ")");
    mctree->Project("h_mc_dz_eta_ulpt", "pv_trk_dz_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (etacut+"pv_trk_pt > 3 && pv_trk_pt < 10").GetTitle() + ")");

    mctree->Project("h_mc_d0_phi_lopt", "pv_trk_d0_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (phicut+"pv_trk_pt > 0.1 && pv_trk_pt < 1").GetTitle() + ")");
    mctree->Project("h_mc_dz_phi_lopt", "pv_trk_dz_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (phicut+"pv_trk_pt > 0.1 && pv_trk_pt < 1").GetTitle() + ")");
    mctree->Project("h_mc_d0_phi_hipt", "pv_trk_d0_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (phicut+"pv_trk_pt > 1 && pv_trk_pt < 3").GetTitle() + ")");
    mctree->Project("h_mc_dz_phi_hipt", "pv_trk_dz_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (phicut+"pv_trk_pt > 1 && pv_trk_pt < 3").GetTitle() + ")");
    mctree->Project("h_mc_d0_phi_ulpt", "pv_trk_d0_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (phicut+"pv_trk_pt > 3 && pv_trk_pt < 10").GetTitle() + ")");
    mctree->Project("h_mc_dz_phi_ulpt", "pv_trk_dz_pvunbiased", TString("xsecweight * PSweight * PU_factor * (") + (phicut+"pv_trk_pt > 3 && pv_trk_pt < 10").GetTitle() + ")");

    auto result_reso_d0_pt_loeta = fit_compare(h_data_d0_pt_loeta, h_mc_d0_pt_loeta, era, figdir+Form("ippv_xy_fit/data_pt_loeta_%d", idx), figdir+Form("ippv_xy_fit/mc_pt_loeta_%d", idx), 0.1);
    auto result_reso_dz_pt_loeta = fit_compare(h_data_dz_pt_loeta, h_mc_dz_pt_loeta, era, figdir+Form("ippv_z_fit/data_pt_loeta_%d", idx), figdir+Form("ippv_z_fit/mc_pt_loeta_%d", idx), 0.1);
    auto result_reso_d0_pt_hieta = fit_compare(h_data_d0_pt_hieta, h_mc_d0_pt_hieta, era, figdir+Form("ippv_xy_fit/data_pt_hieta_%d", idx), figdir+Form("ippv_xy_fit/mc_pt_hieta_%d", idx), 0.1);
    auto result_reso_dz_pt_hieta = fit_compare(h_data_dz_pt_hieta, h_mc_dz_pt_hieta, era, figdir+Form("ippv_z_fit/data_pt_hieta_%d", idx), figdir+Form("ippv_z_fit/mc_pt_hieta_%d", idx), 0.1);
    auto result_reso_d0_pt_eta = fit_compare(h_data_d0_pt_eta, h_mc_d0_pt_eta, era, figdir+Form("ippv_xy_fit/data_pt_eta_%d", idx), figdir+Form("ippv_xy_fit/mc_pt_eta_%d", idx), 0.1);
    auto result_reso_dz_pt_eta = fit_compare(h_data_dz_pt_eta, h_mc_dz_pt_eta, era, figdir+Form("ippv_z_fit/data_pt_eta_%d", idx), figdir+Form("ippv_z_fit/mc_pt_eta_%d", idx), 0.1);

    auto result_reso_d0_eta_lopt = fit_compare(h_data_d0_eta_lopt, h_mc_d0_eta_lopt, era, figdir+Form("ippv_xy_fit/data_eta_lopt_%d", idx), figdir+Form("ippv_xy_fit/mc_eta_lopt_%d", idx), 0.1);
    auto result_reso_dz_eta_lopt = fit_compare(h_data_dz_eta_lopt, h_mc_dz_eta_lopt, era, figdir+Form("ippv_z_fit/data_eta_lopt_%d", idx), figdir+Form("ippv_z_fit/mc_eta_lopt_%d", idx), 0.1);
    auto result_reso_d0_eta_hipt = fit_compare(h_data_d0_eta_hipt, h_mc_d0_eta_hipt, era, figdir+Form("ippv_xy_fit/data_eta_hipt_%d", idx), figdir+Form("ippv_xy_fit/mc_eta_hipt_%d", idx), 0.1);
    auto result_reso_dz_eta_hipt = fit_compare(h_data_dz_eta_hipt, h_mc_dz_eta_hipt, era, figdir+Form("ippv_z_fit/data_eta_hipt_%d", idx), figdir+Form("ippv_z_fit/mc_eta_hipt_%d", idx), 0.1);
    auto result_reso_d0_eta_ulpt = fit_compare(h_data_d0_eta_ulpt, h_mc_d0_eta_ulpt, era, figdir+Form("ippv_xy_fit/data_eta_ulpt_%d", idx), figdir+Form("ippv_xy_fit/mc_eta_ulpt_%d", idx), 0.1);
    auto result_reso_dz_eta_ulpt = fit_compare(h_data_dz_eta_ulpt, h_mc_dz_eta_ulpt, era, figdir+Form("ippv_z_fit/data_eta_ulpt_%d", idx), figdir+Form("ippv_z_fit/mc_eta_ulpt_%d", idx), 0.1);

    auto result_reso_d0_phi_lopt = fit_compare(h_data_d0_phi_lopt, h_mc_d0_phi_lopt, era, figdir+Form("ippv_xy_fit/data_phi_lopt_%d", idx), figdir+Form("ippv_xy_fit/mc_phi_lopt_%d", idx), 0.1);
    auto result_reso_dz_phi_lopt = fit_compare(h_data_dz_phi_lopt, h_mc_dz_phi_lopt, era, figdir+Form("ippv_z_fit/data_phi_lopt_%d", idx), figdir+Form("ippv_z_fit/mc_phi_lopt_%d", idx), 0.1);
    auto result_reso_d0_phi_hipt = fit_compare(h_data_d0_phi_hipt, h_mc_d0_phi_hipt, era, figdir+Form("ippv_xy_fit/data_phi_hipt_%d", idx), figdir+Form("ippv_xy_fit/mc_phi_hipt_%d", idx), 0.1);
    auto result_reso_dz_phi_hipt = fit_compare(h_data_dz_phi_hipt, h_mc_dz_phi_hipt, era, figdir+Form("ippv_z_fit/data_phi_hipt_%d", idx), figdir+Form("ippv_z_fit/mc_phi_hipt_%d", idx), 0.1);
    auto result_reso_d0_phi_ulpt = fit_compare(h_data_d0_phi_ulpt, h_mc_d0_phi_ulpt, era, figdir+Form("ippv_xy_fit/data_phi_ulpt_%d", idx), figdir+Form("ippv_xy_fit/mc_phi_ulpt_%d", idx), 0.1);
    auto result_reso_dz_phi_ulpt = fit_compare(h_data_dz_phi_ulpt, h_mc_dz_phi_ulpt, era, figdir+Form("ippv_z_fit/data_phi_ulpt_%d", idx), figdir+Form("ippv_z_fit/mc_phi_ulpt_%d", idx), 0.1);

    nlohmann::json resojson;
    resojson["pt"] = (pv_trk_pt_edges[idx] + pv_trk_pt_edges[idx+1])/2;
    resojson["eta"] = (pv_trk_eta_edges[idx] + pv_trk_eta_edges[idx+1])/2;
    resojson["phi"] = (pv_trk_phi_edges[idx] + pv_trk_phi_edges[idx+1])/2;

    resojson["reso_data_d0_pt_loeta"] = result_reso_d0_pt_loeta.first;
    resojson["reso_data_dz_pt_loeta"] = result_reso_dz_pt_loeta.first;
    resojson["reso_data_d0_pt_hieta"] = result_reso_d0_pt_hieta.first;
    resojson["reso_data_dz_pt_hieta"] = result_reso_dz_pt_hieta.first;
    resojson["reso_data_d0_pt_eta"] = result_reso_d0_pt_eta.first;
    resojson["reso_data_dz_pt_eta"] = result_reso_dz_pt_eta.first;

    resojson["reso_data_d0_eta_lopt"] = result_reso_d0_eta_lopt.first;
    resojson["reso_data_dz_eta_lopt"] = result_reso_dz_eta_lopt.first;
    resojson["reso_data_d0_eta_hipt"] = result_reso_d0_eta_hipt.first;
    resojson["reso_data_dz_eta_hipt"] = result_reso_dz_eta_hipt.first;
    resojson["reso_data_d0_eta_ulpt"] = result_reso_d0_eta_ulpt.first;
    resojson["reso_data_dz_eta_ulpt"] = result_reso_dz_eta_ulpt.first;

    resojson["reso_data_d0_phi_lopt"] = result_reso_d0_phi_lopt.first;
    resojson["reso_data_dz_phi_lopt"] = result_reso_dz_phi_lopt.first;
    resojson["reso_data_d0_phi_hipt"] = result_reso_d0_phi_hipt.first;
    resojson["reso_data_dz_phi_hipt"] = result_reso_dz_phi_hipt.first;
    resojson["reso_data_d0_phi_ulpt"] = result_reso_d0_phi_ulpt.first;
    resojson["reso_data_dz_phi_ulpt"] = result_reso_dz_phi_ulpt.first;

    resojson["reso_mc_d0_pt_loeta"] = result_reso_d0_pt_loeta.second;
    resojson["reso_mc_dz_pt_loeta"] = result_reso_dz_pt_loeta.second;
    resojson["reso_mc_d0_pt_hieta"] = result_reso_d0_pt_hieta.second;
    resojson["reso_mc_dz_pt_hieta"] = result_reso_dz_pt_hieta.second;
    resojson["reso_mc_d0_pt_eta"] = result_reso_d0_pt_eta.second;
    resojson["reso_mc_dz_pt_eta"] = result_reso_dz_pt_eta.second;

    resojson["reso_mc_d0_eta_lopt"] = result_reso_d0_eta_lopt.second;
    resojson["reso_mc_dz_eta_lopt"] = result_reso_dz_eta_lopt.second;
    resojson["reso_mc_d0_eta_hipt"] = result_reso_d0_eta_hipt.second;
    resojson["reso_mc_dz_eta_hipt"] = result_reso_dz_eta_hipt.second;
    resojson["reso_mc_d0_eta_ulpt"] = result_reso_d0_eta_ulpt.second;
    resojson["reso_mc_dz_eta_ulpt"] = result_reso_dz_eta_ulpt.second;

    resojson["reso_mc_d0_phi_lopt"] = result_reso_d0_phi_lopt.second;
    resojson["reso_mc_dz_phi_lopt"] = result_reso_dz_phi_lopt.second;
    resojson["reso_mc_d0_phi_hipt"] = result_reso_d0_phi_hipt.second;
    resojson["reso_mc_dz_phi_hipt"] = result_reso_dz_phi_hipt.second;
    resojson["reso_mc_d0_phi_ulpt"] = result_reso_d0_phi_ulpt.second;
    resojson["reso_mc_dz_phi_ulpt"] = result_reso_dz_phi_ulpt.second;

    std::ofstream outFile("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_"+era+Form("/ip_res/fit_%d.json",idx));
    outFile << resojson.dump(4);
    outFile.close();

    return 0;
}
