#!/bin/bash

idx=$1
datatype=$2

export HOME=/user/kakang/
mkdir -p ./logs/${datatype}/ip_res_analysis/
mkdir -p ./figures/${datatype}/ip_res_analysis/ippv_xy_fit
mkdir -p ./figures/${datatype}/ip_res_analysis/ippv_z_fit
mkdir -p ./figures/${datatype}/ip_res_analysis/varrange

{
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /user/kakang/IPres/CMSSW_14_0_10/src
cmssw-el8 -- "cmsenv; cd scripts; root -l -b -q \"ip_res_analysis.c(${idx})\""
} &> "logs/${datatype}/ip_res_analysis/run_${idx}.log"
