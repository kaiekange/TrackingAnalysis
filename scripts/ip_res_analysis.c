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

#include "CMSplots/tdrStyle.c"
#include "CMSplots/CMS_lumi.c"
#include "CMSplots/draw_funcs.c"
#include "input_list.c"
/* #include "find_res.c" */

const TString figdir = "./figures/"+datatype+"/ip_res_analysis/";

void variable_range(TChain *mychain){

    lumi_sqrtS = "13.6 TeV, 2022" + all_era;
    
    TH1F * h_pt = new TH1F("h_pt", ";Track #it{p_{T}} [GeV];# tracks;", 100, 0, 20);
    TH1F * h_pt_low = new TH1F("h_pt_low", ";Track #it{p_{T}} [GeV];# tracks;", 100, 0, 2);
    TH1F * h_eta = new TH1F("h_eta", ";Track #it{#eta};# tracks;", 100, -3, 3);

    mychain->Project("h_pt", "pt");
    mychain->Project("h_pt_low", "pt");
    mychain->Project("h_eta", "eta");

    TCanvas *c0 = new TCanvas("c0", "", 800, 600);
    canvas_setup(c0);
    c0->SetLogy();
    h_pt->SetMaximum(pow(h_pt->GetMaximum(),1.3));
    h_pt->Draw("");
    CMS_lumi(c0);
    c0->Update();
    c0->RedrawAxis();
    c0->SaveAs(figdir+"varrange/pt.png");

    TCanvas *c1 = new TCanvas("c1", "", 800, 600);
    canvas_setup(c1);
    h_pt_low->SetMaximum(1.3*h_pt_low->GetMaximum());
    h_pt_low->SetMinimum(0.);
    h_pt_low->Draw("");
    CMS_lumi(c1);
    c1->Update();
    c1->RedrawAxis();
    c1->SaveAs(figdir+"varrange/pt_low.png");

    TCanvas *c2 = new TCanvas("c2", "", 800, 600);
    canvas_setup(c2);
    h_eta->SetMaximum(1.3*h_eta->GetMaximum());
    h_eta->SetMinimum(0.);
    h_eta->Draw("");
    CMS_lumi(c2);
    c2->Update();
    c2->RedrawAxis();
    c2->SaveAs(figdir+"varrange/eta.png");
}

float fit_draw(TH1F* hist, TString figpath, TString ip_axis, TString ip_unit) {
    
    lumi_sqrtS = "13.6 TeV, 2022"+all_era;
    
    TCanvas *canvas = new TCanvas("canvas", "", 800, 600);
    canvas_setup(canvas);
    RooRealVar ip_var("ip_var", "ip_var", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar mu("mu", "mu", hist->GetMean(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
    RooRealVar sigma("sigma", "sigma", hist->GetRMS(), 0., hist->GetRMS()*3);
    RooRealVar alpha1("alpha1", "alpha1", 1.5, 0., 10.);
    RooRealVar n1("n1", "n1", 10., 0., 20.);
    RooRealVar alpha2("alpha2", "alpha2", 1.5, 0., 10.);
    RooRealVar n2("n2", "n2", 10., 0., 20.); 
    RooCrystalBall model("model", "model", ip_var, mu, sigma, alpha1, n1, alpha2, n2);

    RooDataHist hdatahist("hdatahist", "", ip_var, hist);
    RooFitResult *fitResult = model.fitTo(hdatahist, RooFit::Save(true));
    fitResult->Print();
    RooPlot *frame = ip_var.frame();
    hdatahist.plotOn(frame);
    model.plotOn(frame);
    frame->SetYTitle("# tracks");
    frame->SetXTitle("Track IP in " + ip_axis + " [" + ip_unit + "]");
    frame->Draw();

    float mean = mu.getVal();
    float ip_res = sigma.getVal();
    RooAbsReal* integral = nullptr;

    int digit = std::floor(std::log10(ip_res));
    float prob = 0;
    float step = 0; 
    for(int i=0; i<4; i++){
        prob = 0;
        ip_res -= step;
        step = pow(10,digit);
        while (prob < 0.68) {
            ip_res += step;
            ip_var.setRange("symRange", mean - ip_res, mean + ip_res);
            integral = model.createIntegral(ip_var, RooFit::NormSet(ip_var), RooFit::Range("symRange"));
            prob = integral->getVal();
            delete integral;
        }
        digit--;
    }

    /* float ip_res = find_res(hist, mu.getVal()); */

    write_text(0.6, 0.88, datatype_text);
    write_text(0.6, 0.8, hist->GetTitle());
    write_text(0.6, 0.7, Form("Fit Results:"));
    write_text(0.6, 0.65, Form("#mu = %.3f #pm %.3f",   mu.getVal(),     mu.getError()));
    write_text(0.6, 0.6, Form("#sigma = %.3f #pm %.3f",  sigma.getVal(),  sigma.getError()));
    write_text(0.6, 0.55, Form("#alpha1 = %.3f #pm %.3f", alpha1.getVal(), alpha1.getError()));
    write_text(0.6, 0.5, Form("n1 = %.3f #pm %.3f",     n1.getVal(),     n1.getError()));
    write_text(0.6, 0.45, Form("#alpha2 = %.3f #pm %.3f", alpha2.getVal(), alpha2.getError()));
    write_text(0.6, 0.4, Form("n2 = %.3f #pm %.3f",     n2.getVal(),     n2.getError()));
    write_text(0.6, 0.35, Form("resolution = %.*f "+ip_unit, std::max(0, -1-digit), ip_res));
    
    CMS_lumi(canvas);
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath);

    return ip_res;
}

void draw_ip_res(int size, TGraph *gr[], float height, float floor, vector<TString> legendtext, TString figpath){

    lumi_sqrtS = "13.6 TeV, 2022"+all_era;
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
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + ".png");

    gr[0]->SetMaximum(height*100);
    gr[0]->SetMinimum(floor/100);
    canvas->SetLogy();
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + "_log.png");
}

