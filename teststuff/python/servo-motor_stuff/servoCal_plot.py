#!/usr/bin/env python3

#/*
# A script to "attempt" fixing every servo motors
#  error rate by plotting the values on a graph.
# */

servoToTest = [1, 2] #servo[] index's of the ones to test

import numpy as np
import time
import math
import os
import matplotlib.pyplot as plt
from datetime import datetime
import adafruit_adxl34x

from board import SCL, SDA #type: ignore
import busio #type: ignore
from adafruit_motor import servo #type: ignore
from adafruit_servokit import ServoKit #type: ignore
from adafruit_pca9685 import PCA9685 #type: ignore

from IK_module import sendToServo, toDegrees, toRadians, getAngles


i2c = busio.I2C(SCL, SDA)
pca = PCA9685(i2c)
pca.frequency = 50

accelerometer = adafruit_adxl34x.ADXL345(i2c)

X_out, Y_out, Z_out = accelerometer.acceleration
Roll, Pitch, roll, pitch = 0.1, 0.1, 0.1, 0.1
tiltFilter = 0.1


def readAccelerometer():
    global X_out, Y_out, Z_out, Roll, Pitch, roll, pitch
    X_out, Y_out, Z_out = accelerometer.acceleration
    #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = math.atan(Y_out / math.sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    roll = math.atan(-1 * X_out / math.sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    #filter
    Roll = (1-tiltFilter) * Roll + tiltFilter * roll
    Pitch = (1-tiltFilter) * Pitch + tiltFilter * pitch


servo = [servo.Servo(pca.channels[0]),
         servo.Servo(pca.channels[1]),
         servo.Servo(pca.channels[2]),
         servo.Servo(pca.channels[3]),
         servo.Servo(pca.channels[4]),
         servo.Servo(pca.channels[5])]
for i in range(6): servo[i].set_pulse_width_range(500, 2500)

correctionFile = open("servoCorrections.dat", "a")

sDefault = [90,55,180-0,90,180-80,90]

servo[0].angle = sDefault[0]
servo[1].angle = sDefault[1]
servo[2].angle = sDefault[2]
servo[3].angle = sDefault[3]
servo[4].angle = sDefault[4]
servo[5].angle = sDefault[5]
time.sleep(1)

print(f"testing servos: {servoToTest}")
time.sleep(0.5)
for s in servoToTest:
    print(f"- q:[{s}]")
    for a in range(181):
        servo[s].angle = a
        time.sleep(0.02)
    time.sleep(1)
    for a in range(180, -1, -1):
        servo[s].angle = a
    time.sleep(2)


sPrep = [ #is sent AFTER that angle is finished, so loop[q] and then sPrep[q], so sPrep[q] before loop[q]
    [0,90,45,90,90,90],
    [90,0,45,90,90,90],
    [90,90,0,90,90,90],
    [90,90,135,0,90,90],
    [90,90,135,90,0,90],
    [90,90,135,90,90,0]
]

x_q = 6*[19*[0]]
y_q = 6*[19*[0]]
#_q = 6*[18*[0]], 6 lists each with 18 elements

def configure_plots():
    '''
    Plot read value [y] from real life degree disk on given value [x] in degrees.
    '''
    global ax, fig
    fig, ax = plt.subplots()

    plt.legend()

    ax.set_xlim(0,180)
    ax.set_ylim(-90,90)
    ax.set_aspect('equal',adjustable='box')
    ax.set_xlabel("given value [degrees]")
    ax.set_ylabel("read error rate | read-given [degrees]")
    plt.title("Error rate graph")


def main():
    global correctionFile
    configure_plots()
    for q in servoToTest:
        print(f"Testing servo q[{q}]")
        for l in range(6):
            servo[l].angle = sPrep[q][l]
        # os.system('clear')
        servo[q].angle = 180
        time.sleep(0.5)
        servo[q].angle = 0
        if input(" calibrate degreeDisk (fixate disk 0 with current servo position)\n press space and enter to continue (or write 'exit' to exit)...") == "exit":
            return
        print()
        for x in range(0,19,1):
            servo[q].angle = x*10
            print(" sent angle:",x*10, end='')
            readAccelerometer()
            time.sleep(1)
            inpOpt = pitch
            y_q[q][x] = round(inpOpt)
            x_q[q][x] = x*10

            time.sleep(0.1)

        fitPoly = np.polyfit(x_q[q], y_q[q], 2)
        polyFunc = np.poly1d(fitPoly)
        print(str(polyFunc))

        ax.plot(x_q[q], polyFunc(x_q[q]), label=f"q[{q}] fitPoly.", color="green")
        ax.plot(x_q[q], y_q[q], linestyle='solid',label=f"q[{q}] raw")

        print(y_q)
        time.sleep(0.2)
        print(f"finished testing servo q[{q}]")
        print("-----------------")
        input("")

    plt.show()


    currentDate = str(datetime.now()) + ";"
    toFile = currentDate
    tempDict_read = {}
    for q in range(6): tempDict_read.update({q:y_q[q]})
    toFile += str(tempDict_read) + "\n" #type: ignore
    print("toFile <<",toFile)

    toFile_readable = "{\n"
    for q in range(6): toFile_readable += "\t" + str(y_q) + "\n"
    toFile_readable += "}\n"
    print(toFile_readable)
    toFile += toFile_readable
    correctionFile.write(toFile)


if __name__ == "__main__":
    main()
    correctionFile.close()
    
