"""Fits of single IP histograms with the DSCB n <= 10 (old ip_res.cc) and n <= 100: data, both fits, and
(data - fit)/error per bin. Old-category 2022C fill.root (snapshot). For a category and an x window, fits all bins
in the window and draws the one with the largest |half-width difference|.
Usage: python3 nlimit_fits.py <fill.root> <json dir> <outdir> <var> <cat> <xkey> <xmin> <xmax>"""
import sys, json, numpy as np, ROOT
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep
ROOT.gROOT.SetBatch(True); ROOT.RooMsgService.instance().setGlobalKillBelow(ROOT.RooFit.ERROR)
fn, jdir, outdir, var, cat, xkey = sys.argv[1:7]; xmin, xmax = float(sys.argv[7]), float(sys.argv[8])
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
    """returns half-width, parameters, expected counts per histogram bin, chi2/ndf (bins with >= 10 entries)"""
    x = ROOT.RooRealVar("x", "x", h.GetXaxis().GetXmin(), h.GetXaxis().GetXmax()); x.setBins(h.GetNbinsX())
    m, r = h.GetMean(), h.GetRMS()
    mu = ROOT.RooRealVar("mu", "", m, m - r, m + r); s = ROOT.RooRealVar("s", "", .5 * r, .1 * r, r)
    a = ROOT.RooRealVar("a", "", 2, .5, 5); n = ROOT.RooRealVar("n", "", 2, .5, nmax)
    cb = ROOT.RooCrystalBall("cb", "", x, mu, s, a, n, a, n)
    cb.fitTo(ROOT.RooDataHist("d", "", ROOT.RooArgList(x), h), ROOT.RooFit.PrintLevel(-1))
    N = h.Integral()
    x.setRange("full", x.getMin(), x.getMax())
    tot = cb.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("full")).getVal()
    exp = []
    for b in range(1, h.GetNbinsX() + 1):
        x.setRange("b", h.GetBinLowEdge(b), h.GetBinLowEdge(b + 1))
        exp.append(N * cb.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("b")).getVal() / tot)
    exp = np.array(exp); obs = np.array([h.GetBinContent(b) for b in range(1, h.GetNbinsX() + 1)])
    ok = obs >= 10
    chi2 = np.sum((obs[ok] - exp[ok]) ** 2 / obs[ok])
    return dict(hw=hw(cb, x, mu.getVal()), mu=mu.getVal(), sigma=s.getVal(), alpha=a.getVal(), n=n.getVal(),
                exp=exp, chi2ndf=chi2 / (ok.sum() - 4))

best = None
for i in range(200):
    d = json.load(open(f"{jdir}/fit_{i}.json"))
    if not (xmin <= d[xkey] <= xmax):
        continue
    h = f.Get(f"ip_{var}_{i}_{cat}")
    r10, r100 = fit(h, 10.0), fit(h, 100.0)
    bias = r10["hw"] / r100["hw"] - 1
    if best is None or abs(bias) > abs(best[1]):
        best = (i, bias, r10, r100, d[xkey], h)
i, bias, r10, r100, xv, h = best
for lab, r in (("n <= 10 ", r10), ("n <= 100", r100)):
    print(f"{var} {cat} bin {i} ({xkey} = {xv:.3f}), N {h.GetEntries():.0f}: {lab}: half-width {r['hw']:.3f} um, sigma {r['sigma']:.3f},"
          f" alpha {r['alpha']:.3f}, n {r['n']:.2f}, chi2/ndf {r['chi2ndf']:.2f}")
print(f"   half-width (n<=10)/(n<=100) - 1 = {bias * 100:+.2f} %")

c = np.array([h.GetBinCenter(b) for b in range(1, h.GetNbinsX() + 1)])
obs = np.array([h.GetBinContent(b) for b in range(1, h.GetNbinsX() + 1)]); err = np.sqrt(np.maximum(obs, 1))
hep.style.use(hep.style.CMS)
fig, (ax, ax2) = plt.subplots(2, 1, figsize=(8.5, 9), sharex=True, gridspec_kw=dict(height_ratios=(3, 1), hspace=0.05))
ax.errorbar(c, obs, yerr=err, fmt="o", ms=3, color="black", label="Data")
ax.plot(c, r10["exp"], color="tab:red", lw=2, label=rf"DSCB, $n\leq10$: $n$ = {r10['n']:.1f}, $\sigma_{{68}}$ = {r10['hw']:.2f} $\mu$m")
ax.plot(c, r100["exp"], color="tab:blue", lw=2, ls="--", label=rf"DSCB, $n\leq100$: $n$ = {r100['n']:.1f}, $\sigma_{{68}}$ = {r100['hw']:.2f} $\mu$m")
ax.set_ylabel("Tracks / bin"); ax.set_ylim(0, obs.max() * 1.45)
lab = {"pt_loeta": rf"$p_T$ = {xv:.2f} GeV, $|\eta|<1.3$", "eta_hipt": rf"$\eta$ = {xv:.2f}, $1<p_T<3$ GeV"}[cat]
ax.legend(title=f"2022C, {lab}", fontsize=14, title_fontsize=15, loc="upper left")
for r, col, ls in ((r10, "tab:red", "-"), (r100, "tab:blue", "--")):
    ax2.plot(c, (obs - r["exp"]) / err, color=col, ls=ls, lw=1.5, drawstyle="steps-mid")
ax2.axhline(0, color="gray", lw=1); ax2.set_ylim(-6, 6); ax2.set_ylabel("(Data-fit)/err", fontsize=20)
ax2.set_xlabel(r"Track $d_{z}$ [$\mu$m]" if var == "dz" else r"Track $d_{xy}$ [$\mu$m]")
hep.cms.label(label="Preliminary", data=True, com=13.6, lumi=5.02, ax=ax)
fig.savefig(f"{outdir}/nlimit_fit_{var}_{cat}_{i}.png", dpi=100, bbox_inches="tight")
print(f"   -> {outdir}/nlimit_fit_{var}_{cat}_{i}.png")
