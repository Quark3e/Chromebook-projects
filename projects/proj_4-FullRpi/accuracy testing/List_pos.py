#!/usr/bin/env python3

# *add a calibration method for x and y axis

# Import essential libraries
from fileinput import filename
import requests
import cv2
import numpy as np
import imutils
import time
import math
import sys
#from threading import Thread
from IK_module import sendToServo, correctionSetup, toDegrees, toRadians


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

correctionSetup()

servo[0].angle = 90
servo[1].angle = 45
servo[2].angle = 180 - 0
servo[3].angle = 90
servo[4].angle = 90-(90/130)*90
servo[5].angle = 90

d1 = 140; #axial "roll"
d2 = 135; #axial "pitch"
d3 = 70; #axial "pitch"
d4 = 80; #axial "roll"
d5 = 45; #axial "pitch
d6 = 45; #axial "roll" (?)


time.sleep(1)

distanceFromCam = 600 #in mm
webcamFOV = [60, 40] #Unit: degrees, [0] = x fov, [1] = y fov
winScaleX, winScaleY = 1, 1

rowLength = 68
a, b, Y = toRadians(0), toRadians(-90), toRadians(90)

axisFilter = 0.7 #On the new value end
xScaling, yScaling, zScaling = 0.8, 0.8, 1.2
cap = cv2.VideoCapture(0)
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

cv2.namedWindow("Windows")

newSize = (640, 480)

manualInput = True
readX, readY, readZ = 0.1, 0.1, 0.1 #Values that are to be compared with the given values
actualValues = [0, 0]
axisFilter2 = 1

# function to display the coordinates of
# of the points clicked on the image
exitClickEvent = False
def click_event(event, x, y, flags, params):
    global readX, readY, readZ, exitClickEvent, actualValues
    #print("Waiting for left button press...")
    if event == cv2.EVENT_LBUTTONDOWN:
        print("Left button press read")
        actualValues = [x, y]
        readX = int(axisFilter2 * xScaling*(x - newSize[0]*0.5) + (1-axisFilter2) * readX)
        readY = int(axisFilter2 * yScaling*(newSize[1] - y) + (1-axisFilter2) * readY)
        readZ = 1
        print("x:", readX, " y:", readY)
        exitClickEvent = True
 

#Temporary window so the webcam position can be adjusted
#used for calibration of two points for each axis (x y)
while True:
    ret, imgTemp = cap.read()
    img = cv2.resize(imgTemp, newSize)
    img = cv2.flip(img, 1)
    for i in range(1, 5):
        cv2.line(img, (int(newSize[0]*0.5+i*50*(1/xScaling)), int(newSize[1])),  (int(newSize[0]*0.5+i*50*(1/xScaling)), int(newSize[1]-i*50*(1/xScaling))), (0, 255, 0), 1)
        cv2.line(img, (int(newSize[0]*0.5-i*50*(1/xScaling)), int(newSize[1])),  (int(newSize[0]*0.5-i*50*(1/xScaling)), int(newSize[1]-i*50*(1/xScaling))), (0, 255, 0), 1)
        cv2.line(img, (int(newSize[0]*0.5+i*50*(1/xScaling)), int(newSize[1]-i*50*(1/xScaling))),  (int(newSize[0]*0.5-i*50*(1/xScaling)), int(newSize[1]-i*50*(1/xScaling))), (0, 255, 0), 1)

    cv2.circle(img, (int(newSize[0]*0.5), int(newSize[1])), int(50*(1/xScaling)), (255, 255, 255), 1)
    cv2.circle(img, (int(newSize[0]*0.5), int(newSize[1])), int(100*(1/xScaling)), (255, 255, 255), 1)
    cv2.circle(img, (int(newSize[0]*0.5), int(newSize[1])), int(150*(1/xScaling)), (255, 255, 255), 1)
    cv2.circle(img, (int(newSize[0]*0.5), int(newSize[1])), int(200*(1/xScaling)), (255, 255, 255), 1)

    cv2.imshow('Windows', cv2.resize(img, None, fx=winScaleX, fy=winScaleY))
    # cv2.setMouseCallback('Windows', click_event)
    if cv2.waitKey(1) == 32:
        break
    if cv2.waitKey(1) == 27:
        sys.exit()

time.sleep(1)

