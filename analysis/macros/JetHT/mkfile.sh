#!/bin/bash

iera=$1
offset=( 1 1 1 1 1001 2001 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1)
process=$(( $2 + ${offset[$iera]} ))

eras=(
    "2022_data_B"
    "2022_data_C"
    "2022_data_D"
    "2022_data_E"
    "2022_data_E"
    "2022_data_E"
    "2022_mc_50to80_preEE"
    "2022_mc_80to120_preEE"
    "2022_mc_120to170_preEE"
    "2022_mc_170to300_preEE"
    "2022_mc_300to470_preEE"
    "2022_mc_470to600_preEE"
    "2022_mc_600to800_preEE"
    "2022_mc_800to1000_preEE"
    "2022_mc_1000to1400_preEE"
    "2022_mc_1400to1800_preEE"
    "2022_mc_1800to2400_preEE"
    "2022_mc_2400to3200_preEE"
    "2022_mc_3200_preEE"
    "2022_mc_50to80_postEE"
    "2022_mc_80to120_postEE"
    "2022_mc_120to170_postEE"
    "2022_mc_170to300_postEE"
    "2022_mc_300to470_postEE"
    "2022_mc_470to600_postEE"
    "2022_mc_600to800_postEE"
    "2022_mc_800to1000_postEE"
    "2022_mc_1000to1400_postEE"
    "2022_mc_1400to1800_postEE"
    "2022_mc_1800to2400_postEE"
    "2022_mc_2400to3200_postEE"
    "2022_mc_3200_postEE"
)

infilepaths=(
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/JetHT/Run2022B_27Jun2023_v2_MINIAOD/250825_075652/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/JetMET/Run2022C_27Jun2023_v1_MINIAOD/250825_075701/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/JetMET/Run2022D_27Jun2023_v2_MINIAOD/250825_075710/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/JetMET/Run2022E_27Jun2023_v1_MINIAOD/250825_080349/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/JetMET/Run2022E_27Jun2023_v1_MINIAOD/250825_080349/0001"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/JetMET/Run2022E_27Jun2023_v1_MINIAOD/250825_080349/0002"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-50to80_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_143853/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-80to120_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_143901/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-120to170_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_143910/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-170to300_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_143918/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-300to470_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_143927/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-470to600_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_143935/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-600to800_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250821_155013/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-800to1000_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_143944/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-1000to1400_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_143952/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-1400to1800_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_144000/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-1800to2400_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_144009/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-2400to3200_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_144017/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-3200_TuneCP5_13p6TeV_pythia8/Run3Summer22_MINIAODSIM/250828_144026/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-50to80_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143710/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-80to120_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143718/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-120to170_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143726/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-170to300_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143735/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-300to470_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143744/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-470to600_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143752/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-600to800_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250821_155001/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-800to1000_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143801/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-1000to1400_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143810/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-1400to1800_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143819/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-1800to2400_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143827/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-2400to3200_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143836/0000"
    "/pnfs/iihe/cms/store/user/kakang/Run3TrackingAnalysis/Ntuple/Track-v20250819/QCD_PT-3200_TuneCP5_13p6TeV_pythia8/Run3Summer22EE_MINIAODSIM/250828_143844/0000"
)

infile="${infilepaths[$iera]}/output_${process}.root"
logfile="/pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/mkfile/${eras[$iera]}/skim_${process}.log"
outfile="/pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/${eras[$iera]}/skimmed_${process}.root"

mkdir -p /pnfs/iihe/cms/store/user/kakang/IPres/analysis/logs/JetHT/mkfile/${eras[$iera]}
mkdir -p /pnfs/iihe/cms/store/user/kakang/IPres/analysis/tuples/JetHT/${eras[$iera]}

if [[ ! -f "${infile}" ]]; then
    rm -f "${logfile}" "${outfile}"
    echo "${eras[$iera]}, file ${process} doesn't exist" &>> mkfile.log
else
    if [[ ! -f "${logfile}"  ||  ! -f "${outfile}" ]]; then
        rm -f "${logfile}" "${outfile}"
        root -l -b -q "mkfile.cc(\"${infile}\", \"${outfile}\")" &> ${logfile}
    fi
fi
