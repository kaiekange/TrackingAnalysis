#!/bin/bash

idx=$1
sampletype=$(grep '^const TString sampletype' input_list.cc | sed -E 's/.*= *"(.*)".*/\1/')
mkdir -p ../../logs/ZeroBias/pv_res/${sampletype}
mkdir -p ../../figures/ZeroBias/pv_res/${sampletype}/pvx_fit
mkdir -p ../../figures/ZeroBias/pv_res/${sampletype}/pvy_fit
mkdir -p ../../figures/ZeroBias/pv_res/${sampletype}/pvz_fit
mkdir -p ../../figures/ZeroBias/pv_res/${sampletype}/pullx_fit
mkdir -p ../../figures/ZeroBias/pv_res/${sampletype}/pully_fit
mkdir -p ../../figures/ZeroBias/pv_res/${sampletype}/pullz_fit
mkdir -p ../../json/ZeroBias/pv_res/${sampletype}

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/ZeroBias; root -l -b -q \"pv_res_analysis.cc(${idx})\""
} &> "../../logs/ZeroBias/pv_res/${sampletype}/run_${idx}.log"
