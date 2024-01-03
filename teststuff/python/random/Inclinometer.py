#!/usr/bin/env python3

import numpy as np
import time
import sys
import os
import os.path
absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("Inclinometer.py")])
sys.path.append(absPath[:absPath.find("teststuff")])

# import projects.proj_Hexclaw.IK_module as IK #type: ignore
from teststuff.python.modules.nodemcu.main_h import *

def main():
    wifiOrient = espOrient()
    
    while True:
        wifiOrient.readAccelerometer()
        time.sleep(0.01)

if __name__=="__main__":
    main()