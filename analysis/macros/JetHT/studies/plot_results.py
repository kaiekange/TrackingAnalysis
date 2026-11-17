"""IP resolution of one sample and period (DSCB fit): vs pT per |eta| slice, vs eta and vs phi per pT range,
measured and PV error removed (convolution fit). Works with pt / eta / phi binnings of different sizes (a json has
the pt / eta / phi key only where that binning has the bin). Error bars: stored stat. errors (none where invalid).
Usage (cmsenv): python3 plot_results.py <JetHT|ZeroBias> <period> <outdir> [lumi in fb^-1]"""
import sys, os, glob, json, numpy as np
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep

sample, period, outdir = sys.argv[1], sys.argv[2], sys.argv[3]
lumi = float(sys.argv[4]) if len(sys.argv) > 4 else None
R = f"/eos/home-k/kakang/IPres/analysis/{sample}/json/{period}/ip_res"
files = sorted(glob.glob(f"{R}/fit_*.json"), key=lambda p: int(p.split("_")[-1].split(".")[0]))
D = [json.load(open(p)) for p in files]
os.makedirs(outdir, exist_ok=True)
SL = D[0]["abseta_slices"]
EVT = {"JetHT": "Multi-jet events", "ZeroBias": "Zero-bias events"}[sample]
RANGES = [("lopt", "0.1 < $p_T$ < 1 GeV"), ("hipt", "1 < $p_T$ < 3 GeV"), ("ulpt", "3 < $p_T$ < 10 GeV"),
          ("vhpt", "10 < $p_T$ < 15 GeV")]
VL = {"d0": r"$d_{xy}$", "dz": r"$d_{z}$"}
SFX = {"": "measured", "_conv": "PV error removed"}
hep.style.use(hep.style.CMS)


def series(var, cat, sfx, xkey):
    x, y, e = [], [], []
    for d in D:
        k = f"reso_data_{var}_{cat}"
        if xkey not in d or k + sfx not in d or d[f"fitstatus_data_{var}_{cat}"] == -2 or d[k + sfx] <= 0:
            continue
        x.append(d[xkey]); y.append(d[k + sfx]); e.append(max(d.get(k + sfx + "_err", 0), 0))
    o = np.argsort(x)
    return np.array(x)[o], np.array(y)[o], np.array(e)[o]


def figure(var, sfx, xkey, cats, labels, name):
    cats_labels = [(c, l) for c, l in zip(cats, labels) if len(series(var, c, sfx, xkey)[0])]
    if not cats_labels:
        return
    fig, ax = plt.subplots(figsize=(8.5, 7))
    cols = plt.cm.turbo(np.linspace(0.05, 0.95, len(cats_labels))) if len(cats_labels) > 4 else \
        ["tab:blue", "tab:orange", "tab:green", "tab:red"][:len(cats_labels)]
    ymax = 0
    for (c, lab), col in zip(cats_labels, cols):
        x, y, e = series(var, c, sfx, xkey)
        ax.errorbar(x, y, yerr=e, fmt="o", ms=3, color=col, label=lab)
        ymax = max(ymax, y.max())
    ax.set_ylim(0, ymax * 1.35)
    ax.set_xlabel({"pt": r"Track $p_T$ [GeV]", "eta": r"Track $\eta$", "phi": r"Track $\phi$"}[xkey])
    ax.set_ylabel(f"{VL[var]} resolution, {SFX[sfx]} [$\\mu$m]")
    ax.legend(title=f"{EVT}, {period}", fontsize=12, title_fontsize=13, ncol=2, loc="upper right")
    hep.cms.label(label="Preliminary", data=True, com=13.6, ax=ax, **({"lumi": lumi} if lumi else {}))
    fig.savefig(f"{outdir}/{name}.png", dpi=150, bbox_inches="tight")
    plt.close(fig)


for var in ("d0", "dz"):
    for sfx in SFX:
        tag = "meas" if sfx == "" else "conv"
        figure(var, sfx, "pt", [f"pt_eta{s}" for s in range(len(SL) - 1)],
               [f"{SL[s]:g} < $|\\eta|$ < {SL[s + 1]:g}" for s in range(len(SL) - 1)], f"{var}_vs_pt_{tag}")
        for xkey in ("eta", "phi"):
            figure(var, sfx, xkey, [f"{xkey}_{r}" for r, _ in RANGES], [l for _, l in RANGES], f"{var}_vs_{xkey}_{tag}")
print("written to", outdir, sorted(os.listdir(outdir)))
