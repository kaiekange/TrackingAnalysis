#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TSystem.h>
#include <TColor.h>

#include <nlohmann/json.hpp>

#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"

const TString datatype_text = "Unbiased collision events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/ZeroBias";
const Int_t nbins = 200;

const std::vector<TString> periods = {
    "2022",
    "2022EE",
    "2023",
    "2023BPix",
    "2024"};

const Color_t colors[]     = {kBlack, kRed+1, kAzure+7, kGreen+2, kOrange+7};
const Style_t markers[]    = {20,     21,   22,    33,         23          };
const Style_t linestyles[] = {1,      1,    1,     2,          1           };

void draw_all_graphs(std::vector<TGraph> &graphs,
                     Float_t height,
                     TString addtext,
                     TString xlabel,
                     TString ylabel,
                     TString figpath,
                     bool dynamic_range = true)
{
    if (graphs.empty())
        return;

    Float_t floor_val = height;
    for (auto &g : graphs) {
        Double_t *y_arr = g.GetY();
        for (Int_t i = 0; i < g.GetN(); ++i)
            if (y_arr[i] > 0.0 && Float_t(y_arr[i]) < floor_val)
                floor_val = Float_t(y_arr[i]);
    }
    Float_t margin = 0.2f * (height - floor_val);

    lumi_sqrtS = "13.6 TeV, Run 3";

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas_setup(canvas);
    canvas->SetBottomMargin(0.15);
    canvas->SetRightMargin(0.17);

    graphs[0].SetMarkerStyle(markers[0]);
    graphs[0].SetMarkerSize(0.9);
    graphs[0].SetMarkerColor(colors[0]);
    graphs[0].SetLineColor(colors[0]);
    graphs[0].SetLineStyle(linestyles[0]);
    graphs[0].SetLineWidth(2);
    graphs[0].Draw("AP");
    graphs[0].GetYaxis()->SetTitle(ylabel);
    graphs[0].GetXaxis()->SetTitle(xlabel);
    if (dynamic_range) {
        graphs[0].SetMaximum(height + 1.5f * margin);
        graphs[0].SetMinimum(std::max(0.f, floor_val - margin));
    } else {
        graphs[0].SetMaximum(height * 1.3f);
        graphs[0].SetMinimum(0.f);
    }
    graphs[0].GetYaxis()->SetNdivisions(810);

    const size_t nGraphs = graphs.size();

    for (size_t i = 1; i < nGraphs; ++i)
    {
        graphs[i].SetMarkerStyle(markers[i]);
        graphs[i].SetMarkerSize(0.9);
        graphs[i].SetMarkerColor(colors[i]);
        graphs[i].SetLineColor(colors[i]);
        graphs[i].SetLineStyle(linestyles[i]);
        graphs[i].SetLineWidth(2);
        graphs[i].Draw("P SAME");
    }
    write_text(0.5, 0.85, addtext);

    TLegend *mylegend = new TLegend(0.84, 0.5, 0.96, 0.9, "", "brNDC");
    for (size_t i = 0; i < nGraphs && i < periods.size(); ++i)
    {
        TString tmp_period = periods[i];
        tmp_period.ReplaceAll("_", " ");
        mylegend->AddEntry(const_cast<TGraph *>(&graphs[i]), tmp_period, "lp");
    }
    mylegend->SetTextFont(42);
    mylegend->SetTextColor(kBlack);
    mylegend->SetBorderSize(0);
    mylegend->SetFillColor(0);
    mylegend->SetLineWidth(0);
    mylegend->SetTextSize(0.04);
    mylegend->Draw();
    CMS_lumi(canvas);

    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + ".png");

    // graphs[0].SetMaximum(height * 100);
    // graphs[0].SetMinimum(10);
    // canvas->SetLogy(1);
    // canvas->Update();
    // canvas->RedrawAxis();
    // canvas->SaveAs(figpath + "_log.png");

    delete mylegend;
    delete canvas;
}

