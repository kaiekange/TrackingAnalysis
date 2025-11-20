#include <vector>
#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"

const TString datatype_text = "Unbiased collision events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/ZeroBias";

Float_t fit_res(TH1F *hist, TString period, TString sampletype, TString figpath, Float_t tolerance = 1e-4)
{
    setTDRStyle();

    period.ReplaceAll("_", " ");
    lumi_sqrtS = "13.6 TeV, " + period;

    RooRealVar ip_var("ip_var", "ip_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    ip_var.setBins(hist->GetNbinsX());
    RooRealVar mu("mu", "mu", hist->GetMean(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar sigma1("sigma1", "sigma1", 0.5 * hist->GetRMS(), 0., hist->GetRMS());
    RooRealVar sigma2("sigma2", "sigma2", hist->GetRMS(), hist->GetRMS() * 0.5, hist->GetRMS() * 2);
    RooRealVar sigma3("sigma3", "sigma3", 1.5 * hist->GetRMS(), hist->GetRMS(), hist->GetRMS() * 3);
    RooGaussian gauss1("gauss1", "gauss1", ip_var, mu, sigma1);
    RooGaussian gauss2("gauss2", "gauss2", ip_var, mu, sigma2);
    RooGaussian gauss3("gauss3", "gauss3", ip_var, mu, sigma3);
    RooRealVar f1("f1", "f1", 0.3, 0.0, 1.0);
    RooRealVar f2("f2", "f2", 0.3, 0.0, 1.0);
    RooFormulaVar f3("f3", "1 - f1 - f2", RooArgList(f1, f2));
    RooAddPdf triGauss("triGauss", "triGauss", RooArgList(gauss1, gauss2, gauss3), RooArgList(f1, f2, f3));

    RooDataHist hdatahist("hdatahist", "", ip_var, hist);
    RooFitResult *fitResult = triGauss.fitTo(hdatahist, RooFit::Save(true));
    fitResult->Print();
    delete fitResult;

    Float_t ip_var_max = ip_var.getMax();
    Float_t mean = mu.getVal();
    Float_t low = 0.0;
    Float_t high = ip_var_max - mean;
    while (high - low > tolerance)
    {
        Float_t mid = 0.5 * (low + high);
        ip_var.setRange("intRange", mean - mid, mean + mid);
        RooAbsReal *integral = triGauss.createIntegral(ip_var, RooFit::NormSet(ip_var), RooFit::Range("intRange"));
        Float_t prob = integral->getVal();
        if (prob < 0.68)
            low = mid;
        else
            high = mid;
        delete integral;
    }
    Float_t reso = 0.5 * (low + high);

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas_setup(canvas);
    canvas->SetBottomMargin(0.15);
    canvas->SetRightMargin(0.05);
    canvas->SetLogy(0);
    canvas->SetFillColor(0);
    canvas->SetFrameFillColor(0);
    RooPlot *frame = ip_var.frame();

    hdatahist.plotOn(frame, RooFit::Name(sampletype), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1.1), RooFit::Binning(hist->GetNbinsX()), RooFit::DrawOption("ep"));
    triGauss.plotOn(frame, RooFit::Name("triGauss"), RooFit::Components("triGauss"), RooFit::LineStyle(9), RooFit::LineColor(kRed), RooFit::LineWidth(2.0), RooFit::DrawOption("L"));
    frame->Draw("");
    frame->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    frame->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    frame->GetYaxis()->SetNdivisions(810);
    frame->GetXaxis()->SetNdivisions(810);
    frame->SetMinimum(0);
    frame->SetMaximum(frame->GetMaximum() * 1.3);
    write_text(0.6, 0.88, datatype_text);
    write_text(0.6, 0.8, hist->GetTitle());
    write_text(0.6, 0.7, sampletype + " fit results:");
    write_text(0.6, 0.65, Form("reso = %.*f", std::max(0, 2 - (Int_t)floor(log10(reso))), reso));
    CMS_lumi(canvas);

    canvas->Update();
    canvas->SaveAs(figpath + ".png");

    delete canvas;

    return reso;
}

std::pair<Float_t, Float_t> fill_to_fit(TTree *datatree, TTree *mctree, TString var_, TString title_, TString cut_, TString period, TString datafigpath, TString mcfigpath, Float_t lowbound, Float_t highbound, Int_t idx)
{
    TString htmp_name = Form("hist_tmp_%d", idx);
    TString hdata_name = Form("datahist_%d", idx);
    TString hmc_name = Form("mchist_%d", idx);

    TH1F *hist_tmp = new TH1F(htmp_name, "", 200, lowbound, highbound);
    datatree->Project(htmp_name, var_, cut_);
    Float_t hist_mean = hist_tmp->GetMean();
    Float_t hist_stddev = hist_tmp->GetStdDev();
    delete hist_tmp;

    std::cout << hist_mean << " " << hist_stddev << std::endl;

    TH1F *datahist = new TH1F(hdata_name, title_, 200, hist_mean - 8 * hist_stddev, hist_mean + 8 * hist_stddev);
    datatree->Project(hdata_name, var_, cut_);

    TH1F *mchist = new TH1F(hmc_name, title_, 200, hist_mean - 8 * hist_stddev, hist_mean + 8 * hist_stddev);
    mctree->Project(hmc_name, var_, cut_);

    Float_t datareso = fit_res(datahist, period, "Data", datafigpath, 0.1);
    Float_t mcreso = fit_res(mchist, period, "Simulation", mcfigpath, 0.1);

    delete datahist;
    delete mchist;

    return {datareso, mcreso};
}

Int_t ip_res(TString period, Int_t idx)
{

    TString figdir = storage_dir + "/figures/" + period + "/ip_res/";

    TFile *datafile = TFile::Open(storage_dir + "/tuples/" + period + "/data_corr.root");
    TTree *datatree = (TTree *)datafile->Get("mytree");

    TFile *mcfile = TFile::Open(storage_dir + "/tuples/" + period + "/mc_corr.root");
    TTree *mctree = (TTree *)mcfile->Get("mytree");

    std::ifstream infile(storage_dir + "/json/" + period + "/binning.json");
    nlohmann::json binning;
    infile >> binning;
    infile.close();

    std::vector<Float_t> pv_trk_pt_edges = binning["pv_trk_pt"].get<std::vector<Float_t>>();
    std::vector<Float_t> pv_trk_eta_edges = binning["pv_trk_eta"].get<std::vector<Float_t>>();
    std::vector<Float_t> pv_trk_phi_edges = binning["pv_trk_phi"].get<std::vector<Float_t>>();

    TString ptcut_title = Form("%.3f<#it{p_{T}}<%.3f GeV", pv_trk_pt_edges[idx], pv_trk_pt_edges[idx + 1]);
    TCut ptcut = Form("pv_trk_pt > %f && pv_trk_pt < %f", pv_trk_pt_edges[idx], pv_trk_pt_edges[idx + 1]);

    TString etacut_title = Form("%.2f<#it{#eta}<%.2f", pv_trk_eta_edges[idx], pv_trk_eta_edges[idx + 1]);
    TCut etacut = Form("pv_trk_eta > %f && pv_trk_eta < %f", pv_trk_eta_edges[idx], pv_trk_eta_edges[idx + 1]);

    TString phicut_title = Form("%.2f<#it{#phi}<%.2f", pv_trk_phi_edges[idx], pv_trk_phi_edges[idx + 1]);
    TCut phicut = Form("pv_trk_phi > %f && pv_trk_phi < %f", pv_trk_phi_edges[idx], pv_trk_phi_edges[idx + 1]);

    TString lopt_title = "0.1<#it{p_{T}}<1 GeV";
    TCut loptcut = "pv_trk_pt > 0.1 && pv_trk_pt < 1";

    TString hipt_title = "1<#it{p_{T}}<3 GeV";
    TCut hiptcut = "pv_trk_pt > 1 && pv_trk_pt < 3";

    TString ulpt_title = "3<#it{p_{T}}<10 GeV";
    TCut ulptcut = "pv_trk_pt > 3 && pv_trk_pt < 10";

    TString allpt_title = "0.1<#it{p_{T}}<10 GeV";
    TCut allptcut = "pv_trk_pt > 0.1 && pv_trk_pt < 10";

    TString loeta_title = "|#it{#eta}|<1.3";
    TCut loetacut = "abs(pv_trk_eta) < 1.3";

    TString hieta_title = "1.3<|#it{#eta}|<2.5";
    TCut hietacut = "abs(pv_trk_eta) > 1.3 && abs(pv_trk_eta) < 2.5";

    TString uleta_title = "2.5<|#it{#eta}|<3.0";
    TCut uletacut = "abs(pv_trk_eta) > 2.5 && abs(pv_trk_eta) < 3.0";

    TString alleta_title = "|#it{#eta}|<3.0";
    TCut alletacut = "abs(pv_trk_eta) < 3.0";

    auto result_reso_d0_pt_loeta = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + ptcut_title + "}{" + loeta_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (ptcut + loetacut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_pt_loeta_%d", idx),
        figdir + Form("ippv_xy_fit/mc_pt_loeta_%d", idx),
        -2000,
        2000,
        idx);

    auto result_reso_dz_pt_loeta = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + ptcut_title + "}{" + loeta_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (ptcut + loetacut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_pt_loeta_%d", idx),
        figdir + Form("ippv_z_fit/mc_pt_loeta_%d", idx),
        -2000,
        2000,
        idx);

    auto result_reso_d0_pt_hieta = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + ptcut_title + "}{" + hieta_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (ptcut + hietacut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_pt_hieta_%d", idx),
        figdir + Form("ippv_xy_fit/mc_pt_hieta_%d", idx),
        -3000,
        3000,
        idx);

    auto result_reso_dz_pt_hieta = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + ptcut_title + "}{" + hieta_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (ptcut + hietacut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_pt_hieta_%d", idx),
        figdir + Form("ippv_z_fit/mc_pt_hieta_%d", idx),
        -8000,
        8000,
        idx);

    auto result_reso_d0_pt_uleta = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + ptcut_title + "}{" + uleta_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (ptcut + uletacut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_pt_uleta_%d", idx),
        figdir + Form("ippv_xy_fit/mc_pt_uleta_%d", idx),
        -3000,
        3000,
        idx);

    auto result_reso_dz_pt_uleta = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + ptcut_title + "}{" + uleta_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (ptcut + uletacut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_pt_uleta_%d", idx),
        figdir + Form("ippv_z_fit/mc_pt_uleta_%d", idx),
        -8000,
        8000,
        idx);

    auto result_reso_d0_pt_alleta = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + ptcut_title + "}{" + alleta_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (ptcut + alletacut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_pt_alleta_%d", idx),
        figdir + Form("ippv_xy_fit/mc_pt_alleta_%d", idx),
        -3000,
        3000,
        idx);

    auto result_reso_dz_pt_alleta = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + ptcut_title + "}{" + alleta_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (ptcut + alletacut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_pt_alleta_%d", idx),
        figdir + Form("ippv_z_fit/mc_pt_alleta_%d", idx),
        -8000,
        8000,
        idx);

    auto result_reso_d0_eta_lopt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + etacut_title + "}{" + lopt_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (etacut + loptcut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_eta_lopt_%d", idx),
        figdir + Form("ippv_xy_fit/mc_eta_lopt_%d", idx),
        -2000,
        2000,
        idx);

    auto result_reso_dz_eta_lopt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + etacut_title + "}{" + lopt_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (etacut + loptcut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_eta_lopt_%d", idx),
        figdir + Form("ippv_z_fit/mc_eta_lopt_%d", idx),
        -10000,
        10000,
        idx);

    auto result_reso_d0_eta_hipt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + etacut_title + "}{" + hipt_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (etacut + hiptcut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_eta_hipt_%d", idx),
        figdir + Form("ippv_xy_fit/mc_eta_hipt_%d", idx),
        -1000,
        1000,
        idx);

    auto result_reso_dz_eta_hipt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + etacut_title + "}{" + hipt_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (etacut + hiptcut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_eta_hipt_%d", idx),
        figdir + Form("ippv_z_fit/mc_eta_hipt_%d", idx),
        -5000,
        5000,
        idx);

    auto result_reso_d0_eta_ulpt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + etacut_title + "}{" + ulpt_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (etacut + ulptcut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_eta_ulpt_%d", idx),
        figdir + Form("ippv_xy_fit/mc_eta_ulpt_%d", idx),
        -1000,
        1000,
        idx);

    auto result_reso_dz_eta_ulpt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + etacut_title + "}{" + ulpt_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (etacut + ulptcut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_eta_ulpt_%d", idx),
        figdir + Form("ippv_z_fit/mc_eta_ulpt_%d", idx),
        -5000,
        5000,
        idx);

    auto result_reso_d0_eta_allpt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + etacut_title + "}{" + allpt_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (etacut + allptcut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_eta_allpt_%d", idx),
        figdir + Form("ippv_xy_fit/mc_eta_allpt_%d", idx),
        -1000,
        1000,
        idx);

    auto result_reso_dz_eta_allpt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + etacut_title + "}{" + allpt_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (etacut + allptcut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_eta_allpt_%d", idx),
        figdir + Form("ippv_z_fit/mc_eta_allpt_%d", idx),
        -5000,
        5000,
        idx);

    auto result_reso_d0_phi_lopt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + phicut_title + "}{" + lopt_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (phicut + loptcut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_phi_lopt_%d", idx),
        figdir + Form("ippv_xy_fit/mc_phi_lopt_%d", idx),
        -1500,
        1500,
        idx);

    auto result_reso_dz_phi_lopt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + phicut_title + "}{" + lopt_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (phicut + loptcut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_phi_lopt_%d", idx),
        figdir + Form("ippv_z_fit/mc_phi_lopt_%d", idx),
        -3000,
        3000,
        idx);

    auto result_reso_d0_phi_hipt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + phicut_title + "}{" + hipt_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (phicut + hiptcut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_phi_hipt_%d", idx),
        figdir + Form("ippv_xy_fit/mc_phi_hipt_%d", idx),
        -500,
        500,
        idx);

    auto result_reso_dz_phi_hipt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + phicut_title + "}{" + hipt_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (phicut + hiptcut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_phi_hipt_%d", idx),
        figdir + Form("ippv_z_fit/mc_phi_hipt_%d", idx),
        -1000,
        1000,
        idx);

    auto result_reso_d0_phi_ulpt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + phicut_title + "}{" + ulpt_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (phicut + ulptcut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_phi_ulpt_%d", idx),
        figdir + Form("ippv_xy_fit/mc_phi_ulpt_%d", idx),
        -500,
        500,
        idx);

    auto result_reso_dz_phi_ulpt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + phicut_title + "}{" + ulpt_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (phicut + ulptcut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_phi_ulpt_%d", idx),
        figdir + Form("ippv_z_fit/mc_phi_ulpt_%d", idx),
        -1000,
        1000,
        idx);

    auto result_reso_d0_phi_allpt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_d0_pvunbiased",
        "#splitline{" + phicut_title + "}{" + allpt_title + "};Track IP #it{d_{xy}} [#mum];# tracks",
        Form("PU_factor*(%s)", (phicut + allptcut).GetTitle()),
        period,
        figdir + Form("ippv_xy_fit/data_phi_allpt_%d", idx),
        figdir + Form("ippv_xy_fit/mc_phi_allpt_%d", idx),
        -500,
        500,
        idx);

    auto result_reso_dz_phi_allpt = fill_to_fit(
        datatree,
        mctree,
        "pv_trk_dz_pvunbiased",
        "#splitline{" + phicut_title + "}{" + allpt_title + "};Track IP #it{d_{z}} [#mum];# tracks",
        Form("PU_factor*(%s)", (phicut + allptcut).GetTitle()),
        period,
        figdir + Form("ippv_z_fit/data_phi_allpt_%d", idx),
        figdir + Form("ippv_z_fit/mc_phi_allpt_%d", idx),
        -1000,
        1000,
        idx);

    nlohmann::json resojson;
    resojson["pt"] = (pv_trk_pt_edges[idx] + pv_trk_pt_edges[idx + 1]) / 2;
    resojson["eta"] = (pv_trk_eta_edges[idx] + pv_trk_eta_edges[idx + 1]) / 2;
    resojson["phi"] = (pv_trk_phi_edges[idx] + pv_trk_phi_edges[idx + 1]) / 2;

    resojson["reso_data_d0_pt_loeta"] = result_reso_d0_pt_loeta.first;
    resojson["reso_data_dz_pt_loeta"] = result_reso_dz_pt_loeta.first;
    resojson["reso_data_d0_pt_hieta"] = result_reso_d0_pt_hieta.first;
    resojson["reso_data_dz_pt_hieta"] = result_reso_dz_pt_hieta.first;
    resojson["reso_data_d0_pt_uleta"] = result_reso_d0_pt_uleta.first;
    resojson["reso_data_dz_pt_uleta"] = result_reso_dz_pt_uleta.first;
    resojson["reso_data_d0_pt_alleta"] = result_reso_d0_pt_alleta.first;
    resojson["reso_data_dz_pt_alleta"] = result_reso_dz_pt_alleta.first;

    resojson["reso_data_d0_eta_lopt"] = result_reso_d0_eta_lopt.first;
    resojson["reso_data_dz_eta_lopt"] = result_reso_dz_eta_lopt.first;
    resojson["reso_data_d0_eta_hipt"] = result_reso_d0_eta_hipt.first;
    resojson["reso_data_dz_eta_hipt"] = result_reso_dz_eta_hipt.first;
    resojson["reso_data_d0_eta_ulpt"] = result_reso_d0_eta_ulpt.first;
    resojson["reso_data_dz_eta_ulpt"] = result_reso_dz_eta_ulpt.first;
    resojson["reso_data_d0_eta_allpt"] = result_reso_d0_eta_allpt.first;
    resojson["reso_data_dz_eta_allpt"] = result_reso_dz_eta_allpt.first;

    resojson["reso_data_d0_phi_lopt"] = result_reso_d0_phi_lopt.first;
    resojson["reso_data_dz_phi_lopt"] = result_reso_dz_phi_lopt.first;
    resojson["reso_data_d0_phi_hipt"] = result_reso_d0_phi_hipt.first;
    resojson["reso_data_dz_phi_hipt"] = result_reso_dz_phi_hipt.first;
    resojson["reso_data_d0_phi_ulpt"] = result_reso_d0_phi_ulpt.first;
    resojson["reso_data_dz_phi_ulpt"] = result_reso_dz_phi_ulpt.first;
    resojson["reso_data_d0_phi_allpt"] = result_reso_d0_phi_allpt.first;
    resojson["reso_data_dz_phi_allpt"] = result_reso_dz_phi_allpt.first;

    resojson["reso_mc_d0_pt_loeta"] = result_reso_d0_pt_loeta.second;
    resojson["reso_mc_dz_pt_loeta"] = result_reso_dz_pt_loeta.second;
    resojson["reso_mc_d0_pt_hieta"] = result_reso_d0_pt_hieta.second;
    resojson["reso_mc_dz_pt_hieta"] = result_reso_dz_pt_hieta.second;
    resojson["reso_mc_d0_pt_uleta"] = result_reso_d0_pt_uleta.second;
    resojson["reso_mc_dz_pt_uleta"] = result_reso_dz_pt_uleta.second;
    resojson["reso_mc_d0_pt_alleta"] = result_reso_d0_pt_alleta.second;
    resojson["reso_mc_dz_pt_alleta"] = result_reso_dz_pt_alleta.second;

    resojson["reso_mc_d0_eta_lopt"] = result_reso_d0_eta_lopt.second;
    resojson["reso_mc_dz_eta_lopt"] = result_reso_dz_eta_lopt.second;
    resojson["reso_mc_d0_eta_hipt"] = result_reso_d0_eta_hipt.second;
    resojson["reso_mc_dz_eta_hipt"] = result_reso_dz_eta_hipt.second;
    resojson["reso_mc_d0_eta_ulpt"] = result_reso_d0_eta_ulpt.second;
    resojson["reso_mc_dz_eta_ulpt"] = result_reso_dz_eta_ulpt.second;
    resojson["reso_mc_d0_eta_allpt"] = result_reso_d0_eta_allpt.second;
    resojson["reso_mc_dz_eta_allpt"] = result_reso_dz_eta_allpt.second;

    resojson["reso_mc_d0_phi_lopt"] = result_reso_d0_phi_lopt.second;
    resojson["reso_mc_dz_phi_lopt"] = result_reso_dz_phi_lopt.second;
    resojson["reso_mc_d0_phi_hipt"] = result_reso_d0_phi_hipt.second;
    resojson["reso_mc_dz_phi_hipt"] = result_reso_dz_phi_hipt.second;
    resojson["reso_mc_d0_phi_ulpt"] = result_reso_d0_phi_ulpt.second;
    resojson["reso_mc_dz_phi_ulpt"] = result_reso_dz_phi_ulpt.second;
    resojson["reso_mc_d0_phi_allpt"] = result_reso_d0_phi_allpt.second;
    resojson["reso_mc_dz_phi_allpt"] = result_reso_dz_phi_allpt.second;

    std::ofstream outFile(storage_dir + "/json/" + period + Form("/ip_res/fit_%d.json", idx));
    outFile << resojson.dump(4);
    outFile.close();

    datafile->Close();
    mcfile->Close();
    delete datafile;
    delete mcfile;

    return 0;
}
