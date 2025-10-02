#!/bin/bash

# Using find for robustness
dirs=($(find . -maxdepth 1 -type d -name 'crab_QCD*ver1' -printf '%f\n'))

# Loop to verify
for dir in "${dirs[@]}"; do
    crab status -d "$dir"
done
