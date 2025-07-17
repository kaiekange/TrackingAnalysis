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
#include <RooGaussian.h>

#include "input_list.c"
#include "CMSplots/tdrStyle.c"
#include "CMSplots/CMS_lumi.c"
#include "CMSplots/draw_funcs.c"

const TString figdir = "./figures/"+datatype+"/pv_res_analysis/";

float fit_draw(TH1F* hist, TString figpath) {
    
    lumi_sqrtS = "13.6 TeV, 2022"+all_era;
  
    float nentries = hist->GetEntries();

    RooRealVar pvx_diff("pvx_diff", "pvx_diff", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar mu("mu", "mu", 0, hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar sigma1("sigma1", "sigma1", hist->GetRMS()/2, 0., hist->GetRMS()*3);
    RooRealVar sigma2("sigma2", "sigma2", hist->GetRMS(), 0., hist->GetRMS()*3);
    RooRealVar sigma3("sigma3", "sigma3", hist->GetRMS()*2, 0., hist->GetRMS()*3);
    RooGaussian gauss1("gauss1", "gauss1", pvx_diff, mu, sigma1);
    RooGaussian gauss2("gauss2", "gauss2", pvx_diff, mu, sigma2);
    RooGaussian gauss3("gauss3", "gauss3", pvx_diff, mu, sigma3);
    RooRealVar num1("num1", "num1", nentries/10., 0.0, nentries*1.1);
    RooRealVar num2("num2", "num2", nentries/5., 0.0, nentries*1.1);
    RooRealVar num3("num3", "num3", nentries/2., 0.0, nentries*1.1);

    /* RooAddPdf model("model", "model", RooArgList(gauss1, gauss2), RooArgList(num1, num2)); */
    RooAddPdf model("model", "model", RooArgList(gauss1, gauss2, gauss3), RooArgList(num1, num2, num3));

    TCanvas *canvas = new TCanvas("canvas", "", 800, 600);
    canvas_setup(canvas);

    RooDataHist hdatahist("hdatahist", "", pvx_diff, hist);
    RooFitResult *fitResult = model.fitTo(hdatahist, RooFit::Save(true));
    fitResult->Print();
    RooPlot *frame = pvx_diff.frame();
    hdatahist.plotOn(frame);
    model.plotOn(frame);
    frame->SetYTitle("# events");
    frame->SetXTitle(hist->GetXaxis()->GetTitle());
    frame->Draw();

    /* float sigma = sqrt((num1.getVal()*pow(sigma1.getVal(),2) + num2.getVal()*pow(sigma2.getVal(),2))/(num1.getVal()+num2.getVal())); */
    float sigma = sqrt((num1.getVal()*pow(sigma1.getVal(),2) + num2.getVal()*pow(sigma2.getVal(),2) + num3.getVal()*pow(sigma3.getVal(),2))/(num1.getVal()+num2.getVal()+num3.getVal()));
    float mean = mu.getVal();
    float pv_res = sigma/2;
    RooAbsReal* integral = nullptr;

    int digit = std::floor(std::log10(pv_res));
    float prob = 0;
    float step = 0;
    for(int i=0; i<4; i++){
        prob = 0;
        pv_res -= step;
        step = pow(10,digit);
        while (prob < 0.68) {
            pv_res += step;
            pvx_diff.setRange("symRange", mean - pv_res, mean + pv_res);
            integral = model.createIntegral(pvx_diff, RooFit::NormSet(pvx_diff), RooFit::Range("symRange"));
            prob = integral->getVal();
            delete integral;
        }
        digit--;
    }

    write_text(0.17, 0.7, datatype_text);
    write_text(0.6, 0.85, hist->GetTitle());
    write_text(0.6, 0.75, Form("Fit Results:"));
    write_text(0.6, 0.7, Form("#mu = %.3f #pm %.3f",   mu.getVal(),     mu.getError()));
    write_text(0.6, 0.65, Form("#sigma_{1} = %.3f #pm %.3f",  sigma1.getVal(),  sigma1.getError()));
    write_text(0.6, 0.6, Form("#sigma_{2} = %.3f #pm %.3f",  sigma2.getVal(),  sigma2.getError()));
    write_text(0.6, 0.55, Form("#sigma_{3} = %.3f #pm %.3f",  sigma3.getVal(),  sigma3.getError()));
    write_text(0.6, 0.5, Form("#sigma = %.3f", sigma));
    write_text(0.6, 0.45, Form("resolution = %.*f", std::max(0, -1-digit), pv_res));
    
    CMS_lumi(canvas);
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath);

    return pv_res;
}

void draw_pv_res(TGraph *gr, float height, float floor, TString figpath){

    lumi_sqrtS = "13.6 TeV, 2022"+all_era;
    TCanvas *canvas = new TCanvas("canvas", "", 800, 600);
    canvas_setup(canvas);

    gr->SetMaximum(height);
    gr->SetMinimum(floor);
    gr->Draw("AP");

    write_text(0.17, 0.7, datatype_text);
    CMS_lumi(canvas);
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + ".png");

    gr->SetMaximum(height*100);
    gr->SetMinimum(floor/100);
    canvas->SetLogy();
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + "_log.png");
    delete canvas;
    delete gr;
}

