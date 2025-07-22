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

#include "CMSplots/tdrStyle.c"
#include "CMSplots/CMS_lumi.c"
#include "CMSplots/draw_funcs.c"
#include "input_list.cc"

const TString figdir = "./figures/"+datatype+"/ip_res_analysis/";
const int nbins = 100;

void draw_ip_res(int size, TGraphErrors *gr[], float height, float floor, vector<TString> legendtext, TString figpath){

    TString lumi_sqrtS = "13.6 TeV, 2022"+all_era;
    TCanvas *canvas = new TCanvas("canvas", "", 800, 600);
    canvas_setup(canvas);

    gr[0]->Draw("AP");
    
    if(size > 1){
        gr[1]->Draw("P SAME");
        TLegend *legend = new TLegend(0.5, 0.6, 0.75, 0.75);
        legend->AddEntry(gr[0], legendtext[0], "p");
        legend->AddEntry(gr[1], legendtext[1], "p");
        legend->SetTextFont(42);
        legend->SetTextColor(kBlack);
        legend->SetBorderSize(0);
        legend->SetFillColor(0);
        legend->SetLineWidth(0);
        legend->SetTextSize(0.04);
        legend->Draw();
    }
    write_text(0.5, 0.85, datatype_text);
    CMS_lumi(canvas);
    gr[0]->SetMaximum(height*1.3);
    gr[0]->SetMinimum(0);
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + ".png");
    canvas->SaveAs(figpath + ".eps");
    canvas->SaveAs(figpath + ".pdf");

    gr[0]->SetMaximum(height*100);
    gr[0]->SetMinimum(floor/100);
    canvas->SetLogy();
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + "_log.png");
    canvas->SaveAs(figpath + "_log.eps");
    canvas->SaveAs(figpath + "_log.pdf");
}

/* void ip_res_pt(TChain *mychain, TString ip_var, TString ip_varname, TString ip_axis, TString ip_unit, float ip_range) { */

/*     float eta_cut[2] = {1.4, 3.0}; */
/*     TString eta_name[2] = {"center", "endcap"}; */

/*     int nstep = 50; */
/*     float pt_min = 0.1; */
/*     float pt_inc = 0.2; */
/*     float pt[nstep]; */
/*     float ip_res[2][nstep]; */
    
/*     for(int i=0; i<nstep; i++){ */
/*         float pt_lo = pt_min + pt_inc * i; */
/*         float pt_hi = pt_lo + pt_inc; */
/*         pt[i] = (pt_hi + pt_lo)/2.0; */

/*         TCut mycut_pt = Form("pt > %f && pt < %f", pt_lo, pt_hi); */
        
/*         for(int j=0; j<2; j++){ */
/*             TCut mycut = mycut_pt + Form("abs(eta) < %f", eta_cut[j]); */
/*             TH1F *h_pt_tmp = new TH1F("h_pt_tmp", "", 200, -1*ip_range, ip_range); */   
/*             mychain->Project("h_pt_tmp", ip_var, mycut); */
/*             float mu = h_pt_tmp->GetMean(); */
/*             float sigma = h_pt_tmp->GetStdDev(); */
/*             delete h_pt_tmp; */

/*             TH1F *hist = new TH1F("hist", "", 200, mu-8*sigma, mu+8*sigma); */
/*             mychain->Project("hist", ip_var, mycut); */
/*             hist->SetTitle(Form("#splitline{%.2f<#it{p_{T}}<%.2f}{0.0<|#eta|<%.1f}", pt_lo, pt_hi, eta_cut[j])); */
/*             TString figpath = figdir+ip_varname+Form("_fit/pt_%s_%d.png", eta_name[j].Data(), i); */
/*             ip_res[j][i] = fit_draw(hist, figpath, ip_axis, ip_unit); */
/*             delete hist; */
/*         } */ 
/*     } */
    
/*     float height0 = *std::max_element(ip_res[0], ip_res[0]+nstep); */
/*     float height1 = *std::max_element(ip_res[1], ip_res[1]+nstep); */
/*     float height = std::max(height0, height1); */
/*     float floor0 = *std::min_element(ip_res[0], ip_res[0]+nstep); */
/*     float floor1 = *std::min_element(ip_res[1], ip_res[1]+nstep); */
/*     float floor = std::min(height0, height1); */
    
