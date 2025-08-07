#!/bin/bash

idx=$1

mkdir -p ../../logs/JetHT/mkfile
eras=("eraB" "eraC" "sim")

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/JetHT; root -l -b -q \"mkfile.cc(${idx})\""
} &> "../../logs/JetHT/mkfile/${eras[$idx]}.log"
