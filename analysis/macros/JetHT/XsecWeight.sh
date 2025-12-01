#!/bin/bash
period=$1
dataset=$2

source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/
cmsenv
python3 TrackingAnalysis/analysis/macros/JetHT/XsecWeight.py $period $dataset
