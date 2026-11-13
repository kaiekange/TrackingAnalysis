"""IP resolution of the sigma_PV groups vs the group's mean PV error s = sqrt(<sigma_PV^2>) (with pull), for a few
pT bins of one category: measured (open) and with the PV error removed by the convolution fit (filled). If the PV
error is removed correctly, the filled points are flat in s. 3-Gaussian fits; groups with fit status != 0 skipped.
Usage: python3 plot_vs_sigpv.py <period> <outdir> <lumi> <var> <cat> <bin> [<bin> ...]"""
import sys, json, numpy as np
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, mplhep as hep
period, outdir, lumi, var, cat = sys.argv[1], sys.argv[2], float(sys.argv[3]), sys.argv[4], sys.argv[5]
bins = [int(b) for b in sys.argv[6:]]
R = f"/eos/home-k/kakang/IPres/analysis/JetHT/json/{period}/ip_res"
hep.style.use(hep.style.CMS)
fig, ax = plt.subplots(figsize=(8.5, 7))
cols = plt.cm.turbo(np.linspace(0.1, 0.9, len(bins)))
ymax = 0
for i, col in zip(bins, cols):
    d = json.load(open(f"{R}/fit_{i}.json")); k = f"{var}_{cat}"
    s, meas, sub, err = [], [], [], []
    for g in range(d[f"ngroups_data_{k}"]):
        kg = f"{k}_g{g}"
        if d[f"fitstatus_3g_data_{kg}"] != 0 or d[f"fitstatus_3gconv_data_{kg}"] != 0 or d[f"reso_data_{kg}_3g_conv"] <= 0:
            continue
        s.append(d[f"pverr2_data_{kg}"] ** .5); meas.append(d[f"reso_data_{kg}_3g"]); sub.append(d[f"reso_data_{kg}_3g_conv"])
        err.append(max(d[f"reso_data_{kg}_3g_conv_err"], 0))
    ax.errorbar(s, sub, yerr=err, fmt="o", ms=5, color=col, label=f"$p_T$ = {d['pt']:.1f} GeV, PV error removed")
    ax.plot(s, meas, "o", ms=5, mfc="none", color=col, label=f"$p_T$ = {d['pt']:.1f} GeV, measured")
    ymax = max(ymax, max(meas))
    sl = d["abseta_slices"]
slab = f"{sl[int(cat[-1])]} < $|\\eta|$ < {sl[int(cat[-1]) + 1]}" if cat.startswith("pt_eta") else cat
ax.set_ylim(0, ymax * 1.45)
ax.set_xlabel(f"Mean PV error of the group, {'$d_{xy}$' if var == 'd0' else '$d_z$'} [$\\mu$m]")
ax.set_ylabel(f"{'$d_{xy}$' if var == 'd0' else '$d_z$'} resolution [$\\mu$m]")
ax.legend(title=f"{period}, {slab}, 3 Gaussians fit", fontsize=12, title_fontsize=13, ncol=2, loc="upper left")
hep.cms.label(label="Preliminary", data=True, com=13.6, lumi=lumi, ax=ax)
fig.savefig(f"{outdir}/vs_sigpv_{var}_{cat}.png", dpi=100, bbox_inches="tight")
print("wrote", f"{outdir}/vs_sigpv_{var}_{cat}.png")
