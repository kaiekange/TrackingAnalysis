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
#include <RooCrystalBall.h>
#include <RooFFTConvPdf.h>
#include <TMatrixDSym.h>

#include <nlohmann/json.hpp>

#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"

const TString datatype_text = "Multi-jet events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";

const Int_t sample_mod = 1;


// must match IP_SUFFIX in fill_hists.cc: vs pT per |eta| slice (pv_trk_abseta_slices of binning.json) and for
// all |eta| < 2.5; vs eta and vs phi for 1 < pT < 3 GeV (hipt) and 3 < pT < 10 GeV (ulpt)
const Int_t NCOMB = 14;
const char *COMB_SUFFIX[NCOMB] = {
    "pt_eta0", "pt_eta1", "pt_eta2", "pt_eta3", "pt_eta4", "pt_eta5", "pt_eta6", "pt_eta7", "pt_eta8",
    "pt_alleta", "eta_hipt", "eta_ulpt", "phi_hipt", "phi_ulpt"};

const Double_t HW_TOL = 1e-3;     // bisection tolerance of the reported 68% half-widths [um]
const Double_t HW_TOL_ERR = 1e-6; // ... of the half-widths in the numerical derivatives of the error
const Int_t CONV_NCACHE = 8192;   // FFT cache bins of the convolution fit

// 68% half-width of pdf around mean (normalisation range mean +- 1e6), bisection to tol
Double_t halfwidth(RooAbsPdf &pdf, RooRealVar &x, Double_t mean, Double_t tol)
{
    x.setRange("normRange", mean - 1e6, mean + 1e6);
    RooAbsReal *denom = pdf.createIntegral(x, RooFit::Range("normRange"));
    const Double_t den = denom->getVal();
    delete denom;
    Double_t low = 0.0, high = x.getMax() - mean;
    while (high - low > tol)
    {
        const Double_t mid = 0.5 * (low + high);
        x.setRange("intRange", mean - mid, mean + mid);
        RooAbsReal *num = pdf.createIntegral(x, RooFit::Range("intRange"));
        if (num->getVal() / den < 0.68)
            low = mid;
        else
            high = mid;
        delete num;
    }
    return 0.5 * (low + high);
}

// true if a parameter is within 1e-3 of its range from a limit (its Hesse error is then unreliable)
Bool_t at_limit(const RooRealVar &p)
{
    const Double_t d = 1e-3 * (p.getMax() - p.getMin());
    return p.getVal() - p.getMin() < d || p.getMax() - p.getVal() < d;
}

// Statistical error of the half-width of a symmetric pdf: gradient in its shape parameters `par` (not the mean,
// which drops out) by central differences, times the fit covariance of these parameters. Restores the parameters.
// -1 if the covariance is not accurate (covQual < 3) or a parameter is at a limit.
// Not included: the error of the subtracted PV error, the fit-model uncertainty.
Double_t halfwidth_error(RooAbsPdf &pdf, RooRealVar &x, RooRealVar &mu, const RooArgList &par, const RooFitResult *res)
{
    const Int_t np = par.getSize();
    if (res->covQual() < 3)
        return -1.0;
    for (Int_t i = 0; i < np; ++i)
        if (at_limit(*(RooRealVar *)par.at(i)))
            return -1.0;
    const TMatrixDSym cov = res->reducedCovarianceMatrix(par);
    std::vector<Double_t> grad(np);
    for (Int_t i = 0; i < np; ++i)
    {
        RooRealVar *p = (RooRealVar *)par.at(i);
        const Double_t v0 = p->getVal();
        const Double_t h = std::max(1e-3 * std::sqrt(cov(i, i)), 1e-6 * std::fabs(v0));
        const Double_t vp = std::min(v0 + h, p->getMax()), vm = std::max(v0 - h, p->getMin());
        p->setVal(vp);
        const Double_t fp = halfwidth(pdf, x, mu.getVal(), HW_TOL_ERR);
        p->setVal(vm);
        const Double_t fm = halfwidth(pdf, x, mu.getVal(), HW_TOL_ERR);
        p->setVal(v0);
        grad[i] = (fp - fm) / (vp - vm);
    }
    Double_t var = 0;
    for (Int_t i = 0; i < np; ++i)
        for (Int_t j = 0; j < np; ++j)
            var += grad[i] * cov(i, j) * grad[j];
    return var > 0 ? std::sqrt(var) : -1.0;
}