Int_t compare_ip_res()
{
    setTDRStyle();

    Float_t new_height_d0_pt_loeta = 0;
    Float_t new_height_dz_pt_loeta = 0;
    Float_t new_height_d0_pt_hieta = 0;
    Float_t new_height_dz_pt_hieta = 0;
    Float_t new_height_d0_pt_alleta = 0;
    Float_t new_height_dz_pt_alleta = 0;
    Float_t height_d0_pt_loeta = 0;
    Float_t height_dz_pt_loeta = 0;
    Float_t height_d0_pt_hieta = 0;
    Float_t height_dz_pt_hieta = 0;
    Float_t height_d0_pt_uleta = 0;
    Float_t height_dz_pt_uleta = 0;
    Float_t height_d0_pt_alleta = 0;
    Float_t height_dz_pt_alleta = 0;
    Float_t height_d0_eta_lopt = 0;
    Float_t height_dz_eta_lopt = 0;
    Float_t height_d0_eta_hipt = 0;
    Float_t height_dz_eta_hipt = 0;
    Float_t height_d0_eta_ulpt = 0;
    Float_t height_dz_eta_ulpt = 0;
    Float_t height_d0_eta_allpt = 0;
    Float_t height_dz_eta_allpt = 0;
    Float_t height_d0_phi_lopt = 0;
    Float_t height_dz_phi_lopt = 0;
    Float_t height_d0_phi_hipt = 0;
    Float_t height_dz_phi_hipt = 0;
    Float_t height_d0_phi_ulpt = 0;
    Float_t height_dz_phi_ulpt = 0;
    Float_t height_d0_phi_allpt = 0;
    Float_t height_dz_phi_allpt = 0;

    std::vector<TGraph> new_graphs_d0_pt_loeta;
    std::vector<TGraph> new_graphs_dz_pt_loeta;
    std::vector<TGraph> new_graphs_d0_pt_hieta;
    std::vector<TGraph> new_graphs_dz_pt_hieta;
    std::vector<TGraph> new_graphs_d0_pt_alleta;
    std::vector<TGraph> new_graphs_dz_pt_alleta;
    std::vector<TGraph> graphs_d0_pt_loeta;
    std::vector<TGraph> graphs_dz_pt_loeta;
    std::vector<TGraph> graphs_d0_pt_hieta;
    std::vector<TGraph> graphs_dz_pt_hieta;
    std::vector<TGraph> graphs_d0_pt_uleta;
    std::vector<TGraph> graphs_dz_pt_uleta;
    std::vector<TGraph> graphs_d0_pt_alleta;
    std::vector<TGraph> graphs_dz_pt_alleta;
    std::vector<TGraph> graphs_d0_eta_lopt;
    std::vector<TGraph> graphs_dz_eta_lopt;
    std::vector<TGraph> graphs_d0_eta_hipt;
    std::vector<TGraph> graphs_dz_eta_hipt;
    std::vector<TGraph> graphs_d0_eta_ulpt;
    std::vector<TGraph> graphs_dz_eta_ulpt;
    std::vector<TGraph> graphs_d0_eta_allpt;
    std::vector<TGraph> graphs_dz_eta_allpt;
    std::vector<TGraph> graphs_d0_phi_lopt;
    std::vector<TGraph> graphs_dz_phi_lopt;
    std::vector<TGraph> graphs_d0_phi_hipt;
    std::vector<TGraph> graphs_dz_phi_hipt;
    std::vector<TGraph> graphs_d0_phi_ulpt;
    std::vector<TGraph> graphs_dz_phi_ulpt;
    std::vector<TGraph> graphs_d0_phi_allpt;
    std::vector<TGraph> graphs_dz_phi_allpt;

    TString figdir = storage_dir + "/figures/ip_res/";
    if (gSystem->AccessPathName(figdir))
        gSystem->MakeDirectory(figdir);

    for (TString period : periods)
    {
        Float_t reso_data_d0_pt_loeta[nbins];
        Float_t reso_data_dz_pt_loeta[nbins];
        Float_t reso_data_d0_pt_hieta[nbins];
        Float_t reso_data_dz_pt_hieta[nbins];
        Float_t reso_data_d0_pt_uleta[nbins];
        Float_t reso_data_dz_pt_uleta[nbins];
        Float_t reso_data_d0_pt_alleta[nbins];
        Float_t reso_data_dz_pt_alleta[nbins];

        Float_t reso_data_d0_eta_lopt[nbins];
        Float_t reso_data_dz_eta_lopt[nbins];
        Float_t reso_data_d0_eta_hipt[nbins];
        Float_t reso_data_dz_eta_hipt[nbins];
        Float_t reso_data_d0_eta_ulpt[nbins];
        Float_t reso_data_dz_eta_ulpt[nbins];
        Float_t reso_data_d0_eta_allpt[nbins];
        Float_t reso_data_dz_eta_allpt[nbins];

        Float_t reso_data_d0_phi_lopt[nbins];
        Float_t reso_data_dz_phi_lopt[nbins];
        Float_t reso_data_d0_phi_hipt[nbins];
        Float_t reso_data_dz_phi_hipt[nbins];
        Float_t reso_data_d0_phi_ulpt[nbins];
        Float_t reso_data_dz_phi_ulpt[nbins];
        Float_t reso_data_d0_phi_allpt[nbins];
        Float_t reso_data_dz_phi_allpt[nbins];

        Float_t eta[nbins];
        Float_t phi[nbins];
        Float_t pt[nbins];
        Float_t pt_uleta[nbins];

        for (Int_t i = 0; i < nbins; i++)
        {
            nlohmann::json results;
            std::ifstream infile(storage_dir + "/json/" + period + Form("/ip_res/fit_%d.json", i));
            infile >> results;

            eta[i] = results["eta"];
            phi[i] = results["phi"];
            pt[i] = results["pt"];
            pt_uleta[i] = results["pt_uleta"];

            reso_data_d0_pt_loeta[i] = results["reso_data_d0_pt_loeta"];
            reso_data_dz_pt_loeta[i] = results["reso_data_dz_pt_loeta"];
            reso_data_d0_pt_hieta[i] = results["reso_data_d0_pt_hieta"];
            reso_data_dz_pt_hieta[i] = results["reso_data_dz_pt_hieta"];
            reso_data_d0_pt_uleta[i] = results["reso_data_d0_pt_uleta"];
            reso_data_dz_pt_uleta[i] = results["reso_data_dz_pt_uleta"];
            reso_data_d0_pt_alleta[i] = results["reso_data_d0_pt_alleta"];
            reso_data_dz_pt_alleta[i] = results["reso_data_dz_pt_alleta"];

            reso_data_d0_eta_lopt[i] = results["reso_data_d0_eta_lopt"];
            reso_data_dz_eta_lopt[i] = results["reso_data_dz_eta_lopt"];
            reso_data_d0_eta_hipt[i] = results["reso_data_d0_eta_hipt"];
            reso_data_dz_eta_hipt[i] = results["reso_data_dz_eta_hipt"];
            reso_data_d0_eta_ulpt[i] = results["reso_data_d0_eta_ulpt"];
            reso_data_dz_eta_ulpt[i] = results["reso_data_dz_eta_ulpt"];
            reso_data_d0_eta_allpt[i] = results["reso_data_d0_eta_allpt"];
            reso_data_dz_eta_allpt[i] = results["reso_data_dz_eta_allpt"];

            reso_data_d0_phi_lopt[i] = results["reso_data_d0_phi_lopt"];
            reso_data_dz_phi_lopt[i] = results["reso_data_dz_phi_lopt"];
            reso_data_d0_phi_hipt[i] = results["reso_data_d0_phi_hipt"];
            reso_data_dz_phi_hipt[i] = results["reso_data_dz_phi_hipt"];
            reso_data_d0_phi_ulpt[i] = results["reso_data_d0_phi_ulpt"];
            reso_data_dz_phi_ulpt[i] = results["reso_data_dz_phi_ulpt"];
            reso_data_d0_phi_allpt[i] = results["reso_data_d0_phi_allpt"];
            reso_data_dz_phi_allpt[i] = results["reso_data_dz_phi_allpt"];
        }


        std::vector<double> v_pt;
        std::vector<double> v_d0_pt_loeta, v_dz_pt_loeta;
        std::vector<double> v_d0_pt_hieta, v_dz_pt_hieta;
        std::vector<double> v_d0_pt_alleta, v_dz_pt_alleta;

        for (Int_t i = 0; i < nbins; ++i)
        {
            if (pt[i] < 1.0) continue;  // 去掉 pt < 1 GeV 的点

            v_pt.push_back(pt[i]);

            v_d0_pt_loeta.push_back(reso_data_d0_pt_loeta[i]);
            v_dz_pt_loeta.push_back(reso_data_dz_pt_loeta[i]);

            v_d0_pt_hieta.push_back(reso_data_d0_pt_hieta[i]);
            v_dz_pt_hieta.push_back(reso_data_dz_pt_hieta[i]);

            v_d0_pt_alleta.push_back(reso_data_d0_pt_alleta[i]);
            v_dz_pt_alleta.push_back(reso_data_dz_pt_alleta[i]);
        }

        TGraph new_gr_data_d0_pt_loeta(v_pt.size(), v_pt.data(), v_d0_pt_loeta.data());
        TGraph new_gr_data_dz_pt_loeta(v_pt.size(), v_pt.data(), v_dz_pt_loeta.data());
        TGraph new_gr_data_d0_pt_hieta(v_pt.size(), v_pt.data(), v_d0_pt_hieta.data());
        TGraph new_gr_data_dz_pt_hieta(v_pt.size(), v_pt.data(), v_dz_pt_hieta.data());
        TGraph new_gr_data_d0_pt_alleta(v_pt.size(), v_pt.data(), v_d0_pt_alleta.data());
        TGraph new_gr_data_dz_pt_alleta(v_pt.size(), v_pt.data(), v_dz_pt_alleta.data());

        TGraph gr_data_d0_pt_loeta(nbins, pt, reso_data_d0_pt_loeta);
        TGraph gr_data_dz_pt_loeta(nbins, pt, reso_data_dz_pt_loeta);
        TGraph gr_data_d0_pt_hieta(nbins, pt, reso_data_d0_pt_hieta);
        TGraph gr_data_dz_pt_hieta(nbins, pt, reso_data_dz_pt_hieta);
        TGraph gr_data_d0_pt_uleta(nbins, pt_uleta, reso_data_d0_pt_uleta);
        TGraph gr_data_dz_pt_uleta(nbins, pt_uleta, reso_data_dz_pt_uleta);
        TGraph gr_data_d0_pt_alleta(nbins, pt, reso_data_d0_pt_alleta);
        TGraph gr_data_dz_pt_alleta(nbins, pt, reso_data_dz_pt_alleta);

        TGraph gr_data_d0_eta_lopt(nbins, eta, reso_data_d0_eta_lopt);
        TGraph gr_data_dz_eta_lopt(nbins, eta, reso_data_dz_eta_lopt);
        TGraph gr_data_d0_eta_hipt(nbins, eta, reso_data_d0_eta_hipt);
        TGraph gr_data_dz_eta_hipt(nbins, eta, reso_data_dz_eta_hipt);
        TGraph gr_data_d0_eta_ulpt(nbins, eta, reso_data_d0_eta_ulpt);
        TGraph gr_data_dz_eta_ulpt(nbins, eta, reso_data_dz_eta_ulpt);
        TGraph gr_data_d0_eta_allpt(nbins, eta, reso_data_d0_eta_allpt);
        TGraph gr_data_dz_eta_allpt(nbins, eta, reso_data_dz_eta_allpt);

        TGraph gr_data_d0_phi_lopt(nbins, phi, reso_data_d0_phi_lopt);
        TGraph gr_data_dz_phi_lopt(nbins, phi, reso_data_dz_phi_lopt);
        TGraph gr_data_d0_phi_hipt(nbins, phi, reso_data_d0_phi_hipt);
        TGraph gr_data_dz_phi_hipt(nbins, phi, reso_data_dz_phi_hipt);
        TGraph gr_data_d0_phi_ulpt(nbins, phi, reso_data_d0_phi_ulpt);
        TGraph gr_data_dz_phi_ulpt(nbins, phi, reso_data_dz_phi_ulpt);
        TGraph gr_data_d0_phi_allpt(nbins, phi, reso_data_d0_phi_allpt);
        TGraph gr_data_dz_phi_allpt(nbins, phi, reso_data_dz_phi_allpt);

        new_height_d0_pt_loeta = std::max(new_height_d0_pt_loeta, (Float_t)*std::max_element(v_d0_pt_loeta.begin(), v_d0_pt_loeta.end()));
        new_height_dz_pt_loeta = std::max(new_height_dz_pt_loeta, (Float_t)*std::max_element(v_dz_pt_loeta.begin(), v_dz_pt_loeta.end()));
        new_height_d0_pt_hieta = std::max(new_height_d0_pt_hieta, (Float_t)*std::max_element(v_d0_pt_hieta.begin(), v_d0_pt_hieta.end()));
        new_height_dz_pt_hieta = std::max(new_height_dz_pt_hieta, (Float_t)*std::max_element(v_dz_pt_hieta.begin(), v_dz_pt_hieta.end()));
        new_height_d0_pt_alleta = std::max(new_height_d0_pt_alleta, (Float_t)*std::max_element(v_d0_pt_alleta.begin(), v_d0_pt_alleta.end()));
        new_height_dz_pt_alleta = std::max(new_height_dz_pt_alleta, (Float_t)*std::max_element(v_dz_pt_alleta.begin(), v_dz_pt_alleta.end()));

        height_d0_pt_loeta = std::max(height_d0_pt_loeta, *std::max_element(reso_data_d0_pt_loeta, reso_data_d0_pt_loeta + nbins));
        height_dz_pt_loeta = std::max(height_dz_pt_loeta, *std::max_element(reso_data_dz_pt_loeta, reso_data_dz_pt_loeta + nbins));
        height_d0_pt_hieta = std::max(height_d0_pt_hieta, *std::max_element(reso_data_d0_pt_hieta, reso_data_d0_pt_hieta + nbins));
        height_dz_pt_hieta = std::max(height_dz_pt_hieta, *std::max_element(reso_data_dz_pt_hieta, reso_data_dz_pt_hieta + nbins));
        height_d0_pt_uleta = std::max(height_d0_pt_uleta, *std::max_element(reso_data_d0_pt_uleta, reso_data_d0_pt_uleta + nbins));
        height_dz_pt_uleta = std::max(height_dz_pt_uleta, *std::max_element(reso_data_dz_pt_uleta, reso_data_dz_pt_uleta + nbins));
        height_d0_pt_alleta = std::max(height_d0_pt_alleta, *std::max_element(reso_data_d0_pt_alleta, reso_data_d0_pt_alleta + nbins));
        height_dz_pt_alleta = std::max(height_dz_pt_alleta, *std::max_element(reso_data_dz_pt_alleta, reso_data_dz_pt_alleta + nbins));

        height_d0_eta_lopt = std::max(height_d0_eta_lopt, *std::max_element(reso_data_d0_eta_lopt, reso_data_d0_eta_lopt + nbins));
        height_dz_eta_lopt = std::max(height_dz_eta_lopt, *std::max_element(reso_data_dz_eta_lopt, reso_data_dz_eta_lopt + nbins));
        height_d0_eta_hipt = std::max(height_d0_eta_hipt, *std::max_element(reso_data_d0_eta_hipt, reso_data_d0_eta_hipt + nbins));
        height_dz_eta_hipt = std::max(height_dz_eta_hipt, *std::max_element(reso_data_dz_eta_hipt, reso_data_dz_eta_hipt + nbins));
        height_d0_eta_ulpt = std::max(height_d0_eta_ulpt, *std::max_element(reso_data_d0_eta_ulpt, reso_data_d0_eta_ulpt + nbins));
        height_dz_eta_ulpt = std::max(height_dz_eta_ulpt, *std::max_element(reso_data_dz_eta_ulpt, reso_data_dz_eta_ulpt + nbins));
        height_d0_eta_allpt = std::max(height_d0_eta_allpt, *std::max_element(reso_data_d0_eta_allpt, reso_data_d0_eta_allpt + nbins));
        height_dz_eta_allpt = std::max(height_dz_eta_allpt, *std::max_element(reso_data_dz_eta_allpt, reso_data_dz_eta_allpt + nbins));

        height_d0_phi_lopt = std::max(height_d0_phi_lopt, *std::max_element(reso_data_d0_phi_lopt, reso_data_d0_phi_lopt + nbins));
        height_dz_phi_lopt = std::max(height_dz_phi_lopt, *std::max_element(reso_data_dz_phi_lopt, reso_data_dz_phi_lopt + nbins));
        height_d0_phi_hipt = std::max(height_d0_phi_hipt, *std::max_element(reso_data_d0_phi_hipt, reso_data_d0_phi_hipt + nbins));
        height_dz_phi_hipt = std::max(height_dz_phi_hipt, *std::max_element(reso_data_dz_phi_hipt, reso_data_dz_phi_hipt + nbins));
        height_d0_phi_ulpt = std::max(height_d0_phi_ulpt, *std::max_element(reso_data_d0_phi_ulpt, reso_data_d0_phi_ulpt + nbins));
        height_dz_phi_ulpt = std::max(height_dz_phi_ulpt, *std::max_element(reso_data_dz_phi_ulpt, reso_data_dz_phi_ulpt + nbins));
        height_d0_phi_allpt = std::max(height_d0_phi_allpt, *std::max_element(reso_data_d0_phi_allpt, reso_data_d0_phi_allpt + nbins));
        height_dz_phi_allpt = std::max(height_dz_phi_allpt, *std::max_element(reso_data_dz_phi_allpt, reso_data_dz_phi_allpt + nbins));

        new_graphs_d0_pt_loeta.push_back(new_gr_data_d0_pt_loeta);
        new_graphs_dz_pt_loeta.push_back(new_gr_data_dz_pt_loeta);
        new_graphs_d0_pt_hieta.push_back(new_gr_data_d0_pt_hieta);
        new_graphs_dz_pt_hieta.push_back(new_gr_data_dz_pt_hieta);
        new_graphs_d0_pt_alleta.push_back(new_gr_data_d0_pt_alleta);
        new_graphs_dz_pt_alleta.push_back(new_gr_data_dz_pt_alleta);
        graphs_d0_pt_loeta.push_back(gr_data_d0_pt_loeta);
        graphs_dz_pt_loeta.push_back(gr_data_dz_pt_loeta);
        graphs_d0_pt_hieta.push_back(gr_data_d0_pt_hieta);
        graphs_dz_pt_hieta.push_back(gr_data_dz_pt_hieta);
        graphs_d0_pt_uleta.push_back(gr_data_d0_pt_uleta);
        graphs_dz_pt_uleta.push_back(gr_data_dz_pt_uleta);
        graphs_d0_pt_alleta.push_back(gr_data_d0_pt_alleta);
        graphs_dz_pt_alleta.push_back(gr_data_dz_pt_alleta);
        graphs_d0_eta_lopt.push_back(gr_data_d0_eta_lopt);
        graphs_dz_eta_lopt.push_back(gr_data_dz_eta_lopt);
        graphs_d0_eta_hipt.push_back(gr_data_d0_eta_hipt);
        graphs_dz_eta_hipt.push_back(gr_data_dz_eta_hipt);
        graphs_d0_eta_ulpt.push_back(gr_data_d0_eta_ulpt);
        graphs_dz_eta_ulpt.push_back(gr_data_dz_eta_ulpt);
        graphs_d0_eta_allpt.push_back(gr_data_d0_eta_allpt);
        graphs_dz_eta_allpt.push_back(gr_data_dz_eta_allpt);
        graphs_d0_phi_lopt.push_back(gr_data_d0_phi_lopt);
        graphs_dz_phi_lopt.push_back(gr_data_dz_phi_lopt);
        graphs_d0_phi_hipt.push_back(gr_data_d0_phi_hipt);
        graphs_dz_phi_hipt.push_back(gr_data_dz_phi_hipt);
        graphs_d0_phi_ulpt.push_back(gr_data_d0_phi_ulpt);
        graphs_dz_phi_ulpt.push_back(gr_data_dz_phi_ulpt);
        graphs_d0_phi_allpt.push_back(gr_data_d0_phi_allpt);
        graphs_dz_phi_allpt.push_back(gr_data_dz_phi_allpt);
    }

    draw_all_graphs(new_graphs_d0_pt_loeta, new_height_d0_pt_loeta, "#splitline{" + datatype_text + "}{|#it{#eta}|<1.3}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "new_d0_pt_loeta");
    draw_all_graphs(new_graphs_d0_pt_hieta, new_height_d0_pt_hieta, "#splitline{" + datatype_text + "}{1.3<|#it{#eta}|<2.5}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "new_d0_pt_hieta");
    draw_all_graphs(new_graphs_d0_pt_alleta, new_height_d0_pt_alleta, datatype_text, "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "new_d0_pt_alleta");

    draw_all_graphs(graphs_d0_pt_loeta, height_d0_pt_loeta, "#splitline{" + datatype_text + "}{|#it{#eta}|<1.3}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_loeta");
    draw_all_graphs(graphs_d0_pt_hieta, height_d0_pt_hieta, "#splitline{" + datatype_text + "}{1.3<|#it{#eta}|<2.5}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_hieta");
    draw_all_graphs(graphs_d0_pt_uleta, height_d0_pt_uleta, "#splitline{" + datatype_text + "}{2.5<|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_uleta");
    draw_all_graphs(graphs_d0_pt_alleta, height_d0_pt_alleta, datatype_text, "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_pt_alleta");

    draw_all_graphs(graphs_d0_eta_lopt, height_d0_eta_lopt, "#splitline{" + datatype_text + "}{0.1<#it{p_{T}}<1 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_lopt");
    draw_all_graphs(graphs_d0_eta_hipt, height_d0_eta_hipt, "#splitline{" + datatype_text + "}{1<#it{p_{T}}<3 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_hipt");
    draw_all_graphs(graphs_d0_eta_ulpt, height_d0_eta_ulpt, "#splitline{" + datatype_text + "}{3<#it{p_{T}}<10 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_ulpt");
    draw_all_graphs(graphs_d0_eta_allpt, height_d0_eta_allpt, datatype_text, "Track #it{#eta}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_eta_allpt");

    draw_all_graphs(graphs_d0_phi_lopt, height_d0_phi_lopt, "#splitline{" + datatype_text + "}{0.1<#it{p_{T}}<1 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_lopt", false);
    draw_all_graphs(graphs_d0_phi_hipt, height_d0_phi_hipt, "#splitline{" + datatype_text + "}{1<#it{p_{T}}<3 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_hipt", false);
    draw_all_graphs(graphs_d0_phi_ulpt, height_d0_phi_ulpt, "#splitline{" + datatype_text + "}{3<#it{p_{T}}<10 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_ulpt", false);
    draw_all_graphs(graphs_d0_phi_allpt, height_d0_phi_allpt, datatype_text, "Track #it{#phi}", "Track IP resolution #it{d_{xy}} [#mum]", figdir + "d0_phi_allpt", false);

    draw_all_graphs(new_graphs_dz_pt_loeta, new_height_dz_pt_loeta, "#splitline{" + datatype_text + "}{|#it{#eta}|<1.3}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "new_dz_pt_loeta");
    draw_all_graphs(new_graphs_dz_pt_hieta, new_height_dz_pt_hieta, "#splitline{" + datatype_text + "}{1.3<|#it{#eta}|<2.5}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "new_dz_pt_hieta");
    draw_all_graphs(new_graphs_dz_pt_alleta, new_height_dz_pt_alleta, datatype_text, "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "new_dz_pt_alleta");

    draw_all_graphs(graphs_dz_pt_loeta, height_dz_pt_loeta, "#splitline{" + datatype_text + "}{|#it{#eta}|<1.3}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_loeta");
    draw_all_graphs(graphs_dz_pt_hieta, height_dz_pt_hieta, "#splitline{" + datatype_text + "}{1.3<|#it{#eta}|<2.5}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_hieta");
    draw_all_graphs(graphs_dz_pt_uleta, height_dz_pt_uleta, "#splitline{" + datatype_text + "}{2.5<|#it{#eta}|<3.0}", "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_uleta");
    draw_all_graphs(graphs_dz_pt_alleta, height_dz_pt_alleta, datatype_text, "Track #it{p_{T}} [GeV]", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_pt_alleta");

    draw_all_graphs(graphs_dz_eta_lopt, height_dz_eta_lopt, "#splitline{" + datatype_text + "}{0.1<#it{p_{T}}<1 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_lopt");
    draw_all_graphs(graphs_dz_eta_hipt, height_dz_eta_hipt, "#splitline{" + datatype_text + "}{1<#it{p_{T}}<3 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_hipt");
    draw_all_graphs(graphs_dz_eta_ulpt, height_dz_eta_ulpt, "#splitline{" + datatype_text + "}{3<#it{p_{T}}<10 GeV}", "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_ulpt");
    draw_all_graphs(graphs_dz_eta_allpt, height_dz_eta_allpt, datatype_text, "Track #it{#eta}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_eta_allpt");

    draw_all_graphs(graphs_dz_phi_lopt, height_dz_phi_lopt, "#splitline{" + datatype_text + "}{0.1<#it{p_{T}}<1 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_lopt", false);
    draw_all_graphs(graphs_dz_phi_hipt, height_dz_phi_hipt, "#splitline{" + datatype_text + "}{1<#it{p_{T}}<3 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_hipt", false);
    draw_all_graphs(graphs_dz_phi_ulpt, height_dz_phi_ulpt, "#splitline{" + datatype_text + "}{3<#it{p_{T}}<10 GeV}", "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_ulpt", false);
    draw_all_graphs(graphs_dz_phi_allpt, height_dz_phi_allpt, datatype_text, "Track #it{#phi}", "Track IP resolution #it{d_{z}} [#mum]", figdir + "dz_phi_allpt", false);

    return 0;
}
