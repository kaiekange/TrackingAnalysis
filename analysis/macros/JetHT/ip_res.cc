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
#include <TH2.h>
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

const TString datatype_text = "Multi-jet events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";

const Int_t sample_mod = 1;


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
    lumi_sqrtS = period + " (13.6 TeV)";

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

Int_t ip_res(TString period, Int_t idx)
{
    TString figdir = storage_dir + "/figures/" + period + "/ip_res/";

    std::ifstream infile(storage_dir + "/json/binning.json");
    nlohmann::json binning;
    infile >> binning;
    infile.close();

    std::vector<Float_t> pv_trk_pt_edges = binning["pv_trk_pt"].get<std::vector<Float_t>>();
    std::vector<Float_t> pv_trk_pt_uleta_edges = binning["pv_trk_pt_uleta"].get<std::vector<Float_t>>();
    std::vector<Float_t> pv_trk_eta_edges = binning["pv_trk_eta"].get<std::vector<Float_t>>();
    std::vector<Float_t> pv_trk_phi_edges = binning["pv_trk_phi"].get<std::vector<Float_t>>();
    const Int_t n_pv = binning["pv_SumTrackPt2_sqrt"].size() - 1;

    // ---------- PV pulls per sqrt(SumPt2) bin (from pv_res.cc) ----------
    std::vector<Double_t> pull_x(n_pv), pull_y(n_pv), pull_z(n_pv);
    for (Int_t k = 0; k < n_pv; ++k)
    {
        std::ifstream pvfile(storage_dir + "/json/" + period + Form("/pv_res/fit_%d.json", k));
        if (!pvfile)
        {
            std::cerr << "[ERROR] ip_res: missing PV result " << storage_dir + "/json/" + period + Form("/pv_res/fit_%d.json", k) << std::endl;
            return 1;
        }
        nlohmann::json pv;
        pvfile >> pv;
        pull_x[k] = pv["reso_data_pullx"];
        pull_y[k] = pv["reso_data_pully"];
        pull_z[k] = pv["reso_data_pullz"];
        // a failed pull fit (0, or far from 1) would silently distort the subtraction
        auto bad = [](Double_t p) { return p < 0.5 || p > 2.0; };
        if (bad(pull_x[k]) || bad(pull_y[k]) || bad(pull_z[k]))
        {
            std::cerr << "[ERROR] ip_res: implausible PV pull (" << pull_x[k] << ", " << pull_y[k] << ", " << pull_z[k]
                      << ") in sqrt(SumPt2) bin " << k << std::endl;
            return 1;
        }
    }

    // histograms filled by fill_hists.cc
    TFile *histfile = TFile::Open(storage_dir + "/hists/" + period + "/fill.root");
    if (!histfile || histfile->IsZombie())
    {
        std::cerr << "[ERROR] ip_res: cannot open " << storage_dir + "/hists/" + period + "/fill.root" << std::endl;
        return 1;
    }

    // Mean PV error^2 of the tracks in one IP histogram, from the projected unbiased-PV covariance
    // sums of fill_hists.cc (x: sqrt(SumPt2) bin; y: xx yy zz xy xz yz ntrk), with or without pulls.
    auto mean_pverr2 = [&](TH2D *e, Bool_t use_pull)
    {
        Double_t sum = 0, ntrk = 0;
        for (Int_t k = 0; k < n_pv; ++k)
        {
            const Double_t px = use_pull ? pull_x[k] : 1, py = use_pull ? pull_y[k] : 1, pz = use_pull ? pull_z[k] : 1;
            sum += px * px * e->GetBinContent(k + 1, 1) + py * py * e->GetBinContent(k + 1, 2) + pz * pz * e->GetBinContent(k + 1, 3) +
                   px * py * e->GetBinContent(k + 1, 4) + px * pz * e->GetBinContent(k + 1, 5) + py * pz * e->GetBinContent(k + 1, 6);
            ntrk += e->GetBinContent(k + 1, 7);
        }
        return ntrk > 0 ? sum / ntrk : 0.0;
    };

    nlohmann::json resojson;
    resojson["pt"] = (pv_trk_pt_edges[idx] + pv_trk_pt_edges[idx + 1]) / 2.0;
    resojson["pt_uleta"] = (pv_trk_pt_uleta_edges[idx] + pv_trk_pt_uleta_edges[idx + 1]) / 2.0;
    resojson["eta"] = (pv_trk_eta_edges[idx] + pv_trk_eta_edges[idx + 1]) / 2.0;
    resojson["phi"] = (pv_trk_phi_edges[idx] + pv_trk_phi_edges[idx + 1]) / 2.0;

    const char *VAR[2] = {"d0", "dz"};
    const char *FIGSUB[2] = {"ippv_xy_fit", "ippv_z_fit"};
    const Int_t N_GRP = 4;  // PV-error groups per histogram, as in fill_hists.cc

    // Fit one histogram, subtract its mean (pull-corrected) PV error^2 in quadrature, write
    // reso_data_<tag>, reso_data_<tag>_pvsub, pverr2_data_<tag>, pverr2nopull_data_<tag>, ntrk_data_<tag>.
    auto measure = [&](const char *var, Int_t v, TString tag, TString figname)
    {
        TH1F *h = (TH1F *)histfile->Get(Form("ip_%s_%d_%s", var, idx, tag.Data()));
        TH2D *e = (TH2D *)histfile->Get(Form("pverr_%s_%d_%s", var, idx, tag.Data()));

        // measured resolution (68% half-width of the double-sided CB fit), as before
        Float_t reso = 0.0;
        if (h && h->GetEntries() > 0)
            reso = fit_res(h, period, "Data", figdir + FIGSUB[v] + "/" + figname, 0.1);

        Double_t pverr2 = e ? mean_pverr2(e, kTRUE) : 0.0;
        Double_t pverr2_nopull = e ? mean_pverr2(e, kFALSE) : 0.0;
        Double_t ntrk = 0;
        for (Int_t k = 0; e && k < n_pv; ++k)
            ntrk += e->GetBinContent(k + 1, 7);
        Double_t reso_sub = -1.0;
        if (reso > 0 && reso * reso > pverr2)
            reso_sub = std::sqrt(reso * reso - pverr2);
        else if (reso > 0)
            std::cerr << "[WARN] ip_res: PV error^2 " << pverr2 << " >= measured reso^2 " << reso * reso
                      << " for " << var << " " << tag << " bin " << idx << std::endl;

        TString key = TString(var) + "_" + tag;
        resojson["reso_data_" + std::string(key.Data())] = reso;
        resojson["reso_data_" + std::string(key.Data()) + "_pvsub"] = reso_sub;
        resojson["pverr2_data_" + std::string(key.Data())] = pverr2;
        resojson["pverr2nopull_data_" + std::string(key.Data())] = pverr2_nopull;
        resojson["ntrk_data_" + std::string(key.Data())] = ntrk;
    };

    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        TString suffix = COMB_SUFFIX[cid];
        for (Int_t v = 0; v < 2; ++v)
        {
            // all tracks of the bin
            measure(VAR[v], v, suffix, Form("data_%s_%d", suffix.Data(), idx));

            // the same bin split into groups of the track's own PV error (edges: no pull, from fill_hists.cc)
            TH1D *edges = (TH1D *)histfile->Get(Form("grpedges_%s_%d_%s", VAR[v], idx, suffix.Data()));
            resojson[Form("ngroups_data_%s_%s", VAR[v], suffix.Data())] = edges ? N_GRP : 0;
            if (!edges)
            {
                std::cerr << "[WARN] ip_res: no PV-error groups for " << VAR[v] << " " << suffix << " bin " << idx << std::endl;
                continue;
            }
            std::vector<Double_t> ev;
            for (Int_t j = 1; j <= edges->GetNbinsX(); ++j)
                ev.push_back(edges->GetBinContent(j));
            resojson[Form("sigpv_edges_%s_%s", VAR[v], suffix.Data())] = ev;
            for (Int_t g = 0; g < N_GRP; ++g)
                measure(VAR[v], v, suffix + Form("_g%d", g), Form("data_%s_g%d_%d", suffix.Data(), g, idx));
        }
    }

    std::ofstream outFile(storage_dir + "/json/" + period + Form("/ip_res/fit_%d.json", idx));
    outFile << resojson.dump(4);
    outFile.close();

    histfile->Close();
    return 0;
}
