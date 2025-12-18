#include <vector>
#include <iostream>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"

const TString datatype_text = "Unbiased collision events";

int PU_show(){

    TString era = "2022_preEE";

    lumi_sqrtS = "13.6 TeV, 2022 preEE";

    setTDRStyle();

    const TString figdir = "/eos/home-k/kakang/IPres/analysis/ZeroBias/figures/"+era+"/compare";

    if(gSystem->AccessPathName(figdir)) gSystem->MakeDirectory(figdir);

    TFile *histfile = TFile::Open("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/"+era+"/pileupHistogram.root");
    TH1F *h_pileup = (TH1F*)histfile->Get("pileup");
    TFile *mcfile = TFile::Open("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/"+era+"/mc_corr_mask.root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    h_pileup->Scale(1./h_pileup->Integral());
    // h_pileup->SetLineColor(kBlack);
    // h_pileup->SetLineWidth(0.7);
    h_pileup->SetMarkerColor(kBlack);
    h_pileup->SetMarkerSize(0.7);
    h_pileup->SetMarkerStyle(20);

    TH1F *h_mc = new TH1F("h_mc", "", 99, 0, 99);
    // mctree->Project("h_mc", "NumTrueInts");
    mctree->Project("h_mc", "NumTrueInts", "PU_factor");
    h_mc->Scale(1./h_mc->Integral());
    h_mc->SetFillColorAlpha(kOrange-9, 0.3);
    h_mc->SetFillStyle(1001);
    h_mc->SetLineColor(0);
    h_mc->SetLineWidth(0);

    float height = std::max(h_pileup->GetMaximum(), h_mc->GetMaximum());

    h_mc->SetMaximum(height*1.5);
    h_mc->SetMinimum(0);
    h_mc->GetXaxis()->SetTitle("# PV");
    h_mc->GetYaxis()->SetTitle("Normalized # events");

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas_setup(canvas);
    h_mc->Draw("hist");
    h_pileup->Draw("ep same");

    TLegend * mylegend = new TLegend(0.55, 0.7, 0.65, 0.8, NULL, "brNDC");
    mylegend->AddEntry(h_pileup, "Pileup profile", "p");
    mylegend->AddEntry(h_mc, "Simulation", "f");
    mylegend->SetTextFont(42);
    mylegend->SetTextColor(kBlack);
    mylegend->SetBorderSize(0);
    mylegend->SetFillColor(0);
    mylegend->SetLineWidth(0);
    mylegend->SetTextSize(0.04);
    mylegend->Draw();

    write_text(0.55, 0.85, datatype_text);

    CMS_lumi(canvas);
    canvas->Update();
    canvas->RedrawAxis();
    // canvas->SaveAs(figdir+"ev_nPV.png");
    canvas->SaveAs(figdir+"ev_nPV_corr.png");
    // delete h_pileup;
    // delete h_mc;

    return 0;
}
