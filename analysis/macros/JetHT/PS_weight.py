import sys
import ROOT
from ROOT import RDataFrame, RDF, gInterpreter
import json
import numpy as np

def PS_weight(period: str) -> int:

    with open("tuplelist.json", "r") as f:
        fileconfig = json.load(f)

    data_files = [f"{entry['path']}/*.root" for entry in fileconfig[period]["data"]]

    df_data = ROOT.RDataFrame("residuals/tree", data_files)

    triggers = ["PFHT1050","PFHT890","PFHT780","PFHT680","PFHT590","PFHT510","PFHT430","PFHT370","PFHT250","PFHT180"]

    with open("triggerPS.json", "r") as f:
        triggerconfig = json.load(f)

    weight_data = []

    for i, t in enumerate(triggers):
        veto_terms = [f"!trig_{tt}_pass" for tt in triggers[:i]]
        current = f"trig_{t}_pass"
        if veto_terms:
            cut = " && ".join(veto_terms + [current])
        else:
            cut = current
        ndata = df_data.Filter(cut).Count().GetValue()

        Leff = triggerconfig[period][f"HLT_{t}"]

        PS_weight = Leff / ndata

        weight_entry = {"trigger": t, "cut": cut, "PS_weight": PS_weight} if ndata > 0 else 0

        weight_data.append(weight_entry)

    with open(f"/eos/home-k/kakang/IPres/analysis/JetHT/pileup/{period}/PS_weight.json", "w") as f:
        json.dump(weight_data, f, indent=4)

    return 0

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python official_PU_weight.py <period>  # e.g. 2022 or 2022EE")
        sys.exit(1)
    sys.exit(PS_weight(sys.argv[1]))
