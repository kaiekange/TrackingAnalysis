import sys
import ROOT
from ROOT import RDataFrame, RDF, gInterpreter
import json
import numpy as np

nbins = 99

def make_hist(df):
    model = ROOT.RDF.TH1DModel("h", "", nbins, 0, nbins)
    h = df.Define("totweight", "Xsec_weight * PS_weight * TRG_mask").Histo1D(model, "NumTrueInts", "totweight").GetValue()
    if h.Integral() > 0:
        h.Scale(1.0 / h.Integral())
    return h


def build_factors(h_ratio):
    mcPU_fac = []
    for i in range(nbins):
        mcPU_fac.append(h_ratio.GetBinContent(i + 1))
    return mcPU_fac


def declare_lookup_func(name, factors):
    init = ", ".join(f"{x:.7g}" for x in factors)
    code = f"""
    float {name}(int NumTrueInts){{
        static const float lut[{nbins}] = {{{init}}};
        return lut[NumTrueInts];
    }}
    """
    ROOT.gInterpreter.Declare(code)

def official_PU_weight(period: str) -> int:
    
    mc_files = f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/MC_*_XsecPS.root"

    pileup_hist = f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/pileupHistogram.root"

    df_mc = ROOT.RDataFrame("mytree", mc_files)
    pileup_file = ROOT.TFile.Open(pileup_hist)

    h_pileup = pileup_file.Get("pileup")
    h_pileup.Scale(1.0 / h_pileup.Integral())
    h_mc = make_hist(df_mc)

    h_ratio = h_pileup.Clone("h_ratio")
    h_ratio.Divide(h_mc)

    mcPU_fac = build_factors(h_ratio)

    declare_lookup_func("compute_factor_mc", mcPU_fac)

    out_mc = f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/mc_corr_mask.root"

    df_mc.Define("PU_factor", "compute_factor_mc(NumTrueInts)").Snapshot("mytree", out_mc)

    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python official_PU_weight.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    sys.exit(official_PU_weight(sys.argv[1]))
