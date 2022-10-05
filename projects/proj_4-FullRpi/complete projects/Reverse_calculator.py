#!/usr/bin/env python3

#/*
# Solves the position of end-effector from link lengths (d) and solved angles
#*/

# Import essential libraries
import requests
import numpy as np
import imutils
import time
import math
import sys


servo = 6*[0] #The final angle. Do not use this for reverse calculations as it includes default values


servo[0] = 90
servo[1] = 45
servo[2] = 180 - 45
servo[3] = 90
servo[4] = 180 - 180
servo[5] = 90

def getServo4Offset(degrees):
    return 90-(90/130)*degrees
#0-90 is inaccurate where the actual angle is 130 for the given 90 degrees
#Note: Only use this function if the rotation value is 

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


def getP1():
    return [0, 0, 0]
def getP2():
    return (0, 0, d1)
def getP3(q1, q2): #in radians
    return [
        d2*math.cos(q2)*math.sin(q1),
        d2*math.cos(q2)*math.cos(q1),
        d1+d2*math.sin(q2)
        ]
def getP4(P3, q1, q2, q3): #in radians
    return [
        P3[0]+d3*math.cos(q2+q3)*math.sin(q1),
        P3[1]+d3*math.cos(q2+q3)*math.cos(q1),
        P3[2]+d3*math.sin(q2+q3)
        ]
def getP5(P3, q1, q2, q3): #in radians
    return [
        P3[0]+(d3+d4)*math.cos(q2+q3)*math.sin(q1),
        P3[1]+(d3+d4)*math.cos(q2+q3)*math.cos(q1),
        P3[2]+(d3+d4)*math.sin(q2+q3)
        ]

def getPP(P5, q1, q2, q3, q4, q5): #in radians
    # return [
    #     P5[0]+(d5+d6)*math.cos(b)*math.sin(a),
    #     P5[1]+(d5+d6)*math.cos(b)*math.cos(a),
    #     P5[2]+(d5+d6)*math.sin(b)
    # ]
    b = math.asin((d5*math.sin(q5)*math.cos(q4))/d5)+q2+q3
    a = q1+math.atan((d5*math.sin(q5)*math.cos(q4))/(d5*math.cos(q5)))
    print("calculated: alpha: ", toDegrees(a), " beta:", toDegrees(b), sep='')
    return [
        P5[0]+(d5+d6)*math.cos(b)*math.sin(a),
        P5[1]+(d5+d6)*math.cos(b)*math.cos(a),
        P5[2]+(d5+d6)*math.sin(b)
        ]

def getQ4fromQ5(a, b, q1, q2, q3, q5): #returns two answers: [0] from alpha equation, [1] from beta equation
    return [
        math.acos(math.tan(a-q1)/math.tan(q5)),
        math.acos(math.sin(b-q2-q3)/math.sin(q5))
        ]
