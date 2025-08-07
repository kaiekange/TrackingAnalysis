#!/bin/bash

idx=$1
mkdir -p ../../logs/JetHT/pv_res/compare
mkdir -p ../../figures/JetHT/pv_res/compare/pvx_fit
mkdir -p ../../figures/JetHT/pv_res/compare/pvy_fit
mkdir -p ../../figures/JetHT/pv_res/compare/pvz_fit
mkdir -p ../../figures/JetHT/pv_res/compare/pullx_fit
mkdir -p ../../figures/JetHT/pv_res/compare/pully_fit
mkdir -p ../../figures/JetHT/pv_res/compare/pullz_fit
mkdir -p ../../json/JetHT/pv_res/compare

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/JetHT; root -l -b -q \"pv_res.cc(${idx})\""
} &> "../../logs/JetHT/pv_res/compare/run_${idx}.log"
