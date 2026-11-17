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


NBINS = 50         # eta / phi quantile bins (= ip_res jobs; the pT bins share the job index, at most NBINS)
PT_MIN, PT_MAX = 1.0, 15.0
# pT: fixed piecewise equal-width bins (user 2026-11-17): 1-3 GeV in 0.1, 3-5 in 0.25, 5-10 in 0.5, 10-15 in 1 GeV
# (43 bins; ZeroBias uses the same edges up to 5 GeV, then 1 GeV steps to 10 GeV)
PT_EDGES = ([round(1.0 + 0.1 * i, 2) for i in range(20)] + [round(3.0 + 0.25 * i, 2) for i in range(8)] +
            [round(5.0 + 0.5 * i, 2) for i in range(10)] + [float(x) for x in range(10, 16)])
ETA_MAX = 2.5
# |eta| slices of the IP-vs-pT categories: wide where sigma_PV,dz is flat, narrow where it rises
# (max/min of sigma_PV,dz <= 1.15 inside a slice, 2022C, 3-10 GeV)
ABSETA_SLICES = [0.0, 0.8, 1.1, 1.3, 1.5, 1.7, 1.9, 2.1, 2.3, 2.5]
N_FILES = 10       # first files of the period's file list (fill_hists.py) used for the quantiles


def binning(period="2022C") -> int:
    """Track binnings: pT fixed (PT_EDGES); eta / phi quantiles of the tracks in 1 < pT < 15 GeV, |eta| < 2.5,
    from the first N_FILES ntuples of <period>.
    The sqrt(SumPt2) PV binning is copied unchanged from the existing binning.json: the PV pulls of
    pv_res.cc are per PV bin, so changing it would invalidate them.
    Needs cmsenv (xrootd 5.7.2) and the grid proxy: the ntuples are read from T2_BE_IIHE."""
    storage = "/eos/home-k/kakang/IPres/analysis/JetHT"
    out_json = f"{storage}/json/binning.json"
    pv_edges = json.load(open(out_json))["pv_SumTrackPt2_sqrt"]

    data_in = [l.strip() for l in open(f"{storage}/hists/{period}/files.txt") if l.strip()][:N_FILES]
    df_data = ROOT.RDataFrame("residuals/tree", data_in)
    sel = f"pv_trk_pt > {PT_MIN} && pv_trk_pt < {PT_MAX} && abs(pv_trk_eta) < {ETA_MAX}"
    df_sel = df_data.Define("sel_pt", f"pv_trk_pt[{sel}]").Define("sel_eta", f"pv_trk_eta[{sel}]").Define("sel_phi", f"pv_trk_phi[{sel}]")

    trk_pt_hist = df_sel.Histo1D(ROOT.RDF.TH1DModel("trk_pt_hist", "", 100000, PT_MIN, PT_MAX), "sel_pt")
    trk_eta_hist = df_sel.Histo1D(ROOT.RDF.TH1DModel("trk_eta_hist", "", 100000, -ETA_MAX, ETA_MAX), "sel_eta")
    trk_phi_hist = df_sel.Histo1D(ROOT.RDF.TH1DModel("trk_phi_hist", "", 100000, -np.pi, np.pi), "sel_phi")
    # fail loud: an unreadable file must not silently shrink the sample
    n_events = df_data.Count()
    n_trk = trk_pt_hist.GetEntries()
    print(f"{len(data_in)} files, {n_events.GetValue()} events, {n_trk:.0f} selected tracks")
    if n_trk < 1e6:
        print("[ERROR] binning: too few tracks")
        return 1

    splitparams = {
        "pv_SumTrackPt2_sqrt": pv_edges,
        "pv_trk_pt": PT_EDGES,
        "pv_trk_eta": get_quantile_edges_from_hist(trk_eta_hist.GetValue(), NBINS, -ETA_MAX, ETA_MAX),
        "pv_trk_phi": get_quantile_edges_from_hist(trk_phi_hist.GetValue(), NBINS, -np.pi, np.pi),
        "pv_trk_abseta_slices": ABSETA_SLICES,
    }

    with open(out_json, "w") as outfile:
        json.dump(splitparams, outfile, indent=4)

    print(f"Binning saved to {out_json}")

    return 0


if __name__ == "__main__":
    sys.exit(binning())
