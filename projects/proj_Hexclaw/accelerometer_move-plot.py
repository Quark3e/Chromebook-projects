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

position = [[0], [0], [0]]
pos_Direction = ['+', '+', '+']
axisAccel_minimum = [1, 1, 1] #abs(acceleration) reading must be larger than this value to be used
velocity = [0, 0, 0]

accelFilter = 0.1

accel = [0,0,0]
accel[0], accel[1], accel[2] = accelerometer.acceleration #value in acceleration (NOTE: not certain what unit)
# old_accel = accel.copy()

Roll, Pitch = 0.1, 0.1
offset = [0,0,0]
posOffset_scalar = [1000, 1000, 1000]
posOffset_value = [0, 0, 0]

def calibrateOffsets():
    global offset
    ans = input(" Place the tracker on a stationary place to calibrate\n press 'enter' when ready..")
    if ans == "exit": sys.exit()
    allVal = [[],[],[]]
    print("\nCalibrating: reading", end='')
    for i in range(300):
        readAccelerometer()
        allVal[0].append(accel[0])
        allVal[1].append(accel[1])
        allVal[2].append(accel[2])
        if i==0 or i==100 or i==200: print(".", end='')
        time.sleep(0.01)
    for n in range(3):
        offset[n] = (sum(allVal[n]) / 300) #type: ignore

    print("\nOffsets:")
    print(" X_offset:",offset[0])
    print(" Y_offset:",offset[1])
    print(" Z_offset:",offset[1])

def configure_plots():
    global ax, fig
    plt.ion()
    fig = plt.figure(figsize=(10,8))
    ax = fig.add_subplot(projection='3d')
    plt.title("Accelerometer position 3d plot")
    ax.set_xlim(-250, 250)
    ax.set_ylim(-250, 250)
    ax.set_zlim(0, 500) #type: ignore
    ax.set_aspect('equal',adjustable='box')
    ax.set_xlabel('X - axis')
    ax.set_ylabel('Y - axis')
    ax.set_zlabel('Z - axis') #type: ignore
    ax.view_init(elev=20., azim=145, roll=0) #type: ignore

def getPos(iteration_time):
    global position, pos_Direction
    for axis in range(3):
        # if abs(accel[axis]-offset[axis]) > axisAccel_minimum[axis]:
        position[axis][0] += ((((accel[axis]-offset[axis])*pow(iteration_time, 2)) / 2) + posOffset_value[axis])*posOffset_scalar[axis] #type: ignore
        if accel[axis] > 0: pos_Direction[axis] = '+'
        elif accel[axis] < 0: pos_Direction[axis] = '-'
            # pos_Direction[axis] += str((((accel[axis]-offset[axis])*pow(iteration_time, 2)) / 2) + posOffset_value[axis])

def readAccelerometer(axisAccel=[0,0,0],tilt=[0,0]):
    '''
    Reads ADXL345 accelerometer values (and modifies global variables if not module)
    Args:
        [x, y, z] (mutable) list to get acceleration values for each axis through parameter

        [Pitch, Roll, pitch(raw), roll(raw)] (mutable) list to get tilt values through parameter
    '''
    global accel, Roll, Pitch, roll, pitch
    accel = accelerometer.acceleration
    #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = math.atan(accel[1] / math.sqrt(pow(accel[0], 2) + pow(accel[2], 2))) * 180 / math.pi
    roll = math.atan(-1 * accel[0] / math.sqrt(pow(accel[1], 2) + pow(accel[2], 2))) * 180 / math.pi
    #filter
    Roll = (1-accelFilter) * Roll + accelFilter * roll
    Pitch = (1-accelFilter) * Pitch + accelFilter * pitch
    axisAccel = [accel[0], accel[1], accel[2]]
    tilt = [Pitch, Roll, pitch, roll]


def main():
    global ax, fig, plottedPoint
    calibrateOffsets()
    configure_plots()
    plottedPoint, = ax.plot(position[0],position[1],position[2],'bo', linestyle='solid', label='Position')
    t1 = time.perf_counter()
    while True:
        t2 = time.perf_counter()
        readAccelerometer()
        getPos(t2-t1)
        print("[",end='')
        for _ in range(3):
            print("{:4.1f}".format(accel[_] - offset[_]), end='')
        print("] ",end='')
        print(" {:20}".format(str(position)), "{:4.1f}".format(t2-t1), end='')
        print(" direction:", pos_Direction, end='')
        t1 = time.perf_counter()
        plottedPoint.set_xdata(position[0])
        plottedPoint.set_ydata(position[1])
        # plottedPoint.set_zdata(position[2]) #type: ignore
        plottedPoint.set_3d_properties(position[2]) #type: ignore

        fig.canvas.draw()
        fig.canvas.flush_events()
        print()


if __name__ == "__main__":
    main()