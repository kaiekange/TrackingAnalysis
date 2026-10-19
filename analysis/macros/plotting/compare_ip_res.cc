// IP resolution of all periods overlaid (data only) for one sample.
// Usage: root -b -q 'compare_ip_res.cc("JetHT")'
//        optional 2nd argument: output root instead of the sample storage dir
#include "../../functions/plot_common.cc"

Int_t compare_ip_res(TString sample, TString outdir = "")
{
    SampleConfig cfg = sample_config(sample);
    TString figdir = figure_dir(cfg, outdir, "ip_res/");

    setTDRStyle();

    const std::vector<IPPlot> plots = ip_plots();
    std::map<TString, std::vector<TGraph>> graphs;
    std::map<TString, Float_t> heights;

    // "new_" plots: pt dependence with pt < 1 GeV dropped
    const std::vector<TString> new_keys = {"d0_pt_loeta", "dz_pt_loeta", "d0_pt_hieta", "dz_pt_hieta", "d0_pt_alleta", "dz_pt_alleta"};
    std::map<TString, std::vector<TGraph>> new_graphs;
    std::map<TString, Float_t> new_heights;

    for (TString period : periods) {
        FitResults fits;
        if (!load_fits(cfg.storage_dir + "/json/" + period + "/ip_res", nbins_ip, fits))
            return 1;

        for (const IPPlot &p : plots) {
            const auto &x = fits.at(p.xkey);
            const auto &y = fits.at(("reso_data_" + p.key).Data());
            graphs[p.key].emplace_back(nbins_ip, x.data(), y.data());
            heights[p.key] = std::max(heights[p.key], *std::max_element(y.begin(), y.end()));
        }

        const auto &pt = fits.at("pt");
        for (const TString &key : new_keys) {
            const auto &y = fits.at(("reso_data_" + key).Data());
            std::vector<double> vx, vy;
            for (Int_t i = 0; i < nbins_ip; ++i) {
                if (pt[i] < 1.0) continue;
                vx.push_back(pt[i]);
                vy.push_back(y[i]);
            }
            new_graphs[key].emplace_back((Int_t)vx.size(), vx.data(), vy.data());
            new_heights[key] = std::max(new_heights[key], (Float_t)*std::max_element(vy.begin(), vy.end()));
        }
    }

    for (const IPPlot &p : plots) {
        TString addtext = add_region(cfg.datatype_text, p.region_text);
        // phi plots keep a wide fixed y range, pt/eta zoom to the data
        draw_period_overlay(graphs[p.key], heights[p.key], addtext, p.xlabel, p.ylabel,
                            figdir + p.key + ".png", p.axis != "phi");
        if (new_graphs.count(p.key))
            draw_period_overlay(new_graphs[p.key], new_heights[p.key], addtext, p.xlabel, p.ylabel,
                                figdir + "new_" + p.key + ".png");
    }

    return 0;
}
