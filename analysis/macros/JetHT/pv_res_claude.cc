#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include <TFile.h>
#include <TChain.h>
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

const TString datatype_text = "High-q^{2} multi-jet events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";

// 抽样：每 sample_mod 个事件取 1 个；=1 表示不用抽样
const Int_t sample_mod = 1; // 视数据量调整，比如 5 或 10

const Int_t nbins = 200;
const Float_t nsigma = 8.0;
const Float_t sqrt_2 = sqrt(2);

// 组合 ID：和你原来 fill_to_fit 调用一一对应
enum CombId
{
    PV_X = 0,
    PV_Y,
    PV_Z,
    PULL_X,
    PULL_Y,
    PULL_Z,
    NCOMB
};

// 字符串后缀，用于 JSON key 和图像路径
const char *COMB_SUFFIX[NCOMB] = {
    "pvx",
    "pvy",
    "pvz",
    "pullx",
    "pully",
    "pullz"};

// Trigger priority list (highest to lowest)
const int N_TRIGS = 10;
const char *TRIG_NAMES[N_TRIGS] = {
    "HLT_PFHT1050",
    "HLT_PFHT890",
    "HLT_PFHT780",
    "HLT_PFHT680",
    "HLT_PFHT590",
    "HLT_PFHT510",
    "HLT_PFHT430",
    "HLT_PFHT370",
    "HLT_PFHT250",
    "HLT_PFHT180"};

const char *TRIG_BRANCH_NAMES[N_TRIGS] = {
    "trig_PFHT1050_pass",
    "trig_PFHT890_pass",
    "trig_PFHT780_pass",
    "trig_PFHT680_pass",
    "trig_PFHT590_pass",
    "trig_PFHT510_pass",
    "trig_PFHT430_pass",
    "trig_PFHT370_pass",
    "trig_PFHT250_pass",
    "trig_PFHT180_pass"};

// 简单统计结构：加权 mean / sigma
struct Stat
{
    Float_t sw = 0.0;   // sum w
    Float_t swx = 0.0;  // sum w*x
    Float_t swx2 = 0.0; // sum w*x^2

    void Fill(Float_t x, Float_t w)
    {
        sw += w;
        swx += w * x;
        swx2 += w * x * x;
    }

    Bool_t Valid() const { return sw > 0.0; }

    Float_t Mean() const
    {
        return sw > 0.0 ? swx / sw : 0.0;
    }

    Float_t Sigma() const
    {
        if (sw <= 0.0)
            return 0.0;
        Float_t m = swx / sw;
        Float_t v = swx2 / sw - m * m;
        return v > 0.0 ? std::sqrt(v) : 0.0;
    }
};

// 单个直方图配置：标题和图像路径
struct HistInfo
{
    TString title;
    TString dataFigPath;
    TString mcFigPath;
};

