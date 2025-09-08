#!/bin/bash

iera=$1
process=$2

eras=("preEE" "postEE")
era=${eras[$iera]}

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/ZeroBias; root -l -b -q \"pv_res.cc(${iera},${process})\""
} &> "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/ZeroBias/pv_res_${era}/run_${process}.log"
