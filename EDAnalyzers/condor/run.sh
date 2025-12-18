#!/bin/bash

RunOnData=$1
GlobalTag=$2
EventScale=$3
EventModulo=$4
InputList=$5
OutputDir=$6
Process=$(($7 + 1))
GoldenJSON=$8

CfgFile="/afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/TrackingAnalysis/EDAnalyzers/test/residuals_condor.py"
LogFile="${OutputDir}/log/output_${Process}.log"
InputFile="$(sed -n "${Process}p" "${InputList}")"
OutputFile="${OutputDir}/output_${Process}.root"

if [ -e "${OutputFile}" ]; then
    echo "Removing existing ${OutputFile}"
    rm -f "${OutputFile}"
fi

export X509_USER_PROXY="/afs/cern.ch/user/k/kakang/tmp/x509up_u158898"

source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /afs/cern.ch/work/k/kakang/IPres/CMSSW_15_0_16/src/
cmsenv
voms-proxy-info -all -file $X509_USER_PROXY
cmsRun ${CfgFile} RunOnData=${RunOnData} GlobalTag=${GlobalTag} EventScale=${EventScale} EventModulo=${EventModulo} InputFile="root://cms-xrd-global.cern.ch/${InputFile}" OutputFile="${OutputFile}" GoldenJSON="${GoldenJSON}" &> ${LogFile}
