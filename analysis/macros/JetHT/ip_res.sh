#!/bin/bash

idx=$1
mkdir -p ../../logs/JetHT/ip_res/compare
mkdir -p ../../figures/JetHT/ip_res/compare/ippv_xy_fit
mkdir -p ../../figures/JetHT/ip_res/compare/ippv_z_fit
mkdir -p ../../json/JetHT/ip_res/compare

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/JetHT; root -l -b -q \"ip_res.cc(${idx})\""
} &> "../../logs/JetHT/ip_res/compare/run_${idx}.log"
