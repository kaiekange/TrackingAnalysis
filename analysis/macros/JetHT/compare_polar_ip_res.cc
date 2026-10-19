#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>

#include <TCanvas.h>
#include <TGraphPolar.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TSystem.h>
#include <TMath.h>

#include <nlohmann/json.hpp>

#include "../../functions/tdrStyle.cc"
#include "../../functions/CMS_lumi.cc"
#include "../../functions/draw_funcs.cc"

const TString datatype_text = "Multi-jet events";
const TString storage_dir = "/eos/home-k/kakang/IPres/analysis/JetHT";
const Int_t nbins = 200;

const std::vector<TString> periods = {
    "2022",
    "2022EE",
    "2023",
    "2023BPix",
    "2024"};

const Color_t colors[] = {kBlack, kRed + 1, kAzure + 7, kGreen + 2, kOrange + 7};
const Style_t markers[] = {20, 21, 22, 33, 23};
const Style_t linestyles[] = {1, 1, 1, 2, 1};

TGraphPolar make_polar_graph(Int_t n, const Float_t *phi, const Float_t *y)
{
    std::vector<Double_t> vt, vr;
    for (Int_t i = 0; i < n; ++i)
    {
        if (y[i] <= 0.f)
            continue;
        Double_t theta = (phi[i] < 0) ? phi[i] + 2 * TMath::Pi() : (Double_t)phi[i];
        vt.push_back(theta);
        vr.push_back((Double_t)y[i]);
    }
    std::vector<Int_t> idx(vt.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&](Int_t a, Int_t b)
              { return vt[a] < vt[b]; });
    std::vector<Double_t> st(vt.size()), sr(vr.size());
    for (size_t i = 0; i < idx.size(); ++i)
    {
        st[i] = vt[idx[i]];
        sr[i] = vr[idx[i]];
    }
    return TGraphPolar((Int_t)st.size(), st.data(), sr.data());
}

Double_t valid_max_arr(const Float_t *arr, Int_t n)
{
    Double_t m = 0.0;
    for (Int_t i = 0; i < n; ++i)
        if (arr[i] > 0)
            m = std::max(m, (Double_t)arr[i]);
    return m;
}

void draw_polar_graphs(std::vector<TGraphPolar> &graphs,
                       Double_t r_max,
                       TString addtext,
                       TString rlabel,
                       TString figpath)
{
    if (graphs.empty())
        return;

    // 800x600 canvas; polar circle lives in a 600x600 square sub-pad (left 75%)
    // so the polar circle is round, legend goes in the remaining right strip
    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas->SetFillColor(0);
    canvas->SetBorderMode(0);

    TPad *polar_pad = new TPad("polar_pad", "", 0.0, 0.0, 0.75, 1.0);
    polar_pad->SetFillColor(0);
    polar_pad->SetBorderMode(0);
    polar_pad->SetLeftMargin(0.08);
    polar_pad->SetRightMargin(0.08);
    polar_pad->SetTopMargin(0.08);
    polar_pad->SetBottomMargin(0.08);
    polar_pad->Draw();
    polar_pad->cd();

    const size_t nGraphs = graphs.size();
    for (size_t i = 0; i < nGraphs; ++i)
    {
        graphs[i].SetMarkerStyle(markers[i]);
        graphs[i].SetMarkerSize(0.8);
        graphs[i].SetMarkerColor(colors[i]);
        graphs[i].SetLineColor(colors[i]);
        graphs[i].SetLineStyle(linestyles[i]);
        graphs[i].SetLineWidth(2);
        graphs[i].Draw(i == 0 ? "ALP" : "LP");
    }

    // Update first so ROOT creates the polargram, THEN configure it.
    // After configuring, change graphs[0]'s draw option from "ALP" to "LP"
    // so that canvas->Update() below doesn't recreate (and reset) the polargram.
    polar_pad->Update();
    auto *pg = graphs[0].GetPolargram();
    if (pg)
    {
        pg->SetRangeRadial(0, r_max);
        pg->SetToDegree();
        pg->SetNdivPolar(4);
        pg->SetNdivRadial(5);
        pg->SetTextFont(42);
        pg->SetTextSize(0.04);
        pg->SetLineColor(kGray);
        pg->SetLineWidth(1);
    }
    polar_pad->GetListOfPrimitives()->Remove(&graphs[0]);
    graphs[0].AppendPad("LP");
    polar_pad->Modified();
    polar_pad->Update();

    canvas->cd();

    // addtext and radial label go in the right strip, clear of the polar circle
    TLatex annot;
    annot.SetNDC();
    annot.SetTextFont(42);
    annot.SetTextSize(0.038);
    annot.SetTextAlign(32);
    annot.DrawLatex(0.97, 0.38, rlabel);
    annot.SetTextSize(0.034);
    annot.DrawLatex(0.97, 0.28, addtext);

    TLegend *mylegend = new TLegend(0.76, 0.45, 0.97, 0.88, "", "brNDC");
    for (size_t i = 0; i < nGraphs && i < periods.size(); ++i)
    {
        TString tmp = periods[i];
        tmp.ReplaceAll("_", " ");
        mylegend->AddEntry(const_cast<TGraphPolar *>(&graphs[i]), tmp, "lp");
    }
    mylegend->SetTextFont(42);
    mylegend->SetTextColor(kBlack);
    mylegend->SetBorderSize(0);
    mylegend->SetFillColor(0);
    mylegend->SetFillStyle(0);
    mylegend->SetLineWidth(0);
    mylegend->SetTextSize(0.04);
    mylegend->Draw();

    TLatex hdr;
    hdr.SetNDC();
    hdr.SetTextAngle(0);
    hdr.SetTextColor(kBlack);
    hdr.SetTextFont(61);
    hdr.SetTextSize(0.050);
    hdr.SetTextAlign(11);
    hdr.DrawLatex(0.04, 0.94, "CMS");
    hdr.SetTextFont(52);
    hdr.SetTextSize(0.038);
    hdr.DrawLatex(0.14, 0.94, "Preliminary");
    hdr.SetTextFont(42);
    hdr.SetTextSize(0.042);
    hdr.SetTextAlign(31);
    hdr.DrawLatex(0.97, 0.94, "Run 3 (13.6 TeV)");

    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath + ".png");

    delete mylegend;
    delete canvas;
}

