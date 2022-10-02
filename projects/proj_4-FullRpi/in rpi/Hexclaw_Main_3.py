#!/usr/bin/env python3

#/*
# "Automatic" Mode where the coordinate and orientation variables are given by opencv color tracking and accelerometer
#*/

# Import essential libraries
from __future__ import print_function
import argparse
import requests
import cv2
import numpy as np
import imutils
import time
import math
import adafruit_adxl34x
import sys

from board import SCL, SDA
import busio

from adafruit_motor import servo
from adafruit_servokit import ServoKit
from adafruit_pca9685 import PCA9685

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
servo[1].angle = 90
servo[2].angle = 180 - 90
servo[3].angle = 90
servo[4].angle = 180 - 90
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

d1 = 140; #axial "roll"
d2 = 135; #axial "pitch"
d3 = 70; #axial "pitch"
d4 = 80; #axial "roll"
d5 = 45; #axial "pitch
d6 = 45; #axial "roll" (?)

q1, q2, q3, q4, q5, q6, q7 = 0, 0, 0, 0, 0, 0, 0 #NOTE: q1 = servo[0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos

posX2, posY2, posZ2 = 0.01, 0.01, 0.01
a1, b1 = 0.1, 0.1

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

def getAngles(posX, posY, posZ, a, b, Y, posOption, length_scalar = 1, coord_scalar = 1, printText = False):
    global d1, d2, d3, d4, d5, d6, q1, q2, q3, q4, q5, q6, q7, posX2, posY2, posZ2

    posX = posX * coord_scalar
    posY = posY * coord_scalar
    posZ = posZ * coord_scalar
    d1 = d1 * length_scalar
    d2 = d2 * length_scalar
    d3 = d3 * length_scalar
    d4 = d4 * length_scalar
    d5 = d5 * length_scalar
    d6 = d6 * length_scalar
    l = (d5+d6) * math.cos(b)
    posX2 = posX - l * math.sin(a)
    posY2 = posY - l * math.cos(a)
    posZ2 = posZ - (d5+d6) * math.sin(b)
    if posY2 == 0:
        posY2 = 0.00001
    elif posY2 < 0:
        posY2 = 0.00001
    
    q1 = math.atan(posX2 / posY2)
    try:
        if posOption == '+':
            q3 = math.acos((pow(posX2, 2) + pow(posY2, 2) + pow(posZ2 - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /(2 * d2 * (d3 + d4)))
        elif posOption == '-':
            q3 = math.acos((pow(posX2, 2) + pow(posY2, 2) + pow(posZ2 - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /(2 * d2 * (d3 + d4)))
    except ValueError:
        print("domain error triggered")
    lambdaVar = math.atan((posZ2 - d1) / math.sqrt(pow(posX2, 2) + pow(posY2, 2)))
    muVar = math.atan(((d3 + d4) * math.sin(q3)) /(d2 + (d3 + d4) * math.cos(q3)))
    if posOption == '+':
        q2 = lambdaVar - muVar
    elif posOption == '-':
        #if (s[1] < diffCheck and lambdaVar + muVar < 0) or (lambdaVar + muVar > 0):
        if lambdaVar + muVar > 0:
            q2 = lambdaVar + muVar #NOTE: the negative muVar value hasnt been solved. so this is a temp. fix 
        else:
            print("q2 error occured")
        q3 = 0 - q3
    
    #print("lambda:", toDegrees(lambdaVar), " mu:", toDegrees(muVar), end="")
    #print(" q3:", toDegrees(q3), " sin:", math.sin(q3), " cos:", math.cos(q3))
    a1 = a - q1
    b1 = b - (q2 + q3)
    d5x = (d5+d6) * math.cos(b1) * math.sin(a1)
    #NOTE: The x and y axis of the X1Y1Z1 frame in the paper was reverse (compared to this. The names need to be changed!)
    d5z = (d5+d6) * math.sin(b1)
    if b1 == 0:
        q4 = 0
    elif b1 < 0 or b1 > 0:
        q4 = math.atan(d5x / d5z)
    checkVar = math.asin(math.sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
    if math.isnan(checkVar):
        if printText:
            print("asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / d5)  is NaN")
    else:
        if math.sqrt(pow(d5x, 2) + pow(d5z, 2)) > 90:
            q5 = toRadians(90)
        else:
            q5 = math.asin(math.sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
    if d5z < 0:
        q5 = 0-q5
    if b <= math.pi / 2 and b >= 0 - math.pi / 2:
        q6 = Y - q4
    elif b >= math.pi / 2 or b <= 0 - math.pi / 2:
        q6 = math.pi - (Y - q6)
    if printText:
        print(
            " q1: ", toDegrees(q1),
            " q2: ", toDegrees(q2),
            " q3: ", toDegrees(q3),
            " q4: ", toDegrees(q4),
            " q5: ", toDegrees(q5),
            " q6: ", toDegrees(q6),
        )

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

q1_default = 90
q2_default = 0
q3_default = 135
q4_default = 90
q5_default = 90
q6_default = 90

zMax = 300
a, b, Y = 90, 0.1, 90
posX, posY, posZ = 0.1, 0.1, 0.1
coord = ""

while True:
    #get accelerations, roll and pitch
    readAccelerometer()

    ret, imgTemp = cap.read()
    img = cv2.resize(imgTemp, (newSize[0], newSize[1]))
    img = cv2.flip(img, 1)

    detectAndDisplay(img)

    coord = "x:" + str(int(xScaling*(cX - newSize[0]*0.5))) + " y:" + str(int(yScaling*(newSize[1] - cY))) + " z:" +  str(int(zScaling*(zMax - ((cW*cH) / zDefaultVal)*zMax))) #str(cv2.contourArea(contours))
    posX = int(axisFilter * xScaling*(cX - newSize[0]*0.5) + (1-axisFilter) * posX)
    posY = int(axisFilter * yScaling*(newSize[1] - cY) + (1-axisFilter) * posY)
    posZ = int(axisFilter * zScaling*(zMax - ((cW*cH) / zDefaultVal)*zMax) + (1-axisFilter) * posZ)
    if globalPrint or printCoord:
        print(coord)
    if showImage:
        stacked = np.hstack((img, frame_gray))
        cv2.imshow('Windows', cv2.resize(stacked, None, fx=0.7, fy=0.7))
    
    # Calculate the servo rotations
    bPos = False
    if Pitch <= 90 and Pitch >= -90:
        b = toRadians(Pitch * 0.9 + toDegrees(b) * 0.1)
        if b < 0:
            bPos = False
        if b > 0:
            bPos = True
    if Roll <= 90 and Roll >= -90:
        if not bPos:
            a = toRadians(0 - (Roll * 0.9 + toDegrees(a) * 0.1))
        elif bPos:
            a = toRadians(Roll * 0.9 + toDegrees(a) * 0.1)
    getAngles(posX, posY, posZ, a, b, Y, posOption, 1, 1, globalPrint)

    servoExceeded = False
    # "under" = given < 0
    # "over" = given < 180
    whichServoExceeded = 6*[False]
    typeOfExceeded = 6*["null"]

    if toDegrees(q2) - s[1] < diffCheck and readObjects:
        if not (math.isnan(q1) or
            math.isnan(q2) or
            math.isnan(q3) or
            math.isnan(q4) or
            math.isnan(q5) or
            math.isnan(q6)):
            if q1_default - int(round(toDegrees(q1))) < 0: q1 = toRadians(0 + q1_default); servoExceeded = True; whichServoExceeded[0] = True; typeOfExceeded[0] = "under"
            if q2_default + int(round(toDegrees(q2))) < 0: q2 = toRadians(0 - q2_default); servoExceeded = True; whichServoExceeded[1] = True; typeOfExceeded[1] = "under"
            if q3_default + int(round(toDegrees(q3))) < 0: q3 = toRadians(0 - q3_default); servoExceeded = True; whichServoExceeded[2] = True; typeOfExceeded[2] = "under"
            if q4_default + int(round(toDegrees(q4))) < 0: q4 = toRadians(0 - q4_default); servoExceeded = True; whichServoExceeded[3] = True; typeOfExceeded[3] = "under"
            if q5_default + int(round(toDegrees(q5))) < 0: q5 = toRadians(0 - q5_default); servoExceeded = True; whichServoExceeded[4] = True; typeOfExceeded[4] = "under"
            if q6_default + int(round(toDegrees(q6))) < 0: q6 = toRadians(0 - q6_default); servoExceeded = True; whichServoExceeded[5] = True; typeOfExceeded[5] = "under"
            if q1_default - int(round(toDegrees(q1))) > 180: q1 = toRadians(0 - q1_default); servoExceeded = True; whichServoExceeded[0] = True; typeOfExceeded[0] = "over"
            if q2_default + int(round(toDegrees(q2))) > 180: q2 = toRadians(180 - q2_default); servoExceeded = True; whichServoExceeded[1] = True; typeOfExceeded[1] = "over"
            if q3_default + int(round(toDegrees(q3))) > 180: q3 = toRadians(180 - q3_default); servoExceeded = True; whichServoExceeded[2] = True; typeOfExceeded[2] = "over"
            if q4_default + int(round(toDegrees(q4))) > 180: q4 = toRadians(180 - q4_default); servoExceeded = True; whichServoExceeded[3] = True; typeOfExceeded[3] = "over"
            if q5_default + int(round(toDegrees(q5))) > 180: q5 = toRadians(180 - q5_default); servoExceeded = True; whichServoExceeded[4] = True; typeOfExceeded[4] = "over"
            if q6_default + int(round(toDegrees(q6))) > 180: q6 = toRadians(180 - q6_default); servoExceeded = True; whichServoExceeded[5] = True; typeOfExceeded[5] = "over"
            if servoExceeded:
                for i in range(5):
                    if whichServoExceeded[i]:
                        print("\tServo motor: q", i+1, " exceeded \"", typeOfExceeded[i], "\"", sep='')
            s[5] = q6_default - int(round(toDegrees(q6)))
            s[4] = 180 - q5_default - int(round(toDegrees(q5)))
            s[3] = q4_default + int(round(toDegrees(q4)))
            s[2] = 180 - q3_default - int(toDegrees(q3))
            s[1] = q2_default + int(round(toDegrees(q2)))
            s[0] = q1_default - int(round(toDegrees(q1)))
            for x in range(6):
                if x == 4:
                    if s[4]<90:
                        servo[4].angle = getServo4Offset(180 - q5_default - s[4])
                    else:
                        servo[4].angle = s[4]
                else:
                    servo[x].angle = s[x]
        if firstAnglePrint:
            print(
                " q1:", toDegrees(q1), 
                " q2:", toDegrees(q2),
                " q3:", toDegrees(q3),
                " q4:", toDegrees(q4),
                " q5:", toDegrees(q5), 
                " q6:", toDegrees(q6),
                " Roll:", Roll,
                " Pitch:", Pitch
            )

        if globalPrint and endAnglePrint:
            print(
                " q1:", servo[0].angle, 
                " q2:", servo[1].angle,
                " q3:", servo[2].angle,
                " q4:", servo[3].angle,
                " q5:", servo[4].angle, 
                " q6:", servo[5].angle,
                " Roll:", Roll,
                " Pitch:", Pitch
            )

    # Press Esc key to exit
    if cv2.waitKey(1) == 27:
        break
    # plt.show()
        
if showImage:
    cv2.destroyAllWindows()
pca.deinit()

