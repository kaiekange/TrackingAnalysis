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

int draw_sth(TString year)
{

    setTDRStyle();
    ROOT::EnableImplicitMT();

    // TChain *datachain = new TChain("mytree");

    // if(year.Contains("2022"))
    // {
    //     datachain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2022_preEE/data.root");
    //     datachain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2022_postEE/data.root");
    // }
    // else if(year.Contains("2023"))
    // {
    //     datachain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2023_preBPix/data.root");
    //     datachain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2023_postBPix/data.root");
    // }
    // else if(year.Contains("2024"))
    // {
    //     datachain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/data.root");
    // }
    

    // TH2F *h_data_fweta_d0 = new TH2F("h_data_fweta_d0", ";Track #it{d_{xy}} [#mum];Track #it{#eta}", 100, -8000, 8000, 100, 2, 3);
    // datachain->Project("h_data_fweta_d0", "pv_trk_eta : pv_trk_d0_pvunbiased", "pv_trk_pt<0.25");
    // just_draw(year, h_data_fweta_d0, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/data_fweta_d0");
    // delete h_data_fweta_d0;

    // TH2F *h_data_bweta_d0 = new TH2F("h_data_bweta_d0", ";Track #it{d_{xy}} [#mum];Track #it{#eta}", 100, -8000, 8000, 100, -3, -2);
    // datachain->Project("h_data_bweta_d0", "pv_trk_eta : pv_trk_d0_pvunbiased", "pv_trk_pt<0.25");
    // just_draw(year, h_data_bweta_d0, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/data_bweta_d0");
    // delete h_data_bweta_d0;

    // TH2F *h_data_phi_d0 = new TH2F("h_data_phi_d0", ";Track #it{d_{xy}} [#mum];Track #it{#phi}", 100, -8000, 8000, 100, -3.1416, 3.1416);
    // datachain->Project("h_data_phi_d0", "pv_trk_phi : pv_trk_d0_pvunbiased", "(abs(pv_trk_eta)>2.4) && (pv_trk_pt<0.25)");
    // just_draw(year, h_data_phi_d0, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/data_phi_d0");
    // delete h_data_phi_d0;

    // TH2F *h_data_pt_d0 = new TH2F("h_data_pt_d0", ";Track #it{d_{xy}} [#mum];Track #it{p_{T}} [GeV]", 100, -8000, 8000, 100, 0, 0.3);
    // datachain->Project("h_data_pt_d0", "pv_trk_pt : pv_trk_d0_pvunbiased", "abs(pv_trk_eta)>2.4");
    // just_draw(year, h_data_pt_d0, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/data_pt_d0");
    // delete h_data_pt_d0;

    // TH2F *h_data_eta_phi = new TH2F("h_data_eta_phi", ";Track #it{#eta};Track #it{#phi}", 100, -5, 5, 100, -3.1416, 3.1416);
    // datachain->Project("h_data_eta_phi", "pv_trk_phi : pv_trk_eta", "(abs(pv_trk_d0_pvunbiased) > 2000) && (abs(pv_trk_d0_pvunbiased) < 6000) && (pv_trk_pt<0.25)");
    // just_draw(year, h_data_eta_phi, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/data_eta_phi");
    // delete h_data_eta_phi;

    // TH2F *h_data_fweta_pt = new TH2F("h_data_fweta_pt", ";Track #it{#eta};Track #it{p_{T}} [GeV]", 100, 2, 4, 100, 0, 0.3);
    // datachain->Project("h_data_fweta_pt", "pv_trk_pt : pv_trk_eta", "(abs(pv_trk_d0_pvunbiased) > 2000) && (abs(pv_trk_d0_pvunbiased) < 6000)");
    // just_draw(year, h_data_fweta_pt, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/data_fweta_pt");
    // delete h_data_fweta_pt;

    // TH2F *h_data_bweta_pt = new TH2F("h_data_bweta_pt", ";Track #it{#eta};Track #it{p_{T}} [GeV]", 100, -4, -2, 100, 0, 0.3);
    // datachain->Project("h_data_bweta_pt", "pv_trk_pt : pv_trk_eta", "(abs(pv_trk_d0_pvunbiased) > 2000) && (abs(pv_trk_d0_pvunbiased) < 6000)");
    // just_draw(year, h_data_bweta_pt, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/data_bweta_pt");
    // delete h_data_bweta_pt;

    // TH2F *h_data_eta_phi_plus = new TH2F("h_data_eta_phi_plus", ";Track #it{#eta};Track #it{#phi}", 100, -5, 5, 100, -3.1416, 3.1416);
    // datachain->Project("h_data_eta_phi_plus", "pv_trk_phi : pv_trk_eta", "(pv_trk_d0_pvunbiased > 2000) && (pv_trk_d0_pvunbiased < 6000) && (pv_trk_pt<0.25)");
    // just_draw(year, h_data_eta_phi_plus, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/data_eta_phi_plus");
    // delete h_data_eta_phi_plus;
    
    // TH2F *h_data_eta_phi_minus = new TH2F("h_data_eta_phi_minus", ";Track #it{#eta};Track #it{#phi}", 100, -5, 5, 100, -3.1416, 3.1416);
    // datachain->Project("h_data_eta_phi_minus", "pv_trk_phi : pv_trk_eta", "(pv_trk_d0_pvunbiased < -2000) && (pv_trk_d0_pvunbiased > -6000) && (pv_trk_pt<0.25)");
    // just_draw(year, h_data_eta_phi_minus, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/data_eta_phi_minus");
    // delete h_data_eta_phi_minus;

    // delete datachain;

    TChain *mcchain = new TChain("mytree");
    if(year.Contains("2022"))
    {
        mcchain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2022_preEE/mc_corr_mask.root");
        mcchain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2022_postEE/mc_corr_mask.root");
    }
    else if(year.Contains("2023"))
    {
        mcchain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2023_preBPix/mc_corr_mask.root");
        mcchain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2023_postBPix/mc_corr_mask.root");
    }
    else if(year.Contains("2024"))
    {
        mcchain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/mc_corr_mask.root");
    }

    // TH2F *h_mc_fweta_d0 = new TH2F("h_mc_fweta_d0", ";Track #it{d_{xy}} [#mum];Track #it{#eta}", 100, -8000, 8000, 100, 2, 3);
    // mcchain->Project("h_mc_fweta_d0", "pv_trk_eta : pv_trk_d0_pvunbiased", "(pv_trk_pt<0.25) * PU_factor");
    // just_draw(year+" simulation", h_mc_fweta_d0, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/mc_fweta_d0");
    // delete h_mc_fweta_d0;

    // TH2F *h_mc_bweta_d0 = new TH2F("h_mc_bweta_d0", ";Track #it{d_{xy}} [#mum];Track #it{#eta}", 100, -8000, 8000, 100, -3, -2);
    // mcchain->Project("h_mc_bweta_d0", "pv_trk_eta : pv_trk_d0_pvunbiased", "(pv_trk_pt<0.25) * PU_factor");
    // just_draw(year+" simulation", h_mc_bweta_d0, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/mc_bweta_d0");
    // delete h_mc_bweta_d0;

    // TH2F *h_mc_phi_d0 = new TH2F("h_mc_phi_d0", ";Track #it{d_{xy}} [#mum];Track #it{#phi}", 100, -8000, 8000, 100, -3.1416, 3.1416);
    // mcchain->Project("h_mc_phi_d0", "pv_trk_phi : pv_trk_d0_pvunbiased", "((abs(pv_trk_eta)>2.4) && (pv_trk_pt<0.25)) * PU_factor");
    // just_draw(year+" simulation", h_mc_phi_d0, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/mc_phi_d0");
    // delete h_mc_phi_d0;

    // TH2F *h_mc_pt_d0 = new TH2F("h_mc_pt_d0", ";Track #it{d_{xy}} [#mum];Track #it{p_{T}} [GeV]", 100, -8000, 8000, 100, 0, 0.3);
    // mcchain->Project("h_mc_pt_d0", "pv_trk_pt : pv_trk_d0_pvunbiased", "(abs(pv_trk_eta)>2.4) * PU_factor");
    // just_draw(year+" simulation", h_mc_pt_d0, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/mc_pt_d0");
    // delete h_mc_pt_d0;

    // TH2F *h_mc_eta_phi = new TH2F("h_mc_eta_phi", ";Track #it{#eta};Track #it{#phi}", 100, -5, 5, 100, -3.1416, 3.1416);
    // mcchain->Project("h_mc_eta_phi", "pv_trk_phi : pv_trk_eta", "((abs(pv_trk_d0_pvunbiased) > 2000) && (abs(pv_trk_d0_pvunbiased) < 6000) && (pv_trk_pt<0.25)) * PU_factor");
    // just_draw(year+" simulation", h_mc_eta_phi, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/mc_eta_phi");
    // delete h_mc_eta_phi;

    TH2F *h_mc_fweta_pt = new TH2F("h_mc_fweta_pt", ";Track #it{#eta};Track #it{p_{T}} [GeV]", 100, 2, 4, 100, 0, 0.3);
    mcchain->Project("h_mc_fweta_pt", "pv_trk_pt : pv_trk_eta", "((abs(pv_trk_d0_pvunbiased) > 2000) && (abs(pv_trk_d0_pvunbiased) < 6000)) * PU_factor");
    just_draw(year+" simulation", h_mc_fweta_pt, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/mc_fweta_pt");
    delete h_mc_fweta_pt;

    TH2F *h_mc_bweta_pt = new TH2F("h_mc_bweta_pt", ";Track #it{#eta};Track #it{p_{T}} [GeV]", 100, -4, -2, 100, 0, 0.3);
    mcchain->Project("h_mc_bweta_pt", "pv_trk_pt : pv_trk_eta", "((abs(pv_trk_d0_pvunbiased) > 2000) && (abs(pv_trk_d0_pvunbiased) < 6000)) * PU_factor");
    just_draw(year+" simulation", h_mc_bweta_pt, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/mc_bweta_pt");
    delete h_mc_bweta_pt;

    // TH2F *h_mc_eta_phi_plus = new TH2F("h_mc_eta_phi_plus", ";Track #it{#eta};Track #it{#phi}", 100, -5, 5, 100, -3.1416, 3.1416);
    // mcchain->Project("h_mc_eta_phi_plus", "pv_trk_phi : pv_trk_eta", "((pv_trk_d0_pvunbiased > 2000) && (pv_trk_d0_pvunbiased < 6000) && (pv_trk_pt<0.25)) * PU_factor");
    // just_draw(year+" simulation", h_mc_eta_phi_plus, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/mc_eta_phi_plus");
    // delete h_mc_eta_phi_plus;
    
    // TH2F *h_mc_eta_phi_minus = new TH2F("h_mc_eta_phi_minus", ";Track #it{#eta};Track #it{#phi}", 100, -5, 5, 100, -3.1416, 3.1416);
    // mcchain->Project("h_mc_eta_phi_minus", "pv_trk_phi : pv_trk_eta", "((pv_trk_d0_pvunbiased < -2000) && (pv_trk_d0_pvunbiased > -6000) && (pv_trk_pt<0.25)) * PU_factor");
    // just_draw(year+" simulation", h_mc_eta_phi_minus, "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/spfigs/"+year+"/mc_eta_phi_minus");
    // delete h_mc_eta_phi_minus;

    delete mcchain;

    return 0;
}