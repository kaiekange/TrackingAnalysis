#!/usr/bin/env python3

import sys
import json

def list_ip_res(period):

    # Load JSON files
    records = []
    for i in range(500):
        fname = f"/eos/home-k/kakang/IPres/analysis/JetHT/json/{period}/ip_res/fit_{i}.json"
        with open(fname) as f:
            rec = json.load(f)
            rec["_filename"] = fname
            records.append(rec)

    # Helpers
    def find_closest(distance_fn):
        best = None
        best_dist = None
        for rec in records:
            d = distance_fn(rec)
            if best is None or d < best_dist:
                best = rec
                best_dist = d
        return best, best_dist

    def fmt_reso(x):
        # resolution values: no digit decimal
        return f"{x:.0f}"

    # ------------------------------------------------------------------
    # PT selections (bins closest to pt = 0.1, 1, 3, 10)
    # ------------------------------------------------------------------
    rec_pt01, dist_pt01 = find_closest(lambda r: abs(r["pt"] - 0.1))
    rec_pt1,  dist_pt1  = find_closest(lambda r: abs(r["pt"] - 1.0))
    rec_pt3,  dist_pt3  = find_closest(lambda r: abs(r["pt"] - 3.0))
    rec_pt10, dist_pt10 = find_closest(lambda r: abs(r["pt"] - 10.0))

    # Collect values for each region and pt
    def get_pt_resos(rec):
        return {
            "loeta_d0":  rec["reso_data_d0_pt_loeta"],
            "hieta_d0":  rec["reso_data_d0_pt_hieta"],
            "uleta_d0":  rec["reso_data_d0_pt_uleta"],
            "alleta_d0": rec["reso_data_d0_pt_alleta"],

            "loeta_dz":  rec["reso_data_dz_pt_loeta"],
            "hieta_dz":  rec["reso_data_dz_pt_hieta"],
            "uleta_dz":  rec["reso_data_dz_pt_uleta"],
            "alleta_dz": rec["reso_data_dz_pt_alleta"],
        }

    pt_vals = {
        0.1: get_pt_resos(rec_pt01),
        1.0: get_pt_resos(rec_pt1),
        3.0: get_pt_resos(rec_pt3),
        10.0: get_pt_resos(rec_pt10),
    }

    def f(x):
        return fmt_reso(x)

    # Show which bins were chosen for each pt reference
    print("=" * 80)
    print("PT reference points (closest bins)")
    print(f"target pt = 0.1  -> chosen pt = {rec_pt01['pt']}, file = {rec_pt01['_filename']}, |Δpt| = {dist_pt01}")
    print(f"target pt = 1.0  -> chosen pt = {rec_pt1['pt']}, file = {rec_pt1['_filename']}, |Δpt| = {dist_pt1}")
    print(f"target pt = 3.0  -> chosen pt = {rec_pt3['pt']}, file = {rec_pt3['_filename']}, |Δpt| = {dist_pt3}")
    print(f"target pt = 10.0 -> chosen pt = {rec_pt10['pt']}, file = {rec_pt10['_filename']}, |Δpt| = {dist_pt10}")

    # Main PT output in requested form
    print("=" * 80)
    print("PT VALUES (data only) AND RANGES  (format: low→high : high ~ low)")
    regions = ["loeta", "hieta", "uleta", "alleta"]

    for reg in regions:
        # d0
        print("\n" + reg + " d0:")
        print(f"  pt=0.1 : {f(pt_vals[0.1][f'{reg}_d0'])}")
        print(f"  pt=1   : {f(pt_vals[1.0][f'{reg}_d0'])}")
        print(f"    0.1 → 1  : {f(pt_vals[1.0][f'{reg}_d0'])} ~ {f(pt_vals[0.1][f'{reg}_d0'])}")

        print(f"  pt=3   : {f(pt_vals[3.0][f'{reg}_d0'])}")
        print(f"    1   → 3  : {f(pt_vals[3.0][f'{reg}_d0'])} ~ {f(pt_vals[1.0][f'{reg}_d0'])}")

        print(f"  pt=10  : {f(pt_vals[10.0][f'{reg}_d0'])}")
        print(f"    3   →10 : {f(pt_vals[10.0][f'{reg}_d0'])} ~ {f(pt_vals[3.0][f'{reg}_d0'])}")

        # dz
        print("\n" + reg + " dz:")
        print(f"  pt=0.1 : {f(pt_vals[0.1][f'{reg}_dz'])}")
        print(f"  pt=1   : {f(pt_vals[1.0][f'{reg}_dz'])}")
        print(f"    0.1 → 1  : {f(pt_vals[1.0][f'{reg}_dz'])} ~ {f(pt_vals[0.1][f'{reg}_dz'])}")

        print(f"  pt=3   : {f(pt_vals[3.0][f'{reg}_dz'])}")
        print(f"    1   → 3  : {f(pt_vals[3.0][f'{reg}_dz'])} ~ {f(pt_vals[1.0][f'{reg}_dz'])}")

        print(f"  pt=10  : {f(pt_vals[10.0][f'{reg}_dz'])}")
        print(f"    3   →10 : {f(pt_vals[10.0][f'{reg}_dz'])} ~ {f(pt_vals[3.0][f'{reg}_dz'])}")

    # ------------------------------------------------------------------
    # ETA selections (|eta| = 0, 1.3, 2.5, 3.0), data only
    # ------------------------------------------------------------------
    rec_eta0,  dist_eta0  = find_closest(lambda r: abs(abs(r["eta"]) - 0.0))
    rec_eta13, dist_eta13 = find_closest(lambda r: abs(abs(r["eta"]) - 1.3))
    rec_eta25, dist_eta25 = find_closest(lambda r: abs(abs(r["eta"]) - 2.5))
    rec_eta3,  dist_eta3  = find_closest(lambda r: abs(abs(r["eta"]) - 3.0))

    def print_eta_block(label, target_abs_eta, rec, dist):
        print("=" * 80)
        print(label)
        print(f"file: {rec['_filename']}")
        # eta printed as true recorded value
        print(
            f"eta = {rec['eta']}, |eta| = {abs(rec['eta'])}  "
            f"(target |eta| = {target_abs_eta}, | |eta| - target | = {dist})"
        )

        d0_val = rec["reso_data_d0_eta_allpt"]
        dz_val = rec["reso_data_dz_eta_allpt"]

        print("  d0_eta_allpt (data only):")
        print(f"    reso_data_d0_eta_allpt: {fmt_reso(d0_val)}")
        print("  dz_eta_allpt (data only):")
        print(f"    reso_data_dz_eta_allpt: {fmt_reso(dz_val)}")

    print_eta_block("|eta| closest to 0.0", 0.0, rec_eta0,  dist_eta0)
    print_eta_block("|eta| closest to 1.3", 1.3, rec_eta13, dist_eta13)
    print_eta_block("|eta| closest to 2.5", 2.5, rec_eta25, dist_eta25)
    print_eta_block("|eta| closest to 3.0", 3.0, rec_eta3,  dist_eta3)

    # ------------------------------------------------------------------
    # Highest d0_eta_allpt and dz_eta_allpt among ALL etas
    # ------------------------------------------------------------------
    best_d0_rec = max(records, key=lambda r: r["reso_data_d0_eta_allpt"])
    best_dz_rec = max(records, key=lambda r: r["reso_data_dz_eta_allpt"])

    best_d0_val = best_d0_rec["reso_data_d0_eta_allpt"]
    best_dz_val = best_dz_rec["reso_data_dz_eta_allpt"]

    print("=" * 80)
    print("HIGHEST reso_data_d0_eta_allpt among ALL eta bins:")
    print(f"  value      : {fmt_reso(best_d0_val)}")
    print(f"  eta        : {best_d0_rec['eta']}")
    print(f"  |eta|      : {abs(best_d0_rec['eta'])}")
    print(f"  file       : {best_d0_rec['_filename']}")

    print("=" * 80)
    print("HIGHEST reso_data_dz_eta_allpt among ALL eta bins:")
    print(f"  value      : {fmt_reso(best_dz_val)}")
    print(f"  eta        : {best_dz_rec['eta']}")
    print(f"  |eta|      : {abs(best_dz_rec['eta'])}")
    print(f"  file       : {best_dz_rec['_filename']}")

    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python list_ip_res.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    sys.exit(list_ip_res(sys.argv[1]))