#!/usr/bin/env python3


#/*
# Measures time from servo[n].angle being sent to that command being resolved
#   - without power to servo motors
#   - with power to servo motors
# -from angles 0-180 [1]; save the results in an
#
# x: angle
# y_1: delay without power
# y_2: delay with power
# */


import numpy as np
import matplotlib.pyplot as plt
import math
import os
import time

from IK_module import *

from board import SCL, SDA #type: ignore
import busio #type: ignore

from adafruit_motor import servo #type: ignore
# from adafruit_servokit import
from adafruit_pca9685 import PCA9685 #type: ignore

i2c = busio.I2C(SCL, SDA)
pca = PCA9685(i2c)
pca.frequency = 50

servo = [servo.Servo(pca.channels[0]),
         servo.Servo(pca.channels[1]),
         servo.Servo(pca.channels[2]),
         servo.Servo(pca.channels[3]),
         servo.Servo(pca.channels[4]),
         servo.Servo(pca.channels[5]),
         ]

for i in range(6): servo[i].set_pulse_width_range(500, 2500)
servo[0].angle = 108
servo[1].angle = 15
servo[2].angle = 180 - 25
servo[3].angle = 45
servo[4].angle = 180 - 0
servo[5].angle = 90

time.sleep(1)

def configure_plots():
    global ax, fig
    fig = plt.figure(figsize=(11, 6))
    ax = [0,0]
    ax[0] = fig.add_subplot(1,2,1) #type: ignore
    ax[1] = fig.add_subplot(1,2,2) #type: ignore

    ax[0].set_xlim(0,180) #type: ignore
    ax[0].set_ylim(0,2)
    # ax[0].set_aspect('equal',adjustable='box')
    ax[0].set_title("Without power to motors")
    ax[0].set_xlabel("given value [degrees]")
    ax[0].set_ylabel("read delay [milliseconds]")
    ax[0].axhline(y=0, color='black', linestyle='-')

    ax[0].grid()

    ax[1].set_xlim(0,180)
    ax[1].set_ylim(0,2)
    # ax[1].set_aspect('equal',adjustable='box')
    ax[1].set_title("With power to motors")
    ax[1].set_xlabel("given value [degrees]")
    ax[1].set_ylabel("read delay [milliseconds]")
    ax[1].grid()

    plt.title("Delay graph")

def getData(sToTest):
    yValues = []
    servo[sToTest].angle = 90
    for angle in range(180):
        t1 = time.perf_counter()
        servo[sToTest].angle=angle
        t2 = time.perf_counter()
        yValues.append((t2-t1)*1000) 

    return yValues

def main():
    global fig
    configure_plots()

    xValues = [x for x in range(180)]
    #Without power to servo motors
    for joint in range(6):
        # os.system("clear")
        avgList=6*[[]] #avgList[6][10][180] 
        avg=6*[[]]
        print(joint,":",sep='',end=' ')
        for i in range(10):
            avgList[joint].append(
                getData(joint)
            )
        for i in range(180):
            avg[joint].append(sum(
                [lst[i] for lst in avgList[joint]]
            ) /
            len(avgList[joint])) #type:ignore
        print(len(xValues),len(avg[joint]),end=' ')
        ax[0].plot(xValues,avg[joint],linestyle='solid',label=f"q[{joint:1}]") #type: ignore
        print()
    
    time.sleep(2)
    #With power to servo motors


    fig.legend(loc=2)
    relativePath = "/home/pi/Chromebook-projects/projects/proj_Hexclaw/hexclaw_files/"
    # plt.savefig(relativePath+"PCA9685_sendDelay.png")
    plt.show()

    return

if __name__ == "__main__":
    main()