/*     TGraph *gr0 = new TGraph(nstep, pt, ip_res[0]); */
/*     gr0->SetMarkerStyle(20); */
/*     gr0->SetMarkerColor(kRed); */
/*     gr0->GetXaxis()->SetTitle("Track #it{p_{T}} [GeV]"); */
/*     gr0->GetYaxis()->SetTitle("Track IP resolution in "+ip_axis+" ["+ip_unit+"]"); */
/*     TGraph *gr1 = new TGraph(nstep, pt, ip_res[1]); */
/*     gr1->SetMarkerStyle(24); */
/*     gr1->SetMarkerColor(kRed); */
/*     TGraph *gr[2] = {gr0, gr1}; */
/*     TString figpath = figdir+ip_varname+"_pt"; */
/*     draw_ip_res(2, gr, height, floor, {"0.0<|#it{#eta}|<1.4", "0.0<|#it{#eta}|<3.0"}, figpath); */
/* } */

/* void ip_res_eta(TChain *mychain, TString ip_var, TString ip_varname, TString ip_axis, TString ip_unit, float ip_range) { */

/*     int nstep = 106; */
/*     float eta_inc = 0.05; */
/*     float eta[nstep], eta_lo[nstep], eta_hi[nstep]; */
/*     float ip_res[nstep]; */
/*     eta_lo[0] = -2.8;      eta_hi[0] = -2.6; */
/*     eta_lo[1] = -2.6;      eta_hi[1] = eta_lo[1] + eta_inc; */
/*     eta_hi[nstep-1] = 2.8; eta_lo[nstep-1] = 2.6; */
/*     eta_hi[nstep-2] = 2.6; eta_lo[nstep-2] = eta_hi[nstep-2] - eta_inc; */ 
/*     for(int i=2; i<nstep-2; i++){ */
/*         eta_lo[i] = eta_lo[i-1] + eta_inc; */
/*         eta_hi[i] = eta_lo[i] + eta_inc; */
/*     } */

/*     for(int i=0; i<nstep; i++){ */
/*         eta[i] = (eta_lo[i] + eta_hi[i])/2; */
        
/*         TCut mycut_eta = Form("eta > %f && eta < %f", eta_lo[i], eta_hi[i]); */
/*         TCut mycut_pt = "pt > 1.0 && pt < 10.0"; */
/*         mycut_eta += mycut_pt; */

/*         TH1F *h_eta_tmp = new TH1F("h_eta_tmp", "", 200, -1*ip_range, ip_range); */
/*         mychain->Project("h_eta_tmp", ip_var, mycut_eta); */
/*         float mu = h_eta_tmp->GetMean(); */
/*         float sigma = h_eta_tmp->GetStdDev(); */
/*         delete h_eta_tmp; */

/*         TH1F *h_eta = new TH1F("h_eta", "", 200, mu-8*sigma, mu+8*sigma); */
/*         mychain->Project("h_eta", ip_var, mycut_eta); */
/*         h_eta->SetTitle(Form("#splitline{1<#it{p_{T}}<10 GeV}{%.2f<#it{#eta}<%.2f}", eta_lo[i], eta_hi[i])); */
/*         TString figpath = figdir+ip_varname+Form("_fit/eta_%d.png", i); */
/*         ip_res[i] = fit_draw(h_eta, figpath, ip_axis, ip_unit); */
/*         delete h_eta; */
/*     } */
    
/*     float height = *std::max_element(ip_res, ip_res+nstep); */
/*     float floor = *std::min_element(ip_res, ip_res+nstep); */
    
/*     TGraph *gr0 = new TGraph(nstep, eta, ip_res); */
/*     gr0->SetMarkerStyle(20); */
/*     gr0->SetMarkerColor(kRed); */
/*     gr0->GetXaxis()->SetTitle("Track #it{#eta}"); */
/*     gr0->GetYaxis()->SetTitle("Track IP resolution in "+ip_axis+" ["+ip_unit+"]"); */
/*     TGraph *gr[1] = {gr0}; */
/*     TString figpath = figdir+ip_varname+"_eta"; */
/*     draw_ip_res(1, gr, height, floor, {""}, figpath); */
/* } */


