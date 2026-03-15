#!/bin/bash

period=$1
process=$2

logfile="/eos/home-k/kakang/IPres/analysis/ZeroBias/logs/${period}/ip_res/run_${process}.log"
{
    root -b -q "/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/analysis/macros/ZeroBias/ip_res.cc(\"${period}\", ${process})"
} &> "${logfile}"
