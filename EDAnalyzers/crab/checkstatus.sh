#!/bin/bash

# Using find for robustness

# dirs=($(find . -maxdepth 1 -type d -name 'crab_QCD*Run3Summer22_MINIAODSIM_S10M0_ver0' -printf '%f\n'))
# logfile="Status/status_JetHT_2022.log"

# dirs=($(find . -maxdepth 1 -type d -name 'crab_QCD*Run3Summer22EE_MINIAODSIM_S10M0_ver0' -printf '%f\n'))
# logfile="Status/status_JetHT_2022EE.log"

# dirs=($(find . -maxdepth 1 -type d -name 'crab_QCD*Run3Summer23_MINIAODSIM_S10M0_ver0' -printf '%f\n'))
# logfile="Status/status_JetHT_2023.log"

# dirs=($(find . -maxdepth 1 -type d -name 'crab_QCD*Run3Summer23BPix_MINIAODSIM_S10M0_ver0' -printf '%f\n'))
# logfile="Status/status_JetHT_2023BPix.log"

# dirs=($(find . -maxdepth 1 -type d -name 'crab_Jet*_Run2022*M0_ver0' -printf '%f\n'))
# logfile="Status/status_JetHT_2022data.log"

dirs=($(find . -maxdepth 1 -type d -name 'crab_Jet*_Run2023*M0_ver0' -printf '%f\n'))
logfile="Status/status_JetHT_2023data.log"

{
    for dir in "${dirs[@]}"; do
        crab status -d "$dir"
    done
}&> $logfile