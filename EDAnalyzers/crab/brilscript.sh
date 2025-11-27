#!/bin/bash

source /cvmfs/cms-bril.cern.ch/cms-lumi-pog/brilws-docker/brilws-env

# HTS=( "180" )
# YEARS=( "2022" )

HTS=( "180" "250" "370" "430" "510" "590" "680" "780" "890" "1050" )
YEARS=( "2022" "2023" "2024" )
JSON_FILES=( "./JSON/Cert_Collisions2022_355100_362760_Golden.json" "./JSON/Cert_Collisions2023_366442_370790_Golden.json" "./JSON/Cert_Collisions2024_378981_386951_Golden.json" )

for idx in "${!YEARS[@]}"; do
    year=${YEARS[$idx]}
    json_file=${JSON_FILES[$idx]}

    for HT  in "${HTS[@]}"; do
        LUMI_TXT="./LUMI/lumi_${year}_PFHT${HT}.txt"
        HLTPATH="HLT_PFHT${HT}_v*"

        # rm -f "$LUMI_TXT"
        # singularity -s exec  --env PYTHONPATH=/home/bril/.local/lib/python3.10/site-packages /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/cms-cloud/brilws-docker:latest brilcalc lumi --hltpath "$HLTPATH" -i "$json_file" --output-style csv > "$LUMI_TXT"

        TRG_TXT="./LUMI/prescale_${year}_PFHT${HT}.txt"
        rm -f "$TRG_TXT"

        tail -n +2 "$LUMI_TXT" | grep -v '^#' | while IFS=, read -r runfill time ncms hltpath delivered recorded; do
            # extract run number before ':'
            run=${runfill%%:*}

            if [ ! -f "$LUMI_TXT" ]; then
                singularity -s exec  --env PYTHONPATH=/home/bril/.local/lib/python3.10/site-packages /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/cms-cloud/brilws-docker:latest brilcalc trg --prescale --hltpath "$HLTPATH" -r "$run" --output-style csv > "$TRG_TXT"
            else
                # append, skip header
                singularity -s exec  --env PYTHONPATH=/home/bril/.local/lib/python3.10/site-packages /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/cms-cloud/brilws-docker:latest brilcalc trg --prescale --hltpath "$HLTPATH" -r "$run" --output-style csv | tail -n +2 >> "$TRG_TXT"
            fi
        done


    done

done