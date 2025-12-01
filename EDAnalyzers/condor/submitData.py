import sys
import subprocess
import json
import os
import datetime
from pathlib import Path

EVENTSCALE = 20
EVENTMODULO = 0

CONFIG = {

    "2022": {
        "GlobalTag": "124X_dataRun3_v15",
        "GoldenJSON": "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2022_355100_362760_Golden.json",
        "ZeroBias": "DatasetsList/data_2022_ZeroBias.txt",
        "JetHT": "DatasetsList/data_2022_JetHT.txt"
    },

    "2023": {
        "GlobalTag": "130X_dataRun3_PromptAnalysis_v1",
        "GoldenJSON": "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2023_366442_370790_Golden.json",
        "ZeroBias": "DatasetsList/data_2023_ZeroBias.txt",
        "JetHT": "DatasetsList/data_2023_JetHT.txt"
    },

    "2024": {
        "GlobalTag": "150X_dataRun3_v2",
        "GoldenJSON": "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2024_378981_386951_Golden.json",
        "ZeroBias": "DatasetsList/data_2024_ZeroBias.txt",
        "JetHT": "DatasetsList/data_2024_JetHT.txt"
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
    era_pro = era_pro.replace("-", "_")
    format_ = split[2]
    timestamp = datetime.datetime.now().strftime("%y%m%d_%H%M%S")
    outdir = f"{prodv}/{content}/{era_pro}_{format_}_S{EVENTSCALE}M{EVENTMODULO}/{timestamp}/0000"

    return outdir


def submitData(period, datatype):
    DatasetList = CONFIG[period][datatype]
    GlobalTag = CONFIG[period]["GlobalTag"]
    GoldenJSON = CONFIG[period]["GoldenJSON"]
    Datasets = load_datasets(DatasetList)

    ver="Track-v20251201"
    prodv=f"/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/{ver}"

    GroupSize = 20

    for dataset in Datasets:

        title = f"{dataset.split('/')[1]}_{dataset.split('/')[2].split('-')[0]}_{dataset.split('/')[2].split('-')[-1]}"

        InputList = f"/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/condor/InputList/{period}_{datatype}_{title}.txt"

        njobs = save_das_files(dataset, InputList)
        outdir = convert(dataset, prodv, EVENTSCALE, EVENTMODULO)

        os.makedirs(f"{outdir}/log", exist_ok=True)

        n_groups = (njobs + GroupSize - 1) // GroupSize

        scheduler_log = f"condor_logs/IPres_tuple_{period}_{title}.log"
        if os.path.exists(scheduler_log):
            os.remove(scheduler_log)

        submit_description = f"""
executable = run_data.sh
arguments = 1 {GlobalTag} {EVENTSCALE} {EVENTMODULO} {InputList} {outdir} {GroupSize} $(Process) {GoldenJSON}
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
queue {n_groups}
"""
        subprocess.run(["condor_submit"], input=submit_description.encode())

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python submitData.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    submitData(sys.argv[1], sys.argv[2])
    sys.exit(0)
