#!/bin/bash

idx=0

eras=("2022_data_B" "2022_data_C" "2022_data_D" "2022_data_E" "2022_mc_postEE" "2022_mc_preEE")

mkdir -p /pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/ZeroBias/mkfile

rm -f /pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/mkfile/${eras[$idx]}.root
rm -f /pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/ZeroBias/mkfile/run_${idx}.log

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/ZeroBias; root -l -b -q \"mkfile.cc(${idx})\""
} &> "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/ZeroBias/mkfile/run_${idx}.log"
