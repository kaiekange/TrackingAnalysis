#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TH1.h>
#include <TMath.h>

#include <RooRealVar.h>
#include <RooGaussian.h>
#include <RooAddPdf.h>
#include <RooDataHist.h>
#include <RooPlot.h>
#include <RooFitResult.h>
#include <RooFormulaVar.h>

#include <nlohmann/json.hpp>

#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"

const TString datatype_text = "Unbiased collision events";
const TString storage_dir   = "/eos/home-k/kakang/IPres/analysis/ZeroBias";

// 抽样：每 sample_mod 个事件取 1 个；=1 表示不用抽样
const int sample_mod = 1;  // 视数据量调整，比如 5 或 10

const int nbins = 200;
const double nsigma = 8.0;

// 组合 ID：和你原来 fill_to_fit 调用一一对应
enum CombId {
    PT_LOETA = 0,
    PT_HIETA,
    PT_ULETA,
    PT_ALLETA,
    ETA_LOPT,
    ETA_HIPT,
    ETA_ULPT,
    ETA_ALLPT,
    PHI_LOPT,
    PHI_HIPT,
    PHI_ULPT,
    PHI_ALLPT,
    NCOMB
};

// 字符串后缀，用于 JSON key 和图像路径
const char* COMB_SUFFIX[NCOMB] = {
    "pt_loeta",
    "pt_hieta",
    "pt_uleta",
    "pt_alleta",
    "eta_lopt",
    "eta_hipt",
    "eta_ulpt",
    "eta_allpt",
    "phi_lopt",
    "phi_hipt",
    "phi_ulpt",
    "phi_allpt"
};

// 简单统计结构：加权 mean / sigma
struct Stat {
    double sw   = 0.0;  // sum w
    double swx  = 0.0;  // sum w*x
    double swx2 = 0.0;  // sum w*x^2

    void Fill(double x, double w) {
        sw   += w;
        swx  += w * x;
        swx2 += w * x * x;
    }

    bool Valid() const { return sw > 0.0; }

    double Mean() const {
        return sw > 0.0 ? swx / sw : 0.0;
    }

    double Sigma() const {
        if (sw <= 0.0) return 0.0;
        double m = swx / sw;
        double v = swx2 / sw - m * m;
        return v > 0.0 ? std::sqrt(v) : 0.0;
    }
};

// 单个直方图配置：标题和图像路径
struct HistInfo {
    TString title;
    TString dataFigPath;
    TString mcFigPath;
};

