#!/bin/bash

idx=$1

eras=("2022_data_B" "2022_data_C" "2022_data_D" "2022_data_E" "2022_mc_50to80_preEE" "2022_mc_80to120_preEE" "2022_mc_120to170_preEE" "2022_mc_170to300_preEE" "2022_mc_300to470_preEE" "2022_mc_470to600_preEE" "2022_mc_600to800_preEE" "2022_mc_800to1000_preEE" "2022_mc_1000to1400_preEE" "2022_mc_1400to1800_preEE" "2022_mc_1800to2400_preEE" "2022_mc_50to80_postEE" "2022_mc_80to120_postEE" "2022_mc_120to170_postEE" "2022_mc_170to300_postEE" "2022_mc_300to470_postEE" "2022_mc_470to600_postEE" "2022_mc_600to800_postEE" "2022_mc_800to1000_postEE" "2022_mc_1000to1400_postEE" "2022_mc_1400to1800_postEE" "2022_mc_1800to2400_postEE")

mkdir -p /pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/mkfile

rm /pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/mkfile/${eras[$idx]}.root
rm /pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/mkfile/run_${idx}.log

{
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /user/kakang/IPres/CMSSW_14_0_10/src
    cmssw-el8 -- "cmsenv; cd TrackingAnalysis/analysis/macros/JetHT; root -l -b -q \"mkfile.cc(${idx})\""
} &> "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/mkfile/run_${idx}.log"
