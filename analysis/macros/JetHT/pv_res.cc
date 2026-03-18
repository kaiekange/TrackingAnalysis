#include <vector>
#include <map>
#include <string>
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

const TString datatype_text = "High-#it{q}^{2} multi-jet events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";

const Int_t sample_mod = 1;

const Int_t nbins = 200;
const Float_t nsigma = 8.0;
const Float_t sqrt_2 = sqrt(2);

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

const char *COMB_SUFFIX[NCOMB] = {
    "pvx",
    "pvy",
    "pvz",
    "pullx",
    "pully",
    "pullz"};

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

struct Stat
{
    Int_t n = 0;
    Float_t sum = 0.0;
    Float_t sum2 = 0.0;

    void Fill(Float_t x)
    {
        n++;
        sum += x;
        sum2 += x * x;
    }

    Bool_t Valid() const { return n > 0; }

    Float_t Mean() const
    {
        return n > 0 ? sum / Float_t(n) : 0.0;
    }

    Float_t Sigma() const
    {
        if (n <= 0)
            return 0.0;
        Float_t m = sum / Float_t(n);
        Float_t v = sum2 / Float_t(n) - m * m;
        return v > 0.0 ? std::sqrt(v) : 0.0;
    }
};

struct HistInfo
{
    TString title;
    TString dataFigPath;
    TString mcFigPath;
};

