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

const TString datatype_text = "High-q^{2} multi-jet events";

void drawdraw(TString era, TChain *mytree, TString myvar, TString xtitle, TString ytitle, int nbins, float varmin, float varmax, TString figpath){

    lumi_sqrtS = "13.6 TeV, 2022 " + era;

    TH1F *h1 = new TH1F("h1", "", nbins, varmin, varmax);
    /* mytree->Project("h1", myvar); */
    mytree->Project("h1", myvar, "xsecweight * PSweight * PU_factor * cell_mask");
    h1->SetLineColor(kBlack);
    h1->SetMarkerColor(kBlack);
    h1->SetMarkerSize(0.7);
    h1->SetMaximum(h1->GetMaximum()*1.5);
    h1->SetMinimum(0);
    h1->GetXaxis()->SetTitle(xtitle);
    h1->GetYaxis()->SetTitle(ytitle);

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas_setup(canvas);
    canvas->SetBottomMargin(0.15);
    h1->Draw("ep");

    write_text(0.55, 0.85, datatype_text);

    CMS_lumi(canvas);
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath);
    delete h1;
}

int draw_fig(TString era="preEE"){

    setTDRStyle();
    
    const TString figdir = "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_"+era+"/draw_fig/";

    if(gSystem->AccessPathName(figdir)) gSystem->MakeDirectory(figdir);

    TChain *mytree = new TChain("mytree");
    mytree->Add("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_mc_"+era+"_corrmask.root");
    /* mytree->Add("/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/all_skimmed_2022_data_"+era+"_corr.root"); */

    drawdraw(era, mytree, "sqrt(pv_SumTrackPt2)", "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "weighted # events", 500, 0, 500, figdir+"mc3_sumPt2.png");
    /* drawdraw(era, mytree, "sqrt(pv_SumTrackPt2)", "#sqrt{#sum#it{p_{T}}^{2}} [GeV]", "# events", 500, 0, 500, figdir+"data_sumPt2.png"); */

    return 0;
}