void find_pv_res(TChain *mychain, TString pv_var, TString pv_unit, float pv_range){

    const int nstep = 28;
    float pt_min = 4;
    float pt_inc = 0.5;
    float pt[nstep], pv_res[nstep];
    for(int i=0; i<nstep; i++){
        float pt_lo = pt_min + pt_inc * i;
        float pt_hi = pt_lo + pt_inc;
        pt[i] = (pt_lo + pt_hi)/2;
        TCut mycut = Form("sqrt(pt2sum) > %.2f && sqrt(pt2sum) < %.2f", pt_lo, pt_hi);
        mycut += Form("abs(pv_%s1 - pv_%s) < 2*%f", pv_var.Data(), pv_var.Data(), pv_range);
        mycut += Form("abs(pv_%s2 - pv_%s) < 2*%f", pv_var.Data(), pv_var.Data(), pv_range);

        TH1F *h_tmp = new TH1F("h_tmp", "", 200, -pv_range, pv_range);
        mychain->Project("h_tmp", Form("(pv_%s1 - pv_%s2)/sqrt(2)", pv_var.Data(), pv_var.Data()), mycut);
        float mu = h_tmp->GetMean();
        float sigma = h_tmp->GetStdDev();
        delete h_tmp;

        TH1F *hist = new TH1F("hist", "", 200, mu-8*sigma, mu+8*sigma);
        mychain->Project("hist", Form("(pv_%s1 - pv_%s2)/sqrt(2)", pv_var.Data(), pv_var.Data()), mycut);
        hist->SetTitle(Form("%.2f<#sqrt{#sum{#it{p_{T}}^{2}}}<%.2f;(%s_{1}-%s_{2})/#sqrt{2} [%s];", pt_lo, pt_hi, pv_var.Data(), pv_var.Data(), pv_unit.Data()));
        TString figpath = figdir+"pv"+pv_var+"_fit/"+Form("pt_%d.png", i);
        pv_res[i] = fit_draw(hist, figpath);
        delete hist;
    }

    float height = *std::max_element(pv_res, pv_res+nstep);
    float floor = *std::min_element(pv_res, pv_res+nstep);

    TGraph *gr0 = new TGraph(nstep, pt, pv_res);
    gr0->SetMarkerStyle(20);
    gr0->SetMarkerColor(kRed);
    gr0->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}^{2}}} [GeV]");
    gr0->GetYaxis()->SetTitle(Form("PV resolution in %s [%s]", pv_var.Data(), pv_unit.Data()));
    TString figpath = figdir+"pv_res_"+ pv_var;
    draw_pv_res(gr0, 1.3*height, floor/1.3, figpath);
}

/* void find_pv_pull(TChain *mychain, TString pv_var, TString pv_unit, float pv_mean, float pv_range){ */

