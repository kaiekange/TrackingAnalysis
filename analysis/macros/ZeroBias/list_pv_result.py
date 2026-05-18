import json
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
import sys
import warnings
warnings.filterwarnings(
    "ignore",
    message="The value of the smallest subnormal for <class 'numpy.float64'> type is zero.",
    category=UserWarning
)

periods = ["2022", "2022EE", "2023", "2023BPix", "2024"]
keys = ["reso_data_pvx", "reso_data_pvy", "reso_data_pvz"]

def getminmax():

    results = {
        period: {
            k: {"min": float("inf"), "max": float("-inf")}
            for k in keys
        }
        for period in periods
    }

    for period in periods:
        paths = Path(
            f"/eos/home-k/kakang/IPres/analysis/ZeroBias/json/{period}/pv_res/"
        ).glob("*.json")

        for path in paths:
            d = json.loads(path.read_text())

            for k in keys:
                v = d.get(k, float('nan'))
                if not np.isnan(v):
                    if v < results[period][k]["min"]:
                        results[period][k]["min"] = v
                    if v > results[period][k]["max"]:
                        results[period][k]["max"] = v

    return results

def plot_cms_style(stats, key, title, outfile):

    # ---- 收集原始 era 和 min/max ----
    eras_raw = list(stats.keys())
    min_vals_raw = [stats[e][key]["min"] for e in eras_raw]
    max_vals_raw = [stats[e][key]["max"] for e in eras_raw]

    # ---- 过滤掉没有有效数据的 era ----
    eras = []
    min_vals = []
    max_vals = []

    for e, vmin, vmax in zip(eras_raw, min_vals_raw, max_vals_raw):
        if np.isfinite(vmin) and np.isfinite(vmax):
            eras.append(e.replace("_", " "))
            min_vals.append(vmin)
            max_vals.append(vmax)
        else:
            print(f"[WARN] Skip period {e} for {key} (min/max not finite)")

    if len(eras) == 0:
        raise RuntimeError(f"No finite min/max values found for key {key}.")

    # ---- 分别计算 min / max 的 y 轴范围 ----
    min_vals_arr = np.array(min_vals, dtype=float)
    max_vals_arr = np.array(max_vals, dtype=float)

    ymax_min = float(np.max(min_vals_arr))
    ymax_max = float(np.max(max_vals_arr))

    ylim_top_min = ymax_min * 1.28
    ylim_top_max = ymax_max * 1.28

    # colors = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd"]
    colors = [
        "#000000",  # kBlack
        "#ff0000",  # kRed
        "#0000ff",  # kBlue
        "#009900",  # kGreen + 2
        "#ff9900",  # kOrange + 7
    ]

    plt.rcParams.update({
        "font.family": "serif",
        "font.size": 12,
        "axes.linewidth": 1.2,
        "xtick.direction": "in",
        "ytick.direction": "in",
        "axes.grid": False,
    })

    x = np.arange(len(eras))

    fig, axes = plt.subplots(
        1, 2,
        figsize=(8.5, 3.2),
        sharey=False,                 # 不共享 y 轴
        gridspec_kw={"wspace": 0.12}
    )

    # ---------------- left: min ----------------
    ax = axes[0]
    for i, val in enumerate(min_vals):
        ax.bar(x[i], val, color=colors[i % len(colors)], width=0.38)
        ax.text(
            x[i],
            val + 0.025 * ymax_min,
            f"{val:.3f}",
            ha="center",
            va="bottom",
            fontsize=9,
        )

    ax.set_xticks(x)
    ax.set_xticklabels(eras, rotation=25)
    ax.set_title("Min value")
    ax.set_ylabel(title)
    ax.set_ylim(0, ylim_top_min)

    ax.text(0.02, 0.96, "CMS", transform=ax.transAxes,
            fontsize=15, fontweight="bold", va="top")
    ax.text(0.02, 0.88, "Preliminary", transform=ax.transAxes,
            fontsize=11, style="italic", va="top")

    # ---------------- right: max ----------------
    ax = axes[1]
    for i, val in enumerate(max_vals):
        ax.bar(x[i], val, color=colors[i % len(colors)], width=0.38)
        ax.text(
            x[i],
            val + 0.025 * ymax_max,
            f"{val:.3f}",
            ha="center",
            va="bottom",
            fontsize=9,
        )

    ax.set_xticks(x)
    ax.set_xticklabels(eras, rotation=25)
    ax.set_title("Max value")
    ax.set_ylim(0, ylim_top_max)

    ax.text(0.02, 0.96, "CMS", transform=ax.transAxes,
            fontsize=15, fontweight="bold", va="top")
    ax.text(0.02, 0.88, "Preliminary", transform=ax.transAxes,
            fontsize=11, style="italic", va="top")

    plt.tight_layout()
    plt.savefig(outfile, dpi=300, bbox_inches="tight")

    plt.close()


def list_result():
    stats = getminmax()
    # LaTeX 的 μm
    plot_cms_style(stats, "reso_data_pvx", r"PV resolution in x [$\mu$m]", "/eos/home-k/kakang/IPres/analysis/ZeroBias/figures/pv_res/pillar_pvx.png")
    plot_cms_style(stats, "reso_data_pvy", r"PV resolution in y [$\mu$m]", "/eos/home-k/kakang/IPres/analysis/ZeroBias/figures/pv_res/pillar_pvy.png")
    plot_cms_style(stats, "reso_data_pvz", r"PV resolution in z [$\mu$m]", "/eos/home-k/kakang/IPres/analysis/ZeroBias/figures/pv_res/pillar_pvz.png")

if __name__ == "__main__":
    sys.exit(list_result())
