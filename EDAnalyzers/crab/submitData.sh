#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

##2022
#GLOBALTAG="124X_dataRun3_v15"
#datasetlist="data_2022_ZeroBias.txt"
#datasetlist="data_2022_JetHT.txt"

#2023
GLOBALTAG="130X_dataRun3_PromptAnalysis_v1"
datasetlist="data_2023_ZeroBias.txt"
# datasetlist="data_2023_JetHT.txt"

##2024
#GLOBALTAG="150X_dataRun3_v2"
#datasetlist="data_2024_ZeroBias.txt"
#datasetlist="data_2024_JetHT.txt"

configtemplate="crabConfigTemplateData.py"
ver="Track-v20251022"

prodv="/store/user/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
pver="0"
EVENTSCALE=20
EVENTMODULO=0

rm -f crabConfig.py*

datasets=()
while read -r name number; do
  if [[ ${name:0:1} == '#' ]]; then
    continue
  fi
  datasets+=("$name")
done < "$datasetlist"

for i in "${!datasets[@]}"; do

    dataset=${datasets[i]}

    split=(${dataset//\// })
    content=${split[0]}
    format=${split[2]}
    era_pro=$(echo ${split[1]} | sed "s|-|_|g")

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
        | sed "s|EVENTMODULO|${EVENTMODULO}|g" \
        > "crabConfig.py"

    # crab submit -c crabConfig.py --dryrun
    crab submit -c crabConfig.py

done
