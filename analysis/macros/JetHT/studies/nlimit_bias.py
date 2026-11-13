"""Bias of the DSCB 68% half-width from the old upper limit n <= 10 (ip_res.cc before 2026-11-13):
fit the same histogram with n <= 10 and n <= 100 (plain DSCB fit as in ip_res.cc, half-width tolerance 1e-4).
Input: the old-category 2022C fill.root (snapshot). Writes <outdir>/nlimit_bias_<cat>.png and prints the numbers.
Usage: python3 nlimit_bias.py <fill.root> <ip_res json dir> <outdir> <category> <x key> <step>"""
import sys, json, numpy as np, ROOT
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep
ROOT.gROOT.SetBatch(True); ROOT.RooMsgService.instance().setGlobalKillBelow(ROOT.RooFit.ERROR)
fn, jdir, outdir, cat, xkey, step = sys.argv[1:7]
f = ROOT.TFile.Open(fn)

def hw(pdf, x, m, tol=1e-4):
    x.setRange("nr", m - 1e6, m + 1e6); den = pdf.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("nr")).getVal()
    lo, hi = 0.0, x.getMax() - m
    while hi - lo > tol:
        mid = .5 * (lo + hi); x.setRange("ir", m - mid, m + mid)
        p = pdf.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("ir")).getVal() / den
        lo, hi = (mid, hi) if p < .68 else (lo, mid)
    return .5 * (lo + hi)

def fit(h, nmax):
    x = ROOT.RooRealVar("x", "x", h.GetXaxis().GetXmin(), h.GetXaxis().GetXmax()); x.setBins(h.GetNbinsX())
    m, r = h.GetMean(), h.GetRMS()
    mu = ROOT.RooRealVar("mu", "", m, m - r, m + r); s = ROOT.RooRealVar("s", "", .5 * r, .1 * r, r)
    a = ROOT.RooRealVar("a", "", 2, .5, 5); n = ROOT.RooRealVar("n", "", 2, .5, nmax)
    cb = ROOT.RooCrystalBall("cb", "", x, mu, s, a, n, a, n)
    cb.fitTo(ROOT.RooDataHist("d", "", ROOT.RooArgList(x), h), ROOT.RooFit.PrintLevel(-1))
    return hw(cb, x, mu.getVal()), n.getVal()

res = {}
for v in ("d0", "dz"):
    pts = []
    for i in range(0, 200, int(step)):
        h = f.Get(f"ip_{v}_{i}_{cat}")
        if not h or h.GetEntries() < 2000:
            continue
        xv = json.load(open(f"{jdir}/fit_{i}.json"))[xkey]
        h10, n10 = fit(h, 10.0); h100, n100 = fit(h, 100.0)
        pts.append((xv, (h10 / h100 - 1) * 100, n10 > 9.99, n100))
    res[v] = pts
    b = np.array([p[1] for p in pts]); at = np.array([p[2] for p in pts])
    print(f"{cat} {v}: {len(pts)} bins, n at 10 in {at.sum()}; bias at limit: {b[at].min() if at.any() else 0:+.3f} to {b[at].max() if at.any() else 0:+.3f} %,"
          f" median {np.median(b[at]) if at.any() else 0:+.3f} %; not at limit: max |bias| {np.abs(b[~at]).max() if (~at).any() else 0:.4f} %", flush=True)

hep.style.use(hep.style.CMS)
fig, ax = plt.subplots(figsize=(8.5, 7))
for v, col, lab in (("d0", "tab:blue", r"$d_{xy}$"), ("dz", "tab:red", r"$d_{z}$")):
    p = res[v]
    xa = np.array([q[0] for q in p]); ya = np.array([q[1] for q in p]); at = np.array([q[2] for q in p], dtype=bool)
    ax.plot(xa[~at], ya[~at], "o", color=col, ms=5, label=f"{lab}, $n$ free")
    ax.plot(xa[at], ya[at], "o", mfc="none", color=col, ms=6, label=f"{lab}, $n$ at limit 10")
ax.axhline(0, color="gray", lw=1)
ax.set_xlabel(r"Track $p_T$ [GeV]" if xkey == "pt" else r"Track $\eta$")
ax.set_ylabel(r"$\sigma_{68}(n\leq10)\,/\,\sigma_{68}(n\leq100) - 1$ [%]")
title = {"pt_loeta": r"$|\eta|<1.3$", "eta_hipt": r"$1<p_T<3$ GeV", "eta_ulpt": r"$3<p_T<10$ GeV"}.get(cat, cat)
ax.legend(title=f"2022C, {title}", fontsize=16, title_fontsize=16)
hep.cms.label(label="Preliminary", data=True, com=13.6, lumi=5.02, ax=ax)
fig.savefig(f"{outdir}/nlimit_bias_{cat}.png", dpi=100, bbox_inches="tight")
