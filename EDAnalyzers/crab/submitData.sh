#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

SAMPLETYPE="ZeroBias"
# SAMPLETYPE="JetHT"

# CMSSW_13_0_13: the global tag is given per dataset in the dataset list (2nd column)
YEAR="2023BPix"  # one of: 2022 2022EE 2023 2023BPix

# golden JSON per YEAR (2022-2024: official versions after the TrkML2026 review)
case "${YEAR}" in
    2022|2022EE) LUMIMASK="JSON/Cert_Collisions2022_355100_362760_Golden.json" ;;
    2023|2023BPix) LUMIMASK="JSON/Cert_Collisions2023_366442_370790_Golden.json" ;;
    *) echo "ERROR: unknown YEAR ${YEAR}" >&2; exit 1 ;;
esac

datasetlist="DatasetList/data_${YEAR}_${SAMPLETYPE}.txt"

configtemplate="crabConfigTemplateData.py"
ver="Track-v20261122"
# prodv="/store/group/phys_tracking/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
prodv="/store/user/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
pver="0"
EVENTSCALE=0
EVENTMODULO=0
UNITSPERJOB=150  # lumis per job; ~100 for JetMET, ~150 for ZeroBias at EventScale=0

datasets=()
globaltags=()
while read -r name gt; do
  if [[ -z ${name} || ${name:0:1} == '#' ]]; then
    continue
  fi
  if [[ -z ${gt} ]]; then
    echo "ERROR: no global tag for ${name} in ${datasetlist}" >&2
    exit 1
  fi
  datasets+=("$name")
  globaltags+=("$gt")
done < "$datasetlist"

for i in "${!datasets[@]}"; do

    dataset=${datasets[i]}
    GLOBALTAG=${globaltags[i]}

    split=(${dataset//\// })
    content=${split[0]}
    format=${split[2]}
    era_pro=$(echo ${split[1]} | sed "s|-|_|g")

	INPUTDATASET=${dataset}
	OUTLFN="${prodv}"

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
        | sed "s|SAMPLETYPE|${SAMPLETYPE}|g" \
        | sed "s|LUMIMASK|${LUMIMASK}|g" \
        | sed "s|UNITSPERJOB|${UNITSPERJOB}|g" \
        > "crabConfig.py"

    # crab submit -c crabConfig.py --dryrun
    crab submit -c crabConfig.py

done
