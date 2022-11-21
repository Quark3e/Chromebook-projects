#!/usr/bin/env python3

#/*
# Terminal User input
# Manual Mode where the coordinate and orientation variables are input
#*/

# Import essential libraries
import requests
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

if False:
    for i in range(0,181):
        servo[4].angle = i
        if i == 90:
            time.sleep(1)
        else:
            time.sleep(0.01)
    time.sleep(1)
    for i in range(180,1, -1):
        servo[4].angle = i
        if i == 90:
            time.sleep(1)
        else:
            time.sleep(0.01)
    time.sleep(1.5)
if False:
    for i in range(0,181):
        servo[0].angle = i
        if i == 45 or i == 90 or i == 135:
            time.sleep(0.5)
        else:
            time.sleep(0.01)
    time.sleep(1)
    for i in range(180,1, -1):
        servo[0].angle = i
        if i == 45 or i == 90 or i == 135:
            time.sleep(0.5)
        else:
            time.sleep(0.01)
    time.sleep(1.5)
if False:
    for i in range(0,181, 10):
        servo[0].angle = i
        if i == 45 or i == 90 or i == 135:
            time.sleep(0.5)
        else:
            time.sleep(0.75)
    time.sleep(1)
    for i in range(180,1, -10):
        servo[0].angle = i
        if i == 45 or i == 90 or i == 135:
            time.sleep(0.5)
        else:
            time.sleep(0.75)
    time.sleep(1.5)


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
    global d1, d2, d3, d4, d5, d6, q1, q2, q3, q4, q5, q6, q7, posX2, posY2, posZ2, b1, a1

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
    if posY2 == 0 or posY2 < 0:
        posY2 = 0.00001
        print("posY2 reached Y0 value!")
    
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
        if lambdaVar + muVar > 0:
            q2 = lambdaVar + muVar #NOTE: the negative muVar value hasnt been solved. so this is a temp. fix 
        else:
            print("q2 error occured")
        q3 = 0 - q3

    a1 = a - q1
    b1 = b - (q2 + q3)
    d5x = (d5+d6) * math.cos(b1) * math.sin(a1)
    #NOTE: The x and y axis of the X1Y1Z1 frame in the paper was reverse (compared to this. The names need to be changed!)
    d5z = (d5+d6) * math.sin(b1)
    if d5z == 0:
        q4 = 0
    elif b1 < 0 or b1 > 0:
        q4 = math.atan((d5x) / d5z)
    checkVar = math.asin(math.sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
    if math.isnan(checkVar):
        if printText:
            print("asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / d5)  is NaN")
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
            " q1:", round(toDegrees(q1), 1),
            " q2:", round(toDegrees(q2), 1),
            " q3:", round(toDegrees(q3), 1),
            " q4:", round(toDegrees(q4), 1),
            " q5:", round(toDegrees(q5), 1),
            " q6:", round(toDegrees(q6), 1),
            sep=''
        )


q1_default = 90
q2_default = 0
q3_default = 135
q4_default = 90
q5_default = 90
q6_default = 90

zMax = 300
a, b, Y = toRadians(0), toRadians(-45), toRadians(90)
posX, posY, posZ = 0.1, 0.1, 0.1
coord = ""

while True:
    #get accelerations, roll and pitch

    # readAccelerometer()
    
    tempInput_1 = input("Enter coordinates [x y z] in mm: ").split()
    if tempInput_1[0] == "exit":
        sys.exit()
    else:
        posX = (float(tempInput_1[0]))
        posY = (float(tempInput_1[1]))
        posZ = (float(tempInput_1[2]))
    tempInput_2 = input("Enter orientation values [a b Y] in degrees: ").split()
    a = toRadians(float(tempInput_2[0]))
    b = toRadians(float(tempInput_2[1]))
    Y = toRadians(float(tempInput_2[2]))

    print()
    if diagnostics:
        print("x:", posX, " y:", posY, " z:", posZ, " a:", toDegrees(a), " b:", toDegrees(b), " Y:", toDegrees(Y), sep='')
    getAngles(posX, posY, posZ, a, b, Y, posOption, 1, 1, globalPrint)

    if diagnostics:
        print(" Read: alpha:", toDegrees(q1 + math.atan((d5*math.sin(q5)*math.cos(q4)) / (d5*math.cos(q5))) ), end='')
        print(" beta:", toDegrees(math.asin((d5*math.sin(q5)*math.cos(q4)) / d5) + q2+q3))
        

    servoExceeded = False
    # "under" = given < 0
    # "over" = given < 180
    whichServoExceeded = 6*[False]
    typeOfExceeded = 6*["null"]

    if toDegrees(q2) - s[1] < diffCheck:
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
                for i in range(6):
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
                " read:\n"
                " q1:", toDegrees(q1), 
                " q2:", toDegrees(q2),
                " q3:", toDegrees(q3),
                " q4:", toDegrees(q4),
                " q5:", toDegrees(q5), 
                " q6:", toDegrees(q6),
                " Roll:", Roll,
                " Pitch:", Pitch
            )

        if globalPrint or endAnglePrint:
            print(
                " Sent: q1:", int(servo[0].angle), 
                " q2:", int(servo[1].angle),
                " q3:", int(servo[2].angle),
                " q4:", int(servo[3].angle),
                " q5:", int(servo[4].angle), 
                " q6:", int(servo[5].angle),
                # " Roll:", Roll,
                # " Pitch:", Pitch
                sep=''
            )

pca.deinit()

