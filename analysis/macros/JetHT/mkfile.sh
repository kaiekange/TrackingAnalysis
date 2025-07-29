#!/bin/bash

idx=$1
datatype=$2

export HOME=/user/kakang/

mkdir -p ../../logs/${datatype}/mkfile

eras=("B" "C" "D" "E" "F" "G")

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/${datatype}; root -l -b -q \"mkfile.cc(${idx})\""
} &> "../../logs/${datatype}/mkfile/era${eras[$idx]}.log"
