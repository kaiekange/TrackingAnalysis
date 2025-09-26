#!/bin/bash

condor_submit PU_factor.sub Era=preEE
condor_submit PU_factor.sub Era=postEE
