#!/usr/bin/env python3
import ROOT
import os
import sys

if len(sys.argv) != 2:
    print("Usage: python3 clean_info.py /path/to/directory")
    sys.exit(1)

dirpath = sys.argv[1]


for fname in os.listdir(dirpath):
    if not fname.startswith("skimmed_") or not fname.endswith(".root"):
        continue

    fpath = os.path.join(dirpath, fname)
    # print("Checking:", fpath)

    f = ROOT.TFile.Open(fpath)
    if not f or f.IsZombie():
        # print("  -> Remove (BADFILE)")
        os.remove(fpath)
        continue

    obj = f.Get("Info")
    if not obj:
        # print("  -> Remove (NO Info)")
        os.remove(fpath)
        continue

    if not obj.InheritsFrom("TTree"):
        # print(f"  -> Remove (Info exists but type = {obj.ClassName()})")
        os.remove(fpath)
        continue

    # print("  -> Keep (Info is TTree)")
    f.Close()