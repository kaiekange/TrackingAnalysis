import sys
import ROOT
from ROOT import RDataFrame, RDF, gInterpreter
import json
import glob
import numpy as np

def xsec_weight(period: str) -> int:

    with open("tuplelist.json", "r") as f:
        config = json.load(f)

    weight_data = []

    for entry in config[period]["mc"]:
        mc_files = f"{entry['path']}/*.root"
        xsec = entry["xsec"]

        df_mc = ROOT.RDataFrame("residuals/RunInfo", mc_files)

        nevents = df_mc.Sum("nEventsProcessed_").GetValue()

        weight = xsec / nevents

        weight_entry = {"dataset": entry["dataset"], "xsec_weight": weight}
        weight_data.append(weight_entry)

    with open(f"/eos/home-k/kakang/IPres/analysis/JetHT/pileup/{period}/xsec_weight.json", "w") as f:
        json.dump(weight_data, f, indent=4)

    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python official_PU_weight.py <period>  # e.g. 2022 or 2022EE")
        sys.exit(1)
    sys.exit(xsec_weight(sys.argv[1]))
