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

    RooRealVar pv_var("pv_var", "pv_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    pv_var.setBins(hist->GetNbinsX());
    RooRealVar mu("mu", "mu", hist->GetMean(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar sigma1("sigma1", "sigma1", 0.5 * hist->GetRMS(), 0., hist->GetRMS());
    RooRealVar sigma2("sigma2", "sigma2", hist->GetRMS(), hist->GetRMS() * 0.5, hist->GetRMS() * 2);
    RooRealVar sigma3("sigma3", "sigma3", 1.5 * hist->GetRMS(), hist->GetRMS(), hist->GetRMS() * 3);
    RooGaussian gauss1("gauss1", "gauss1", pv_var, mu, sigma1);
    RooGaussian gauss2("gauss2", "gauss2", pv_var, mu, sigma2);
    RooGaussian gauss3("gauss3", "gauss3", pv_var, mu, sigma3);
    RooRealVar f1("f1", "f1", 0.3, 0.0, 1.0);
    RooRealVar f2("f2", "f2", 0.3, 0.0, 1.0);
    RooFormulaVar f3("f3", "1 - f1 - f2", RooArgList(f1, f2));
    RooAddPdf triGauss("triGauss", "triGauss", RooArgList(gauss1, gauss2, gauss3), RooArgList(f1, f2, f3));

    RooDataHist hdatahist("hdatahist", "", pv_var, hist);
    RooFitResult *fitResult = triGauss.fitTo(hdatahist, RooFit::Save(true));
    fitResult->Print();
    delete fitResult;

    Float_t pv_var_max = pv_var.getMax();
    Float_t mean = mu.getVal();
    Float_t low = 0.0;
    Float_t high = pv_var_max - mean;
    while (high - low > tolerance)
    {
        Float_t mid = 0.5 * (low + high);
        pv_var.setRange("intRange", mean - mid, mean + mid);
        RooAbsReal *integral = triGauss.createIntegral(pv_var, RooFit::NormSet(pv_var), RooFit::Range("intRange"));
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
    canvas->SetFillColor(0);
    canvas->SetFrameFillColor(0);
    RooPlot *frame = pv_var.frame();

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

    Float_t datareso = fit_res(datahist, period, "Data", datafigpath);
    Float_t mcreso = fit_res(mchist, period, "Simulation", mcfigpath);

    delete datahist;
    delete mchist;

    return {datareso, mcreso};
}

Int_t pv_res(TString period, Int_t idx)
{
    TString figdir = storage_dir + "/figures/" + period + "/pv_res/";

    TFile *datafile = TFile::Open(storage_dir + "/tuples/" + period + "/data_corr.root");
    TTree *datatree = (TTree *)datafile->Get("mytree");

    TFile *mcfile = TFile::Open(storage_dir + "/tuples/" + period + "/mc_corr.root");
    TTree *mctree = (TTree *)mcfile->Get("mytree");

    std::ifstream infile(storage_dir + "/json/" + period + "/binning.json");
    nlohmann::json binning;
    infile >> binning;
    infile.close();

    std::vector<Float_t> pv_SumTrackPt2_sqrt_edges = binning["pv_SumTrackPt2_sqrt"].get<std::vector<Float_t>>();

    TString ptcut_title = Form("%.2f<#sqrt{#sum#it{p_{T}}^{2}}<%.2f GeV", pv_SumTrackPt2_sqrt_edges[idx], pv_SumTrackPt2_sqrt_edges[idx + 1]);
    TCut ptcut = Form("sqrt(pv_SumTrackPt2) > %f && sqrt(pv_SumTrackPt2) < %f", pv_SumTrackPt2_sqrt_edges[idx], pv_SumTrackPt2_sqrt_edges[idx + 1]);
    TCut xnull_cut = "pv_x_p1 != -777 && pv_x_p2 != -777";
    TCut ynull_cut = "pv_y_p1 != -777 && pv_y_p2 != -777";
    TCut znull_cut = "pv_z_p1 != -777 && pv_z_p2 != -777";

    TString pvx_cut = Form("PU_factor*(%s)", (ptcut + xnull_cut).GetTitle());
    TString pvy_cut = Form("PU_factor*(%s)", (ptcut + ynull_cut).GetTitle());
    TString pvz_cut = Form("PU_factor*(%s)", (ptcut + znull_cut).GetTitle());

    auto result_reso_pvx = fill_to_fit(
        datatree,
        mctree,
        "(pv_x_p1 - pv_x_p2)/sqrt(2)",
        ptcut_title + ";(#it{x}_{1}-#it{x}_{2})/#sqrt{2} [#mum];# PV",
        Form("PU_factor*(%s)", (ptcut + xnull_cut).GetTitle()),
        period,
        figdir + Form("pvx_fit/data_pt_%d", idx),
        figdir + Form("pvx_fit/mc_pt_%d", idx),
        -300,
        300,
        idx);

    auto result_reso_pvy = fill_to_fit(
        datatree,
        mctree,
        "(pv_y_p1 - pv_y_p2)/sqrt(2)",
        ptcut_title + ";(#it{y}_{1}-#it{y}_{2})/#sqrt{2} [#mum];# PV",
        Form("PU_factor*(%s)", (ptcut + ynull_cut).GetTitle()),
        period,
        figdir + Form("pvy_fit/data_pt_%d", idx),
        figdir + Form("pvy_fit/mc_pt_%d", idx),
        -300,
        300,
        idx);

    auto result_reso_pvz = fill_to_fit(
        datatree,
        mctree,
        "(pv_z_p1 - pv_z_p2)/sqrt(2)",
        ptcut_title + ";(#it{z}_{1}-#it{z}_{2})/#sqrt{2} [mm];# PV",
        Form("PU_factor*(%s)", (ptcut + znull_cut).GetTitle()),
        period,
        figdir + Form("pvz_fit/data_pt_%d", idx),
        figdir + Form("pvz_fit/mc_pt_%d", idx),
        -300,
        300,
        idx);

    auto result_reso_pullx = fill_to_fit(
        datatree,
        mctree,
        "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))",
        ptcut_title + ";(#it{x}_{1}-#it{x}_{2})/#sqrt{#Delta#it{x}_{1}^{2}+#Delta#it{x}_{2}^{2}};# PV",
        Form("PU_factor*(%s)", (ptcut + xnull_cut).GetTitle()),
        period,
        figdir + Form("pullx_fit/data_pt_%d", idx),
        figdir + Form("pullx_fit/mc_pt_%d", idx),
        -10,
        10,
        idx);

    auto result_reso_pully = fill_to_fit(
        datatree,
        mctree,
        "(pv_y_p1 - pv_y_p2)/sqrt(pow(pv_yError_p1,2)+pow(pv_yError_p2,2))",
        ptcut_title + ";(#it{y}_{1}-#it{y}_{2})/#sqrt{#Delta#it{y}_{1}^{2}+#Delta#it{y}_{2}^{2}};# PV",
        Form("PU_factor*(%s)", (ptcut + ynull_cut).GetTitle()),
        period,
        figdir + Form("pully_fit/data_pt_%d", idx),
        figdir + Form("pully_fit/mc_pt_%d", idx),
        -10,
        10,
        idx);

    auto result_reso_pullz = fill_to_fit(
        datatree,
        mctree,
        "(pv_z_p1 - pv_z_p2)/sqrt(pow(pv_zError_p1,2)+pow(pv_zError_p2,2))",
        ptcut_title + ";(#it{z}_{1}-#it{z}_{2})/#sqrt{#Delta#it{z}_{1}^{2}+#Delta#it{z}_{2}^{2}};# PV",
        Form("PU_factor*(%s)", (ptcut + znull_cut).GetTitle()),
        period,
        figdir + Form("pullz_fit/data_pt_%d", idx),
        figdir + Form("pullz_fit/mc_pt_%d", idx),
        -10,
        10,
        idx);

    nlohmann::json resojson;
    resojson["sumpt2_sqrt"] = (pv_SumTrackPt2_sqrt_edges[idx] + pv_SumTrackPt2_sqrt_edges[idx + 1]) / 2;
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
    std::ofstream outFile(storage_dir + "/json/" + period + Form("/pv_res/fit_%d.json", idx));
    outFile << resojson.dump(4);
    outFile.close();

    datafile->Close();
    mcfile->Close();

    return 0;
}