struct FitOut
{
    Float_t reso = -1, err = -1;          // fit: 68% half-width and its stat. error (-1: not valid)
    Int_t status = -1, covqual = -1;      // RooFit status (0 = converged; -1: not fitted, too few filled bins), covQual
    Float_t conv = -1, conv_err = -1;     // convolution fit model (x) Gauss(0, s_PV): half-width of the model alone
    Int_t conv_status = -1, conv_covqual = -1;
};

// model-free 68% half-width: counted around the median, all entries (incl. under/overflow) in the total,
// linear interpolation inside the bins
Double_t counted_halfwidth(TH1F *h)
{
    const Int_t nb = h->GetNbinsX();
    const Double_t tot = h->Integral(0, nb + 1);
    if (tot <= 0)
        return -1.0;
    std::vector<Double_t> edge(nb + 1), cum(nb + 1);
    cum[0] = h->GetBinContent(0);
    for (Int_t b = 0; b <= nb; ++b)
        edge[b] = h->GetBinLowEdge(b + 1);
    for (Int_t b = 1; b <= nb; ++b)
        cum[b] = cum[b - 1] + h->GetBinContent(b);
    auto F = [&](Double_t v) // cumulative count at v
    {
        if (v <= edge[0]) return cum[0];
        if (v >= edge[nb]) return cum[nb];
        const Int_t b = Int_t(std::upper_bound(edge.begin(), edge.end(), v) - edge.begin()) - 1;
        return cum[b] + (cum[b + 1] - cum[b]) * (v - edge[b]) / (edge[b + 1] - edge[b]);
    };
    Double_t lo = edge[0], hi = edge[nb];
    while (hi - lo > HW_TOL) // median
    {
        const Double_t mid = 0.5 * (lo + hi);
        (F(mid) < 0.5 * tot ? lo : hi) = mid;
    }
    const Double_t med = 0.5 * (lo + hi);
    lo = 0.0;
    hi = edge[nb] - edge[0];
    while (hi - lo > HW_TOL)
    {
        const Double_t mid = 0.5 * (lo + hi);
        (F(med + mid) - F(med - mid) < 0.68 * tot ? lo : hi) = mid;
    }
    return 0.5 * (lo + hi);
}

// figure of a fit (data and model), as before
void draw_fit(TH1F *hist, RooRealVar &ip_var, RooDataHist &hdatahist, RooAbsPdf &model, TString period,
              TString sampletype, TString figpath, Float_t reso, TString model_text)
{
    setTDRStyle();
    lumi_sqrtS = period + " (13.6 TeV)";
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
    write_text(0.6, 0.7, sampletype + " fit results (" + model_text + "):");
    write_text(0.6, 0.65, Form("reso = %.*f", std::max(0, 2 - (Int_t)floor(log10(reso))), reso));
    CMS_lumi(canvas);

    canvas->Update();
    canvas->SaveAs(figpath + ".png");
    delete canvas;
}

