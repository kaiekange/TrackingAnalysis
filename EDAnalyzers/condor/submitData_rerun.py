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
        "JetHT": "DatasetsList/data_2022_JetHT.txt",
    },
    "2023": {
        "GlobalTag": "130X_dataRun3_PromptAnalysis_v1",
        "GoldenJSON": "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2023_366442_370790_Golden.json",
        "ZeroBias": "DatasetsList/data_2023_ZeroBias.txt",
        "JetHT": "DatasetsList/data_2023_JetHT.txt",
    },
    "2024": {
        "GlobalTag": "150X_dataRun3_v2",
        "GoldenJSON": "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2024_378981_386951_Golden.json",
        "ZeroBias": "DatasetsList/data_2024_ZeroBias.txt",
        "JetHT": "DatasetsList/data_2024_JetHT.txt",
    },
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


def submitData():
    
    # period = "2022"
    # title = "2022_JetHT_JetMET_Run2022C_v1"
    # outdir = "/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/Track-v20251201/JetMET/Run2022C_27Jun2023_v1_MINIAOD_S20M0/251202_131010/0000"
    # runlist = [367, 1091, 1503]

    # period = "2023"
    # title = "2023_JetHT_JetMET0_Run2023C_v1"
    # outdir = "/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/Track-v20251201/JetMET0/Run2023C_22Sep2023_v2_v1_MINIAOD_S20M0/251202_174334/0000"
    # runlist = [12, 147, 163, 172, 193, 242, 319]

    # period = "2023"
    # title = "2023_JetHT_JetMET1_Run2023C_v1"
    # outdir = "/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/Track-v20251201/JetMET1/Run2023C_22Sep2023_v3_v1_MINIAOD_S20M0/251202_174337/0000"
    # runlist = [6, 35, 132, 183, 193, 210, 213, 253, 265]

    period = "2023"
    title = "2023_JetHT_JetMET1_Run2023D_v1"
    outdir = "/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/Track-v20251201/JetMET1/Run2023D_22Sep2023_v1_v1_MINIAOD_S20M0/251202_174339/0000"
    runlist = [75, 76, 119, 123, 326, 392, 399, 330, 677, 756, 807, 842, 880, 952, 1027, 1052, 1060]

    GlobalTag = CONFIG[period]["GlobalTag"]
    GoldenJSON = CONFIG[period]["GoldenJSON"]
    InputList = f"/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/condor/InputList/{title}.txt"

    for run in runlist:
        scheduler_log = f"rerun_logs/IPres_tuple_{period}_{title}_{run}.log"

        # if os.path.exists(f"{outdir}/output_{run}root"):
        #     os.remove(f"{outdir}/output_{run}root")

        if os.path.exists(scheduler_log):
            os.remove(scheduler_log)

        submit_description = f"""
executable = run.sh
arguments = 1 {GlobalTag} {EVENTSCALE} {EVENTMODULO} {InputList} {outdir} {run} {GoldenJSON}
log = {scheduler_log}
JobBatchName = IPres_tuple_{period}_{title}_rerun_{run}
request_cpus = 1
request_memory = 1G
request_disk = 10M
+JobFlavour = "tomorrow"
notify_user = kai.kang@cern.ch
notification = error
max_retries = 3
should_transfer_files = NO
queue 1
"""
        subprocess.run(["condor_submit"], input=submit_description.encode())


if __name__ == "__main__":
    # if len(sys.argv) < :
    #     print("Usage: python submitData.py <period>  # e.g. 2022preEE or 2022postEE")
    #     sys.exit(1)
    submitData()
    sys.exit(0)
