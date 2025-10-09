#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

datasetlist="mc_JetHT.txt"
GTpreEE="130X_mcRun3_2022_realistic_v5"
GTpostEE="130X_mcRun3_2022_realistic_postEE_v6"

# datasetlist="mc_ZeroBias.txt"
# GTpreEE="132X_mcRun3_2022_realistic_v3"
# GTpostEE="132X_mcRun3_2022_realistic_postEE_v4"
configtemplate="crabConfigTemplate.py"
ver="Track-v20251002"
prodv="/store/user/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
pver="0"
EVENTSCALE=5

datasets=()
while read -r name; do
  if [[ ${name:0:1} == '#' ]]; then
    continue
  fi
  datasets+=("$name")
done < "$datasetlist"

for i in ${!datasets[@]}; do

    dataset=${datasets[i]}

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

    INPUTDATASET=${dataset}
    OUTLFN="${prodv}"

    for (( EVENTMODULO=0; EVENTMODULO<${EVENTSCALE}; EVENTMODULO++ ))
    do

        rm -f crabConfig.py*
        
        REQUESTNAME="${content}_${era_pro}_${format}_S${EVENTSCALE}M${EVENTMODULO}_ver${pver}"
        OUTPUTDATASETTAG="${era_pro}_${format}_S${EVENTSCALE}M${EVENTMODULO}"
        
        cat ${configtemplate} | sed "s|REQUESTNAME|${REQUESTNAME}|g" \
            | sed "s|INPUTDATASET|${INPUTDATASET}|g" \
            | sed "s|OUTPUTDATASETTAG|${OUTPUTDATASETTAG}|g" \
            | sed "s|OUTLFN|${OUTLFN}|g" \
            | sed "s|GLOBALTAG|${GLOBALTAG}|g" \
            | sed "s|EVENTSCALE|${EVENTSCALE}|g" \
            | sed "s|EVENTMODULO|${EVENTMODULO}|g" \
            > "crabConfig.py"

        # crab submit -c crabConfig.py --dryrun
        crab submit -c crabConfig.py
    done

done
