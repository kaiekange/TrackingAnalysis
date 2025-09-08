#!/bin/bash

iera=$1
process=$(( $2 + 1 ))

eras=(
    "2022_data_B"
    "2022_data_C"
    "2022_data_D"
    "2022_data_E"
    "2022_mc_preEE"
    "2022_mc_postEE"
)
infilepaths=(
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/ZeroBias/Run2022B_27Jun2023_v1_MINIAOD/250820_124458/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/ZeroBias/Run2022C_27Jun2023_v1_MINIAOD/250820_130734/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/ZeroBias/Run2022D_27Jun2023_v1_MINIAOD/250820_130743/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/ZeroBias/Run2022E_27Jun2023_v1_MINIAOD/250820_130754/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/SingleNeutrino_E-10_gun/Run3Summer22_MINIAODSIM/250820_142350/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/SingleNeutrino_E-10_gun/Run3Summer22EE_MINIAODSIM/250820_142401/0000"
)

infile="${infilepaths[$iera]}/output_${process}.root"
logfile="/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/ZeroBias/mkfile/${eras[$iera]}/skim_${process}.log"
outfile="/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/${eras[$iera]}/skimmed_${process}.root"

mkdir -p /pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/ZeroBias/mkfile/${eras[$iera]}
mkdir -p /pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/ZeroBias/${eras[$iera]}

if [[ ! -f "${infile}" ]]; then
    rm -f "${logfile}" "${outfile}"
    echo "${eras[$iera]}, file ${process} doesn't exist" &>> mkfile.log
else
    if [[ ! -f "${logfile}"  ||  ! -f "${outfile}" ]]; then
        rm -f "${logfile}" "${outfile}"
        root -l -b -q "mkfile.cc(\"${infile}\", \"${outfile}\")" &> ${logfile}
    fi
fi
