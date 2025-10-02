#include "tdrStyle.cc"
#include "CMS_lumi.cc"
#include "draw_funcs.cc"

std::vector<float> fit_compare(TH1F * hist_data, TH1F * hist_mc, TString era, TString figpath_data, TString figpath_mc, float tolerance = 1e-4) {

    setTDRStyle();

    std::vector<float> reso_results;

    lumi_sqrtS = "13.6 TeV, 2022 " + era;

    RooRealVar pv_var_data("pv_var_data", "pv_var_data", hist_data->GetXaxis()->GetXmin(), hist_data->GetXaxis()->GetXmax());
    pv_var_data.setBins(hist_data->GetNbinsX());
    RooRealVar mu_data("mu_data", "mu_data", hist_data->GetMean(), hist_data->GetXaxis()->GetXmin(), hist_data->GetXaxis()->GetXmax());
    RooRealVar sigma_data("sigma_data", "sigma_data", 0.5*hist_data->GetRMS(), 0., hist_data->GetRMS());
    RooGaussian gauss_data("gauss_data", "gauss_data", pv_var_data, mu_data, sigma_data);

    RooDataHist hdatahist_data("hdatahist_data", "", pv_var_data, hist_data);
    RooFitResult *fitResult_data = gauss_data.fitTo(hdatahist_data, RooFit::Save(true));
    fitResult_data->Print();

    reso_results.push_back(sigma_data.getVal());
    reso_results.push_back(sigma_data.getError());

    RooRealVar pv_var_mc("pv_var_mc", "pv_var_mc", hist_mc->GetXaxis()->GetXmin(), hist_mc->GetXaxis()->GetXmax());
    pv_var_mc.setBins(hist_mc->GetNbinsX());
    RooRealVar mu_mc("mu_mc", "mu_mc", hist_mc->GetMean(), hist_mc->GetXaxis()->GetXmin(), hist_mc->GetXaxis()->GetXmax());
    RooRealVar sigma_mc("sigma_mc", "sigma_mc", 0.5*hist_mc->GetRMS(), 0., hist_mc->GetRMS());
    RooGaussian gauss_mc("gauss_mc", "gauss_mc", pv_var_mc, mu_mc, sigma_mc);

    RooDataHist hdatahist_mc("hdatahist_mc", "", pv_var_mc, hist_mc);
    RooFitResult *fitResult_mc = gauss_mc.fitTo(hdatahist_mc, RooFit::Save(true));
    fitResult_mc->Print();
    
    reso_results.push_back(sigma_mc.getVal());
    reso_results.push_back(sigma_mc.getError());

    TCanvas *canvas_data = new TCanvas("canvas_data", "canvas_data", 800, 600);
    canvas_setup(canvas_data);
    canvas_data->SetBottomMargin(0.15);
    canvas_data->SetRightMargin(0.05);
    canvas_data->SetLogy(0);
    canvas_data->SetFillColor(0);
    canvas_data->SetFrameFillColor(0);
    RooPlot *frame_data = pv_var_data.frame();

    hdatahist_data.plotOn(frame_data, RooFit::Name("data"), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1.1), RooFit::Binning(hist_data->GetNbinsX()), RooFit::DrawOption("ep"));
    gauss_data.plotOn(frame_data, RooFit::Name("gauss_data"), RooFit::Components("gauss_data"), RooFit::LineStyle(9), RooFit::LineColor(kRed), RooFit::LineWidth(2.0), RooFit::DrawOption("L"));
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
    write_text(0.6, 0.65, Form("reso = %.*f $pm %.*f", std::max(0, 2-(int)floor(log10(sigma_data.getVal()))), sigma_data.getVal(), std::max(0, 2-(int)floor(log10(sigma_data.getError()))), sigma_data.getError()));
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
    gauss_mc.plotOn(frame_mc, RooFit::Name("gauss_mc"), RooFit::Components("gauss_mc"), RooFit::LineStyle(9), RooFit::LineColor(kRed), RooFit::LineWidth(2.0), RooFit::DrawOption("L"));
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
    write_text(0.6, 0.65, Form("reso = %.*f $pm %.*f", std::max(0, 2-(int)floor(log10(sigma_mc.getVal()))), sigma_mc.getVal(), std::max(0, 2-(int)floor(log10(sigma_mc.getError()))), sigma_mc.getError()));
    CMS_lumi(canvas_mc);

    canvas_mc->Update();
    canvas_mc->SaveAs(figpath_mc+".png");

    return reso_results;
}