Float_t fit_res(TH1F *hist, TString period, TString sampletype, TString figpath, Float_t tolerance = 1e-4)
{
    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, " + period;

    RooRealVar pv_var("pv_var", "pv_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    pv_var.setBins(hist->GetNbinsX());

    Float_t hist_mean = hist->GetMean();
    Float_t hist_rms = hist->GetRMS();

    RooRealVar mu("mu", "mu", hist_mean, hist_mean - hist_rms, hist_mean + hist_rms);
    RooRealVar sigma("sigma", "sigma", 0.5 * hist_rms, 0.1 * hist_rms, hist_rms);
    RooRealVar alpha("alpha", "alpha", 2.0, 0.5, 5.0);
    RooRealVar n("n", "n", 2.0, 0.5, 10.0);
    RooCrystalBall model("model", "Double Crystal Ball", pv_var, mu, sigma, alpha, n, alpha, n);

    RooDataHist hdatahist("hdatahist", "", pv_var, hist);
    RooFitResult *fitResult = model.fitTo(hdatahist, RooFit::Save(true));
    fitResult->Print();
    delete fitResult;

    Float_t pv_var_max = pv_var.getMax();
    Float_t mean = mu.getVal();
    Float_t low = 0.0;
    Float_t high = pv_var_max - mean;

    pv_var.setRange("normRange", mean - 1e6, mean + 1e6);
    // pv_var.setRange("normRange", mean - 8 * hist_rms, mean + 8 * hist_rms);
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

template <typename F>
void process_tree_tracks(TChain *tree,
                         Long64_t nEntries,
                         Int_t idx,
                         const std::vector<Float_t> &pv_SumTrackPt2_sqrt_edges,
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
                         F &&action)
{
    const Float_t lo = pv_SumTrackPt2_sqrt_edges[idx];
    const Float_t hi = pv_SumTrackPt2_sqrt_edges[idx + 1];

    for (Long64_t ie = 0; ie < nEntries; ++ie)
    {
        tree->GetEntry(ie);

        Float_t pv_SumTrackPt2_sqrt = std::sqrt(pv_SumTrackPt2);
        Bool_t in_bin = (pv_SumTrackPt2_sqrt > lo && pv_SumTrackPt2_sqrt < hi);
        Bool_t pvx_nonull = (pv_x_p1 != -777 && pv_x_p2 != -777 && pv_xError_p1 != -777 && pv_xError_p2 != -777);
        Bool_t pvy_nonull = (pv_y_p1 != -777 && pv_y_p2 != -777 && pv_yError_p1 != -777 && pv_yError_p2 != -777);
        Bool_t pvz_nonull = (pv_z_p1 != -777 && pv_z_p2 != -777 && pv_zError_p1 != -777 && pv_zError_p2 != -777);

        if (in_bin && pvx_nonull)
        {
            action(PV_X, (pv_x_p1 - pv_x_p2) / sqrt_2);
            action(PULL_X, (pv_x_p1 - pv_x_p2) / sqrt(pv_xError_p1 * pv_xError_p1 + pv_xError_p2 * pv_xError_p2));
        }
        if (in_bin && pvy_nonull)
        {
            action(PV_Y, (pv_y_p1 - pv_y_p2) / sqrt_2);
            action(PULL_Y, (pv_y_p1 - pv_y_p2) / sqrt(pv_yError_p1 * pv_yError_p1 + pv_yError_p2 * pv_yError_p2));
        }
        if (in_bin && pvz_nonull)
        {
            action(PV_Z, (pv_z_p1 - pv_z_p2) / sqrt_2);
            action(PULL_Z, (pv_z_p1 - pv_z_p2) / sqrt(pv_zError_p1 * pv_zError_p1 + pv_zError_p2 * pv_zError_p2));
        }
    }
}

Int_t pv_res(TString period, Int_t idx)
{
    TString figdir = storage_dir + "/figures/" + period + "/pv_res/";

    // ---------- Read JSONs ----------
    std::ifstream tuplelist_file("/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/JetHT/tuplelist.json");
    nlohmann::json tuplelist;
    tuplelist_file >> tuplelist;
    tuplelist_file.close();

    std::ifstream infile(storage_dir + "/json/binning.json");
    // std::ifstream infile(storage_dir + "/json/binning_mc.json");
    nlohmann::json binning;
    infile >> binning;
    infile.close();

    std::ifstream pileup_weightfile(storage_dir + "/pileup/" + period + "/pileup_ratio.json");
    nlohmann::json pu_weights_json;
    pileup_weightfile >> pu_weights_json;
    pileup_weightfile.close();

    std::ifstream xsec_file(storage_dir + "/pileup/" + period + "/xsec_weight.json");
    nlohmann::json xsec_json;
    xsec_file >> xsec_json;
    xsec_file.close();

    std::ifstream ps_file(storage_dir + "/pileup/" + period + "/PS_weight.json");
    nlohmann::json ps_json;
    ps_file >> ps_json;
    ps_file.close();

    std::ifstream mask_file(storage_dir + "/pileup/" + period + "/trigger_mask.json");
    nlohmann::json mask_json;
    mask_file >> mask_json;
    mask_file.close();

    // ---------- Build lookup tables ----------
    std::vector<Float_t> pv_SumTrackPt2_sqrt_edges = binning["pv_SumTrackPt2_sqrt"].get<std::vector<Float_t>>();

    std::vector<Float_t> PU_weights(pu_weights_json.size());
    for (const auto &item : pu_weights_json)
    {
        Int_t bin = item["bin"];
        PU_weights[bin - 1] = item["content"].get<Float_t>();
    }

    std::map<std::string, Float_t> xsec_weight_map;
    for (const auto &item : xsec_json)
        xsec_weight_map[item["dataset"].get<std::string>()] = item["xsec_weight"].get<Float_t>();

    Float_t ps_weights_final[N_TRIGS];
    for (Int_t it = 0; it < N_TRIGS; ++it)
        ps_weights_final[it] = ps_json[it]["PS_weight"].get<Float_t>();

    // ---------- Branch variables ----------
    Float_t pv_SumTrackPt2;
    Float_t pv_x_p1, pv_y_p1, pv_z_p1;
    Float_t pv_x_p2, pv_y_p2, pv_z_p2;
    Float_t pv_xError_p1, pv_yError_p1, pv_zError_p1;
    Float_t pv_xError_p2, pv_yError_p2, pv_zError_p2;
    Int_t NumTrueInts;
    Bool_t trig_pass_raw[N_TRIGS];

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

    // ---------- Build data TChain ----------
    TChain *datatree = new TChain("residuals/tree");
    for (auto &entry : tuplelist[period.Data()]["data"])
    {
        std::string path = entry["path"];
        if (!path.empty())
            datatree->Add(TString(path) + "/*.root");
    }
    bind_common_branches(datatree);
    Long64_t nData = datatree->GetEntries();

    // ---------- Histogram info ----------
    TString ptcut_title = Form("%.2f<#sqrt{#sum#it{p_{T}}^{2}}<%.2f GeV", pv_SumTrackPt2_sqrt_edges[idx], pv_SumTrackPt2_sqrt_edges[idx + 1]);

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

    // ---------- First pass on data: compute histogram ranges ----------
    Stat stats_data[NCOMB];
    auto stat_action_data = [&](Int_t cid, Float_t var)
    {
        if (var >= init_min[cid] && var <= init_max[cid])
            stats_data[cid].Fill(var);
    };

    process_tree_tracks(datatree, nData, idx,
                        pv_SumTrackPt2_sqrt_edges,
                        pv_SumTrackPt2,
                        pv_x_p1, pv_y_p1, pv_z_p1,
                        pv_x_p2, pv_y_p2, pv_z_p2,
                        pv_xError_p1, pv_yError_p1, pv_zError_p1,
                        pv_xError_p2, pv_yError_p2, pv_zError_p2,
                        stat_action_data);

    // ---------- Build histograms ----------
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
            mean = 0.5f * (init_min[cid] + init_max[cid]);
            sigma = (init_max[cid] - init_min[cid]) / (2.f * nsigma);
        }
        Float_t varmin = mean - nsigma * sigma;
        Float_t varmax = mean + nsigma * sigma;
        h_data[cid] = new TH1F(Form("h_data_%d_%d", idx, cid), histinfo[cid].title, nbins, varmin, varmax);
        h_mc[cid] = new TH1F(Form("h_mc_%d_%d", idx, cid), histinfo[cid].title, nbins, varmin, varmax);
        h_data[cid]->Sumw2();
        h_mc[cid]->Sumw2();
    }

    // ---------- Second pass on data: fill ----------
    auto fill_action_data = [&](Int_t cid, Float_t var)
    {
        if (h_data[cid])
            h_data[cid]->Fill(var);
    };
    process_tree_tracks(datatree, nData, idx,
                        pv_SumTrackPt2_sqrt_edges,
                        pv_SumTrackPt2,
                        pv_x_p1, pv_y_p1, pv_z_p1,
                        pv_x_p2, pv_y_p2, pv_z_p2,
                        pv_xError_p1, pv_yError_p1, pv_zError_p1,
                        pv_xError_p2, pv_yError_p2, pv_zError_p2,
                        fill_action_data);

    // ---------- MC: process per entry ----------
    const Float_t lo = pv_SumTrackPt2_sqrt_edges[idx];
    const Float_t hi = pv_SumTrackPt2_sqrt_edges[idx + 1];

    for (auto &mc_entry : tuplelist[period.Data()]["mc"])
    {
        std::string path = mc_entry["path"];
        std::string dataset = mc_entry["dataset"];
        if (path.empty())
            continue;

        if (xsec_weight_map.find(dataset) == xsec_weight_map.end())
            continue;
        Float_t xsec_weight = xsec_weight_map[dataset];

        // PS weights with mask
        double ps_weights_entry[N_TRIGS];
        for (int it = 0; it < N_TRIGS; ++it)
        {
            std::string trig_key = std::string(TRIG_NAMES[it]).substr(4); // strip "HLT_"
            bool masked = mask_json[period.Data()][dataset][trig_key].get<bool>();
            ps_weights_entry[it] = masked ? 0.0 : ps_weights_final[it];
        }

        TChain *mctree_entry = new TChain("residuals/tree");
        mctree_entry->Add(TString(path) + "/*.root");
        bind_common_branches(mctree_entry);
        mctree_entry->SetBranchAddress("NumTrueInts", &NumTrueInts);
        bind_trig_branches(mctree_entry);

        Long64_t nMC_entry = mctree_entry->GetEntries();

        for (Long64_t ie = 0; ie < nMC_entry; ++ie)
        {
            mctree_entry->GetEntry(ie);

            Float_t w_ps = 0.0;
            for (Int_t it = 0; it < N_TRIGS; ++it)
            {
                if (trig_pass_raw[it])
                {
                    w_ps = ps_weights_entry[it];
                    break;
                }
            }

            Float_t w_pu = (NumTrueInts >= 1 && NumTrueInts <= (Int_t)PU_weights.size()) ? PU_weights[NumTrueInts - 1] : 0.0;

            Float_t w = w_pu * xsec_weight * w_ps;

            Float_t pv_SumTrackPt2_sqrt = std::sqrt(pv_SumTrackPt2);
            Bool_t in_bin = (pv_SumTrackPt2_sqrt > lo && pv_SumTrackPt2_sqrt < hi);
            Bool_t pvx_nonull = (pv_x_p1 != -777 && pv_x_p2 != -777 && pv_xError_p1 != -777 && pv_xError_p2 != -777);
            Bool_t pvy_nonull = (pv_y_p1 != -777 && pv_y_p2 != -777 && pv_yError_p1 != -777 && pv_yError_p2 != -777);
            Bool_t pvz_nonull = (pv_z_p1 != -777 && pv_z_p2 != -777 && pv_zError_p1 != -777 && pv_zError_p2 != -777);

            if (in_bin && pvx_nonull)
            {
                if (h_mc[PV_X])
                    h_mc[PV_X]->Fill((pv_x_p1 - pv_x_p2) / sqrt_2, w);
                if (h_mc[PULL_X])
                    h_mc[PULL_X]->Fill((pv_x_p1 - pv_x_p2) / sqrt(pv_xError_p1 * pv_xError_p1 + pv_xError_p2 * pv_xError_p2), w);
            }
            if (in_bin && pvy_nonull)
            {
                if (h_mc[PV_Y])
                    h_mc[PV_Y]->Fill((pv_y_p1 - pv_y_p2) / sqrt_2, w);
                if (h_mc[PULL_Y])
                    h_mc[PULL_Y]->Fill((pv_y_p1 - pv_y_p2) / sqrt(pv_yError_p1 * pv_yError_p1 + pv_yError_p2 * pv_yError_p2), w);
            }
            if (in_bin && pvz_nonull)
            {
                if (h_mc[PV_Z])
                    h_mc[PV_Z]->Fill((pv_z_p1 - pv_z_p2) / sqrt_2, w);
                if (h_mc[PULL_Z])
                    h_mc[PULL_Z]->Fill((pv_z_p1 - pv_z_p2) / sqrt(pv_zError_p1 * pv_zError_p1 + pv_zError_p2 * pv_zError_p2), w);
            }
        }

        delete mctree_entry;
    }

    // ---------- Fit & write JSON ----------
    nlohmann::json resojson;
    resojson["pv_SumTrackPt2_sqrt"] = (pv_SumTrackPt2_sqrt_edges[idx] + pv_SumTrackPt2_sqrt_edges[idx + 1]) / 2.0;

    Float_t reso_data[NCOMB] = {0.0};
    Float_t reso_mc[NCOMB] = {0.0};

    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        if (h_data[cid] && h_data[cid]->GetEntries() > 0)
            reso_data[cid] = fit_res(h_data[cid], period, "Data", histinfo[cid].dataFigPath, 0.1);
        if (h_mc[cid] && h_mc[cid]->GetEntries() > 0)
            reso_mc[cid] = fit_res(h_mc[cid], period, "Simulation", histinfo[cid].mcFigPath, 0.1);

        TString suffix = COMB_SUFFIX[cid];
        resojson[Form("reso_data_%s", suffix.Data())] = reso_data[cid];
        resojson[Form("reso_mc_%s", suffix.Data())] = reso_mc[cid];
    }

    std::ofstream outFile(storage_dir + "/json/" + period + Form("/pv_res/fit_%d.json", idx));
    outFile << resojson.dump(4);
    outFile.close();

    // ---------- Cleanup ----------
    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        delete h_data[cid];
        delete h_mc[cid];
    }
    delete datatree;

    return 0;
}