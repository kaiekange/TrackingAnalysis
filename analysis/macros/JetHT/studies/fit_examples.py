"""Single IP histograms of fill.root with the two fit models of ip_res.cc: DSCB (n <= 100) and sum of 3 Gaussians,
plus (data - fit)/error per bin. Prints the parameters, chi2/ndf and half-widths (and the counted one from the json).
Usage: python3 fit_examples.py <period> <outdir> <lumi> <var>:<cat>:<bin> [...]"""
import sys, json, numpy as np, ROOT
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep
ROOT.gROOT.SetBatch(True); ROOT.RooMsgService.instance().setGlobalKillBelow(ROOT.RooFit.ERROR)
period, outdir, lumi = sys.argv[1], sys.argv[2], float(sys.argv[3])
S = f"/eos/home-k/kakang/IPres/analysis/JetHT"
f = ROOT.TFile.Open(f"{S}/hists/{period}/fill.root")

def hw(pdf, x, m, tol=1e-4):
    x.setRange("nr", m - 1e6, m + 1e6); den = pdf.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("nr")).getVal()
    lo, hi = 0.0, x.getMax() - m
    while hi - lo > tol:
        mid = .5 * (lo + hi); x.setRange("ir", m - mid, m + mid)
        p = pdf.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("ir")).getVal() / den
        lo, hi = (mid, hi) if p < .68 else (lo, mid)
    return .5 * (lo + hi)

def expected(pdf, x, h):
    N = h.Integral(); x.setRange("full", x.getMin(), x.getMax())
    tot = pdf.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("full")).getVal()
    out = []
    for b in range(1, h.GetNbinsX() + 1):
        x.setRange("b", h.GetBinLowEdge(b), h.GetBinLowEdge(b + 1))
        out.append(N * pdf.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("b")).getVal() / tot)
    return np.array(out)

hep.style.use(hep.style.CMS)
for spec in sys.argv[4:]:
    var, cat, i = spec.split(":"); i = int(i)
    d = json.load(open(f"{S}/json/{period}/ip_res/fit_{i}.json"))
    h = f.Get(f"ip_{var}_{i}_{cat}")
    x = ROOT.RooRealVar("x", "x", h.GetXaxis().GetXmin(), h.GetXaxis().GetXmax()); x.setBins(h.GetNbinsX())
    data = ROOT.RooDataHist("d", "", ROOT.RooArgList(x), h); m, r = h.GetMean(), h.GetRMS()
    mu = ROOT.RooRealVar("mu", "", m, m - r, m + r); s = ROOT.RooRealVar("s", "", .5 * r, .1 * r, r)
    a = ROOT.RooRealVar("a", "", 2, .5, 5); n = ROOT.RooRealVar("n", "", 2, .5, 100)
    cb = ROOT.RooCrystalBall("cb", "", x, mu, s, a, n, a, n); cb.fitTo(data, ROOT.RooFit.PrintLevel(-1))
    mg = ROOT.RooRealVar("mg", "", m, m - r, m + r)
    sig = [ROOT.RooRealVar(f"s{k}", "", r * (0.4, 1.0, 2.5)[k], 0.05 * r, 10 * r) for k in range(3)]
    gs = [ROOT.RooGaussian(f"g{k}", "", x, mg, sig[k]) for k in range(3)]
    fr = [ROOT.RooRealVar(f"f{k}", "", (0.6, 0.3)[k], 0.0, 1.0) for k in range(2)]
    g3 = ROOT.RooAddPdf("g3", "", ROOT.RooArgList(*gs), ROOT.RooArgList(*fr), True); g3.fixCoefNormalization(ROOT.RooArgSet(x))
    g3.fitTo(data, ROOT.RooFit.PrintLevel(-1))
    c = np.array([h.GetBinCenter(b) for b in range(1, h.GetNbinsX() + 1)])
    obs = np.array([h.GetBinContent(b) for b in range(1, h.GetNbinsX() + 1)]); err = np.sqrt(np.maximum(obs, 1))
    e_cb, e_g3 = expected(cb, x, h), expected(g3, x, h)
    ok = obs >= 10
    chi = lambda e, npar: np.sum(((obs - e) ** 2 / obs)[ok]) / (ok.sum() - npar)
    hcb, hg3 = hw(cb, x, mu.getVal()), hw(g3, x, mg.getVal())
    cnt = d[f"reso_data_{var}_{cat}_counted"]
    print(f"{var} {cat} bin {i} (pT {d['pt']:.2f} GeV, N {h.GetEntries():.0f}): counted {cnt:.2f} um | DSCB {hcb:.2f} ({(hcb/cnt-1)*100:+.2f}%), "
          f"alpha {a.getVal():.3f} n {n.getVal():.1f}, chi2/ndf {chi(e_cb, 4):.2f} | 3G {hg3:.2f} ({(hg3/cnt-1)*100:+.2f}%), "
          f"widths {', '.join(f'{v.getVal():.1f}' for v in sig)}, chi2/ndf {chi(e_g3, 6):.2f}", flush=True)
    fig, (ax, ax2) = plt.subplots(2, 1, figsize=(8.5, 9), sharex=True, gridspec_kw=dict(height_ratios=(3, 1), hspace=0.05))
    ax.errorbar(c, obs, yerr=err, fmt="o", ms=3, color="black", label="Data")
    ax.plot(c, e_cb, color="tab:red", lw=2, label=rf"DSCB: $n$ = {n.getVal():.0f}, $\sigma_{{68}}$ = {hcb:.1f} $\mu$m")
    ax.plot(c, e_g3, color="tab:blue", lw=2, ls="--", label=rf"3 Gaussians: $\sigma_{{68}}$ = {hg3:.1f} $\mu$m")
    ax.set_ylabel("Tracks / bin"); ax.set_ylim(0, obs.max() * 1.5)
    sl = d["abseta_slices"]
    etal = {f"pt_eta{k}": f"{sl[k]} < $|\\eta|$ < {sl[k+1]}" for k in range(9)}.get(cat, cat)
    ax.legend(title=f"{period}, $p_T$ = {d['pt']:.2f} GeV, {etal}\ncounted $\\sigma_{{68}}$ = {cnt:.1f} $\\mu$m", fontsize=14, title_fontsize=14, loc="upper left")
    for e, col, ls in ((e_cb, "tab:red", "-"), (e_g3, "tab:blue", "--")):
        ax2.plot(c, (obs - e) / err, color=col, ls=ls, lw=1.5, drawstyle="steps-mid")
    ax2.axhline(0, color="gray", lw=1); ax2.set_ylim(-6, 6); ax2.set_ylabel("(Data-fit)/err", fontsize=20)
    ax2.set_xlabel(r"Track $d_{z}$ [$\mu$m]" if var == "dz" else r"Track $d_{xy}$ [$\mu$m]")
    hep.cms.label(label="Preliminary", data=True, com=13.6, lumi=lumi, ax=ax)
    fig.savefig(f"{outdir}/fit_{var}_{cat}_{i}.png", dpi=100, bbox_inches="tight"); plt.close(fig)
