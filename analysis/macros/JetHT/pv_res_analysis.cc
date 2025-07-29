#include <vector>
#include <iostream>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooAddition.h>
#include <RooFitResult.h>
#include <RooCBShape.h>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"
#include "input_list.cc"

const TString figdir = "../../figures/"+datatype+"/pv_res_analysis/";
const int nbins = 20;

std::pair<float, float> fit_draw(TH1F* hist, TString figpath) {

    lumi_sqrtS = "13.6 TeV, 2022"+all_era;

    RooRealVar pv_var("pv_var", "pv_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar mu("mu", "mu", hist->GetMean(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar sigma1("sigma1", "sigma1", 0.5*hist->GetRMS(), 0., hist->GetRMS());
    RooRealVar sigma2("sigma2", "sigma2", hist->GetRMS(), hist->GetRMS()*0.5, hist->GetRMS()*2);
    RooRealVar sigma3("sigma3", "sigma3", 1.5*hist->GetRMS(), hist->GetRMS(), hist->GetRMS()*3);
    RooGaussian gauss1("gauss1", "gauss1", pv_var, mu, sigma1);
    RooGaussian gauss2("gauss2", "gauss2", pv_var, mu, sigma2);
    RooGaussian gauss3("gauss3", "gauss3", pv_var, mu, sigma3);
    RooRealVar f1("f1", "f1", 0.5, 0.0, 1.0);
    RooRealVar f2("f2", "f2", 0.5, 0.0, 1.0);
    RooAddPdf triGauss("triGauss", "triGauss", RooArgList(gauss1, gauss2, gauss3), RooArgList(f1, f2));

    RooDataHist hdatahist("hdatahist", "", pv_var, hist);
    RooFitResult *fitResult = triGauss.fitTo(hdatahist, RooFit::Save(true));
    fitResult->Print();

    /* float mean = mu.getVal(); */
    /* int meanbin = hist->FindBin(mean); */

    /* float nentries = hist->GetEntries(); */
    /* int nreso = 0; */
    /* int sumentries = 0; */

    /* while(sumentries < nentries*0.68) { */
    /*     nreso++; */
    /*     sumentries = 0; */
    /*     for(int bin = meanbin - nreso; bin <= meanbin + nreso; bin++){ */
    /*         sumentries += hist->GetBinContent(bin); */
    /*     } */ 
    /* } */
    /* float reso = (nreso - 0.5) * hist->GetBinWidth(1); */

    float mean = mu.getVal();
    float tolerance = 1e-4;
    float low = 0.0;
    float high = pv_var.getMax() - mean;
    while (high - low > tolerance) {
        float mid = 0.5 * (low + high);
        pv_var.setRange("intRange", mean - mid, mean + mid);
        RooAbsReal* integral = triGauss.createIntegral(pv_var, RooFit::NormSet(pv_var), RooFit::Range("intRange"));
        float prob = integral->getVal();
        if (prob < 0.68) low = mid;
        else high = mid;
        delete integral;
    }
    float reso = 0.5 * (low + high);


    /* int nnbins = hist->GetNbinsX(); */
    /* float max = hist->GetMaximum(); */
    /* float half_max = max / 2.0; */

    /* int bin1 = -1, bin2 = -1; */
    /* float reso2 = -1; */ 
    /* for (int i = 1; i <= nnbins; ++i) { */
    /*     float y = hist->GetBinContent(i); */
    /*     if (y >= half_max) { */
    /*         bin1 = i; */
    /*         break; */
    /*     } */
    /* } */

    /* for (int i = nnbins; i >= 1; --i) { */
    /*     float y = hist->GetBinContent(i); */
    /*     if (y >= half_max) { */
    /*         bin2 = i; */
    /*         break; */
    /*     } */
    /* } */

    /* if (bin1 != -1 && bin2 != -1) { */
    /*     float x1 = hist->GetBinCenter(bin1); */
    /*     float x2 = hist->GetBinCenter(bin2); */
    /*     reso2 = (x2 - x1)/2.36; */
    /* } */

    float xmin = pv_var.getMin();
    float xmax = pv_var.getMax();
    float maxVal = 0.0;
    float xAtMax = 0.0;
    for (float val = xmin; val <= xmax; val += tolerance) {
        pv_var.setVal(val);
        float y = triGauss.getVal(pv_var);
        if (y > maxVal) {
            maxVal = y;
            xAtMax = val;
        }
    }
    float halfMax = maxVal / 2.0;
    float left = xmin, right = xmax;
    for (float val = xAtMax; val >= xmin; val -= tolerance) {
        pv_var.setVal(val);
        if (triGauss.getVal(pv_var) < halfMax) {
            left = val;
            break;
        }
    }
    for (float val = xAtMax; val <= xmax; val += tolerance) {
        pv_var.setVal(val);
        if (triGauss.getVal(pv_var) < halfMax) {
            right = val;
            break;
        }
    }

    float reso2 = (right - left) / 2.36;

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 700);
    canvas->Divide(1,2);
    canvas->cd(1);
    TVirtualPad* c1 = canvas->GetPad(1);
    canvas_setup_sub(c1);
    c1->SetPad(0,0.3,1,1);
    c1->SetBottomMargin(0.0);
    c1->SetRightMargin(0.05);
    c1->SetLogy(0);
    c1->SetFillColor(0);
    c1->SetFrameFillColor(0);
    RooPlot *frame = pv_var.frame();
    hdatahist.plotOn(frame, RooFit::Name("data"), RooFit::MarkerColor(kBlack), RooFit::MarkerSize(1.1), RooFit::Binning(100), RooFit::DrawOption("ep"));
    triGauss.plotOn(frame, RooFit::Name("triGauss"), RooFit::Components("triGauss"), RooFit::LineStyle(9), RooFit::LineColor(kBlue), RooFit::LineWidth(2.0), RooFit::DrawOption("L"));
    frame->Draw("");
    frame->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    frame->SetMinimum(0.01);
    frame->SetMaximum(frame->GetMaximum()*1.3);
    TLegend *legend = new TLegend(0.2, 0.5, 0.4, 0.7, NULL, "brNDC");
    legend->AddEntry(frame->findObject("data"), "data", "ep");
    legend->AddEntry(frame->findObject("triGauss"), "triple Gaussian", "l");
    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    legend->SetLineWidth(0);
    legend->SetTextSize(0.03);
    legend->Draw();
    write_text(0.68, 0.88, datatype_text);
    write_text(0.68, 0.8, hist->GetTitle());
    write_text(0.68, 0.7, Form("Fit Results:"));
    write_text(0.68, 0.65, Form("#mu = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(mu.getVal())))), mu.getVal(), std::max(0, 2-(int)floor(log10(abs(mu.getVal())))), mu.getError()));
    write_text(0.68, 0.6, Form("#sigma_{1} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma1.getVal())))), sigma1.getVal(), std::max(0, 2-(int)floor(log10(abs(sigma1.getVal())))), sigma1.getError()));
    write_text(0.68, 0.55, Form("#sigma_{2} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma2.getVal())))), sigma2.getVal(), std::max(0, 2-(int)floor(log10(abs(sigma2.getVal())))), sigma2.getError()));
    write_text(0.68, 0.5, Form("#sigma_{3} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma3.getVal())))), sigma3.getVal(), std::max(0, 2-(int)floor(log10(abs(sigma3.getVal())))), sigma3.getError()));
    write_text(0.68, 0.45, Form("f_{1} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(f1.getVal())))), f1.getVal(), std::max(0, 2-(int)floor(log10(abs(f1.getVal())))), f1.getError()));
    write_text(0.68, 0.4, Form("f_{2} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(f2.getVal())))), f2.getVal(), std::max(0, 2-(int)floor(log10(abs(f2.getVal())))), f2.getError()));
    write_text(0.68, 0.35, Form("reso = %.*f", std::max(0, 2-(int)floor(log10(abs(reso)))), reso));
    write_text(0.68, 0.3, Form("FWHM/2.36 = %.*f", std::max(0, 2-(int)floor(log10(abs(reso2)))), reso2));
    CMS_lumi_sub(c1);

    canvas->cd(2);
    TVirtualPad* c2 = canvas->GetPad(2);
    c2->SetPad(0,0,1,0.3);
    c2->SetRightMargin(0.05);
    c2->SetTopMargin(0.05);
    c2->SetBottomMargin(0.4);
    RooHist* hpull = frame->pullHist("data","triGauss");
    RooPlot* pull = pv_var.frame();
    hpull->SetFillColor(1);
    pull->addPlotable(hpull, "BX");
    pull->Draw();
    TAxis* xpull = pull->GetXaxis();
    TAxis* ypull = pull->GetYaxis();
    xpull->SetTitleOffset(3.2);
    xpull->SetTickLength(0.06);
    ypull->SetNdivisions(504);
    pull->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    pull->GetXaxis()->SetTitleOffset(1.);
    pull->SetYTitle("pull");
    pull->GetYaxis()->CenterTitle(true);
    pull->GetYaxis()->SetRangeUser(-5, 5);
    pull->GetYaxis()->SetTitleOffset(0.3);
    pull->GetXaxis()->SetTitleSize(0.1);
    pull->GetXaxis()->SetLabelSize(0.12);
    pull->GetYaxis()->SetTitleSize(0.12);
    pull->GetYaxis()->SetLabelSize(0.12);
    canvas->Update();
    canvas->SaveAs(figpath+".png");

    return {reso, reso2};
}

int pv_res_analysis(int idx) {

    /* if(gSystem->AccessPathName("./figures")) gSystem->MakeDirectory("./figures"); */
    /* if(gSystem->AccessPathName("./figures/"+datatype)) gSystem->MakeDirectory("./figures/"+datatype); */
    /* if(gSystem->AccessPathName(figdir)) gSystem->MakeDirectory(figdir); */
    /* if(gSystem->AccessPathName(figdir+"pvx_fit")) gSystem->MakeDirectory(figdir+"pvx_fit"); */
    /* if(gSystem->AccessPathName(figdir+"pvy_fit")) gSystem->MakeDirectory(figdir+"pvy_fit"); */
    /* if(gSystem->AccessPathName(figdir+"pvz_fit")) gSystem->MakeDirectory(figdir+"pvz_fit"); */
    /* if(gSystem->AccessPathName(figdir+"pullx_fit")) gSystem->MakeDirectory(figdir+"pullx_fit"); */
    /* if(gSystem->AccessPathName(figdir+"pully_fit")) gSystem->MakeDirectory(figdir+"pully_fit"); */
    /* if(gSystem->AccessPathName(figdir+"pullz_fit")) gSystem->MakeDirectory(figdir+"pullz_fit"); */
    if(gSystem->AccessPathName("../../json/"+datatype+"/pv_res")) gSystem->MakeDirectory("../../json/"+datatype+"/pv_res");

    setTDRStyle();

    std::cout << anaroot << std::endl;
    TFile *myfile = TFile::Open(anaroot);
    TTree *mytree = (TTree*)myfile->Get("mytree");

    std::ifstream infile("../../json/"+datatype+"/binning.json");
    nlohmann::json binning;
    infile >> binning;

    std::vector<float> pv_SumTrackPt2_sqrt_edges = binning["pv_sumTrackPt2_sqrt"].get<std::vector<float>>();

    TString ptcut_title = Form("%.2f<#sqrt{#sum#it{p_{T}}^{2}}<%.2f GeV", pv_SumTrackPt2_sqrt_edges[idx], pv_SumTrackPt2_sqrt_edges[idx+1]);
    TCut ptcut = Form("sqrt(pv_SumTrackPt2) > %f && sqrt(pv_SumTrackPt2) < %f", pv_SumTrackPt2_sqrt_edges[idx], pv_SumTrackPt2_sqrt_edges[idx+1]);
    TCut xnull_cut = "pv_x_p1 != -777 && pv_x_p2 != -777";
    TCut ynull_cut = "pv_y_p1 != -777 && pv_y_p2 != -777";
    TCut znull_cut = "pv_z_p1 != -777 && pv_z_p2 != -777";

    TH1F * h_diff_x_tmp = new TH1F("h_diff_x_tmp", "", 200, -100, 100); 
    TH1F * h_diff_y_tmp = new TH1F("h_diff_y_tmp", "", 200, -100, 100); 
    TH1F * h_diff_z_tmp = new TH1F("h_diff_z_tmp", "", 200, -150, 150); 
    TH1F * h_pull_x_tmp = new TH1F("h_pull_x_tmp", "", 200, -40, 40); 
    TH1F * h_pull_y_tmp = new TH1F("h_pull_y_tmp", "", 200, -40, 40); 
    TH1F * h_pull_z_tmp = new TH1F("h_pull_z_tmp", "", 200, -40, 40); 

    mytree->Project("h_diff_x_tmp", "(pv_x_p1 - pv_x_p2)/sqrt(2)", ptcut+xnull_cut);
    mytree->Project("h_diff_y_tmp", "(pv_y_p1 - pv_y_p2)/sqrt(2)", ptcut+ynull_cut);
    mytree->Project("h_diff_z_tmp", "(pv_z_p1 - pv_z_p2)/sqrt(2)", ptcut+znull_cut);
    mytree->Project("h_pull_x_tmp", "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))", ptcut+xnull_cut);
    mytree->Project("h_pull_y_tmp", "(pv_y_p1 - pv_y_p2)/sqrt(pow(pv_yError_p1,2)+pow(pv_yError_p2,2))", ptcut+ynull_cut);
    mytree->Project("h_pull_z_tmp", "(pv_z_p1 - pv_z_p2)/sqrt(pow(pv_zError_p1,2)+pow(pv_zError_p2,2))", ptcut+znull_cut);

    float diff_x_mean = h_diff_x_tmp->GetMean();
    float diff_y_mean = h_diff_y_tmp->GetMean();
    float diff_z_mean = h_diff_z_tmp->GetMean();
    float pull_x_mean = h_pull_x_tmp->GetMean();
    float pull_y_mean = h_pull_y_tmp->GetMean();
    float pull_z_mean = h_pull_z_tmp->GetMean();

    float diff_x_stddev = h_diff_x_tmp->GetStdDev();
    float diff_y_stddev = h_diff_y_tmp->GetStdDev();
    float diff_z_stddev = h_diff_z_tmp->GetStdDev();
    float pull_x_stddev = h_pull_x_tmp->GetStdDev();
    float pull_y_stddev = h_pull_y_tmp->GetStdDev();
    float pull_z_stddev = h_pull_z_tmp->GetStdDev();

    TH1F *h_diff_x = new TH1F("h_diff_x", ptcut_title+";(#it{x}_{1}-#it{x}_{2})/#sqrt{2} [#mum];# PV", 200, diff_x_mean-8*diff_x_stddev, diff_x_mean+8*diff_x_stddev);
    TH1F *h_diff_y = new TH1F("h_diff_y", ptcut_title+";(#it{y}_{1}-#it{y}_{2})/#sqrt{2} [#mum];# PV", 200, diff_y_mean-8*diff_y_stddev, diff_y_mean+8*diff_y_stddev);
    TH1F *h_diff_z = new TH1F("h_diff_z", ptcut_title+";(#it{z}_{1}-#it{z}_{2})/#sqrt{2} [mm];# PV", 200, diff_z_mean-8*diff_z_stddev, diff_z_mean+8*diff_z_stddev);
    TH1F *h_pull_x = new TH1F("h_pull_x", ptcut_title+";(#it{x}_{1}-#it{x}_{2})/#sqrt{#Delta#it{x}_{1}^{2}+#Delta#it{x}_{2}^{2}};# PV", 200, pull_x_mean-8*pull_x_stddev, pull_x_mean+8*pull_x_stddev);
    TH1F *h_pull_y = new TH1F("h_pull_y", ptcut_title+";(#it{y}_{1}-#it{y}_{2})/#sqrt{#Delta#it{y}_{1}^{2}+#Delta#it{y}_{2}^{2}};# PV", 200, pull_y_mean-8*pull_y_stddev, pull_y_mean+8*pull_y_stddev);
    TH1F *h_pull_z = new TH1F("h_pull_z", ptcut_title+";(#it{z}_{1}-#it{z}_{2})/#sqrt{#Delta#it{z}_{1}^{2}+#Delta#it{z}_{2}^{2}};# PV", 200, pull_z_mean-8*pull_z_stddev, pull_z_mean+8*pull_z_stddev);

    mytree->Project("h_diff_x", "(pv_x_p1 - pv_x_p2)/sqrt(2)", ptcut+xnull_cut);
    mytree->Project("h_diff_y", "(pv_y_p1 - pv_y_p2)/sqrt(2)", ptcut+ynull_cut);
    mytree->Project("h_diff_z", "(pv_z_p1 - pv_z_p2)/sqrt(2)", ptcut+znull_cut);
    mytree->Project("h_pull_x", "(pv_x_p1 - pv_x_p2)/sqrt(pow(pv_xError_p1,2)+pow(pv_xError_p2,2))", ptcut+xnull_cut);
    mytree->Project("h_pull_y", "(pv_y_p1 - pv_y_p2)/sqrt(pow(pv_yError_p1,2)+pow(pv_yError_p2,2))", ptcut+ynull_cut);
    mytree->Project("h_pull_z", "(pv_z_p1 - pv_z_p2)/sqrt(pow(pv_zError_p1,2)+pow(pv_zError_p2,2))", ptcut+znull_cut);


    auto result_reso_pvx = fit_draw(h_diff_x, figdir+Form("pvx_fit/pt_%d", idx));
    auto result_reso_pvy = fit_draw(h_diff_y, figdir+Form("pvy_fit/pt_%d", idx));
    auto result_reso_pvz = fit_draw(h_diff_z, figdir+Form("pvz_fit/pt_%d", idx));

    auto result_reso_pullx = fit_draw(h_pull_x, figdir+Form("pullx_fit/pt_%d", idx));
    auto result_reso_pully = fit_draw(h_pull_y, figdir+Form("pully_fit/pt_%d", idx));
    auto result_reso_pullz = fit_draw(h_pull_z, figdir+Form("pullz_fit/pt_%d", idx));


    nlohmann::json resojson;
    resojson["sumpt2_sqrt"] = (pv_SumTrackPt2_sqrt_edges[idx] + pv_SumTrackPt2_sqrt_edges[idx+1])/2;

    /* resojson["reso_pvx"] = fit_draw(h_diff_x, figdir+Form("pvx_fit/pt_%d", idx)); */
    /* resojson["reso_pvy"] = fit_draw(h_diff_y, figdir+Form("pvy_fit/pt_%d", idx)); */
    /* resojson["reso_pvz"] = fit_draw(h_diff_z, figdir+Form("pvz_fit/pt_%d", idx)); */

    /* resojson["reso_pullx"] = fit_draw(h_pull_x, figdir+Form("pullx_fit/pt_%d", idx)); */
    /* resojson["reso_pully"] = fit_draw(h_pull_y, figdir+Form("pully_fit/pt_%d", idx)); */
    /* resojson["reso_pullz"] = fit_draw(h_pull_z, figdir+Form("pullz_fit/pt_%d", idx)); */

    resojson["reso_pvx"] = result_reso_pvx.first;
    resojson["reso_pvy"] = result_reso_pvy.first;
    resojson["reso_pvz"] = result_reso_pvz.first;

    resojson["reso_pullx"] = result_reso_pullx.first;
    resojson["reso_pully"] = result_reso_pully.first;
    resojson["reso_pullz"] = result_reso_pullz.first;

    resojson["reso2_pvx"] = result_reso_pvx.second;
    resojson["reso2_pvy"] = result_reso_pvy.second;
    resojson["reso2_pvz"] = result_reso_pvz.second;

    resojson["reso2_pullx"] = result_reso_pullx.second;
    resojson["reso2_pully"] = result_reso_pully.second;
    resojson["reso2_pullz"] = result_reso_pullz.second;

    std::ofstream outFile("../../json/"+datatype+Form("/pv_res/fit_%d.json",idx));
    outFile << resojson.dump(4);
    outFile.close();

    return 0;
}
