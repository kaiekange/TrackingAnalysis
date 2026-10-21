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

const TString datatype_text = "Multi-jet events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";

const Int_t sample_mod = 1;


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

Float_t fit_res(TH1F *hist, TString period, TString sampletype, TString figpath, Float_t tolerance = 1e-4)
{
    Int_t filledBins = 0;
    for (Int_t i = 1; i <= hist->GetNbinsX(); i++)
        if (hist->GetBinContent(i) > 0) filledBins++;
    if ((Double_t)filledBins / hist->GetNbinsX() < 0.1)
        return -1.f;

    setTDRStyle();
    lumi_sqrtS = period + " (13.6 TeV)";

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
    if (fitResult->status() != 0)
        std::cerr << "[WARN] fit_res: RooFit did not converge (status=" << fitResult->status() << ") for " << hist->GetName() << std::endl;
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

Int_t pv_res(TString period, Int_t idx)
{
    TString figdir = storage_dir + "/figures/" + period + "/pv_res/";

    std::ifstream infile(storage_dir + "/json/binning.json");
    nlohmann::json binning;
    infile >> binning;
    infile.close();
    std::vector<Float_t> pv_SumTrackPt2_sqrt_edges = binning["pv_SumTrackPt2_sqrt"].get<std::vector<Float_t>>();

    // histograms filled by fill_hists.cc
    TFile *histfile = TFile::Open(storage_dir + "/hists/" + period + "/fill.root");
    if (!histfile || histfile->IsZombie())
    {
        std::cerr << "[ERROR] pv_res: cannot open " << storage_dir + "/hists/" + period + "/fill.root" << std::endl;
        return 1;
    }

    const TString figsub[NCOMB] = {"pvx_fit", "pvy_fit", "pvz_fit", "pullx_fit", "pully_fit", "pullz_fit"};

    nlohmann::json resojson;
    resojson["pv_SumTrackPt2_sqrt"] = (pv_SumTrackPt2_sqrt_edges[idx] + pv_SumTrackPt2_sqrt_edges[idx + 1]) / 2.0;

    for (Int_t cid = 0; cid < NCOMB; ++cid)
    {
        TString suffix = COMB_SUFFIX[cid];
        TH1F *h = (TH1F *)histfile->Get(Form("pv_%d_%s", idx, suffix.Data()));
        Float_t reso = 0.0;
        if (h && h->GetEntries() > 0)
            reso = fit_res(h, period, "Data", figdir + figsub[cid] + Form("/data_pt_%d", idx));
        resojson[Form("reso_data_%s", suffix.Data())] = reso;
    }

    std::ofstream outFile(storage_dir + "/json/" + period + Form("/pv_res/fit_%d.json", idx));
    outFile << resojson.dump(4);
    outFile.close();

    histfile->Close();
    return 0;
}
