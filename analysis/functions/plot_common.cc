#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <TString.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TSystem.h>

#include <nlohmann/json.hpp>

#include "tdrStyle.cc"
#include "CMS_lumi.cc"
#include "draw_funcs.cc"

// ---------- Samples, periods, luminosities ----------

struct SampleConfig {
    TString storage_dir;
    TString datatype_text;
};

SampleConfig sample_config(TString sample) {
    if (sample == "JetHT")    return {"/eos/home-k/kakang/IPres/analysis/JetHT",    "Multi-jet events"};
    if (sample == "ZeroBias") return {"/eos/home-k/kakang/IPres/analysis/ZeroBias", "Zero-bias events"};
    std::cerr << "Unknown sample " << sample << " (expected JetHT or ZeroBias)" << std::endl;
    gSystem->Exit(1);
    return {};
}

const Int_t nbins_ip = 200;
const Int_t nbins_pv = 50;

const std::vector<TString> periods = {"2022", "2022EE", "2023", "2023BPix", "2024"};

const Color_t colors[]     = {kBlack, kRed+1, kAzure+7, kGreen+2, kOrange+7};
const Style_t markers[]    = {20,     21,     22,       33,       23       };
const Style_t linestyles[] = {1,      1,      1,        2,        1        };

TString period_title(TString period) {
    period.ReplaceAll("_", " ");
    return period;
}

TString lumi_label(TString period) {
    static const std::map<TString, TString> lumi_map = {
        {"2022",     "7.99 fb^{-1}"},
        {"2022EE",   "5.81 fb^{-1}"},
        {"2023",     "17.96 fb^{-1}"},
        {"2023BPix", "9.68 fb^{-1}"},
        {"2024",     "109.82 fb^{-1}"}
    };
    auto it = lumi_map.find(period);
    return (it != lumi_map.end() ? it->second : period_title(period)) + " (13.6 TeV)";
}

// Figures go to <outdir or storage_dir>/figures/<sub>
TString figure_dir(const SampleConfig &cfg, TString outdir, TString sub) {
    TString dir = (outdir.IsNull() ? cfg.storage_dir : outdir) + "/figures/" + sub;
    if (gSystem->AccessPathName(dir))
        gSystem->mkdir(dir, kTRUE);
    return dir;
}

// ---------- Fit results ----------

typedef std::map<std::string, std::vector<Float_t>> FitResults;

// Reads <dir>/fit_<i>.json for i < nbins; every numeric key becomes a per-bin array.
bool load_fits(TString dir, Int_t nbins, FitResults &fits) {
    fits.clear();
    for (Int_t i = 0; i < nbins; ++i) {
        TString path = dir + Form("/fit_%d.json", i);
        std::ifstream infile(path.Data());
        if (!infile.is_open()) {
            std::cerr << "Cannot open " << path << std::endl;
            return false;
        }
        nlohmann::json results;
        infile >> results;
        for (auto &item : results.items())
            if (item.value().is_number()) {
                auto &v = fits[item.key()];
                v.resize(nbins, 0.f);
                v[i] = item.value().get<Float_t>();
            }
    }
    return true;
}

TGraph *make_graph(const std::vector<Float_t> &x, const std::vector<Float_t> &y) {
    std::vector<Float_t> vx, vy;
    for (size_t i = 0; i < y.size(); ++i)
        if (y[i] > 0.f) { vx.push_back(x[i]); vy.push_back(y[i]); }
    return new TGraph((Int_t)vx.size(), vx.data(), vy.data());
}

Float_t valid_max(const std::vector<Float_t> &v) {
    Float_t m = 0.f;
    for (Float_t x : v) if (x > m) m = x;
    return m;
}

std::vector<Float_t> safe_ratio(const std::vector<Float_t> &num, const std::vector<Float_t> &den) {
    std::vector<Float_t> r(num.size());
    for (size_t i = 0; i < num.size(); ++i)
        r[i] = (den[i] > 0.f) ? num[i] / den[i] : 0.f;
    return r;
}

TString add_region(TString datatype_text, TString region_text) {
    return region_text.IsNull() ? datatype_text : "#splitline{" + datatype_text + "}{" + region_text + "}";
}

// ---------- IP plot definitions: d0/dz x pt/eta/phi x 4 regions ----------

