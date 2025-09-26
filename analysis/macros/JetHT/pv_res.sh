#!/bin/bash

iera=$1
trigHT=$2
process=$3

eras=("preEE" "postEE")
era=${eras[$iera]}

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/JetHT; root -l -b -q \"pv_res.cc(${iera},${process},${trigHT})\""
} &> "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/pv_res_${era}/PFHT_${trigHT}/run_${process}.log"
