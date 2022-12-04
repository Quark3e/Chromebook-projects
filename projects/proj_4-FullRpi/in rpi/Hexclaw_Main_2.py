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
import cv2
import os
from IK_module import sendToServo, correctionSetup, toDegrees, toRadians, getAngles

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
         servo.Servo(pca.channels[5])]
for i in range(6):
    servo[i].set_pulse_width_range(500, 2500)

servo[0].angle = 90
servo[1].angle = 45
servo[2].angle = 180 - 45
servo[3].angle = 90
servo[4].angle = 180 - 180
servo[5].angle = 90
time.sleep(0.5)

diagnostics = True

correctionSetup()

servo[0].angle = 90
servo[4].angle = 135
servo[2].angle = 180 - 0
servo[1].angle = 90


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


q = [0]*6 #NOTE: q = q[0] = servo[0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos


zMax = 300
a, b, Y = toRadians(0), toRadians(-45), toRadians(90)
PP = [0]*3
coord = ""


drawing = False # true if mouse is pressed
buttonPressed = False
x1,y1,x2,y2 = -1,-1,-1,-1
windowRes = (600,300)
# mouse callback function
def mouseTrack(event,x,y,flags,param):
    global x1,y1,x2,y2,drawing,buttonPressed,img
    x2,y2 = x,y
    if drawing:
      img = np.zeros((windowRes[1],windowRes[0],3), np.uint8)
    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        x1,y1 = x2,y2
    elif event == cv2.EVENT_MOUSEMOVE:
        buttonPressed = True
        if drawing == True:
            cv2.line(img,(0,y2),(windowRes[0],y2),(255,255,255), 1)
            cv2.line(img,(x2,0),(x2,windowRes[1]),(255,255,255),1)
            cv2.circle(img,(x2,y2),10,(0,0,255),1)
    elif event == cv2.EVENT_LBUTTONUP:
        buttonPressed = False
        drawing = False
        cv2.line(img,(0,y2),(windowRes[0],y2),(255,255,255), 1)
        cv2.line(img,(x2,0),(x2,windowRes[1]),(255,255,255),1)
        cv2.circle(img,(x2,y2),10,(0,0,255),1)
        
img = np.zeros((windowRes[1],windowRes[0],3), np.uint8)
cv2.namedWindow('tracking_window')
cv2.setMouseCallback('tracking_window',mouseTrack)


mode = 0

print(" Different modes for tracking:")
print(" 1. Enter position and orientation in terminal")
print(" 2. Mouse position tracking on window")
option = input(" input: ")
if option == "exit": sys.exit()
else: mode = int(option)

# "under" = given < 0
# "over" = given < 180
servoExceeded = False
whichServoExceeded = 6*[False]
typeOfExceeded = 6*["null"]

while True:
    os.system("clear")
    #Get end-effector/PP position/coordinate
    print(" ---Enter mode_(n) to change mode to (n)--- \n")
    if mode==1:
        tempInput_1 = input("Enter coordinates [x y z] in mm: ").split()
        if tempInput_1[0] == "exit": sys.exit()
        elif tempInput_1[0] == "mode_1": mode=1
        elif tempInput_1[0] == "mode_2": mode=2
        else:
            PP[0] = (float(tempInput_1[0])) # type: ignore
            PP[1] = (float(tempInput_1[1])) # type: ignore
            PP[2] = (float(tempInput_1[2])) # type: ignore
    elif mode==2:
        tempInput_1 = input("Enter z-value in mm: ")
        if tempInput_1 == "exit": sys.exit()
        elif tempInput_1 == "mode_1": mode=1
        elif tempInput_1 == "mode_2": mode=2
        else: PP[2] = float(tempInput_1) # type: ignore

    tempInput_2 = input("Enter orientation values [a b Y] in degrees: ").split()
    a,b,Y = toRadians(float(tempInput_2[0])), toRadians(float(tempInput_2[1])), toRadians(float(tempInput_2[2]))

    if mode==1:
        if diagnostics: print("x:", PP[0], " y:", PP[1], " z:", PP[2], " a:", toDegrees(a), " b:", toDegrees(b), " Y:", toDegrees(Y), sep='')
        q = getAngles(PP,a,b,Y,'-')
        sendToServo(q,s,servo,servoExceeded,whichServoExceeded,typeOfExceeded)
    elif mode==2:
        while True:
            print("\n 'Esc' - change z-value and orientation\n '")
            cv2.imshow('tracking win', img) #use this if the previous drawings are not to be used
            k = cv2.waitKey(1) & 0xFF
            if k == 27: break
            PP[0], PP[1] = x2,y2
            q = getAngles(PP,a,b,Y,'-')
            sendToServo(q,s,servo,servoExceeded,whichServoExceeded,typeOfExceeded)

cv2.destroyAllWindows()
pca.deinit()