struct IPPlot {
    TString key;          // e.g. "d0_pt_loeta" -> JSON keys reso_data_<key>, reso_mc_<key>
    TString var;          // "d0" or "dz"
    TString axis;         // "pt", "eta" or "phi"
    TString region;       // "loeta", ..., "allpt"
    std::string xkey;     // JSON key of the bin centres
    TString xlabel;
    TString ylabel;
    TString region_text;  // empty for the inclusive region
};

std::vector<IPPlot> ip_plots() {
    const std::vector<std::pair<TString, TString>> eta_regions = {
        {"loeta", "|#it{#eta}|<1.3"}, {"hieta", "1.3<|#it{#eta}|<2.5"}, {"uleta", "2.5<|#it{#eta}|<3.0"}, {"alleta", ""}};
    const std::vector<std::pair<TString, TString>> pt_regions = {
        {"lopt", "0.1<#it{p_{T}}<1 GeV"}, {"hipt", "1<#it{p_{T}}<3 GeV"}, {"ulpt", "3<#it{p_{T}}<10 GeV"}, {"allpt", ""}};
    const std::vector<std::pair<TString, TString>> axes = {
        {"pt", "Track #it{p_{T}} [GeV]"}, {"eta", "Track #it{#eta}"}, {"phi", "Track #it{#phi}"}};

    std::vector<IPPlot> plots;
    for (TString var : {"d0", "dz"}) {
        TString ylabel = TString("Track IP resolution #it{") + (var == "d0" ? "d_{xy}" : "d_{z}") + "} [#mum]";
        for (auto &axis : axes) {
            auto &regions = (axis.first == "pt") ? eta_regions : pt_regions;
            for (auto &reg : regions) {
                std::string xkey = (axis.first == "pt" && reg.first == "uleta") ? "pt_uleta" : axis.first.Data();
                plots.push_back({var + "_" + axis.first + "_" + reg.first, var, axis.first, reg.first,
                                 xkey, axis.second, ylabel, reg.second});
            }
        }
    }
    return plots;
}

// ---------- Multi-period overlay (used by compare_ip_res / compare_pv_res) ----------

void draw_period_overlay(std::vector<TGraph> &graphs,
                         Float_t height,
                         TString addtext,
                         TString xlabel,
                         TString ylabel,
                         TString figpath,
                         bool dynamic_range = true,
                         Float_t bottom_margin = 0.15,
                         Float_t xtitle_offset = -1)
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

    lumi_sqrtS = "Run 3 (13.6 TeV)";

    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    canvas_setup(canvas);
    canvas->SetBottomMargin(bottom_margin);
    canvas->SetRightMargin(0.17);

    for (size_t i = 0; i < graphs.size(); ++i) {
        graphs[i].SetMarkerStyle(markers[i]);
        graphs[i].SetMarkerSize(0.9);
        graphs[i].SetMarkerColor(colors[i]);
        graphs[i].SetLineColor(colors[i]);
        graphs[i].SetLineStyle(linestyles[i]);
        graphs[i].SetLineWidth(2);
        if (i > 0) {
            graphs[i].Draw("P SAME");
            continue;
        }
        graphs[0].Draw("AP");
        graphs[0].GetYaxis()->SetTitle(ylabel);
        graphs[0].GetXaxis()->SetTitle(xlabel);
        if (xtitle_offset > 0)
            graphs[0].GetXaxis()->SetTitleOffset(xtitle_offset);
        if (dynamic_range) {
            graphs[0].SetMaximum(height + 1.5f * margin);
            graphs[0].SetMinimum(std::max(0.f, floor_val - margin));
        } else {
            graphs[0].SetMaximum(height * 1.3f);
            graphs[0].SetMinimum(0.f);
        }
        graphs[0].GetYaxis()->SetNdivisions(810);
    }
    write_text(0.5, 0.85, addtext);

    TLegend *legend = new TLegend(0.84, 0.5, 0.96, 0.9, "", "brNDC");
    for (size_t i = 0; i < graphs.size() && i < periods.size(); ++i)
        legend->AddEntry(&graphs[i], period_title(periods[i]), "p");
    legend->SetTextFont(42);
    legend->SetTextColor(kBlack);
    legend->SetBorderSize(0);
    legend->SetFillColor(0);
    legend->SetLineWidth(0);
    legend->SetTextSize(0.04);
    legend->Draw();
    CMS_lumi(canvas);

    canvas->Update();
    canvas->RedrawAxis();
    canvas->SaveAs(figpath);

    delete legend;
    delete canvas;
}
