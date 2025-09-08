#!/bin/bash

condor_submit mkfile.sub Era=0 JobName=2022_data_B NumJobs=97
condor_submit mkfile.sub Era=1 JobName=2022_data_C NumJobs=518
condor_submit mkfile.sub Era=2 JobName=2022_data_D NumJobs=219
condor_submit mkfile.sub Era=3 JobName=2022_data_E NumJobs=436
condor_submit mkfile.sub Era=4 JobName=2022_mc_preEE NumJobs=94
condor_submit mkfile.sub Era=5 JobName=2022_mc_postEE NumJobs=84
