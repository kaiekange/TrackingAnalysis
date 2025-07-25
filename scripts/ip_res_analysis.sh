#!/bin/bash

idx=$1
dataset=$2

export HOME=/user/kakang/
mkdir -p ./logs/${dataset}/ip_res_analysis/
mkdir -p ./figures/${dataset}/ip_res_analysis/ippv_xy_fit
mkdir -p ./figures/${dataset}/ip_res_analysis/ippv_z_fit
mkdir -p ./figures/${dataset}/ip_res_analysis/ipbs_xy_fit
mkdir -p ./figures/${dataset}/ip_res_analysis/ipbs_z_fit
mkdir -p ./json/${dataset}/ip_res

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/scripts; root -l -b -q \"ip_res_analysis.cc(${idx})\""
} &> "logs/${dataset}/ip_res_analysis/run_${idx}.log"
