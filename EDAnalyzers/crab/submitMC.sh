#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

datasetlist="mc_JetHT.txt"
GTpreEE="130X_mcRun3_2022_realistic_v5"
GTpostEE="130X_mcRun3_2022_realistic_postEE_v6"

# datasetlist="mc_ZeroBias.txt"
# GTpreEE="132X_mcRun3_2022_realistic_v3"
# GTpostEE="132X_mcRun3_2022_realistic_postEE_v4"
configtemplate="crabConfigTemplate.py"
ver="Track-v20250819"
prodv="/store/user/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
pver="1"

rm -f crabConfig.py*

datasets=()
eventscales=()
while read -r name number; do
  if [[ ${name:0:1} == '#' ]]; then
    continue
  fi
  datasets+=("$name")
  eventscales+=("$number")
done < "$datasetlist"

for i in ${!datasets[@]}; do

    dataset=${datasets[i]}
    EVENTSCALE=${eventscales[i]}

    if [[ "${dataset}" == *"postEE"* ]]; then
        GLOBALTAG=${GTpostEE}
    else
        GLOBALTAG=${GTpreEE}
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
        | sed "s|GLOBALTAG|${GLOBALTAG}|g" \
        | sed "s|EVENTSCALE|${EVENTSCALE}|g" \
        > "crabConfig.py"

    # crab submit -c crabConfig.py --dryrun
    crab submit -c crabConfig.py

done