q1, q2, q3, q4, q5, q6, q7 = 0, 0, 0, 0, 0, 0, 0 #NOTE: q1 = servo[0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos
posX2, posY2, posZ2 = 0.01, 0.01, 0.01
a1, b1 = 0.1, 0.1
q1_default, q2_default, q3_default, q4_default, q5_default, q6_default = 90, 0, 135, 90, 90, 90
zMax = 300
posX, posY, posZ = 0.1, 0.1, 0.1


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

    if posY2 < 0:
        posY2 = 0
    if posY2 == 0:
        if posX2 > 0: q1 = toRadians(90)
        elif posX2 < 0: q1 = toRadians(-90)
        elif posX2 == 0: q1 = toRadians(0)
    else: q1 = math.atan(posX2 / posY2)
    
    try:
        if posOption == '+': q3 = math.acos((pow(posX2, 2) + pow(posY2, 2) + pow(posZ2 - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /(2 * d2 * (d3 + d4)))
        elif posOption == '-': q3 = math.acos((pow(posX2, 2) + pow(posY2, 2) + pow(posZ2 - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /(2 * d2 * (d3 + d4)))
    except ValueError: print("domain error triggered")
    
    lambdaVar = math.atan((posZ2 - d1) / math.sqrt(pow(posX2, 2) + pow(posY2, 2)))
    muVar = math.atan(((d3 + d4) * math.sin(q3)) /(d2 + (d3 + d4) * math.cos(q3)))
    if posOption == '+': q2 = lambdaVar - muVar
    elif posOption == '-':
        #if (s[1] < diffCheck and lambdaVar + muVar < 0) or (lambdaVar + muVar > 0):
        if lambdaVar + muVar > 0: q2 = lambdaVar + muVar #NOTE: the negative muVar value hasnt been solved. so this is a temp. fix 
        else: print("q2 error occured")
        q3 = 0 - q3
    
    a1 = a - q1
    b1 = b - (q2 + q3)
    d5x = (d5+d6) * math.cos(b1) * math.sin(a1)
    #NOTE: The x and y axis of the X1Y1Z1 frame in the paper was reverse (compared to this. The names need to be changed!)
    d5z = (d5+d6) * math.sin(b1)

    if b1 == 0: q4 = 0
    elif b1 < 0 or b1 > 0: q4 = math.atan(d5x / d5z)
    checkVar = math.asin(math.sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
    if math.isnan(checkVar):
        if printText: print("asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / d5)  is NaN")
    else:
        if math.sqrt(pow(d5x, 2) + pow(d5z, 2)) > 90: q5 = toRadians(90)
        else: q5 = math.asin(math.sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
    if d5z < 0: q5 = 0-q5
    if b <= math.pi / 2 and b >= 0 - math.pi / 2: q6 = Y - q4
    elif b >= math.pi / 2 or b <= 0 - math.pi / 2: q6 = math.pi - (Y - q6)
    if printText:
        print(
            " q1: ", toDegrees(q1),
            " q2: ", toDegrees(q2),
            " q3: ", toDegrees(q3),
            " q4: ", toDegrees(q4),
            " q5: ", toDegrees(q5),
            " q6: ", toDegrees(q6),
        )


fileExtension = ".dat"
toReadFile = open(fileName + fileExtension, "r")
toWriteFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_read" + fileExtension, "w") #Note: add +"\n" at the end of each write
accWriteFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_acc" + fileExtension, "w") #Note: add +"\n" at the end of each write
correctWriteFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_correct" + fileExtension, "w") #Note: add +"\n" at the end of each write
resultFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_Result" + fileExtension, "w")

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

cv2.setMouseCallback('Windows', click_event)


def drawGrid(img, spacing):
    height, width = img.shape[:2]
    for x in range(0, width, spacing): cv2.line(img, (x, 0), (x, height), (255, 255, 255), 1)
    for y in range(0, height, spacing): cv2.line(img, (0, y), (width, y), (255, 255, 255), 1)
    return img
def drawCircles(img, center, spacings, atCenter=True): #NOTE: Enter array in "spacings"
    height, width = img.shape[:2]
    for i in range(0, len(spacings)): cv2.circle(img, center, spacings[i], (255, 255, 255), 1)
    return img

def getPos():
    global readX, readY, readZ, newSize, exitClickEvent
    if manualInput:
        exitClickEvent = False
        print("Waiting for key input...")
        while not exitClickEvent:
            ret, imgTemp = cap.read()
            img = drawCircles(cv2.flip(cv2.resize(imgTemp, newSize), 1), (int(newSize[0]*0.5), int(newSize[1])),
            [50*(1/xScaling), 100*(1/xScaling), 150*(1/xScaling), 200*(1/xScaling)])

            cv2.imshow('Windows', cv2.resize(img, None, fx=winScaleX, fy=winScaleY))

            if cv2.waitKey(1) == 27:
                print("Exiting.", end='')
                for i in range(3):
                    time.sleep(0.5)
                    print(" .", end='')
                print()
                sys.exit()
    elif not manualInput:
        ret, imgTemp = cap.read()
        img = drawCircles(cv2.flip(cv2.resize(imgTemp, newSize), 1), (int(newSize[0]*0.5), int(newSize[1])),
        [50*(1/xScaling), 100*(1/xScaling), 150*(1/xScaling), 200*(1/xScaling)])
        
        into_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        L_limit = np.array(L_values)
        U_limit = np.array(U_values)
        threshold = cv2.inRange(into_hsv, L_limit, U_limit)
        filtered = cv2.bitwise_and(img, img, mask = threshold)
        filtered[filtered!=0] = 255 #change to value of everything that's not 0(black) to 255(white)
        filtered = cv2.erode(filtered, np.ones((5,5), np.uint8), iterations = 1)
        filtered = cv2.dilate(filtered, np.ones((5,5), np.uint8), iterations = 1)
        gray_image = cv2.cvtColor(filtered, cv2.COLOR_BGR2GRAY)
        ret,thresh = cv2.threshold(gray_image,127,255, 0)
        # contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_NONE)
        contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
        print("Number of contours ", len(contours))
        for c in contours:
            M = cv2.moments(c)
            M = cv2.moments(thresh)
            if M["m00"] != 0:
                cX = int(M["m10"] / M["m00"])
                cY = int(M["m01"] / M["m00"])
            else:
                cX, cY = 0, 0
            cv2.circle(img, (cX, cY), 5, (255, 255, 255), -1)
            cv2.putText(img, "centroid" + str(cv2.contourArea(contours[0])), (cX - 25, cY - 25),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
            readX = int(axisFilter2 * xScaling*(cX - newSize[0]*0.5) + (1-axisFilter2) * readX)
            readY = int(axisFilter2 * yScaling*(newSize[1] - cY) + (1-axisFilter2) * readY)
            readZ = int(axisFilter2 * zScaling*(zMax - (M["m00"] / zDefaultVal)*zMax) + (1-axisFilter2) * readZ)
    if globalPrint: print(coord, end="")
    if showImage and not manualInput:
        stacked = np.hstack((img, filtered))
        cv2.imshow('Windows', cv2.resize(stacked, None, fx=winScaleX, fy=winScaleY))
    if manualInput and showImage: cv2.imshow('Windows', cv2.resize(img, None, fx=winScaleX, fy=winScaleY))


for n in range(rowLength):
    # get xyz a b values
    ##check values just in case that they don't return NaN angles
    # send values to robot arm (i.e solve angles and send those)
    # read store the coordinate that's read from openCV in a new, corresponding file
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
    sendToServo()
    
    if not manualInput:
        time.sleep(2)
    #Read values from webcam
    getPos()
    readX = 0 - readX
    toWriteFile.write(str(readX) + " " + str(readY) + " " + str(readZ) + "\n")
    correctWriteFile.write(
        str(math.tan((abs(newSize[0]*0.5-actualValues[0])*(webcamFOV[0]*0.5))/newSize[0])*distanceFromCam) + " " +
        # str(math.atan(toRadians((abs(actualValues[0] - newSize[0]*0.5) * webcamFOV[0])/(newSize[0]*0.5))) * distanceFromCam) + " " +
        str(readY) + " " +
        # str(math.atan(toRadians((abs(actualValues[1] - newSize[1]*0.5) * webcamFOV[1])/(newSize[1]*0.5))) * distanceFromCam) + " " +
        str(readZ) + "\n"
        )
    accWriteFile.write(str(abs(readX-posX)) + " " + str(abs(readY-posY)) + " " + str(1) + "\n")
    resultFile.write(
        str(posX) + " " + 
        str(posY) + " " + 
        str(posZ) + " " + 
        str(math.tan((abs(newSize[0]*0.5-actualValues[0])*(webcamFOV[0]*0.5))/newSize[0])*distanceFromCam) + " " +
        str(readY) + " " +
        str(posZ) + "\n" 
        # Even though posZ is not measured it's still added in the "_Result"
        #-file in case a seconds webcam for z-axis measuring is added later on.
    )

    # Press Esc key to exit
    if cv2.waitKey(1) == 27:
        break
        
if showImage:
    cv2.destroyAllWindows()
pca.deinit()
toReadFile.close()
toWriteFile.close()
accWriteFile.close()
correctWriteFile.close()
