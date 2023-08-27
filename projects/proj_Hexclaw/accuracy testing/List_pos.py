#!/usr/bin/env python3

# *add a calibration method for x and y axis
# define center origo where everything is calibrated off of that.

# Import essential libraries
from fileinput import filename
import requests
import cv2 #type: ignore
import numpy as np
import imutils
import time
import math
import sys
#from threading import Thread
from IK_module import sendToServo, toDegrees, toRadians, getAngles, default_q


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

fileName = "basicTest_2" #Dont enter filetype
hardSetAngle = [0, -60]
useHardSetAngle = True

if useHardSetAngle:
    ans = input(" Enter alpha and beta values to test (with space): ").split()
    if ans[0] == "exit":
        sys.exit()
    hardSetAngle = [int(ans[0]), int(ans[1])]


servo[0].angle = 90
servo[1].angle = 45
servo[2].angle = 180 - 0
servo[3].angle = 90
servo[4].angle = 90-(90/130)*90
servo[5].angle = 90


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
readX, readY, readZ = 0.1, 0.1, 1 #Values that are to be compared with the given values
actualValues = [0, 0]
axisFilter2 = 1

# function to display the coordinates of
# of the points clicked on the image
exitClickEvent = False
def click_event(event, x, y, flags, params):
    global readX, readY, exitClickEvent, actualValues
    #print("Waiting for left button press...")
    if event == cv2.EVENT_LBUTTONDOWN:
        print("Left button press read")
        actualValues = [x, y]
        readX = int(axisFilter2 * xScaling*(x - newSize[0]*0.5) + (1-axisFilter2) * readX)
        readY = int(axisFilter2 * yScaling*(newSize[1] - y) + (1-axisFilter2) * readY)
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

    cv2.imshow('Windows', cv2.resize(img, None, fx=winScaleX, fy=winScaleY)) # type: ignore
    # cv2.setMouseCallback('Windows', click_event)
    if cv2.waitKey(1) == 32:
        break
    if cv2.waitKey(1) == 27:
        sys.exit()

time.sleep(1)

q = [0]*6 #NOTE: q = q[0] = servo[0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos
PP = [float(0)]*3
zMax = 300
PP[0], PP[1], PP[2] = 0.1, 0.1, 0.1


fileExtension = ".dat"
toReadFile = open(fileName + fileExtension, "r")
toWriteFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_read" + fileExtension, "w") #Note: add +"\n" at the end of each write
accWriteFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_acc" + fileExtension, "w") #Note: add +"\n" at the end of each write
correctWriteFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_correct" + fileExtension, "w") #Note: add +"\n" at the end of each write
resultFile = open("testResult/" + fileName + "_" + str(hardSetAngle[0]) + "." + str(hardSetAngle[1]) + "_Result" + fileExtension, "w")

coord = ""

def check_qMinMax():
    global q
    if default_q[0] - int(round(toDegrees(q[0]))) < 0: q[0] = toRadians(0 + default_q[0]) # type: ignore
    if default_q[1] + int(round(toDegrees(q[1]))) < 0: q[1] = toRadians(0 - default_q[1]) # type: ignore
    if default_q[2] + int(round(toDegrees(q[2]))) < 0: q[2] = toRadians(0 - default_q[2]) # type: ignore
    if default_q[3] + int(round(toDegrees(q[3]))) < 0: q[3] = toRadians(0 - default_q[3]) # type: ignore
    if default_q[4] + int(round(toDegrees(q[4]))) < 0: q[4] = toRadians(0 - default_q[4]) # type: ignore
    if default_q[5] + int(round(toDegrees(q[5]))) < 0: q[5] = toRadians(0 - default_q[5]) # type: ignore
    if default_q[0] - int(round(toDegrees(q[0]))) > 180: q[0] = toRadians(0 - default_q[0]) # type: ignore
    if default_q[1] + int(round(toDegrees(q[1]))) > 180: q[1] = toRadians(180 - default_q[1]) # type: ignore
    if default_q[2] + int(round(toDegrees(q[2]))) > 180: q[2] = toRadians(180 - default_q[2]) # type: ignore
    if default_q[3] + int(round(toDegrees(q[3]))) > 180: q[3] = toRadians(180 - default_q[3]) # type: ignore
    if default_q[4] + int(round(toDegrees(q[4]))) > 180: q[4] = toRadians(180 - default_q[4]) # type: ignore
    if default_q[5] + int(round(toDegrees(q[5]))) > 180: q[5] = toRadians(180 - default_q[5]) # type: ignore

