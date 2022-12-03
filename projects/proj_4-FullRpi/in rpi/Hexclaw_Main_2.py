#!/usr/bin/env python3

#/*
# Terminal User input
# Manual Mode where the coordinate and orientation variables are input
# Doesn't use accelerometer
#*/

# Import essential libraries
import requests
import numpy as np
import imutils
import time
import math
import adafruit_adxl34x # type: ignore
import sys
from IK_module import sendToServo, correctionSetup, toDegrees, toRadians, getAngles

from board import SCL, SDA # type: ignore
import busio # type: ignore

from adafruit_motor import servo # type: ignore
from adafruit_servokit import ServoKit # type: ignore
from adafruit_pca9685 import PCA9685 # type: ignore

i2c = busio.I2C(SCL, SDA)
pca = PCA9685(i2c)
accelerometer = adafruit_adxl34x.ADXL345(i2c)

pca.frequency = 50

servo = [servo.Servo(pca.channels[0]),
         servo.Servo(pca.channels[1]),
         servo.Servo(pca.channels[2]),
         servo.Servo(pca.channels[3]),
         servo.Servo(pca.channels[4]),
         servo.Servo(pca.channels[5])]
for i in range(6):
    servo[i].set_pulse_width_range(500, 2500)

servo[0].angle = 90
servo[1].angle = 45
servo[2].angle = 180 - 45
servo[3].angle = 90
servo[4].angle = 180 - 180
servo[5].angle = 90

def getServo4Offset(degrees):
    return 90-(90/130)*degrees
#0-90 is inaccurate where the actual angle is 130 for the given 90 degrees
#Note: Only use this function if the rotation value is 

#servo[0].angle = 0
time.sleep(0.5)

diagnostics = True


correctionSetup()

servo[0].angle = 90
servo[4].angle = 135
#time.sleep(1)
servo[2].angle = 180 - 0
servo[1].angle = 90
#time.sleep(1)


axisFilter = 0.7 #On the new value end
xScaling, yScaling, zScaling = 0.8, 0.8, 1.2
brightVal = 75
zDefaultVal = 3000000
L_values, U_values = [36, 72, 8], [86, 240, 204]
#L_values, U_values = input("Enter L- and U-values without comma: "), input()
diffCheck = 100
showImage = False
globalPrint = True
endAnglePrint = False
firstAnglePrint = False
posOption = '-'

X_out, Y_out, Z_out = accelerometer.acceleration #acceleration values for each axis
Roll, Pitch = 0.1, 0.1

q = [0]*6 #NOTE: q = q[0] = servo[0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos

def toDegrees(radians):
    return (radians * 180) / math.pi
def toRadians(degrees):
    return (degrees * math.pi) / 180


def readAccelerometer():
    global X_out, Y_out, Z_out, Roll, Pitch, roll, pitch
    X_out, Y_out, Z_out = accelerometer.acceleration
    #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = math.atan(Y_out / math.sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    roll = math.atan(-1 * X_out / math.sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    #filter
    Roll = 0.8 * Roll + 0.2 * roll
    Pitch = 0.8 * Pitch + 0.2 * pitch



zMax = 300
a, b, Y = toRadians(0), toRadians(-45), toRadians(90)
PP = [0]*3
coord = ""

while True:    
    tempInput_1 = input("Enter coordinates [x y z] in mm: ").split()
    if tempInput_1[0] == "exit":
        sys.exit()
    else:
        PP[0] = (float(tempInput_1[0])) # type: ignore
        PP[1] = (float(tempInput_1[1])) # type: ignore
        PP[2] = (float(tempInput_1[2])) # type: ignore
    tempInput_2 = input("Enter orientation values [a b Y] in degrees: ").split()
    a = toRadians(float(tempInput_2[0]))
    b = toRadians(float(tempInput_2[1]))
    Y = toRadians(float(tempInput_2[2]))

    print()
    if diagnostics:
        print("x:", PP[0], " y:", PP[1], " z:", PP[2], " a:", toDegrees(a), " b:", toDegrees(b), " Y:", toDegrees(Y), sep='')
    q = getAngles(PP,a,b,Y,'-')
        

    # "under" = given < 0
    # "over" = given < 180
    servoExceeded = False
    whichServoExceeded = 6*[False]
    typeOfExceeded = 6*["null"]

    sendToServo(q,s,servo,servoExceeded,whichServoExceeded,typeOfExceeded)

pca.deinit()

