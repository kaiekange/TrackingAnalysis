#!/bin/bash

datatype=$1

export HOME=/user/kakang/

rm -r ./logs/${datatype}/vertex_analysis
mkdir -p ./logs/${datatype}/vertex_analysis
rm -r ./figures/${datatype}/vertex_analysis
mkdir -p ./figures/${datatype}/vertex_analysis

{
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /user/kakang/IPres/CMSSW_14_0_10/src
cmssw-el8 -- "cmsenv; cd scripts; root -l -b -q vertex_analysis.c"
} &> "logs/${datatype}/vertex_analysis/run.log"
