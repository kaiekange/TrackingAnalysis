#!/bin/bash

idx=$1
datatype=$2

export HOME=/user/kakang/
mkdir -p ./logs/${datatype}/pv_res_analysis/
mkdir -p ./figures/${datatype}/pv_res_analysis/pvx_fit
mkdir -p ./figures/${datatype}/pv_res_analysis/pvy_fit
mkdir -p ./figures/${datatype}/pv_res_analysis/pvz_fit
mkdir -p ./figures/${datatype}/pv_res_analysis/pvx_pull
mkdir -p ./figures/${datatype}/pv_res_analysis/pvy_pull
mkdir -p ./figures/${datatype}/pv_res_analysis/pvz_pull

{
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /user/kakang/IPres/CMSSW_14_0_10/src
cmssw-el8 -- "cmsenv; cd scripts; root -l -b -q \"pv_res_analysis.c(${idx})\""
} &> "logs/${datatype}/pv_res_analysis/run_${idx}.log"
