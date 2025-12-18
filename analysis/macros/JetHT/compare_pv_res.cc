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

const TString datatype_text = "High-q^{2} multi-jet events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";
const Int_t nbins = 50;

const std::vector<TString> periods = {
    "2022_preEE",
    "2022_postEE",
    "2023_preBPix",
    "2023_postBPix",
    "2024"
};

// 颜色常量（非字符串）
const Color_t colors[] = {kBlack, kRed, kBlue, kGreen + 2, kOrange + 7};

void draw_all_graphs(std::vector<TGraph> &graphs,
                Float_t height,
                TString ylabel,
                TString figpath)
{
    if (graphs.empty())
        return;

    lumi_sqrtS = "13.6 TeV, Run 3";

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas_setup(canvas);
    canvas->SetBottomMargin(0.15);
    
    graphs[0].SetMarkerStyle(20);
    graphs[0].SetMarkerSize(0.5);
    graphs[0].SetMarkerColor(colors[0]);
    graphs[0].Draw("AP");
    graphs[0].GetYaxis()->SetTitle(ylabel);
    graphs[0].GetXaxis()->SetTitle("#sqrt{#sum #it{p_{T}}^{2}} [GeV]");
    graphs[0].SetMaximum(height * 1.5);
    graphs[0].SetMinimum(0.0);
    graphs[0].GetYaxis()->SetNdivisions(810);

    const size_t nGraphs = graphs.size();

    for (size_t i = 1; i < nGraphs; ++i)
    {
        graphs[i].SetMarkerStyle(20);
        graphs[i].SetMarkerSize(0.5);
        graphs[i].SetMarkerColor(colors[i]);
        graphs[i].Draw("P SAME");
    }
    write_text(0.55, 0.85, datatype_text);

    TLegend *mylegend = new TLegend(0.55, 0.6, 0.65, 0.8, "", "brNDC");
    for (size_t i = 0; i < nGraphs && i < periods.size(); ++i)
    {
        TString tmp_period = periods[i];
        tmp_period.ReplaceAll("_", " ");
        mylegend->AddEntry(const_cast<TGraph *>(&graphs[i]), tmp_period, "p");
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
    canvas->SaveAs(figpath);

    delete mylegend;
    delete canvas;
}

Int_t compare_pv_res()
{
    setTDRStyle();

    Float_t height_pvx = 0;
    Float_t height_pvy = 0;
    Float_t height_pvz = 0;

    std::vector<TGraph> graphs_pvx;
    std::vector<TGraph> graphs_pvy;
    std::vector<TGraph> graphs_pvz;

    TString figdir = storage_dir + "/figures/pv_res/";
    if (gSystem->AccessPathName(figdir))
        gSystem->MakeDirectory(figdir);

    for (TString period : periods)
    {

        TString period_title = period;
        period_title.ReplaceAll("_", " ");
        lumi_sqrtS = "13.6 TeV, " + period_title;

        Float_t sumpt2_sqrt[nbins];
        Float_t reso_data_pvx[nbins];
        Float_t reso_data_pvy[nbins];
        Float_t reso_data_pvz[nbins];

        for (Int_t i = 0; i < nbins; ++i)
        {
            nlohmann::json results;

            TString json_path = storage_dir + "/json/" + period + Form("/pv_res/fit_%d.json", i);
            std::ifstream infile(json_path.Data());
            if (!infile.is_open())
            {
                std::cerr << "Cannot open " << json_path << std::endl;
                return 1;
            }
            infile >> results;

            sumpt2_sqrt[i] = results["sumpt2_sqrt"];
            reso_data_pvx[i] = results["reso_data_pvx"];
            reso_data_pvy[i] = results["reso_data_pvy"];
            reso_data_pvz[i] = results["reso_data_pvz"];
        }

        TGraph gr_data_pvx(nbins, sumpt2_sqrt, reso_data_pvx);
        TGraph gr_data_pvy(nbins, sumpt2_sqrt, reso_data_pvy);
        TGraph gr_data_pvz(nbins, sumpt2_sqrt, reso_data_pvz);

        height_pvx = std::max(height_pvx,
                              *std::max_element(reso_data_pvx, reso_data_pvx + nbins));
        height_pvy = std::max(height_pvy,
                              *std::max_element(reso_data_pvy, reso_data_pvy + nbins));
        height_pvz = std::max(height_pvz,
                              *std::max_element(reso_data_pvz, reso_data_pvz + nbins));

        graphs_pvx.push_back(gr_data_pvx);
        graphs_pvy.push_back(gr_data_pvy);
        graphs_pvz.push_back(gr_data_pvz);
    }

    draw_all_graphs(graphs_pvx, height_pvx, "PV resolution in x [#mum]", figdir + "pvx.png");
    draw_all_graphs(graphs_pvy, height_pvy, "PV resolution in y [#mum]", figdir + "pvy.png");
    draw_all_graphs(graphs_pvz, height_pvz, "PV resolution in z [#mum]", figdir + "pvz.png");

    return 0;
}