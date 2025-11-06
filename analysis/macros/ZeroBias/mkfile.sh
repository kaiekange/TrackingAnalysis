#!/bin/bash

period=$1
dataset=$2
infilepath=$3
process=$(( $4 + 1 ))

infile="${infilepath}/output_${process}.root"
mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/ZeroBias/tuples/${period}/${dataset}"
outfile="/pnfs/iihe/cms/store/user/kakang/IPres/analysis/ZeroBias/tuples/${period}/${dataset}/skimmed_${process}.root"
mkdir -p "/pnfs/iihe/cms/store/user/kakang/IPres/analysis/ZeroBias/logs/${period}/mkfile/${dataset}"
logfile="/pnfs/iihe/cms/store/user/kakang/IPres/analysis/ZeroBias/logs/${period}/mkfile/${dataset}/skimmed_${process}.log"

if [[ ! -f "${infile}" ]]; then
    rm -f "${logfile}" "${outfile}"
    echo "${period}, ${dataset}, file ${process} doesn't exist" &>> mkfile.log
else
    if [[ ! -f "${logfile}"  ||  ! -f "${outfile}" ]]; then
        rm -f "${logfile}" "${outfile}"
        root -l -b -q "mkfile.cc(\"${infile}\", \"${outfile}\")" &> ${logfile}
    fi
fi
