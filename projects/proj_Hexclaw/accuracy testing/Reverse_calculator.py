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
import readline

sys.path.append('/home/berkhme/Chromebook-projects/projects/proj_4-FullRpi')

from IK_module import toDegrees,toRadians,getAngles,getDistance,getSubframe
from IK_module import link, default_q

fileName = "basicTest_2"
hardSetAngle = [0, 0]
useHardSetAngle = True

checkList = False
answer = input(" To check accuracy of test# list? [y/n]: ")
if answer == "exit":
    sys.exit()
elif answer == "y":
    checkList = True
elif answer == "n":
    checkList = False


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

q = 6*[0.0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos

posX2, posY2, posZ2 = 0.01, 0.01, 0.01
a1, b1 = 0.1, 0.1


def getP1():
    return [0, 0, 0]
def getP2():
    return (0, 0, link[0])
def getP3(q): #in radians
    return [
        link[1]*math.cos(q[1])*math.sin(q[0]),
        link[1]*math.cos(q[1])*math.cos(q[0]),
        link[0]+link[1]*math.sin(q[1])
        ]
def getP4(P3, q): #in radians
    return [
        P3[0]+link[2]*math.cos(q[1]+q[2])*math.sin(q[0]),
        P3[1]+link[2]*math.cos(q[1]+q[2])*math.cos(q[0]),
        P3[2]+link[2]*math.sin(q[1]+q[2])
        ]
def getP5(P3, q): #in radians
    return [
        P3[0]+(link[2]+link[3])*math.cos(q[1]+q[2])*math.sin(q[0]),
        P3[1]+(link[2]+link[3])*math.cos(q[1]+q[2])*math.cos(q[0]),
        P3[2]+(link[2]+link[3])*math.sin(q[1]+q[2])
        ]

def getPP(P5, q, purpose="no purpose"+":"): #in radians
    global a, b
    # return [
    #     P5[0]+(link[4]+link[5])*math.cos(b)*math.sin(a),
    #     P5[1]+(link[4]+link[5])*math.cos(b)*math.cos(a),
    #     P5[2]+(link[4]+link[5])*math.sin(b)
    # ]
    b = math.asin(((link[4]+link[5])*math.sin(q[4])*math.cos(q[3]))/(link[4]+link[5]))+q[1]+q[2]
    if int(toDegrees(q[4])) == 90 or int(toDegrees(q[4])) == -90:
        if ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) == 0:
            a = 0
        elif ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) > 0:
            a = toRadians(90)
        elif ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) < 0:
            a = toRadians(-90)
    else:
        a = q[0]-math.atan(
            ((link[4]+link[5])*math.sin(q[4])*math.sin(q[3])) / ((link[4]+link[5])*math.cos(q[4]))
            )
    print("{:16} ".format(purpose)," calculated: alpha:{:5} beta:{:5}".format(str(round(toDegrees(a))), str(round(toDegrees(b)))), 
    "  solved y:{:3}".format(round((link[4]+link[5])*math.cos(b)*math.cos(a))), 
    "  given(read): q4:{:3}".format(round(toDegrees(q[3]))),
    "  given(read): q5:{:3}".format(round(toDegrees(q[4]))), sep='')

    return [
        P5[0]+(link[4]+link[5])*math.cos(b)*math.sin(a),
        P5[1]+(link[4]+link[5])*math.cos(b)*math.cos(a),
        P5[2]+(link[4]+link[5])*math.sin(b)
        ]

def getQ4fromQ5(a, b, q): #returns two answers: [0] from alpha equation, [1] from beta equation
    print(q[4])
    return [
        math.acos(math.tan(a-q[0])/math.tan(q[4])),
        math.acos(math.sin(b-q[1]-q[2])/math.sin(q[4]))
        ]
def getQ5fromQ4(a, b, q):
    return [
        math.atan(math.tan(a-q[0])/math.cos(q[3])),
        math.asin(math.sin(b-q[1]-q[2])/math.cos(q[3]))
        ]

zMax = 300
a, b, Y = toRadians(0), toRadians(-45), toRadians(90)
posX, posY, posZ = 0.1, 0.1, 0.1
coord = ""

