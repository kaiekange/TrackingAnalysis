"""sqrt<sigma_PV^2> (with pull) of the probe tracks vs pT, for dz and d0, per |eta| range (pt_* categories of the
ip_res jsons). Usage: python3 plot_sigpv_pt.py <period>"""
import sys, os, json, numpy as np, matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep

period = sys.argv[1]
LUMI = {"2022C": 5.02}
R = "/eos/home-k/kakang/IPres/analysis/JetHT"
OUT = f"{R}/test_figures/{period}/convfit"
os.makedirs(OUT, exist_ok=True)
F = [json.load(open(f"{R}/json/{period}/ip_res/fit_{i}.json")) for i in range(200)]
# pt_uleta has its own pT binning (json key "pt_uleta")
CATS = [("pt_loeta", "pt", r"$|\eta| < 1.3$", "tab:blue"), ("pt_hieta", "pt", r"$1.3 < |\eta| < 2.5$", "tab:orange"),
        ("pt_uleta", "pt_uleta", r"$2.5 < |\eta| < 3.0$", "tab:green"), ("pt_alleta", "pt", r"All $\eta$", "black")]
VL = {"d0": "d_0", "dz": "d_z"}
hep.style.use(hep.style.CMS)
for v in ("dz", "d0"):
    fig, ax = plt.subplots(figsize=(8.5, 7))
    vmax = 0
    for s, xkey, lab, c in CATS:
        x = np.array([d[xkey] for d in F])
        y = np.array([d.get(f"pverr2_data_{v}_{s}", np.nan) ** 0.5 if d.get(f"ntrk_data_{v}_{s}", 0) > 0 else np.nan
                      for d in F])
        ax.plot(x, y, "o", ms=3, color=c, label=lab)
        vmax = max(vmax, np.nanmax(y))
    ax.set_ylim(0, 1.35 * vmax)
    ax.set_xlabel(r"$p_T$ [GeV]")
    ax.set_ylabel(rf"$\sqrt{{\langle\sigma^2_{{PV,{VL[v]}}}\rangle}}$ (with pull) [µm]")
    ax.legend(title=f"Multi-jet events, {period}", loc="upper center", ncol=2, fontsize=14, title_fontsize=16)
    # mplhep 0.3.47 (cmsenv) has no text=; label= is the same "Preliminary" in this version
    hep.cms.label(label="Preliminary", data=True, lumi=LUMI[period], com=13.6, ax=ax)
    fig.savefig(f"{OUT}/sigpv_{v}_vs_pt.png", dpi=150, bbox_inches="tight")
    plt.close(fig)
print("written to", OUT)
