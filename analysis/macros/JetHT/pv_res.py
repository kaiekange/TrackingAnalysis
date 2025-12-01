import sys
import subprocess
import os;

def submit_jobs(period):

    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/logs/{period}/pv_res/", exist_ok=True)
    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/json/{period}/pv_res/", exist_ok=True)
    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/figures/{period}/pv_res/pvx_fit", exist_ok=True)
    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/figures/{period}/pv_res/pvy_fit", exist_ok=True)
    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/figures/{period}/pv_res/pvz_fit", exist_ok=True)
    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/figures/{period}/pv_res/pullx_fit", exist_ok=True)
    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/figures/{period}/pv_res/pully_fit", exist_ok=True)
    os.makedirs(f"/eos/home-k/kakang/IPres/analysis/JetHT/figures/{period}/pv_res/pullz_fit", exist_ok=True)

    scheduler_log = f"./condor_logs/pvres_{period}.log"
    if os.path.exists(scheduler_log):
        os.remove(scheduler_log)

    submit_description = f"""
executable = pv_res.sh
arguments = {period} $(Process)
log = ./condor_logs/pvres_{period}.log
JobBatchName = IPres_JetHT_pvres_{period}
request_cpus = 1
request_memory = 8G
request_disk = 10M
+JobFlavour = "workday"
notify_user = kai.kang@cern.ch
notification = error
max_retries = 1
should_transfer_files = NO
queue 100
"""
    subprocess.run(["condor_submit"], input=submit_description.encode())

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python pv_res.py <period>  # e.g. 2022preEE or 2022postEE")
        sys.exit(1)
    sys.exit(submit_jobs(sys.argv[1]))
