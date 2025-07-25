#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

datasetlist="mc.txt"
configtemplate="crabConfigTemplate.py"
ver="Track-v20250717"
<<<<<<< HEAD
prodv="/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
=======
prodv="/store/user/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
>>>>>>> b9b8ce0 (ip resolution)
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
    content=${content//_*}
    era_pro=${split[1]}
    era_pro=${era_pro//-*}
    format=${split[2]}

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
    crab submit -c crabConfig.py --dryrun
    # crab submit -c crabConfig.py
=======
    # crab submit -c crabConfig.py --dryrun
    crab submit -c crabConfig.py
>>>>>>> b9b8ce0 (ip resolution)

done

