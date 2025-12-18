import sys
import subprocess
import json
import os

# samples = ["data", "mc"]
# samples = ["mc"]
samples = ["data"]

with open("tuplelist.json") as f:
    cfg = json.load(f)


def submit_jobs(period):
    for sample in samples:
        if sample == "data":
            isData = 1
        else:
            isData = 0

        # for era in eras:
        for era in cfg[period][sample]:

            if(era['dataset'] != "MET1_eraG_v2"): continue

            os.makedirs(
                f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/{era['dataset']}",
                exist_ok=True,
            )
            os.makedirs(
                f"/eos/home-k/kakang/IPres/analysis/JetHT/logs/{period}/mkfile",
                exist_ok=True,
            )
            os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/json/{period}", exist_ok=True)

            if os.path.exists(f"/eos/home-k/kakang/IPres/analysis/JetHT/logs/{period}/mkfile/{era['dataset']}.log"):
                os.remove(f"/eos/home-k/kakang/IPres/analysis/JetHT/logs/{period}/mkfile/{era['dataset']}.log")

            scheduler_log = f"./condor_logs/mkfile_{period}_{era['dataset']}.log"
            if os.path.exists(scheduler_log):
                os.remove(scheduler_log)

            submit_description = f"""
executable = mkfile.sh
arguments = {period} {era['dataset']} {era['path']} {isData} $(Process)
log = {scheduler_log}
JobBatchName = IPres_JetHT_mkfile_{period}_{era['dataset']}
request_cpus = 1
request_memory = 1G
request_disk = 10M
+JobFlavour = "workday"
notify_user = kai.kang@cern.ch
notification = error
max_retries = 1
should_transfer_files = NO
queue {era['njobs']}
"""
            subprocess.run(["condor_submit"], input=submit_description.encode())


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python mkfile.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    submit_jobs(sys.argv[1])
    sys.exit(0)
