#!/bin/bash

# Using find for robustness
dirs=($(find . -maxdepth 1 -type d -name 'crab_*' -printf '%f\n'))

# Loop to verify
for dir in "${dirs[@]}"; do
    crab resubmit -d "$dir"
done
