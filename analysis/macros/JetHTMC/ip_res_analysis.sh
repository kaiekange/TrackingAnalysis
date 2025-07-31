#!/bin/bash

idx=$1

export HOME=/user/kakang/
mkdir -p ../../logs/JetHTMC/ip_res_analysis/
mkdir -p ../../figures/JetHTMC/ip_res_analysis/ippv_xy_fit
mkdir -p ../../figures/JetHTMC/ip_res_analysis/ippv_z_fit
mkdir -p ../../json/JetHTMC/ip_res

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/JetHTMC; root -l -b -q \"ip_res_analysis.cc(${idx})\""
} &> "../../logs/JetHTMC/ip_res_analysis/run_${idx}.log"
