import sys
import ROOT
from ROOT import RDataFrame, RDF, gInterpreter
import json
import glob
import numpy as np


def make_hist(df, nbins, xmin, xmax):
    model = ROOT.RDF.TH1DModel("h", "", nbins, xmin, xmax)
    h = df.Histo1D(model, "NumTrueInts").GetValue()
    if h.Integral() > 0:
        h.Scale(1.0 / h.Integral())
    return h


def official_PU_weight(period: str) -> int:

    with open("tuplelist.json", "r") as f:
        config = json.load(f)

    mc_files = [f for entry in config[period]["mc"] for f in glob.glob(entry["path"] + "/*.root")]

    pileup_hist = f"/eos/home-k/kakang/IPres/analysis/JetHT/pileup/{period}/pileupHistogram.root"

    df_mc = ROOT.RDataFrame("residuals/tree", mc_files)
    pileup_file = ROOT.TFile.Open(pileup_hist)

    h_pileup = pileup_file.Get("pileup")

    nbins = h_pileup.GetNbinsX()
    xmin = h_pileup.GetXaxis().GetXmin()
    xmax = h_pileup.GetXaxis().GetXmax()

    h_pileup.Scale(1.0 / h_pileup.Integral())
    h_mc = make_hist(df_mc, nbins, xmin, xmax)
    h_ratio = h_pileup.Clone("h_ratio")
    h_ratio.Divide(h_mc)

    bin_data = []
    for i in range(1, nbins + 1):
        bin_entry = {"bin": i, "low_edge": h_ratio.GetBinLowEdge(i), "high_edge": h_ratio.GetBinLowEdge(i) + h_ratio.GetBinWidth(i), "content": h_ratio.GetBinContent(i)}
        bin_data.append(bin_entry)

    with open(f"/eos/home-k/kakang/IPres/analysis/JetHT/pileup/{period}/pileup_ratio.json", "w") as f:
        json.dump(bin_data, f, indent=4)

    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python official_PU_weight.py <period>  # e.g. 2022 or 2022EE")
        sys.exit(1)
    sys.exit(official_PU_weight(sys.argv[1]))
