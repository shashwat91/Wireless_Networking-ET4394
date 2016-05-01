#!/usr/bin/env python2
import os
import subprocess

payload = [1472,1000,500]

#iteration for mobility method for constant values

 for t in range(1,4):
	    z='./waf --run "scratch/main_code --nStanodes=20 --frequency=5 --simulationTime=10 --payloadSize=1472 --DataModevalue=3 --MobilityMethod=' + str(t)
	    print z
	    direct_output = subprocess.check_output(z, shell=True)