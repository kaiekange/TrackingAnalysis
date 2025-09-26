#!/bin/bash

condor_submit splitbin.sub Era=preEE
condor_submit splitbin.sub Era=postEE
