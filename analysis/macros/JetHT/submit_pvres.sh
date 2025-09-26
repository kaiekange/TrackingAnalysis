#!/bin/bash

# ERAS=( "postEE" )
ERAS=( "preEE" "postEE" )
# TRIGHTS=( 780 680 590 510 430 370 250 180 )
TRIGHTS=( 1050 890 780 680 590 510 430 370 250 180 )

for IERA in "${!ERAS[@]}"; do
    ERA=${ERAS[$IERA]}

    for ITRIG in "${!TRIGHTS[@]}"; do
        TRIGHT=${TRIGHTS[$ITRIG]}

        rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/pv_res_${ERA}/PFHT_${TRIGHT}/"
        mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/pv_res_${ERA}/PFHT_${TRIGHT}/"

        rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_${ERA}/pv_res_${TRIGHT}/"
        mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/json/JetHT_${ERA}/pv_res_${TRIGHT}/"

        rm -rf "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res_${TRIGHT}/"
        mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res_${TRIGHT}/pvx_fit"
        mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res_${TRIGHT}/pvy_fit"
        mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res_${TRIGHT}/pvz_fit"
        mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res_${TRIGHT}/pullx_fit"
        mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res_${TRIGHT}/pully_fit"
        mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/figures/JetHT_${ERA}/pv_res_${TRIGHT}/pullz_fit"

        condor_submit pv_res.sub iera=${IERA} Era=${ERA} trigHT=${TRIGHT}
    done
done