Int_t compare_polar_ip_res()
{
    setTDRStyle();

    Double_t r_max_d0_phi_lopt = 0, r_max_dz_phi_lopt = 0;
    Double_t r_max_d0_phi_hipt = 0, r_max_dz_phi_hipt = 0;
    Double_t r_max_d0_phi_ulpt = 0, r_max_dz_phi_ulpt = 0;
    Double_t r_max_d0_phi_allpt = 0, r_max_dz_phi_allpt = 0;

    std::vector<TGraphPolar> graphs_d0_phi_lopt;
    std::vector<TGraphPolar> graphs_dz_phi_lopt;
    std::vector<TGraphPolar> graphs_d0_phi_hipt;
    std::vector<TGraphPolar> graphs_dz_phi_hipt;
    std::vector<TGraphPolar> graphs_d0_phi_ulpt;
    std::vector<TGraphPolar> graphs_dz_phi_ulpt;
    std::vector<TGraphPolar> graphs_d0_phi_allpt;
    std::vector<TGraphPolar> graphs_dz_phi_allpt;

    TString figdir = storage_dir + "/figures/ip_res/";
    if (gSystem->AccessPathName(figdir))
        gSystem->MakeDirectory(figdir);

    for (TString period : periods)
    {
        Float_t phi[nbins];
        Float_t reso_data_d0_phi_lopt[nbins], reso_data_dz_phi_lopt[nbins];
        Float_t reso_data_d0_phi_hipt[nbins], reso_data_dz_phi_hipt[nbins];
        Float_t reso_data_d0_phi_ulpt[nbins], reso_data_dz_phi_ulpt[nbins];
        Float_t reso_data_d0_phi_allpt[nbins], reso_data_dz_phi_allpt[nbins];

        for (Int_t i = 0; i < nbins; i++)
        {
            nlohmann::json results;
            std::ifstream infile(storage_dir + "/json/" + period + Form("/ip_res/fit_%d.json", i));
            infile >> results;

            phi[i] = results["phi"];
            reso_data_d0_phi_lopt[i] = results["reso_data_d0_phi_lopt"];
            reso_data_dz_phi_lopt[i] = results["reso_data_dz_phi_lopt"];
            reso_data_d0_phi_hipt[i] = results["reso_data_d0_phi_hipt"];
            reso_data_dz_phi_hipt[i] = results["reso_data_dz_phi_hipt"];
            reso_data_d0_phi_ulpt[i] = results["reso_data_d0_phi_ulpt"];
            reso_data_dz_phi_ulpt[i] = results["reso_data_dz_phi_ulpt"];
            reso_data_d0_phi_allpt[i] = results["reso_data_d0_phi_allpt"];
            reso_data_dz_phi_allpt[i] = results["reso_data_dz_phi_allpt"];
        }

        r_max_d0_phi_lopt = std::max(r_max_d0_phi_lopt, valid_max_arr(reso_data_d0_phi_lopt, nbins));
        r_max_dz_phi_lopt = std::max(r_max_dz_phi_lopt, valid_max_arr(reso_data_dz_phi_lopt, nbins));
        r_max_d0_phi_hipt = std::max(r_max_d0_phi_hipt, valid_max_arr(reso_data_d0_phi_hipt, nbins));
        r_max_dz_phi_hipt = std::max(r_max_dz_phi_hipt, valid_max_arr(reso_data_dz_phi_hipt, nbins));
        r_max_d0_phi_ulpt = std::max(r_max_d0_phi_ulpt, valid_max_arr(reso_data_d0_phi_ulpt, nbins));
        r_max_dz_phi_ulpt = std::max(r_max_dz_phi_ulpt, valid_max_arr(reso_data_dz_phi_ulpt, nbins));
        r_max_d0_phi_allpt = std::max(r_max_d0_phi_allpt, valid_max_arr(reso_data_d0_phi_allpt, nbins));
        r_max_dz_phi_allpt = std::max(r_max_dz_phi_allpt, valid_max_arr(reso_data_dz_phi_allpt, nbins));

        graphs_d0_phi_lopt.push_back(make_polar_graph(nbins, phi, reso_data_d0_phi_lopt));
        graphs_dz_phi_lopt.push_back(make_polar_graph(nbins, phi, reso_data_dz_phi_lopt));
        graphs_d0_phi_hipt.push_back(make_polar_graph(nbins, phi, reso_data_d0_phi_hipt));
        graphs_dz_phi_hipt.push_back(make_polar_graph(nbins, phi, reso_data_dz_phi_hipt));
        graphs_d0_phi_ulpt.push_back(make_polar_graph(nbins, phi, reso_data_d0_phi_ulpt));
        graphs_dz_phi_ulpt.push_back(make_polar_graph(nbins, phi, reso_data_dz_phi_ulpt));
        graphs_d0_phi_allpt.push_back(make_polar_graph(nbins, phi, reso_data_d0_phi_allpt));
        graphs_dz_phi_allpt.push_back(make_polar_graph(nbins, phi, reso_data_dz_phi_allpt));
    }

    draw_polar_graphs(graphs_d0_phi_lopt, r_max_d0_phi_lopt * 1.18, "#splitline{" + datatype_text + "}{0.1<#it{p_{T}}<1 GeV}", "#it{d_{xy}} IP resolution [#mum]", figdir + "compare_polar_d0_phi_lopt");
    draw_polar_graphs(graphs_d0_phi_hipt, r_max_d0_phi_hipt * 1.18, "#splitline{" + datatype_text + "}{1<#it{p_{T}}<3 GeV}", "#it{d_{xy}} IP resolution [#mum]", figdir + "compare_polar_d0_phi_hipt");
    draw_polar_graphs(graphs_d0_phi_ulpt, r_max_d0_phi_ulpt * 1.18, "#splitline{" + datatype_text + "}{3<#it{p_{T}}<10 GeV}", "#it{d_{xy}} IP resolution [#mum]", figdir + "compare_polar_d0_phi_ulpt");
    draw_polar_graphs(graphs_d0_phi_allpt, r_max_d0_phi_allpt * 1.18, datatype_text, "#it{d_{xy}} IP resolution [#mum]", figdir + "compare_polar_d0_phi_allpt");

    draw_polar_graphs(graphs_dz_phi_lopt, r_max_dz_phi_lopt * 1.18, "#splitline{" + datatype_text + "}{0.1<#it{p_{T}}<1 GeV}", "#it{d_{z}} IP resolution [#mum]", figdir + "compare_polar_dz_phi_lopt");
    draw_polar_graphs(graphs_dz_phi_hipt, r_max_dz_phi_hipt * 1.18, "#splitline{" + datatype_text + "}{1<#it{p_{T}}<3 GeV}", "#it{d_{z}} IP resolution [#mum]", figdir + "compare_polar_dz_phi_hipt");
    draw_polar_graphs(graphs_dz_phi_ulpt, r_max_dz_phi_ulpt * 1.18, "#splitline{" + datatype_text + "}{3<#it{p_{T}}<10 GeV}", "#it{d_{z}} IP resolution [#mum]", figdir + "compare_polar_dz_phi_ulpt");
    draw_polar_graphs(graphs_dz_phi_allpt, r_max_dz_phi_allpt * 1.18, datatype_text, "#it{d_{z}} IP resolution [#mum]", figdir + "compare_polar_dz_phi_allpt");

    return 0;
}
