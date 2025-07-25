#!/bin/bash

idx=$1
datatype=$2

export HOME=/user/kakang/
mkdir -p ./logs/${datatype}/pv_res_analysis/
mkdir -p ./figures/${datatype}/pv_res_analysis/pvx_fit
mkdir -p ./figures/${datatype}/pv_res_analysis/pvy_fit
mkdir -p ./figures/${datatype}/pv_res_analysis/pvz_fit
mkdir -p ./figures/${datatype}/pv_res_analysis/pullx_fit
mkdir -p ./figures/${datatype}/pv_res_analysis/pully_fit
mkdir -p ./figures/${datatype}/pv_res_analysis/pullz_fit
mkdir -p ./json/${datatype}/pv_res

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/scripts; root -l -b -q \"pv_res_analysis.cc(${idx})\""
} &> "logs/${datatype}/pv_res_analysis/run_${idx}.log"
