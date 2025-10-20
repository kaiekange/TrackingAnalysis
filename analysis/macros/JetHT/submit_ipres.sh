#!/bin/bash
IERAS=( 1 )
# IERAS=( 0 1 )
ERAS=( "preEE" "postEE" )

for IERA in "${IERAS[@]}"; do
    ERA=${ERAS[$IERA]}
    
    rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/ip_res_${ERA}/"
    rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_${ERA}/ip_res/"
    rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/ip_res/ippv_xy_fit/"
    rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/ip_res/ippv_z_fit/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/ip_res_${ERA}/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_${ERA}/ip_res/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/ip_res/ippv_xy_fit"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/ip_res/ippv_z_fit"

    condor_submit ip_res.sub iera=${IERA} Era=${ERA}
done
