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

const TString figdir = "../../figures/"+datatype+"/ip_res_analysis/";
const int nbins = 20;

int ip_res_analysis(int idx) {

    setTDRStyle();

    TFile *myfile = TFile::Open(anaroot);
    TTree *mytree = (TTree*)myfile->Get("mytree");

    std::ifstream infile("../../json/"+datatype+"/binning.json");
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

    TH1F *h_d0_eta_lopt_tmp = new TH1F("h_d0_eta_lopt_tmp", "", 200, -2000, 2000);
    TH1F *h_dz_eta_lopt_tmp = new TH1F("h_dz_eta_lopt_tmp", "", 200, -8000, 8000);
    TH1F *h_d0_eta_hipt_tmp = new TH1F("h_d0_eta_hipt_tmp", "", 200, -800, 800);
    TH1F *h_dz_eta_hipt_tmp = new TH1F("h_dz_eta_hipt_tmp", "", 200, -3000, 3000);
    TH1F *h_d0_eta_ulpt_tmp = new TH1F("h_d0_eta_ulpt_tmp", "", 200, -300, 300);
    TH1F *h_dz_eta_ulpt_tmp = new TH1F("h_dz_eta_ulpt_tmp", "", 200, -1500, 1500);

    TH1F *h_d0_phi_lopt_tmp = new TH1F("h_d0_phi_lopt_tmp", "", 200, -1500, 1500);
    TH1F *h_dz_phi_lopt_tmp = new TH1F("h_dz_phi_lopt_tmp", "", 200, -3000, 3000);
    TH1F *h_d0_phi_hipt_tmp = new TH1F("h_d0_phi_hipt_tmp", "", 200, -600, 600);
    TH1F *h_dz_phi_hipt_tmp = new TH1F("h_dz_phi_hipt_tmp", "", 200, -1000, 1000);
    TH1F *h_d0_phi_ulpt_tmp = new TH1F("h_d0_phi_ulpt_tmp", "", 200, -200, 200);
    TH1F *h_dz_phi_ulpt_tmp = new TH1F("h_dz_phi_ulpt_tmp", "", 200, -500, 500);

    mytree->Project("h_d0_pt_loeta_tmp", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    mytree->Project("h_dz_pt_loeta_tmp", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    mytree->Project("h_d0_pt_hieta_tmp", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");
    mytree->Project("h_dz_pt_hieta_tmp", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");

    mytree->Project("h_d0_eta_lopt_tmp", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    mytree->Project("h_dz_eta_lopt_tmp", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    mytree->Project("h_d0_eta_hipt_tmp", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");
    mytree->Project("h_dz_eta_hipt_tmp", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");
    mytree->Project("h_d0_eta_ulpt_tmp", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>3 && pv_trk_pt<10");
    mytree->Project("h_dz_eta_ulpt_tmp", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>3 && pv_trk_pt<10");

    mytree->Project("h_d0_phi_lopt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    mytree->Project("h_dz_phi_lopt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    mytree->Project("h_d0_phi_hipt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    mytree->Project("h_dz_phi_hipt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    mytree->Project("h_d0_phi_ulpt_tmp", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");
    mytree->Project("h_dz_phi_ulpt_tmp", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");

    float d0_pt_loeta_mean = h_d0_pt_loeta_tmp->GetMean();
    float dz_pt_loeta_mean = h_dz_pt_loeta_tmp->GetMean();
    float d0_pt_hieta_mean = h_d0_pt_hieta_tmp->GetMean();
    float dz_pt_hieta_mean = h_dz_pt_hieta_tmp->GetMean();

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

    TH1F *h_d0_pt_loeta = new TH1F("h_d0_pt_loeta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<1.4};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_pt_loeta_mean-8*d0_pt_loeta_stddev, d0_pt_loeta_mean+8*d0_pt_loeta_stddev);
    TH1F *h_dz_pt_loeta = new TH1F("h_dz_pt_loeta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<1.4};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_pt_loeta_mean-8*dz_pt_loeta_stddev, dz_pt_loeta_mean+8*dz_pt_loeta_stddev);
    TH1F *h_d0_pt_hieta = new TH1F("h_d0_pt_hieta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<3.0};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_pt_hieta_mean-8*d0_pt_hieta_stddev, d0_pt_hieta_mean+8*d0_pt_hieta_stddev);
    TH1F *h_dz_pt_hieta = new TH1F("h_dz_pt_hieta", "#splitline{"+ptcut_title+"}{|#it{#eta}|<3.0};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_pt_hieta_mean-8*dz_pt_hieta_stddev, dz_pt_hieta_mean+8*dz_pt_hieta_stddev);

    TH1F *h_d0_eta_lopt = new TH1F("h_d0_eta_lopt", "#splitline{"+etacut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_eta_lopt_mean-8*d0_eta_lopt_stddev, d0_eta_lopt_mean+8*d0_eta_lopt_stddev);
    TH1F *h_dz_eta_lopt = new TH1F("h_dz_eta_lopt", "#splitline{"+etacut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_eta_lopt_mean-8*dz_eta_lopt_stddev, dz_eta_lopt_mean+8*dz_eta_lopt_stddev);
    TH1F *h_d0_eta_hipt = new TH1F("h_d0_eta_hipt", "#splitline{"+etacut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_eta_hipt_mean-8*d0_eta_hipt_stddev, d0_eta_hipt_mean+8*d0_eta_hipt_stddev);
    TH1F *h_dz_eta_hipt = new TH1F("h_dz_eta_hipt", "#splitline{"+etacut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_eta_hipt_mean-8*dz_eta_hipt_stddev, dz_eta_hipt_mean+8*dz_eta_hipt_stddev);
    TH1F *h_d0_eta_ulpt = new TH1F("h_d0_eta_ulpt", "#splitline{"+etacut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_eta_ulpt_mean-8*d0_eta_ulpt_stddev, d0_eta_ulpt_mean+8*d0_eta_ulpt_stddev);
    TH1F *h_dz_eta_ulpt = new TH1F("h_dz_eta_ulpt", "#splitline{"+etacut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_eta_ulpt_mean-8*dz_eta_ulpt_stddev, dz_eta_ulpt_mean+8*dz_eta_ulpt_stddev);

    TH1F *h_d0_phi_lopt = new TH1F("h_d0_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_phi_lopt_mean-8*d0_phi_lopt_stddev, d0_phi_lopt_mean+8*d0_phi_lopt_stddev);
    TH1F *h_dz_phi_lopt = new TH1F("h_dz_phi_lopt", "#splitline{"+phicut_title+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_phi_lopt_mean-8*dz_phi_lopt_stddev, dz_phi_lopt_mean+8*dz_phi_lopt_stddev);
    TH1F *h_d0_phi_hipt = new TH1F("h_d0_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_phi_hipt_mean-8*d0_phi_hipt_stddev, d0_phi_hipt_mean+8*d0_phi_hipt_stddev);
    TH1F *h_dz_phi_hipt = new TH1F("h_dz_phi_hipt", "#splitline{"+phicut_title+"}{1<#it{p_{T}}<3 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_phi_hipt_mean-8*dz_phi_hipt_stddev, dz_phi_hipt_mean+8*dz_phi_hipt_stddev);
    TH1F *h_d0_phi_ulpt = new TH1F("h_d0_phi_ulpt", "#splitline{"+phicut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 500, d0_phi_ulpt_mean-8*d0_phi_ulpt_stddev, d0_phi_ulpt_mean+8*d0_phi_ulpt_stddev);
    TH1F *h_dz_phi_ulpt = new TH1F("h_dz_phi_ulpt", "#splitline{"+phicut_title+"}{3<#it{p_{T}}<10 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 500, dz_phi_ulpt_mean-8*dz_phi_ulpt_stddev, dz_phi_ulpt_mean+8*dz_phi_ulpt_stddev);

    mytree->Project("h_d0_pt_loeta", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    mytree->Project("h_dz_pt_loeta", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 1.4");
    mytree->Project("h_d0_pt_hieta", "pv_trk_d0_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");
    mytree->Project("h_dz_pt_hieta", "pv_trk_dz_pvunbiased", ptcut+"abs(pv_trk_eta) < 3");

    mytree->Project("h_d0_eta_lopt", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    mytree->Project("h_dz_eta_lopt", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    mytree->Project("h_d0_eta_hipt", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");
    mytree->Project("h_dz_eta_hipt", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>1 && pv_trk_pt<3");
    mytree->Project("h_d0_eta_ulpt", "pv_trk_d0_pvunbiased", etacut+"pv_trk_pt>3 && pv_trk_pt<10");
    mytree->Project("h_dz_eta_ulpt", "pv_trk_dz_pvunbiased", etacut+"pv_trk_pt>3 && pv_trk_pt<10");

    mytree->Project("h_d0_phi_lopt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    mytree->Project("h_dz_phi_lopt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>0.1 && pv_trk_pt<1");
    mytree->Project("h_d0_phi_hipt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    mytree->Project("h_dz_phi_hipt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>1 && pv_trk_pt<3");
    mytree->Project("h_d0_phi_ulpt", "pv_trk_d0_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");
    mytree->Project("h_dz_phi_ulpt", "pv_trk_dz_pvunbiased", phicut+"pv_trk_pt>3 && pv_trk_pt<10");

    auto result_d0_pt_loeta = fit_draw(h_d0_pt_loeta, figdir+Form("ippv_xy_fit/pt_loeta_%d", idx), 0.1);
    auto result_dz_pt_loeta = fit_draw(h_dz_pt_loeta, figdir+Form("ippv_z_fit/pt_loeta_%d", idx), 0.1);
    auto result_d0_pt_hieta = fit_draw(h_d0_pt_hieta, figdir+Form("ippv_xy_fit/pt_hieta_%d", idx), 0.1);
    auto result_dz_pt_hieta = fit_draw(h_dz_pt_hieta, figdir+Form("ippv_z_fit/pt_hieta_%d", idx), 0.1);

    auto result_d0_eta_lopt = fit_draw(h_d0_eta_lopt, figdir+Form("ippv_xy_fit/eta_lopt_%d", idx), 0.1);
    auto result_dz_eta_lopt = fit_draw(h_dz_eta_lopt, figdir+Form("ippv_z_fit/eta_lopt_%d", idx), 0.1);
    auto result_d0_eta_hipt = fit_draw(h_d0_eta_hipt, figdir+Form("ippv_xy_fit/eta_hipt_%d", idx), 0.1);
    auto result_dz_eta_hipt = fit_draw(h_dz_eta_hipt, figdir+Form("ippv_z_fit/eta_hipt_%d", idx), 0.1);
    auto result_d0_eta_ulpt = fit_draw(h_d0_eta_ulpt, figdir+Form("ippv_xy_fit/eta_ulpt_%d", idx), 0.1);
    auto result_dz_eta_ulpt = fit_draw(h_dz_eta_ulpt, figdir+Form("ippv_z_fit/eta_ulpt_%d", idx), 0.1);

    auto result_d0_phi_lopt = fit_draw(h_d0_phi_lopt, figdir+Form("ippv_xy_fit/phi_lopt_%d", idx), 0.1);
    auto result_dz_phi_lopt = fit_draw(h_dz_phi_lopt, figdir+Form("ippv_z_fit/phi_lopt_%d", idx), 0.1);
    auto result_d0_phi_hipt = fit_draw(h_d0_phi_hipt, figdir+Form("ippv_xy_fit/phi_hipt_%d", idx), 0.1);
    auto result_dz_phi_hipt = fit_draw(h_dz_phi_hipt, figdir+Form("ippv_z_fit/phi_hipt_%d", idx), 0.1);
    auto result_d0_phi_ulpt = fit_draw(h_d0_phi_ulpt, figdir+Form("ippv_xy_fit/phi_ulpt_%d", idx), 0.1);
    auto result_dz_phi_ulpt = fit_draw(h_dz_phi_ulpt, figdir+Form("ippv_z_fit/phi_ulpt_%d", idx), 0.1);

    nlohmann::json resojson;
    resojson["pt"] = (pv_trk_pt_edges[idx] + pv_trk_pt_edges[idx+1])/2;
    resojson["eta"] = (pv_trk_eta_edges[idx] + pv_trk_eta_edges[idx+1])/2;
    resojson["phi"] = (pv_trk_phi_edges[idx] + pv_trk_phi_edges[idx+1])/2;

    resojson["reso_d0_pt_loeta"] = result_d0_pt_loeta.first;
    resojson["reso_dz_pt_loeta"] = result_dz_pt_loeta.first;
    resojson["reso_d0_pt_hieta"] = result_d0_pt_hieta.first;
    resojson["reso_dz_pt_hieta"] = result_dz_pt_hieta.first;

    resojson["reso_d0_eta_lopt"] = result_d0_eta_lopt.first;
    resojson["reso_dz_eta_lopt"] = result_dz_eta_lopt.first;
    resojson["reso_d0_eta_hipt"] = result_d0_eta_hipt.first;
    resojson["reso_dz_eta_hipt"] = result_dz_eta_hipt.first;
    resojson["reso_d0_eta_ulpt"] = result_d0_eta_ulpt.first;
    resojson["reso_dz_eta_ulpt"] = result_dz_eta_ulpt.first;

    resojson["reso_d0_phi_lopt"] = result_d0_phi_lopt.first;
    resojson["reso_dz_phi_lopt"] = result_dz_phi_lopt.first;
    resojson["reso_d0_phi_hipt"] = result_d0_phi_hipt.first;
    resojson["reso_dz_phi_hipt"] = result_dz_phi_hipt.first;
    resojson["reso_d0_phi_ulpt"] = result_d0_phi_ulpt.first;
    resojson["reso_dz_phi_ulpt"] = result_dz_phi_ulpt.first;

    resojson["reso2_d0_pt_loeta"] = result_d0_pt_loeta.second;
    resojson["reso2_dz_pt_loeta"] = result_dz_pt_loeta.second;
    resojson["reso2_d0_pt_hieta"] = result_d0_pt_hieta.second;
    resojson["reso2_dz_pt_hieta"] = result_dz_pt_hieta.second;

    resojson["reso2_d0_eta_lopt"] = result_d0_eta_lopt.second;
    resojson["reso2_dz_eta_lopt"] = result_dz_eta_lopt.second;
    resojson["reso2_d0_eta_hipt"] = result_d0_eta_hipt.second;
    resojson["reso2_dz_eta_hipt"] = result_dz_eta_hipt.second;
    resojson["reso2_d0_eta_ulpt"] = result_d0_eta_ulpt.second;
    resojson["reso2_dz_eta_ulpt"] = result_dz_eta_ulpt.second;

    resojson["reso2_d0_phi_lopt"] = result_d0_phi_lopt.second;
    resojson["reso2_dz_phi_lopt"] = result_dz_phi_lopt.second;
    resojson["reso2_d0_phi_hipt"] = result_d0_phi_hipt.second;
    resojson["reso2_dz_phi_hipt"] = result_dz_phi_hipt.second;
    resojson["reso2_d0_phi_ulpt"] = result_d0_phi_ulpt.second;
    resojson["reso2_dz_phi_ulpt"] = result_dz_phi_ulpt.second;

    std::ofstream outFile("../../json/"+datatype+Form("/ip_res/fit_%d.json",idx));
    outFile << resojson.dump(4);
    outFile.close();

    return 0;
}
