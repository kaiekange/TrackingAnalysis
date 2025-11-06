import subprocess
import json

# periods = ["2022_preEE", "2022_postEE", "2023_preBPix", "2023_postBPix", "2024"]
periods = ["2023_preBPix", "2023_postBPix"]

# samples = ["data", "mc"]
# samples = ["mc"]
samples = ["data"]

with open("mkfile.json") as f:
    cfg = json.load(f)

for period in periods:
    for sample in samples:
        for entry in cfg[period][sample]:
            submit_description = f"""
executable = mkfile.sh
arguments = {period} {entry['dataset']} {entry['path']} $(Process)
log = ./condor_logs/mkfile_{period}_{entry['dataset']}.log
JobBatchName = IPres_ZeroBias_mkfile_{period}_{entry['dataset']}
request_cpus = 4
request_memory = 8G
request_disk = 10M
notify_user = kai.kang@cern.ch
notification = error 
max_retries = 1
should_transfer_files = NO
queue {entry['njobs']}
"""
            subprocess.run(["condor_submit"], input=submit_description.encode())