def findAlphaBeta(PP, posOption):
    global a, b
    #Find the a, b combination that is valid
    breakVal = False
    for beta in range(-90, 0):
        for alpha in range(-90, 90):
            q = getAngles(PP, toRadians(alpha), toRadians(beta), Y, posOption, 1, 1, False)
            if not (math.isnan(q[0]) or
                math.isnan(q[1]) or
                math.isnan(q[2]) or
                math.isnan(q[3]) or
                math.isnan(q[4]) or
                math.isnan(q[5])):
                a = toRadians(alpha)
                b = toRadians(beta)
                breakVal = True
                break
        if breakVal:
            break
    if not breakVal:
        for beta in range(0, 90):
            for alpha in range(-90, 90):
                q = getAngles(PP, toRadians(alpha), toRadians(beta), Y, posOption, 1, 1, False)
                if not (math.isnan(q[0]) or
                    math.isnan(q[1]) or
                    math.isnan(q[2]) or
                    math.isnan(q[3]) or
                    math.isnan(q[4]) or
                    math.isnan(q[5])):
                    a = toRadians(alpha)
                    b = toRadians(beta)
                    breakVal = True
                    break
            if breakVal:
                break
    return a, b


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
    cv2.setMouseCallback('Windows', click_event)
    if manualInput:
        exitClickEvent = False
        print("Waiting for key input...")
        while not exitClickEvent:
            ret, imgTemp = cap.read()
            img = drawCircles(cv2.flip(cv2.resize(imgTemp, newSize), 1), (int(newSize[0]*0.5), int(newSize[1])),
            [50*(1/xScaling), 100*(1/xScaling), 150*(1/xScaling), 200*(1/xScaling)])

            cv2.imshow('Windows', cv2.resize(img, None, fx=winScaleX, fy=winScaleY)) # type: ignore

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
        filtered = cv2.erode(filtered, np.ones((5,5), np.uint8), iterations = 2)
        filtered = cv2.dilate(filtered, np.ones((5,5), np.uint8), iterations = 2)
        gray_image = cv2.cvtColor(filtered, cv2.COLOR_BGR2GRAY)
        ret,thresh = cv2.threshold(gray_image,127,255, 0)
        # contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_NONE)
        contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
        print("Number of contours ", len(contours))
        for c in contours:
            M = cv2.moments(c)
            # M = cv2.moments(thresh)
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
        stacked = np.hstack((img, filtered)) # type: ignore
        cv2.imshow('Windows', cv2.resize(stacked, None, fx=winScaleX, fy=winScaleY)) # type: ignore
    if manualInput and showImage: cv2.imshow('Windows', cv2.resize(img, None, fx=winScaleX, fy=winScaleY)) # type: ignore

def getAngle():
    # Marks two dots which are visible on the img/frame displayed on the window
    # a line between first and second dot is formed the moment the second dot is placed, calculates and displays angle, pause, continues to end of function
    global readAlpha, readBeta, newSize, exitClickEvent



for n in range(rowLength):
    # get xyz a b values
    ##check values just in case that they don't return NaN angles
    # send values to robot arm (i.e solve angles and send those)
    # read store the coordinate that's read from openCV in a new, corresponding file
    # repeat

    #Read line and get the xyz value
    fileLine = toReadFile.readline()
    print("fileLine:", fileLine, end='', sep='')
    PP[0] = float(fileLine[0:fileLine.find(" ")+1])
    print("PP[0]:", fileLine[0:fileLine.find(" ")+1], end='', sep='')
    fileLine = fileLine.replace(fileLine[0:fileLine.find(" ")+1], '', 1)
    PP[1] = float(fileLine[0:fileLine.find(" ")+1])
    print(" PP[1]:", fileLine[0:fileLine.find(" ")+1], end='', sep='')
    fileLine = fileLine.replace(fileLine[0:fileLine.find(" ")+1], '', 1)
    PP[2] = float(fileLine[0:fileLine.find(" ")+1])
    print(" PP[2]:", fileLine[0:fileLine.find(" ")+1], end='\n', sep='')
    fileLine = fileLine.replace(fileLine[0:fileLine.find(" ")+1], '', 1)
    a = toRadians(float(fileLine[0:fileLine.find(" ")+1]))
    # print(" a:", fileLine[0:fileLine.find(" ")+1], end='', sep='')
    fileLine = fileLine.replace(fileLine[0:fileLine.find(" ")+1], '', 1)
    b = toRadians(float(fileLine[0:fileLine.find("\n")+1]))
    # print(" b:", fileLine[0:fileLine.find(" ")+1], "\n", sep='')

    if useHardSetAngle:
        a = toRadians(hardSetAngle[0])
        b = toRadians(hardSetAngle[1])

    # "under" = given < 0
    # "over" = given < 180
    servoExceeded = False
    whichServoExceeded = 6*[False]
    typeOfExceeded = 6*["null"]
    #Check if values are NaN
    q = getAngles(PP, a, b, Y, posOption, 1, 1, globalPrint)
    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=2)
    
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
    accWriteFile.write(str(abs(readX-PP[0])) + " " + str(abs(readY-PP[1])) + " " + str(1) + "\n")
    resultFile.write(
        str(PP[0]) + " " + 
        str(PP[1]) + " " + 
        str(PP[2]) + " " + 
        str(math.tan((abs(newSize[0]*0.5-actualValues[0])*(webcamFOV[0]*0.5))/newSize[0])*distanceFromCam) + " " +
        str(readY) + " " +
        str(PP[2]) + "\n" 
        # Even though PP[2] is not measured it's still added in the "_Result"
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
