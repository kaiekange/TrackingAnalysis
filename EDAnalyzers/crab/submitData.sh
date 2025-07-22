#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

<<<<<<< HEAD
datasetlist="data.txt"
configtemplate="crabConfigTemplateData.py"
=======
datasetlist="mc.txt"
configtemplate="crabConfigTemplate.py"
>>>>>>> b9b8ce0 (ip resolution)
ver="Track-v20250717"
prodv="/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
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
<<<<<<< HEAD
    format=${split[2]}
    era_pro=$(echo ${split[1]} | sed "s|-|_|g")
=======
    content=${content//_*}
    era_pro=${split[1]}
    era_pro=${era_pro//-*}
    format=${split[2]}
>>>>>>> b9b8ce0 (ip resolution)

    REQUESTNAME="${content}_${era_pro}_${format}_ver${pver}"
    INPUTDATASET=${dataset}
    OUTPUTDATASETTAG="${era_pro}_${format}"
    OUTLFN="${prodv}"

    cat ${configtemplate} | sed "s|REQUESTNAME|${REQUESTNAME}|g" \
        | sed "s|INPUTDATASET|${INPUTDATASET}|g" \
        | sed "s|OUTPUTDATASETTAG|${OUTPUTDATASETTAG}|g" \
        | sed "s|OUTLFN|${OUTLFN}|g" \
        > "crabConfig.py"

<<<<<<< HEAD
    # crab submit -c crabConfig.py --dryrun
=======
    crab submit -c crabConfig.py --dryrun
>>>>>>> b9b8ce0 (ip resolution)
    # crab submit -c crabConfig.py

done

