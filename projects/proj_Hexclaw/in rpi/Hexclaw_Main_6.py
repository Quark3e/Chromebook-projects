#!/usr/bin/env python3

#/*
# Give XYZ position line by line in terminal
# */

# Import essential libraries
import requests
import numpy as np
import imutils
import time
import math
import adafruit_adxl34x # type: ignore
import sys
import cv2
import os
import RPi.GPIO as GPIO #type: ignore

sys.path.append('/home/pi/Chromebook-projects/projects/proj_Hexclaw')

from IK_module import *
from h2_module import *

from board import SCL, SDA # type: ignore
import busio # type: ignore

from adafruit_motor import servo # type: ignore
from adafruit_servokit import ServoKit # type: ignore
from adafruit_pca9685 import PCA9685 # type: ignore

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

for i in range(6):
    servo[i].set_pulse_width_range(500, 2500)

servo[0].angle = 108
servo[1].angle = 15
servo[2].angle = 180 - 25
servo[3].angle = 45
servo[4].angle = 180 - 0
servo[5].angle = 90
time.sleep(1)

GPIO.setmode(GPIO.BCM) # GPIO Numbers instead of board numbers
 
ledRelay = 23
GPIO.setup(ledRelay, GPIO.OUT) # GPIO Assign mode
GPIO.output(ledRelay, GPIO.LOW) # out
GPIO.output(ledRelay, GPIO.HIGH) # on

time.sleep(0.75)

sendToServo(servo,[90,115,145,90,125,90],0,mode=2)


time.sleep(2)
for _ in range(4):
    GPIO.output(ledRelay, False)
    time.sleep(0.03)
    GPIO.output(ledRelay, True)
    time.sleep(0.03)
time.sleep(1.5)
GPIO.output(ledRelay, False)
time.sleep(0.25)
GPIO.output(ledRelay, True)
time.sleep(0.5)
GPIO.output(ledRelay, False)
time.sleep(0.1)
GPIO.output(ledRelay, True)
time.sleep(2)
GPIO.output(ledRelay, False)



print("------")
time.sleep(2)

GPIO.output(ledRelay, True)

diagnostics = True


axisFilter = 0.7 #On the new value end
xScaling, yScaling, zScaling = 0.8, 0.8, 1.2
brightVal = 75
diffCheck = 100
showImage = False
globalPrint = True
endAnglePrint = False
firstAnglePrint = False
posOption = '-'

q = [0]*6 #NOTE: q = q[0] = servo[0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos

zMax = 300
a, b, Y = toRadians(0), toRadians(-45), toRadians(90)
PP = [0, 200, 200]
coord = ""


drawing = False # true if mouse is pressed
buttonPressed = False
x1,y1,x2,y2 = -1,-1,-1,-1
windowRes = (600,300)


def main():
    global PP, a, b, Y

    # os.system("clear")
    PP = [0, 200, 200]
    posSequence =[]
    isReachable = [True]
    print(" Enter coordinates [x y z] in mm:")
    while True:
        tempInput_1 = input(">> ")
        if tempInput_1 == "exit": return
        elif tempInput_1 == "end": break
        else: posSequence.append(getNumFromString(tempInput_1, " "))


    for pos in posSequence:
        orient = findOrients(pos)
        if orient != None:
            print("pos: {:18}")
            q = getAngles(pos,toRadians(orient[0]),toRadians(orient[1]),toRadians(orient[2]),'-')
            sendToServo(servo, [toDegrees(joint) for joint in q], 0, useDefault=True, mode=2)
        # input("\npaused. Press enter to continue...")


if __name__ == "__main__":
    main()
    sendToServo(servo,[135,15,155,45,180,90],1,mode=2)
    GPIO.output(ledRelay, False)
    pca.deinit()

