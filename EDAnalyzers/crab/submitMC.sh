#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

datasetlist="mc_JetHT.txt"
# datasetlist="mc_ZeroBias.txt"
configtemplate="crabConfigTemplate.py"
ver="Track-v20250717"
prodv="/store/user/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
pver="0"

rm -f crabConfig.py*

datasets=()
while read -r line; do
  if [[ ${line:0:1} == '#' ]]; then
    continue
  fi
  datasets+=("$line")
done < "$datasetlist"

for dataset in ${datasets[@]}
do

    if [[ "${dataset}" == *"postEE"* ]]; then
        flag_2022preEE="0"
        flag_2022postEE="1"
    else
        flag_2022preEE="1"
        flag_2022postEE="0"
    fi

    tmpdataset=${dataset#/}
    IFS='/' read -r -a split <<< "$tmpdataset"

    content=${split[0]}
    content=$(echo "$content" | cut -d'_' -f1-3)

    era_pro=${split[1]}
    era_pro=${era_pro%%MiniAOD*}

    format=${split[2]}


    REQUESTNAME="${content}_${era_pro}_${format}_ver${pver}"
    INPUTDATASET=${dataset}
    OUTPUTDATASETTAG="${era_pro}_${format}"
    OUTLFN="${prodv}"

    cat ${configtemplate} | sed "s|REQUESTNAME|${REQUESTNAME}|g" \
        | sed "s|INPUTDATASET|${INPUTDATASET}|g" \
        | sed "s|OUTPUTDATASETTAG|${OUTPUTDATASETTAG}|g" \
        | sed "s|OUTLFN|${OUTLFN}|g" \
        | sed "s|flag_2022preEE|${flag_2022preEE}|g" \
        | sed "s|flag_2022postEE|${flag_2022postEE}|g" \
        > "crabConfig.py"

    # crab submit -c crabConfig.py --dryrun
    crab submit -c crabConfig.py

done
