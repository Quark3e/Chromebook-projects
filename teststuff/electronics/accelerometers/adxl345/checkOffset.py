#!/usr/bin/env python3

import time
import sys
import os
import os.path

absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("checkOffset.py")])
sys.path.append(absPath[:len(absPath)-len("teststuff/electronics/accelerometers/adxl345/")]+"projects/")

import proj_Hexclaw.IK_module as IK #type: ignore


def main():
    wifiOrient = IK.nodemcuOrient()
    
    while True:
        wifiOrient.readAccelerometer()
        time.sleep(0.01)

if __name__=="__main__":
    main()