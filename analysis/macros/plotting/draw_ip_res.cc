// Data vs MC IP resolution (with ratio) for one sample and period.
// Usage: root -b -q 'draw_ip_res.cc("JetHT", "2023")'
//        optional 3rd argument: output root instead of the sample storage dir
#include "../../functions/compare_gr.cc"

Int_t draw_ip_res(TString sample, TString period, TString outdir = "")
{
    SampleConfig cfg = sample_config(sample);
    TString figdir = figure_dir(cfg, outdir, period + "/ip_res/");

    setTDRStyle();

    FitResults fits;
    if (!load_fits(cfg.storage_dir + "/json/" + period + "/ip_res", nbins_ip, fits))
        return 1;

    for (const IPPlot &p : ip_plots()) {
        const auto &x = fits.at(p.xkey);
        const auto &data = fits.at(("reso_data_" + p.key).Data());
        const auto &mc = fits.at(("reso_mc_" + p.key).Data());
        std::vector<Float_t> div = safe_ratio(data, mc);

        compare_gr(make_graph(x, data), make_graph(x, mc), make_graph(x, div),
                   std::max(valid_max(data), valid_max(mc)),
                   x.front(), x.back(),
                   valid_max(div), *std::min_element(div.begin(), div.end()),
                   period, add_region(cfg.datatype_text, p.region_text),
                   p.xlabel, p.ylabel, figdir + p.key);
    }

    return 0;
}
