#!/usr/bin/env python3
"""Draw reso_data_d0_phi_allpt for era 2022 in polar coordinates (CMS style).
angle = phi, radius = d0 resolution (all pT, phi-scan bins).
"""

import json
import os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

STORAGE_DIR = "/eos/home-k/kakang/IPres/analysis/ZeroBias"
FIGURE_DIR  = STORAGE_DIR + "/figures/2022/ip_res"
PERIOD      = "2022"
NBINS       = 200

# ── CMS matplotlib style ───────────────────────────────────────────────────────
plt.rcParams.update({
    "font.family":       "sans-serif",
    "font.sans-serif":   ["Helvetica", "Arial", "DejaVu Sans"],
    "mathtext.fontset":  "custom",
    "mathtext.rm":       "Helvetica",
    "mathtext.it":       "Helvetica:italic",
    "mathtext.bf":       "Helvetica:bold",
    "axes.labelsize":    14,
    "xtick.labelsize":   12,
    "ytick.labelsize":   12,
    "xtick.direction":   "in",
    "ytick.direction":   "in",
    "xtick.major.size":  6,
    "ytick.major.size":  6,
    "xtick.minor.size":  3,
    "ytick.minor.size":  3,
    "xtick.top":         True,
    "ytick.right":       True,
    "figure.facecolor":  "white",
    "axes.facecolor":    "white",
    "axes.linewidth":    1.2,
})

# ── Load data ──────────────────────────────────────────────────────────────────
phi_vals  = np.empty(NBINS)
reso_vals = np.empty(NBINS)

for i in range(NBINS):
    with open(f"{STORAGE_DIR}/json/{PERIOD}/ip_res/fit_{i}.json") as f:
        d = json.load(f)
    phi_vals[i]  = d["phi"]
    reso_vals[i] = d["reso_data_d0_phi_allpt"]

mask = reso_vals > 0
phi_vals  = phi_vals[mask]
reso_vals = reso_vals[mask]

order     = np.argsort(phi_vals)
phi_vals  = phi_vals[order]
reso_vals = reso_vals[order]

# ── Plot ───────────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(subplot_kw={"projection": "polar"}, figsize=(7, 7))

ax.plot(phi_vals, reso_vals, color="#3182bd", lw=1.8, zorder=2)
ax.scatter(phi_vals, reso_vals, color="#3182bd", s=18, zorder=3, linewidths=0)

ax.set_theta_zero_location("E")
ax.set_theta_direction(1)

r_max = reso_vals.max() * 1.18
ax.set_ylim(0, r_max)
ax.set_rlabel_position(22.5)

# Radial tick labels
r_ticks = np.arange(0, r_max, 50)
ax.set_rticks(r_ticks)
ax.set_yticklabels([f"{int(r)}" for r in r_ticks], fontsize=10, color="gray")

# Azimuthal labels — 4 divisions (every 90°) to keep the grid sparse
ax.set_thetagrids(np.arange(0, 360, 90), fontsize=11)

ax.tick_params(pad=4)
ax.grid(color="gray", linestyle="--", linewidth=0.5, alpha=0.6)
ax.spines["polar"].set_linewidth(1.2)

# ── CMS labels (placed in figure coordinates above the polar axes) ─────────────
# "CMS Preliminary" on one line: CMS bold, Preliminary italic, both at same y
fig.text(0.12, 0.96, "CMS", fontsize=13, fontweight="bold",
         ha="left", va="top", transform=fig.transFigure)
fig.text(0.21, 0.96, "Preliminary", fontsize=10, fontstyle="italic",
         ha="left", va="top", transform=fig.transFigure)
# Lumi + energy
fig.text(0.88, 0.96, "Run 3 (13.6 TeV)", fontsize=10,
         ha="right", va="top", transform=fig.transFigure)

# Radial axis label
ax.text(np.deg2rad(22.5), r_max * 1.12,
        r"$\sigma(d_{xy})\ [\mu\mathrm{m}]$",
        ha="center", va="bottom", fontsize=11, color="gray")

# Dataset label inside plot
ax.text(np.deg2rad(270), r_max * 0.55,
        "Zero-bias events, all $p_T$",
        ha="center", va="center", fontsize=10, color="#444444")

os.makedirs(FIGURE_DIR, exist_ok=True)
out_path = f"{FIGURE_DIR}/polar_d0_phi_allpt_2022.png"
fig.savefig(out_path, dpi=150, bbox_inches="tight")
print(f"Saved: {out_path}")
plt.close(fig)
