import sys
import ROOT
from ROOT import RDataFrame, RDF, gInterpreter
import json
import numpy as np

nbins = 90

with open("tuplelist.json") as f:
    cfg = json.load(f)


def build_globs_by_period(cfg: dict, period: str):
    if period not in cfg:
        raise KeyError(
            f"period '{period}' not found in config keys: {list(cfg.keys())}"
        )

    section = cfg[period]
    data_entries = section.get("data", []) or []
    mc_entries = section.get("mc", []) or []

    data_files = [
        f"/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/{period}/{era['dataset']}/skimmed_*.root"
        for era in data_entries
        if "dataset" in era
    ]
    mc_files = [
        f"/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/{period}/{era['dataset']}/skimmed_*.root"
        for era in mc_entries
        if "dataset" in era
    ]

    return data_files, mc_files


def make_hist(df):
    model = ROOT.RDF.TH1DModel("h", "", nbins, 0.5, nbins + 0.5)
    h = df.Histo1D(model, "ev_nPV").GetValue()
    if h.Integral() > 0:
        h.Scale(1.0 / h.Integral())
    return h


def build_factors(h_ratio):
    dataPU_fac, mcPU_fac = [], []
    for i in range(nbins):
        dataPU_fac.append(1.0)
        mcPU_fac.append(h_ratio.GetBinContent(i + 1))
    return dataPU_fac, mcPU_fac


def declare_lookup_func(name, factors):
    init = ", ".join(f"{x:.7g}" for x in factors)
    code = f"""
    float {name}(int npv){{
        static const float lut[{nbins}] = {{{init}}};
        return lut[npv-1];
    }}
    """
    ROOT.gInterpreter.Declare(code)

def PU_factor(period: str) -> int:

    data_in, mc_in = build_globs_by_period(cfg, period)

    df_data = ROOT.RDataFrame("mytree", data_in)
    df_mc = ROOT.RDataFrame("mytree", mc_in)

    h_data = make_hist(df_data)
    h_mc = make_hist(df_mc)

    h_ratio = h_data.Clone("h_ratio")
    h_ratio.Divide(h_mc)

    dataPU_fac, mcPU_fac = build_factors(h_ratio)

    declare_lookup_func("compute_factor_data", dataPU_fac)
    declare_lookup_func("compute_factor_mc", mcPU_fac)

    out_data = (
        f"/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/{period}/data_corr.root"
    )
    out_mc = f"/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/{period}/mc_corr.root"

    df_data.Define("PU_factor", "compute_factor_data(ev_nPV)").Snapshot(
        "mytree", out_data
    )
    df_mc.Define("PU_factor", "compute_factor_mc(ev_nPV)").Snapshot("mytree", out_mc)

    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python PU_factor.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    sys.exit(PU_factor(sys.argv[1]))