void ip_res_pt(TChain *mychain, TString ip_var, TString ip_varname, TString ip_axis, TString ip_unit, float ip_range) {

    float eta_cut[2] = {1.4, 3.0};
    TString eta_name[2] = {"center", "endcap"};

    int nstep = 50;
    float pt_min = 0.1;
    float pt_inc = 0.2;
    float pt[nstep];
    float ip_res[2][nstep];
    
    for(int i=0; i<nstep; i++){
        float pt_lo = pt_min + pt_inc * i;
        float pt_hi = pt_lo + pt_inc;
        pt[i] = (pt_hi + pt_lo)/2.0;

        TCut mycut_pt = Form("pt > %f && pt < %f", pt_lo, pt_hi);
        
        for(int j=0; j<2; j++){
            TCut mycut = mycut_pt + Form("abs(eta) < %f", eta_cut[j]);
            TH1F *h_pt_tmp = new TH1F("h_pt_tmp", "", 200, -1*ip_range, ip_range);   
            mychain->Project("h_pt_tmp", ip_var, mycut);
            float mu = h_pt_tmp->GetMean();
            float sigma = h_pt_tmp->GetStdDev();
            delete h_pt_tmp;

            TH1F *hist = new TH1F("hist", "", 200, mu-8*sigma, mu+8*sigma);
            mychain->Project("hist", ip_var, mycut);
            hist->SetTitle(Form("#splitline{%.2f<#it{p_{T}}<%.2f}{0.0<|#eta|<%.1f}", pt_lo, pt_hi, eta_cut[j]));
            TString figpath = figdir+ip_varname+Form("_fit/pt_%s_%d.png", eta_name[j].Data(), i);
            ip_res[j][i] = fit_draw(hist, figpath, ip_axis, ip_unit);
            delete hist;
        } 
    }
    
    float height0 = *std::max_element(ip_res[0], ip_res[0]+nstep);
    float height1 = *std::max_element(ip_res[1], ip_res[1]+nstep);
    float height = std::max(height0, height1);
    float floor0 = *std::min_element(ip_res[0], ip_res[0]+nstep);
    float floor1 = *std::min_element(ip_res[1], ip_res[1]+nstep);
    float floor = std::min(height0, height1);
    
    TGraph *gr0 = new TGraph(nstep, pt, ip_res[0]);
    gr0->SetMarkerStyle(20);
    gr0->SetMarkerColor(kRed);
    gr0->GetXaxis()->SetTitle("Track #it{p_{T}} [GeV]");
    gr0->GetYaxis()->SetTitle("Track IP resolution in "+ip_axis+" ["+ip_unit+"]");
    TGraph *gr1 = new TGraph(nstep, pt, ip_res[1]);
    gr1->SetMarkerStyle(24);
    gr1->SetMarkerColor(kRed);
    TGraph *gr[2] = {gr0, gr1};
    TString figpath = figdir+ip_varname+"_pt";
    draw_ip_res(2, gr, height, floor, {"0.0<|#it{#eta}|<1.4", "0.0<|#it{#eta}|<3.0"}, figpath);
}

