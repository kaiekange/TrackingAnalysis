#!/bin/bash

idx=$1
dataset=$2

export HOME=/user/kakang/

mkdir -p ./logs/${dataset}/vertex_analysis
mkdir -p ./figures/${dataset}/vertex_analysis

eras=("B" "C" "D" "E" "F" "G" "ALL")

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/scripts; root -l -b -q \"vertex_analysis.cc(${idx})\""
} &> "logs/${dataset}/vertex_analysis/run_${eras[$idx]}.log"
