import sys
import subprocess
import json
import os
import datetime
from pathlib import Path

# SAMPLETYPE = "ZeroBias"
SAMPLETYPE = "JetHT"

# YEAR = "2022"
# GLOBALTAG="132X_mcRun3_2022_realistic_v3"
# GLOBALTAG = "130X_mcRun3_2022_realistic_v5"

# YEAR="2022EE"
# GLOBALTAG="132X_mcRun3_2022_realistic_postEE_v4"
# GLOBALTAG="130X_mcRun3_2022_realistic_postEE_v6"

# YEAR="2023"
# GLOBALTAG="130X_mcRun3_2023_realistic_v14"

# YEAR="2023BPix"
# GLOBALTAG="130X_mcRun3_2023_realistic_postBPix_v2"

YEAR="2024"
GLOBALTAG="150X_mcRun3_2024_realistic_v2"

EVENTSCALE = 1
EVENTMODULO = 0
GroupSize = 1


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


def submitMC():
    DatasetList = f"DatasetList/mc_{YEAR}_{SAMPLETYPE}.txt"
    GlobalTag = GLOBALTAG
    Datasets = load_datasets(DatasetList)

    ver = "Track-v20260210"
    prodv = f"/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/{ver}"

    for dataset in Datasets:

        title = dataset.split("/")[1].replace("-", "_").split("_Tune")[0]

        InputList = f"/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/condor/InputList/{YEAR}_{SAMPLETYPE}_{title}.txt"

        njobs = save_das_files(dataset, InputList)
        ngroup = min(njobs, 100)
        # ngroup = (njobs + GroupSize - 1) // GroupSize
        # ngroup = min(ngroup, 100)
        

        outdir = convert(dataset, prodv, EVENTSCALE, EVENTMODULO)

        print(njobs)

        os.makedirs(f"{outdir}/log", exist_ok=True)

        scheduler_log = f"condor_logs/IPres_tuple_{YEAR}_{title}.log"
        if os.path.exists(scheduler_log):
            os.remove(scheduler_log)

        submit_description = f"""
executable = run_group.sh
arguments = 0 {GlobalTag} {EVENTSCALE} {EVENTMODULO} {SAMPLETYPE} {InputList} {outdir} {GroupSize} $(Process)
log = {scheduler_log}
JobBatchName = IPres_tuple_{YEAR}_{title}
request_cpus = 1
request_memory = 1G
request_disk = 10M
+JobFlavour = "tomorrow"
notify_user = kai.kang@cern.ch
notification = error
max_retries = 1
should_transfer_files = NO
queue {ngroup}
"""
        subprocess.run(["condor_submit"], input=submit_description.encode())


if __name__ == "__main__":
    submitMC()
    sys.exit(0)