/*     const int nstep = 28; */
/*     float pt_min = 4; */
/*     float pt_inc = 0.5; */
/*     float pt[nstep], pull[nstep]; */
/*     for(int i=0; i<nstep; i++){ */
/*         float pt_lo = pt_min + pt_inc * i; */
/*         float pt_hi = pt_lo + pt_inc; */
/*         pt[i] = (pt_lo + pt_hi)/2; */
/*         TCut mycut = Form("sqrt(pt2sum) > %.2f && sqrt(pt2sum) < %.2f", pt_lo, pt_hi); */
/*         mycut += Form("abs(pv_%s1 - pv_%s) < 2*%f", pv_var.Data(), pv_var.Data(), pv_range); */
/*         mycut += Form("abs(pv_%s2 - pv_%s) < 2*%f", pv_var.Data(), pv_var.Data(), pv_range); */
       
/*         TH1F *h1_tmp = new TH1F("h1_tmp", Form("%.2f<#sqrt{#sum{#it{p_{T}}^{2}}}<%.2f;%s_{1} [%s];", pt_lo, pt_hi, pv_var.Data(), pv_unit.Data()), 200, pv_mean - pv_range, pv_mean + pv_range); */
/*         /1* mychain->Project("h1_tmp", "pv_"+pv_var+"1", mycut); *1/ */
/*         mychain->Project("h1_tmp", "pv_"+pv_var+"1-pv_"+pv_var, mycut); */
/*         TString figpath1 = figdir+"pv"+pv_var+"_pull/"+Form("pt_%d_1.png", i); */
/*         float sigma1 = fit_draw(h1_tmp, figpath1); */
/*         delete h1_tmp; */

/*         TH1F *h2_tmp = new TH1F("h2_tmp", Form("%.2f<#sqrt{#sum{#it{p_{T}}^{2}}}<%.2f;%s_{2} [%s];", pt_lo, pt_hi, pv_var.Data(), pv_unit.Data()), 200, pv_mean - pv_range, pv_mean + pv_range); */
/*         /1* mychain->Project("h2_tmp", "pv_"+pv_var+"2", mycut); *1/ */
/*         mychain->Project("h2_tmp", "pv_"+pv_var+"2-pv_"+pv_var, mycut); */
/*         TString figpath2 = figdir+"pv"+pv_var+"_pull/"+Form("pt_%d_2.png", i); */
/*         float sigma2 = fit_draw(h2_tmp, figpath2); */
/*         delete h2_tmp; */

/*         float denom = sqrt(pow(sigma1, 2) + pow(sigma2, 2)); */

/*         TH1F *h_tmp = new TH1F("h_tmp", "", 200, -3*pv_range/denom, 3*pv_range/denom); */
/*         mychain->Project("h_tmp", Form("(pv_%s1 - pv_%s2)/%f", pv_var.Data(), pv_var.Data(), denom), mycut); */
/*         float mu = h_tmp->GetMean(); */
/*         float sigma = h_tmp->GetStdDev(); */
/*         delete h_tmp; */

/*         TH1F *hist = new TH1F("hist", "", 200, mu-8*sigma, mu+8*sigma); */
/*         hist->SetTitle( Form("%.2f<#sqrt{#sum{#it{p_{T}}^{2}}}<%.2f", pt_lo, pt_hi) ); */
/*         hist->GetXaxis()->SetTitle("("+pv_var+"_{1}-"+pv_var+"_{2})/#sqrt{#sigma^{2}_{1}+#sigma^{2}_{2}} ["+pv_unit+"]"); */
/*         mychain->Project("hist", Form("(pv_%s1 - pv_%s2)/%f", pv_var.Data(), pv_var.Data(), denom), mycut); */
/*         TString figpath_diff = figdir+"pv"+pv_var+"_pull/"+Form("pt_%d_diff.png", i); */
/*         pull[i] = fit_draw(hist, figpath_diff); */
/*         delete hist; */
/*     } */

/*     float height = *std::max_element(pull, pull+nstep); */
/*     float floor = *std::min_element(pull, pull+nstep); */

/*     TGraph *gr0 = new TGraph(nstep, pt, pull); */
/*     gr0->SetMarkerStyle(20); */
/*     gr0->SetMarkerColor(kRed); */
/*     gr0->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}^{2}}} [GeV]"); */
/*     gr0->GetYaxis()->SetTitle("Pull in "+pv_var+" ["+pv_unit+"]"); */
/*     TString figpath = figdir+"pull_"+pv_var; */
/*     draw_pv_res(gr0, 2*height, 0, figpath); */
/* } */

