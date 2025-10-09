#!/bin/bash

# Using find for robustness
# dirs=($(find . -maxdepth 1 -type d -name 'crab_QCD*50to80*S5M*_ver0' -printf '%f\n'))


PTs=(
    "50to80"
    # "80to120"
    "120to170"
    # "170to300"
    "300to470"
    # "470to600"
    # "600to800"
    # "800to1000"
    # "1000to1400"
    # "1400to1800"
    # "1800to2400"
    # "2400to3200"
    # "3200"
)
SMs=( "S5M0" "S5M1" "S5M2" "S5M3" "S5M4" )

# Loop to verify
# for dir in "${dirs[@]}"; do
for PT in "${PTs[@]}"; do
    for SM in "${SMs[@]}"; do
        dir="crab_QCD_PT-${PT}_TuneCP5_Run3Summer22_MINIAODSIM_${SM}_ver0"
        crab status -d "$dir"
        # crab resubmit -d "$dir" --maxjobruntime 3000
        # crab kill -d "$dir"
    done
done
