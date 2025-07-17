#include <vector>
#include <iostream>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>

#include "CMSplots/tdrStyle.c"
#include "CMSplots/CMS_lumi.c"
#include "CMSplots/draw_funcs.c"

// ZeroBias no BS
const TString all_era = "BCDEFG";
const TString datatype_text = "Unbiased collision events";
const TString NOfilepath[] = {
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250328_noBS/ZeroBias/Run2022B_22Sep2023_v1_MINIAOD/250328_122620/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250328_noBS/ZeroBias/Run2022C_22Sep2023_v1_MINIAOD/250328_122837/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250328_noBS/ZeroBias/Run2022D_22Sep2023_v1_MINIAOD/250328_123055/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250328_noBS/ZeroBias/Run2022E_22Sep2023_v1_MINIAOD/250328_123946/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250328_noBS/ZeroBias/Run2022F_22Sep2023_v1_MINIAOD/250328_124203/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250328_noBS/ZeroBias/Run2022G_22Sep2023_v1_MINIAOD/250328_125858/"
};
const TString BSfilepath[] = {
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250401_withBS/ZeroBias/Run2022B_22Sep2023_v1_MINIAOD/250401_111242/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250401_withBS/ZeroBias/Run2022C_22Sep2023_v1_MINIAOD/250401_111503/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250401_withBS/ZeroBias/Run2022D_22Sep2023_v1_MINIAOD/250401_111933/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250401_withBS/ZeroBias/Run2022E_22Sep2023_v1_MINIAOD/250401_112402/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250401_withBS/ZeroBias/Run2022F_22Sep2023_v1_MINIAOD/250401_112622/",
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250401_withBS/ZeroBias/Run2022G_22Sep2023_v1_MINIAOD/250401_111004/"
};


const TString figdir = "./figures/comparison/";

void sscheck(TChain *mychain, TString myvar, TString BSconfig, TString xtitle, TString figname, float xmin, float xmax){

    lumi_sqrtS = "13.6 TeV, 2022" + all_era;

    TCut mycut = "abs(pv_x1 - pv_x) < 200 && abs(pv_x2 - pv_x) < 200";

    TH1F *h1 = new TH1F("h1", ";;# events", 200, xmin, xmax);
    mychain->Project("h1", myvar, mycut);
    h1->GetXaxis()->SetTitle(xtitle);

    TCanvas *c1 = new TCanvas("c1", "", 800, 600);
    canvas_setup(c1);
    h1->SetMaximum(1.3*h1->GetMaximum());
    h1->Draw("");
    write_text(0.6, 0.8, BSconfig);
    write_text(0.7, 0.7, Form("#mu = %.2f", h1->GetMean()));
    write_text(0.7, 0.65, Form("#sigma = %.2f", h1->GetStdDev()));
    CMS_lumi(c1);
    c1->RedrawAxis();
    c1->SaveAs(figdir+figname);

    delete h1;
    delete c1;
}

int short_check() {
    setTDRStyle();

    const int nera = 6;
    TChain *BSchain = new TChain("mytree");
    TChain *NOchain = new TChain("mytree");
    for(int i=0; i<nera; i++){
        TString tmp_BSpath = BSfilepath[i] + "vertex_analysis.root";
        BSchain->Add(tmp_BSpath);
        TString tmp_NOpath = NOfilepath[i] + "vertex_analysis.root";
        NOchain->Add(tmp_NOpath);
    }

    TString withBS = "refit with beamspot";
    TString noBS = "refit without beamspot";

    sscheck(BSchain, "pv_x", withBS, "#it{x}_{PV} [#mum]", "withBS_x.png", 1600, 1850);
    sscheck(BSchain, "pv_x1", withBS, "#it{x_{1}} [#mum]", "withBS_x1.png", 1600, 1850);
    sscheck(BSchain, "pv_x2", withBS, "#it{x_{2}} [#mum]", "withBS_x2.png", 1600, 1850);
    sscheck(BSchain, "pv_x1 - pv_x2", withBS, "#it{x_{1} - x_{2}} [#mum]", "withBS_xdiff.png", -25, 25);
    sscheck(BSchain, "pv_x - bs_x*10000", withBS, "#it{x}_{PV} - #it{x}_{BS} [#mum]", "withBS_x_pvbs.png", -25, 25);
    sscheck(BSchain, "pv_x1 - bs_x*10000", withBS, "#it{x_{1}} - #it{x}_{BS} [#mum]", "withBS_x1_pvbs.png", -25, 25);
    sscheck(BSchain, "pv_x2 - bs_x*10000", withBS, "#it{x_{2}} - #it{x}_{BS} [#mum]", "withBS_x2_pvbs.png", -25, 25);
    sscheck(NOchain, "pv_x", noBS, "#it{x}_{PV} [#mum]", "noBS_x.png", 1600, 1850);
    sscheck(NOchain, "pv_x1", noBS, "#it{x_{1}} [#mum]", "noBS_x1.png", 1600, 1850);
    sscheck(NOchain, "pv_x2", noBS, "#it{x_{2}} [#mum]", "noBS_x2.png", 1600, 1850);
    sscheck(NOchain, "pv_x1 - pv_x2", noBS, "#it{x_{1} - x_{2}} [#mum]", "noBS_xdiff.png", -100, 100);
    sscheck(NOchain, "pv_x - bs_x*10000", noBS, "#it{x}_{PV} - #it{x}_{BS} [#mum]", "noBS_x_pvbs.png", -100, 100);
    sscheck(NOchain, "pv_x1 - bs_x*10000", noBS, "#it{x_{1}} - #it{x}_{BS} [#mum]", "noBS_x1_pvbs.png", -100, 100);
    sscheck(NOchain, "pv_x2 - bs_x*10000", noBS, "#it{x_{2}} - #it{x}_{BS} [#mum]", "noBS_x2_pvbs.png", -100, 100);

    return 0;
}