void ip_res_eta(TChain *mychain, TString ip_var, TString ip_varname, TString ip_axis, TString ip_unit, float ip_range) {

    int nstep = 106;
    float eta_inc = 0.05;
    float eta[nstep], eta_lo[nstep], eta_hi[nstep];
    float ip_res[nstep];
    eta_lo[0] = -2.8;      eta_hi[0] = -2.6;
    eta_lo[1] = -2.6;      eta_hi[1] = eta_lo[1] + eta_inc;
    eta_hi[nstep-1] = 2.8; eta_lo[nstep-1] = 2.6;
    eta_hi[nstep-2] = 2.6; eta_lo[nstep-2] = eta_hi[nstep-2] - eta_inc; 
    for(int i=2; i<nstep-2; i++){
        eta_lo[i] = eta_lo[i-1] + eta_inc;
        eta_hi[i] = eta_lo[i] + eta_inc;
    }

    for(int i=0; i<nstep; i++){
        eta[i] = (eta_lo[i] + eta_hi[i])/2;
        
        TCut mycut_eta = Form("eta > %f && eta < %f", eta_lo[i], eta_hi[i]);
        TCut mycut_pt = "pt > 1.0 && pt < 10.0";
        mycut_eta += mycut_pt;

        TH1F *h_eta_tmp = new TH1F("h_eta_tmp", "", 200, -1*ip_range, ip_range);
        mychain->Project("h_eta_tmp", ip_var, mycut_eta);
        float mu = h_eta_tmp->GetMean();
        float sigma = h_eta_tmp->GetStdDev();
        delete h_eta_tmp;

        TH1F *h_eta = new TH1F("h_eta", "", 200, mu-8*sigma, mu+8*sigma);
        mychain->Project("h_eta", ip_var, mycut_eta);
        h_eta->SetTitle(Form("#splitline{1<#it{p_{T}}<10 GeV}{%.2f<#it{#eta}<%.2f}", eta_lo[i], eta_hi[i]));
        TString figpath = figdir+ip_varname+Form("_fit/eta_%d.png", i);
        ip_res[i] = fit_draw(h_eta, figpath, ip_axis, ip_unit);
        delete h_eta;
    }
    
    float height = *std::max_element(ip_res, ip_res+nstep);
    float floor = *std::min_element(ip_res, ip_res+nstep);
    
    TGraph *gr0 = new TGraph(nstep, eta, ip_res);
    gr0->SetMarkerStyle(20);
    gr0->SetMarkerColor(kRed);
    gr0->GetXaxis()->SetTitle("Track #it{#eta}");
    gr0->GetYaxis()->SetTitle("Track IP resolution in "+ip_axis+" ["+ip_unit+"]");
    TGraph *gr[1] = {gr0};
    TString figpath = figdir+ip_varname+"_eta";
    draw_ip_res(1, gr, height, floor, {""}, figpath);
}

int ip_res_analysis(int idx) {
    setTDRStyle();

    TString ip_var[2] = {"d0_pvunbiased", "dz_pvunbiased/1000"};
    TString ip_varname[2] = {"ippv_xy", "ippv_z"};
    TString ip_axis[2] = {"xy", "z"};
    TString ip_unit[2] = {"#mum", "mm"};
    float ip_range[2] = {2000, 8};

    
    const int nera = 6;
    TChain *all_chain = new TChain("mytree");
    for(int i=0; i<nera; i++){
        TString temp_path = filepath[i] + "ip_res_analysis.root";
        all_chain->Add(temp_path);
    }

    if(idx == 0) ip_res_pt(all_chain, ip_var[0], ip_varname[0], ip_axis[0], ip_unit[0], ip_range[0]);
    else if(idx == 1) ip_res_eta(all_chain, ip_var[0], ip_varname[0], ip_axis[0], ip_unit[0], ip_range[0]);
    else if(idx == 2) ip_res_pt(all_chain, ip_var[1], ip_varname[1], ip_axis[1], ip_unit[1], ip_range[1]);
    else if(idx == 3) ip_res_eta(all_chain, ip_var[1], ip_varname[1], ip_axis[1], ip_unit[1], ip_range[1]);
    else variable_range(all_chain);

    delete all_chain;

    return 0;
}
