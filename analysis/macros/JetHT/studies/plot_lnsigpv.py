"""ln(sigma_PV) distributions (no pull) per |eta| slice, all pT bins of the pt_eta<s> categories, from a range-step
output (fill_hists.cc, log bins). Prints mean, std, skewness of ln(sigma_PV) per slice.
Usage: python3 plot_lnsigpv.py <range.root> <outdir> <label>"""
import sys, math, numpy as np, ROOT
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep

fn, outdir, label = sys.argv[1:4]
N_IP, SLICES = 14, [0.0, 0.8, 1.1, 1.3, 1.5, 1.7, 1.9, 2.1, 2.3, 2.5]
SHOW, REBIN = (0, 4, 8), 5
f = ROOT.TFile.Open(fn)
hep.style.use(hep.style.CMS)
for v, vl in (("d0", r"$\sigma_{PV,d_{xy}}$"), ("dz", r"$\sigma_{PV,d_{z}}$")):
    h = f.Get(f"sig_{v}")
    ny, ax = h.GetNbinsY(), h.GetYaxis()
    lnedge = np.log([ax.GetBinLowEdge(j) for j in range(1, ny + 2)])
    fig, a = plt.subplots(figsize=(8.5, 7))
    for s in range(9):
        w = np.zeros(ny)
        for xb in range(s + 1, h.GetNbinsX() + 1, N_IP):
            w += np.array([h.GetBinContent(xb, j) for j in range(1, ny + 1)])
        c = 0.5 * (lnedge[:-1] + lnedge[1:])
        m = np.average(c, weights=w); sd = math.sqrt(np.average((c - m) ** 2, weights=w))
        sk = np.average((c - m) ** 3, weights=w) / sd ** 3
        print(f"{v} |eta| {SLICES[s]}-{SLICES[s+1]}: N {w.sum():.0f}, ln mean {m:.3f} (sigma_PV {math.exp(m):.1f} um), "
              f"std {sd:.3f}, skewness {sk:+.2f}")
        if s not in SHOW:
            continue
        wr = w[: ny // REBIN * REBIN].reshape(-1, REBIN).sum(1)
        er = lnedge[: ny // REBIN * REBIN + 1 : REBIN]
        cr, dx = 0.5 * (er[:-1] + er[1:]), er[1:] - er[:-1]
        n = wr.sum()
        a.errorbar(cr, wr / n / dx, yerr=np.sqrt(wr) / n / dx, fmt="o", ms=3,
                   label=rf"{SLICES[s]} < $|\eta|$ < {SLICES[s+1]}")
    a.set_xlim(0, 5)
    a.set_ylim(0, a.get_ylim()[1] * 1.45)
    a.set_xlabel(rf"ln({vl} / $\mu$m)")
    a.set_ylabel("Fraction of tracks / unit ln")
    a.legend(title=f"{label}, 1 < $p_T$ < 15 GeV", fontsize=18, title_fontsize=18)
    hep.cms.label(label="Preliminary", data=True, com=13.6, ax=a, lumi=None)
    fig.savefig(f"{outdir}/lnsigpv_{v}.png", dpi=100, bbox_inches="tight")
    plt.close(fig)
