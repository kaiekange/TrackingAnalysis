import json
from pathlib import Path
import sys
import numpy as np
import matplotlib.pyplot as plt
import warnings

# ------------ global settings ------------

warnings.filterwarnings(
    "ignore",
    message="The value of the smallest subnormal for <class 'numpy.float64'> type is zero.",
    category=UserWarning,
)

# 5 个 era
periods = ["2022", "2022EE", "2023", "2023BPix", "2024"]

# 每个变量的 y 轴标题（LaTeX）
VAR_TITLES = {
    "reso_data_d0_pt_loeta":   r"$d_{xy}$ resolution ($|\eta|$<1.3) [$\mu$m]",
    "reso_data_d0_pt_hieta":   r"$d_{xy}$ resolution (1.3<$|\eta|$<2.5) [$\mu$m]",
    "reso_data_d0_pt_uleta":   r"$d_{xy}$ resolution (2.5<$|\eta|$<3.0) [$\mu$m]",
    "reso_data_d0_pt_alleta":  r"$d_{xy}$ resolution [$\mu$m]",

    "reso_data_dz_pt_loeta":   r"$d_{z}$ resolution ($|\eta|$<1.3) [$\mu$m]",
    "reso_data_dz_pt_hieta":   r"$d_{z}$ resolution (1.3<$|\eta|$<2.5) [$\mu$m]",
    "reso_data_dz_pt_uleta":   r"$d_{z}$ resolution (2.5<$|\eta|$<3.0) [$\mu$m]",
    "reso_data_dz_pt_alleta":  r"$d_{z}$ resolution [$\mu$m]",
}

# 8 个关心的变量
ALL_VARS = list(VAR_TITLES.keys())

# 目标 pt
TARGET_PTS = [0.1, 1.0, 3.0, 10.0]

# 区间 [low, high)
PT_BINS = [(0.1, 1.0), (1.0, 3.0), (3.0, 10.0)]
PT_BIN_LABELS = ["0.1-1", "1-3", "3-10"]

# JSON 路径模板
BASE_DIR_TEMPLATE = "/eos/home-k/kakang/IPres/analysis/JetHT/json/{period}/ip_res/"
# 输出目录
OUT_DIR = Path("/eos/home-k/kakang/IPres/analysis/JetHT/figures/ip_res")


# ============ 1. 读取数据 ============

def load_all_data():
    data = {}

    for period in periods:
        base_dir = Path(BASE_DIR_TEMPLATE.format(period=period))
        pts = []
        pt_uleta_list = []
        vals = {v: [] for v in ALL_VARS}

        for path in base_dir.glob("*.json"):
            d = json.loads(path.read_text())

            pt = d["pt"]
            pt_uleta = d.get("pt_uleta", pt)  # fallback

            pts.append(pt)
            pt_uleta_list.append(pt_uleta)

            for v in ALL_VARS:
                vals[v].append(d.get(v, float('nan')))

        if len(pts) == 0:
            print(f"[WARN] period {period}: no entries found.")
            period_data = {
                "pt": np.array([]),
                "pt_uleta": np.array([]),
            }
            for v in ALL_VARS:
                period_data[v] = np.array([])
        else:
            period_data = {
                "pt": np.array(pts, dtype=float),
                "pt_uleta": np.array(pt_uleta_list, dtype=float),
            }
            for v in ALL_VARS:
                period_data[v] = np.array(vals[v], dtype=float)

        data[period] = period_data

    return data


# ============ 2. 最近 pt 的值 + 画 2×2 柱状图 ============

def get_values_at_closest_pts(data, var):
    n_targets = len(TARGET_PTS)
    n_periods = len(periods)
    values = np.full((n_targets, n_periods), np.nan, dtype=float)

    use_uleta = "uleta" in var

    for j, period in enumerate(periods):
        pts = data[period]["pt_uleta"] if use_uleta else data[period]["pt"]

        if pts.size == 0:
            continue

        arr = data[period][var]

        for i, t in enumerate(TARGET_PTS):
            idx = np.argmin(np.abs(pts - t))
            values[i, j] = float(arr[idx])

    return values


def plot_closest_pt_bar(data, var, ytitle, outfile):
    """
    对某个 var，在 pt 最接近 0.1,1,3,10 时，画 2×2 柱状图（一个图文件），
    x 轴为 5 个 era。
    """
    values = get_values_at_closest_pts(data, var)  # shape (4, 5)
    eras_label = [p.replace("_", " ") for p in periods]
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
        "font.size": 11,
        "axes.linewidth": 1.2,
        "xtick.direction": "in",
        "ytick.direction": "in",
        "axes.grid": False,
    })

    fig, axes = plt.subplots(2, 2, figsize=(8.0, 5.0))
    axes = axes.flatten()

    for i, ax in enumerate(axes):
        if i >= len(TARGET_PTS):
            ax.axis("off")
            continue

        vals = values[i, :]
        finite_mask = np.isfinite(vals)
        x = np.arange(len(periods))

        if not np.any(finite_mask):
            ax.text(0.5, 0.5, "No data", ha="center", va="center")
            continue

        for j in range(len(periods)):
            if finite_mask[j]:
                ax.bar(x[j], vals[j], color=colors[j], width=0.35)

        ymax = float(np.nanmax(vals))
        margin = 0.2 * ymax
        ax.set_ylim(0, ymax + margin)

        for j in range(len(periods)):
            if finite_mask[j]:
                ax.text(
                    x[j],
                    vals[j] + 0.04 * ymax,
                    f"{vals[j]:.1f}",
                    ha="center",
                    va="bottom",
                    fontsize=8,
                )

        ax.set_xticks(x)
        ax.set_xticklabels(eras_label, rotation=25)
        ax.set_title(f"p$_{{T}}$ ≈ {TARGET_PTS[i]} GeV")

    # 全图 y 轴 label
    fig.text(0.02, 0.5, ytitle, va="center", rotation="vertical")

    # 顶部 CMS 标签
    fig.text(0.04, 0.96, "CMS", fontsize=16, fontweight="bold")
    fig.text(0.12, 0.96, "Preliminary", fontsize=12, style="italic")
    # 如需加 Run 信息可再加一行，例如：
    # fig.text(0.70, 0.96, "Run 3 (13.6 TeV)", fontsize=12)

    plt.tight_layout(rect=(0.05, 0.05, 1.0, 0.92))
    plt.savefig(outfile, dpi=300, bbox_inches="tight")
    plt.close()


