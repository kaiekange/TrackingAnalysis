"""IP resolution of two eras on one figure (same binning.json, so the same bins): vs pT per |eta| slice and vs
eta / phi per pT range, measured and with the PV error removed (convolution fit), with a ratio panel era2/era1.
Era 1: open markers, era 2: filled. Error bars: the stored statistical errors (none where not valid).
Prints the median era2/era1 - 1 per category.
Usage: python3 compare_eras.py <era1> <lumi1> <era2> <lumi2> <model: 3g|dscb> <outdir>"""
import sys, json, numpy as np
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep
era1, lumi1, era2, lumi2, model, outdir = sys.argv[1], float(sys.argv[2]), sys.argv[3], float(sys.argv[4]), sys.argv[5], sys.argv[6]
ERAS = (era1, era2)
R = "/eos/home-k/kakang/IPres/analysis/JetHT/json/{}/ip_res"
D = {e: [json.load(open(f"{R.format(e)}/fit_{i}.json")) for i in range(50)] for e in ERAS}
SL = D[era1][0]["abseta_slices"]
if D[era2][0]["abseta_slices"] != SL or any(D[era1][i]["pt"] != D[era2][i]["pt"] for i in range(50)):
    sys.exit("[ERROR] the two eras have different binnings")
M = "_3g" if model == "3g" else ""
QUANT = (("meas", M, "measured"), ("conv", M + "_conv", "PV error removed"))
VL = {"d0": r"$d_{xy}$", "dz": r"$d_{z}$"}
hep.style.use(hep.style.CMS)

def series(era, var, cat, sfx, xkey):
    """x, y, err per bin; nan where not fitted or not valid"""
    x, y, e = [], [], []
    for d in D[era]:
        k = f"reso_data_{var}_{cat}"
        ok = d[f"fitstatus_data_{var}_{cat}"] != -2 and d[k + sfx] > 0
        x.append(d[xkey]); y.append(d[k + sfx] if ok else np.nan); e.append(max(d.get(k + sfx + "_err", 0), 0) if ok else 0)
    o = np.argsort(x)
    return np.array(x)[o], np.array(y)[o], np.array(e)[o]

def figure(var, xkey, cats, labels, name, xlabel):
    for q, sfx, qlab in QUANT:
        fig, (ax, rx) = plt.subplots(2, 1, figsize=(8.5, 9), sharex=True, gridspec_kw={"height_ratios": [3, 1.2], "hspace": 0.05})
        cols = plt.cm.turbo(np.linspace(0.05, 0.95, len(cats))) if len(cats) > 2 else ["tab:blue", "tab:red"]
        ymax, rats = 0, []
        for c, lab, col in zip(cats, labels, cols):
            x, y1, e1 = series(era1, var, c, sfx, xkey)
            _, y2, e2 = series(era2, var, c, sfx, xkey)
            ax.errorbar(x, y1, yerr=e1, fmt="o", ms=4, mfc="none", color=col)
            ax.errorbar(x, y2, yerr=e2, fmt="o", ms=4, color=col, label=lab)
            r = y2 / y1
            re = np.where((e1 > 0) & (e2 > 0), r * np.sqrt((e1 / y1) ** 2 + (e2 / y2) ** 2), 0)
            rx.errorbar(x, r, yerr=re, fmt="o", ms=3, color=col)
            ymax = max(ymax, np.nanmax(y1), np.nanmax(y2)); rats.append(r)
        # marker legend: open = era 1, filled = era 2
        ax.plot([], [], "o", color="gray", mfc="none", label=f"{era1} ({lumi1:g} fb$^{{-1}}$)")
        ax.plot([], [], "o", color="gray", label=f"{era2} ({lumi2:g} fb$^{{-1}}$)")
        ax.set_ylim(0, ymax * 1.4)
        ax.set_ylabel(f"{VL[var]} resolution, {qlab} [$\\mu$m]", fontsize=20)
        ax.legend(title=f"{'3 Gaussians' if model == '3g' else 'DSCB'} fit", fontsize=12, title_fontsize=13, ncol=2, loc="upper right")
        hep.cms.label(label="Preliminary", data=True, ax=ax, rlabel="(13.6 TeV)")
        a = np.concatenate(rats); a = a[np.isfinite(a)]
        lo, hi = np.percentile(a, 1), np.percentile(a, 99)
        rx.axhline(1, color="gray", lw=1)
        rx.set_ylim(min(lo, 1) - 0.02, max(hi, 1) + 0.02)
        rx.set_ylabel(f"{era2} / {era1}", fontsize=20); rx.set_xlabel(xlabel)
        fig.align_ylabels([ax, rx])
        fn = f"{outdir}/{name}_{var}_{q}_{model}.png"
        fig.savefig(fn, dpi=100, bbox_inches="tight"); plt.close(fig)
        print("wrote", fn)

PT = ["1 < $p_T$ < 3 GeV", "3 < $p_T$ < 10 GeV"]
for var in ("d0", "dz"):
    figure(var, "pt", [f"pt_eta{s}" for s in range(9)], [f"{SL[s]} < $|\\eta|$ < {SL[s+1]}" for s in range(9)],
           "vs_pt", r"Track $p_T$ [GeV]")
    figure(var, "eta", ["eta_hipt", "eta_ulpt"], PT, "vs_eta", r"Track $\eta$")
    figure(var, "phi", ["phi_hipt", "phi_ulpt"], PT, "vs_phi", r"Track $\phi$")

print(f"\nmedian {era2}/{era1} - 1 [%] (min, max), {model}:")
for var in ("d0", "dz"):
    for cat in [f"pt_eta{s}" for s in range(9)] + ["pt_alleta", "eta_hipt", "eta_ulpt", "phi_hipt", "phi_ulpt"]:
        xkey = "pt" if cat.startswith("pt") else cat.split("_")[0]
        out = []
        for q, sfx, _ in QUANT:
            r = series(era2, var, cat, sfx, xkey)[1] / series(era1, var, cat, sfx, xkey)[1] - 1
            r = r[np.isfinite(r)] * 100
            out.append(f"{q} {np.median(r):+6.2f} ({r.min():+6.2f}, {r.max():+6.2f})")
        print(f"{var} {cat:9s}: " + ";  ".join(out))
