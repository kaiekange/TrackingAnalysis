#!/bin/bash

ERAS=( "preEE" "postEE" )

for IERA in "${!ERAS[@]}"; do
    ERA=${ERAS[$IERA]}

    rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/ZeroBias/pv_res_${ERA}/"
    rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/ZeroBias_${ERA}/pv_res/"
    rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias_${ERA}/pv_res/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/ZeroBias/pv_res_${ERA}/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/ZeroBias_${ERA}/pv_res/"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias_${ERA}/pv_res/pvx_fit"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias_${ERA}/pv_res/pvy_fit"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias_${ERA}/pv_res/pvz_fit"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias_${ERA}/pv_res/pullx_fit"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias_${ERA}/pv_res/pully_fit"
    mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/ZeroBias_${ERA}/pv_res/pullz_fit"
    condor_submit pv_res.sub iera=${IERA} Era=${ERA}
done