void find_pv_pull(TChain *mychain, TString pv_var, float pv_range){

    const int nstep = 28;
    float pt_min = 4;
    float pt_inc = 0.5;
    float pt[nstep], pull[nstep];
    for(int i=0; i<nstep; i++){
        float pt_lo = pt_min + pt_inc * i;
        float pt_hi = pt_lo + pt_inc;
        pt[i] = (pt_lo + pt_hi)/2;
        TCut mycut = Form("sqrt(pt2sum) > %.2f && sqrt(pt2sum) < %.2f", pt_lo, pt_hi);
        mycut += Form("abs(pv_%s1 - pv_%s) < 2*%f", pv_var.Data(), pv_var.Data(), pv_range);
        mycut += Form("abs(pv_%s2 - pv_%s) < 2*%f", pv_var.Data(), pv_var.Data(), pv_range);

        TH1F *h_tmp = new TH1F("h_tmp", "", 200, -5, 5);
        mychain->Project("h_tmp", Form("(pv_%s1 - pv_%s2)/sqrt(pow(pv_%s1Error,2) + pow(pv_%s2Error,2))", pv_var.Data(), pv_var.Data(), pv_var.Data(), pv_var.Data()), mycut);
        float mu = h_tmp->GetMean();
        float sigma = h_tmp->GetStdDev();
        delete h_tmp;

        TH1F *hist = new TH1F("hist", "", 200, mu-8*sigma, mu+8*sigma);
        hist->SetTitle( Form("%.2f<#sqrt{#sum{#it{p_{T}}^{2}}}<%.2f", pt_lo, pt_hi) );
        hist->GetXaxis()->SetTitle("("+pv_var+"_{1}-"+pv_var+"_{2})/#sqrt{#Delta"+pv_var+"^{2}_{1}+#Delta"+pv_var+"^{2}_{2}}");
        mychain->Project("hist", Form("(pv_%s1 - pv_%s2)/sqrt(pow(pv_%s1Error,2) + pow(pv_%s2Error,2))", pv_var.Data(), pv_var.Data(), pv_var.Data(), pv_var.Data()), mycut);
        TString figpath_diff = figdir+"pv"+pv_var+"_pull/"+Form("pt_%d_diff.png", i);
        pull[i] = fit_draw(hist, figpath_diff);
        delete hist;
    }

    float height = *std::max_element(pull, pull+nstep);
    float floor = *std::min_element(pull, pull+nstep);

    TGraph *gr0 = new TGraph(nstep, pt, pull);
    gr0->SetMarkerStyle(20);
    gr0->SetMarkerColor(kRed);
    gr0->GetXaxis()->SetTitle("#sqrt{#sum#it{p_{T}^{2}}} [GeV]");
    gr0->GetYaxis()->SetTitle("Pull in "+pv_var);
    TString figpath = figdir+"pull_"+pv_var;
    draw_pv_res(gr0, 2*height, 0, figpath);
}

int pv_res_analysis(int idx) {

    setTDRStyle();

    TString pv_var[] = {"x", "y", "z"};
    TString pv_unit[] = {"#mum", "#mum", "#mum"};
    /* float pv_mean[] = {1735, -1824, 0}; */
    float pv_mean[] = {0, 0, 0};
    float pv_range[] = {200, 200, 300000};
    float pv_mpv_range[] = {200, 200, 300000};
    /* float pv_mpv_range[] = {200, 200, 200}; */

    const int nera = 6;
    TChain *all_chain = new TChain("mytree");
    for(int i=0; i<nera; i++){
        TString temp_path = filepath[i] + "vertex_analysis.root";
        all_chain->Add(temp_path);
    }

    /* find_pv_res(all_chain, pv_var[idx], pv_unit[idx], pv_range[idx]); */
    find_pv_pull(all_chain, pv_var[idx], pv_range[idx]);
    
    delete all_chain;

    return 0;
}