// 三高斯拟合 + 68% 区间解分辨率（基本保留你的实现）
Float_t fit_res(TH1F *hist, TString period, TString sampletype, TString figpath, Float_t tolerance = 1e-4)
{
    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, " + period;

    RooRealVar ip_var("ip_var", "ip_var",
                      hist->GetXaxis()->GetXmin(),
                      hist->GetXaxis()->GetXmax());
    ip_var.setBins(hist->GetNbinsX());

    RooRealVar mu("mu", "mu",
                  hist->GetMean(),
                  hist->GetXaxis()->GetXmin(),
                  hist->GetXaxis()->GetXmax());
    RooRealVar sigma1("sigma1", "sigma1",
                      0.5 * hist->GetRMS(), 0., hist->GetRMS());
    RooRealVar sigma2("sigma2", "sigma2",
                      hist->GetRMS(), hist->GetRMS() * 0.5, hist->GetRMS() * 2);
    RooRealVar sigma3("sigma3", "sigma3",
                      1.5 * hist->GetRMS(), hist->GetRMS(), hist->GetRMS() * 3);

    RooGaussian gauss1("gauss1", "gauss1", ip_var, mu, sigma1);
    RooGaussian gauss2("gauss2", "gauss2", ip_var, mu, sigma2);
    RooGaussian gauss3("gauss3", "gauss3", ip_var, mu, sigma3);

    RooRealVar f1("f1", "f1", 0.3, 0.0, 1.0);
    RooRealVar f2("f2", "f2", 0.3, 0.0, 1.0);
    RooFormulaVar f3("f3", "1 - f1 - f2", RooArgList(f1, f2));

    RooAddPdf triGauss("triGauss", "triGauss",
                       RooArgList(gauss1, gauss2, gauss3),
                       RooArgList(f1, f2, f3));

    RooDataHist hdatahist("hdatahist", "", ip_var, hist);
    RooFitResult *fitResult = triGauss.fitTo(hdatahist, RooFit::Save(true));
    fitResult->Print();
    delete fitResult;

    Float_t ip_var_max = ip_var.getMax();
    Float_t mean = mu.getVal();
    Float_t low  = 0.0;
    Float_t high = ip_var_max - mean;

    while (high - low > tolerance)
    {
        Float_t mid = 0.5 * (low + high);
        ip_var.setRange("intRange", mean - mid, mean + mid);
        RooAbsReal *integral = triGauss.createIntegral(
            ip_var, RooFit::NormSet(ip_var), RooFit::Range("intRange"));
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

    hdatahist.plotOn(frame,
                     RooFit::Name(sampletype),
                     RooFit::MarkerColor(kBlack),
                     RooFit::MarkerSize(1.1),
                     RooFit::Binning(hist->GetNbinsX()),
                     RooFit::DrawOption("ep"));
    triGauss.plotOn(frame,
                    RooFit::Name("triGauss"),
                    RooFit::Components("triGauss"),
                    RooFit::LineStyle(9),
                    RooFit::LineColor(kRed),
                    RooFit::LineWidth(2.0),
                    RooFit::DrawOption("L"));

    frame->Draw("");
    frame->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    frame->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    frame->GetYaxis()->SetNdivisions(810);
    frame->GetXaxis()->SetNdivisions(810);
    frame->SetMinimum(0);
    frame->SetMaximum(frame->GetMaximum() * 1.3);

    write_text(0.6, 0.88, datatype_text);
    write_text(0.6, 0.8,  hist->GetTitle());
    write_text(0.6, 0.7,  sampletype + " fit results:");
    write_text(0.6, 0.65, Form("reso = %.*f",
                               std::max(0, 2 - (Int_t)floor(log10(reso))), reso));
    CMS_lumi(canvas);

    canvas->Update();
    canvas->SaveAs(figpath + ".png");
    delete canvas;

    return reso;
}

// 统一的 track 处理：对每个满足条件的组合调用 action(combId, d0, dz, weight)
template <typename F>
void process_tree_tracks(TTree *tree,
                         Long64_t nEntries,
                         int idx,
                         const std::vector<Float_t> &pv_trk_pt_edges,
                         const std::vector<Float_t> &pv_trk_eta_edges,
                         const std::vector<Float_t> &pv_trk_phi_edges,
                         F &&action)
{
    // 共享的分支指针（外面会 SetBranchAddress 一次即可）
    static std::vector<Float_t> *pv_trk_pt  = nullptr;
    static std::vector<Float_t> *pv_trk_eta = nullptr;
    static std::vector<Float_t> *pv_trk_phi = nullptr;
    static std::vector<Float_t> *pv_trk_d0  = nullptr;
    static std::vector<Float_t> *pv_trk_dz  = nullptr;
    static Float_t PU_factor = 1.0;

    tree->SetBranchAddress("pv_trk_pt",  &pv_trk_pt);
    tree->SetBranchAddress("pv_trk_eta", &pv_trk_eta);
    tree->SetBranchAddress("pv_trk_phi", &pv_trk_phi);
    tree->SetBranchAddress("pv_trk_d0_pvunbiased", &pv_trk_d0);
    tree->SetBranchAddress("pv_trk_dz_pvunbiased", &pv_trk_dz);
    tree->SetBranchAddress("PU_factor", &PU_factor);

    // 预先算好与 idx 相关的一些区间
    const float pt_lo  = pv_trk_pt_edges[idx];
    const float pt_hi  = pv_trk_pt_edges[idx + 1];
    const float eta_lo = pv_trk_eta_edges[idx];
    const float eta_hi = pv_trk_eta_edges[idx + 1];
    const float phi_lo = pv_trk_phi_edges[idx];
    const float phi_hi = pv_trk_phi_edges[idx + 1];

    for (Long64_t ie = 0; ie < nEntries; ++ie)
    {
        // if (sample_mod > 1 && (ie % sample_mod) != 0) continue;

        tree->GetEntry(ie);

        const size_t nTrk = pv_trk_pt->size();
        for (size_t it = 0; it < nTrk; ++it)
        {
            float pt  = pv_trk_pt->at(it);
            float eta = pv_trk_eta->at(it);
            float phi = pv_trk_phi->at(it);
            float d0  = pv_trk_d0->at(it);
            float dz  = pv_trk_dz->at(it);
            float w   = PU_factor;

            const float aeta = std::fabs(eta);

            // bin 定义（与你原来一致）
            bool in_ptbin  = (pt  > pt_lo  && pt  < pt_hi);
            bool in_etabin = (eta > eta_lo && eta < eta_hi);
            bool in_phibin = (phi > phi_lo && phi < phi_hi);

            bool loeta = (aeta < 1.3);
            bool hieta = (aeta > 1.3 && aeta < 2.5);
            bool uleta = (aeta > 2.5 && aeta < 3.0);
            bool alleta = (aeta < 3.0);

            bool lopt  = (pt > 0.1 && pt < 1.0);
            bool hipt  = (pt > 1.0 && pt < 3.0);
            bool ulpt  = (pt > 3.0 && pt < 10.0);
            bool allpt = (pt > 0.1 && pt < 10.0);

            // pt × eta
            if (in_ptbin && loeta)  action(PT_LOETA,  d0, dz, w);
            if (in_ptbin && hieta)  action(PT_HIETA,  d0, dz, w);
            if (in_ptbin && uleta)  action(PT_ULETA,  d0, dz, w);
            if (in_ptbin && alleta) action(PT_ALLETA, d0, dz, w);

            // eta × pt
            if (in_etabin && lopt)  action(ETA_LOPT,  d0, dz, w);
            if (in_etabin && hipt)  action(ETA_HIPT,  d0, dz, w);
            if (in_etabin && ulpt)  action(ETA_ULPT,  d0, dz, w);
            if (in_etabin && allpt) action(ETA_ALLPT, d0, dz, w);

            // phi × pt
            if (in_phibin && lopt)  action(PHI_LOPT,  d0, dz, w);
            if (in_phibin && hipt)  action(PHI_HIPT,  d0, dz, w);
            if (in_phibin && ulpt)  action(PHI_ULPT,  d0, dz, w);
            if (in_phibin && allpt) action(PHI_ALLPT, d0, dz, w);
        }
    }
}

Int_t ip_res2(TString period, Int_t idx)
{
    // 输出目录
    TString figdir = storage_dir + "/figures/" + period + "/ip_res/";

    // 打开 data / MC 文件
    TFile *datafile = TFile::Open(storage_dir + "/tuples/" + period + "/data_corr.root");
    TTree *datatree = (TTree *)datafile->Get("mytree");

    TFile *mcfile   = TFile::Open(storage_dir + "/tuples/" + period + "/mc_corr.root");
    TTree *mctree   = (TTree *)mcfile->Get("mytree");

    // 读 binning.json
    std::ifstream infile(storage_dir + "/json/" + period + "/binning.json");
    nlohmann::json binning;
    infile >> binning;
    infile.close();

    std::vector<Float_t> pv_trk_pt_edges  = binning["pv_trk_pt"].get<std::vector<Float_t>>();
    std::vector<Float_t> pv_trk_eta_edges = binning["pv_trk_eta"].get<std::vector<Float_t>>();
    std::vector<Float_t> pv_trk_phi_edges = binning["pv_trk_phi"].get<std::vector<Float_t>>();

    // 文本（沿用你原来的）
    TString ptcut_title  = Form("%.3f<#it{p_{T}}<%.3f GeV",
                                pv_trk_pt_edges[idx], pv_trk_pt_edges[idx + 1]);
    TString etacut_title = Form("%.2f<#it{#eta}<%.2f",
                                pv_trk_eta_edges[idx], pv_trk_eta_edges[idx + 1]);
    TString phicut_title = Form("%.2f<#it{#phi}<%.2f",
                                pv_trk_phi_edges[idx], pv_trk_phi_edges[idx + 1]);

    TString lopt_title  = "0.1<#it{p_{T}}<1 GeV";
    TString hipt_title  = "1<#it{p_{T}}<3 GeV";
    TString ulpt_title  = "3<#it{p_{T}}<10 GeV";
    TString allpt_title = "0.1<#it{p_{T}}<10 GeV";

    TString loeta_title  = "|#it{#eta}|<1.3";
    TString hieta_title  = "1.3<|#it{#eta}|<2.5";
    TString uleta_title  = "2.5<|#it{#eta}|<3.0";
    TString alleta_title = "|#it{#eta}|<3.0";

    // 准备 HistInfo（标题 & 图像路径）—— d0
    HistInfo info_d0[NCOMB];
    HistInfo info_dz[NCOMB];

    // pt × eta
    info_d0[PT_LOETA].title      = "#splitline{" + ptcut_title + "}{" + loeta_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PT_LOETA].dataFigPath= figdir + Form("ippv_xy_fit/data_pt_loeta_%d", idx);
    info_d0[PT_LOETA].mcFigPath  = figdir + Form("ippv_xy_fit/mc_pt_loeta_%d",   idx);

    info_dz[PT_LOETA].title      = "#splitline{" + ptcut_title + "}{" + loeta_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PT_LOETA].dataFigPath= figdir + Form("ippv_z_fit/data_pt_loeta_%d", idx);
    info_dz[PT_LOETA].mcFigPath  = figdir + Form("ippv_z_fit/mc_pt_loeta_%d",   idx);

    info_d0[PT_HIETA].title      = "#splitline{" + ptcut_title + "}{" + hieta_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PT_HIETA].dataFigPath= figdir + Form("ippv_xy_fit/data_pt_hieta_%d", idx);
    info_d0[PT_HIETA].mcFigPath  = figdir + Form("ippv_xy_fit/mc_pt_hieta_%d",   idx);

    info_dz[PT_HIETA].title      = "#splitline{" + ptcut_title + "}{" + hieta_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PT_HIETA].dataFigPath= figdir + Form("ippv_z_fit/data_pt_hieta_%d", idx);
    info_dz[PT_HIETA].mcFigPath  = figdir + Form("ippv_z_fit/mc_pt_hieta_%d",   idx);

    info_d0[PT_ULETA].title      = "#splitline{" + ptcut_title + "}{" + uleta_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PT_ULETA].dataFigPath= figdir + Form("ippv_xy_fit/data_pt_uleta_%d", idx);
    info_d0[PT_ULETA].mcFigPath  = figdir + Form("ippv_xy_fit/mc_pt_uleta_%d",   idx);

    info_dz[PT_ULETA].title      = "#splitline{" + ptcut_title + "}{" + uleta_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PT_ULETA].dataFigPath= figdir + Form("ippv_z_fit/data_pt_uleta_%d", idx);
    info_dz[PT_ULETA].mcFigPath  = figdir + Form("ippv_z_fit/mc_pt_uleta_%d",   idx);

    info_d0[PT_ALLETA].title      = "#splitline{" + ptcut_title + "}{" + alleta_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PT_ALLETA].dataFigPath= figdir + Form("ippv_xy_fit/data_pt_alleta_%d", idx);
    info_d0[PT_ALLETA].mcFigPath  = figdir + Form("ippv_xy_fit/mc_pt_alleta_%d",   idx);

    info_dz[PT_ALLETA].title      = "#splitline{" + ptcut_title + "}{" + alleta_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PT_ALLETA].dataFigPath= figdir + Form("ippv_z_fit/data_pt_alleta_%d", idx);
    info_dz[PT_ALLETA].mcFigPath  = figdir + Form("ippv_z_fit/mc_pt_alleta_%d",   idx);

    // eta × pt
    info_d0[ETA_LOPT].title      = "#splitline{" + etacut_title + "}{" + lopt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[ETA_LOPT].dataFigPath= figdir + Form("ippv_xy_fit/data_eta_lopt_%d", idx);
    info_d0[ETA_LOPT].mcFigPath  = figdir + Form("ippv_xy_fit/mc_eta_lopt_%d",   idx);

    info_dz[ETA_LOPT].title      = "#splitline{" + etacut_title + "}{" + lopt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[ETA_LOPT].dataFigPath= figdir + Form("ippv_z_fit/data_eta_lopt_%d", idx);
    info_dz[ETA_LOPT].mcFigPath  = figdir + Form("ippv_z_fit/mc_eta_lopt_%d",   idx);

    info_d0[ETA_HIPT].title      = "#splitline{" + etacut_title + "}{" + hipt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[ETA_HIPT].dataFigPath= figdir + Form("ippv_xy_fit/data_eta_hipt_%d", idx);
    info_d0[ETA_HIPT].mcFigPath  = figdir + Form("ippv_xy_fit/mc_eta_hipt_%d",   idx);

    info_dz[ETA_HIPT].title      = "#splitline{" + etacut_title + "}{" + hipt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[ETA_HIPT].dataFigPath= figdir + Form("ippv_z_fit/data_eta_hipt_%d", idx);
    info_dz[ETA_HIPT].mcFigPath  = figdir + Form("ippv_z_fit/mc_eta_hipt_%d",   idx);

    info_d0[ETA_ULPT].title      = "#splitline{" + etacut_title + "}{" + ulpt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[ETA_ULPT].dataFigPath= figdir + Form("ippv_xy_fit/data_eta_ulpt_%d", idx);
    info_d0[ETA_ULPT].mcFigPath  = figdir + Form("ippv_xy_fit/mc_eta_ulpt_%d",   idx);

    info_dz[ETA_ULPT].title      = "#splitline{" + etacut_title + "}{" + ulpt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[ETA_ULPT].dataFigPath= figdir + Form("ippv_z_fit/data_eta_ulpt_%d", idx);
    info_dz[ETA_ULPT].mcFigPath  = figdir + Form("ippv_z_fit/mc_eta_ulpt_%d",   idx);

    info_d0[ETA_ALLPT].title      = "#splitline{" + etacut_title + "}{" + allpt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[ETA_ALLPT].dataFigPath= figdir + Form("ippv_xy_fit/data_eta_allpt_%d", idx);
    info_d0[ETA_ALLPT].mcFigPath  = figdir + Form("ippv_xy_fit/mc_eta_allpt_%d",   idx);

    info_dz[ETA_ALLPT].title      = "#splitline{" + etacut_title + "}{" + allpt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[ETA_ALLPT].dataFigPath= figdir + Form("ippv_z_fit/data_eta_allpt_%d", idx);
    info_dz[ETA_ALLPT].mcFigPath  = figdir + Form("ippv_z_fit/mc_eta_allpt_%d",   idx);

    // phi × pt
    info_d0[PHI_LOPT].title      = "#splitline{" + phicut_title + "}{" + lopt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PHI_LOPT].dataFigPath= figdir + Form("ippv_xy_fit/data_phi_lopt_%d", idx);
    info_d0[PHI_LOPT].mcFigPath  = figdir + Form("ippv_xy_fit/mc_phi_lopt_%d",   idx);

    info_dz[PHI_LOPT].title      = "#splitline{" + phicut_title + "}{" + lopt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PHI_LOPT].dataFigPath= figdir + Form("ippv_z_fit/data_phi_lopt_%d", idx);
    info_dz[PHI_LOPT].mcFigPath  = figdir + Form("ippv_z_fit/mc_phi_lopt_%d",   idx);

    info_d0[PHI_HIPT].title      = "#splitline{" + phicut_title + "}{" + hipt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PHI_HIPT].dataFigPath= figdir + Form("ippv_xy_fit/data_phi_hipt_%d", idx);
    info_d0[PHI_HIPT].mcFigPath  = figdir + Form("ippv_xy_fit/mc_phi_hipt_%d",   idx);

    info_dz[PHI_HIPT].title      = "#splitline{" + phicut_title + "}{" + hipt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PHI_HIPT].dataFigPath= figdir + Form("ippv_z_fit/data_phi_hipt_%d", idx);
    info_dz[PHI_HIPT].mcFigPath  = figdir + Form("ippv_z_fit/mc_phi_hipt_%d",   idx);

    info_d0[PHI_ULPT].title      = "#splitline{" + phicut_title + "}{" + ulpt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PHI_ULPT].dataFigPath= figdir + Form("ippv_xy_fit/data_phi_ulpt_%d", idx);
    info_d0[PHI_ULPT].mcFigPath  = figdir + Form("ippv_xy_fit/mc_phi_ulpt_%d",   idx);

    info_dz[PHI_ULPT].title      = "#splitline{" + phicut_title + "}{" + ulpt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PHI_ULPT].dataFigPath= figdir + Form("ippv_z_fit/data_phi_ulpt_%d", idx);
    info_dz[PHI_ULPT].mcFigPath  = figdir + Form("ippv_z_fit/mc_phi_ulpt_%d",   idx);

    info_d0[PHI_ALLPT].title      = "#splitline{" + phicut_title + "}{" + allpt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PHI_ALLPT].dataFigPath= figdir + Form("ippv_xy_fit/data_phi_allpt_%d", idx);
    info_d0[PHI_ALLPT].mcFigPath  = figdir + Form("ippv_xy_fit/mc_phi_allpt_%d",   idx);

    info_dz[PHI_ALLPT].title      = "#splitline{" + phicut_title + "}{" + allpt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PHI_ALLPT].dataFigPath= figdir + Form("ippv_z_fit/data_phi_allpt_%d", idx);
    info_dz[PHI_ALLPT].mcFigPath  = figdir + Form("ippv_z_fit/mc_phi_allpt_%d",   idx);

    // 初始裁剪范围（照搬你原来的 lowbound/highbound）
    double init_d0_min[NCOMB];
    double init_d0_max[NCOMB];
    double init_dz_min[NCOMB];
    double init_dz_max[NCOMB];

    // pt × eta
    init_d0_min[PT_LOETA] = -2000; init_d0_max[PT_LOETA] = 2000;
    init_dz_min[PT_LOETA] = -2000; init_dz_max[PT_LOETA] = 2000;

    init_d0_min[PT_HIETA] = -4000; init_d0_max[PT_HIETA] = 4000;
    init_dz_min[PT_HIETA] = -10000; init_dz_max[PT_HIETA] = 10000;

    init_d0_min[PT_ULETA] = -6000; init_d0_max[PT_ULETA] = 6000;
    init_dz_min[PT_ULETA] = -10000; init_dz_max[PT_ULETA] = 10000;

    init_d0_min[PT_ALLETA] = -4000; init_d0_max[PT_ALLETA] = 4000;
    init_dz_min[PT_ALLETA] = -10000; init_dz_max[PT_ALLETA] = 10000;

    // eta × pt
    init_d0_min[ETA_LOPT] = -2000;  init_d0_max[ETA_LOPT] = 2000;
    init_dz_min[ETA_LOPT] = -8000; init_dz_max[ETA_LOPT] = 8000;

    init_d0_min[ETA_HIPT] = -1000; init_d0_max[ETA_HIPT] = 1000;
    init_dz_min[ETA_HIPT] = -4000; init_dz_max[ETA_HIPT] = 4000;

    init_d0_min[ETA_ULPT] = -400; init_d0_max[ETA_ULPT] = 400;
    init_dz_min[ETA_ULPT] = -2000; init_dz_max[ETA_ULPT] = 2000;

    init_d0_min[ETA_ALLPT] = -2000; init_d0_max[ETA_ALLPT] = 2000;
    init_dz_min[ETA_ALLPT] = -5000; init_dz_max[ETA_ALLPT] = 5000;

    // phi × pt
    init_d0_min[PHI_LOPT] = -2000; init_d0_max[PHI_LOPT] = 2000;
    init_dz_min[PHI_LOPT] = -3000; init_dz_max[PHI_LOPT] = 3000;

    init_d0_min[PHI_HIPT] = -500; init_d0_max[PHI_HIPT] = 500;
    init_dz_min[PHI_HIPT] = -1500; init_dz_max[PHI_HIPT] = 1500;

    init_d0_min[PHI_ULPT] = -400; init_d0_max[PHI_ULPT] = 400;
    init_dz_min[PHI_ULPT] = -600; init_dz_max[PHI_ULPT] = 600;

    init_d0_min[PHI_ALLPT] = -1500; init_d0_max[PHI_ALLPT] = 1500;
    init_dz_min[PHI_ALLPT] = -2000; init_dz_max[PHI_ALLPT] = 2000;

    // 第 1 遍：在 data 上统计 mean / sigma
    Stat stats_d0_data[NCOMB];
    Stat stats_dz_data[NCOMB];

    auto stat_action_data = [&](int cid, double d0, double dz, double w) {
        if (d0 >= init_d0_min[cid] && d0 <= init_d0_max[cid])
            stats_d0_data[cid].Fill(d0, w);
        if (dz >= init_dz_min[cid] && dz <= init_dz_max[cid])
            stats_dz_data[cid].Fill(dz, w);
    };

    Long64_t nData = datatree->GetEntries();
    process_tree_tracks(datatree, nData, idx,
                        pv_trk_pt_edges, pv_trk_eta_edges, pv_trk_phi_edges,
                        stat_action_data);

    // 根据 data 的 mean / sigma 建 data / MC 直方图（同一范围，以便比较）
    TH1F *h_d0_data[NCOMB] = {nullptr};
    TH1F *h_d0_mc[NCOMB]   = {nullptr};
    TH1F *h_dz_data[NCOMB] = {nullptr};
    TH1F *h_dz_mc[NCOMB]   = {nullptr};


    for (int cid = 0; cid < NCOMB; ++cid)
    {
        if (!stats_d0_data[cid].Valid() || !stats_dz_data[cid].Valid())
            continue;

        double mean_d0  = stats_d0_data[cid].Mean();
        double sigma_d0 = stats_d0_data[cid].Sigma();
        double mean_dz  = stats_dz_data[cid].Mean();
        double sigma_dz = stats_dz_data[cid].Sigma();

        if (sigma_d0 <= 0.0) {
            mean_d0  = 0.5 * (init_d0_min[cid] + init_d0_max[cid]);
            sigma_d0 = (init_d0_max[cid] - init_d0_min[cid]) / (2 * nsigma);
        }
        if (sigma_dz <= 0.0) {
            mean_dz  = 0.5 * (init_dz_min[cid] + init_dz_max[cid]);
            sigma_dz = (init_dz_max[cid] - init_dz_min[cid]) / (2 * nsigma);
        }

        double d0_min = mean_d0 - nsigma * sigma_d0;
        double d0_max = mean_d0 + nsigma * sigma_d0;
        double dz_min = mean_dz - nsigma * sigma_dz;
        double dz_max = mean_dz + nsigma * sigma_dz;

        TString name_d0_data = Form("h_d0_data_%d_%d", idx, cid);
        TString name_d0_mc   = Form("h_d0_mc_%d_%d",   idx, cid);
        TString name_dz_data = Form("h_dz_data_%d_%d", idx, cid);
        TString name_dz_mc   = Form("h_dz_mc_%d_%d",   idx, cid);

        h_d0_data[cid] = new TH1F(name_d0_data, info_d0[cid].title, nbins, d0_min, d0_max);
        h_d0_mc[cid]   = new TH1F(name_d0_mc,   info_d0[cid].title, nbins, d0_min, d0_max);
        h_dz_data[cid] = new TH1F(name_dz_data, info_dz[cid].title, nbins, dz_min, dz_max);
        h_dz_mc[cid]   = new TH1F(name_dz_mc,   info_dz[cid].title, nbins, dz_min, dz_max);

        h_d0_data[cid]->Sumw2();
        h_d0_mc[cid]->Sumw2();
        h_dz_data[cid]->Sumw2();
        h_dz_mc[cid]->Sumw2();
    }

    // 第 2 遍：填 data 直方图
    auto fill_action_data = [&](int cid, double d0, double dz, double w) {
        if (h_d0_data[cid]) h_d0_data[cid]->Fill(d0, w);
        if (h_dz_data[cid]) h_dz_data[cid]->Fill(dz, w);
    };
    process_tree_tracks(datatree, nData, idx,
                        pv_trk_pt_edges, pv_trk_eta_edges, pv_trk_phi_edges,
                        fill_action_data);

    // 第 2 遍：填 MC 直方图（注意：范围仍来自 data）
    Long64_t nMC = mctree->GetEntries();
    auto fill_action_mc = [&](int cid, double d0, double dz, double w) {
        if (h_d0_mc[cid]) h_d0_mc[cid]->Fill(d0, w);
        if (h_dz_mc[cid]) h_dz_mc[cid]->Fill(dz, w);
    };
    process_tree_tracks(mctree, nMC, idx,
                        pv_trk_pt_edges, pv_trk_eta_edges, pv_trk_phi_edges,
                        fill_action_mc);

    // RooFit 拟合并写 JSON
    nlohmann::json resojson;

    resojson["pt"]  = (pv_trk_pt_edges[idx]  + pv_trk_pt_edges[idx + 1])  / 2.0;
    resojson["eta"] = (pv_trk_eta_edges[idx] + pv_trk_eta_edges[idx + 1]) / 2.0;
    resojson["phi"] = (pv_trk_phi_edges[idx] + pv_trk_phi_edges[idx + 1]) / 2.0;

    // 存结果数组
    double reso_data_d0[NCOMB] = {0.0};
    double reso_data_dz[NCOMB] = {0.0};
    double reso_mc_d0[NCOMB]   = {0.0};
    double reso_mc_dz[NCOMB]   = {0.0};

    for (int cid = 0; cid < NCOMB; ++cid)
    {
        if (h_d0_data[cid] && h_d0_data[cid]->GetEntries() > 0)
            reso_data_d0[cid] = fit_res(h_d0_data[cid], period, "Data",
                                        info_d0[cid].dataFigPath, 0.1);
        if (h_dz_data[cid] && h_dz_data[cid]->GetEntries() > 0)
            reso_data_dz[cid] = fit_res(h_dz_data[cid], period, "Data",
                                        info_dz[cid].dataFigPath, 0.1);

        if (h_d0_mc[cid] && h_d0_mc[cid]->GetEntries() > 0)
            reso_mc_d0[cid] = fit_res(h_d0_mc[cid], period, "Simulation",
                                      info_d0[cid].mcFigPath, 0.1);
        if (h_dz_mc[cid] && h_dz_mc[cid]->GetEntries() > 0)
            reso_mc_dz[cid] = fit_res(h_dz_mc[cid], period, "Simulation",
                                      info_dz[cid].mcFigPath, 0.1);

        // 写 JSON key（保持和你原来的命名一致）
        TString suffix = COMB_SUFFIX[cid];

        resojson[Form("reso_data_d0_%s", suffix.Data())] = reso_data_d0[cid];
        resojson[Form("reso_data_dz_%s", suffix.Data())] = reso_data_dz[cid];
        resojson[Form("reso_mc_d0_%s",  suffix.Data())] = reso_mc_d0[cid];
        resojson[Form("reso_mc_dz_%s",  suffix.Data())] = reso_mc_dz[cid];
    }

    // 输出 JSON
    std::ofstream outFile(storage_dir + "/json/" + period + Form("/ip_res/fit_%d.json", idx));
    outFile << resojson.dump(4);
    outFile.close();

    // 清理
    for (int cid = 0; cid < NCOMB; ++cid) {
        delete h_d0_data[cid];
        delete h_d0_mc[cid];
        delete h_dz_data[cid];
        delete h_dz_mc[cid];
    }

    datafile->Close();
    mcfile->Close();
    delete datafile;
    delete mcfile;

    return 0;
}