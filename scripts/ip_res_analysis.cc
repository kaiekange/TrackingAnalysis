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

std::pair<float, float> fit_draw(TH1F* hist, TString figpath) {
    
    lumi_sqrtS = "13.6 TeV, 2022"+all_era;
    
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
    float reso = (nreso - 0.5) * hist->GetBinWidth(1);
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
    write_text(0.68, 0.88, datatype_text);
    write_text(0.68, 0.8, hist->GetTitle());
    write_text(0.68, 0.7, Form("Fit Results:"));
    write_text(0.68, 0.65, Form("#mu = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(mu.getVal())))), mu.getVal(), std::max(0, 2-(int)floor(log10(abs(mu.getVal())))), mu.getError()));
    write_text(0.68, 0.6, Form("#sigma_{1} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma1.getVal())))), sigma1.getVal(), std::max(0, 2-(int)floor(log10(abs(sigma1.getVal())))), sigma1.getError()));
    write_text(0.68, 0.55, Form("#sigma_{2} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma2.getVal())))), sigma2.getVal(), std::max(0, 2-(int)floor(log10(abs(sigma2.getVal())))), sigma2.getError()));
    write_text(0.68, 0.5, Form("#sigma_{3} = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma3.getVal())))), sigma3.getVal(), std::max(0, 2-(int)floor(log10(abs(sigma3.getVal())))), sigma3.getError()));
    write_text(0.68, 0.45, Form("reso = %.*f #pm %.*f", std::max(0, 2-(int)floor(log10(abs(sigma3.getVal())))), reso, std::max(0, 2-(int)floor(log10(abs(sigma3.getVal())))), resoerr));
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
    canvas->Update();
    canvas->SaveAs(figpath+".png");

    return {reso, resoerr};
}

int ip_res_analysis(int idx) {

    /* if(gSystem->AccessPathName("./figures")) gSystem->MakeDirectory("./figures"); */
    /* if(gSystem->AccessPathName("./figures/"+datatype)) gSystem->MakeDirectory("./figures/"+datatype); */
    /* if(gSystem->AccessPathName(figdir)) gSystem->MakeDirectory(figdir); */
    /* if(gSystem->AccessPathName(figdir+"ippv_xy_fit")) gSystem->MakeDirectory(figdir+"ippv_xy_fit"); */
    /* if(gSystem->AccessPathName(figdir+"ippv_z_fit")) gSystem->MakeDirectory(figdir+"ippv_z_fit"); */
    /* if(gSystem->AccessPathName(figdir+"ipbs_xy_fit")) gSystem->MakeDirectory(figdir+"ipbs_xy_fit"); */
    /* if(gSystem->AccessPathName(figdir+"ipbs_z_fit")) gSystem->MakeDirectory(figdir+"ipbs_z_fit"); */
    /* if(gSystem->AccessPathName("./json/"+datatype+"/ip_res")) gSystem->MakeDirectory("./json/"+datatype+"/ip_res"); */

    setTDRStyle();

    TChain *mychain = new TChain("mytree");
    mychain->Add(anaroot);

    std::ifstream infile("./json/"+datatype+"/binning.json");
    nlohmann::json binning;
    infile >> binning;

    std::vector<float> pv_trk_pt_edges = binning["pv_trk_pt"].get<std::vector<float>>();
    std::vector<float> pv_trk_eta_edges = binning["pv_trk_eta"].get<std::vector<float>>();
    std::vector<float> pv_trk_phi_edges = binning["pv_trk_phi"].get<std::vector<float>>();
    
    TString ptcut = Form("%.3f<#it{p_{T}}<%.3f GeV", pv_trk_pt_edges[idx], pv_trk_pt_edges[idx+1]);
    TString etacut = Form("%.2f<#it{#eta}<%.2f", pv_trk_eta_edges[idx], pv_trk_eta_edges[idx+1]);
    TString phicut = Form("%.2f<#it{#phi}<%.2f", pv_trk_phi_edges[idx], pv_trk_phi_edges[idx+1]);

    std::vector<float> * pv_trk_pt = nullptr;
    std::vector<float> * pv_trk_eta = nullptr;
    std::vector<float> * pv_trk_phi = nullptr;
    std::vector<float> * pv_trk_d0_pvunbiased = nullptr;
    std::vector<float> * pv_trk_dz_pvunbiased = nullptr;
    std::vector<float> * pv_trk_d0_bs = nullptr;
    std::vector<float> * pv_trk_dz_bs = nullptr;
    mychain->SetBranchAddress("pv_trk_pt", &pv_trk_pt);
    mychain->SetBranchAddress("pv_trk_eta", &pv_trk_eta);
    mychain->SetBranchAddress("pv_trk_phi", &pv_trk_phi);
    mychain->SetBranchAddress("pv_trk_d0_pvunbiased", &pv_trk_d0_pvunbiased);
    mychain->SetBranchAddress("pv_trk_dz_pvunbiased", &pv_trk_dz_pvunbiased);
    mychain->SetBranchAddress("pv_trk_d0_bs", &pv_trk_d0_bs);
    mychain->SetBranchAddress("pv_trk_dz_bs", &pv_trk_dz_bs);
    
    std::vector<float> d0_pv_pt_loeta_vec, d0_pv_pt_hieta_vec, d0_pv_eta_lopt_vec, d0_pv_eta_hipt_vec, d0_pv_eta_ulpt_vec, d0_pv_phi_lopt_vec, d0_pv_phi_hipt_vec, d0_pv_phi_ulpt_vec;
    std::vector<float> dz_pv_pt_loeta_vec, dz_pv_pt_hieta_vec, dz_pv_eta_lopt_vec, dz_pv_eta_hipt_vec, dz_pv_eta_ulpt_vec, dz_pv_phi_lopt_vec, dz_pv_phi_hipt_vec, dz_pv_phi_ulpt_vec;
    std::vector<float> d0_bs_pt_loeta_vec, d0_bs_pt_hieta_vec, d0_bs_eta_lopt_vec, d0_bs_eta_hipt_vec, d0_bs_eta_ulpt_vec, d0_bs_phi_lopt_vec, d0_bs_phi_hipt_vec, d0_bs_phi_ulpt_vec;
    std::vector<float> dz_bs_pt_loeta_vec, dz_bs_pt_hieta_vec, dz_bs_eta_lopt_vec, dz_bs_eta_hipt_vec, dz_bs_eta_ulpt_vec, dz_bs_phi_lopt_vec, dz_bs_phi_hipt_vec, dz_bs_phi_ulpt_vec;

    d0_pv_pt_loeta_vec.clear();
    dz_pv_pt_loeta_vec.clear();
    d0_bs_pt_loeta_vec.clear();
    dz_bs_pt_loeta_vec.clear();
    d0_pv_pt_hieta_vec.clear();
    dz_pv_pt_hieta_vec.clear();
    d0_bs_pt_hieta_vec.clear();
    dz_bs_pt_hieta_vec.clear();

    d0_pv_eta_lopt_vec.clear();
    dz_pv_eta_lopt_vec.clear();
    d0_bs_eta_lopt_vec.clear();
    dz_bs_eta_lopt_vec.clear();
    d0_pv_eta_hipt_vec.clear();
    dz_pv_eta_hipt_vec.clear();
    d0_bs_eta_hipt_vec.clear();
    dz_bs_eta_hipt_vec.clear();
    d0_pv_eta_ulpt_vec.clear();
    dz_pv_eta_ulpt_vec.clear();
    d0_bs_eta_ulpt_vec.clear();
    dz_bs_eta_ulpt_vec.clear();

    d0_pv_phi_lopt_vec.clear();
    dz_pv_phi_lopt_vec.clear();
    d0_bs_phi_lopt_vec.clear();
    dz_bs_phi_lopt_vec.clear();
    d0_pv_phi_hipt_vec.clear();
    dz_pv_phi_hipt_vec.clear();
    d0_bs_phi_hipt_vec.clear();
    dz_bs_phi_hipt_vec.clear();
    d0_pv_phi_ulpt_vec.clear();
    dz_pv_phi_ulpt_vec.clear();
    d0_bs_phi_ulpt_vec.clear();
    dz_bs_phi_ulpt_vec.clear();

    int nentries = mychain->GetEntries();

    for(int i=0; i<nentries; i++) {

        mychain->GetEntry(i);

        int vecsize = pv_trk_pt->size();

        for(int j=0; j<vecsize; j++){
            if( pv_trk_pt->at(j) > pv_trk_pt_edges[idx] &&  pv_trk_pt->at(j) < pv_trk_pt_edges[idx+1] ){
                if( abs( pv_trk_eta->at(j) ) < 1.4 ){
                    d0_pv_pt_loeta_vec.push_back(pv_trk_d0_pvunbiased->at(j));
                    dz_pv_pt_loeta_vec.push_back(pv_trk_dz_pvunbiased->at(j)/1000);
                    d0_bs_pt_loeta_vec.push_back(pv_trk_d0_bs->at(j));
                    dz_bs_pt_loeta_vec.push_back(pv_trk_dz_bs->at(j)/1000);
                }
                if( abs( pv_trk_eta->at(j) ) < 3.0 ){
                    d0_pv_pt_hieta_vec.push_back(pv_trk_d0_pvunbiased->at(j));
                    dz_pv_pt_hieta_vec.push_back(pv_trk_dz_pvunbiased->at(j)/1000);
                    d0_bs_pt_hieta_vec.push_back(pv_trk_d0_bs->at(j));
                    dz_bs_pt_hieta_vec.push_back(pv_trk_dz_bs->at(j)/1000);
                }
            }

            if( pv_trk_eta->at(j) > pv_trk_eta_edges[idx] &&  pv_trk_eta->at(j) < pv_trk_eta_edges[idx+1] ){
                if( pv_trk_pt->at(j) > 0.1 && pv_trk_pt->at(j) < 1 ){
                    d0_pv_eta_lopt_vec.push_back(pv_trk_d0_pvunbiased->at(j));
                    dz_pv_eta_lopt_vec.push_back(pv_trk_dz_pvunbiased->at(j)/1000);
                    d0_bs_eta_lopt_vec.push_back(pv_trk_d0_bs->at(j));
                    dz_bs_eta_lopt_vec.push_back(pv_trk_dz_bs->at(j)/1000);
                }
                else if( pv_trk_pt->at(j) > 1 && pv_trk_pt->at(j) < 5 ){
                    d0_pv_eta_hipt_vec.push_back(pv_trk_d0_pvunbiased->at(j));
                    dz_pv_eta_hipt_vec.push_back(pv_trk_dz_pvunbiased->at(j)/1000);
                    d0_bs_eta_hipt_vec.push_back(pv_trk_d0_bs->at(j));
                    dz_bs_eta_hipt_vec.push_back(pv_trk_dz_bs->at(j)/1000);
                }
                else if( pv_trk_pt->at(j) > 5 && pv_trk_pt->at(j) < 15 ){
                    d0_pv_eta_ulpt_vec.push_back(pv_trk_d0_pvunbiased->at(j));
                    dz_pv_eta_ulpt_vec.push_back(pv_trk_dz_pvunbiased->at(j)/1000);
                    d0_bs_eta_ulpt_vec.push_back(pv_trk_d0_bs->at(j));
                    dz_bs_eta_ulpt_vec.push_back(pv_trk_dz_bs->at(j)/1000);
                }
            }

            if( pv_trk_phi->at(j) > pv_trk_phi_edges[idx] &&  pv_trk_phi->at(j) < pv_trk_phi_edges[idx+1] ){
                if( pv_trk_pt->at(j) > 0.1 && pv_trk_pt->at(j) < 1 ){
                    d0_pv_phi_lopt_vec.push_back(pv_trk_d0_pvunbiased->at(j));
                    dz_pv_phi_lopt_vec.push_back(pv_trk_dz_pvunbiased->at(j)/1000);
                    d0_bs_phi_lopt_vec.push_back(pv_trk_d0_bs->at(j));
                    dz_bs_phi_lopt_vec.push_back(pv_trk_dz_bs->at(j)/1000);
                }
                else if( pv_trk_pt->at(j) > 1 && pv_trk_pt->at(j) < 5 ){
                    d0_pv_phi_hipt_vec.push_back(pv_trk_d0_pvunbiased->at(j));
                    dz_pv_phi_hipt_vec.push_back(pv_trk_dz_pvunbiased->at(j)/1000);
                    d0_bs_phi_hipt_vec.push_back(pv_trk_d0_bs->at(j));
                    dz_bs_phi_hipt_vec.push_back(pv_trk_dz_bs->at(j)/1000);
                }
                else if( pv_trk_pt->at(j) > 5 && pv_trk_pt->at(j) < 15 ){
                    d0_pv_phi_ulpt_vec.push_back(pv_trk_d0_pvunbiased->at(j));
                    dz_pv_phi_ulpt_vec.push_back(pv_trk_dz_pvunbiased->at(j)/1000);
                    d0_bs_phi_ulpt_vec.push_back(pv_trk_d0_bs->at(j));
                    dz_bs_phi_ulpt_vec.push_back(pv_trk_dz_bs->at(j)/1000);
                }
            }
        } 
    }

    float d0_pv_pt_loeta_mean = TMath::Mean(d0_pv_pt_loeta_vec.begin(), d0_pv_pt_loeta_vec.end());
    float dz_pv_pt_loeta_mean = TMath::Mean(dz_pv_pt_loeta_vec.begin(), dz_pv_pt_loeta_vec.end());
    float d0_bs_pt_loeta_mean = TMath::Mean(d0_bs_pt_loeta_vec.begin(), d0_bs_pt_loeta_vec.end());
    float dz_bs_pt_loeta_mean = TMath::Mean(dz_bs_pt_loeta_vec.begin(), dz_bs_pt_loeta_vec.end());
    float d0_pv_pt_hieta_mean = TMath::Mean(d0_pv_pt_hieta_vec.begin(), d0_pv_pt_hieta_vec.end());
    float dz_pv_pt_hieta_mean = TMath::Mean(dz_pv_pt_hieta_vec.begin(), dz_pv_pt_hieta_vec.end());
    float d0_bs_pt_hieta_mean = TMath::Mean(d0_bs_pt_hieta_vec.begin(), d0_bs_pt_hieta_vec.end());
    float dz_bs_pt_hieta_mean = TMath::Mean(dz_bs_pt_hieta_vec.begin(), dz_bs_pt_hieta_vec.end());

    float d0_pv_eta_lopt_mean = TMath::Mean(d0_pv_eta_lopt_vec.begin(), d0_pv_eta_lopt_vec.end());
    float dz_pv_eta_lopt_mean = TMath::Mean(dz_pv_eta_lopt_vec.begin(), dz_pv_eta_lopt_vec.end());
    float d0_bs_eta_lopt_mean = TMath::Mean(d0_bs_eta_lopt_vec.begin(), d0_bs_eta_lopt_vec.end());
    float dz_bs_eta_lopt_mean = TMath::Mean(dz_bs_eta_lopt_vec.begin(), dz_bs_eta_lopt_vec.end());
    float d0_pv_eta_hipt_mean = TMath::Mean(d0_pv_eta_hipt_vec.begin(), d0_pv_eta_hipt_vec.end());
    float dz_pv_eta_hipt_mean = TMath::Mean(dz_pv_eta_hipt_vec.begin(), dz_pv_eta_hipt_vec.end());
    float d0_bs_eta_hipt_mean = TMath::Mean(d0_bs_eta_hipt_vec.begin(), d0_bs_eta_hipt_vec.end());
    float dz_bs_eta_hipt_mean = TMath::Mean(dz_bs_eta_hipt_vec.begin(), dz_bs_eta_hipt_vec.end());
    float d0_pv_eta_ulpt_mean = TMath::Mean(d0_pv_eta_ulpt_vec.begin(), d0_pv_eta_ulpt_vec.end());
    float dz_pv_eta_ulpt_mean = TMath::Mean(dz_pv_eta_ulpt_vec.begin(), dz_pv_eta_ulpt_vec.end());
    float d0_bs_eta_ulpt_mean = TMath::Mean(d0_bs_eta_ulpt_vec.begin(), d0_bs_eta_ulpt_vec.end());
    float dz_bs_eta_ulpt_mean = TMath::Mean(dz_bs_eta_ulpt_vec.begin(), dz_bs_eta_ulpt_vec.end());

    float d0_pv_phi_lopt_mean = TMath::Mean(d0_pv_phi_lopt_vec.begin(), d0_pv_phi_lopt_vec.end());
    float dz_pv_phi_lopt_mean = TMath::Mean(dz_pv_phi_lopt_vec.begin(), dz_pv_phi_lopt_vec.end());
    float d0_bs_phi_lopt_mean = TMath::Mean(d0_bs_phi_lopt_vec.begin(), d0_bs_phi_lopt_vec.end());
    float dz_bs_phi_lopt_mean = TMath::Mean(dz_bs_phi_lopt_vec.begin(), dz_bs_phi_lopt_vec.end());
    float d0_pv_phi_hipt_mean = TMath::Mean(d0_pv_phi_hipt_vec.begin(), d0_pv_phi_hipt_vec.end());
    float dz_pv_phi_hipt_mean = TMath::Mean(dz_pv_phi_hipt_vec.begin(), dz_pv_phi_hipt_vec.end());
    float d0_bs_phi_hipt_mean = TMath::Mean(d0_bs_phi_hipt_vec.begin(), d0_bs_phi_hipt_vec.end());
    float dz_bs_phi_hipt_mean = TMath::Mean(dz_bs_phi_hipt_vec.begin(), dz_bs_phi_hipt_vec.end());
    float d0_pv_phi_ulpt_mean = TMath::Mean(d0_pv_phi_ulpt_vec.begin(), d0_pv_phi_ulpt_vec.end());
    float dz_pv_phi_ulpt_mean = TMath::Mean(dz_pv_phi_ulpt_vec.begin(), dz_pv_phi_ulpt_vec.end());
    float d0_bs_phi_ulpt_mean = TMath::Mean(d0_bs_phi_ulpt_vec.begin(), d0_bs_phi_ulpt_vec.end());
    float dz_bs_phi_ulpt_mean = TMath::Mean(dz_bs_phi_ulpt_vec.begin(), dz_bs_phi_ulpt_vec.end());

    float d0_pv_pt_loeta_stddev = TMath::StdDev(d0_pv_pt_loeta_vec.begin(), d0_pv_pt_loeta_vec.end());
    float dz_pv_pt_loeta_stddev = TMath::StdDev(dz_pv_pt_loeta_vec.begin(), dz_pv_pt_loeta_vec.end());
    float d0_bs_pt_loeta_stddev = TMath::StdDev(d0_bs_pt_loeta_vec.begin(), d0_bs_pt_loeta_vec.end());
    float dz_bs_pt_loeta_stddev = TMath::StdDev(dz_bs_pt_loeta_vec.begin(), dz_bs_pt_loeta_vec.end());
    float d0_pv_pt_hieta_stddev = TMath::StdDev(d0_pv_pt_hieta_vec.begin(), d0_pv_pt_hieta_vec.end());
    float dz_pv_pt_hieta_stddev = TMath::StdDev(dz_pv_pt_hieta_vec.begin(), dz_pv_pt_hieta_vec.end());
    float d0_bs_pt_hieta_stddev = TMath::StdDev(d0_bs_pt_hieta_vec.begin(), d0_bs_pt_hieta_vec.end());
    float dz_bs_pt_hieta_stddev = TMath::StdDev(dz_bs_pt_hieta_vec.begin(), dz_bs_pt_hieta_vec.end());

    float d0_pv_eta_lopt_stddev = TMath::StdDev(d0_pv_eta_lopt_vec.begin(), d0_pv_eta_lopt_vec.end());
    float dz_pv_eta_lopt_stddev = TMath::StdDev(dz_pv_eta_lopt_vec.begin(), dz_pv_eta_lopt_vec.end());
    float d0_bs_eta_lopt_stddev = TMath::StdDev(d0_bs_eta_lopt_vec.begin(), d0_bs_eta_lopt_vec.end());
    float dz_bs_eta_lopt_stddev = TMath::StdDev(dz_bs_eta_lopt_vec.begin(), dz_bs_eta_lopt_vec.end());
    float d0_pv_eta_hipt_stddev = TMath::StdDev(d0_pv_eta_hipt_vec.begin(), d0_pv_eta_hipt_vec.end());
    float dz_pv_eta_hipt_stddev = TMath::StdDev(dz_pv_eta_hipt_vec.begin(), dz_pv_eta_hipt_vec.end());
    float d0_bs_eta_hipt_stddev = TMath::StdDev(d0_bs_eta_hipt_vec.begin(), d0_bs_eta_hipt_vec.end());
    float dz_bs_eta_hipt_stddev = TMath::StdDev(dz_bs_eta_hipt_vec.begin(), dz_bs_eta_hipt_vec.end());
    float d0_pv_eta_ulpt_stddev = TMath::StdDev(d0_pv_eta_ulpt_vec.begin(), d0_pv_eta_ulpt_vec.end());
    float dz_pv_eta_ulpt_stddev = TMath::StdDev(dz_pv_eta_ulpt_vec.begin(), dz_pv_eta_ulpt_vec.end());
    float d0_bs_eta_ulpt_stddev = TMath::StdDev(d0_bs_eta_ulpt_vec.begin(), d0_bs_eta_ulpt_vec.end());
    float dz_bs_eta_ulpt_stddev = TMath::StdDev(dz_bs_eta_ulpt_vec.begin(), dz_bs_eta_ulpt_vec.end());

    float d0_pv_phi_lopt_stddev = TMath::StdDev(d0_pv_phi_lopt_vec.begin(), d0_pv_phi_lopt_vec.end());
    float dz_pv_phi_lopt_stddev = TMath::StdDev(dz_pv_phi_lopt_vec.begin(), dz_pv_phi_lopt_vec.end());
    float d0_bs_phi_lopt_stddev = TMath::StdDev(d0_bs_phi_lopt_vec.begin(), d0_bs_phi_lopt_vec.end());
    float dz_bs_phi_lopt_stddev = TMath::StdDev(dz_bs_phi_lopt_vec.begin(), dz_bs_phi_lopt_vec.end());
    float d0_pv_phi_hipt_stddev = TMath::StdDev(d0_pv_phi_hipt_vec.begin(), d0_pv_phi_hipt_vec.end());
    float dz_pv_phi_hipt_stddev = TMath::StdDev(dz_pv_phi_hipt_vec.begin(), dz_pv_phi_hipt_vec.end());
    float d0_bs_phi_hipt_stddev = TMath::StdDev(d0_bs_phi_hipt_vec.begin(), d0_bs_phi_hipt_vec.end());
    float dz_bs_phi_hipt_stddev = TMath::StdDev(dz_bs_phi_hipt_vec.begin(), dz_bs_phi_hipt_vec.end());
    float d0_pv_phi_ulpt_stddev = TMath::StdDev(d0_pv_phi_ulpt_vec.begin(), d0_pv_phi_ulpt_vec.end());
    float dz_pv_phi_ulpt_stddev = TMath::StdDev(dz_pv_phi_ulpt_vec.begin(), dz_pv_phi_ulpt_vec.end());
    float d0_bs_phi_ulpt_stddev = TMath::StdDev(d0_bs_phi_ulpt_vec.begin(), d0_bs_phi_ulpt_vec.end());
    float dz_bs_phi_ulpt_stddev = TMath::StdDev(dz_bs_phi_ulpt_vec.begin(), dz_bs_phi_ulpt_vec.end());

    
    TH1F *h_d0_pv_pt_loeta = new TH1F("h_d0_pv_pt_loeta", "#splitline{"+ptcut+"}{|#it{#eta}|<1.4};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 200, d0_pv_pt_loeta_mean-8*d0_pv_pt_loeta_stddev, d0_pv_pt_loeta_mean+8*d0_pv_pt_loeta_stddev);
    TH1F *h_dz_pv_pt_loeta = new TH1F("h_dz_pv_pt_loeta", "#splitline{"+ptcut+"}{|#it{#eta}|<1.4};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 200, dz_pv_pt_loeta_mean-8*dz_pv_pt_loeta_stddev, dz_pv_pt_loeta_mean+8*dz_pv_pt_loeta_stddev);
    TH1F *h_d0_bs_pt_loeta = new TH1F("h_d0_bs_pt_loeta", "#splitline{"+ptcut+"}{|#it{#eta}|<1.4};Track IP #it{d_{xy}} (IPBS) [#mum];# tracks", 200, d0_bs_pt_loeta_mean-8*d0_bs_pt_loeta_stddev, d0_bs_pt_loeta_mean+8*d0_bs_pt_loeta_stddev);
    TH1F *h_dz_bs_pt_loeta = new TH1F("h_dz_bs_pt_loeta", "#splitline{"+ptcut+"}{|#it{#eta}|<1.4};Track IP #it{d_{z}} (IPBS) [mm];# tracks", 200, dz_bs_pt_loeta_mean-8*dz_bs_pt_loeta_stddev, dz_bs_pt_loeta_mean+8*dz_bs_pt_loeta_stddev);
    TH1F *h_d0_pv_pt_hieta = new TH1F("h_d0_pv_pt_hieta", "#splitline{"+ptcut+"}{|#it{#eta}|<3.0};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 200, d0_pv_pt_hieta_mean-8*d0_pv_pt_hieta_stddev, d0_pv_pt_hieta_mean+8*d0_pv_pt_hieta_stddev);
    TH1F *h_dz_pv_pt_hieta = new TH1F("h_dz_pv_pt_hieta", "#splitline{"+ptcut+"}{|#it{#eta}|<3.0};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 200, dz_pv_pt_hieta_mean-8*dz_pv_pt_hieta_stddev, dz_pv_pt_hieta_mean+8*dz_pv_pt_hieta_stddev);
    TH1F *h_d0_bs_pt_hieta = new TH1F("h_d0_bs_pt_hieta", "#splitline{"+ptcut+"}{|#it{#eta}|<3.0};Track IP #it{d_{xy}} (IPBS) [#mum];# tracks", 200, d0_bs_pt_hieta_mean-8*d0_bs_pt_hieta_stddev, d0_bs_pt_hieta_mean+8*d0_bs_pt_hieta_stddev);
    TH1F *h_dz_bs_pt_hieta = new TH1F("h_dz_bs_pt_hieta", "#splitline{"+ptcut+"}{|#it{#eta}|<3.0};Track IP #it{d_{z}} (IPBS) [mm];# tracks", 200, dz_bs_pt_hieta_mean-8*dz_bs_pt_hieta_stddev, dz_bs_pt_hieta_mean+8*dz_bs_pt_hieta_stddev);

    TH1F *h_d0_pv_eta_lopt = new TH1F("h_d0_pv_eta_lopt", "#splitline{"+etacut+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 200, d0_pv_eta_lopt_mean-8*d0_pv_eta_lopt_stddev, d0_pv_eta_lopt_mean+8*d0_pv_eta_lopt_stddev);
    TH1F *h_dz_pv_eta_lopt = new TH1F("h_dz_pv_eta_lopt", "#splitline{"+etacut+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 200, dz_pv_eta_lopt_mean-8*dz_pv_eta_lopt_stddev, dz_pv_eta_lopt_mean+8*dz_pv_eta_lopt_stddev);
    TH1F *h_d0_bs_eta_lopt = new TH1F("h_d0_bs_eta_lopt", "#splitline{"+etacut+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} (IPBS) [#mum];# tracks", 200, d0_bs_eta_lopt_mean-8*d0_bs_eta_lopt_stddev, d0_bs_eta_lopt_mean+8*d0_bs_eta_lopt_stddev);
    TH1F *h_dz_bs_eta_lopt = new TH1F("h_dz_bs_eta_lopt", "#splitline{"+etacut+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} (IPBS) [mm];# tracks", 200, dz_bs_eta_lopt_mean-8*dz_bs_eta_lopt_stddev, dz_bs_eta_lopt_mean+8*dz_bs_eta_lopt_stddev);
    TH1F *h_d0_pv_eta_hipt = new TH1F("h_d0_pv_eta_hipt", "#splitline{"+etacut+"}{1<#it{p_{T}}<5 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 200, d0_pv_eta_hipt_mean-8*d0_pv_eta_hipt_stddev, d0_pv_eta_hipt_mean+8*d0_pv_eta_hipt_stddev);
    TH1F *h_dz_pv_eta_hipt = new TH1F("h_dz_pv_eta_hipt", "#splitline{"+etacut+"}{1<#it{p_{T}}<5 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 200, dz_pv_eta_hipt_mean-8*dz_pv_eta_hipt_stddev, dz_pv_eta_hipt_mean+8*dz_pv_eta_hipt_stddev);
    TH1F *h_d0_bs_eta_hipt = new TH1F("h_d0_bs_eta_hipt", "#splitline{"+etacut+"}{1<#it{p_{T}}<5 GeV};Track IP #it{d_{xy}} (IPBS) [#mum];# tracks", 200, d0_bs_eta_hipt_mean-8*d0_bs_eta_hipt_stddev, d0_bs_eta_hipt_mean+8*d0_bs_eta_hipt_stddev);
    TH1F *h_dz_bs_eta_hipt = new TH1F("h_dz_bs_eta_hipt", "#splitline{"+etacut+"}{1<#it{p_{T}}<5 GeV};Track IP #it{d_{z}} (IPBS) [mm];# tracks", 200, dz_bs_eta_hipt_mean-8*dz_bs_eta_hipt_stddev, dz_bs_eta_hipt_mean+8*dz_bs_eta_hipt_stddev);
    TH1F *h_d0_pv_eta_ulpt = new TH1F("h_d0_pv_eta_ulpt", "#splitline{"+etacut+"}{5<#it{p_{T}}<15 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 200, d0_pv_eta_ulpt_mean-8*d0_pv_eta_ulpt_stddev, d0_pv_eta_ulpt_mean+8*d0_pv_eta_ulpt_stddev);
    TH1F *h_dz_pv_eta_ulpt = new TH1F("h_dz_pv_eta_ulpt", "#splitline{"+etacut+"}{5<#it{p_{T}}<15 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 200, dz_pv_eta_ulpt_mean-8*dz_pv_eta_ulpt_stddev, dz_pv_eta_ulpt_mean+8*dz_pv_eta_ulpt_stddev);
    TH1F *h_d0_bs_eta_ulpt = new TH1F("h_d0_bs_eta_ulpt", "#splitline{"+etacut+"}{5<#it{p_{T}}<15 GeV};Track IP #it{d_{xy}} (IPBS) [#mum];# tracks", 200, d0_bs_eta_ulpt_mean-8*d0_bs_eta_ulpt_stddev, d0_bs_eta_ulpt_mean+8*d0_bs_eta_ulpt_stddev);
    TH1F *h_dz_bs_eta_ulpt = new TH1F("h_dz_bs_eta_ulpt", "#splitline{"+etacut+"}{5<#it{p_{T}}<15 GeV};Track IP #it{d_{z}} (IPBS) [mm];# tracks", 200, dz_bs_eta_ulpt_mean-8*dz_bs_eta_ulpt_stddev, dz_bs_eta_ulpt_mean+8*dz_bs_eta_ulpt_stddev);

    TH1F *h_d0_pv_phi_lopt = new TH1F("h_d0_pv_phi_lopt", "#splitline{"+phicut+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 200, d0_pv_phi_lopt_mean-8*d0_pv_phi_lopt_stddev, d0_pv_phi_lopt_mean+8*d0_pv_phi_lopt_stddev);
    TH1F *h_dz_pv_phi_lopt = new TH1F("h_dz_pv_phi_lopt", "#splitline{"+phicut+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 200, dz_pv_phi_lopt_mean-8*dz_pv_phi_lopt_stddev, dz_pv_phi_lopt_mean+8*dz_pv_phi_lopt_stddev);
    TH1F *h_d0_bs_phi_lopt = new TH1F("h_d0_bs_phi_lopt", "#splitline{"+phicut+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{xy}} (IPBS) [#mum];# tracks", 200, d0_bs_phi_lopt_mean-8*d0_bs_phi_lopt_stddev, d0_bs_phi_lopt_mean+8*d0_bs_phi_lopt_stddev);
    TH1F *h_dz_bs_phi_lopt = new TH1F("h_dz_bs_phi_lopt", "#splitline{"+phicut+"}{0.1<#it{p_{T}}<1 GeV};Track IP #it{d_{z}} (IPBS) [mm];# tracks", 200, dz_bs_phi_lopt_mean-8*dz_bs_phi_lopt_stddev, dz_bs_phi_lopt_mean+8*dz_bs_phi_lopt_stddev);
    TH1F *h_d0_pv_phi_hipt = new TH1F("h_d0_pv_phi_hipt", "#splitline{"+phicut+"}{1<#it{p_{T}}<5 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 200, d0_pv_phi_hipt_mean-8*d0_pv_phi_hipt_stddev, d0_pv_phi_hipt_mean+8*d0_pv_phi_hipt_stddev);
    TH1F *h_dz_pv_phi_hipt = new TH1F("h_dz_pv_phi_hipt", "#splitline{"+phicut+"}{1<#it{p_{T}}<5 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 200, dz_pv_phi_hipt_mean-8*dz_pv_phi_hipt_stddev, dz_pv_phi_hipt_mean+8*dz_pv_phi_hipt_stddev);
    TH1F *h_d0_bs_phi_hipt = new TH1F("h_d0_bs_phi_hipt", "#splitline{"+phicut+"}{1<#it{p_{T}}<5 GeV};Track IP #it{d_{xy}} (IPBS) [#mum];# tracks", 200, d0_bs_phi_hipt_mean-8*d0_bs_phi_hipt_stddev, d0_bs_phi_hipt_mean+8*d0_bs_phi_hipt_stddev);
    TH1F *h_dz_bs_phi_hipt = new TH1F("h_dz_bs_phi_hipt", "#splitline{"+phicut+"}{1<#it{p_{T}}<5 GeV};Track IP #it{d_{z}} (IPBS) [mm];# tracks", 200, dz_bs_phi_hipt_mean-8*dz_bs_phi_hipt_stddev, dz_bs_phi_hipt_mean+8*dz_bs_phi_hipt_stddev);
    TH1F *h_d0_pv_phi_ulpt = new TH1F("h_d0_pv_phi_ulpt", "#splitline{"+phicut+"}{5<#it{p_{T}}<15 GeV};Track IP #it{d_{xy}} (IPPV) [#mum];# tracks", 200, d0_pv_phi_ulpt_mean-8*d0_pv_phi_ulpt_stddev, d0_pv_phi_ulpt_mean+8*d0_pv_phi_ulpt_stddev);
    TH1F *h_dz_pv_phi_ulpt = new TH1F("h_dz_pv_phi_ulpt", "#splitline{"+phicut+"}{5<#it{p_{T}}<15 GeV};Track IP #it{d_{z}} (IPPV) [mm];# tracks", 200, dz_pv_phi_ulpt_mean-8*dz_pv_phi_ulpt_stddev, dz_pv_phi_ulpt_mean+8*dz_pv_phi_ulpt_stddev);
    TH1F *h_d0_bs_phi_ulpt = new TH1F("h_d0_bs_phi_ulpt", "#splitline{"+phicut+"}{5<#it{p_{T}}<15 GeV};Track IP #it{d_{xy}} (IPBS) [#mum];# tracks", 200, d0_bs_phi_ulpt_mean-8*d0_bs_phi_ulpt_stddev, d0_bs_phi_ulpt_mean+8*d0_bs_phi_ulpt_stddev);
    TH1F *h_dz_bs_phi_ulpt = new TH1F("h_dz_bs_phi_ulpt", "#splitline{"+phicut+"}{5<#it{p_{T}}<15 GeV};Track IP #it{d_{z}} (IPBS) [mm];# tracks", 200, dz_bs_phi_ulpt_mean-8*dz_bs_phi_ulpt_stddev, dz_bs_phi_ulpt_mean+8*dz_bs_phi_ulpt_stddev);

    for (float val : d0_pv_pt_loeta_vec) h_d0_pv_pt_loeta->Fill(val);
    for (float val : dz_pv_pt_loeta_vec) h_dz_pv_pt_loeta->Fill(val);
    for (float val : d0_bs_pt_loeta_vec) h_d0_bs_pt_loeta->Fill(val);
    for (float val : dz_bs_pt_loeta_vec) h_dz_bs_pt_loeta->Fill(val);
    for (float val : d0_pv_pt_hieta_vec) h_d0_pv_pt_hieta->Fill(val);
    for (float val : dz_pv_pt_hieta_vec) h_dz_pv_pt_hieta->Fill(val);
    for (float val : d0_bs_pt_hieta_vec) h_d0_bs_pt_hieta->Fill(val);
    for (float val : dz_bs_pt_hieta_vec) h_dz_bs_pt_hieta->Fill(val);
    for (float val : d0_pv_eta_lopt_vec) h_d0_pv_eta_lopt->Fill(val);
    for (float val : dz_pv_eta_lopt_vec) h_dz_pv_eta_lopt->Fill(val);
    for (float val : d0_bs_eta_lopt_vec) h_d0_bs_eta_lopt->Fill(val);
    for (float val : dz_bs_eta_lopt_vec) h_dz_bs_eta_lopt->Fill(val);
    for (float val : d0_pv_eta_hipt_vec) h_d0_pv_eta_hipt->Fill(val);
    for (float val : dz_pv_eta_hipt_vec) h_dz_pv_eta_hipt->Fill(val);
    for (float val : d0_bs_eta_hipt_vec) h_d0_bs_eta_hipt->Fill(val);
    for (float val : dz_bs_eta_hipt_vec) h_dz_bs_eta_hipt->Fill(val);
    for (float val : d0_pv_eta_ulpt_vec) h_d0_pv_eta_ulpt->Fill(val);
    for (float val : dz_pv_eta_ulpt_vec) h_dz_pv_eta_ulpt->Fill(val);
    for (float val : d0_bs_eta_ulpt_vec) h_d0_bs_eta_ulpt->Fill(val);
    for (float val : dz_bs_eta_ulpt_vec) h_dz_bs_eta_ulpt->Fill(val);
    for (float val : d0_pv_phi_lopt_vec) h_d0_pv_phi_lopt->Fill(val);
    for (float val : dz_pv_phi_lopt_vec) h_dz_pv_phi_lopt->Fill(val);
    for (float val : d0_bs_phi_lopt_vec) h_d0_bs_phi_lopt->Fill(val);
    for (float val : dz_bs_phi_lopt_vec) h_dz_bs_phi_lopt->Fill(val);
    for (float val : d0_pv_phi_hipt_vec) h_d0_pv_phi_hipt->Fill(val);
    for (float val : dz_pv_phi_hipt_vec) h_dz_pv_phi_hipt->Fill(val);
    for (float val : d0_bs_phi_hipt_vec) h_d0_bs_phi_hipt->Fill(val);
    for (float val : dz_bs_phi_hipt_vec) h_dz_bs_phi_hipt->Fill(val);
    for (float val : d0_pv_phi_ulpt_vec) h_d0_pv_phi_ulpt->Fill(val);
    for (float val : dz_pv_phi_ulpt_vec) h_dz_pv_phi_ulpt->Fill(val);
    for (float val : d0_bs_phi_ulpt_vec) h_d0_bs_phi_ulpt->Fill(val);
    for (float val : dz_bs_phi_ulpt_vec) h_dz_bs_phi_ulpt->Fill(val);

    auto result_d0_pv_pt_loeta = fit_draw(h_d0_pv_pt_loeta, figdir+Form("ippv_xy_fit/pt_loeta_%d", idx));
    auto result_dz_pv_pt_loeta = fit_draw(h_dz_pv_pt_loeta, figdir+Form("ippv_z_fit/pt_loeta_%d", idx));
    auto result_d0_bs_pt_loeta = fit_draw(h_d0_bs_pt_loeta, figdir+Form("ipbs_xy_fit/pt_loeta_%d", idx));
    auto result_dz_bs_pt_loeta = fit_draw(h_dz_bs_pt_loeta, figdir+Form("ipbs_z_fit/pt_loeta_%d", idx));
    auto result_d0_pv_pt_hieta = fit_draw(h_d0_pv_pt_hieta, figdir+Form("ippv_xy_fit/pt_hieta_%d", idx));
    auto result_dz_pv_pt_hieta = fit_draw(h_dz_pv_pt_hieta, figdir+Form("ippv_z_fit/pt_hieta_%d", idx));
    auto result_d0_bs_pt_hieta = fit_draw(h_d0_bs_pt_hieta, figdir+Form("ipbs_xy_fit/pt_hieta_%d", idx));
    auto result_dz_bs_pt_hieta = fit_draw(h_dz_bs_pt_hieta, figdir+Form("ipbs_z_fit/pt_hieta_%d", idx));

    auto result_d0_pv_eta_lopt = fit_draw(h_d0_pv_eta_lopt, figdir+Form("ippv_xy_fit/eta_lopt_%d", idx));
    auto result_dz_pv_eta_lopt = fit_draw(h_dz_pv_eta_lopt, figdir+Form("ippv_z_fit/eta_lopt_%d", idx));
    auto result_d0_bs_eta_lopt = fit_draw(h_d0_bs_eta_lopt, figdir+Form("ipbs_xy_fit/eta_lopt_%d", idx));
    auto result_dz_bs_eta_lopt = fit_draw(h_dz_bs_eta_lopt, figdir+Form("ipbs_z_fit/eta_lopt_%d", idx));
    auto result_d0_pv_eta_hipt = fit_draw(h_d0_pv_eta_hipt, figdir+Form("ippv_xy_fit/eta_hipt_%d", idx));
    auto result_dz_pv_eta_hipt = fit_draw(h_dz_pv_eta_hipt, figdir+Form("ippv_z_fit/eta_hipt_%d", idx));
    auto result_d0_bs_eta_hipt = fit_draw(h_d0_bs_eta_hipt, figdir+Form("ipbs_xy_fit/eta_hipt_%d", idx));
    auto result_dz_bs_eta_hipt = fit_draw(h_dz_bs_eta_hipt, figdir+Form("ipbs_z_fit/eta_hipt_%d", idx));
    auto result_d0_pv_eta_ulpt = fit_draw(h_d0_pv_eta_ulpt, figdir+Form("ippv_xy_fit/eta_ulpt_%d", idx));
    auto result_dz_pv_eta_ulpt = fit_draw(h_dz_pv_eta_ulpt, figdir+Form("ippv_z_fit/eta_ulpt_%d", idx));
    auto result_d0_bs_eta_ulpt = fit_draw(h_d0_bs_eta_ulpt, figdir+Form("ipbs_xy_fit/eta_ulpt_%d", idx));
    auto result_dz_bs_eta_ulpt = fit_draw(h_dz_bs_eta_ulpt, figdir+Form("ipbs_z_fit/eta_ulpt_%d", idx));

    auto result_d0_pv_phi_lopt = fit_draw(h_d0_pv_phi_lopt, figdir+Form("ippv_xy_fit/phi_lopt_%d", idx));
    auto result_dz_pv_phi_lopt = fit_draw(h_dz_pv_phi_lopt, figdir+Form("ippv_z_fit/phi_lopt_%d", idx));
    auto result_d0_bs_phi_lopt = fit_draw(h_d0_bs_phi_lopt, figdir+Form("ipbs_xy_fit/phi_lopt_%d", idx));
    auto result_dz_bs_phi_lopt = fit_draw(h_dz_bs_phi_lopt, figdir+Form("ipbs_z_fit/phi_lopt_%d", idx));
    auto result_d0_pv_phi_hipt = fit_draw(h_d0_pv_phi_hipt, figdir+Form("ippv_xy_fit/phi_hipt_%d", idx));
    auto result_dz_pv_phi_hipt = fit_draw(h_dz_pv_phi_hipt, figdir+Form("ippv_z_fit/phi_hipt_%d", idx));
    auto result_d0_bs_phi_hipt = fit_draw(h_d0_bs_phi_hipt, figdir+Form("ipbs_xy_fit/phi_hipt_%d", idx));
    auto result_dz_bs_phi_hipt = fit_draw(h_dz_bs_phi_hipt, figdir+Form("ipbs_z_fit/phi_hipt_%d", idx));
    auto result_d0_pv_phi_ulpt = fit_draw(h_d0_pv_phi_ulpt, figdir+Form("ippv_xy_fit/phi_ulpt_%d", idx));
    auto result_dz_pv_phi_ulpt = fit_draw(h_dz_pv_phi_ulpt, figdir+Form("ippv_z_fit/phi_ulpt_%d", idx));
    auto result_d0_bs_phi_ulpt = fit_draw(h_d0_bs_phi_ulpt, figdir+Form("ipbs_xy_fit/phi_ulpt_%d", idx));
    auto result_dz_bs_phi_ulpt = fit_draw(h_dz_bs_phi_ulpt, figdir+Form("ipbs_z_fit/phi_ulpt_%d", idx));
    
    nlohmann::json resojson;
    resojson["pt"] = (pv_trk_pt_edges[idx] + pv_trk_pt_edges[idx+1])/2;
    resojson["eta"] = (pv_trk_eta_edges[idx] + pv_trk_eta_edges[idx+1])/2;
    resojson["phi"] = (pv_trk_phi_edges[idx] + pv_trk_phi_edges[idx+1])/2;
    
    resojson["reso_d0_pv_pt_loeta"] = result_d0_pv_pt_loeta.first;
    resojson["reso_dz_pv_pt_loeta"] = result_dz_pv_pt_loeta.first;
    resojson["reso_d0_bs_pt_loeta"] = result_d0_bs_pt_loeta.first;
    resojson["reso_dz_bs_pt_loeta"] = result_dz_bs_pt_loeta.first;
    resojson["reso_d0_pv_pt_hieta"] = result_d0_pv_pt_hieta.first;
    resojson["reso_dz_pv_pt_hieta"] = result_dz_pv_pt_hieta.first;
    resojson["reso_d0_bs_pt_hieta"] = result_d0_bs_pt_hieta.first;
    resojson["reso_dz_bs_pt_hieta"] = result_dz_bs_pt_hieta.first;

    resojson["reso_d0_pv_eta_lopt"] = result_d0_pv_eta_lopt.first;
    resojson["reso_dz_pv_eta_lopt"] = result_dz_pv_eta_lopt.first;
    resojson["reso_d0_bs_eta_lopt"] = result_d0_bs_eta_lopt.first;
    resojson["reso_dz_bs_eta_lopt"] = result_dz_bs_eta_lopt.first;
    resojson["reso_d0_pv_eta_hipt"] = result_d0_pv_eta_hipt.first;
    resojson["reso_dz_pv_eta_hipt"] = result_dz_pv_eta_hipt.first;
    resojson["reso_d0_bs_eta_hipt"] = result_d0_bs_eta_hipt.first;
    resojson["reso_dz_bs_eta_hipt"] = result_dz_bs_eta_hipt.first;
    resojson["reso_d0_pv_eta_ulpt"] = result_d0_pv_eta_ulpt.first;
    resojson["reso_dz_pv_eta_ulpt"] = result_dz_pv_eta_ulpt.first;
    resojson["reso_d0_bs_eta_ulpt"] = result_d0_bs_eta_ulpt.first;
    resojson["reso_dz_bs_eta_ulpt"] = result_dz_bs_eta_ulpt.first;

    resojson["reso_d0_pv_phi_lopt"] = result_d0_pv_phi_lopt.first;
    resojson["reso_dz_pv_phi_lopt"] = result_dz_pv_phi_lopt.first;
    resojson["reso_d0_bs_phi_lopt"] = result_d0_bs_phi_lopt.first;
    resojson["reso_dz_bs_phi_lopt"] = result_dz_bs_phi_lopt.first;
    resojson["reso_d0_pv_phi_hipt"] = result_d0_pv_phi_hipt.first;
    resojson["reso_dz_pv_phi_hipt"] = result_dz_pv_phi_hipt.first;
    resojson["reso_d0_bs_phi_hipt"] = result_d0_bs_phi_hipt.first;
    resojson["reso_dz_bs_phi_hipt"] = result_dz_bs_phi_hipt.first;
    resojson["reso_d0_pv_phi_ulpt"] = result_d0_pv_phi_ulpt.first;
    resojson["reso_dz_pv_phi_ulpt"] = result_dz_pv_phi_ulpt.first;
    resojson["reso_d0_bs_phi_ulpt"] = result_d0_bs_phi_ulpt.first;
    resojson["reso_dz_bs_phi_ulpt"] = result_dz_bs_phi_ulpt.first;
    
    resojson["resoerr_d0_pv_pt_loeta"] = result_d0_pv_pt_loeta.second;
    resojson["resoerr_dz_pv_pt_loeta"] = result_dz_pv_pt_loeta.second;
    resojson["resoerr_d0_bs_pt_loeta"] = result_d0_bs_pt_loeta.second;
    resojson["resoerr_dz_bs_pt_loeta"] = result_dz_bs_pt_loeta.second;
    resojson["resoerr_d0_pv_pt_hieta"] = result_d0_pv_pt_hieta.second;
    resojson["resoerr_dz_pv_pt_hieta"] = result_dz_pv_pt_hieta.second;
    resojson["resoerr_d0_bs_pt_hieta"] = result_d0_bs_pt_hieta.second;
    resojson["resoerr_dz_bs_pt_hieta"] = result_dz_bs_pt_hieta.second;

    resojson["resoerr_d0_pv_eta_lopt"] = result_d0_pv_eta_lopt.second;
    resojson["resoerr_dz_pv_eta_lopt"] = result_dz_pv_eta_lopt.second;
    resojson["resoerr_d0_bs_eta_lopt"] = result_d0_bs_eta_lopt.second;
    resojson["resoerr_dz_bs_eta_lopt"] = result_dz_bs_eta_lopt.second;
    resojson["resoerr_d0_pv_eta_hipt"] = result_d0_pv_eta_hipt.second;
    resojson["resoerr_dz_pv_eta_hipt"] = result_dz_pv_eta_hipt.second;
    resojson["resoerr_d0_bs_eta_hipt"] = result_d0_bs_eta_hipt.second;
    resojson["resoerr_dz_bs_eta_hipt"] = result_dz_bs_eta_hipt.second;
    resojson["resoerr_d0_pv_eta_ulpt"] = result_d0_pv_eta_ulpt.second;
    resojson["resoerr_dz_pv_eta_ulpt"] = result_dz_pv_eta_ulpt.second;
    resojson["resoerr_d0_bs_eta_ulpt"] = result_d0_bs_eta_ulpt.second;
    resojson["resoerr_dz_bs_eta_ulpt"] = result_dz_bs_eta_ulpt.second;

    resojson["resoerr_d0_pv_phi_lopt"] = result_d0_pv_phi_lopt.second;
    resojson["resoerr_dz_pv_phi_lopt"] = result_dz_pv_phi_lopt.second;
    resojson["resoerr_d0_bs_phi_lopt"] = result_d0_bs_phi_lopt.second;
    resojson["resoerr_dz_bs_phi_lopt"] = result_dz_bs_phi_lopt.second;
    resojson["resoerr_d0_pv_phi_hipt"] = result_d0_pv_phi_hipt.second;
    resojson["resoerr_dz_pv_phi_hipt"] = result_dz_pv_phi_hipt.second;
    resojson["resoerr_d0_bs_phi_hipt"] = result_d0_bs_phi_hipt.second;
    resojson["resoerr_dz_bs_phi_hipt"] = result_dz_bs_phi_hipt.second;
    resojson["resoerr_d0_pv_phi_ulpt"] = result_d0_pv_phi_ulpt.second;
    resojson["resoerr_dz_pv_phi_ulpt"] = result_dz_pv_phi_ulpt.second;
    resojson["resoerr_d0_bs_phi_ulpt"] = result_d0_bs_phi_ulpt.second;
    resojson["resoerr_dz_bs_phi_ulpt"] = result_dz_bs_phi_ulpt.second;
    
    std::ofstream outFile("./json/"+datatype+Form("/ip_res/fit_%d.json",idx));
    outFile << resojson.dump(4);
    outFile.close();
    
    return 0;
}
