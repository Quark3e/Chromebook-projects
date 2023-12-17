#!/usr/bin/env python3

#/*
# Haar Cascade
# "Automatic" Mode where the coordinate and orientation variables are given by opencv HaarCascade hand tracking
#*/

# Import essential libraries
from __future__ import print_function
import argparse
import requests
import cv2 #type: ignore
import numpy as np
import imutils
import time
import math
import adafruit_adxl34x # type: ignore
import sys

sys.path.append('/home/pi/Chromebook-projects/projects/proj_Hexclaw')

from IK_module import sendToServo, toRadians, toDegrees, getAngles


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


def getServo4Offset(degrees):
    return 90-(90/130)*degrees

servo[0].angle = 90
servo[1].angle = 15
servo[2].angle = 150
servo[3].angle = 90
servo[4].angle = 180
servo[5].angle = 90

time.sleep(1)

axisFilter = 0.7 #On the new value end

newSize = [640, 480]
xScaling, yScaling, zScaling = 0.8, 0.8, 1.2
cap = cv2.VideoCapture(0)
brightVal = 75
zDefaultVal = 100000
#L_values, U_values = input("Enter L- and U-values without comma: "), input()
diffCheck = 100
showImage = False
globalPrint = False
printCoord = True
endAnglePrint = False
firstAnglePrint = False
posOption = '-'
if showImage:
    cv2.namedWindow("Windows")

X_out, Y_out, Z_out = accelerometer.acceleration #acceleration values for each axis
Roll, Pitch = 0.1, 0.1


q = [0]*6 #NOTE: q = q[0] = servo[0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos

def readAccelerometer():
    global X_out, Y_out, Z_out, Roll, Pitch, roll, pitch
    X_out, Y_out, Z_out = accelerometer.acceleration
    #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = math.atan(Y_out / math.sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    roll = math.atan(-1 * X_out / math.sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    #filter
    Roll = 0.8 * Roll + 0.2 * roll
    Pitch = 0.8 * Pitch + 0.2 * pitch


cX, cY, cW, cH = 0, 0, 0, 0

frame_gray = 0
readObjects = False

def detectAndDisplay(frame):
    global cX, cY, cW, cH, frame_gray, readObjects
    frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    frame_gray = cv2.equalizeHist(frame_gray)
    #-- Detect objects
    objects = object_cascade.detectMultiScale(frame_gray, 1.5, 1, 0, [int(round(newSize[0]/3)), int(round(newSize[0]/3))], [int(round(newSize[0]*0.4)), int(round(newSize[1]*0.4))])
    for (x,y,w,h) in objects:
        readObjects = True
        center = (x + w//2, y + h//2)
        # print("x:", x+w//2, " y:", y+h//2, " width:", w, " height:", h, " area:", w*h, sep='')
        print(" area:", w*h, " w:", w, " h:", h, sep='', end='')
        frame = cv2.ellipse(frame, center, (w//2, h//2), 0, 0, 360, (255, 0, 255), 4)
        cX, cY, cW, cH = x, y, w, h
        break
    cv2.imshow('Capture - Face detection', frame)

haarCascadePath = "/home/pi/OpenCV/haarcascades/"
parser = argparse.ArgumentParser(description='Code for Cascade Classifier tutorial.')
parser.add_argument('--object_cascade', help='Path to face cascade.', default= haarCascadePath + 'Hand.Cascade.1.xml')
parser.add_argument('--camera', help='Camera divide number.', type=int, default=0)
args = parser.parse_args()
object_cascade_name = args.object_cascade
object_cascade = cv2.CascadeClassifier()
if not object_cascade.load(cv2.samples.findFile(object_cascade_name)):
    print('--(!)Error loading face cascade')
    exit(0)


zMax = 300
a, b, Y = 90, 0.1, 90
PP = [float(0)]*3
coord = ""

while True:
    #get accelerations, roll and pitch
    readAccelerometer()

    ret, imgTemp = cap.read()
    img = cv2.resize(imgTemp, (newSize[0], newSize[1]))
    img = cv2.flip(img, 1)

    detectAndDisplay(img)

    coord = "x:" + str(int(xScaling*(cX - newSize[0]*0.5))) + " y:" + str(int(yScaling*(newSize[1] - cY))) + " z:" +  str(int(zScaling*(zMax - ((cW*cH) / zDefaultVal)*zMax))) #str(cv2.contourArea(contours))
    PP[0] = int(axisFilter * xScaling*(cX - newSize[0]*0.5) + (1-axisFilter) * PP[0])
    PP[1] = int(axisFilter * yScaling*(newSize[1] - cY) + (1-axisFilter) * PP[1])
    PP[2] = int(axisFilter * zScaling*(zMax - ((cW*cH) / zDefaultVal)*zMax) + (1-axisFilter) * PP[2])
    if globalPrint or printCoord: print(coord)
    if showImage:
        stacked = np.hstack((img, frame_gray))
        cv2.imshow('Windows', cv2.resize(stacked, None, fx=0.7, fy=0.7)) # type: ignore
    
    # Calculate the servo rotations
    bPos = False
    if Pitch <= 90 and Pitch >= -90:
        b = toRadians(Pitch * 0.9 + toDegrees(b) * 0.1)
        if b < 0: bPos = False
        if b > 0: bPos = True
    if Roll <= 90 and Roll >= -90:
        if not bPos: a = toRadians(0 - (Roll * 0.9 + toDegrees(a) * 0.1))
        elif bPos: a = toRadians(Roll * 0.9 + toDegrees(a) * 0.1)
    q = getAngles(PP,a,b,Y,'-')

    servoExceeded = False
    # "under" = given < 0
    # "over" = given < 180
    whichServoExceeded = 6*[False]
    typeOfExceeded = 6*["null"]

    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)

    # Press Esc key to exit
    if cv2.waitKey(1) == 27:
        break
    # plt.show()
        
if showImage:
    cv2.destroyAllWindows()
pca.deinit()

