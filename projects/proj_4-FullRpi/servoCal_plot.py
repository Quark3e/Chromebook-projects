#!/usr/bin/env python3

#/*
# A script to "attempt" fixing every servo motors
#  error rate by plotting the values on a graph.
# */


import numpy as np
import time
import os
import matplotlib.pyplot as plt

from IK_module import sendToServo, correctionSetup, toDegrees, toRadians, getAngles, custom_sendToServo


from board import SCL, SDA #type: ignore
import busio #type: ignore
from adafruit_motor import servo #type: ignore
from adafruit_servokit import ServoKit #type: ignore
from adafruit_pca9685 import PCA9685 #type: ignore

i2c = busio.I2C(SCL, SDA)
pca = PCA9685(i2c)
pca.frequency = 50

servo = [servo.Servo(pca.channels[0]),
         servo.Servo(pca.channels[1]),
         servo.Servo(pca.channels[2]),
         servo.Servo(pca.channels[3]),
         servo.Servo(pca.channels[4]),
         servo.Servo(pca.channels[5])]
for i in range(6): servo[i].set_pulse_width_range(500, 2500)

sDefault = [90,45,180-135,90,180-0,90]

servo[0].angle = sDefault[0]
servo[1].angle = sDefault[1]
servo[2].angle = sDefault[2]
servo[3].angle = sDefault[3]
servo[4].angle = sDefault[4]
servo[5].angle = sDefault[5]
time.sleep(1)

x_q = 6*[18*[0]]
y_q = 6*[18*[0]]
#_q = 6*[18*[0]], 6 lists each with 18 elements

def configure_plots():
    '''
    Plot read value [y] from real life degree disk on given value [x] in degrees.
    '''
    global ax, fig
    fig = plt.figure()
    fig.legend()
    ax = fig.add_subplot()
    ax.set_xlim(0,180)
    ax.set_ylim(-90,90)
    ax.set_aspect('equal',adjustable='box')
    ax.set_xlabel("given value [degrees]")
    ax.set_ylabel("read error rate | read-given [degrees]")
    plt.title("Error rate graph")


def main():
    configure_plots()
    for q in range(6):
        os.system('clear')
        print("Testing servo:",q)
        servo[q].angle = 180
        time.sleep(1.5)
        servo[q].angle = 0
        input(" calibrate degreeDisk (fixate disk 0 with current servo position)\n press enter to continue...")
        print()
        for x in range(0,18,1):
            servo[q].angle = x*10
            print(" sent angle:",x*10, end='')
            y_q[q][x] = round(float(input(" read angle: "))) - x*10
            x_q[q][x] = x*10
        servo[q].angle = sDefault[q]
        ax.plot(x_q[q],y_q[q],linestyle='solid',label=str(q))
        time.sleep(0.2)

    ax.legend()

    plt.show()


if __name__ == "__main__":
    main()
    