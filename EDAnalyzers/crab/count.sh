#!/bin/bash

dir="/eos/home-k/kakang/Run3TrackingAnalysis/Ntuple/Track-v20251112/SingleNeutrino_Par-E-10_gun/RunIII2024Summer24_MINIAODSIM_S5M0/251112_230233/0000/log"

before=0
after=0

for f in ${dir}/cmsRun_*.log.tar.gz; do
    content=$(tar -xOf "$f" --wildcards '*.log')

    b=$(echo "$content" | grep "before event scale" | awk '{print $NF}')
    before=$((before + b))

    a=$(echo "$content" | grep "after event scale" | awk '{print $NF}')
    after=$((after + a))
done

echo "Total before = $before"
echo "Total after  = $after"