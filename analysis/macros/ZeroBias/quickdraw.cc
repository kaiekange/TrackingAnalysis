#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"

int quickdraw(){
    
    setTDRStyle();

    ROOT::EnableImplicitMT();

    TChain *mychain = new TChain("mytree");
    mychain->Add("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/mc_corr_mask.root");

    // TH2F *h1 = new TH2F("h1", ";Track #it{d_{xy}} [#mum];Track #it{#eta}", 100, -6000, 6000, 100, 2, 3);
    TH2F *h2 = new TH2F("h2", ";Track #it{d_{xy}} [#mum];Track #it{#phi}", 100, -6000, 6000, 100, -3.1416, 3.1416);
    TH2F *h3 = new TH2F("h3", ";Track #it{d_{xy}} [#mum];Track #it{p_{T}} [GeV]", 100, -6000, 6000, 100, 0, 0.3);
    // TH2F *h4 = new TH2F("h4", ";Track #it{#eta};Track #it{#phi}", 100, -3, 3, 100, -3.1416, 3.1416);

    // mychain->Project("h1", "pv_trk_eta : pv_trk_d0_pvunbiased", "(pv_trk_pt<0.15) * PU_factor");
    mychain->Project("h2", "pv_trk_phi : pv_trk_d0_pvunbiased", "( (abs(pv_trk_eta)>2.4) && (pv_trk_pt<0.15) ) * PU_factor");
    
    // mychain->Project("h4", "pv_trk_phi : pv_trk_eta", "( (abs(pv_trk_d0_pvunbiased) > 2000) && (abs(pv_trk_d0_pvunbiased) < 6000) && (pv_trk_pt<0.15) ) * PU_factor");

    // TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    // canvas_setup(c1);
    // gPad->SetLogz(0);
    // h1->Draw("colz");
    // c1->Update();
    // c1->SaveAs("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/mc/eta_d0.png");
    // gPad->SetLogz(1);
    // c1->Update();
    // c1->SaveAs("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/mc/eta_d0_log.png");

    TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
    canvas_setup(c2);
    gPad->SetLogz(0);
    h2->Draw("colz");
    c2->Update();
    c2->SaveAs("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/mc/phi_d0.png");
    gPad->SetLogz(1);
    c2->Update();
    c2->SaveAs("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/mc/phi_d0_log.png");

    mychain->Project("h3", "pv_trk_pt : pv_trk_d0_pvunbiased", "(abs(pv_trk_eta)>2.4) * PU_factor");
    TCanvas *c3 = new TCanvas("c3", "c3", 800, 600);
    canvas_setup(c3);
    gPad->SetLogz(0);
    h3->Draw("colz");
    c3->Update();
    c3->SaveAs("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/mc/pt_d0.png");
    gPad->SetLogz(1);
    c3->Update();
    c3->SaveAs("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/mc/pt_d0_log.png");

    // TCanvas *c4 = new TCanvas("c4", "c4", 800, 600);
    // canvas_setup(c4);
    // gPad->SetLogz(0);
    // h4->Draw("colz");
    // c4->Update();
    // c4->SaveAs("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/mc/phi_eta.png");
    // gPad->SetLogz(1);
    // c4->Update();
    // c4->SaveAs("/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/2024/mc/phi_eta_log.png");

    return 0;
}