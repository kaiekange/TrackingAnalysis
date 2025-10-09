#!/bin/bash

era=$1
iPT=$2
PTranges=(
    "50to80"
    "80to120"
    "120to170"
    "170to300"
    "300to470"
    "470to600"
    "600to800"
    "800to1000"
    "1000to1400"
    "1400to1800"
    "1800to2400"
    "2400to3200"
    "3200"
)

root -l -b -q "XsecPSweight.cc(\"${era}\", \"${PTranges[$iPT]}\", ${iPT})"
