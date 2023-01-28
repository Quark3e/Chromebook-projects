#!/usr/bin/env python3

# purpose: Manually input self-measured xyz coords of the irl end-effector "tip"/"point" 

# Import essential libraries
from fileinput import filename
import requests
import numpy as np
import imutils
import time
import math
import sys

from board import SCL, SDA
import busio

from adafruit_motor import servo
from adafruit_servokit import ServoKit
from adafruit_pca9685 import PCA9685

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

fileName = "basicTest_2" #Dont enter filetype
hardSetAngle = [0, -60]
useHardSetAngle = True

if useHardSetAngle:
    ans = input(" Enter alpha and beta values to test (with space): ").split()
    if ans[0] == "exit":
        sys.exit()
    hardSetAngle = [int(ans[0]), int(ans[1])]


def getServo4Offset(degrees):
    return 90-(90/130)*degrees
#0-90 is inaccurate where the actual angle is 130 for the given 90 degrees
#Note: Only use this function if the rotation value is 

servo[0].angle = 90
servo[1].angle = 45
servo[2].angle = 180 - 0
servo[3].angle = 90
servo[4].angle = getServo4Offset(90)
servo[5].angle = 90

d1 = 140; #axial "roll"
d2 = 135; #axial "pitch"
d3 = 70; #axial "pitch"
d4 = 80; #axial "roll"
d5 = 45; #axial "pitch
d6 = 45; #axial "roll" (?)


def smoothServo_2(newAng, oldAng, servoNum, delayTimer = 0.01):
    k1, k2 = 0.1, 0.9
    oA = oldAng-1
    n = 0
    while oldAng<newAng:
        oldAng=k1*newAng+k2*oldAng
        servoNum.angle = int(round(oldAng))
        #if int(round(oldAng))==oA:
            #break
        #oA=int(round(oldAng))
        time.sleep(delayTimer)
        n+=1
        if n>=100:
            break

def smoothServo(newAng, oldAng, servoNum, delayTimer = 0.01):
    for i in range(int(round(oldAng)), int(round(newAng))):
        servoNum.angle = i

def toDegrees(radians):
    return (radians * 180) / math.pi
def toRadians(degrees):
    return (degrees * math.pi) / 180

time.sleep(1)

distanceFromCam = 600 #in mm
webcamFOV = [60, 40] #Unit: degrees, [0] = x fov, [1] = y fov
winScaleX, winScaleY = 1, 1

rowLength = 68
a, b, Y = toRadians(0), toRadians(-90), toRadians(90)

axisFilter = 0.7 #On the new value end
xScaling, yScaling, zScaling = 0.8, 0.8, 1.2
brightVal = 75
zDefaultVal = 3000000
L_values, U_values = [0, 0, 255], [0, 0, 255]
#L_values, U_values = input("Enter L- and U-values without comma: "), input()
diffCheck = 100
showImage = True
globalPrint = False
endAnglePrint = False
firstAnglePrint = False
posOption = '-'


newSize = (640, 480)

manualInput = True
readX, readY, readZ = 0.1, 0.1, 0.1 #Values that are to be compared with the given values
actualValues = [0, 0]
axisFilter2 = 1


time.sleep(1)

q1, q2, q3, q4, q5, q6, q7 = 0, 0, 0, 0, 0, 0, 0 #NOTE: q1 = servo[0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos

posX2, posY2, posZ2 = 0.01, 0.01, 0.01
a1, b1 = 0.1, 0.1


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

q1_default = 90
q2_default = 0
q3_default = 135
q4_default = 90
q5_default = 90
q6_default = 90

zMax = 300

fileExtension = ".dat"
toReadFile = open(fileName + fileExtension, "r")
toWriteFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_read" + fileExtension, "w") #Note: add +"\n" at the end of each write
accWriteFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_acc" + fileExtension, "w") #Note: add +"\n" at the end of each write
resultFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_Result" + fileExtension, "w") #Note: add +"\n" at the end of each write


posX, posY, posZ = 0.1, 0.1, 0.1
coord = ""

def check_qMinMax():
    global q1, q2, q3, q4, q5, q6
    if q1_default - int(round(toDegrees(q1))) < 0: q1 = toRadians(0 + q1_default)
    if q2_default + int(round(toDegrees(q2))) < 0: q2 = toRadians(0 - q2_default)
    if q3_default + int(round(toDegrees(q3))) < 0: q3 = toRadians(0 - q3_default)
    if q4_default + int(round(toDegrees(q4))) < 0: q4 = toRadians(0 - q4_default)
    if q5_default + int(round(toDegrees(q5))) < 0: q5 = toRadians(0 - q5_default)
    if q6_default + int(round(toDegrees(q6))) < 0: q6 = toRadians(0 - q6_default)
    if q1_default - int(round(toDegrees(q1))) > 180: q1 = toRadians(0 - q1_default)
    if q2_default + int(round(toDegrees(q2))) > 180: q2 = toRadians(180 - q2_default)
    if q3_default + int(round(toDegrees(q3))) > 180: q3 = toRadians(180 - q3_default)
    if q4_default + int(round(toDegrees(q4))) > 180: q4 = toRadians(180 - q4_default)
    if q5_default + int(round(toDegrees(q5))) > 180: q5 = toRadians(180 - q5_default)
    if q6_default + int(round(toDegrees(q6))) > 180: q6 = toRadians(180 - q6_default)

