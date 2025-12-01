import sys
import subprocess
import json
import os

with open("tuplelist.json") as f:
    cfg = json.load(f)


def submit_jobs(period):
    sample = "mc"

    os.makedirs("./condor_logs", exist_ok=True)

    for era in cfg[period][sample]:
        dataset = era["dataset"]

        if(dataset == "MC_50to80"): continue
        if(dataset == "MC_3200"): continue

        os.makedirs(
            f"/eos/home-k/kakang/IPres/analysis/JetHT/tuples/{period}/{dataset}",
            exist_ok=True,
        )

        scheduler_log = f"./condor_logs/XsecWeight_{period}_{dataset}.log"
        if os.path.exists(scheduler_log):
            os.remove(scheduler_log)

        submit_description = f"""
executable = XsecWeight.sh
arguments = {period} {dataset}
log = {scheduler_log}
JobBatchName = IPres_JetHT_XsecWeight_{period}_{dataset}
request_cpus = 1
request_memory = 1G
request_disk = 10M
+JobFlavour = "workday"
notify_user = kai.kang@cern.ch
notification = error
max_retries = 1
should_transfer_files = NO
queue 1
"""
        subprocess.run(["condor_submit"], input=submit_description.encode(), check=True)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python submit_XsecWeight.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    submit_jobs(sys.argv[1])
    sys.exit(0)
