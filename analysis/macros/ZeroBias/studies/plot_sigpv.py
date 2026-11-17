"""Median sigma_PV (no pull) of the probe tracks (0.1 < pT < 10 GeV, |eta| < 2.5) vs pT (per |eta| slice)
and vs eta (per pT range), d0 and dz,
from the output of sigpv_hists.C. Also prints max/min of the median sigma_PV,dz vs |eta| inside each |eta| slice
at 3 < pT < 10 GeV (the criterion used for the JetHT slices).
Usage (cmsenv): python3 plot_sigpv.py <sigpv.root> <legend title> <outdir> [pT max, default 10]"""
import sys, os, numpy as np, matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep, ROOT

fin, title, out = sys.argv[1], sys.argv[2], sys.argv[3]
os.makedirs(out, exist_ok=True)
f = ROOT.TFile.Open(fin)
MIN_ENTRIES = 100
VL = {"d0": "d_0", "dz": "d_z"}
# ZeroBias phase space: 0.1 < pT < 10 GeV (user 2026-11-17: no > 10 GeV for ZeroBias; 0.1-1 GeV shown on request),
# |eta| < 2.5 (the histograms of sigpv_hists.C cover 0.1-15 GeV, |eta| < 3; histogram index = position in the full
# lists, so slice 2.5-3.0 = s9 and pT 10-15 = p3 are skipped)
# optional 4th argument: pT upper limit (10 or 15 GeV; 15 adds the 10-15 GeV range, e.g. for JetHT)
SLICES = [0.0, 0.8, 1.1, 1.3, 1.5, 1.7, 1.9, 2.1, 2.3, 2.5]
PT_MIN, PT_MAX, ETA_MAX = 0.1, float(sys.argv[4]) if len(sys.argv) > 4 else 10.0, 2.5
PTR = [0.1, 1.0, 3.0, 10.0] + ([15.0] if PT_MAX > 10.0 else [])


def medians(h2, kind="eta"):
    x, y = [], []
    for i in range(1, h2.GetNbinsX() + 1):
        c = h2.GetXaxis().GetBinCenter(i)
        if (kind == "pt" and not PT_MIN < c < PT_MAX) or (kind == "eta" and abs(c) > ETA_MAX):
            continue
        p = h2.ProjectionY("_p", i, i)
        if p.GetEntries() < MIN_ENTRIES:
            continue
        q = np.zeros(1)
        p.GetQuantiles(1, q, np.array([0.5]))
        x.append(h2.GetXaxis().GetBinCenter(i))
        y.append(q[0])
    return np.array(x), np.array(y)


hep.style.use(hep.style.CMS)
colors = plt.cm.tab10.colors
for v in ("d0", "dz"):
    for kind, n, lab, xlab in (("pt", len(SLICES) - 1, [rf"${SLICES[s]:g} < |\eta| < {SLICES[s + 1]:g}$" for s in range(len(SLICES) - 1)], r"$p_T$ [GeV]"),
                               ("eta", len(PTR) - 1, [rf"${PTR[p]:g} < p_T < {PTR[p + 1]:g}$ GeV" for p in range(len(PTR) - 1)], r"$\eta$")):
        fig, ax = plt.subplots(figsize=(8.5, 7))
        vmax = 0
        for k in range(n):
            x, y = medians(f.Get(f"sig_{v}_{kind}_s{k}" if kind == "pt" else f"sig_{v}_{kind}_p{k}"), kind)
            ax.plot(x, y, "o", ms=3, color=colors[k % 10], label=lab[k])
            vmax = max(vmax, y.max())
        ax.set_ylim(0, 1.45 * vmax)
        ax.set_xlabel(xlab)
        ax.set_ylabel(rf"median $\sigma_{{PV,{VL[v]}}}$ [µm]")
        ax.legend(title=title, loc="upper center", ncol=2, fontsize=13, title_fontsize=15)
        hep.cms.label(label="Preliminary", data=True, com=13.6, ax=ax)
        fig.savefig(f"{out}/sigpv_{v}_vs_{kind}.png", dpi=150, bbox_inches="tight")
        plt.close(fig)

# JetHT slice criterion: max/min of the median sigma_PV,dz vs |eta| inside each slice, 3 < pT < 10 GeV
x, y = medians(f.Get("sig_dz_eta_p2"))
print("slice           max/min of median sigma_PV,dz (3-10 GeV)   median range [um]")
for s in range(len(SLICES) - 1):
    m = (np.abs(x) >= SLICES[s]) & (np.abs(x) < SLICES[s + 1])
    print(f"{SLICES[s]:.1f}-{SLICES[s + 1]:.1f}        {y[m].max() / y[m].min():.3f}                                   {y[m].min():.1f} - {y[m].max():.1f}")
print("written to", out)