def findAlphaBeta(posX, posY, posZ, posOption):
    global a, b
    #Find the a, b combination that is valid
    breakVal = False
    for beta in range(-90, 0):
        for alpha in range(-90, 90):
            getAngles(posX, posY, posZ, toRadians(alpha), toRadians(beta), Y, posOption, 1, 1, False)
            if not (math.isnan(q1) or
                math.isnan(q2) or
                math.isnan(q3) or
                math.isnan(q4) or
                math.isnan(q5) or
                math.isnan(q6)):
                a = toRadians(alpha)
                b = toRadians(beta)
                breakVal = True
                break
        if breakVal:
            break
    if not breakVal:
        for beta in range(0, 90):
            for alpha in range(-90, 90):
                getAngles(posX, posY, posZ, toRadians(alpha), toRadians(beta), Y, posOption, 1, 1, False)
                if not (math.isnan(q1) or
                    math.isnan(q2) or
                    math.isnan(q3) or
                    math.isnan(q4) or
                    math.isnan(q5) or
                    math.isnan(q6)):
                    a = toRadians(alpha)
                    b = toRadians(beta)
                    breakVal = True
                    break
            if breakVal:
                break
    return a, b


for n in range(rowLength):
    # get xyz a b values
    ##check values just in case that they don't return NaN angles
    # send values to robot arm (i.e solve angles and send those)
    # read and write the real coordinate and write it to the corresponding file
    # repeat

    #Read line and get the xyz value
    fileLine = toReadFile.readline()
    print("fileLine:", fileLine, end='', sep='')
    posX = float(fileLine[0:fileLine.find(" ")+1])
    print("posX:", fileLine[0:fileLine.find(" ")+1], end='', sep='')
    fileLine = fileLine.replace(fileLine[0:fileLine.find(" ")+1], '', 1)
    posY = float(fileLine[0:fileLine.find(" ")+1])
    print(" posY:", fileLine[0:fileLine.find(" ")+1], end='', sep='')
    fileLine = fileLine.replace(fileLine[0:fileLine.find(" ")+1], '', 1)
    posZ = float(fileLine[0:fileLine.find(" ")+1])
    print(" posZ:", fileLine[0:fileLine.find(" ")+1], end='\n', sep='')
    fileLine = fileLine.replace(fileLine[0:fileLine.find(" ")+1], '', 1)
    a = toRadians(float(fileLine[0:fileLine.find(" ")+1]))
    # print(" a:", fileLine[0:fileLine.find(" ")+1], end='', sep='')
    fileLine = fileLine.replace(fileLine[0:fileLine.find(" ")+1], '', 1)
    b = toRadians(float(fileLine[0:fileLine.find("\n")+1]))
    # print(" b:", fileLine[0:fileLine.find(" ")+1], "\n", sep='')

    if useHardSetAngle:
        a = toRadians(hardSetAngle[0])
        b = toRadians(hardSetAngle[1])

    #Check if values are NaN
    getAngles(posX, posY, posZ, a, b, Y, posOption, 1, 1, globalPrint)
    if not (math.isnan(q1) or
        math.isnan(q2) or
        math.isnan(q3) or
        math.isnan(q4) or
        math.isnan(q5) or
        math.isnan(q6)):
        #Send angles to servo motors
        check_qMinMax()
        s[5] = q6_default - int(round(toDegrees(q6)))
        s[4] = 180 - q5_default - int(round(toDegrees(q5)))
        s[3] = q4_default + int(round(toDegrees(q4)))
        s[2] = 180 - q3_default - int(round(toDegrees(q3)))
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
                " q6:", toDegrees(q6)
            )
        if globalPrint or endAnglePrint:
            print(
                " q1:", servo[0].angle, 
                " q2:", servo[1].angle,
                " q3:", servo[2].angle,
                " q4:", servo[3].angle,
                " q5:", servo[4].angle, 
                " q6:", servo[5].angle
            )
        
        if not manualInput:
            time.sleep(2)

        #Input xyz commands
        ans = input(" Read and input the real X Y and Z coordinate of the end-effector(unit in mm): ").split()
        if ans[0] == 'exit':
            sys.exit()
        readX, readY, readZ = ans[0], ans[1], ans[2]

        toWriteFile.write(readX + " " + readY + " " + readZ + "\n")
        accWriteFile.write(str(abs(float(readX)-posX)) + " " + str(abs(float(readY)-posY)) + " " + str(abs(float(readZ)-posZ)) + "\n")
        resultFile.write(str(posX) + " " + str(posY) + " " + str(posZ) + " " + readX + " " + readY + " " + readZ + "\n")

pca.deinit()
toReadFile.close()
toWriteFile.close()
accWriteFile.close()
resultFile.close()
