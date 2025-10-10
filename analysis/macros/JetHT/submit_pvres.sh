#!/bin/bash

IERAS=( 0 )
# IERAS=( 0 1 )
ERAS=( "preEE" "postEE" )

for IERA in "${IERAS[@]}"; do
    ERA=${ERAS[$IERA]}

    rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/pv_res_${ERA}/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/pv_res_${ERA}/"

    rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_${ERA}/pv_res/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_${ERA}/pv_res/"

    rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res/pvx_fit/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res/pvy_fit/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res/pvz_fit/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res/pullx_fit/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res/pully_fit/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res/pullz_fit/"

    condor_submit pv_res.sub iera=${IERA} Era=${ERA}
done
