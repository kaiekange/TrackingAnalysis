#!/bin/bash

idx=$1
datatype=JetHTMC

export HOME=/user/kakang/

mkdir -p ../../logs/JetHTMC/mkfile


{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/JetHTMC; root -l -b -q \"mkfile.cc(0)\""
} &> "../../logs/JetHTMC/mkfile/sim.log"
