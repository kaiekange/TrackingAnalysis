import sys
import subprocess
import os;

def submit_jobs(period):

    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/logs/{period}/ip_res/", exist_ok=True)
    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/json/{period}/ip_res/", exist_ok=True)
    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/figures/{period}/ip_res/ippv_xy_fit", exist_ok=True)
    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/figures/{period}/ip_res/ippv_z_fit", exist_ok=True)

    scheduler_log = f"./condor_logs/ipres_{period}.log"
    if os.path.exists(scheduler_log):
        os.remove(scheduler_log)

    submit_description = f"""
executable = ip_res.sh
arguments = {period} $(Process)
log = ./condor_logs/ipres_{period}.log
JobBatchName = IPres_JetHT_ipres_{period}
request_cpus = 1
request_memory = 8G
request_disk = 10M
+JobFlavour = "workday"
notify_user = kai.kang@cern.ch
notification = error
max_retries = 1
should_transfer_files = NO
queue 1
"""
    subprocess.run(["condor_submit"], input=submit_description.encode())

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python ip_res.py <period>  # e.g. 2022 or 2022EE")
        sys.exit(1)
    sys.exit(submit_jobs(sys.argv[1]))
