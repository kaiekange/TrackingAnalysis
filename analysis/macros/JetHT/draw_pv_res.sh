#!/bin/bash

era=$1
itrig=$2

trigHT=( "1050" "890" "780" "680" "590" "510" "430" "370" "250" "180" )

root -l -b -q "draw_pv_res.cc(\"${era}\", \"${trigHT[$itrig]}\")"
