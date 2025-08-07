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
/* #include "../../functions/compare_gr.cc" */

const TString figdir = "../../figures/"+datatype+"/compare/";


void compare_draw(TTree *datatree, TTree *mctree, TString myvar, TString xtitle, TString ytitle, int nbins, float varmin, float varmax, TString figpath){

    lumi_sqrtS = "13.6 TeV, 2022";

    TH1F *h1 = new TH1F("h1", "", nbins, varmin, varmax);
    datatree->Project("h1", myvar);
    h1->Scale(1./h1->Integral());
    h1->SetLineColor(kBlack);
    h1->SetMarkerColor(kBlack);
    h1->SetMarkerSize(0.7);

    TH1F *h2 = new TH1F("h2", "", nbins, varmin, varmax);
    mctree->Project("h2", myvar);
    h2->Scale(1./h2->Integral());
    h2->SetFillColorAlpha(kOrange-9, 0.3);
    h2->SetFillStyle(1001);
    h2->SetLineColor(0);
    h2->SetLineWidth(0);
    /* h2->SetMarkerColor(kRed); */
    /* h2->SetMarkerSize(0.7); */

    float height = std::max(h1->GetMaximum(), h2->GetMaximum());

    h2->SetMaximum(height*1.5);
    h2->SetMinimum(0);
    h2->GetXaxis()->SetTitle(xtitle);
    h2->GetYaxis()->SetTitle(ytitle);

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas_setup(canvas);
    h2->Draw("hist");
    h1->Draw("ep same");

    TLegend * mylegend = new TLegend(0.55, 0.7, 0.65, 0.8, NULL, "brNDC");
    mylegend->AddEntry(h1, "Data", "p");
    mylegend->AddEntry(h2, "Simulation", "f");
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
    canvas->SaveAs(figpath);
    delete h1;
    delete h2;
}

int compare_data_mc(){

    setTDRStyle();

    TFile *datafile = TFile::Open("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/tuples/JetHT_data2022.root");
    TTree *datatree = (TTree*)datafile->Get("mytree");
    TFile *mcfile = TFile::Open("/user/kakang/IPres/CMSSW_14_0_10/src/TrackingAnalysis/analysis/tuples/JetHT_mc2022.root");
    TTree *mctree = (TTree*)mcfile->Get("mytree");

    compare_draw(datatree, mctree, "ev_nPV", "# primary vertices", "Normalised # events", 90, 0, 90, figdir+"nPV.png");
    compare_draw(datatree, mctree, "pv_SumTrackPt", "#sum#it{p_{T}} [GeV]", "Normalised # events", 100, 0, 2000, figdir+"sumPt.png");
    compare_draw(datatree, mctree, "sqrt(pv_SumTrackPt2)", "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "Normalised # events", 100, 0, 500, figdir+"sumPt2.png");
    compare_draw(datatree, mctree, "pv_NTracks", "# tracks", "Normalised # events", 50, 0, 250, figdir+"ntracks.png");
    compare_draw(datatree, mctree, "pv_chi2", "#chi^{2}", "Normalised # events", 100, 0, 500, figdir+"chi2.png");
    compare_draw(datatree, mctree, "pv_ndof", "NDOF", "Normalised # events", 100, 0, 500, figdir+"ndof.png");
    compare_draw(datatree, mctree, "pv_chi2/pv_ndof", "#chi^{2}/NDOF", "Normalised # events", 100, 0, 2, figdir+"chi2ndof.png");
    compare_draw(datatree, mctree, "pv_trk_pt", "Track #it{p_{T}} [GeV]", "Normalised # events", 100, 0, 10, figdir+"pt.png");
    compare_draw(datatree, mctree, "pv_trk_eta", "Track #it{#eta}", "Normalised # events", 100, -3, 3, figdir+"eta.png");
    compare_draw(datatree, mctree, "pv_trk_phi", "Track #it{#phi}", "Normalised # events", 100, -3.1416, 3.1416, figdir+"phi.png");

    return 0;
}
