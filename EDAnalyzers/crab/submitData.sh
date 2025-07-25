#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

datasetlist="data_JetHT.txt"
configtemplate="crabConfigTemplateData.py"
ver="Track-v20250328_noBS"
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
        > "crabConfig.py"

    # crab submit -c crabConfig.py --dryrun
    crab submit -c crabConfig.py

done
