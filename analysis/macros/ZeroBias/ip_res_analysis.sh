#!/bin/bash

idx=$1
sampletype=$(grep '^const TString sampletype' input_list.cc | sed -E 's/.*= *"(.*)".*/\1/')
mkdir -p ../../logs/ZeroBias/ip_res/${sampletype}
mkdir -p ../../figures/ZeroBias/ip_res/${sampletype}/ippv_xy_fit
mkdir -p ../../figures/ZeroBias/ip_res/${sampletype}/ippv_z_fit
mkdir -p ../../json/ZeroBias/ip_res/${sampletype}

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/ZeroBias; root -l -b -q \"ip_res_analysis.cc(${idx})\""
} &> "../../logs/ZeroBias/ip_res/${sampletype}/run_${idx}.log"
