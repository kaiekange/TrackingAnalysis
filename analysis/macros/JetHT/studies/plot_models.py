"""IP resolution after PV-error removal (convolution fit) for the two fit models of ip_res.cc, DSCB and sum of
3 Gaussians: one figure per model with the same axes, vs pT per |eta| slice and vs eta per pT range.
Error bars: the stored statistical errors (none where not valid). Prints the 3G/DSCB comparison.
Usage: python3 plot_models.py <period> <outdir> <lumi>"""
import sys, json, numpy as np
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep
period, outdir, lumi = sys.argv[1], sys.argv[2], float(sys.argv[3])
R = f"/eos/home-k/kakang/IPres/analysis/JetHT/json/{period}/ip_res"
D = [json.load(open(f"{R}/fit_{i}.json")) for i in range(50)]
SL = D[0]["abseta_slices"]
MODELS = (("DSCB", "_conv"), ("3 Gaussians", "_3g_conv"))
VL = {"d0": r"$d_{xy}$", "dz": r"$d_{z}$"}
hep.style.use(hep.style.CMS)

def series(var, cat, sfx, xkey):
    x, y, e = [], [], []
    for d in D:
        k = f"reso_data_{var}_{cat}"
        if d[f"fitstatus_data_{var}_{cat}"] == -2 or d[k + sfx] <= 0:
            continue
        x.append(d[xkey]); y.append(d[k + sfx]); e.append(max(d.get(k + sfx + "_err", 0), 0))
    o = np.argsort(x)
    return np.array(x)[o], np.array(y)[o], np.array(e)[o]

def figure(var, xkey, cats, labels, legtitle, name):
    ymax = 0
    for _, sfx in MODELS:
        for c in cats:
            ymax = max(ymax, series(var, c, sfx, xkey)[1].max())
    for model, sfx in MODELS:
        fig, ax = plt.subplots(figsize=(8.5, 7))
        cols = plt.cm.turbo(np.linspace(0.05, 0.95, len(cats))) if len(cats) > 2 else ["tab:blue", "tab:red"]
        for c, lab, col in zip(cats, labels, cols):
            x, y, e = series(var, c, sfx, xkey)
            ax.errorbar(x, y, yerr=e, fmt="o", ms=3, color=col, label=lab)
        ax.set_ylim(0, ymax * 1.35)
        ax.set_xlabel(r"Track $p_T$ [GeV]" if xkey == "pt" else r"Track $\eta$")
        ax.set_ylabel(f"{VL[var]} resolution, PV error removed [$\\mu$m]")
        ax.legend(title=f"{period}, {model} fit, {legtitle}", fontsize=12, title_fontsize=13, ncol=2, loc="upper right")
        hep.cms.label(label="Preliminary", data=True, com=13.6, lumi=lumi, ax=ax)
        fn = f"{outdir}/{name}_{var}_{'dscb' if model == 'DSCB' else '3g'}.png"
        fig.savefig(fn, dpi=100, bbox_inches="tight"); plt.close(fig)
        print("wrote", fn)

for var in ("d0", "dz"):
    figure(var, "pt", [f"pt_eta{s}" for s in range(9)],
           [f"{SL[s]} < $|\\eta|$ < {SL[s+1]}" for s in range(9)], "vs $p_T$", "vs_pt")
    figure(var, "eta", ["eta_hipt", "eta_ulpt"], ["1 < $p_T$ < 3 GeV", "3 < $p_T$ < 10 GeV"], "vs $\\eta$", "vs_eta")
    for cat in [f"pt_eta{s}" for s in range(9)] + ["pt_alleta", "eta_hipt", "eta_ulpt", "phi_hipt", "phi_ulpt"]:
        xkey = "pt" if cat.startswith("pt") else cat.split("_")[0]
        _, yd, _ = series(var, cat, "_conv", xkey); _, yg, _ = series(var, cat, "_3g_conv", xkey)
        _, yc, _ = series(var, cat, "_counted", xkey); _, ym, _ = series(var, cat, "", xkey); _, ym3, _ = series(var, cat, "_3g", xkey)
        if len(yd) == len(yg) == len(yc) == len(ym) == len(ym3):
            r = (yg / yd - 1) * 100; md = (ym / yc - 1) * 100; m3 = (ym3 / yc - 1) * 100
            print(f"{var} {cat:9s}: 3G/DSCB-1 (PV removed) median {np.median(r):+.2f}% [{r.min():+.2f}, {r.max():+.2f}];"
                  f"  measured vs counted: DSCB {np.median(md):+.2f}%, 3G {np.median(m3):+.2f}%")
