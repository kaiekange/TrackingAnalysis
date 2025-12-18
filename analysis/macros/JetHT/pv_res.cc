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

const TString datatype_text = "High-q^{2} multi-jet events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";

const Int_t nbins = 100;

Float_t fit_res(TH1F *hist, TString period, TString sampletype, TString figpath, Float_t tolerance = 1e-4)
{
    setTDRStyle();
    TString period_title = period;
    period_title.ReplaceAll("_", " ");
    lumi_sqrtS = "13.6 TeV, " + period_title;

    RooRealVar pv_var("pv_var", "pv_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    pv_var.setBins(hist->GetNbinsX());

    double hist_mean = hist->GetMean();
    double hist_rms = hist->GetRMS();

    // 均值
    RooRealVar mu("mu", "mu", hist_mean, hist_mean - hist_rms, hist_mean + hist_rms);

    // 核心宽度
    RooRealVar sigma("sigma", "sigma", 0.5*hist_rms, 0.1*hist_rms, hist_rms);

    // 对称的尾部参数
    RooRealVar alpha("alpha", "alpha", 2.0, 0.5, 5.0);  // 尾部切换点
    RooRealVar n("n", "n", 2.0, 0.5, 10.0);              // 尾部幂律指数

    // 创建对称的Double Crystal Ball
    RooCrystalBall model("model", "Double Crystal Ball", pv_var, mu, sigma, 
                    alpha, n, alpha, n);  // 左右使用相同的alpha和n保证对称

    RooDataHist hdatahist("hdatahist", "", pv_var, hist);
    RooFitResult *fitResult = model.fitTo(hdatahist, RooFit::Save(true));
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
        RooAbsReal *integral = model.createIntegral(pv_var, RooFit::NormSet(pv_var), RooFit::Range("intRange"));
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
    model.plotOn(frame, RooFit::Name("model"), RooFit::Components("model"), RooFit::LineStyle(9), RooFit::LineColor(kRed), RooFit::LineWidth(2.0), RooFit::DrawOption("L"));
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

std::pair<Float_t, Float_t> fill_to_fit(TTree *datatree, TTree *mctree, TString var_, TString title_, TCut cut_, TString period, TString datafigpath, TString mcfigpath, Float_t lowbound, Float_t highbound, Int_t idx)
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

    TH1F *datahist = new TH1F(hdata_name, title_, nbins, hist_mean - 8 * hist_stddev, hist_mean + 8 * hist_stddev);
    datatree->Project(hdata_name, var_, cut_);

    TH1F *mchist = new TH1F(hmc_name, title_, nbins, hist_mean - 8 * hist_stddev, hist_mean + 8 * hist_stddev);
    mctree->Project(hmc_name, var_, Form("Xsec_weight * PS_weight * TRG_mask * PU_factor *(%s)", cut_.GetTitle()));

    Float_t datareso = fit_res(datahist, period, "Data", datafigpath);
    Float_t mcreso = fit_res(mchist, period, "Simulation", mcfigpath);

    delete datahist;
    delete mchist;

    return {datareso, mcreso};
}

Int_t pv_res(TString period, Int_t idx)
{
    TString figdir = storage_dir + "/figures/" + period + "/pv_res/";

    TFile *datafile = TFile::Open(storage_dir + "/tuples/" + period + "/data.root");
    TTree *datatree = (TTree *)datafile->Get("mytree");

    TFile *mcfile = TFile::Open(storage_dir + "/tuples/" + period + "/mc_corr_mask.root");
    TTree *mctree = (TTree *)mcfile->Get("mytree");

    // std::ifstream infile(storage_dir + "/json/" + period + "/binning.json");
    std::ifstream infile(storage_dir + "/json/binning.json");
    nlohmann::json binning;
    infile >> binning;
    infile.close();

    // Int_t idx_lo = idx;
    // Int_t idx_hi = idx+1;
    Int_t idx_lo = 2 * idx;
    Int_t idx_hi = 2 * (idx + 1);

    std::vector<Float_t> pv_SumTrackPt2_sqrt_edges = binning["pv_SumTrackPt2_sqrt"].get<std::vector<Float_t>>();

    TString ptcut_title = Form("%.2f<#sqrt{#sum#it{p_{T}}^{2}}<%.2f GeV", pv_SumTrackPt2_sqrt_edges[idx_lo], pv_SumTrackPt2_sqrt_edges[idx_hi]);
    TCut ptcut = Form("sqrt(pv_SumTrackPt2) > %f && sqrt(pv_SumTrackPt2) < %f", pv_SumTrackPt2_sqrt_edges[idx_lo], pv_SumTrackPt2_sqrt_edges[idx_hi]);
    TCut xnull_cut = "pv_x_p1 != -777 && pv_x_p2 != -777";
    TCut ynull_cut = "pv_y_p1 != -777 && pv_y_p2 != -777";
    TCut znull_cut = "pv_z_p1 != -777 && pv_z_p2 != -777";

    auto result_reso_pvx = fill_to_fit(
        datatree,
        mctree,
        "(pv_x_p1 - pv_x_p2)/sqrt(2)",
        ptcut_title + ";(#it{x}_{1}-#it{x}_{2})/#sqrt{2} [#mum];# PV",
        ptcut + xnull_cut,
        period,
        figdir + Form("pvx_fit/data_pt_%d", idx),
        figdir + Form("pvx_fit/mc_pt_%d", idx),
        -100,
        100,
        idx);

    auto result_reso_pvy = fill_to_fit(
        datatree,
        mctree,
        "(pv_y_p1 - pv_y_p2)/sqrt(2)",
        ptcut_title + ";(#it{y}_{1}-#it{y}_{2})/#sqrt{2} [#mum];# PV",
        ptcut + ynull_cut,
        period,
        figdir + Form("pvy_fit/data_pt_%d", idx),
        figdir + Form("pvy_fit/mc_pt_%d", idx),
        -100,
        100,
        idx);

    auto result_reso_pvz = fill_to_fit(
        datatree,
        mctree,
        "(pv_z_p1 - pv_z_p2)/sqrt(2)",
        ptcut_title + ";(#it{z}_{1}-#it{z}_{2})/#sqrt{2} [mm];# PV",
        ptcut + znull_cut,
        period,
        figdir + Form("pvz_fit/data_pt_%d", idx),
        figdir + Form("pvz_fit/mc_pt_%d", idx),
        -200,
        200,
        idx);

    auto result_reso_pullx = fill_to_fit(
        datatree,
        mctree,
        "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))",
        ptcut_title + ";(#it{x}_{1}-#it{x}_{2})/#sqrt{#Delta#it{x}_{1}^{2}+#Delta#it{x}_{2}^{2}};# PV",
        ptcut + xnull_cut,
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
        ptcut + ynull_cut,
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
        ptcut + znull_cut,
        period,
        figdir + Form("pullz_fit/data_pt_%d", idx),
        figdir + Form("pullz_fit/mc_pt_%d", idx),
        -10,
        10,
        idx);

    nlohmann::json resojson;
    resojson["sumpt2_sqrt"] = (pv_SumTrackPt2_sqrt_edges[idx_lo] + pv_SumTrackPt2_sqrt_edges[idx_hi]) / 2;
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
