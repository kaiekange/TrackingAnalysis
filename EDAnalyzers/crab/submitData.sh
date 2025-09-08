#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

GLOBALTAG="124X_dataRun3_v15"
# datasetlist="data_ZeroBias.txt"
datasetlist="data_JetHT.txt"

configtemplate="crabConfigTemplateData.py"
ver="Track-v20250819"

prodv="/store/user/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
pver="1"

rm -f crabConfig.py*

datasets=()
eventscale=()
while read -r name number; do
  if [[ ${name:0:1} == '#' ]]; then
    continue
  fi
  datasets+=("$name")
  eventscales+=("$number")
done < "$datasetlist"

for i in "${!datasets[@]}"; do

    dataset=${datasets[i]}
    EVENTSCALE=${eventscales[i]}

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
        > "crabConfig.py"

    # crab submit -c crabConfig.py --dryrun
    crab submit -c crabConfig.py

done
