#!/bin/bash

idx=$1

export HOME=/user/kakang/
mkdir -p ../../logs/JetHTMC/pv_res_analysis/
mkdir -p ../../figures/JetHTMC/pv_res_analysis/pvx_fit
mkdir -p ../../figures/JetHTMC/pv_res_analysis/pvy_fit
mkdir -p ../../figures/JetHTMC/pv_res_analysis/pvz_fit
mkdir -p ../../figures/JetHTMC/pv_res_analysis/pullx_fit
mkdir -p ../../figures/JetHTMC/pv_res_analysis/pully_fit
mkdir -p ../../figures/JetHTMC/pv_res_analysis/pullz_fit
mkdir -p ../../json/JetHTMC/pv_res

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/JetHTMC; root -l -b -q \"pv_res_analysis.cc(${idx})\""
} &> "../../logs/JetHTMC/pv_res_analysis/run_${idx}.log"
