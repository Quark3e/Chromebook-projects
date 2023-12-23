#!/usr/bin/env python3

import numpy as np
import time
import sys
import os
import os.path
absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("nodemcu_accel.py")])
sys.path.append(absPath[:len(absPath)-len("teststuff/python/random/")]+"projects/")

import proj_Hexclaw.IK_module as IK #type: ignore


def main():
    wifiOrient = IK.nodemcuOrient()
    
    while True:
        wifiOrient.readAccelerometer()
        time.sleep(0.01)

if __name__=="__main__":
    main()