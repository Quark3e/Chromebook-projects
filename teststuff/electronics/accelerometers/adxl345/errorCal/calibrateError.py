#!/usr/bin/env python3

import time
import sys
import os
import os.path
from datetime import datetime

absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("calibrateError.py")])
IK_path = absPath[:len(absPath)-len("teststuff/electronics/accelerometers/adxl345/errorCal/")]+"projects/"
print(IK_path)
sys.path.append(absPath[:len(absPath)-len("teststuff/electronics/accelerometers/adxl345/errorCal/")]+"projects/")

import proj_Hexclaw.IK_module as IK #type: ignore


PON = lambda x: x/abs(x)

offsets = {
    "x+": [0, None,  1],
    "x-": [0, None, -1],
    "y+": [1, None,  1],
    "y-": [1, None, -1],
    "z+": [2, None,  1],
    "z-": [2, None, -1]
}

def takeReading(wifiOrient):
    readVal = None
    try:
        while True:
            wifiOrient.readAccelerometer(True)
            readVal = wifiOrient.axis_gRaw.copy()
    except KeyboardInterrupt:
        print(f"\n captured:", readVal)
        time.sleep(0.2)
    return readVal
def main():
    wifiOrient = IK.espOrient(emptyTest="pi" not in absPath)
    wifiOrient.client_socket.settimeout(1)
    print("\nStarting calibration testing:",10*"-")
    for key,val in offsets.items():
        print(20*"-")
        print(f"testing: [{key}]")
        reading = takeReading(wifiOrient)[val[0]]
        offsets[key][1] = reading-val[2]
        time.sleep(1)
    print("\nFinished:", 31*"-")
    grouped = {}
    for key,val in offsets.items():
        if key[1]=="+": grouped.update({key[0]: [round(val[1], 2), None, None]})
        else:
            grouped[key[0]][1] = round(val[1], 2)
            grouped[key[0]][2] = round((grouped[key[0]][0]+val[1])/2, 4)
    print(f"\nReadings:{31*'-'}")
    print(f"      axis|  ax.+|  ax.-|  average")
    for key,val in grouped.items():
        print(f"\t{key:<2}| {val[0]:<5}| {val[1]:<5}| {val[2]:<7}") #4+2+2+4+2+4+2+6=36
    print("")

if __name__=="__main__":
    main()