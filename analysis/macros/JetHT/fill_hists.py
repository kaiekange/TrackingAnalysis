"""Steps 1-2 of the PV/IP resolution measurement, as parallel condor jobs.

  python fill_hists.py range <period>   list the input files, submit the "range" jobs
  python fill_hists.py fill  <period>   merge range_*.root -> range.root, submit the "fill" jobs
  python fill_hists.py merge <period>   merge fill_*.root -> fill.root

Step 3: pv_res.cc (PV fits, pull table), then ip_res.cc (IP fits, PV error subtraction).
"""
import glob
import json
import os
import re
import subprocess
import sys

FILES_PER_JOB = 10

MACRO_DIR = "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/JetHT"
STORAGE = "/eos/home-k/kakang/IPres/analysis/JetHT"
PROXY = "/afs/cern.ch/user/k/kakang/tmp/x509up_u158898"


def cmsenv_run(cmd):
    """Run a shell command in the CMSSW environment. Its xrootd client (5.7.2) works with IIHE;
    the system one (5.9.8) fails the GSI handshake ("malformed random tag ... kXGS_cert")."""
    script = ("source /cvmfs/cms.cern.ch/cmsset_default.sh && "
              "cd /afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src && eval $(scramv1 runtime -sh) && "
              f"export X509_USER_PROXY={PROXY} && cd {MACRO_DIR} && {cmd}")
    return subprocess.run(["bash", "-c", script], stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                          universal_newlines=True, stdin=subprocess.DEVNULL)


def list_files(period):
    """List the input files on the submit host, so the jobs need no directory listing."""
    tuplelist = json.load(open(f"{MACRO_DIR}/tuplelist.json"))
    files = []
    for entry in tuplelist[period]["data"]:
        m = re.match(r"(root://[^/]+)/(/.*)", entry["path"])
        server, path = m.group(1), m.group(2)
        out = cmsenv_run(f"xrdfs {server} ls -l {path}")
        if out.returncode != 0:
            sys.exit(f"cannot list {entry['path']}: {out.stderr.strip()}")
        n = 0
        for line in out.stdout.splitlines():
            parts = line.split()
            if parts and parts[-1].endswith(".root") and int(parts[3]) > 0:
                files.append(f"{server}/{parts[-1]}")
                n += 1
        print(f"{entry['path']}: {n} files")
    return files


def njobs(period):
    nfiles = sum(1 for line in open(f"{STORAGE}/hists/{period}/files.txt") if line.strip())
    return (nfiles + FILES_PER_JOB - 1) // FILES_PER_JOB


def hadd(period, step):
    """Merge the per-job outputs; refuse if any job output is missing."""
    n = njobs(period)
    parts = [f"{STORAGE}/hists/{period}/{step}/{step}_{i}.root" for i in range(n)]
    missing = [i for i, p in enumerate(parts) if not os.path.exists(p)]
    if missing:
        sys.exit(f"{step}: {len(missing)}/{n} job outputs missing: {missing}")
    subprocess.run(["hadd", "-f", f"{STORAGE}/hists/{period}/{step}.root"] + parts, check=True)


def submit(period, step):
    n = njobs(period)
    os.makedirs(f"{STORAGE}/logs/{period}/fill", exist_ok=True)
    os.makedirs(f"{STORAGE}/hists/{period}/{step}", exist_ok=True)
    for old in glob.glob(f"{STORAGE}/hists/{period}/{step}/{step}_*.root"):
        os.remove(old)

    scheduler_log = f"./condor_logs/{step}_{period}.log"
    if os.path.exists(scheduler_log):
        os.remove(scheduler_log)

    submit_description = f"""
executable = fill_hists.sh
arguments = {period} {step} $(Process) {FILES_PER_JOB}
log = {scheduler_log}
JobBatchName = IPres_JetHT_{step}_{period}
MY.WantOS = "el8"
request_cpus = 1
request_memory = 2G
request_disk = 10M
+JobFlavour = "longlunch"
notify_user = kai.kang@cern.ch
notification = error
max_retries = 3
should_transfer_files = NO
queue {n}
"""
    subprocess.run(["condor_submit"], input=submit_description.encode(), check=True)


def main():
    if len(sys.argv) != 3 or sys.argv[1] not in ("range", "fill", "merge"):
        sys.exit(__doc__)
    step, period = sys.argv[1], sys.argv[2]
    os.chdir(MACRO_DIR)
    # compile once here with CMSSW's ROOT (as the jobs use it), so the jobs do not all compile at once
    out = cmsenv_run("root -b -q -e 'gSystem->SetBuildDir(\"build_cmssw\", kTRUE);' -e '.L fill_hists.cc+O'")
    if out.returncode != 0 or "error" in out.stdout.lower() + out.stderr.lower():
        sys.exit("compilation of fill_hists.cc failed:\n" + out.stdout + out.stderr)

    if step == "range":
        os.makedirs(f"{STORAGE}/hists/{period}", exist_ok=True)
        files = list_files(period)
        with open(f"{STORAGE}/hists/{period}/files.txt", "w") as f:
            f.write("\n".join(files) + "\n")
        print(f"{len(files)} files -> {njobs(period)} jobs")
        submit(period, "range")
    elif step == "fill":
        hadd(period, "range")
        submit(period, "fill")
    else:
        hadd(period, "fill")


if __name__ == "__main__":
    main()
