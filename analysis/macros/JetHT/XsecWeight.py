import sys
import ROOT
from ROOT import RDataFrame, RDF, gInterpreter
import json
import numpy as np

# nbins = 99

with open("tuplelist.json") as f:
    cfg = json.load(f)

# def build_factors(h_ratio):
#     dataPU_fac, mcPU_fac = [], []
#     for i in range(nbins):
#         dataPU_fac.append(1.0)
#         mcPU_fac.append(h_ratio.GetBinContent(i + 1))
#     return dataPU_fac, mcPU_fac


# def declare_lookup_func(name, factors):
#     init = ", ".join(f"{x:.7g}" for x in factors)
#     code = f"""
#     float {name}(int NumTrueInts){{
#         static const float lut[{nbins}] = {{{init}}};
#         return lut[NumTrueInts];
#     }}
#     """
#     ROOT.gInterpreter.Declare(code)

def XsecWeight(period: str) -> int:

    data_files = [
        f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/{entry['dataset']}/skimmed_*.root"
        for entry in cfg[period]["data"]
    ]

    df_data = ROOT.RDataFrame("mytree", data_files)

    ndata_1050 = df_data.Filter("trig_PFHT1050_pass").Count().GetValue()
    ndata_890 = df_data.Filter("trig_PFHT890_pass").Count().GetValue()
    ndata_780 = df_data.Filter("trig_PFH_780_pass").Count().GetValue()
    ndata_680 = df_data.Filter("trig_PFHT680_pass").Count().GetValue()
    ndata_590 = df_data.Filter("trig_PFHT590_pass").Count().GetValue()
    ndata_510 = df_data.Filter("trig_PFHT510_pass").Count().GetValue()
    ndata_430 = df_data.Filter("trig_PFHT430_pass").Count().GetValue()
    ndata_370 = df_data.Filter("trig_PFHT370_pass").Count().GetValue()
    ndata_250 = df_data.Filter("trig_PFHT250_pass").Count().GetValue()
    ndata_180 = df_data.Filter("trig_PFHT180_pass").Count().GetValue()

    df_data.SnapShot(f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/data.root")

    for entry in cfg["2022_preEE"]["mc"]:

        mc_files = f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/{entry['dataset']}/skimmed_*.root"
        
        info_mc = ROOT.RDataFrame("Info", mc_files)
        total_entries = info_mc.Sum("total_entries").GetValue()

        Xsec = cfg[period]["mc"][entry['era']]["xsec"]
        Xsec_weight = Xsec / total_entries

        df_mc = ROOT.RDataFrame("mytree", mc_files)

        df_mc.Define("Xsec_weight", f"{Xsec_weight}")

    # pileup_file = ROOT.TFile.Open(pileup_in)

    # h_ratio = h_pileup.Clone("h_ratio")
    # h_ratio.Divide(h_mc)

    # dataPU_fac, mcPU_fac = build_factors(h_ratio)

    # # declare_lookup_func("compute_factor_data", dataPU_fac)
    # declare_lookup_func("compute_factor_mc", mcPU_fac)

    # # out_data = (
    # #     f"/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/{period}/data_corr.root"
    # # )
    # out_mc = f"/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/{period}/mc_corr.root"

    # # df_data.Define("PU_factor", "compute_factor_data(ev_nPV)").Snapshot(
    # #     "mytree", out_data
    # # )
    # df_mc.Define("PU_factor", "compute_factor_mc(NumTrueInts)").Snapshot("mytree", out_mc)

    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python PU_factor.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    sys.exit(XsecWeight(sys.argv[1]))
