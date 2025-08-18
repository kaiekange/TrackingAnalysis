#!/bin/bash

idx=$1

mkdir -p /pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/ZeroBias/mkfile

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/ZeroBias; root -l -b -q \"mkfile.cc(${idx})\""
} &> "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/ZeroBias/mkfile/run_${idx}.log"
