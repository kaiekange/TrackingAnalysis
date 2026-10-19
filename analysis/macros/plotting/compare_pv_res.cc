// PV resolution of all periods overlaid (data only) for one sample.
// Usage: root -b -q 'compare_pv_res.cc("JetHT")'
//        optional 2nd argument: output root instead of the sample storage dir
#include "../../functions/plot_common.cc"

Int_t compare_pv_res(TString sample, TString outdir = "")
{
    SampleConfig cfg = sample_config(sample);
    TString figdir = figure_dir(cfg, outdir, "pv_res/");

    setTDRStyle();

    const std::vector<std::pair<TString, TString>> plots = {
        {"pvx", "PV resolution in x [#mum]"}, {"pvy", "PV resolution in y [#mum]"}, {"pvz", "PV resolution in z [#mum]"}};
    std::map<TString, std::vector<TGraph>> graphs;
    std::map<TString, Float_t> heights;

    for (TString period : periods) {
        FitResults fits;
        if (!load_fits(cfg.storage_dir + "/json/" + period + "/pv_res", nbins_pv, fits))
            return 1;

        const auto &x = fits.at("pv_SumTrackPt2_sqrt");
        for (const auto &p : plots) {
            const auto &y = fits.at(("reso_data_" + p.first).Data());
            graphs[p.first].emplace_back(nbins_pv, x.data(), y.data());
            heights[p.first] = std::max(heights[p.first], *std::max_element(y.begin(), y.end()));
        }
    }

    for (const auto &p : plots)
        draw_period_overlay(graphs[p.first], heights[p.first], cfg.datatype_text,
                            "#sqrt{#sum #it{p_{T}}^{2}} [GeV]", p.second,
                            figdir + p.first + ".png", true, 0.17, 1.1);

    return 0;
}
