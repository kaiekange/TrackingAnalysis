import sys
import subprocess
import json
import os
import datetime
from pathlib import Path


# SAMPLETYPE = "ZeroBias"
SAMPLETYPE = "JetHT"

# YEAR = "2022"
# GLOBALTAG = "124X_dataRun3_v15"
# LUMIMASK = "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2022_355100_362760_Golden.json"

# YEAR = "2022EE"
# GLOBALTAG = "124X_dataRun3_v15"
# LUMIMASK = "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2022_355100_362760_Golden.json"

# YEAR = "2023"
# GLOBALTAG = "130X_dataRun3_PromptAnalysis_v1"
# LUMIMASK = "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2023_366442_370790_Golden.json"

YEAR = "2023BPix"
GLOBALTAG = "130X_dataRun3_PromptAnalysis_v1"
LUMIMASK = "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2023_366442_370790_Golden.json"

# YEAR = "2024"
# GLOBALTAG = "150X_dataRun3_v2"
# LUMIMASK = "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/crab/JSON/Cert_Collisions2024_378981_386951_Golden.json"

GroupSize = 10

EVENTSCALE = 2
EVENTMODULO = 0


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


def submitData():
    DatasetList = f"DatasetList/data_{YEAR}_{SAMPLETYPE}.txt"
    GlobalTag = GLOBALTAG
    GoldenJSON = LUMIMASK
    Datasets = load_datasets(DatasetList)

    ver = "Track-v20260210"
    prodv = f"/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/{ver}"

    for dataset in Datasets:

        split = dataset.split("/")
        content = split[1]
        era_pro = split[2]
        format_ = split[3]
        era_pro = era_pro.replace("-", "_")
        title = f"{content}_{era_pro}"
        timestamp = datetime.datetime.now().strftime("%y%m%d_%H%M%S")
        outdir = f"{prodv}/{content}/{era_pro}_{format_}_S{EVENTSCALE}M{EVENTMODULO}/{timestamp}/0000"

        InputList = f"/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/condor/InputList/{YEAR}_{SAMPLETYPE}_{title}.txt"

        njobs = save_das_files(dataset, InputList)

        ngroup = (njobs + GroupSize - 1) // GroupSize

        os.makedirs(f"{outdir}/log", exist_ok=True)

        scheduler_log = f"condor_logs/IPres_tuple_{YEAR}_{title}.log"
        if os.path.exists(scheduler_log):
            os.remove(scheduler_log)

        submit_description = f"""
executable = run_group.sh
arguments = 1 {GlobalTag} {EVENTSCALE} {EVENTMODULO} {SAMPLETYPE} {InputList} {outdir} {GroupSize} $(Process) {GoldenJSON}
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
    submitData()
    sys.exit(0)
