#!/bin/bash

source /cvmfs/cms.cern.ch/crab3/crab.sh

SAMPLETYPE="ZeroBias"
# SAMPLETYPE="JetHT"

##2022 preEE
# datasetlist="DatasetList/mc_2022_ZeroBias_preEE.txt"
# GLOBALTAG="132X_mcRun3_2022_realistic_v3"
# datasetlist="DatasetList/mc_2022_JetHT_preEE.txt"
# GLOBALTAG="130X_mcRun3_2022_realistic_v5"

##2022 postEE
# datasetlist="DatasetList/mc_2022_ZeroBias_postEE.txt"
# GLOBALTAG="132X_mcRun3_2022_realistic_postEE_v4"
# datasetlist="DatasetList/mc_2022_JetHT_postEE.txt"
# GLOBALTAG="130X_mcRun3_2022_realistic_postEE_v6"

##2023 preBPix
# datasetlist="DatasetList/mc_2023_ZeroBias_preBPix.txt"
# datasetlist="DatasetList/mc_2023_JetHT_preBPix.txt"
# GLOBALTAG="130X_mcRun3_2023_realistic_v14"

##2023 postBPix
# datasetlist="DatasetList/mc_2023_ZeroBias_postBPix.txt"
# datasetlist="DatasetList/mc_2023_JetHT_postBPix.txt"
# GLOBALTAG="130X_mcRun3_2023_realistic_postBPix_v2"

##2024
datasetlist="DatasetList/mc_2024_ZeroBias.txt"
# datasetlist="DatasetList/mc_2024_JetHT.txt"
GLOBALTAG="150X_mcRun3_2024_realistic_v2"

configtemplate="crabConfigTemplate.py"
ver="Track-v20260113"
prodv="/store/group/phys_tracking/kakang/Run3TrackingAnalysis/Ntuple/${ver}"
pver="0"
EVENTSCALE=10

datasets=()
while read -r name; do
  	if [[ ${name:0:1} == '#' ]]; then
		continue
  	fi
  	datasets+=("$name")
done < "$datasetlist"

for i in ${!datasets[@]}; do

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
    
    # for EVENTMODULO in 0 1 2 3 4; do
	for EVENTMODULO in 0; do
      
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

		crab submit -c crabConfig.py --dryrun
		# crab submit -c crabConfig.py
    done 

done
