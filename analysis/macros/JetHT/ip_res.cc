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

const char *COMB_SUFFIX[NCOMB] = {
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
    "phi_allpt"};

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

const Float_t PS_VALUES[N_TRIGS] = {1, 16, 32, 64, 128, 256, 500, 1600, 4100, 11400};

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
    Int_t filledBins = 0;
    for (Int_t i = 1; i <= hist->GetNbinsX(); i++)
    {
        if (hist->GetBinContent(i) > 0)
            filledBins++;
    }
    if ((Double_t)filledBins / hist->GetNbinsX() < 0.1)
        return -1.f;

    setTDRStyle();
    lumi_sqrtS = "13.6 TeV, " + period;

    RooRealVar ip_var("ip_var", "ip_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    ip_var.setBins(hist->GetNbinsX());

    Float_t hist_mean = hist->GetMean();
    Float_t hist_rms = hist->GetRMS();

    RooRealVar mu("mu", "mu", hist_mean, hist_mean - hist_rms, hist_mean + hist_rms);
    RooRealVar sigma("sigma", "sigma", 0.5 * hist_rms, 0.1 * hist_rms, hist_rms);
    RooRealVar alpha("alpha", "alpha", 2.0, 0.5, 5.0);
    RooRealVar n("n", "n", 2.0, 0.5, 10.0);
    RooCrystalBall model("model", "Double Crystal Ball", ip_var, mu, sigma, alpha, n, alpha, n);

    RooDataHist hdatahist("hdatahist", "", ip_var, hist);
    RooFitResult *fitResult = model.fitTo(hdatahist, RooFit::Save(true));
    if (fitResult->status() != 0)
        std::cerr << "[WARN] fit_res: RooFit did not converge (status=" << fitResult->status() << ") for " << hist->GetName() << std::endl;
    fitResult->Print();
    delete fitResult;

    Float_t ip_var_max = ip_var.getMax();
    Float_t mean = mu.getVal();
    Float_t low = 0.0;
    Float_t high = ip_var_max - mean;

    ip_var.setRange("normRange", mean - 1e6, mean + 1e6);
    // ip_var.setRange("normRange", mean - 8 * hist_rms, mean + 8 * hist_rms);
    RooAbsReal *denom = model.createIntegral(ip_var, RooFit::Range("normRange"));
    while (high - low > tolerance)
    {
        Float_t mid = 0.5 * (low + high);
        ip_var.setRange("intRange", mean - mid, mean + mid);
        RooAbsReal *num = model.createIntegral(ip_var, RooFit::Range("intRange"));
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

    RooPlot *frame = ip_var.frame();
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
                         const std::vector<Float_t> &pv_trk_pt_edges,
                         const std::vector<Float_t> &pv_trk_pt_uleta_edges,
                         const std::vector<Float_t> &pv_trk_eta_edges,
                         const std::vector<Float_t> &pv_trk_phi_edges,
                         const std::vector<Float_t> &pv_trk_pt,
                         const std::vector<Float_t> &pv_trk_eta,
                         const std::vector<Float_t> &pv_trk_phi,
                         const std::vector<Float_t> &pv_trk_d0,
                         const std::vector<Float_t> &pv_trk_dz,
                         F &&action)
{
    const Float_t pt_lo = pv_trk_pt_edges[idx];
    const Float_t pt_hi = pv_trk_pt_edges[idx + 1];
    const Float_t pt_uleta_lo = pv_trk_pt_uleta_edges[idx];
    const Float_t pt_uleta_hi = pv_trk_pt_uleta_edges[idx + 1];
    const Float_t eta_lo = pv_trk_eta_edges[idx];
    const Float_t eta_hi = pv_trk_eta_edges[idx + 1];
    const Float_t phi_lo = pv_trk_phi_edges[idx];
    const Float_t phi_hi = pv_trk_phi_edges[idx + 1];

    for (Long64_t ie = 0; ie < nEntries; ++ie)
    {
        tree->GetEntry(ie);

        const size_t nTrk = pv_trk_pt.size();
        for (size_t it = 0; it < nTrk; ++it)
        {
            Float_t pt = pv_trk_pt[it];
            Float_t eta = pv_trk_eta[it];
            Float_t phi = pv_trk_phi[it];
            Float_t d0 = pv_trk_d0[it];
            Float_t dz = pv_trk_dz[it];

            const Float_t aeta = std::fabs(eta);

            Bool_t in_ptbin = (pt > pt_lo && pt < pt_hi);
            Bool_t in_ptuletabin = (pt > pt_uleta_lo && pt < pt_uleta_hi);
            Bool_t in_etabin = (eta > eta_lo && eta < eta_hi);
            Bool_t in_phibin = (phi > phi_lo && phi < phi_hi);

            Bool_t loeta = (aeta < 1.3);
            Bool_t hieta = (aeta > 1.3 && aeta < 2.5);
            Bool_t uleta = (aeta > 2.5 && aeta < 3.0);
            Bool_t alleta = (aeta < 10.0);

            Bool_t lopt = (pt > 0.1 && pt < 1.0);
            Bool_t hipt = (pt > 1.0 && pt < 3.0);
            Bool_t ulpt = (pt > 3.0 && pt < 10.0);
            Bool_t allpt = (pt > 0);

            // pt × eta
            if (in_ptbin && loeta)
                action(PT_LOETA, d0, dz);
            if (in_ptbin && hieta)
                action(PT_HIETA, d0, dz);
            if (in_ptuletabin && uleta)
                action(PT_ULETA, d0, dz);
            if (in_ptbin && alleta)
                action(PT_ALLETA, d0, dz);

            // eta × pt
            if (in_etabin && lopt)
                action(ETA_LOPT, d0, dz);
            if (in_etabin && hipt)
                action(ETA_HIPT, d0, dz);
            if (in_etabin && ulpt)
                action(ETA_ULPT, d0, dz);
            if (in_etabin && allpt)
                action(ETA_ALLPT, d0, dz);

            // phi × pt
            if (in_phibin && lopt)
                action(PHI_LOPT, d0, dz);
            if (in_phibin && hipt)
                action(PHI_HIPT, d0, dz);
            if (in_phibin && ulpt)
                action(PHI_ULPT, d0, dz);
            if (in_phibin && allpt)
                action(PHI_ALLPT, d0, dz);
        }
    }
}

Int_t ip_res(TString period, Int_t idx)
{
    TString figdir = storage_dir + "/figures/" + period + "/ip_res/";

    // ---------- Read JSONs ----------
    std::ifstream tuplelist_file("/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/JetHT/tuplelist.json");
    nlohmann::json tuplelist;
    tuplelist_file >> tuplelist;
    tuplelist_file.close();

    std::ifstream infile(storage_dir + "/json/binning.json");
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

    // std::ifstream ps_file(storage_dir + "/pileup/" + period + "/PS_weight.json");
    // nlohmann::json ps_json;
    // ps_file >> ps_json;
    // ps_file.close();

    std::ifstream mask_file(storage_dir + "/pileup/" + period + "/trigger_mask.json");
    nlohmann::json mask_json;
    mask_file >> mask_json;
    mask_file.close();

    // ---------- Build lookup tables ----------
    std::vector<Float_t> pv_trk_pt_edges = binning["pv_trk_pt"].get<std::vector<Float_t>>();
    std::vector<Float_t> pv_trk_pt_uleta_edges = binning["pv_trk_pt_uleta"].get<std::vector<Float_t>>();
    std::vector<Float_t> pv_trk_eta_edges = binning["pv_trk_eta"].get<std::vector<Float_t>>();
    std::vector<Float_t> pv_trk_phi_edges = binning["pv_trk_phi"].get<std::vector<Float_t>>();

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
        ps_weights_final[it] = 1.0f / PS_VALUES[it];
    // ps_weights_final[it] = ps_json[it]["PS_weight"].get<Float_t>();

    // ---------- Branch variables ----------
    std::vector<Float_t> *pv_trk_pt = nullptr;
    std::vector<Float_t> *pv_trk_eta = nullptr;
    std::vector<Float_t> *pv_trk_phi = nullptr;
    std::vector<Float_t> *pv_trk_d0 = nullptr;
    std::vector<Float_t> *pv_trk_dz = nullptr;
    Int_t NumTrueInts;
    Bool_t trig_pass_raw[N_TRIGS];

    auto bind_common_branches = [&](TChain *tree)
    {
        tree->SetBranchAddress("pv_trk_pt", &pv_trk_pt);
        tree->SetBranchAddress("pv_trk_eta", &pv_trk_eta);
        tree->SetBranchAddress("pv_trk_phi", &pv_trk_phi);
        tree->SetBranchAddress("pv_trk_d0_pvunbiased", &pv_trk_d0);
        tree->SetBranchAddress("pv_trk_dz_pvunbiased", &pv_trk_dz);
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
    TString ptcut_title = Form("%.3f<#it{p_{T}}<%.3f GeV", pv_trk_pt_edges[idx], pv_trk_pt_edges[idx + 1]);
    TString ptcut_uleta_title = Form("%.3f<#it{p_{T}}<%.3f GeV", pv_trk_pt_uleta_edges[idx], pv_trk_pt_uleta_edges[idx + 1]);
    TString etacut_title = Form("%.2f<#it{#eta}<%.2f", pv_trk_eta_edges[idx], pv_trk_eta_edges[idx + 1]);
    TString phicut_title = Form("%.2f<#it{#phi}<%.2f", pv_trk_phi_edges[idx], pv_trk_phi_edges[idx + 1]);

    TString lopt_title = "0.1<#it{p_{T}}<1 GeV";
    TString hipt_title = "1<#it{p_{T}}<3 GeV";
    TString ulpt_title = "3<#it{p_{T}}<10 GeV";

    TString loeta_title = "|#it{#eta}|<1.3";
    TString hieta_title = "1.3<|#it{#eta}|<2.5";
    TString uleta_title = "2.5<|#it{#eta}|<3.0";

    HistInfo info_d0[NCOMB];
    HistInfo info_dz[NCOMB];

    info_d0[PT_LOETA].title = "#splitline{" + ptcut_title + "}{" + loeta_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PT_LOETA].dataFigPath = figdir + Form("ippv_xy_fit/data_pt_loeta_%d", idx);
    info_d0[PT_LOETA].mcFigPath = figdir + Form("ippv_xy_fit/mc_pt_loeta_%d", idx);
    info_dz[PT_LOETA].title = "#splitline{" + ptcut_title + "}{" + loeta_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PT_LOETA].dataFigPath = figdir + Form("ippv_z_fit/data_pt_loeta_%d", idx);
    info_dz[PT_LOETA].mcFigPath = figdir + Form("ippv_z_fit/mc_pt_loeta_%d", idx);

    info_d0[PT_HIETA].title = "#splitline{" + ptcut_title + "}{" + hieta_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PT_HIETA].dataFigPath = figdir + Form("ippv_xy_fit/data_pt_hieta_%d", idx);
    info_d0[PT_HIETA].mcFigPath = figdir + Form("ippv_xy_fit/mc_pt_hieta_%d", idx);
    info_dz[PT_HIETA].title = "#splitline{" + ptcut_title + "}{" + hieta_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PT_HIETA].dataFigPath = figdir + Form("ippv_z_fit/data_pt_hieta_%d", idx);
    info_dz[PT_HIETA].mcFigPath = figdir + Form("ippv_z_fit/mc_pt_hieta_%d", idx);

    info_d0[PT_ULETA].title = "#splitline{" + ptcut_uleta_title + "}{" + uleta_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PT_ULETA].dataFigPath = figdir + Form("ippv_xy_fit/data_pt_uleta_%d", idx);
    info_d0[PT_ULETA].mcFigPath = figdir + Form("ippv_xy_fit/mc_pt_uleta_%d", idx);
    info_dz[PT_ULETA].title = "#splitline{" + ptcut_uleta_title + "}{" + uleta_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PT_ULETA].dataFigPath = figdir + Form("ippv_z_fit/data_pt_uleta_%d", idx);
    info_dz[PT_ULETA].mcFigPath = figdir + Form("ippv_z_fit/mc_pt_uleta_%d", idx);

    info_d0[PT_ALLETA].title = ptcut_title + ";Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PT_ALLETA].dataFigPath = figdir + Form("ippv_xy_fit/data_pt_alleta_%d", idx);
    info_d0[PT_ALLETA].mcFigPath = figdir + Form("ippv_xy_fit/mc_pt_alleta_%d", idx);
    info_dz[PT_ALLETA].title = ptcut_title + ";Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PT_ALLETA].dataFigPath = figdir + Form("ippv_z_fit/data_pt_alleta_%d", idx);
    info_dz[PT_ALLETA].mcFigPath = figdir + Form("ippv_z_fit/mc_pt_alleta_%d", idx);

    info_d0[ETA_LOPT].title = "#splitline{" + etacut_title + "}{" + lopt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[ETA_LOPT].dataFigPath = figdir + Form("ippv_xy_fit/data_eta_lopt_%d", idx);
    info_d0[ETA_LOPT].mcFigPath = figdir + Form("ippv_xy_fit/mc_eta_lopt_%d", idx);
    info_dz[ETA_LOPT].title = "#splitline{" + etacut_title + "}{" + lopt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[ETA_LOPT].dataFigPath = figdir + Form("ippv_z_fit/data_eta_lopt_%d", idx);
    info_dz[ETA_LOPT].mcFigPath = figdir + Form("ippv_z_fit/mc_eta_lopt_%d", idx);

    info_d0[ETA_HIPT].title = "#splitline{" + etacut_title + "}{" + hipt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[ETA_HIPT].dataFigPath = figdir + Form("ippv_xy_fit/data_eta_hipt_%d", idx);
    info_d0[ETA_HIPT].mcFigPath = figdir + Form("ippv_xy_fit/mc_eta_hipt_%d", idx);
    info_dz[ETA_HIPT].title = "#splitline{" + etacut_title + "}{" + hipt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[ETA_HIPT].dataFigPath = figdir + Form("ippv_z_fit/data_eta_hipt_%d", idx);
    info_dz[ETA_HIPT].mcFigPath = figdir + Form("ippv_z_fit/mc_eta_hipt_%d", idx);

    info_d0[ETA_ULPT].title = "#splitline{" + etacut_title + "}{" + ulpt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[ETA_ULPT].dataFigPath = figdir + Form("ippv_xy_fit/data_eta_ulpt_%d", idx);
    info_d0[ETA_ULPT].mcFigPath = figdir + Form("ippv_xy_fit/mc_eta_ulpt_%d", idx);
    info_dz[ETA_ULPT].title = "#splitline{" + etacut_title + "}{" + ulpt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[ETA_ULPT].dataFigPath = figdir + Form("ippv_z_fit/data_eta_ulpt_%d", idx);
    info_dz[ETA_ULPT].mcFigPath = figdir + Form("ippv_z_fit/mc_eta_ulpt_%d", idx);

    info_d0[ETA_ALLPT].title = etacut_title + ";Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[ETA_ALLPT].dataFigPath = figdir + Form("ippv_xy_fit/data_eta_allpt_%d", idx);
    info_d0[ETA_ALLPT].mcFigPath = figdir + Form("ippv_xy_fit/mc_eta_allpt_%d", idx);
    info_dz[ETA_ALLPT].title = etacut_title + ";Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[ETA_ALLPT].dataFigPath = figdir + Form("ippv_z_fit/data_eta_allpt_%d", idx);
    info_dz[ETA_ALLPT].mcFigPath = figdir + Form("ippv_z_fit/mc_eta_allpt_%d", idx);

    info_d0[PHI_LOPT].title = "#splitline{" + phicut_title + "}{" + lopt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PHI_LOPT].dataFigPath = figdir + Form("ippv_xy_fit/data_phi_lopt_%d", idx);
    info_d0[PHI_LOPT].mcFigPath = figdir + Form("ippv_xy_fit/mc_phi_lopt_%d", idx);
    info_dz[PHI_LOPT].title = "#splitline{" + phicut_title + "}{" + lopt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PHI_LOPT].dataFigPath = figdir + Form("ippv_z_fit/data_phi_lopt_%d", idx);
    info_dz[PHI_LOPT].mcFigPath = figdir + Form("ippv_z_fit/mc_phi_lopt_%d", idx);

    info_d0[PHI_HIPT].title = "#splitline{" + phicut_title + "}{" + hipt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PHI_HIPT].dataFigPath = figdir + Form("ippv_xy_fit/data_phi_hipt_%d", idx);
    info_d0[PHI_HIPT].mcFigPath = figdir + Form("ippv_xy_fit/mc_phi_hipt_%d", idx);
    info_dz[PHI_HIPT].title = "#splitline{" + phicut_title + "}{" + hipt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PHI_HIPT].dataFigPath = figdir + Form("ippv_z_fit/data_phi_hipt_%d", idx);
    info_dz[PHI_HIPT].mcFigPath = figdir + Form("ippv_z_fit/mc_phi_hipt_%d", idx);

    info_d0[PHI_ULPT].title = "#splitline{" + phicut_title + "}{" + ulpt_title + "};Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PHI_ULPT].dataFigPath = figdir + Form("ippv_xy_fit/data_phi_ulpt_%d", idx);
    info_d0[PHI_ULPT].mcFigPath = figdir + Form("ippv_xy_fit/mc_phi_ulpt_%d", idx);
    info_dz[PHI_ULPT].title = "#splitline{" + phicut_title + "}{" + ulpt_title + "};Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PHI_ULPT].dataFigPath = figdir + Form("ippv_z_fit/data_phi_ulpt_%d", idx);
    info_dz[PHI_ULPT].mcFigPath = figdir + Form("ippv_z_fit/mc_phi_ulpt_%d", idx);

    info_d0[PHI_ALLPT].title = phicut_title + ";Track IP #it{d_{xy}} [#mum];# tracks";
    info_d0[PHI_ALLPT].dataFigPath = figdir + Form("ippv_xy_fit/data_phi_allpt_%d", idx);
    info_d0[PHI_ALLPT].mcFigPath = figdir + Form("ippv_xy_fit/mc_phi_allpt_%d", idx);
    info_dz[PHI_ALLPT].title = phicut_title + ";Track IP #it{d_{z}} [#mum];# tracks";
    info_dz[PHI_ALLPT].dataFigPath = figdir + Form("ippv_z_fit/data_phi_allpt_%d", idx);
    info_dz[PHI_ALLPT].mcFigPath = figdir + Form("ippv_z_fit/mc_phi_allpt_%d", idx);

    Float_t init_d0_min[NCOMB] = {-2000, -4000, -6000, -4000, -2000, -1000, -400, -2000, -2000, -500, -400, -1500};
    Float_t init_d0_max[NCOMB] = {2000, 4000, 6000, 4000, 2000, 1000, 400, 2000, 2000, 500, 400, 1500};
    Float_t init_dz_min[NCOMB] = {-2000, -10000, -10000, -10000, -8000, -4000, -2000, -5000, -3000, -1500, -600, -2000};
    Float_t init_dz_max[NCOMB] = {2000, 10000, 10000, 10000, 8000, 4000, 2000, 5000, 3000, 1500, 600, 2000};

    // ---------- First pass on data: compute histogram ranges ----------
    Stat stats_d0_data[NCOMB];
    Stat stats_dz_data[NCOMB];
    auto stat_action_data = [&](Int_t cid, Float_t d0, Float_t dz)
    {
        if (d0 >= init_d0_min[cid] && d0 <= init_d0_max[cid])
            stats_d0_data[cid].Fill(d0);
        if (dz >= init_dz_min[cid] && dz <= init_dz_max[cid])
            stats_dz_data[cid].Fill(dz);
    };

    process_tree_tracks(datatree, nData, idx,
                        pv_trk_pt_edges, pv_trk_pt_uleta_edges, pv_trk_eta_edges, pv_trk_phi_edges,
                        *pv_trk_pt, *pv_trk_eta, *pv_trk_phi,
                        *pv_trk_d0, *pv_trk_dz,
                        stat_action_data);

    // ---------- Build histograms ----------
    TH1F *h_d0_data[NCOMB] = {nullptr};
    TH1F *h_d0_mc[NCOMB] = {nullptr};
    TH1F *h_dz_data[NCOMB] = {nullptr};
    TH1F *h_dz_mc[NCOMB] = {nullptr};

    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        if (!stats_d0_data[cid].Valid() || !stats_dz_data[cid].Valid())
            continue;
        Float_t mean_d0 = stats_d0_data[cid].Mean();
        Float_t sigma_d0 = stats_d0_data[cid].Sigma();
        Float_t mean_dz = stats_dz_data[cid].Mean();
        Float_t sigma_dz = stats_dz_data[cid].Sigma();
        if (sigma_d0 <= 0.0)
        {
            mean_d0 = 0.5f * (init_d0_min[cid] + init_d0_max[cid]);
            sigma_d0 = (init_d0_max[cid] - init_d0_min[cid]) / (2.f * nsigma);
        }
        if (sigma_dz <= 0.0)
        {
            mean_dz = 0.5f * (init_dz_min[cid] + init_dz_max[cid]);
            sigma_dz = (init_dz_max[cid] - init_dz_min[cid]) / (2.f * nsigma);
        }
        Float_t d0_min = mean_d0 - nsigma * sigma_d0;
        Float_t d0_max = mean_d0 + nsigma * sigma_d0;
        Float_t dz_min = mean_dz - nsigma * sigma_dz;
        Float_t dz_max = mean_dz + nsigma * sigma_dz;
        // if (abs(pv_trk_eta_edges[idx] + pv_trk_eta_edges[idx + 1]) / 2 > 2.4 && (cid >= 5) && (cid <= 7))
        // {
        //     dz_min = -1000.f;
        //     dz_max = 1000.f;
        // }
        h_d0_data[cid] = new TH1F(Form("h_d0_data_%d_%d", idx, cid), info_d0[cid].title, nbins, d0_min, d0_max);
        h_d0_mc[cid] = new TH1F(Form("h_d0_mc_%d_%d", idx, cid), info_d0[cid].title, nbins, d0_min, d0_max);
        h_dz_data[cid] = new TH1F(Form("h_dz_data_%d_%d", idx, cid), info_dz[cid].title, nbins, dz_min, dz_max);
        h_dz_mc[cid] = new TH1F(Form("h_dz_mc_%d_%d", idx, cid), info_dz[cid].title, nbins, dz_min, dz_max);
        h_d0_data[cid]->Sumw2();
        h_d0_mc[cid]->Sumw2();
        h_dz_data[cid]->Sumw2();
        h_dz_mc[cid]->Sumw2();
    }

    // ---------- Second pass on data: fill ----------
    auto fill_action_data = [&](Int_t cid, Float_t d0, Float_t dz)
    {
        if (h_d0_data[cid])
            h_d0_data[cid]->Fill(d0);
        if (h_dz_data[cid])
            h_dz_data[cid]->Fill(dz);
    };
    process_tree_tracks(datatree, nData, idx,
                        pv_trk_pt_edges, pv_trk_pt_uleta_edges, pv_trk_eta_edges, pv_trk_phi_edges,
                        *pv_trk_pt, *pv_trk_eta, *pv_trk_phi,
                        *pv_trk_d0, *pv_trk_dz,
                        fill_action_data);

    // ---------- MC: process per entry ----------
    const Float_t pt_lo = pv_trk_pt_edges[idx];
    const Float_t pt_hi = pv_trk_pt_edges[idx + 1];
    const Float_t pt_uleta_lo = pv_trk_pt_uleta_edges[idx];
    const Float_t pt_uleta_hi = pv_trk_pt_uleta_edges[idx + 1];
    const Float_t eta_lo = pv_trk_eta_edges[idx];
    const Float_t eta_hi = pv_trk_eta_edges[idx + 1];
    const Float_t phi_lo = pv_trk_phi_edges[idx];
    const Float_t phi_hi = pv_trk_phi_edges[idx + 1];

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
            bool masked = false;
            if (mask_json.contains(period.Data()) && mask_json[period.Data()].contains(dataset))
                masked = mask_json[period.Data()][dataset].value(trig_key, false);
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

            const size_t nTrk = pv_trk_pt->size();
            for (size_t it = 0; it < nTrk; ++it)
            {
                Float_t pt = (*pv_trk_pt)[it];
                Float_t eta = (*pv_trk_eta)[it];
                Float_t phi = (*pv_trk_phi)[it];
                Float_t d0 = (*pv_trk_d0)[it];
                Float_t dz = (*pv_trk_dz)[it];

                const Float_t aeta = std::fabs(eta);

                Bool_t in_ptbin = (pt > pt_lo && pt < pt_hi);
                Bool_t in_ptuletabin = (pt > pt_uleta_lo && pt < pt_uleta_hi);
                Bool_t in_etabin = (eta > eta_lo && eta < eta_hi);
                Bool_t in_phibin = (phi > phi_lo && phi < phi_hi);

                Bool_t loeta = (aeta < 1.3);
                Bool_t hieta = (aeta > 1.3 && aeta < 2.5);
                Bool_t uleta = (aeta > 2.5 && aeta < 3.0);
                Bool_t alleta = (aeta < 10.0);

                Bool_t lopt = (pt > 0.1 && pt < 1.0);
                Bool_t hipt = (pt > 1.0 && pt < 3.0);
                Bool_t ulpt = (pt > 3.0 && pt < 10.0);
                Bool_t allpt = (pt > 0);

                auto fill = [&](Int_t cid, Float_t d0v, Float_t dzv)
                {
                    if (h_d0_mc[cid])
                        h_d0_mc[cid]->Fill(d0v, w);
                    if (h_dz_mc[cid])
                        h_dz_mc[cid]->Fill(dzv, w);
                };

                if (in_ptbin && loeta)
                    fill(PT_LOETA, d0, dz);
                if (in_ptbin && hieta)
                    fill(PT_HIETA, d0, dz);
                if (in_ptuletabin && uleta)
                    fill(PT_ULETA, d0, dz);
                if (in_ptbin && alleta)
                    fill(PT_ALLETA, d0, dz);

                if (in_etabin && lopt)
                    fill(ETA_LOPT, d0, dz);
                if (in_etabin && hipt)
                    fill(ETA_HIPT, d0, dz);
                if (in_etabin && ulpt)
                    fill(ETA_ULPT, d0, dz);
                if (in_etabin && allpt)
                    fill(ETA_ALLPT, d0, dz);

                if (in_phibin && lopt)
                    fill(PHI_LOPT, d0, dz);
                if (in_phibin && hipt)
                    fill(PHI_HIPT, d0, dz);
                if (in_phibin && ulpt)
                    fill(PHI_ULPT, d0, dz);
                if (in_phibin && allpt)
                    fill(PHI_ALLPT, d0, dz);
            }
        }

        delete mctree_entry;
    }

    // ---------- Fit & write JSON ----------
    nlohmann::json resojson;
    resojson["pt"] = (pv_trk_pt_edges[idx] + pv_trk_pt_edges[idx + 1]) / 2.0;
    resojson["pt_uleta"] = (pv_trk_pt_uleta_edges[idx] + pv_trk_pt_uleta_edges[idx + 1]) / 2.0;
    resojson["eta"] = (pv_trk_eta_edges[idx] + pv_trk_eta_edges[idx + 1]) / 2.0;
    resojson["phi"] = (pv_trk_phi_edges[idx] + pv_trk_phi_edges[idx + 1]) / 2.0;

    Float_t reso_data_d0[NCOMB] = {0.0};
    Float_t reso_data_dz[NCOMB] = {0.0};
    Float_t reso_mc_d0[NCOMB] = {0.0};
    Float_t reso_mc_dz[NCOMB] = {0.0};

    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        if (h_d0_data[cid] && h_d0_data[cid]->GetEntries() > 0)
            reso_data_d0[cid] = fit_res(h_d0_data[cid], period, "Data", info_d0[cid].dataFigPath, 0.1);
        if (h_dz_data[cid] && h_dz_data[cid]->GetEntries() > 0)
            reso_data_dz[cid] = fit_res(h_dz_data[cid], period, "Data", info_dz[cid].dataFigPath, 0.1);
        if (h_d0_mc[cid] && h_d0_mc[cid]->GetEntries() > 0)
            reso_mc_d0[cid] = fit_res(h_d0_mc[cid], period, "Simulation", info_d0[cid].mcFigPath, 0.1);
        if (h_dz_mc[cid] && h_dz_mc[cid]->GetEntries() > 0)
            reso_mc_dz[cid] = fit_res(h_dz_mc[cid], period, "Simulation", info_dz[cid].mcFigPath, 0.1);

        TString suffix = COMB_SUFFIX[cid];
        resojson[Form("reso_data_d0_%s", suffix.Data())] = reso_data_d0[cid];
        resojson[Form("reso_data_dz_%s", suffix.Data())] = reso_data_dz[cid];
        resojson[Form("reso_mc_d0_%s", suffix.Data())] = reso_mc_d0[cid];
        resojson[Form("reso_mc_dz_%s", suffix.Data())] = reso_mc_dz[cid];
    }

    std::ofstream outFile(storage_dir + "/json/" + period + Form("/ip_res/fit_%d.json", idx));
    outFile << resojson.dump(4);
    outFile.close();

    // ---------- Cleanup ----------
    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        delete h_d0_data[cid];
        delete h_d0_mc[cid];
        delete h_dz_data[cid];
        delete h_dz_mc[cid];
    }
    delete datatree;

    return 0;
}