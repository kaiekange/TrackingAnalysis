import sys
import ROOT
from ROOT import RDataFrame, RDF
import json
import numpy as np
import os


def get_quantile_edges_from_hist(hist, nbins, low, high):
    probs = np.linspace(0.0, 1.0, nbins + 1, dtype="float64")
    qs = np.zeros_like(probs)
    hist.GetQuantiles(len(probs), qs, probs)
    qs[0] = low
    qs[-1] = high
    return qs.tolist()


def binning() -> int:

    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/json/", exist_ok=True)

    data_in = ["/eos/cms/store/group/phys_tracking/kakang/Run3TrackingAnalysis/Ntuple/Track-v20260210/JetMET/Run2022E_27Jun2023_v1_MINIAOD_S1M0/260303_124533/0000/*.root"]

    df_data = ROOT.RDataFrame("residuals/tree", data_in)

    sumpt_model = ROOT.RDF.TH1DModel("sumpt_hist", "", 100000, 30.0, 400.0)
    sumpt_hist = df_data.Define("pv_SumTrackPt2_sqrt", "sqrt(pv_SumTrackPt2)").Histo1D(sumpt_model, "pv_SumTrackPt2_sqrt").GetValue()
    pv_SumTrackPt_binedges = get_quantile_edges_from_hist(sumpt_hist, 50, 30.0, 400.0)

    trk_pt_model = ROOT.RDF.TH1DModel("trk_pt_hist", "", 100000, 0.1, 15.0)
    trk_pt_hist = df_data.Histo1D(trk_pt_model, "pv_trk_pt").GetValue()
    pv_trk_pt_binedges = get_quantile_edges_from_hist(trk_pt_hist, 200, 0.1, 15.0)

    trk_pt_uleta_model = ROOT.RDF.TH1DModel("trk_pt_uleta_hist", "", 100000, 1, 15.0)
    df_sel = df_data.Define("pv_trk_pt_uleta", "pv_trk_pt[abs(pv_trk_eta)>2.5 && abs(pv_trk_eta)<3.0]")
    trk_pt_uleta_hist = df_sel.Histo1D(trk_pt_uleta_model, "pv_trk_pt_uleta").GetValue()
    pv_trk_pt_uleta_binedges = get_quantile_edges_from_hist(trk_pt_uleta_hist, 200, 1, 15.0)

    trk_eta_model = ROOT.RDF.TH1DModel("trk_eta_hist", "", 100000, -3.0, 3.0)
    trk_eta_hist = df_data.Histo1D(trk_eta_model, "pv_trk_eta").GetValue()
    pv_trk_eta_binedges = get_quantile_edges_from_hist(trk_eta_hist, 200, -3.0, 3.0)

    trk_phi_model = ROOT.RDF.TH1DModel("trk_phi_hist", "", 100000, -np.pi, np.pi)
    trk_phi_hist = df_data.Histo1D(trk_phi_model, "pv_trk_phi").GetValue()
    pv_trk_phi_binedges = get_quantile_edges_from_hist(trk_phi_hist, 200, -np.pi, np.pi)

    splitparams = {
        "pv_SumTrackPt2_sqrt": pv_SumTrackPt_binedges,
        "pv_trk_pt": pv_trk_pt_binedges,
        "pv_trk_pt_uleta": pv_trk_pt_uleta_binedges,
        "pv_trk_eta": pv_trk_eta_binedges,
        "pv_trk_phi": pv_trk_phi_binedges,
    }

    out_json = f"/eos/home-k/kakang/IPres/analysis/JetHT/json/binning.json"
    with open(out_json, "w") as outfile:
        json.dump(splitparams, outfile, indent=4)

    print(f"Binning saved to {out_json}")

    return 0


if __name__ == "__main__":
    sys.exit(binning())
