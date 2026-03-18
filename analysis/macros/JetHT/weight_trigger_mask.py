import sys
import ROOT
from ROOT import RDataFrame, RDF, gInterpreter
import json
import numpy as np

def weight_trigger_mask(period: str) -> int:

    with open("tuplelist.json", "r") as f:
        fileconfig = json.load(f)

    triggers = ["PFHT1050","PFHT890","PFHT780","PFHT680","PFHT590","PFHT510","PFHT430","PFHT370","PFHT250","PFHT180"]

    mask_data = {}

    for entry in fileconfig[period]["mc"]:
        mc_files = f"{entry['path']}/*.root"
        dataset = entry["dataset"]

        df_RunInfo_mc = ROOT.RDataFrame("residuals/RunInfo", mc_files)
        df_tree_mc = ROOT.RDataFrame("residuals/tree", mc_files)

        nevents = df_RunInfo_mc.Sum("nEventsProcessed_").GetValue()

        dataset_mask = {}

        for i, t in enumerate(triggers):
            veto_terms = [f"!trig_{tt}_pass" for tt in triggers[:i]]
            current = f"trig_{t}_pass"
            if veto_terms:
                cut = " && ".join(veto_terms + [current])
            else:
                cut = current
            ntrigger = df_tree_mc.Filter(cut).Count().GetValue()
            dataset_mask[t] = bool(ntrigger < 0.01 * nevents)

        mask_data[dataset] = dataset_mask

    out = {period: mask_data}
    with open(f"/eos/home-k/kakang/IPres/analysis/JetHT/pileup/{period}/trigger_mask.json", "w") as f:
        json.dump(out, f, indent=4)

    return 0

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python official_PU_weight.py <period>  # e.g. 2022 or 2022EE")
        sys.exit(1)
    sys.exit(weight_trigger_mask(sys.argv[1]))
