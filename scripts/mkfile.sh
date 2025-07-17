#!/bin/bash

idx=$1
dataset=$2

export HOME=/user/kakang/
mkdir -p ./logs/${dataset}/mkfile

eras=("B" "C" "D" "E" "F" "G")

{
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /user/kakang/IPres/CMSSW_14_0_10/src
cmssw-el8 -- "cmsenv; cd scripts; root -l -b -q \"mkfile.c(${idx})\""
} &> "logs/${dataset}/mkfile/era${eras[$idx]}.log"
