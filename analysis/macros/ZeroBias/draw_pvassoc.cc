#include <iostream>
#include <TChain.h>
#include <TString.h>
#include <TH2.h>
#include <TCanvas.h>

#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"

void just_draw(TString txtt, TH2F *hist, TString figpath)
{
    lumi_sqrtS = "13.6 TeV, " + txtt;

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas_setup(canvas);
    gPad->SetLogz(0);
    hist->Draw("colz");
    CMS_lumi(canvas);
    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + ".png");

    gPad->SetLogz(1);
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + "_log.png");
}

int draw_pvassoc()
{
    setTDRStyle();
    ROOT::EnableImplicitMT();

    TChain *mcchain = new TChain("residuals/tree");
    mcchain->Add("/eos/cms/store/group/phys_tracking/kakang/Run3TrackingAnalysis/Ntuple/Track-v20260113/SingleNeutrino_Par-E-10_gun/RunIII2024Summer24_MINIAODSIM_S10M0/260113_150515/0000/output_*.root");

    // TH2F *h_mc_pvassoc = new TH2F("h_mc_pvassoc", ";Track #it{d_{xy}} [cm];pv association quality", 100, -8000, 8000, 8, 0, 8);
    // mcchain->Project("h_mc_pvassoc", "pv_trk_pvAssociationQuality : pv_trk_d0_pvunbiased", "");
    // just_draw("2024 simulation", h_mc_pvassoc, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/2024/mc_pvassoc");
    // delete h_mc_pvassoc;

    // TH2F *h_mc_pvassoc_lo = new TH2F("h_mc_pvassoc_lo", ";Track #it{d_{xy}} [cm];pv association quality", 100, -8000, 8000, 8, 0, 8);
    // mcchain->Project("h_mc_pvassoc_lo", "pv_trk_pvAssociationQuality : pv_trk_d0_pvunbiased", "(abs(pv_trk_eta)>2.5) && (pv_trk_pt<0.15)");
    // just_draw("2024 simulation", h_mc_pvassoc_lo, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/2024/mc_pvassoc_lo");
    // delete h_mc_pvassoc_lo;

    // TH2F *h_mc_weight = new TH2F("h_mc_weight", ";Track #it{d_{xy}} [cm];Track weight in PV refit", 100, -8000, 8000, 100, 0, 1);
    // mcchain->Project("h_mc_weight", "pv_trk_weight : pv_trk_d0_pvunbiased", "(abs(pv_trk_eta)>2.5) && (pv_trk_pt<0.15)");
    // just_draw("2024 simulation", h_mc_weight, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/2024/mc_weight");
    // delete h_mc_weight;

    // TH1F *h1 = new TH1F("h1", ";PV association quality;# tracks", 8, 0, 8);
    // mcchain->Project("h1", "pv_trk_pvAssociationQuality", "");
    // lumi_sqrtS = "13.6 TeV, 2024 simulation";
    // TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    // canvas_setup(c1);
    // h1->SetMinimum(0);
    // h1->Draw("hist");
    // CMS_lumi(c1);
    // c1->Update();
    // c1->RedrawAxis();
    // c1->SaveAs("/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/2024/all_pvassoc.png");

    TH1F *h1 = new TH1F("h1", ";Track IP #it{d_{xy}} [cm];# tracks", 100, -8000, 8000);
    mcchain->Project("h1", "pv_trk_d0_pvunbiased", "(abs(pv_trk_eta)>2.5) && (pv_trk_pt<0.15) && (pv_trk_weight>0.1)");
    lumi_sqrtS = "13.6 TeV, 2024 simulation";
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    canvas_setup(c1);
    h1->SetMinimum(0);
    h1->Draw("ep");
    CMS_lumi(c1);
    c1->Update();
    c1->RedrawAxis();
    c1->SaveAs("/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/2024/mc_weigth.png");

    delete mcchain;

    return 0;
}