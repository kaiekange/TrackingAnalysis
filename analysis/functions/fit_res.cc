#include "tdrStyle.cc"
#include "CMS_lumi.cc"
#include "draw_funcs.cc"

std::pair<float, float> fit_compare(TH1F * hist_data, TH1F * hist_mc, TString era, TString figpath_data, TString figpath_mc, float tolerance = 1e-4) {

    setTDRStyle();

    lumi_sqrtS = "13.6 TeV, 2022 " + era;

    RooRealVar pv_var_data("pv_var_data", "pv_var_data", hist_data->GetXaxis()->GetXmin(), hist_data->GetXaxis()->GetXmax());
    pv_var_data.setBins(hist_data->GetNbinsX());
    RooRealVar mu_data("mu_data", "mu_data", hist_data->GetMean(), hist_data->GetXaxis()->GetXmin(), hist_data->GetXaxis()->GetXmax());
    RooRealVar sigma1_data("sigma1_data", "sigma1_data", 0.5*hist_data->GetRMS(), 0., hist_data->GetRMS());
    RooRealVar sigma2_data("sigma2_data", "sigma2_data", hist_data->GetRMS(), hist_data->GetRMS()*0.5, hist_data->GetRMS()*2);
    RooRealVar sigma3_data("sigma3_data", "sigma3_data", 1.5*hist_data->GetRMS(), hist_data->GetRMS(), hist_data->GetRMS()*3);
    RooGaussian gauss1_data("gauss1_data", "gauss1_data", pv_var_data, mu_data, sigma1_data);
    RooGaussian gauss2_data("gauss2_data", "gauss2_data", pv_var_data, mu_data, sigma2_data);
    RooGaussian gauss3_data("gauss3_data", "gauss3_data", pv_var_data, mu_data, sigma3_data);
    RooRealVar f1_data("f1_data", "f1_data", 0.3, 0.0, 1.0);
    RooRealVar f2_data("f2_data", "f2_data", 0.3, 0.0, 1.0);
    RooFormulaVar f3_data("f3_data", "1 - f1_data - f2_data", RooArgList(f1_data, f2_data));
    RooAddPdf triGauss_data("triGauss_data", "triGauss_data", RooArgList(gauss1_data, gauss2_data, gauss3_data), RooArgList(f1_data, f2_data, f3_data));

    RooDataHist hdatahist_data("hdatahist_data", "", pv_var_data, hist_data);
    RooFitResult *fitResult_data = triGauss_data.fitTo(hdatahist_data, RooFit::Save(true));
    fitResult_data->Print();

    RooRealVar pv_var_mc("pv_var_mc", "pv_var_mc", hist_mc->GetXaxis()->GetXmin(), hist_mc->GetXaxis()->GetXmax());
    pv_var_mc.setBins(hist_mc->GetNbinsX());
    RooRealVar mu_mc("mu_mc", "mu_mc", hist_mc->GetMean(), hist_mc->GetXaxis()->GetXmin(), hist_mc->GetXaxis()->GetXmax());
    RooRealVar sigma1_mc("sigma1_mc", "sigma1_mc", 0.5*hist_mc->GetRMS(), 0., hist_mc->GetRMS());
    RooRealVar sigma2_mc("sigma2_mc", "sigma2_mc", hist_mc->GetRMS(), hist_mc->GetRMS()*0.5, hist_mc->GetRMS()*2);
    RooRealVar sigma3_mc("sigma3_mc", "sigma3_mc", 1.5*hist_mc->GetRMS(), hist_mc->GetRMS(), hist_mc->GetRMS()*3);
    RooGaussian gauss1_mc("gauss1_mc", "gauss1_mc", pv_var_mc, mu_mc, sigma1_mc);
    RooGaussian gauss2_mc("gauss2_mc", "gauss2_mc", pv_var_mc, mu_mc, sigma2_mc);
    RooGaussian gauss3_mc("gauss3_mc", "gauss3_mc", pv_var_mc, mu_mc, sigma3_mc);
    RooRealVar f1_mc("f1_mc", "f1_mc", 0.3, 0.0, 1.0);
    RooRealVar f2_mc("f2_mc", "f2_mc", 0.3, 0.0, 1.0);
    RooFormulaVar f3_mc("f3_mc", "1 - f1_mc - f2_mc", RooArgList(f1_mc, f2_mc));
    RooAddPdf triGauss_mc("triGauss_mc", "triGauss_mc", RooArgList(gauss1_mc, gauss2_mc, gauss3_mc), RooArgList(f1_mc, f2_mc, f3_mc));

    RooDataHist hdatahist_mc("hdatahist_mc", "", pv_var_mc, hist_mc);
    RooFitResult *fitResult_mc = triGauss_mc.fitTo(hdatahist_mc, RooFit::Save(true));
    fitResult_mc->Print();

    float pv_var_data_max = pv_var_data.getMax();
    float mean_data = mu_data.getVal();
    float low_data = 0.0;
    float high_data = pv_var_data_max - mean_data;
    while (high_data - low_data > tolerance) {
        float mid_data = 0.5 * (low_data + high_data);
        pv_var_data.setRange("intRange", mean_data - mid_data, mean_data + mid_data);
        RooAbsReal* integral_data = triGauss_data.createIntegral(pv_var_data, RooFit::NormSet(pv_var_data), RooFit::Range("intRange"));
        float prob_data = integral_data->getVal();
        if (prob_data < 0.68) low_data = mid_data;
        else high_data = mid_data;
        delete integral_data;
    }
    float reso_data = 0.5 * (low_data + high_data);

    float pv_var_mc_max = pv_var_mc.getMax();
    float mean_mc = mu_mc.getVal();
    float low_mc = 0.0;
    float high_mc =pv_var_mc_max - mean_mc;
    while (high_mc - low_mc > tolerance) {
        float mid_mc = 0.5 * (low_mc + high_mc);
        pv_var_mc.setRange("intRange", mean_mc - mid_mc, mean_mc + mid_mc);
        RooAbsReal* integral_mc = triGauss_mc.createIntegral(pv_var_mc, RooFit::NormSet(pv_var_mc), RooFit::Range("intRange"));
        float prob_mc = integral_mc->getVal();
        if (prob_mc < 0.68) low_mc = mid_mc;
        else high_mc = mid_mc;
        delete integral_mc;
    }
    float reso_mc = 0.5 * (low_mc + high_mc);

    TCanvas *canvas_data = new TCanvas("canvas_data", "canvas_data", 800, 600);
    canvas_setup(canvas_data);
    canvas_data->SetBottomMargin(0.15);
    canvas_data->SetRightMargin(0.05);
    canvas_data->SetLogy(0);
    canvas_data->SetFillColor(0);
    canvas_data->SetFrameFillColor(0);
    RooPlot *frame_data = pv_var_data.frame();

    hdatahist_data.plotOn(frame_data, RooFit::Name("data"), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1.1), RooFit::Binning(hist_data->GetNbinsX()), RooFit::DrawOption("ep"));
    triGauss_data.plotOn(frame_data, RooFit::Name("triGauss_data"), RooFit::Components("triGauss_data"), RooFit::LineStyle(9), RooFit::LineColor(kRed), RooFit::LineWidth(2.0), RooFit::DrawOption("L"));
    frame_data->Draw("");
    frame_data->GetYaxis()->SetTitle(hist_data->GetYaxis()->GetTitle());
    frame_data->GetXaxis()->SetTitle(hist_data->GetXaxis()->GetTitle());
    frame_data->GetYaxis()->SetNdivisions(810);
    frame_data->GetXaxis()->SetNdivisions(810);
    frame_data->SetMinimum(0);
    frame_data->SetMaximum(frame_data->GetMaximum()*1.3);
    write_text(0.6, 0.88, datatype_text);
    write_text(0.6, 0.8, hist_data->GetTitle());
    write_text(0.6, 0.7, Form("Data fit results:"));
    write_text(0.6, 0.65, Form("reso = %.*f", std::max(0, 2-(int)floor(log10(reso_data))), reso_data));
    CMS_lumi(canvas_data);

    canvas_data->Update();
    canvas_data->SaveAs(figpath_data+".png");

    TCanvas *canvas_mc = new TCanvas("canvas_mc", "canvas_mc", 800, 600);
    canvas_setup(canvas_mc);
    canvas_mc->SetBottomMargin(0.15);
    canvas_mc->SetRightMargin(0.05);
    canvas_mc->SetLogy(0);
    canvas_mc->SetFillColor(0);
    canvas_mc->SetFrameFillColor(0);
    RooPlot *frame_mc = pv_var_mc.frame();

    hdatahist_mc.plotOn(frame_mc, RooFit::Name("mc"), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1.1), RooFit::Binning(hist_mc->GetNbinsX()), RooFit::DrawOption("ep"));
    triGauss_mc.plotOn(frame_mc, RooFit::Name("triGauss_mc"), RooFit::Components("triGauss_mc"), RooFit::LineStyle(9), RooFit::LineColor(kRed), RooFit::LineWidth(2.0), RooFit::DrawOption("L"));
    frame_mc->Draw("");
    frame_mc->GetYaxis()->SetTitle(hist_mc->GetYaxis()->GetTitle());
    frame_mc->GetXaxis()->SetTitle(hist_mc->GetXaxis()->GetTitle());
    frame_mc->GetYaxis()->SetNdivisions(810);
    frame_mc->GetXaxis()->SetNdivisions(810);
    frame_mc->SetMinimum(0);
    frame_mc->SetMaximum(frame_mc->GetMaximum()*1.3);
    write_text(0.6, 0.88, datatype_text);
    write_text(0.6, 0.8, hist_mc->GetTitle());
    write_text(0.6, 0.7, Form("MC fit results:"));
    write_text(0.6, 0.65, Form("reso = %.*f", std::max(0, 2-(int)floor(log10(reso_mc))), reso_mc));
    CMS_lumi(canvas_mc);

    canvas_mc->Update();
    canvas_mc->SaveAs(figpath_mc+".png");

    return {reso_data, reso_mc};
}
