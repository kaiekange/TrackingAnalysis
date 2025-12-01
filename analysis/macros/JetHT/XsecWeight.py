import sys
import ROOT
from ROOT import RDataFrame, RDF, gInterpreter
import json

with open("/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/JetHT/tuplelist.json") as f:
    cfg_tuple = json.load(f)
with open("/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/JetHT/triggerPS.json") as f:
    cfg_trigger = json.load(f)

def XsecWeight(period: str, dataset: str) -> int:

    # data_files = [
    #     f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/{entry['dataset']}/skimmed_*.root"
    #     for entry in cfg_tuple[period]["data"]
    # ]
    data_files = f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/data.root"

    df_data = ROOT.RDataFrame("mytree", data_files)

    ndata = {}
    ndata["1050"] = df_data.Filter("trig_PFHT1050_pass").Count().GetValue()
    ndata["890"]  = df_data.Filter("(!trig_PFHT1050_pass)&&trig_PFHT890_pass").Count().GetValue()
    ndata["780"]  = df_data.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&trig_PFHT780_pass").Count().GetValue()
    ndata["680"]  = df_data.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&trig_PFHT680_pass").Count().GetValue()
    ndata["590"]  = df_data.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&trig_PFHT590_pass").Count().GetValue()
    ndata["510"]  = df_data.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&trig_PFHT510_pass").Count().GetValue()
    ndata["430"]  = df_data.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&trig_PFHT430_pass").Count().GetValue()
    ndata["370"]  = df_data.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&trig_PFHT370_pass").Count().GetValue()
    ndata["250"]  = df_data.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&trig_PFHT250_pass").Count().GetValue()
    ndata["180"]  = df_data.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&(!trig_PFHT250_pass)&&trig_PFHT180_pass").Count().GetValue()

    # print(ndata)

    # df_data.Snapshot("mytree", f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/data.root")

    for entry in cfg_tuple[period]["mc"]:

        # if (entry['dataset'] == "MC_50to80"): continue
        # if (entry['dataset'] == "MC_80to120"): continue
        # if (entry['dataset'] == "MC_120to170"): continue
        # if (entry['dataset'] == "MC_170to300"): continue
        if (entry['dataset'] != dataset): continue

        mc_files = f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/{entry['dataset']}/skimmed_*.root"
        out_mc = f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/{entry['dataset']}_XsecPS.root"

        info_mc = ROOT.RDataFrame("Info", mc_files)
        total_entries = info_mc.Sum("total_entries").GetValue()

        Xsec = entry["xsec"]
        Xsec_weight = Xsec / total_entries
        # print(f"Xsec: {Xsec}, total entries: {total_entries}, weight: {Xsec_weight}")

        df_mc = ROOT.RDataFrame("mytree", mc_files)


        nmc = {}

        nmc["1050"] = df_mc.Filter("trig_PFHT1050_pass").Count().GetValue()
        nmc["890"]  = df_mc.Filter("(!trig_PFHT1050_pass)&&trig_PFHT890_pass").Count().GetValue()
        nmc["780"]  = df_mc.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&trig_PFHT780_pass").Count().GetValue()
        nmc["680"]  = df_mc.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&trig_PFHT680_pass").Count().GetValue()
        nmc["590"]  = df_mc.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&trig_PFHT590_pass").Count().GetValue()
        nmc["510"]  = df_mc.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&trig_PFHT510_pass").Count().GetValue()
        nmc["430"]  = df_mc.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&trig_PFHT430_pass").Count().GetValue()
        nmc["370"]  = df_mc.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&trig_PFHT370_pass").Count().GetValue()
        nmc["250"]  = df_mc.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&trig_PFHT250_pass").Count().GetValue()
        nmc["180"]  = df_mc.Filter("(!trig_PFHT1050_pass)&&(!trig_PFHT890_pass)&&(!trig_PFHT780_pass)&&(!trig_PFHT680_pass)&&(!trig_PFHT590_pass)&&(!trig_PFHT510_pass)&&(!trig_PFHT430_pass)&&(!trig_PFHT370_pass)&&(!trig_PFHT250_pass)&&trig_PFHT180_pass").Count().GetValue()

        Leff = {}
        PS_weight = {}
        TRG_mask = {}

        trigs = ["1050", "890", "780", "680", "590", "510", "430", "370", "250", "180"]
        for trig in trigs:
            Leff[trig] = cfg_trigger[period][f"HLT_PFHT{trig}"]["Leff"]
            PS_weight[trig] = Leff[trig] / ndata[trig]
            # print(f"HLT_PFHT{trig}: {PS_weight[trig]}")
            frac = nmc[trig] / total_entries if total_entries > 0 else 0.0
            if frac < 0.001:
                TRG_mask[trig] = 0
            else:
                TRG_mask[trig] = 1

        # expr = ""
        # for i, trig in enumerate(trigs):
        #     expr += f"(trig_PFHT{trig}_pass ? {PS_weight[trig]} : "
        # expr += "0.0" + ")" * len(trigs)

        # df_mc = df_mc.Define("Xsec_weight", f"{Xsec_weight}") \
        #             .Define("PS_weight", expr).Snapshot("mytree", out_mc)

        # PS_weight: 按优先级选对应 trigger 的 PS_weight[trig]
        expr = ""
        for trig in trigs:
            expr += f"(trig_PFHT{trig}_pass ? {PS_weight[trig]} : "
        expr += "0.0" + ")" * len(trigs)

        # mask: 按优先级选对应 trigger 的 TRG_mask[trig] (0 或 1)，与 PS_weight 无关
        mask_expr = ""
        for trig in trigs:
            mask_expr += f"(trig_PFHT{trig}_pass ? {TRG_mask[trig]} : "
        mask_expr += "0" + ")" * len(trigs)

        df_mc.Define("Xsec_weight", f"{Xsec_weight}") \
                     .Define("PS_weight", expr) \
                     .Define("TRG_mask", mask_expr) \
                     .Snapshot("mytree", out_mc)        



    return 0


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python PU_factor.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    sys.exit(XsecWeight(sys.argv[1], sys.argv[2]))