# ============ 3. 区间 median / mean + 画 1×3 柱状图 ============

def get_bin_stats(data, var, stat="median"):
    n_bins = len(PT_BINS)
    n_periods = len(periods)
    stats_arr = np.full((n_bins, n_periods), np.nan, dtype=float)

    use_uleta = "uleta" in var

    for j, period in enumerate(periods):
        pts = data[period]["pt_uleta"] if use_uleta else data[period]["pt"]
        vals = data[period][var]

        if pts.size == 0:
            continue

        for i, (low, high) in enumerate(PT_BINS):
            mask = (pts >= low) & (pts < high)
            subset = vals[mask]

            if subset.size == 0:
                continue

            if stat == "median":
                stats_arr[i, j] = float(np.nanmedian(subset))
            elif stat == "mean":
                stats_arr[i, j] = float(np.nanmean(subset))
            else:
                raise ValueError("stat must be 'median' or 'mean'")

    return stats_arr


def plot_bin_stats_bar(data, var, ytitle, stat, outfile):
    """
    对某个 var，在 0.1–1, 1–3, 3–10 三个区间的 median/mean，
    画 1×3 柱状图（一个图文件）。
    """
    stats_arr = get_bin_stats(data, var, stat=stat)  # shape (3, 5)

    eras_label = [p.replace("_", " ") for p in periods]
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
        "font.size": 11,
        "axes.linewidth": 1.2,
        "xtick.direction": "in",
        "ytick.direction": "in",
        "axes.grid": False,
    })

    fig, axes = plt.subplots(1, 3, figsize=(10.0, 3.5), sharey=False)

    for i, ax in enumerate(axes):
        vals = stats_arr[i, :]
        finite_mask = np.isfinite(vals)
        x = np.arange(len(periods))

        if not np.any(finite_mask):
            ax.text(0.5, 0.5, "No data", ha="center", va="center")
            ax.set_title(f"p$_{{T}}$ ∈ ({PT_BINS[i][0]}, {PT_BINS[i][1]}) GeV")
            continue

        for j in range(len(periods)):
            if finite_mask[j]:
                ax.bar(x[j], vals[j], color=colors[j], width=0.35)

        ymax = float(np.nanmax(vals))
        margin = 0.2 * ymax
        ax.set_ylim(0, ymax + margin)

        for j in range(len(periods)):
            if finite_mask[j]:
                ax.text(
                    x[j],
                    vals[j] + 0.04 * ymax,
                    f"{vals[j]:.1f}",
                    ha="center",
                    va="bottom",
                    fontsize=8,
                )

        ax.set_xticks(x)
        ax.set_xticklabels(eras_label, rotation=25)
        ax.set_title(f"p$_{{T}}$ ∈ ({PT_BINS[i][0]}, {PT_BINS[i][1]}) GeV")

    # 全图 y 轴 label
    fig.text(0.02, 0.5, f"{ytitle} ({stat})", va="center", rotation="vertical")

    # 顶部 CMS 标签
    fig.text(0.04, 0.96, "CMS", fontsize=16, fontweight="bold")
    fig.text(0.12, 0.96, "Preliminary", fontsize=12, style="italic")

    plt.tight_layout(rect=(0.05, 0.05, 1.0, 0.92))
    plt.savefig(outfile, dpi=300, bbox_inches="tight")
    plt.close()


# ============ 4. 主入口：对 8 个变量全部画图 ============

def main():
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    data = load_all_data()

    for var in ALL_VARS:
        ytitle = VAR_TITLES[var]

        # 4 个 pt 点 (,1,3,10) 的 2×2 图
        plot_closest_pt_bar(
            data,
            var,
            ytitle=ytitle,
            outfile=str(OUT_DIR / f"pillar_{var}_closest.png"),
        )

        # 3 个 pt 区间的 median 图
        plot_bin_stats_bar(
            data,
            var,
            ytitle=ytitle,
            stat="median",
            outfile=str(OUT_DIR / f"pillar_{var}_median.png"),
        )

        # 3 个 pt 区间的 mean 图
        plot_bin_stats_bar(
            data,
            var,
            ytitle=ytitle,
            stat="mean",
            outfile=str(OUT_DIR / f"pillar_{var}_mean.png"),
        )

    return 0


if __name__ == "__main__":
    sys.exit(main())