// 三高斯拟合 + 68% 区间解分辨率（基本保留你的实现）
Float_t fit_res(TH1F *hist, TString period, TString sampletype, TString figpath, Float_t tolerance = 1e-4)
{
    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, " + period;

    RooRealVar pv_var("pv_var", "pv_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    pv_var.setBins(hist->GetNbinsX());

    Float_t hist_mean = hist->GetMean();
    Float_t hist_rms = hist->GetRMS();

    // 均值
    RooRealVar mu("mu", "mu", hist_mean, hist_mean - hist_rms, hist_mean + hist_rms);

    // 核心宽度
    RooRealVar sigma("sigma", "sigma", 0.5 * hist_rms, 0.1 * hist_rms, hist_rms);

    // 对称的尾部参数
    RooRealVar alpha("alpha", "alpha", 2.0, 0.5, 5.0); // 尾部切换点
    RooRealVar n("n", "n", 2.0, 0.5, 10.0);            // 尾部幂律指数

    // 创建对称的Double Crystal Ball
    RooCrystalBall model("model", "Double Crystal Ball", pv_var, mu, sigma, alpha, n, alpha, n); // 左右使用相同的alpha和n保证对称

    RooDataHist hdatahist("hdatahist", "", pv_var, hist);
    RooFitResult *fitResult = model.fitTo(hdatahist, RooFit::Save(true));
    fitResult->Print();
    delete fitResult;

    Float_t pv_var_max = pv_var.getMax();
    Float_t mean = mu.getVal();
    Float_t low = 0.0;
    Float_t high = pv_var_max - mean;

    pv_var.setRange("normRange", mean - 1e6, mean + 1e6);
    RooAbsReal *denom = model.createIntegral(pv_var, RooFit::Range("normRange"));
    while (high - low > tolerance)
    {
        Float_t mid = 0.5 * (low + high);
        pv_var.setRange("intRange", mean - mid, mean + mid);
        RooAbsReal *num = model.createIntegral(pv_var, RooFit::Range("intRange"));
        Float_t prob = num->getVal() / denom->getVal();
        if (prob < 0.68)
            low = mid;
        else
            high = mid;
        delete num;
    }
    delete denom;
    Float_t reso = 0.5 * (low + high);

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas_setup(canvas);
    canvas->SetBottomMargin(0.15);
    canvas->SetRightMargin(0.05);
    canvas->SetLogy(0);
    canvas->SetFillColor(0);
    canvas->SetFrameFillColor(0);

    RooPlot *frame = pv_var.frame();

    hdatahist.plotOn(frame,
                     RooFit::Name(sampletype),
                     RooFit::MarkerColor(kBlack),
                     RooFit::MarkerSize(1.1),
                     RooFit::Binning(hist->GetNbinsX()),
                     RooFit::DrawOption("ep"));
    model.plotOn(frame,
                 RooFit::Name("model"),
                 RooFit::Components("model"),
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
    write_text(0.6, 0.8, hist->GetTitle());
    write_text(0.6, 0.7, sampletype + " fit results:");
    write_text(0.6, 0.65, Form("reso = %.*f", std::max(0, 2 - (Int_t)floor(log10(reso))), reso));
    CMS_lumi(canvas);

    canvas->Update();
    canvas->SaveAs(figpath + ".png");
    delete canvas;

    return reso;
}

// For data: trig_pass and ps_weights are unused (isData=true)
// For MC:   w already includes PU*xsec; ps_weights[i] is per-trigger PS weight (0 if masked)
//           trig_pass[i] is the exclusive trigger decision (priority-resolved)
template <typename F>
void process_tree_tracks(TChain *tree,
                         Bool_t isData,
                         Long64_t nEntries,
                         Int_t idx,
                         const std::vector<Float_t> &pv_SumTrackPt2_sqrt_edges,
                         const std::vector<Float_t> &PU_weights,
                         Float_t &pv_SumTrackPt2,
                         Float_t &pv_x_p1,
                         Float_t &pv_y_p1,
                         Float_t &pv_z_p1,
                         Float_t &pv_x_p2,
                         Float_t &pv_y_p2,
                         Float_t &pv_z_p2,
                         Float_t &pv_xError_p1,
                         Float_t &pv_yError_p1,
                         Float_t &pv_zError_p1,
                         Float_t &pv_xError_p2,
                         Float_t &pv_yError_p2,
                         Float_t &pv_zError_p2,
                         Int_t &NumTrueInts,
                         Bool_t *trig_pass,        // [N_TRIGS], only used for MC
                         const double *ps_weights, // [N_TRIGS], PS weight per trigger (0 if masked), only used for MC
                         Float_t xsec_weight,      // 1.0 for data
                         F &&action)
{

    // 预先算好与 idx 相关的一些区间
    const Float_t pv_SumTrackPt2_sqrt_lo = pv_SumTrackPt2_sqrt_edges[idx];
    const Float_t pv_SumTrackPt2_sqrt_hi = pv_SumTrackPt2_sqrt_edges[idx + 1];

    for (Long64_t ie = 0; ie < nEntries; ++ie)
    {
        // if (sample_mod > 1 && (ie % sample_mod) != 0) continue;

        tree->GetEntry(ie);

        Float_t w = 1.0;
        if (!isData)
        {
            // PU weight
            Float_t w_pu = 1.0;
            if ((NumTrueInts < 1) || (NumTrueInts > (Int_t)PU_weights.size()))
                w_pu = 0.0;
            else
                w_pu = PU_weights[NumTrueInts - 1];

            // PS weight: find highest-priority unmasked trigger this event passes
            // trig_pass[i] is already exclusive (priority-resolved by caller)
            Float_t w_ps = 0.0;
            for (int it = 0; it < N_TRIGS; ++it)
            {
                if (trig_pass[it])
                {
                    w_ps = ps_weights[it]; // 0 if masked
                    break;
                }
            }

            w = w_pu * xsec_weight * w_ps;
        }

        Float_t pv_SumTrackPt2_sqrt = std::sqrt(pv_SumTrackPt2);
        Bool_t in_pv_SumTrackPt2_sqrtbin = (pv_SumTrackPt2_sqrt > pv_SumTrackPt2_sqrt_lo && pv_SumTrackPt2_sqrt < pv_SumTrackPt2_sqrt_hi);
        Bool_t pvx_nonull = (pv_x_p1 != -777 && pv_x_p2 != -777 && pv_xError_p1 != -777 && pv_xError_p2 != -777);
        Bool_t pvy_nonull = (pv_y_p1 != -777 && pv_y_p2 != -777 && pv_yError_p1 != -777 && pv_yError_p2 != -777);
        Bool_t pvz_nonull = (pv_z_p1 != -777 && pv_z_p2 != -777 && pv_zError_p1 != -777 && pv_zError_p2 != -777);

        if (in_pv_SumTrackPt2_sqrtbin && pvx_nonull)
        {
            action(PV_X, (pv_x_p1 - pv_x_p2) / sqrt_2, w);
            action(PULL_X, (pv_x_p1 - pv_x_p2) / sqrt(pv_xError_p1 * pv_xError_p1 + pv_xError_p2 * pv_xError_p2), w);
        }
        if (in_pv_SumTrackPt2_sqrtbin && pvy_nonull)
        {
            action(PV_Y, (pv_y_p1 - pv_y_p2) / sqrt_2, w);
            action(PULL_Y, (pv_y_p1 - pv_y_p2) / sqrt(pv_yError_p1 * pv_yError_p1 + pv_yError_p2 * pv_yError_p2), w);
        }
        if (in_pv_SumTrackPt2_sqrtbin && pvz_nonull)
        {
            action(PV_Z, (pv_z_p1 - pv_z_p2) / sqrt_2, w);
            action(PULL_Z, (pv_z_p1 - pv_z_p2) / sqrt(pv_zError_p1 * pv_zError_p1 + pv_zError_p2 * pv_zError_p2), w);
        }
    }
}

Int_t pv_res_claude(TString period, Int_t idx)
{
    // 输出目录
    TString figdir = storage_dir + "/figures/" + period + "/pv_res/";

    // 打开 data / MC 文件
    std::ifstream tuplelist_file("/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/JetHT/tuplelist.json");
    nlohmann::json tuplelist;
    tuplelist_file >> tuplelist;
    tuplelist_file.close();

    // ---------- Read PS weight JSON ----------
    std::ifstream ps_file("/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/JetHT/triggerPS.json");
    nlohmann::json ps_json;
    ps_file >> ps_json;
    ps_file.close();

    // L_eff per trigger for this period
    double L_eff[N_TRIGS];
    for (int it = 0; it < N_TRIGS; ++it)
        L_eff[it] = ps_json[period.Data()][TRIG_NAMES[it]].get<double>();

    // ---------- Build data TChain ----------
    TChain *datatree = new TChain("residuals/tree");
    for (auto &entry : tuplelist[period.Data()]["data"])
    {
        std::string path = entry["path"];
        TString datapath = TString(path) + "/*.root";
        datatree->Add(datapath);
    }

    Float_t pv_SumTrackPt2;
    Float_t pv_x_p1;
    Float_t pv_y_p1;
    Float_t pv_z_p1;
    Float_t pv_x_p2;
    Float_t pv_y_p2;
    Float_t pv_z_p2;
    Float_t pv_xError_p1;
    Float_t pv_yError_p1;
    Float_t pv_zError_p1;
    Float_t pv_xError_p2;
    Float_t pv_yError_p2;
    Float_t pv_zError_p2;
    Int_t NumTrueInts;
    Bool_t trig_pass_raw[N_TRIGS];  // raw (inclusive) trigger decisions from branch
    Bool_t trig_pass_excl[N_TRIGS]; // exclusive (priority-resolved), computed per event

    auto bind_common_branches = [&](TChain *tree)
    {
        tree->SetBranchAddress("pv_SumTrackPt2", &pv_SumTrackPt2);
        tree->SetBranchAddress("pv_x_p1", &pv_x_p1);
        tree->SetBranchAddress("pv_y_p1", &pv_y_p1);
        tree->SetBranchAddress("pv_z_p1", &pv_z_p1);
        tree->SetBranchAddress("pv_x_p2", &pv_x_p2);
        tree->SetBranchAddress("pv_y_p2", &pv_y_p2);
        tree->SetBranchAddress("pv_z_p2", &pv_z_p2);
        tree->SetBranchAddress("pv_xError_p1", &pv_xError_p1);
        tree->SetBranchAddress("pv_yError_p1", &pv_yError_p1);
        tree->SetBranchAddress("pv_zError_p1", &pv_zError_p1);
        tree->SetBranchAddress("pv_xError_p2", &pv_xError_p2);
        tree->SetBranchAddress("pv_yError_p2", &pv_yError_p2);
        tree->SetBranchAddress("pv_zError_p2", &pv_zError_p2);
    };

    auto bind_trig_branches = [&](TChain *tree)
    {
        for (int it = 0; it < N_TRIGS; ++it)
            tree->SetBranchAddress(TRIG_BRANCH_NAMES[it], &trig_pass_raw[it]);
    };

    bind_common_branches(datatree);
    bind_trig_branches(datatree);

    // ---------- Read binning & PU weights ----------
    std::ifstream infile(storage_dir + "/json/binning.json");
    nlohmann::json binning;
    infile >> binning;
    infile.close();

    std::vector<Float_t> pv_SumTrackPt2_sqrt_edges = binning["pv_SumTrackPt2_sqrt"].get<std::vector<Float_t>>();

    std::ifstream pileup_weightfile(storage_dir + "/pileup/" + period + "/pileup_ratio.json");
    nlohmann::json pu_weights;
    pileup_weightfile >> pu_weights;
    pileup_weightfile.close();

    std::vector<Float_t> PU_weights;
    PU_weights.resize(pu_weights.size());
    for (const auto &item : pu_weights)
    {
        Int_t bin = item["bin"];
        PU_weights[bin - 1] = item["content"];
    }

    // ---------- Count exclusive trigger pass events in data (for PS weight) ----------
    // n_data_excl[it] = #events where trigger it is the highest-priority pass
    Long64_t n_data_excl[N_TRIGS] = {0};
    Long64_t nData = datatree->GetEntries();

    for (Long64_t ie = 0; ie < nData; ++ie)
    {
        datatree->GetEntry(ie);
        for (int it = 0; it < N_TRIGS; ++it)
        {
            if (trig_pass_raw[it])
            {
                n_data_excl[it]++;
                break; // exclusive: stop at highest-priority pass
            }
        }
    }

    // ---------- Compute PS weights ----------
    double ps_weights_final[N_TRIGS];
    for (int it = 0; it < N_TRIGS; ++it)
    {
        if (n_data_excl[it] > 0)
            ps_weights_final[it] = L_eff[it] / static_cast<double>(n_data_excl[it]);
        else
            ps_weights_final[it] = 0.0;
    }

    // ---------- Histograms ----------
    TString ptcut_title = Form("%.2f<#sqrt{#sum#it{p_{T}}^{2}}<%.2f GeV", pv_SumTrackPt2_sqrt_edges[idx], pv_SumTrackPt2_sqrt_edges[idx + 1]);

    // 准备 HistInfo（标题 & 图像路径）—— d0
    HistInfo histinfo[NCOMB];
    histinfo[PV_X].title = ptcut_title + ";(#it{x}_{1}-#it{x}_{2})/#sqrt{2} [#mum];# PV";
    histinfo[PV_X].dataFigPath = figdir + Form("pvx_fit/data_pt_%d", idx);
    histinfo[PV_X].mcFigPath = figdir + Form("pvx_fit/mc_pt_%d", idx);

    histinfo[PV_Y].title = ptcut_title + ";(#it{y}_{1}-#it{y}_{2})/#sqrt{2} [#mum];# PV";
    histinfo[PV_Y].dataFigPath = figdir + Form("pvy_fit/data_pt_%d", idx);
    histinfo[PV_Y].mcFigPath = figdir + Form("pvy_fit/mc_pt_%d", idx);

    histinfo[PV_Z].title = ptcut_title + ";(#it{z}_{1}-#it{z}_{2})/#sqrt{2} [#mum];# PV";
    histinfo[PV_Z].dataFigPath = figdir + Form("pvz_fit/data_pt_%d", idx);
    histinfo[PV_Z].mcFigPath = figdir + Form("pvz_fit/mc_pt_%d", idx);

    histinfo[PULL_X].title = ptcut_title + ";(#it{x}_{1}-#it{x}_{2})/#sqrt{#Delta#it{x}_{1}^{2}+#Delta#it{x}_{2}^{2}};# PV";
    histinfo[PULL_X].dataFigPath = figdir + Form("pullx_fit/data_pt_%d", idx);
    histinfo[PULL_X].mcFigPath = figdir + Form("pullx_fit/mc_pt_%d", idx);

    histinfo[PULL_Y].title = ptcut_title + ";(#it{y}_{1}-#it{y}_{2})/#sqrt{#Delta#it{y}_{1}^{2}+#Delta#it{y}_{2}^{2}};# PV";
    histinfo[PULL_Y].dataFigPath = figdir + Form("pully_fit/data_pt_%d", idx);
    histinfo[PULL_Y].mcFigPath = figdir + Form("pully_fit/mc_pt_%d", idx);

    histinfo[PULL_Z].title = ptcut_title + ";(#it{z}_{1}-#it{z}_{2})/#sqrt{#Delta#it{z}_{1}^{2}+#Delta#it{z}_{2}^{2}};# PV";
    histinfo[PULL_Z].dataFigPath = figdir + Form("pullz_fit/data_pt_%d", idx);
    histinfo[PULL_Z].mcFigPath = figdir + Form("pullz_fit/mc_pt_%d", idx);

    Float_t init_min[NCOMB] = {-300, -300, -300, -10, -10, -10};
    Float_t init_max[NCOMB] = {300, 300, 300, 10, 10, 10};

    // 第 1 遍：在 data 上统计 mean / sigma
    Stat stats_data[NCOMB];

    auto stat_action_data = [&](Int_t cid, Float_t var, Float_t w)
    {
        if (var >= init_min[cid] && var <= init_max[cid])
            stats_data[cid].Fill(var, w);
    };

    // Dummy arrays for data (not used inside process_tree_tracks when isData=true)
    double ps_dummy[N_TRIGS] = {0};

    process_tree_tracks(datatree,
                        true,
                        nData,
                        idx,
                        pv_SumTrackPt2_sqrt_edges,
                        PU_weights,
                        pv_SumTrackPt2,
                        pv_x_p1,
                        pv_y_p1,
                        pv_z_p1,
                        pv_x_p2,
                        pv_y_p2,
                        pv_z_p2,
                        pv_xError_p1,
                        pv_yError_p1,
                        pv_zError_p1,
                        pv_xError_p2,
                        pv_yError_p2,
                        pv_zError_p2,
                        NumTrueInts,
                        trig_pass_excl,
                        ps_dummy,
                        1.0,
                        stat_action_data);

    // 根据 data 的 mean / sigma 建 data / MC 直方图（同一范围，以便比较）
    TH1F *h_data[NCOMB] = {nullptr};
    TH1F *h_mc[NCOMB] = {nullptr};

    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        if (!stats_data[cid].Valid())
            continue;

        Float_t mean = stats_data[cid].Mean();
        Float_t sigma = stats_data[cid].Sigma();

        if (sigma <= 0.0)
        {
            mean = 0.5 * (init_min[cid] + init_max[cid]);
            sigma = (init_max[cid] - init_min[cid]) / (2 * nsigma);
        }

        Float_t varmin = mean - nsigma * sigma;
        Float_t varmax = mean + nsigma * sigma;

        TString name_data = Form("h_data_%d_%d", idx, cid);
        TString name_mc = Form("h_mc_%d_%d", idx, cid);

        h_data[cid] = new TH1F(name_data, histinfo[cid].title, nbins, varmin, varmax);
        h_mc[cid] = new TH1F(name_mc, histinfo[cid].title, nbins, varmin, varmax);

        h_data[cid]->Sumw2();
        h_mc[cid]->Sumw2();
    }

    // 第 2 遍：填 data 直方图
    auto fill_action_data = [&](Int_t cid, Float_t var, Float_t w)
    {
        if (h_data[cid])
            h_data[cid]->Fill(var, w);
    };
    process_tree_tracks(datatree,
                        true,
                        nData,
                        idx,
                        pv_SumTrackPt2_sqrt_edges,
                        PU_weights,
                        pv_SumTrackPt2,
                        pv_x_p1,
                        pv_y_p1,
                        pv_z_p1,
                        pv_x_p2,
                        pv_y_p2,
                        pv_z_p2,
                        pv_xError_p1,
                        pv_yError_p1,
                        pv_zError_p1,
                        pv_xError_p2,
                        pv_yError_p2,
                        pv_zError_p2,
                        NumTrueInts,
                        trig_pass_excl,
                        ps_dummy,
                        1.0,
                        fill_action_data);

    // ---------- MC: process per entry ----------
    for (auto &mc_entry : tuplelist[period.Data()]["mc"])
    {
        std::string path = mc_entry["path"];

        double xsec = mc_entry["xsec"].get<double>();

        // --- Sum nEventsProcessed_ from RunInfo tree ---
        TChain *runinfo = new TChain("residuals/RunInfo"); // adjust tree path if needed
        runinfo->Add(TString(path) + "/*.root");
        ULong_t nEventsProcessed_branch;
        runinfo->SetBranchAddress("nEventsProcessed_", &nEventsProcessed_branch);
        Long64_t nRunEntries = runinfo->GetEntries();
        ULong_t total_nEventsProcessed = 0;
        for (Long64_t ir = 0; ir < nRunEntries; ++ir)
        {
            runinfo->GetEntry(ir);
            total_nEventsProcessed += nEventsProcessed_branch;
        }
        delete runinfo;

        if (total_nEventsProcessed == 0)
            continue;
        Float_t xsec_weight = static_cast<Float_t>(xsec / static_cast<double>(total_nEventsProcessed));

        // --- Build MC event tree for this entry ---
        TChain *mctree_entry = new TChain("residuals/tree");
        mctree_entry->Add(TString(path) + "/*.root");

        bind_common_branches(mctree_entry);
        mctree_entry->SetBranchAddress("NumTrueInts", &NumTrueInts);
        bind_trig_branches(mctree_entry);

        Long64_t nMC_entry = mctree_entry->GetEntries();

        // --- Count exclusive trigger pass events in this MC entry (for mask) ---
        Long64_t n_mc_excl[N_TRIGS] = {0};
        for (Long64_t ie = 0; ie < nMC_entry; ++ie)
        {
            mctree_entry->GetEntry(ie);
            for (int it = 0; it < N_TRIGS; ++it)
            {
                if (trig_pass_raw[it])
                {
                    n_mc_excl[it]++;
                    break;
                }
            }
        }

        // --- Compute per-entry PS weights, applying mask ---
        // mask: n_mc_excl[it] < 0.01 * total_nEventsProcessed
        double ps_weights_entry[N_TRIGS];
        for (int it = 0; it < N_TRIGS; ++it)
        {
            if (n_mc_excl[it] < 0.01 * static_cast<double>(total_nEventsProcessed))
                ps_weights_entry[it] = 0.0; // masked
            else
                ps_weights_entry[it] = ps_weights_final[it];
        }

        // --- Fill MC histograms ---
        // Compute exclusive trigger decisions per event inside the action lambda.
        // We pass trig_pass_excl[] which is filled just before calling process_tree_tracks
        // by overriding the action to first resolve exclusivity.
        // Actually: exclusivity is resolved inside process_tree_tracks via trig_pass_raw,
        // but process_tree_tracks only sees trig_pass (already exclusive).
        // Solution: resolve exclusivity in a wrapper lambda before calling action.

        auto fill_action_mc = [&](Int_t cid, Float_t var, Float_t w)
        {
            if (h_mc[cid])
                h_mc[cid]->Fill(var, w);
        };

        // We need to resolve exclusivity per event. Do it in a custom loop here
        // rather than inside process_tree_tracks, to keep the function generic.
        {
            const Float_t lo = pv_SumTrackPt2_sqrt_edges[idx];
            const Float_t hi = pv_SumTrackPt2_sqrt_edges[idx + 1];

            for (Long64_t ie = 0; ie < nMC_entry; ++ie)
            {
                mctree_entry->GetEntry(ie);

                // Resolve exclusive trigger
                Float_t w_ps = 0.0;
                for (int it = 0; it < N_TRIGS; ++it)
                {
                    if (trig_pass_raw[it])
                    {
                        w_ps = ps_weights_entry[it]; // 0 if masked
                        break;
                    }
                }

                // PU weight
                Float_t w_pu = 0.0;
                if (NumTrueInts >= 1 && NumTrueInts <= (Int_t)PU_weights.size())
                    w_pu = PU_weights[NumTrueInts - 1];

                Float_t w = w_pu * xsec_weight * w_ps;

                Float_t pv_SumTrackPt2_sqrt = std::sqrt(pv_SumTrackPt2);
                Bool_t in_bin = (pv_SumTrackPt2_sqrt > lo && pv_SumTrackPt2_sqrt < hi);

                Bool_t pvx_nonull = (pv_x_p1 != -777 && pv_x_p2 != -777 && pv_xError_p1 != -777 && pv_xError_p2 != -777);
                Bool_t pvy_nonull = (pv_y_p1 != -777 && pv_y_p2 != -777 && pv_yError_p1 != -777 && pv_yError_p2 != -777);
                Bool_t pvz_nonull = (pv_z_p1 != -777 && pv_z_p2 != -777 && pv_zError_p1 != -777 && pv_zError_p2 != -777);

                if (in_bin && pvx_nonull)
                {
                    fill_action_mc(PV_X, (pv_x_p1 - pv_x_p2) / sqrt_2, w);
                    fill_action_mc(PULL_X, (pv_x_p1 - pv_x_p2) / sqrt(pv_xError_p1 * pv_xError_p1 + pv_xError_p2 * pv_xError_p2), w);
                }
                if (in_bin && pvy_nonull)
                {
                    fill_action_mc(PV_Y, (pv_y_p1 - pv_y_p2) / sqrt_2, w);
                    fill_action_mc(PULL_Y, (pv_y_p1 - pv_y_p2) / sqrt(pv_yError_p1 * pv_yError_p1 + pv_yError_p2 * pv_yError_p2), w);
                }
                if (in_bin && pvz_nonull)
                {
                    fill_action_mc(PV_Z, (pv_z_p1 - pv_z_p2) / sqrt_2, w);
                    fill_action_mc(PULL_Z, (pv_z_p1 - pv_z_p2) / sqrt(pv_zError_p1 * pv_zError_p1 + pv_zError_p2 * pv_zError_p2), w);
                }
            }
        }

        delete mctree_entry;
    }

    // ---------- Fit & write JSON ----------
    nlohmann::json resojson;

    resojson["pv_SumTrackPt2_sqrt"] = (pv_SumTrackPt2_sqrt_edges[idx] + pv_SumTrackPt2_sqrt_edges[idx + 1]) / 2.0;

    // 存结果数组
    Float_t reso_data[NCOMB] = {0.0};
    Float_t reso_mc[NCOMB] = {0.0};

    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        if (h_data[cid] && h_data[cid]->GetEntries() > 0)
            reso_data[cid] = fit_res(h_data[cid], period, "Data", histinfo[cid].dataFigPath, 0.1);
        if (h_mc[cid] && h_mc[cid]->GetEntries() > 0)
            reso_mc[cid] = fit_res(h_mc[cid], period, "Simulation", histinfo[cid].mcFigPath, 0.1);

        // 写 JSON key（保持和你原来的命名一致）
        TString suffix = COMB_SUFFIX[cid];

        resojson[Form("reso_data_%s", suffix.Data())] = reso_data[cid];
        resojson[Form("reso_mc_%s", suffix.Data())] = reso_mc[cid];
    }

    // 输出 JSON
    std::ofstream outFile(storage_dir + "/json/" + period + Form("/pv_res/fit_%d.json", idx));
    outFile << resojson.dump(4);
    outFile.close();

    // 清理
    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        delete h_data[cid];
        delete h_mc[cid];
    }
    delete datatree;

    return 0;
}
