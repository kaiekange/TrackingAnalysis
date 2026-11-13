"""Compare fit models for the IP histograms: DSCB (n <= 100), sum of 2 and 3 Gaussians (common mean), and the
model-free 68% half-width counted from the histogram (around the median, under/overflow in the total, linear
interpolation in the bins). Prints chi2/ndf (bins with >= 10 entries) and half-widths. Old-category 2022C fill.root.
Usage: python3 model_compare.py <fill.root> <hist> [<hist> ...]"""
import sys, numpy as np, ROOT
ROOT.gROOT.SetBatch(True); ROOT.RooMsgService.instance().setGlobalKillBelow(ROOT.RooFit.ERROR)
f = ROOT.TFile.Open(sys.argv[1])

def hw(pdf, x, m, tol=1e-4):
    x.setRange("nr", m - 1e6, m + 1e6); den = pdf.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("nr")).getVal()
    lo, hi = 0.0, x.getMax() - m
    while hi - lo > tol:
        mid = .5 * (lo + hi); x.setRange("ir", m - mid, m + mid)
        p = pdf.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("ir")).getVal() / den
        lo, hi = (mid, hi) if p < .68 else (lo, mid)
    return .5 * (lo + hi)

def chi2(pdf, x, h):
    N = h.Integral(); x.setRange("full", x.getMin(), x.getMax())
    tot = pdf.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("full")).getVal()
    c, k = 0.0, 0
    for b in range(1, h.GetNbinsX() + 1):
        o = h.GetBinContent(b)
        if o < 10: continue
        x.setRange("b", h.GetBinLowEdge(b), h.GetBinLowEdge(b + 1))
        e = N * pdf.createIntegral(ROOT.RooArgSet(x), ROOT.RooFit.Range("b")).getVal() / tot
        c += (o - e) ** 2 / o; k += 1
    return c, k

def counted(h):
    """68% half-width around the median, counted (all entries incl. under/overflow)"""
    nb = h.GetNbinsX(); tot = h.Integral(0, nb + 1)
    edges = np.array([h.GetBinLowEdge(b) for b in range(1, nb + 2)]); cont = np.array([h.GetBinContent(b) for b in range(1, nb + 1)])
    cum = h.GetBinContent(0) + np.concatenate([[0], np.cumsum(cont)])
    F = lambda v: np.interp(v, edges, cum)
    med = np.interp(0.5 * tot, cum, edges)
    lo, hi = 0.0, edges[-1] - med
    while hi - lo > 1e-5:
        mid = .5 * (lo + hi)
        lo, hi = (mid, hi) if F(med + mid) - F(med - mid) < 0.68 * tot else (lo, mid)
    return .5 * (lo + hi), (h.GetBinContent(0) + h.GetBinContent(nb + 1)) / tot

for name in sys.argv[2:]:
    h = f.Get(name)
    x = ROOT.RooRealVar("x", "x", h.GetXaxis().GetXmin(), h.GetXaxis().GetXmax()); x.setBins(h.GetNbinsX())
    d = ROOT.RooDataHist("d", "", ROOT.RooArgList(x), h)
    m, r = h.GetMean(), h.GetRMS()
    out = {}
    # DSCB
    mu = ROOT.RooRealVar("mu", "", m, m - r, m + r); s = ROOT.RooRealVar("s", "", .5 * r, .1 * r, r)
    a = ROOT.RooRealVar("a", "", 2, .5, 5); n = ROOT.RooRealVar("n", "", 2, .5, 100)
    cb = ROOT.RooCrystalBall("cb", "", x, mu, s, a, n, a, n)
    cb.fitTo(d, ROOT.RooFit.PrintLevel(-1)); out["DSCB"] = (hw(cb, x, mu.getVal()), *chi2(cb, x, h), 4)
    # sums of Gaussians, common mean, ordered widths
    keep = []
    for ng in (2, 3):
        mg = ROOT.RooRealVar(f"mg{ng}", "", m, m - r, m + r)
        sig = [ROOT.RooRealVar(f"s{ng}_{i}", "", r * (0.4, 1.0, 2.5)[i], 0.05 * r, 10 * r) for i in range(ng)]
        gs = [ROOT.RooGaussian(f"g{ng}_{i}", "", x, mg, sig[i]) for i in range(ng)]
        fr = [ROOT.RooRealVar(f"f{ng}_{i}", "", (0.6, 0.3)[i], 0.0, 1.0) for i in range(ng - 1)]
        pdf = ROOT.RooAddPdf(f"sum{ng}", "", ROOT.RooArgList(*gs), ROOT.RooArgList(*fr), True)
        # fractions refer to the full observable range, not to the sub-ranges integrated in hw() / chi2()
        pdf.fixCoefNormalization(ROOT.RooArgSet(x))
        keep += [mg, sig, gs, fr, pdf]
        res = pdf.fitTo(d, ROOT.RooFit.PrintLevel(-1), ROOT.RooFit.Save(True))
        out[f"{ng} Gauss"] = (hw(pdf, x, mg.getVal()), *chi2(pdf, x, h), 2 * ng)
        out[f"{ng} Gauss"] += (res.status(),)
    cw, ovf = counted(h)
    print(f"{name} (N {h.GetEntries():.0f}, outside histogram range {ovf:.1e}): counted half-width {cw:.3f}")
    for k, v in out.items():
        print(f"   {k:8s}: half-width {v[0]:8.3f} ({(v[0] / cw - 1) * 100:+.2f}% vs counted), chi2/ndf {v[1] / (v[2] - v[3]):6.2f}" + (f", status {v[4]}" if len(v) > 4 else ""))
    sys.stdout.flush()