// Fits hist with a symmetric double-sided Crystal Ball (half-width = measured resolution) and, if s_pv > 0,
// with the same DSCB convolved with a Gaussian of width s_pv (the mean PV error of the tracks): the half-width
// of the DSCB alone is then the resolution with the PV error removed. The convolution fit starts from the DSCB
// fit. The figure shows the DSCB fit.
FitOut fit_res(TH1F *hist, TString period, TString sampletype, TString figpath, Double_t s_pv = -1)
{
    FitOut out;
    Int_t filledBins = 0;
    for (Int_t i = 1; i <= hist->GetNbinsX(); i++)
    {
        if (hist->GetBinContent(i) > 0)
            filledBins++;
    }
    if ((Double_t)filledBins / hist->GetNbinsX() < 0.1)
        return out;

    RooRealVar ip_var("ip_var", "ip_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    ip_var.setBins(hist->GetNbinsX());

    Float_t hist_mean = hist->GetMean();
    Float_t hist_rms = hist->GetRMS();

    RooRealVar mu("mu", "mu", hist_mean, hist_mean - hist_rms, hist_mean + hist_rms);
    RooRealVar sigma("sigma", "sigma", 0.5 * hist_rms, 0.1 * hist_rms, hist_rms);
    RooRealVar alpha("alpha", "alpha", 2.0, 0.5, 5.0);
    // n up to 100: with the old limit 10 the dz fits stopped at n = 10 (optimum 11-17), which biased the
    // half-width low by 0.2-0.6 % and left no valid covariance (2022C, checked 2026-11-13)
    RooRealVar n("n", "n", 2.0, 0.5, 100.0);
    RooCrystalBall model("model", "Double Crystal Ball", ip_var, mu, sigma, alpha, n, alpha, n);

    RooDataHist hdatahist("hdatahist", "", ip_var, hist);
    RooFitResult *fitResult = model.fitTo(hdatahist, RooFit::Save(true));
    out.status = fitResult->status();
    out.covqual = fitResult->covQual();
    if (fitResult->status() != 0)
        std::cerr << "[WARN] fit_res: RooFit did not converge (status=" << fitResult->status() << ") for " << hist->GetName() << std::endl;
    fitResult->Print();

    Float_t reso = halfwidth(model, ip_var, mu.getVal(), HW_TOL);
    out.reso = reso;
    out.err = halfwidth_error(model, ip_var, mu, RooArgList(sigma, alpha, n), fitResult);
    delete fitResult;

    draw_fit(hist, ip_var, hdatahist, model, period, sampletype, figpath, reso, "DSCB");

    // convolution fit, starting from the DSCB fit
    if (s_pv > 0)
    {
        ip_var.setBins(CONV_NCACHE, "cache");
        RooRealVar zero("zero", "zero", 0.0);
        RooRealVar s("s", "s", s_pv, 1e-3, 1e9);
        s.setConstant(true);
        RooGaussian pvgauss("pvgauss", "PV error", ip_var, zero, s);
        RooFFTConvPdf conv("conv", "DSCB (x) PV error", ip_var, model, pvgauss);
        RooFitResult *convResult = conv.fitTo(hdatahist, RooFit::Save(true), RooFit::PrintLevel(-1));
        out.conv_status = convResult->status();
        out.conv_covqual = convResult->covQual();
        if (convResult->status() != 0)
            std::cerr << "[WARN] fit_res: convolution fit status " << convResult->status() << " for " << hist->GetName() << std::endl;
        out.conv = halfwidth(model, ip_var, mu.getVal(), HW_TOL);
        out.conv_err = halfwidth_error(model, ip_var, mu, RooArgList(sigma, alpha, n), convResult);
        delete convResult;
    }
    return out;
}

// The same with a sum of 3 Gaussians with a common mean (widths s1..s3, recursive fractions f1, f2). The
// convolution with the PV-error Gaussian is analytic: each width becomes sqrt(s_i^2 + s_pv^2); the half-width of
// the sum with the widths s_i is then the resolution with the PV error removed. The figure shows the plain fit.
FitOut fit_3g(TH1F *hist, TString period, TString sampletype, TString figpath, Double_t s_pv = -1)
{
    FitOut out;
    Int_t filledBins = 0;
    for (Int_t i = 1; i <= hist->GetNbinsX(); i++)
        if (hist->GetBinContent(i) > 0)
            filledBins++;
    if ((Double_t)filledBins / hist->GetNbinsX() < 0.1)
        return out;

    RooRealVar ip_var("ip_var", "ip_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    ip_var.setBins(hist->GetNbinsX());
    const Double_t m = hist->GetMean(), r = hist->GetRMS();
    RooRealVar mu("mu", "mu", m, m - r, m + r);
    RooRealVar s1("s1", "s1", 0.4 * r, 0.05 * r, 10 * r), s2("s2", "s2", 1.0 * r, 0.05 * r, 10 * r),
        s3("s3", "s3", 2.5 * r, 0.05 * r, 10 * r);
    RooRealVar f1("f1", "f1", 0.6, 0.0, 1.0), f2("f2", "f2", 0.3, 0.0, 1.0);
    RooGaussian g1("g1", "", ip_var, mu, s1), g2("g2", "", ip_var, mu, s2), g3("g3", "", ip_var, mu, s3);
    RooAddPdf model("model3", "3 Gaussians", RooArgList(g1, g2, g3), RooArgList(f1, f2), true);
    // fractions refer to the full observable range, not to the sub-ranges integrated in halfwidth()
    model.fixCoefNormalization(RooArgSet(ip_var));

    RooDataHist hdatahist("hdatahist", "", ip_var, hist);
    RooFitResult *fitResult = model.fitTo(hdatahist, RooFit::Save(true), RooFit::PrintLevel(-1));
    out.status = fitResult->status();
    out.covqual = fitResult->covQual();
    if (fitResult->status() != 0)
        std::cerr << "[WARN] fit_3g: status " << fitResult->status() << " for " << hist->GetName() << std::endl;
    const RooArgList shape(s1, s2, s3, f1, f2);
    out.reso = halfwidth(model, ip_var, mu.getVal(), HW_TOL);
    out.err = halfwidth_error(model, ip_var, mu, shape, fitResult);
    delete fitResult;

    draw_fit(hist, ip_var, hdatahist, model, period, sampletype, figpath, out.reso, "3 Gaussians");

    if (s_pv > 0)
    {
        RooRealVar s("s", "s", s_pv);
        s.setConstant(true);
        // start from the plain fit: s_i -> sqrt(s_i^2 - s_pv^2) (at least the lower limit)
        for (RooRealVar *si : {&s1, &s2, &s3})
            si->setVal(std::max(std::sqrt(std::max(si->getVal() * si->getVal() - s_pv * s_pv, 0.0)), 1.01 * si->getMin()));
        RooFormulaVar w1("w1", "sqrt(@0*@0+@1*@1)", RooArgList(s1, s)), w2("w2", "sqrt(@0*@0+@1*@1)", RooArgList(s2, s)),
            w3("w3", "sqrt(@0*@0+@1*@1)", RooArgList(s3, s));
        RooGaussian c1("c1", "", ip_var, mu, w1), c2("c2", "", ip_var, mu, w2), c3("c3", "", ip_var, mu, w3);
        RooAddPdf conv("conv3", "3 Gaussians (x) PV error", RooArgList(c1, c2, c3), RooArgList(f1, f2), true);
        conv.fixCoefNormalization(RooArgSet(ip_var));
        RooFitResult *convResult = conv.fitTo(hdatahist, RooFit::Save(true), RooFit::PrintLevel(-1));
        out.conv_status = convResult->status();
        out.conv_covqual = convResult->covQual();
        if (convResult->status() != 0)
            std::cerr << "[WARN] fit_3g: convolution fit status " << convResult->status() << " for " << hist->GetName() << std::endl;
        out.conv = halfwidth(model, ip_var, mu.getVal(), HW_TOL);
        out.conv_err = halfwidth_error(model, ip_var, mu, shape, convResult);
        delete convResult;
    }
    return out;
}

Int_t ip_res(TString period, Int_t idx)
{
    TString figdir = storage_dir + "/figures/" + period + "/ip_res/";

    std::ifstream infile(storage_dir + "/json/binning.json");
    nlohmann::json binning;
    infile >> binning;
    infile.close();

    std::vector<Float_t> pv_trk_pt_edges = binning["pv_trk_pt"].get<std::vector<Float_t>>();
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
    resojson["eta"] = (pv_trk_eta_edges[idx] + pv_trk_eta_edges[idx + 1]) / 2.0;
    // |eta| slice edges of the categories pt_eta0..8 (slice s: [edges[s], edges[s+1]))
    resojson["abseta_slices"] = binning["pv_trk_abseta_slices"];
    resojson["phi"] = (pv_trk_phi_edges[idx] + pv_trk_phi_edges[idx + 1]) / 2.0;

    const char *VAR[2] = {"d0", "dz"};
    const char *FIGSUB[2] = {"ippv_xy_fit", "ippv_z_fit"};
    const Double_t GRP_MIN_ENTRIES = 2000;  // PV-error groups with fewer tracks are not fitted

    // Fit one histogram; remove its mean (pull-corrected) PV error^2 by quadrature subtraction and by the
    // convolution fit. Writes (k = <var>_<tag>):
    //   reso_data_k, reso_data_k_err          DSCB 68% half-width (measured) and its stat. error
    //   reso_data_k_pvsub, reso_data_k_pvsub_err  quadrature-subtracted (error: reso/reso_pvsub * err; the error
    //                                         of the PV error is not included)
    //   reso_data_k_conv, reso_data_k_conv_err    convolution fit with s = sqrt(pverr2)
    //   pverr2_data_k, pverr2nopull_data_k, ntrk_data_k
    //   fitstatus_data_k, fitstatus_conv_data_k  RooFit status (0 = converged; -1: too few filled bins; -2: not
    //                                         fitted, no histogram or fewer than min_entries entries)
    //   covqual_data_k, covqual_conv_data_k   Minuit covariance quality (3 = accurate)
    // Errors are -1 when not valid (covQual < 3 or a DSCB parameter at its limit).
    auto measure = [&](const char *var, Int_t v, TString tag, TString figname, Double_t min_entries = 1)
    {
        TH1F *h = (TH1F *)histfile->Get(Form("ip_%s_%d_%s", var, idx, tag.Data()));
        TH2D *e = (TH2D *)histfile->Get(Form("pverr_%s_%d_%s", var, idx, tag.Data()));

        Double_t pverr2 = e ? mean_pverr2(e, kTRUE) : 0.0;
        Double_t pverr2_nopull = e ? mean_pverr2(e, kFALSE) : 0.0;
        Double_t ntrk = 0;
        for (Int_t k = 0; e && k < n_pv; ++k)
            ntrk += e->GetBinContent(k + 1, 7);

        FitOut fo, fo3;
        Int_t status = -2, conv_status = -2, status3 = -2, conv_status3 = -2;
        Double_t counted = -1.0;
        if (h && h->GetEntries() >= min_entries)
        {
            const Double_t s_pv = pverr2 > 0 ? std::sqrt(pverr2) : -1.0;
            fo = fit_res(h, period, "Data", figdir + FIGSUB[v] + "/" + figname, s_pv);
            status = fo.status;
            conv_status = fo.conv_status;
            fo3 = fit_3g(h, period, "Data", figdir + FIGSUB[v] + "_3g/" + figname, s_pv);
            status3 = fo3.status;
            conv_status3 = fo3.conv_status;
            counted = counted_halfwidth(h);
        }
        // as before: 0 if not fitted, -1 if too few filled bins
        const Float_t reso = status == -2 ? 0.0 : fo.reso;

        Double_t reso_sub = -1.0, reso_sub_err = -1.0;
        if (reso > 0 && reso * reso > pverr2)
        {
            reso_sub = std::sqrt(reso * reso - pverr2);
            if (fo.err > 0)
                reso_sub_err = reso / reso_sub * fo.err;
        }
        else if (reso > 0)
            std::cerr << "[WARN] ip_res: PV error^2 " << pverr2 << " >= measured reso^2 " << reso * reso
                      << " for " << var << " " << tag << " bin " << idx << std::endl;

        const std::string key = std::string(var) + "_" + tag.Data();
        resojson["reso_data_" + key] = reso;
        resojson["reso_data_" + key + "_err"] = fo.err;
        resojson["reso_data_" + key + "_pvsub"] = reso_sub;
        resojson["reso_data_" + key + "_pvsub_err"] = reso_sub_err;
        resojson["reso_data_" + key + "_conv"] = fo.conv;
        resojson["reso_data_" + key + "_conv_err"] = fo.conv_err;
        resojson["pverr2_data_" + key] = pverr2;
        resojson["pverr2nopull_data_" + key] = pverr2_nopull;
        resojson["ntrk_data_" + key] = ntrk;
        resojson["fitstatus_data_" + key] = status;
        resojson["fitstatus_conv_data_" + key] = conv_status;
        resojson["covqual_data_" + key] = fo.covqual;
        resojson["covqual_conv_data_" + key] = fo.conv_covqual;

        // the same with the sum of 3 Gaussians (suffix _3g, not _g3: that is PV-error group 3; status keys fitstatus_3g_, fitstatus_3gconv_, covqual_3g_,
        // covqual_3gconv_), and the model-free counted half-width of the measured distribution
        const Float_t reso3 = status3 == -2 ? 0.0 : fo3.reso;
        Double_t reso3_sub = -1.0, reso3_sub_err = -1.0;
        if (reso3 > 0 && reso3 * reso3 > pverr2)
        {
            reso3_sub = std::sqrt(reso3 * reso3 - pverr2);
            if (fo3.err > 0)
                reso3_sub_err = reso3 / reso3_sub * fo3.err;
        }
        resojson["reso_data_" + key + "_3g"] = reso3;
        resojson["reso_data_" + key + "_3g_err"] = fo3.err;
        resojson["reso_data_" + key + "_3g_pvsub"] = reso3_sub;
        resojson["reso_data_" + key + "_3g_pvsub_err"] = reso3_sub_err;
        resojson["reso_data_" + key + "_3g_conv"] = fo3.conv;
        resojson["reso_data_" + key + "_3g_conv_err"] = fo3.conv_err;
        resojson["fitstatus_3g_data_" + key] = status3;
        resojson["fitstatus_3gconv_data_" + key] = conv_status3;
        resojson["covqual_3g_data_" + key] = fo3.covqual;
        resojson["covqual_3gconv_data_" + key] = fo3.conv_covqual;
        resojson["reso_data_" + key + "_counted"] = counted;
    };

    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        TString suffix = COMB_SUFFIX[cid];
        for (Int_t v = 0; v < 2; ++v)
        {
            // all tracks of the bin
            measure(VAR[v], v, suffix, Form("data_%s_%d", suffix.Data(), idx));

            // the same bin split into groups of the track's own PV error (edges: with pull, lower and upper
            // included, from fill_hists.cc; n groups = n edges - 1)
            TH1D *edges = (TH1D *)histfile->Get(Form("grpedges_%s_%d_%s", VAR[v], idx, suffix.Data()));
            const Int_t ngroups = edges ? edges->GetNbinsX() - 1 : 0;
            resojson[Form("ngroups_data_%s_%s", VAR[v], suffix.Data())] = ngroups;
            if (!edges)
            {
                std::cerr << "[WARN] ip_res: no PV-error groups for " << VAR[v] << " " << suffix << " bin " << idx << std::endl;
                continue;
            }
            std::vector<Double_t> ev;
            for (Int_t j = 1; j <= edges->GetNbinsX(); ++j)
                ev.push_back(edges->GetBinContent(j));
            resojson[Form("sigpv_edges_%s_%s", VAR[v], suffix.Data())] = ev;
            for (Int_t g = 0; g < ngroups; ++g)
                measure(VAR[v], v, suffix + Form("_g%d", g), Form("data_%s_g%d_%d", suffix.Data(), g, idx), GRP_MIN_ENTRIES);
        }
    }

    std::ofstream outFile(storage_dir + "/json/" + period + Form("/ip_res/fit_%d.json", idx));
    outFile << resojson.dump(4);
    outFile.close();

    histfile->Close();
    return 0;
}
