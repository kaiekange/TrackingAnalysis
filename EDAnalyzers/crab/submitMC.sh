#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

SAMPLETYPE="ZeroBias"
# SAMPLETYPE="JetHT"

# YEAR="2022"
# GLOBALTAG="132X_mcRun3_2022_realistic_v3"
# GLOBALTAG="130X_mcRun3_2022_realistic_v5"

YEAR="2022EE"
GLOBALTAG="132X_mcRun3_2022_realistic_postEE_v4"
# GLOBALTAG="130X_mcRun3_2022_realistic_postEE_v6"

# YEAR="2023"
# GLOBALTAG="130X_mcRun3_2023_realistic_v14"

# YEAR="2023BPix"
# GLOBALTAG="130X_mcRun3_2023_realistic_postBPix_v2"

# YEAR="2024"
# GLOBALTAG="150X_mcRun3_2024_realistic_v2"

datasetlist="DatasetList/mc_${YEAR}_${SAMPLETYPE}.txt"

configtemplate="crabConfigTemplateMC.py"
ver="Track-v20260210"
prodv="/store/group/phys_tracking/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
pver="0"
EVENTSCALE=1
EVENTMODULO=0

datasets=()
while read -r name; do
  	if [[ ${name:0:1} == '#' ]]; then
		continue
  	fi
  	datasets+=("$name")
done < "$datasetlist"

for i in "${!datasets[@]}"; do

    dataset=${datasets[i]}

    tmpdataset=${dataset#/}
    IFS='/' read -r -a split <<< "$tmpdataset"

    content=${split[0]}
    content=$(echo "$content" | cut -d'_' -f1-3)

    era_pro=${split[1]}
    era_pro=${era_pro%%MiniAOD*}

    format=${split[2]}

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
		> "crabConfig.py"

	# crab submit -c crabConfig.py --dryrun
	crab submit -c crabConfig.py

done
