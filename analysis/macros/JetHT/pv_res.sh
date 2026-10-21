#!/bin/bash

period=$1
process=$2

# use the ROOT of this project's release (CMSSW_15_0_16)
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src
eval $(scramv1 runtime -sh)

logfile="/eos/home-k/kakang/IPres/analysis/JetHT/logs/${period}/pv_res/run_${process}.log"
{
    root -b -q "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/JetHT/pv_res.cc(\"${period}\", ${process})"
} &> "${logfile}"
