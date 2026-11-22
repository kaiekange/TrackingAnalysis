"""IP resolution of several eras of one sample on one figure (same binning.json, so the same bins), with a ratio panel
era / first era: one figure per |eta| slice (vs pT) and per pT range (vs eta, vs phi where present), measured and
with the PV error removed (convolution fit). Error bars: the stored statistical errors (none where not valid).
Prints the median era / first era - 1 per category.
Usage (cmsenv): python3 compare_eras.py <JetHT|ZeroBias> <model: dscb|3g> <outdir> <era1[:lumi]> <era2[:lumi]> ..."""
import sys, os, glob, json, numpy as np
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep

sample, model, outdir = sys.argv[1], sys.argv[2], sys.argv[3]
ERAS, LUMI = [], {}
for a in sys.argv[4:]:
    e, _, l = a.partition(":")
    ERAS.append(e); LUMI[e] = float(l) if l else None
REF = ERAS[0]
R = "/eos/home-k/kakang/IPres/analysis/{}/json/{}/ip_res"


def load(era):
    files = glob.glob(f"{R.format(sample, era)}/fit_*.json")
    return {int(p.split("_")[-1].split(".")[0]): json.load(open(p)) for p in files}


D = {e: load(e) for e in ERAS}
SL = D[REF][0]["abseta_slices"]
for e in ERAS[1:]:
    if sorted(D[e]) != sorted(D[REF]) or D[e][0]["abseta_slices"] != SL or any(
            D[e][i].get(x) != D[REF][i].get(x) for i in D[REF] for x in ("pt", "eta", "phi")):
        sys.exit(f"[ERROR] {e} and {REF} have different binnings")
os.makedirs(outdir, exist_ok=True)
EVT = {"JetHT": "Multi-jet events", "ZeroBias": "Zero-bias events"}[sample]
M = "_3g" if model == "3g" else ""
QUANT = (("meas", M, "measured"), ("conv", M + "_conv", "PV error removed"))
VL = {"d0": r"$d_{xy}$", "dz": r"$d_{z}$"}
COL = ["black", "tab:red", "tab:blue", "tab:green", "tab:orange", "tab:purple", "tab:brown"]
RANGES = [("lopt", "0.1 < $p_T$ < 1 GeV"), ("hipt", "1 < $p_T$ < 3 GeV"), ("ulpt", "3 < $p_T$ < 10 GeV"),
          ("vhpt", "10 < $p_T$ < 15 GeV")]
XL = {"pt": r"Track $p_T$ [GeV]", "eta": r"Track $\eta$", "phi": r"Track $\phi$"}
hep.style.use(hep.style.CMS)


def series(era, var, cat, sfx, xkey):
    """x, y, err over the bins that have this binning key; y = nan where not fitted or not valid"""
    x, y, e = [], [], []
    for i in sorted(D[era]):
        d = D[era][i]
        k = f"reso_data_{var}_{cat}"
        if xkey not in d or k + sfx not in d:
            continue
        ok = d[f"fitstatus_data_{var}_{cat}"] != -2 and d[k + sfx] > 0
        x.append(d[xkey]); y.append(d[k + sfx] if ok else np.nan)
        e.append(max(d.get(k + sfx + "_err", 0), 0) if ok else 0)
    o = np.argsort(x)
    return np.array(x)[o], np.array(y)[o], np.array(e)[o]


def figure(var, xkey, cat, title, name):
    if not len(series(REF, var, cat, "", xkey)[0]):
        return
    for q, sfx, qlab in QUANT:
        fig, (ax, rx) = plt.subplots(2, 1, figsize=(8.5, 9), sharex=True,
                                     gridspec_kw={"height_ratios": [3, 1.2], "hspace": 0.05})
        x, y0, e0 = series(REF, var, cat, sfx, xkey)
        ymax, rats = 0, []
        for era, col in zip(ERAS, COL):
            _, y, e = series(era, var, cat, sfx, xkey)
            lab = era + (f" ({LUMI[era]:g} fb$^{{-1}}$)" if LUMI[era] else "")
            ax.errorbar(x, y, yerr=e, fmt="o", ms=4, color=col, label=lab)
            ymax = max(ymax, np.nanmax(y))
            if era != REF:
                r = y / y0
                re = np.where((e > 0) & (e0 > 0), r * np.sqrt((e / y) ** 2 + (e0 / y0) ** 2), 0)
                rx.errorbar(x, r, yerr=re, fmt="o", ms=3, color=col)
                rats.append(r)
        ax.set_ylim(0, ymax * 1.4)
        ax.set_ylabel(f"{VL[var]} resolution, {qlab} [$\\mu$m]", fontsize=20)
        ax.legend(title=f"{EVT}, {title}", fontsize=13, title_fontsize=14, ncol=2, loc="upper right")
        hep.cms.label(label="Preliminary", data=True, ax=ax, rlabel="(13.6 TeV)")
        a = np.concatenate(rats); a = a[np.isfinite(a)]
        rx.axhline(1, color="black", lw=1)
        rx.set_ylim(min(np.percentile(a, 1), 1) - 0.02, max(np.percentile(a, 99), 1) + 0.02)
        rx.set_ylabel(f"era / {REF}", fontsize=20); rx.set_xlabel(XL[xkey])
        fig.align_ylabels([ax, rx])
        fig.savefig(f"{outdir}/{name}_{var}_{q}_{model}.png", dpi=100, bbox_inches="tight")
        plt.close(fig)


CATS = []  # (xkey, category, title)
for s in range(len(SL) - 1):
    CATS.append(("pt", f"pt_eta{s}", f"{SL[s]:g} < $|\\eta|$ < {SL[s + 1]:g}"))
for xkey in ("eta", "phi"):
    for r, lab in RANGES:
        CATS.append((xkey, f"{xkey}_{r}", lab))
for var in ("d0", "dz"):
    for xkey, cat, title in CATS:
        figure(var, xkey, cat, title, f"vs_{xkey}_{cat.split('_', 1)[1]}")
print("written to", outdir, len(os.listdir(outdir)), "files")

print(f"\nmedian era/{REF} - 1 [%], {model}, PV error removed (conv):")
print(f"{'':16s}" + "".join(f"{e:>10s}" for e in ERAS[1:]))
for var in ("d0", "dz"):
    for xkey, cat, _ in CATS + [("pt", "pt_alleta", "")]:
        y0 = series(REF, var, cat, M + "_conv", xkey)[1]
        if not len(y0):
            continue
        cells = []
        for e in ERAS[1:]:
            r = (series(e, var, cat, M + "_conv", xkey)[1] / y0 - 1) * 100
            cells.append(f"{np.nanmedian(r):+10.1f}")
        print(f"{var} {cat:12s} " + "".join(cells))
