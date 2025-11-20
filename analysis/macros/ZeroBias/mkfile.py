import sys
import subprocess
import json

# samples = ["data", "mc"]
samples = ["mc"]
# samples = ["data"]

# eras = ["eraC_v1", "eraC_v2", "eraC_v3", "eraC_v4", "eraD_v1", "eraD_v2"]

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
            

            submit_description = f"""
executable = mkfile.sh
arguments = {period} {era['dataset']} {era['path']} {isData} $(Process)
log = ./condor_logs/mkfile_{period}_{era['dataset']}.log
JobBatchName = IPres_ZeroBias_mkfile_{period}_{era['dataset']}
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
        print("Usage: python PU_factor.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    sys.exit(submit_jobs(sys.argv[1]))
