// IP resolution vs phi of all periods overlaid on polar plots (data only) for one sample.
// Usage: root -b -q 'compare_polar_ip_res.cc("JetHT")'
//        optional 2nd argument: output root instead of the sample storage dir
#include <numeric>

#include <TGraphPolar.h>
#include <TGraphPolargram.h>
#include <TLatex.h>
#include <TMath.h>

#include "../../functions/plot_common.cc"

TGraphPolar make_polar_graph(const std::vector<Float_t> &phi, const std::vector<Float_t> &y)
{
    std::vector<Double_t> vt, vr;
    for (size_t i = 0; i < y.size(); ++i)
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
        mylegend->AddEntry(&graphs[i], period_title(periods[i]), "lp");
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

Int_t compare_polar_ip_res(TString sample, TString outdir = "")
{
    SampleConfig cfg = sample_config(sample);
    TString figdir = figure_dir(cfg, outdir, "ip_res/");

    setTDRStyle();

    std::vector<IPPlot> plots;
    for (const IPPlot &p : ip_plots())
        if (p.axis == "phi")
            plots.push_back(p);

    std::map<TString, std::vector<TGraphPolar>> graphs;
    std::map<TString, Double_t> r_max;

    for (TString period : periods)
    {
        FitResults fits;
        if (!load_fits(cfg.storage_dir + "/json/" + period + "/ip_res", nbins_ip, fits))
            return 1;

        for (const IPPlot &p : plots)
        {
            const auto &y = fits.at(("reso_data_" + p.key).Data());
            r_max[p.key] = std::max(r_max[p.key], (Double_t)valid_max(y));
            graphs[p.key].push_back(make_polar_graph(fits.at("phi"), y));
        }
    }

    for (const IPPlot &p : plots)
    {
        TString rlabel = TString("#it{") + (p.var == "d0" ? "d_{xy}" : "d_{z}") + "} IP resolution [#mum]";
        draw_polar_graphs(graphs[p.key], r_max[p.key] * 1.18, add_region(cfg.datatype_text, p.region_text),
                          rlabel, figdir + "compare_polar_" + p.key);
    }

    return 0;
}