def getQ5fromQ4(a, b, q1, q2, q3, q4):
    return [
        math.atan(math.tan(a-q1)/math.cos(q4)),
        math.asin(math.sin(b-q2-q3)/math.cos(q4))
        ]

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
    if d5z == 0:
        q4 = 0
    elif b1 < 0 or b1 > 0:
        q4 = math.atan(d5x / d5z)
    checkVar = math.asin(math.sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
    if math.isnan(checkVar):
        if printText:
            print("asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / d5)  is NaN")
    else:
        # if math.sqrt(pow(d5x, 2) + pow(d5z, 2)) > 90:
        #     q5 = toRadians(90)
        # else:
        q5 = math.asin(math.sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
        #q5 = math.atan(math.sqrt(pow(d5x,2) + pow(d5z, 2)) / (math.cos(b1)*math.cos(a1)))
        #q5 = math.atan(math.sqrt(pow(d5x, 2) + pow(d5z, 2))/(d5x/(math.tan(a1))))
    if d5z < 0:
        q5 = 0-q5
    if b <= math.pi / 2 and b >= 0 - math.pi / 2:
        q6 = Y - q4
    elif b >= math.pi / 2 or b <= 0 - math.pi / 2:
        q6 = math.pi - (Y - q6)
    if printText:
        print(
            " q1: ", round(toDegrees(q1), 1),
            " q2: ", round(toDegrees(q2), 1),
            " q3: ", round(toDegrees(q3), 1),
            " q4: ", round(toDegrees(q4), 1),
            " q5: ", round(toDegrees(q5), 1),
            " q6: ", round(toDegrees(q6), 1),
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
    print()
    tempInput_2 = input("Enter orientation values [a b Y] in degrees: ").split()
    a = toRadians(float(tempInput_2[0]))
    b = toRadians(float(tempInput_2[1]))
    Y = toRadians(float(tempInput_2[2]))

    print("x:", posX, " y:", posY, " z:", posZ, " a:", toDegrees(a), " b:", toDegrees(b), " Y:", toDegrees(Y), sep='')
    getAngles(posX, posY, posZ, a, b, Y, posOption, 1, 1, globalPrint)

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
            P1 = getP1()
            P2 = getP2()
            P3 = getP3(q1, q2)
            P4 = getP4(P3, q1, q2, q3)
            P5 = getP5(P3, q1, q2, q3)
            PP = getPP(P5, q1, q2, q3, q4, q5)

            q4_from_q5 = getQ4fromQ5(a, b, q1, q2, q3, q5) #[0]=a, [1]=b
            q5_from_q4 = getQ5fromQ4(a, b, q1, q2, q3, q4) #[0]=a, [1]=b
            PP_fromAlpha_fromQ5 = getPP(P5, q1, q2, q3, q4_from_q5[0], q5)
            PP_fromAlpha_fromQ4 = getPP(P5, q1, q2, q3, q4, q5_from_q4[0])
            PP_fromAlpha_fromBoth = getPP(P5, q1, q2, q3, q4_from_q5[0], q5_from_q4[0])

            PP_fromBeta_fromQ5 = getPP(P5, q1, q2, q3, q4_from_q5[1], q5)
            PP_fromBeta_fromQ4 = getPP(P5, q1, q2, q3, q4, q5_from_q4[1])
            PP_fromBeta_fromBoth = getPP(P5, q1, q2, q3, q4_from_q5[1], q5_from_q4[1])

            print("----------------------")
            print(" P1: x:", P1[0], " y:", P1[1], " z:", P1[2], sep='')
            print(" P2: x:", P2[0], " y:", P2[1], " z:", P2[2], sep='')
            print(" P3: x:", P3[0], " y:", P3[1], " z:", P3[2], sep='')
            print(" P4: x:", P4[0], " y:", P4[1], " z:", P4[2], sep='')
            print(" P5: x:", P5[0], " y:", P5[1], " z:", P5[2], sep='')
            print(" PP: x:", PP[0], " y:", PP[1], " z:", PP[2], sep='')
            print("\n\tError value: x:", float(PP[0])-posX, " y:", float(PP[1])-posY, " z:", float(PP[2])-posZ, sep='')

            print("\n _fromAlpha: from (default)q5: x:", PP_fromAlpha_fromQ5[0], " y:", PP_fromAlpha_fromQ5[1], " z:", PP_fromAlpha_fromQ5[2], sep='') 
            print(" _fromAlpha: from (default)q4: x:", PP_fromAlpha_fromQ4[0], " y:", PP_fromAlpha_fromQ4[1], " z:", PP_fromAlpha_fromQ4[2], sep='') 
            print(" _fromAlpha: from both: x:", PP_fromAlpha_fromBoth[0], " y:", PP_fromAlpha_fromBoth[1], " z:", PP_fromAlpha_fromBoth[2], sep='') 

            print(" _fromBeta:  from (default)q5: x:", PP_fromBeta_fromQ5[0], " y:", PP_fromBeta_fromQ5[1], " z:", PP_fromBeta_fromQ5[2], sep='')
            print(" _fromBeta:  from (default)q4: x:", PP_fromBeta_fromQ4[0], " y:", PP_fromBeta_fromQ4[1], " z:", PP_fromBeta_fromQ4[2], sep='')
            print(" _fromBeta:  from both: x:", PP_fromBeta_fromBoth[0], " y:", PP_fromBeta_fromBoth[1], " z:", PP_fromBeta_fromBoth[2], sep='')

            print("----------------------")

            s[5] = q6_default - int(round(toDegrees(q6)))
            s[4] = 180 - q5_default - int(round(toDegrees(q5)))
            s[3] = q4_default + int(round(toDegrees(q4)))
            s[2] = 180 - q3_default - int(toDegrees(q3))
            s[1] = q2_default + int(round(toDegrees(q2)))
            s[0] = q1_default - int(round(toDegrees(q1)))
            for x in range(6):
                if x == 4:
                    if s[4]<90:
                        servo[4] = getServo4Offset(180 - q5_default - s[4])
                    else:
                        servo[4] = s[4]
                else:
                    servo[x] = s[x]
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
                " q1:", servo[0], 
                " q2:", servo[1],
                " q3:", servo[2],
                " q4:", servo[3],
                " q5:", servo[4], 
                " q6:", servo[5],
                " Roll:", Roll,
                " Pitch:", Pitch
            )

pca.deinit()

