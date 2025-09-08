#!/bin/bash

# condor_submit mkfile.sub Era=0 JobName=2022_data_B NumJobs=233
# condor_submit mkfile.sub Era=1 JobName=2022_data_C NumJobs=954
# condor_submit mkfile.sub Era=2 JobName=2022_data_D NumJobs=540
# condor_submit mkfile.sub Era=3 JobName=2022_data_E0 NumJobs=1000
# condor_submit mkfile.sub Era=4 JobName=2022_data_E1 NumJobs=1000
# condor_submit mkfile.sub Era=5 JobName=2022_data_E2 NumJobs=187
condor_submit mkfile.sub Era=6 JobName=2022_mc_50to80_preEE NumJobs=39
condor_submit mkfile.sub Era=7 JobName=2022_mc_80to120_preEE NumJobs=22
condor_submit mkfile.sub Era=8 JobName=2022_mc_120to170_preEE NumJobs=30
condor_submit mkfile.sub Era=9 JobName=2022_mc_170to300_preEE NumJobs=20
condor_submit mkfile.sub Era=10 JobName=2022_mc_300to470_preEE NumJobs=27
condor_submit mkfile.sub Era=11 JobName=2022_mc_470to600_preEE NumJobs=28
condor_submit mkfile.sub Era=12 JobName=2022_mc_600to800_preEE NumJobs=187
condor_submit mkfile.sub Era=13 JobName=2022_mc_800to1000_preEE NumJobs=120
condor_submit mkfile.sub Era=14 JobName=2022_mc_1000to1400_preEE NumJobs=76
condor_submit mkfile.sub Era=15 JobName=2022_mc_1400to1800_preEE NumJobs=38
condor_submit mkfile.sub Era=16 JobName=2022_mc_1800to2400_preEE NumJobs=24
condor_submit mkfile.sub Era=17 JobName=2022_mc_2400to3200_preEE NumJobs=8
condor_submit mkfile.sub Era=18 JobName=2022_mc_3200_preEE NumJobs=7
condor_submit mkfile.sub Era=19 JobName=2022_mc_50to80_postEE NumJobs=241
condor_submit mkfile.sub Era=20 JobName=2022_mc_80to120_postEE NumJobs=387
condor_submit mkfile.sub Era=21 JobName=2022_mc_120to170_postEE NumJobs=250
condor_submit mkfile.sub Era=22 JobName=2022_mc_170to300_postEE NumJobs=228
condor_submit mkfile.sub Era=23 JobName=2022_mc_300to470_postEE NumJobs=463
condor_submit mkfile.sub Era=24 JobName=2022_mc_470to600_postEE NumJobs=417
condor_submit mkfile.sub Era=25 JobName=2022_mc_600to800_postEE NumJobs=685
condor_submit mkfile.sub Era=26 JobName=2022_mc_800to1000_postEE NumJobs=392
condor_submit mkfile.sub Era=27 JobName=2022_mc_1000to1400_postEE NumJobs=201
condor_submit mkfile.sub Era=28 JobName=2022_mc_1400to1800_postEE NumJobs=114
condor_submit mkfile.sub Era=29 JobName=2022_mc_1800to2400_postEE NumJobs=67
condor_submit mkfile.sub Era=30 JobName=2022_mc_2400to3200_postEE NumJobs=21
condor_submit mkfile.sub Era=31 JobName=2022_mc_3200_postEE NumJobs=13
