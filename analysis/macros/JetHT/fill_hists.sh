#!/bin/bash

period=$1
step=$2
chunk=$3
files_per_job=$4

# needed to read the ntuples from T2_BE_IIHE over xrootd
export X509_USER_PROXY=/afs/cern.ch/user/k/kakang/tmp/x509up_u158898
# the worker nodes' own CA bundle cannot verify the IIHE server certificate; the CVMFS bundle can
export X509_CERT_DIR=/cvmfs/grid.cern.ch/etc/grid-security/certificates

# Use CMSSW's ROOT and xrootd client (5.7.2): the system xrootd 5.9.8 client rejects the IIHE
# GSI handshake ("secgsi_AddSerialized: malformed random tag ... kXGS_cert").
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src
eval $(scramv1 runtime -sh)

cd /afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/JetHT
logfile="/eos/home-k/kakang/IPres/analysis/JetHT/logs/${period}/fill/${step}_${chunk}.log"
# compiled once by fill_hists.py into build_cmssw (separate from any system-ROOT build)
root -b -q -e 'gSystem->SetBuildDir("build_cmssw", kTRUE);' -e '.L fill_hists.cc+O' \
    -e "fill_hists(\"${period}\", \"${step}\", ${chunk}, ${files_per_job})" &> "${logfile}"
