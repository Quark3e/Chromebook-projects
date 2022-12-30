#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import math
import sys
import os
import time

import adafruit_adxl34x #type:ignore
from board import SCL, SDA #type: ignore
import busio #type: ignore

i2c = busio.I2C(SCL, SDA)
accelerometer = adafruit_adxl34x.ADXL345(i2c)

position = [0, 0, 0]
axisAccel_minimum = [0, 0, 0] #abs(acceleration) reading must be larger than this value to be used
velocity = [0, 0, 0]

accelFilter = 0.1

accel = accelerometer.acceleration #value in acceleration (NOTE: not certain what unit)
old_accel = accel.copy()

Roll, Pitch = 0.1, 0.1
X_offset, Y_offset, Z_offset = 0,0,0

def readAccelerometer(axisAccel=[0,0,0],tilt=[0,0]):
    '''
    Reads ADXL345 accelerometer values (and modifies global variables if not module)
    Args:
        [x, y, z] (mutable) list to get acceleration values for each axis through parameter

        [Pitch, Roll, pitch(raw), roll(raw)] (mutable) list to get tilt values through parameter
    '''
    global accel, old_accel, Roll, Pitch, roll, pitch
    old_accel = accel.copy()
    accel = accelerometer.acceleration
    #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = math.atan(accel[1] / math.sqrt(pow(accel[0], 2) + pow(accel[2], 2))) * 180 / math.pi
    roll = math.atan(-1 * accel[0] / math.sqrt(pow(accel[1], 2) + pow(accel[2], 2))) * 180 / math.pi
    #filter
    Roll = (1-accelFilter) * Roll + accelFilter * roll
    Pitch = (1-accelFilter) * Pitch + accelFilter * pitch
    
    axisAccel = [accel[0], accel[1], accel[2]]
    tilt = [Pitch, Roll, pitch, roll]

def calibrateOffsets():
    global X_offset, Y_offset, Z_offset
    ans = input(" Place the tracker on a stationary place to calibrate\n press 'enter' when ready..")
    if ans == "exit": sys.exit()
    allVal = [[],[],[]]
    print("\nCalibrating: reading", end='')
    for i in range(300):
        readAccelerometer()
        allVal[0].append(accel[0])
        allVal[1].append(accel[1])
        allVal[2].append(accel[2])
        if i%100 == 0: print(".", end='')
        time.sleep(0.01)
    X_offset = (sum(allVal[0]) / 300)
    Y_offset = (sum(allVal[1]) / 300)
    Z_offset = (sum(allVal[2]) / 300)
    print("\nOffsets:")
    print(" X_offset:",X_offset)
    print(" Y_offset:",Y_offset)
    print(" Z_offset:",Z_offset)

def getVelocity(iteration_time):
    global velocity
    pos_temp = position.copy()
    for axis in range(3):
        if (accel[axis]-old_accel[axis]) > axisAccel_minimum[axis]:
            velocity+=accel[axis] * iteration_time


def main():
    return


if __name__ == "__main__":
    main()