def getErrorRate(comparedTo, toCompare, getRawVal = True, getTotalAverage = True):
    for i in range(3):
        if comparedTo[i] == 0:
            comparedTo[i] = comparedTo[i]+1
            toCompare[i] = toCompare[i]+1
    
    errorRate = [
        (comparedTo[0] - abs(toCompare[0] - comparedTo[0]))/comparedTo[0],
        (comparedTo[1] - abs(toCompare[1] - comparedTo[1]))/comparedTo[1],
        (comparedTo[2] - abs(toCompare[2] - comparedTo[2]))/comparedTo[2]
        ]
    if getTotalAverage:
        if getRawVal:
            return int(100*(errorRate[0]+errorRate[1]+errorRate[2]) / 3)
        else:
            return (errorRate[0]+errorRate[1]+errorRate[2]) / 3
    else:
        if getRawVal:
            return [int(100*errorRate[0]), int(100*errorRate[1]), int(100*errorRate[2])]
        else:
            return errorRate

if checkList:
    while True:
        if useHardSetAngle and checkList:
            ans = input(" Enter alpha and beta values to test (with space): ").split()
        if ans[0] == "exit": #type: ignore
            sys.exit()
        hardSetAngle = [int(ans[0]), int(ans[1])] #type: ignore

        rowLength = 68
        fileExtension = ".dat"
        filePath = "/home/berkhme/Chromebook-projects/projects/proj_4-FullRpi/accuracy testing/test2/"
        toReadFile = open(filePath + fileName + fileExtension, "r")
        # toWriteFile = open(filePath + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_read" + fileExtension, "w") #Note: add +"\n" at the end of each write
        # accWriteFile = open(filePath + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_acc" + fileExtension, "w") #Note: add +"\n" at the end of each write
        resultFile = open(filePath + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_Result" + fileExtension, "w") #Note: add +"\n" at the end of each write

        for n in range(rowLength):
            
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
                a = toRadians(float(hardSetAngle[0]))
                b = toRadians(float(hardSetAngle[1]))
            Y = toRadians(0)

            print()

            print(" x:", posX, " y:", posY, " z:", posZ, " a:", toDegrees(a), " b:", toDegrees(b), " Y:", toDegrees(Y), sep='')
            q = getAngles([posX, posY, posZ], a, b, Y, posOption, globalPrint)
            print(" a1:", int(toDegrees(a1)), " b1:", int(toDegrees(b1)), sep='')

            servoExceeded = False
            # "under" = given < 0
            # "over" = given < 180
            whichServoExceeded = 6*[False]
            typeOfExceeded = 6*["null"]

            if toDegrees(q[1]) - s[1] < diffCheck:
                if not (math.isnan(q[0]) or
                    math.isnan(q[1]) or
                    math.isnan(q[2]) or
                    math.isnan(q[3]) or
                    math.isnan(q[4]) or
                    math.isnan(q[5])):
                    if default_q[0] - int(round(toDegrees(q[0]))) < 0: q[0] = toRadians(0 + default_q[0]); servoExceeded = True; whichServoExceeded[0] = True; typeOfExceeded[0] = "under"
                    if default_q[1] + int(round(toDegrees(q[1]))) < 0: q[1] = toRadians(0 - default_q[1]); servoExceeded = True; whichServoExceeded[1] = True; typeOfExceeded[1] = "under"
                    if default_q[2] + int(round(toDegrees(q[2]))) < 0: q[2] = toRadians(0 - default_q[2]); servoExceeded = True; whichServoExceeded[2] = True; typeOfExceeded[2] = "under"
                    if default_q[3] + int(round(toDegrees(q[3]))) < 0: q[3] = toRadians(0 - default_q[3]); servoExceeded = True; whichServoExceeded[3] = True; typeOfExceeded[3] = "under"
                    if default_q[4] + int(round(toDegrees(q[4]))) < 0: q[4] = toRadians(0 - default_q[4]); servoExceeded = True; whichServoExceeded[4] = True; typeOfExceeded[4] = "under"
                    if default_q[5] + int(round(toDegrees(q[5]))) < 0: q[5] = toRadians(0 - default_q[5]); servoExceeded = True; whichServoExceeded[5] = True; typeOfExceeded[5] = "under"
                    if default_q[0] - int(round(toDegrees(q[0]))) > 180: q[0] = toRadians(0 - default_q[0]); servoExceeded = True; whichServoExceeded[0] = True; typeOfExceeded[0] = "over"
                    if default_q[1] + int(round(toDegrees(q[1]))) > 180: q[1] = toRadians(180 - default_q[1]); servoExceeded = True; whichServoExceeded[1] = True; typeOfExceeded[1] = "over"
                    if default_q[2] + int(round(toDegrees(q[2]))) > 180: q[2] = toRadians(180 - default_q[2]); servoExceeded = True; whichServoExceeded[2] = True; typeOfExceeded[2] = "over"
                    if default_q[3] + int(round(toDegrees(q[3]))) > 180: q[3] = toRadians(180 - default_q[3]); servoExceeded = True; whichServoExceeded[3] = True; typeOfExceeded[3] = "over"
                    if default_q[4] + int(round(toDegrees(q[4]))) > 180: q[4] = toRadians(180 - default_q[4]); servoExceeded = True; whichServoExceeded[4] = True; typeOfExceeded[4] = "over"
                    if default_q[5] + int(round(toDegrees(q[5]))) > 180: q[5] = toRadians(180 - default_q[5]); servoExceeded = True; whichServoExceeded[5] = True; typeOfExceeded[5] = "over"
                    if servoExceeded:
                        for i in range(6):
                            if whichServoExceeded[i]:
                                print("\tServo motor: q", i+1, " exceeded \"", typeOfExceeded[i], "\"", sep='')
                    P1 = getP1()
                    P2 = getP2()
                    P3 = getP3(q)
                    P4 = getP4(P3, q)
                    P5 = getP5(P3, q)
                    PP = getPP(P5, q)
                    

                    readX, readY, readZ = str(PP[0]), str(PP[1]), str(PP[2])

                    # q4_from_q5 = getQ4fromQ5(a, b, q[0], q[1], q[2], q[4]) #[0]=a, [1]=b
                    # q5_from_q4 = getQ5fromQ4(a, b, q[0], q[1], q[2], q[3]) #[0]=a, [1]=b
                    # PP_fromAlpha_fromQ5 = getPP(P5, q[0], q[1], q[2], q4_from_q5[0], q[4])
                    # PP_fromAlpha_fromQ4 = getPP(P5, q[0], q[1], q[2], q[3], q5_from_q4[0])
                    # PP_fromAlpha_fromBoth = getPP(P5, q[0], q[1], q[2], q4_from_q5[0], q5_from_q4[0])

                    # PP_fromBeta_fromQ5 = getPP(P5, q[0], q[1], q[2], q4_from_q5[1], q[4])
                    # PP_fromBeta_fromQ4 = getPP(P5, q[0], q[1], q[2], q[3], q5_from_q4[1])
                    # PP_fromBeta_fromBoth = getPP(P5, q[0], q[1], q[2], q4_from_q5[1], q5_from_q4[1])

                    # print("----------------------")
                    # print(" P1: x:", P1[0], " y:", P1[1], " z:", P1[2], sep='')
                    # print(" P2: x:", P2[0], " y:", P2[1], " z:", P2[2], sep='')
                    # print(" P3: x:", int(round(P3[0])), " y:", int(round(P3[1])), " z:", int(round(P3[2])), sep='')
                    # print(" P4: x:", int(round(P4[0])), " y:", int(round(P4[1])), " z:", int(round(P4[2])), sep='')
                    # print(" P5: x:", int(round(P5[0])), " y:", int(round(P5[1])), " z:", int(round(P5[2])), sep='')
                    # print(" PP: x:", int(round(PP[0])), " y:", int(round(PP[1])), " z:", int(round(PP[2])), sep='')
                    # print("\n\tError value: x:", float(PP[0])-posX, " y:", float(PP[1])-posY, " z:", float(PP[2])-posZ, sep='')

                    # print()
                    # print(" _fromAlpha: from (default)q[4]: \tx:", int(round(PP_fromAlpha_fromQ5[0])), " y:", int(round(PP_fromAlpha_fromQ5[1])), " z:", int(round(PP_fromAlpha_fromQ5[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromAlpha_fromQ5), "%", sep='') 
                    # print(" _fromAlpha: from (default)q[3]: \tx:", int(round(PP_fromAlpha_fromQ4[0])), " y:", int(round(PP_fromAlpha_fromQ4[1])), " z:", int(round(PP_fromAlpha_fromQ4[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromAlpha_fromQ4), "%", sep='') 
                    # print(" _fromAlpha: from both: \tx:", int(round(PP_fromAlpha_fromBoth[0])), " y:", int(round(PP_fromAlpha_fromBoth[1])), " z:", int(round(PP_fromAlpha_fromBoth[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromAlpha_fromBoth), "%", sep='') 

                    # print(" _fromBeta:  from (default)q[4]: \tx:", int(round(PP_fromBeta_fromQ5[0])), " y:", int(round(PP_fromBeta_fromQ5[1])), " z:", int(round(PP_fromBeta_fromQ5[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromBeta_fromQ5), "%", sep='')
                    # print(" _fromBeta:  from (default)q[3]: \tx:", int(round(PP_fromBeta_fromQ4[0])), " y:", int(round(PP_fromBeta_fromQ4[1])), " z:", int(round(PP_fromBeta_fromQ4[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromBeta_fromQ4), "%", sep='')
                    # print(" _fromBeta:  from both: \tx:", int(round(PP_fromBeta_fromBoth[0])), " y:", int(round(PP_fromBeta_fromBoth[1])), " z:", int(round(PP_fromBeta_fromBoth[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromBeta_fromBoth), "%", sep='')

                    print("----------------------")

                    # toWriteFile.write(readX + " " + readY + " " + readZ + "\n")
                    # accWriteFile.write(str(abs(float(readX)-posX)) + " " + str(abs(float(readY)-posY)) + " " + str(abs(float(readZ)-posZ)) + "\n")
                    resultFile.write(str(posX) + " " + str(posY) + " " + str(posZ) + " " + readX + " " + readY + " " + readZ + "\n")


                    s[5] = default_q[5] - int(round(toDegrees(q[5])))
                    s[4] = 180 - default_q[4] - int(round(toDegrees(q[4])))
                    s[3] = default_q[3] + int(round(toDegrees(q[3])))
                    s[2] = 180 - default_q[2] - int(toDegrees(q[2]))
                    s[1] = default_q[1] + int(round(toDegrees(q[1])))
                    s[0] = default_q[0] - int(round(toDegrees(q[0])))
                    for x in range(6):
                        if x == 4:
                            if s[4]<90:
                                servo[4] = getServo4Offset(180 - default_q[4] - s[4]) #type: ignore
                            else:
                                servo[4] = s[4]
                        else:
                            servo[x] = s[x]
                if firstAnglePrint:
                    print(
                        " q[0]:", toDegrees(q[0]), 
                        " q[1]:", toDegrees(q[1]),
                        " q[2]:", toDegrees(q[2]),
                        " q[3]:", toDegrees(q[3]),
                        " q[4]:", toDegrees(q[4]), 
                        " q[5]:", toDegrees(q[5]),
                        " Roll:", Roll,
                        " Pitch:", Pitch
                    )

                if globalPrint and endAnglePrint:
                    print(
                        " q[0]:", servo[0], 
                        " q[1]:", servo[1],
                        " q[2]:", servo[2],
                        " q[3]:", servo[3],
                        " q[4]:", servo[4], 
                        " q[5]:", servo[5],
                        " Roll:", Roll,
                        " Pitch:", Pitch
                    )
        toReadFile.close()
        # toWriteFile.close()
        # accWriteFile.close()
        resultFile.close()

elif not checkList:
    while True:
        #get accelerations, roll and pitch

        # readAccelerometer()
        print()
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

        print(" x:", posX, " y:", posY, " z:", posZ, " a:", toDegrees(a), " b:", toDegrees(b), " Y:", toDegrees(Y), sep='')
        q = getAngles([posX, posY, posZ], a, b, Y, posOption, 1, 1, globalPrint)
        print(" a1:", toDegrees(a1), " b1:", toDegrees(b1), sep='')


        servoExceeded = False
        # "under" = given < 0
        # "over" = given < 180
        whichServoExceeded = 6*[False]
        typeOfExceeded = 6*["null"]

        if toDegrees(q[1]) - s[1] < diffCheck:
            if not (math.isnan(q[0]) or
                math.isnan(q[1]) or
                math.isnan(q[2]) or
                math.isnan(q[3]) or
                math.isnan(q[4]) or
                math.isnan(q[5])):
                if default_q[0] - int(round(toDegrees(q[0]))) < 0: q[0] = toRadians(0 + default_q[0]); servoExceeded = True; whichServoExceeded[0] = True; typeOfExceeded[0] = "under"
                if default_q[1] + int(round(toDegrees(q[1]))) < 0: q[1] = toRadians(0 - default_q[1]); servoExceeded = True; whichServoExceeded[1] = True; typeOfExceeded[1] = "under"
                if default_q[2] + int(round(toDegrees(q[2]))) < 0: q[2] = toRadians(0 - default_q[2]); servoExceeded = True; whichServoExceeded[2] = True; typeOfExceeded[2] = "under"
                if default_q[3] + int(round(toDegrees(q[3]))) < 0: q[3] = toRadians(0 - default_q[3]); servoExceeded = True; whichServoExceeded[3] = True; typeOfExceeded[3] = "under"
                if default_q[4] + int(round(toDegrees(q[4]))) < 0: q[4] = toRadians(0 - default_q[4]); servoExceeded = True; whichServoExceeded[4] = True; typeOfExceeded[4] = "under"
                if default_q[5] + int(round(toDegrees(q[5]))) < 0: q[5] = toRadians(0 - default_q[5]); servoExceeded = True; whichServoExceeded[5] = True; typeOfExceeded[5] = "under"
                if default_q[0] - int(round(toDegrees(q[0]))) > 180: q[0] = toRadians(0 - default_q[0]); servoExceeded = True; whichServoExceeded[0] = True; typeOfExceeded[0] = "over"
                if default_q[1] + int(round(toDegrees(q[1]))) > 180: q[1] = toRadians(180 - default_q[1]); servoExceeded = True; whichServoExceeded[1] = True; typeOfExceeded[1] = "over"
                if default_q[2] + int(round(toDegrees(q[2]))) > 180: q[2] = toRadians(180 - default_q[2]); servoExceeded = True; whichServoExceeded[2] = True; typeOfExceeded[2] = "over"
                if default_q[3] + int(round(toDegrees(q[3]))) > 180: q[3] = toRadians(180 - default_q[3]); servoExceeded = True; whichServoExceeded[3] = True; typeOfExceeded[3] = "over"
                if default_q[4] + int(round(toDegrees(q[4]))) > 180: q[4] = toRadians(180 - default_q[4]); servoExceeded = True; whichServoExceeded[4] = True; typeOfExceeded[4] = "over"
                if default_q[5] + int(round(toDegrees(q[5]))) > 180: q[5] = toRadians(180 - default_q[5]); servoExceeded = True; whichServoExceeded[5] = True; typeOfExceeded[5] = "over"
                if servoExceeded:
                    for i in range(6):
                        if whichServoExceeded[i]:
                            print("\tServo motor: q", i+1, " exceeded \"", typeOfExceeded[i], "\"", sep='')

                P1 = getP1()
                P2 = getP2()
                P3 = getP3(q)
                P4 = getP4(P3, q)
                P5 = getP5(P3, q)
                PP = getPP(P5, q)

                q4_from_q5 = getQ4fromQ5(a, b, q) #[0]=a, [1]=b
                q5_from_q4 = getQ5fromQ4(a, b, q) #[0]=a, [1]=b
                PP_fromAlpha_fromQ5 = getPP(P5, [q[0], q[1], q[2], q4_from_q5[0], q[4]], purpose="Alpha from q[4]")
                PP_fromAlpha_fromQ4 = getPP(P5, [q[0], q[1], q[2], q[3], q5_from_q4[0]], purpose="Alpha from q[3]")
                PP_fromAlpha_fromBoth = getPP(P5, [q[0], q[1], q[2], q4_from_q5[0], q5_from_q4[0]], purpose="Alpha from both")

                PP_fromBeta_fromQ5 = getPP(P5, [q[0], q[1], q[2], q4_from_q5[1], q[4]], purpose="Beta from q[4]")
                PP_fromBeta_fromQ4 = getPP(P5, [q[0], q[1], q[2], q[3], q5_from_q4[1]], purpose="Beta from q[3]")
                PP_fromBeta_fromBoth = getPP(P5, [q[0], q[1], q[2], q4_from_q5[1], q5_from_q4[1]], purpose="Beta from both")

                print("----------------------")
                print(" P1: x:", P1[0], " y:", P1[1], " z:", P1[2], sep='')
                print(" P2: x:", P2[0], " y:", P2[1], " z:", P2[2], sep='')
                print(" P3: x:", int(round(P3[0])), " y:", int(round(P3[1])), " z:", int(round(P3[2])), sep='')
                print(" P4: x:", int(round(P4[0])), " y:", int(round(P4[1])), " z:", int(round(P4[2])), sep='')
                print(" P5: x:", int(round(P5[0])), " y:", int(round(P5[1])), " z:", int(round(P5[2])), sep='')
                print(" PP: x:", int(round(PP[0])), " y:", int(round(PP[1])), " z:", int(round(PP[2])), sep='')
                print(" a:", int(toDegrees(a)), " b:", int(toDegrees(b)), sep='')
                print("\n\tError value: x:", float(PP[0])-posX, " y:", float(PP[1])-posY, " z:", float(PP[2])-posZ, sep='')

                print()
                print("{:32}".format("_fromAlpha: from (default)q[4]:"), " x:", int(round(PP_fromAlpha_fromQ5[0])), " y:", int(round(PP_fromAlpha_fromQ5[1])), " z:", int(round(PP_fromAlpha_fromQ5[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromAlpha_fromQ5), "%", sep='') 
                print("{:32}".format("_fromAlpha: from (default)q[3]:"), " x:", int(round(PP_fromAlpha_fromQ4[0])), " y:", int(round(PP_fromAlpha_fromQ4[1])), " z:", int(round(PP_fromAlpha_fromQ4[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromAlpha_fromQ4), "%", sep='') 
                print("{:32}".format("_fromAlpha: from both:"), " x:", int(round(PP_fromAlpha_fromBoth[0])), " y:", int(round(PP_fromAlpha_fromBoth[1])), " z:", int(round(PP_fromAlpha_fromBoth[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromAlpha_fromBoth), "%", sep='') 

                print("{:32}".format("_fromBeta:  from (default)q[4]:"), " x:", int(round(PP_fromBeta_fromQ5[0])), " y:", int(round(PP_fromBeta_fromQ5[1])), " z:", int(round(PP_fromBeta_fromQ5[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromBeta_fromQ5), "%", sep='')
                print("{:32}".format("_fromBeta:  from (default)q[3]:"), " x:", int(round(PP_fromBeta_fromQ4[0])), " y:", int(round(PP_fromBeta_fromQ4[1])), " z:", int(round(PP_fromBeta_fromQ4[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromBeta_fromQ4), "%", sep='')
                print("{:32}".format("_fromBeta:  from both:"), " x:", int(round(PP_fromBeta_fromBoth[0])), " y:", int(round(PP_fromBeta_fromBoth[1])), " z:", int(round(PP_fromBeta_fromBoth[2])), " \taccuracy:", getErrorRate([posX, posY, posZ], PP_fromBeta_fromBoth), "%", sep='')

                print("----------------------")

                s[5] = default_q[5] - int(round(toDegrees(q[5])))
                s[4] = 180 - default_q[4] - int(round(toDegrees(q[4])))
                s[3] = default_q[3] + int(round(toDegrees(q[3])))
                s[2] = 180 - default_q[2] - int(toDegrees(q[2]))
                s[1] = default_q[1] + int(round(toDegrees(q[1])))
                s[0] = default_q[0] - int(round(toDegrees(q[0])))
                for x in range(6):
                    if x == 4:
                        if s[4]<90:
                            servo[4] = getServo4Offset(180 - default_q[4] - s[4]) #type: ignore
                        else:
                            servo[4] = s[4]
                    else:
                        servo[x] = s[x]
            if firstAnglePrint:
                print(
                    " q[0]:", toDegrees(q[0]), 
                    " q[1]:", toDegrees(q[1]),
                    " q[2]:", toDegrees(q[2]),
                    " q[3]:", toDegrees(q[3]),
                    " q[4]:", toDegrees(q[4]), 
                    " q[5]:", toDegrees(q[5]),
                    " Roll:", Roll,
                    " Pitch:", Pitch
                )

            if globalPrint and endAnglePrint:
                print(
                    " q[0]:", servo[0], 
                    " q[1]:", servo[1],
                    " q[2]:", servo[2],
                    " q[3]:", servo[3],
                    " q[4]:", servo[4], 
                    " q[5]:", servo[5],
                    " Roll:", Roll,
                    " Pitch:", Pitch
                )