std::pair<float, float> fit_draw(TH1F* hist, TString figpath) {
    
    TString lumi_sqrtS = "13.6 TeV, 2022"+all_era;
    
    RooRealVar ip_var("ip_var", "ip_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar mu("mu", "mu", hist->GetMean(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar sigma1("sigma1", "sigma1", 0.5*hist->GetRMS(), 0., hist->GetRMS());
    RooRealVar sigma2("sigma2", "sigma2", hist->GetRMS(), hist->GetRMS()*0.5, hist->GetRMS()*2);
    RooRealVar sigma3("sigma3", "sigma3", 1.5*hist->GetRMS(), hist->GetRMS(), hist->GetRMS()*3);
    RooGaussian gauss1("gauss1", "gauss1", ip_var, mu, sigma1);
    RooGaussian gauss2("gauss2", "gauss2", ip_var, mu, sigma2);
    RooGaussian gauss3("gauss3", "gauss3", ip_var, mu, sigma3);
    RooRealVar f1("f1", "f1", 0.5, 0.0, 1.0);
    RooRealVar f2("f2", "f2", 0.5, 0.0, 1.0);
    RooAddPdf triGauss("triGauss", "triGauss", RooArgList(gauss1, gauss2, gauss3), RooArgList(f1, f2));

    RooDataHist hdatahist("hdatahist", "", ip_var, hist);
    RooFitResult *fitResult = triGauss.fitTo(hdatahist, RooFit::Save(true));
    fitResult->Print();
    
    float mean = mu.getVal();
    int meanbin = hist->FindBin(mean);

    float nentries = hist->GetEntries();
    int nreso = 0;
    int sumentries = 0;

    while(sumentries < nentries*0.68) {
        nreso++;
        sumentries = 0;
        for(int bin = meanbin - nreso; bin <= meanbin + nreso; bin++){
            sumentries += hist->GetBinContent(bin);
        } 
    }
    float reso = (2 * nreso - 1) * hist->GetBinWidth(1);
    float resoerr = reso / std::sqrt(2*nentries);
    
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
    RooPlot *frame = ip_var.frame();
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
    write_text(0.7, 0.88, datatype_text);
    write_text(0.7, 0.8, hist->GetTitle());
    write_text(0.7, 0.7, Form("Fit Results:"));
    write_text(0.7, 0.65, Form("#mu = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(mu.getVal())))), mu.getVal(), std::max(0, 2-(int)floor(log10(abs(mu.getVal())))), mu.getError()));
    write_text(0.7, 0.6, Form("#sigma_{1} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma1.getVal())))), sigma1.getVal(), std::max(0, 2-(int)floor(log10(abs(sigma1.getVal())))), sigma1.getError()));
    write_text(0.7, 0.55, Form("#sigma_{2} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma2.getVal())))), sigma2.getVal(), std::max(0, 2-(int)floor(log10(abs(sigma2.getVal())))), sigma2.getError()));
    write_text(0.7, 0.5, Form("#sigma_{3} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma3.getVal())))), sigma3.getVal(), std::max(0, 2-(int)floor(log10(abs(sigma3.getVal())))), sigma3.getError()));
    write_text(0.7, 0.45, Form("reso = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma3.getVal())))), reso, std::max(0, 2-(int)floor(log10(abs(sigma3.getVal())))), resoerr));
    CMS_lumi_sub(c1);

    canvas->cd(2);
    TVirtualPad* c2 = canvas->GetPad(2);
    c2->SetPad(0,0,1,0.3);
    c2->SetRightMargin(0.05);
    c2->SetTopMargin(0.05);
    c2->SetBottomMargin(0.4);
    RooHist* hpull = frame->pullHist("data","triGauss");
    RooPlot* pull = ip_var.frame();
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
    CMS_lumi(canvas);
    canvas->Update();
    /* canvas->SaveAs(figpath); */
    canvas->SaveAs(figpath+".png");
    canvas->SaveAs(figpath+".eps");
    canvas->SaveAs(figpath+".pdf");

    return {reso, resoerr};
}

void ip_res_eta(TChain *mychain, TString ip_var, TString ip_varname, TString xtitle, float ip_range) {

    if(gSystem->AccessPathName(figdir+ip_varname+"_fit")) gSystem->MakeDirectory(figdir+ip_varname+"_fit");
    
    std::ifstream infile("binning.json");
    nlohmann::json binning;
    infile >> binning;

    std::vector<float> pv_trk_eta_edges = binning["pv_trk_eta"].get<std::vector<float>>();

    float eta[nbins], reso_lopt[nbins], resoerr_lopt[nbins], reso_hipt[nbins], resoerr_hipt[nbins];

    for(int i=0; i<nbins; i++){
        
        eta[i] = ( pv_trk_eta_edges[i] + pv_trk_eta_edges[i+1] ) / 2;

        /* TCut mycut( ip_var + "!=-777" ); */
        TCut mycut = Form("pv_trk_eta > %.2f && pv_trk_eta < %.2f", pv_trk_eta_edges[i], pv_trk_eta_edges[i+1]);

        TH1F *h_eta_lopt_tmp = new TH1F("h_eta_lopt_tmp", "", 200, -ip_range, ip_range);
        mychain->Project("h_eta_lopt_tmp", ip_var, mycut + "pv_trk_pt < 1.0");
        float mu = h_eta_lopt_tmp->GetMean();
        float sigma = h_eta_lopt_tmp->GetStdDev();
        delete h_eta_lopt_tmp;

        TH1F *h_eta_lopt = new TH1F("h_eta_lopt", "", 200, mu-8*sigma, mu+8*sigma);
        mychain->Project("h_eta_lopt", ip_var, mycut + "pv_trk_pt < 1.0");

        h_eta_lopt->SetTitle(Form("#splitline{0.1<#it{p_{T}}<1 GeV}{%.2f<#it{#eta}<%.2f}", pv_trk_eta_edges[i], pv_trk_eta_edges[i+1]));
        h_eta_lopt->SetXTitle("Track IP " + xtitle);
        h_eta_lopt->SetYTitle("# tracks");

        auto reso_result_lopt = fit_draw(h_eta_lopt, figdir+ip_varname+Form("_fit/lopt_eta_%d", i));
        reso_lopt[i] = reso_result_lopt.first;
        resoerr_lopt[i] = reso_result_lopt.second;
        delete h_eta_lopt;

        TH1F *h_eta_hipt_tmp = new TH1F("h_eta_hipt_tmp", "", 200, -ip_range, ip_range);
        mychain->Project("h_eta_hipt_tmp", ip_var, mycut + "pv_trk_pt > 1.0 && pv_trk_pt < 10.0");
        mu = h_eta_hipt_tmp->GetMean();
        sigma = h_eta_hipt_tmp->GetStdDev();
        delete h_eta_hipt_tmp;

        TH1F *h_eta_hipt = new TH1F("h_eta_hipt", "", 200, mu-8*sigma, mu+8*sigma);
        mychain->Project("h_eta_hipt", ip_var, mycut + "pv_trk_pt > 1.0 && pv_trk_pt < 10.0");

        h_eta_hipt->SetTitle(Form("#splitline{1<#it{p_{T}}<10 GeV}{%.2f<#it{#eta}<%.2f}", pv_trk_eta_edges[i], pv_trk_eta_edges[i+1]));
        h_eta_hipt->SetXTitle("Track IP " + xtitle);
        h_eta_hipt->SetYTitle("# tracks");

        auto reso_result_hipt = fit_draw(h_eta_hipt, figdir+ip_varname+Form("_fit/hipt_eta_%d", i));
        reso_hipt[i] = reso_result_hipt.first;
        resoerr_hipt[i] = reso_result_hipt.second;
        delete h_eta_hipt;

    }
    
    float height_lopt = *std::max_element(reso_lopt, reso_lopt+nbins);
    float height_hipt = *std::max_element(reso_hipt, reso_hipt+nbins);
    float height = std::max(height_lopt, height_hipt);
    float floor_lopt = *std::min_element(reso_lopt, reso_lopt+nbins);
    float floor_hipt = *std::min_element(reso_hipt, reso_hipt+nbins);
    float floor = std::min(floor_lopt, floor_hipt);

    /* TGraph *gr_lopt = new TGraph(nbins, eta, reso_lopt); */
    TGraphErrors *gr_lopt = new TGraphErrors(nbins, eta, reso_lopt, nullptr, resoerr_lopt);
    gr_lopt->SetMarkerStyle(20);
    gr_lopt->SetMarkerColor(kRed);
    gr_lopt->GetXaxis()->SetTitle("Track #it{#eta}");
    gr_lopt->GetYaxis()->SetTitle("Track IP resolution " + xtitle);
    /* TGraph *gr_hipt = new TGraph(nbins, eta, reso_hipt); */
    TGraphErrors *gr_hipt = new TGraphErrors(nbins, eta, reso_hipt, nullptr, resoerr_hipt);
    gr_hipt->SetMarkerStyle(24);
    gr_hipt->SetMarkerColor(kRed);
    /* TGraph *gr[2] = {gr_lopt, gr_hipt}; */
    TGraphErrors *gr[2] = {gr_lopt, gr_hipt};
    TString figpath = figdir+ip_varname+"_eta";
    draw_ip_res(2, gr, height, floor, {"0.1<#it{p_{T}}<1 GeV", "1<#it{p_{T}}<10 GeV"}, figpath);

}

void ip_res_phi(TChain *mychain, TString ip_var, TString ip_varname, TString xtitle, float ip_range) {

    if(gSystem->AccessPathName(figdir+ip_varname+"_fit")) gSystem->MakeDirectory(figdir+ip_varname+"_fit");
    
    std::ifstream infile("binning.json");
    nlohmann::json binning;
    infile >> binning;

    std::vector<float> pv_trk_phi_edges = binning["pv_trk_phi"].get<std::vector<float>>();

    float phi[nbins], reso_lopt[nbins], resoerr_lopt[nbins], reso_hipt[nbins], resoerr_hipt[nbins];

    for(int i=0; i<nbins; i++){
        
        phi[i] = ( pv_trk_phi_edges[i] + pv_trk_phi_edges[i+1] ) / 2;

        /* TCut mycut( ip_var + "!=-777" ); */
        TCut mycut = Form("pv_trk_phi > %.2f && pv_trk_phi < %.2f", pv_trk_phi_edges[i], pv_trk_phi_edges[i+1]);

        TH1F *h_phi_lopt_tmp = new TH1F("h_phi_lopt_tmp", "", 200, -ip_range, ip_range);
        mychain->Project("h_phi_lopt_tmp", ip_var, mycut + "pv_trk_pt < 1.0");
        float mu = h_phi_lopt_tmp->GetMean();
        float sigma = h_phi_lopt_tmp->GetStdDev();
        delete h_phi_lopt_tmp;

        TH1F *h_phi_lopt = new TH1F("h_phi_lopt", "", 200, mu-8*sigma, mu+8*sigma);
        mychain->Project("h_phi_lopt", ip_var, mycut + "pv_trk_pt < 1.0");

        h_phi_lopt->SetTitle(Form("#splitline{0.1<#it{p_{T}}<1 GeV}{%.2f<#it{#phi}<%.2f}", pv_trk_phi_edges[i], pv_trk_phi_edges[i+1]));
        h_phi_lopt->SetXTitle("Track IP " + xtitle);
        h_phi_lopt->SetYTitle("# tracks");

        auto reso_result_lopt = fit_draw(h_phi_lopt, figdir+ip_varname+Form("_fit/lopt_phi_%d", i));
        reso_lopt[i] = reso_result_lopt.first;
        resoerr_lopt[i] = reso_result_lopt.second;
        delete h_phi_lopt;

        TH1F *h_phi_hipt_tmp = new TH1F("h_phi_hipt_tmp", "", 200, -ip_range, ip_range);
        mychain->Project("h_phi_hipt_tmp", ip_var, mycut + "pv_trk_pt > 1.0 && pv_trk_pt < 10.0");
        mu = h_phi_hipt_tmp->GetMean();
        sigma = h_phi_hipt_tmp->GetStdDev();
        delete h_phi_hipt_tmp;

        TH1F *h_phi_hipt = new TH1F("h_phi_hipt", "", 200, mu-8*sigma, mu+8*sigma);
        mychain->Project("h_phi_hipt", ip_var, mycut + "pv_trk_pt > 1.0 && pv_trk_pt < 10.0");

        h_phi_hipt->SetTitle(Form("#splitline{1<#it{p_{T}}<10 GeV}{%.2f<#it{#phi}<%.2f}", pv_trk_phi_edges[i], pv_trk_phi_edges[i+1]));
        h_phi_hipt->SetXTitle("Track IP " + xtitle);
        h_phi_hipt->SetYTitle("# tracks");

        auto reso_result_hipt = fit_draw(h_phi_hipt, figdir+ip_varname+Form("_fit/hipt_phi_%d", i));
        reso_hipt[i] = reso_result_hipt.first;
        resoerr_hipt[i] = reso_result_hipt.second;
        delete h_phi_hipt;

    }
    
    float height_lopt = *std::max_element(reso_lopt, reso_lopt+nbins);
    float height_hipt = *std::max_element(reso_hipt, reso_hipt+nbins);
    float height = std::max(height_lopt, height_hipt);
    float floor_lopt = *std::min_element(reso_lopt, reso_lopt+nbins);
    float floor_hipt = *std::min_element(reso_hipt, reso_hipt+nbins);
    float floor = std::min(floor_lopt, floor_hipt);

    /* TGraph *gr_lopt = new TGraph(nbins, phi, reso_lopt); */
    TGraphErrors *gr_lopt = new TGraphErrors(nbins, phi, reso_lopt, nullptr, resoerr_lopt);
    gr_lopt->SetMarkerStyle(20);
    gr_lopt->SetMarkerColor(kRed);
    gr_lopt->GetXaxis()->SetTitle("Track #it{#phi}");
    gr_lopt->GetYaxis()->SetTitle("Track IP resolution " + xtitle);
    TGraphErrors *gr_hipt = new TGraphErrors(nbins, phi, reso_hipt, nullptr, resoerr_hipt);
    gr_hipt->SetMarkerStyle(24);
    gr_hipt->SetMarkerColor(kRed);
    TGraphErrors *gr[2] = {gr_lopt, gr_hipt};
    TString figpath = figdir+ip_varname+"_phi";
    draw_ip_res(2, gr, height, floor, {"0.1<#it{p_{T}}<1 GeV", "1<#it{p_{T}}<10 GeV"}, figpath);

}

void ip_res_pt(TChain *mychain, TString ip_var, TString ip_varname, TString xtitle, float ip_range) {

    if(gSystem->AccessPathName(figdir+ip_varname+"_fit")) gSystem->MakeDirectory(figdir+ip_varname+"_fit");
    
    std::ifstream infile("binning.json");
    nlohmann::json binning;
    infile >> binning;

    std::vector<float> pv_trk_pt_edges = binning["pv_trk_pt"].get<std::vector<float>>();

    float pt[nbins], reso_loeta[nbins], resoerr_loeta[nbins], reso_hieta[nbins], resoerr_hieta[nbins];

    for(int i=0; i<nbins; i++){
        
        pt[i] = ( pv_trk_pt_edges[i] + pv_trk_pt_edges[i+1] ) / 2;

        /* TCut mycut( ip_var + "!=-777" ); */
        TCut mycut = Form("pv_trk_pt > %.2f && pv_trk_pt < %.2f", pv_trk_pt_edges[i], pv_trk_pt_edges[i+1]);

        TH1F *h_pt_loeta_tmp = new TH1F("h_pt_loeta_tmp", "", 200, -ip_range, ip_range);
        mychain->Project("h_pt_loeta_tmp", ip_var, mycut + "abs(pv_trk_eta) < 1.4");
        float mu = h_pt_loeta_tmp->GetMean();
        float sigma = h_pt_loeta_tmp->GetStdDev();
        delete h_pt_loeta_tmp;

        TH1F *h_pt_loeta = new TH1F("h_pt_loeta", "", 200, mu-8*sigma, mu+8*sigma);
        mychain->Project("h_pt_loeta", ip_var, mycut + "abs(pv_trk_eta) < 1.4");

        h_pt_loeta->SetTitle(Form("#splitline{%.2f<#it{p_{T}}<%.2f GeV}{0<|#it{#eta}|<1.4}", pv_trk_pt_edges[i], pv_trk_pt_edges[i+1]));
        h_pt_loeta->SetXTitle("Track IP " + xtitle);
        h_pt_loeta->SetYTitle("# tracks");

        auto reso_result_loeta = fit_draw(h_pt_loeta, figdir+ip_varname+Form("_fit/loeta_pt_%d", i));
        reso_loeta[i] = reso_result_loeta.first;
        resoerr_loeta[i] = reso_result_loeta.second;
        delete h_pt_loeta;

        TH1F *h_pt_hieta_tmp = new TH1F("h_pt_hieta_tmp", "", 200, -ip_range, ip_range);
        mychain->Project("h_pt_hieta_tmp", ip_var, mycut + "abs(pv_trk_eta) < 3.0");
        mu = h_pt_hieta_tmp->GetMean();
        sigma = h_pt_hieta_tmp->GetStdDev();
        delete h_pt_hieta_tmp;

        TH1F *h_pt_hieta = new TH1F("h_pt_hieta", "", 200, mu-8*sigma, mu+8*sigma);
        mychain->Project("h_pt_hieta", ip_var, mycut + "abs(pv_trk_eta) < 3.0");

        h_pt_hieta->SetTitle(Form("#splitline{%.2f<#it{p_{T}}<%.2f GeV}{0<|#it{#eta}|<3.0}", pv_trk_pt_edges[i], pv_trk_pt_edges[i+1]));
        h_pt_hieta->SetXTitle("Track IP " + xtitle);
        h_pt_hieta->SetYTitle("# tracks");

        auto reso_result_hieta = fit_draw(h_pt_hieta, figdir+ip_varname+Form("_fit/hieta_pt_%d", i));
        reso_hieta[i] = reso_result_hieta.first;
        resoerr_hieta[i] = reso_result_hieta.second;
        delete h_pt_hieta;

    }
    
    float height_loeta = *std::max_element(reso_loeta, reso_loeta+nbins);
    float height_hieta = *std::max_element(reso_hieta, reso_hieta+nbins);
    float height = std::max(height_loeta, height_hieta);
    float floor_loeta = *std::min_element(reso_loeta, reso_loeta+nbins);
    float floor_hieta = *std::min_element(reso_hieta, reso_hieta+nbins);
    float floor = std::min(floor_loeta, floor_hieta);

    TGraphErrors *gr_loeta = new TGraphErrors(nbins, pt, reso_loeta, nullptr, resoerr_loeta);
    gr_loeta->SetMarkerStyle(20);
    gr_loeta->SetMarkerColor(kRed);
    gr_loeta->GetXaxis()->SetTitle("Track #it{p_{T}} [GeV]");
    gr_loeta->GetYaxis()->SetTitle("Track IP resolution " + xtitle);
    TGraphErrors *gr_hieta = new TGraphErrors(nbins, pt, reso_hieta, nullptr, resoerr_hieta);
    gr_hieta->SetMarkerStyle(24);
    gr_hieta->SetMarkerColor(kRed);
    TGraphErrors *gr[2] = {gr_loeta, gr_hieta};
    TString figpath = figdir+ip_varname+"_pt";
    draw_ip_res(2, gr, height, floor, {"0<|#it{#eta}|<1.4", "0<|#it{#eta}|<3.0"}, figpath);

}


int ip_res_analysis(int idx) {
    
    if(gSystem->AccessPathName("./figures")) gSystem->MakeDirectory("./figures");
    if(gSystem->AccessPathName("./figures/"+datatype)) gSystem->MakeDirectory("./figures/"+datatype);
    if(gSystem->AccessPathName(figdir)) gSystem->MakeDirectory(figdir);

    setTDRStyle();

    TString ip_var[4] = {"pv_trk_d0_pvunbiased", "pv_trk_dz_pvunbiased/1000", "pv_trk_d0_bs", "pv_trk_dz_bs/1000"};
    TString ip_varname[4] = {"ippv_xy", "ippv_z", "ipbs_xy", "ipbs_z"};
    TString ip_axis[4] = {"#it{d_{xy}} (IPPV) [#mum]", "#it{d_{z}} (IPPV) [mm]", "#it{d_{xy}} (IPBS) [#mum]", "#it{d_{z}} (IPBS) [mm]"};
    float ip_range[4] = {2000, 8, 2000, 200};
    
    TChain *mychain = new TChain("mytree");
    mychain->Add("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/scripts/tuples/ZeroBias_data2022.root");

    /* ip_res_eta(mychain, "pv_trk_d0_pvunbiased", "ippv_xy", "#it{d_{xy}} [#mum]", 2000); */ 
    if(idx < 4){
        ip_res_eta(mychain, ip_var[idx], ip_varname[idx], ip_axis[idx], ip_range[idx]);
    }
    else if(idx > 3 && idx < 8){
        ip_res_phi(mychain, ip_var[idx-4], ip_varname[idx-4], ip_axis[idx-4], ip_range[idx-4]);
    }
    else if(idx > 7){
        ip_res_pt(mychain, ip_var[idx-8], ip_varname[idx-8], ip_axis[idx-8], ip_range[idx-8]);
    }

    delete mychain;

    return 0;
}
