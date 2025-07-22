#include <iostream>
#include <vector>
#include <TString.h>
#include <TChain.h>

#include "CMSplots/tdrStyle.c"
#include "CMSplots/CMS_lumi.c"
#include "CMSplots/draw_funcs.c"
#include "input_list.cc"

const float pvxmin = 1.60;
const float pvxmax = 1.85;
const float pvymin = -2.0;
const float pvymax = -1.7;
const float pvzmin = -200;
const float pvzmax = 200;

const float bsxmin = 1.7;
const float bsxmax = 1.78;
const float bsymin = -1.86;
const float bsymax = -1.78;
const float bszmin = -200;
const float bszmax = 200;


void vertex_pos(TChain *mychain, TString era){

    TString fig_dir = "./figures/"+datatype+"/vertex_analysis/2022"+era;
    if(gSystem->AccessPathName(fig_dir)) gSystem->MakeDirectory(fig_dir);

    lumi_sqrtS = "13.6 TeV, 2022" + era;

    TH2F *h1 = new TH2F("h1", ";x [mm];y [mm]", 200, pvxmin, pvxmax, 200, pvymin, pvymax);
    TString figpath1 = fig_dir + "/pv_xy.png";
    mychain->Project("h1", "pv_y/1000.0 : pv_x/1000.0");
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600); canvas_setup(c1);
    h1->SetMinimum(3);
    h1->Draw("COLZ");
    write_text(0.5, 0.85, datatype_text);
    write_text(0.2, 0.26, Form("#splitline{RMS (PV)}{#splitline{x = %.0f #mum}{y = %.0f #mum}}", h1->ProjectionX()->GetRMS()*1000, h1->ProjectionY()->GetRMS()*1000));
    c1->SetLogz();
    CMS_lumi(c1); c1->Update(); c1->RedrawAxis(); c1->SaveAs(figpath1);
    delete h1; delete c1;

    TH2F *h2 = new TH2F("h2", ";z [mm];x [mm]", 200, pvzmin, pvzmax, 200, pvxmin, pvxmax);
    TString figpath2 = fig_dir + "/pv_xz.png";
    mychain->Project("h2", "pv_x/1000.0 : pv_z/1000.0");
    TCanvas *c2 = new TCanvas("c2", "c2", 800, 600); canvas_setup(c2);
    h2->SetMinimum(3);
    h2->Draw("COLZ");
    write_text(0.5, 0.85, datatype_text);
    write_text(0.2, 0.26, Form("#splitline{RMS (PV)}{#splitline{x = %.0f #mum}{z = %.0f mm}}", h2->ProjectionY()->GetRMS()*1000, h2->ProjectionX()->GetRMS()));
    c2->SetLogz();
    CMS_lumi(c2); c2->Update();c2->RedrawAxis(); c2->SaveAs(figpath2);
    delete h2; delete c2; 

    TH2F *h3 = new TH2F("h3", ";z [mm];y [mm]", 200, pvzmin, pvzmax, 200, pvymin, pvymax);
    TString figpath3 = fig_dir + "/pv_yz.png";
    mychain->Project("h3", "pv_y/1000.0 : pv_z/1000.0");
    TCanvas *c3 = new TCanvas("c3", "c3", 800, 600); canvas_setup(c3);
    h3->SetMinimum(3);
    h3->Draw("COLZ");
    write_text(0.5, 0.85, datatype_text);
    write_text(0.2, 0.26, Form("#splitline{RMS (PV)}{#splitline{y = %.0f #mum}{z = %.0f mm}}", h3->ProjectionY()->GetRMS()*1000, h3->ProjectionX()->GetRMS()));
    c3->SetLogz();
    CMS_lumi(c3); c3->Update(); c3->RedrawAxis(); c3->SaveAs(figpath3);
    delete h3; delete c3; 





    TH2F *h4 = new TH2F("h4", ";lumi number;x [mm]", 50, 0, 200, 50, pvxmin-0.1, pvxmax+0.1); 
    TString figpath4 = fig_dir + "/lumi_x.png";
    mychain->Project("h4", "pv_x/1000.0 : ev_lumi");
    TCanvas *c4 = new TCanvas("c4", "c4", 800, 600); canvas_setup(c4);
    h4->SetMinimum(3);
    h4->Draw("COLZ");
    write_text(0.5, 0.85, datatype_text);
    CMS_lumi(c4); c4->Update(); c4->RedrawAxis(); c4->SaveAs(figpath4);
    delete h4; delete c4; 

    TH2F *h5 = new TH2F("h5", ";lumi number;y [mm]", 50, 0, 200, 50, pvymin-0.1, pvymax+0.1);
    TString figpath5 = fig_dir + "/lumi_y.png";
    mychain->Project("h5", "pv_y/1000.0 : ev_lumi");
    TCanvas *c5 = new TCanvas("c5", "c5", 800, 600); canvas_setup(c5);
    h5->SetMinimum(3);
    h5->Draw("COLZ");
    write_text(0.5, 0.85, datatype_text);
    CMS_lumi(c5); c5->Update(); c5->RedrawAxis(); c5->SaveAs(figpath5);
    delete h5; delete c5; 

    TH2F *h6 = new TH2F("h6", ";lumi number;z [mm]", 50, 0, 200, 50, pvzmin-20, pvzmax+20);
    TString figpath6 = fig_dir + "/lumi_z.png";
    mychain->Project("h6", "pv_z/1000.0 : ev_lumi");
    TCanvas *c6 = new TCanvas("c6", "c6", 800, 600); canvas_setup(c6);
    h6->SetMinimum(3);
    h6->Draw("COLZ");
    write_text(0.5, 0.85, datatype_text);
    CMS_lumi(c6); c6->Update(); c6->RedrawAxis(); c6->SaveAs(figpath6);
    delete h6; delete c6; 





    TH1F *h7 = new TH1F("h7", ";x [mm];# beamspots", 200, bsxmin, bsxmax);
    TString figpath7 = fig_dir + "/bs_x0.png";
    mychain->Project("h7", "bs_x0*10");
    TCanvas *c7 = new TCanvas("c7", "c7", 800, 600); canvas_setup(c7);
    h7->SetMaximum(1.3*h7->GetMaximum());
    h7->Draw();
    write_text(0.5, 0.85, datatype_text);
    CMS_lumi(c7); c7->Update(); c7->RedrawAxis(); c7->SaveAs(figpath7);
    delete h7; delete c7;

    TH1F *h8 = new TH1F("h8", ";y [mm];# beamspots", 200, bsymin, bsymax);
    TString figpath8 = fig_dir + "/bs_y0.png";
    mychain->Project("h8", "bs_y0*10");
    TCanvas *c8 = new TCanvas("c8", "c8", 800, 600); canvas_setup(c8);
    h8->SetMaximum(1.3*h8->GetMaximum());
    h8->Draw();
    write_text(0.5, 0.85, datatype_text);
    CMS_lumi(c8); c8->Update(); c8->RedrawAxis(); c8->SaveAs(figpath8);
    delete h8; delete c8; 

    TH1F *h9 = new TH1F("h9", ";z [mm];# beamspots", 200, bszmin, bszmax);
    TString figpath9 = fig_dir + "/bs_z0.png";
    mychain->Project("h9", "bs_z0*10");
    TCanvas *c9 = new TCanvas("c9", "c9", 800, 600); canvas_setup(c9);
    h9->SetMaximum(1.3*h9->GetMaximum());
    h9->Draw();
    write_text(0.5, 0.85, datatype_text);
    CMS_lumi(c9); c9->Update(); c9->RedrawAxis(); c9->SaveAs(figpath9);
    delete h9; delete c9; 
}

int vertex_analysis(int idx) {
    setTDRStyle();

    TString era[] = {"B", "C", "D", "E", "F", "G"};
    /* const int nera = sizeof(era) / sizeof(era[0]); */
    /* TChain *all_chain = new TChain("mytree"); */

    if(idx == 6){
        TChain *mychain = new TChain("mytree");
        for(int i=0; i<6; i++){
            mychain->Add(Form("%sanalysis.root", filepath[i].c_str()));
        }
        vertex_pos(mychain, all_era);
    }

    else{
        TChain *mychain = new TChain("mytree");
        mychain->Add(Form("%sanalysis.root", filepath[idx].c_str()));
        vertex_pos(mychain, era[idx]);
    }

    /* for(int i=0; i<nera; i++){ */
    /*     TChain *mychain = new TChain("mytree"); */
    /*     mychain->Add(Form("%sanalysis.root", filepath[i].c_str())); */
    /*     all_chain->Add(Form("%sanalysis.root", filepath[i].c_str())); */
    /*     vertex_pos(mychain, era[i]); */
    /*     delete mychain; */
    /* } */

    /* vertex_pos(all_chain, all_era); */
    /* delete all_chain; */

    return 0;
}
