import sys
import subprocess
import json
import os
import datetime
from pathlib import Path

EVENTSCALE = 10
EVENTMODULO = 0

CONFIG = {
    "2022_preEE": {
        "ZeroBias": {
            "Datasets": "DatasetsList/mc_2022_ZeroBias_preEE.txt",
            "GlobalTag": "132X_mcRun3_2022_realistic_v3"
        },
        "JetHT": {
            "Datasets": "DatasetsList/mc_2022_JetHT_preEE.txt",
            "GlobalTag": "130X_mcRun3_2022_realistic_v5"
        }
    },

    "2022_postEE": {
        "ZeroBias": {
            "Datasets": "DatasetsList/mc_2022_ZeroBias_postEE.txt",
            "GlobalTag": "132X_mcRun3_2022_realistic_postEE_v4"
        },
        "JetHT": {
            "Datasets": "DatasetsList/mc_2022_JetHT_postEE.txt",
            "GlobalTag": "130X_mcRun3_2022_realistic_postEE_v6"
        }
    },

    "2023_preBPix": {
        "ZeroBias": {
            "Datasets": "DatasetsList/mc_2023_ZeroBias_preBPix.txt",
            "GlobalTag": "130X_mcRun3_2023_realistic_v14"
        },
        "JetHT": {
            "Datasets": "DatasetsList/mc_2023_JetHT_preBPix.txt",
            "GlobalTag": "130X_mcRun3_2023_realistic_v14"
        }
    },


    "2023_postBPix": {
        "ZeroBias": {
            "Datasets": "DatasetsList/mc_2023_ZeroBias_postBPix.txt",
            "GlobalTag": "130X_mcRun3_2023_realistic_postBPix_v2"
        },
        "JetHT": {
            "Datasets": "DatasetsList/mc_2023_JetHT_postBPix.txt",
            "GlobalTag": "130X_mcRun3_2023_realistic_postBPix_v2"
        }
    },

    "2024": {
        "ZeroBias": {
            "Datasets": "DatasetsList/mc_2024_ZeroBias.txt",
            "GlobalTag": "150X_mcRun3_2024_realistic_v2"
        },
        "JetHT": {
            "Datasets": "DatasetsList/mc_2024_JetHT.txt",
            "GlobalTag": "150X_mcRun3_2024_realistic_v2"
        }
    }
}

def save_das_files(dataset, outfile):
    cmd = f'dasgoclient --query="file dataset={dataset}"'

    out = subprocess.check_output(cmd, shell=True, text=True)

    lines = [l.strip() for l in out.splitlines() if l.strip()]
    Path(outfile).write_text("\n".join(lines))

    return len(lines)

def load_datasets(txtfile):
    datasets = []

    with open(txtfile) as f:
        for line in f:
            s = line.strip()
            if not s:
                continue
            if s.startswith("#"):
                continue
            datasets.append(s)

    return datasets


def convert(dataset, prodv, EVENTSCALE, EVENTMODULO):
    tmpdataset = dataset.lstrip("/")
    split = tmpdataset.split("/")
    content = split[0]
    era_pro = split[1]
    era_pro = era_pro.split("MiniAOD")[0]
    format_ = split[2]
    timestamp = datetime.datetime.now().strftime("%y%m%d_%H%M%S")
    outdir = f"{prodv}/{content}/{era_pro}_{format_}_S{EVENTSCALE}M{EVENTMODULO}/{timestamp}/0000"

    return outdir


def submitMC(period, datatype):
    DatasetList = CONFIG[period][datatype]["Datasets"]
    GlobalTag = CONFIG[period][datatype]["GlobalTag"]
    Datasets = load_datasets(DatasetList)

    ver="Track-v20251127"
    prodv=f"/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/{ver}"

    for dataset in Datasets:
        
        title = dataset.split("/")[1].replace("-", "_").split("_Tune")[0]
        
        InputList = f"/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/condor/InputList/{period}_{datatype}_{title}.txt"

        njobs = save_das_files(dataset, InputList)

        outdir = convert(dataset, prodv, EVENTSCALE, EVENTMODULO)

        os.makedirs(f"{outdir}/log", exist_ok=True)

        # test_cmd = f'./run.sh 0 {GlobalTag} {EVENTSCALE} {EVENTMODULO} {InputList} {outdir} 0'
        # subprocess.run(test_cmd, shell=True)

        scheduler_log = f"condor_logs/IPres_tuple_{period}_{title}.log"
        if os.path.exists(scheduler_log):
            os.remove(scheduler_log)

        submit_description = f"""
executable = run.sh
arguments = 0 {GlobalTag} {EVENTSCALE} {EVENTMODULO} {InputList} {outdir} $(Process)
log = {scheduler_log}
JobBatchName = IPres_tuple_{period}_{title}
request_cpus = 1
request_memory = 1G
request_disk = 10M
+JobFlavour = "tomorrow"
notify_user = kai.kang@cern.ch
notification = error
max_retries = 1
should_transfer_files = NO
queue {njobs}
"""
        subprocess.run(["condor_submit"], input=submit_description.encode())


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python submitMC.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    submitMC(sys.argv[1], sys.argv[2])
    sys.exit(0)
