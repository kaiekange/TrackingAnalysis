#!/bin/bash

period=$1
dataset=$2
infilepath=$3
isData=$4
process=$(( $5 + 1 ))

mkdir -p "/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/${period}/${dataset}"
mkdir -p "/eos/home-k/kakang/IPres/analysis/ZeroBias/logs/${period}/mkfile/"

infile="${infilepath}/output_${process}.root"
outfile="/eos/home-k/kakang/IPres/analysis/ZeroBias/tuples/${period}/${dataset}/skimmed_${process}.root"
logfile="/eos/home-k/kakang/IPres/analysis/ZeroBias/logs/${period}/mkfile/${dataset}.log"

if [[ ! -f "${infile}" ]]; then
    echo "${period}, ${dataset}, file ${process} doesn't exist" &>> ${logfile}
else
    if [[ ! -f "${outfile}" ]]; then
        root -l -b -q "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/ZeroBias/mkfile.cc(\"${infile}\", \"${outfile}\", ${isData})"
    fi
fi
