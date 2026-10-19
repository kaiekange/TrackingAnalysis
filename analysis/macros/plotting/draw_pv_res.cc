// Data vs MC PV resolution and pull (with ratio) for one sample and period.
// Usage: root -b -q 'draw_pv_res.cc("JetHT", "2023")'
//        optional 3rd argument: output root instead of the sample storage dir
#include "../../functions/compare_gr.cc"

Int_t draw_pv_res(TString sample, TString period, TString outdir = "")
{
    SampleConfig cfg = sample_config(sample);
    TString figdir = figure_dir(cfg, outdir, period + "/pv_res/");

    setTDRStyle();

    FitResults fits;
    if (!load_fits(cfg.storage_dir + "/json/" + period + "/pv_res", nbins_pv, fits))
        return 1;

    const auto &x = fits.at("pv_SumTrackPt2_sqrt");
    const TString xlabel = "#sqrt{#sum#it{p_{T}}^{2}} [GeV]";

    // {key, y-axis label}; pulls use a fixed y range
    const std::vector<std::pair<TString, TString>> plots = {
        {"pvx", "PV resolution in x [#mum]"}, {"pvy", "PV resolution in y [#mum]"}, {"pvz", "PV resolution in z [#mum]"},
        {"pullx", "PV pull in x"}, {"pully", "PV pull in y"}, {"pullz", "PV pull in z"}};

    for (const auto &p : plots) {
        const auto &data = fits.at(("reso_data_" + p.first).Data());
        const auto &mc = fits.at(("reso_mc_" + p.first).Data());
        std::vector<Float_t> div = safe_ratio(data, mc);
        Float_t height = p.first.BeginsWith("pull") ? 1.5 : std::max(valid_max(data), valid_max(mc));

        compare_gr(make_graph(x, data), make_graph(x, mc), make_graph(x, div),
                   height, x.front(), x.back(),
                   valid_max(div), *std::min_element(div.begin(), div.end()),
                   period, cfg.datatype_text, xlabel, p.second, figdir + p.first);
    }

    return 0;
}
