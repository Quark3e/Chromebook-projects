#!/usr/bin/env python3

# Import essential libraries
import requests
import cv2 #type: ignore
import numpy as np
import imutils
import time
import math
import sys
#from threading import Thread

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
         servo.Servo(pca.channels[8]),
         servo.Servo(pca.channels[3]),
         servo.Servo(pca.channels[4]),
         servo.Servo(pca.channels[5])]
for i in range(6):
    servo[i].set_pulse_width_range(500, 2500)

servo[0].angle = 90
servo[1].angle = 90
servo[2].angle = 180 - 90
servo[3].angle = 90
servo[4].angle = 180 - 90
servo[5].angle = 90

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


time.sleep(1)

axisFilter = 0.7 #On the new value end
xScaling, yScaling, zScaling = 0.8, 0.8, 1.2
cap = cv2.VideoCapture(0)
brightVal = 75
zDefaultVal = 3000000
L_values, U_values = [36, 72, 8], [86, 240, 204]
#L_values, U_values = input("Enter L- and U-values without comma: "), input()
diffCheck = 100
showImage = False
globalPrint = False
endAnglePrint = False
firstAnglePrint = False
posOption = '-'
if showImage:
    cv2.namedWindow("Windows")


d1 = 140; #axial "roll"
d2 = 135; #axial "pitch"
d3 = 70; #axial "pitch"
d4 = 80; #axial "roll"
d5 = 45; #axial "pitch
d6 = 30; #axial "roll" (?)

q1, q2, q3, q4, q5, q6, q7 = 0, 0, 0, 0, 0, 0, 0 #NOTE: q1 = servo[0]
q = [0,0,0,0,0,0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos

posX2, posY2, posZ2 = 0.01, 0.01, 0.01
a1, b1 = 0.1, 0.1

def toDegrees(radians):
    return (radians * 180) / math.pi
def toRadians(degrees):
    return (degrees * math.pi) / 180


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

def getPos():
    global posX, posY, posZ
    ret, imgTemp = cap.read()
    img = cv2.resize(imgTemp, (640, 480))
    img = cv2.flip(img, 1)
    into_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    #L, U color values: [[62, 108, 0], [85, 238, 249]]
    L_limit = np.array(L_values)
    U_limit = np.array(U_values)
    threshold = cv2.inRange(into_hsv, L_limit, U_limit)
    filtered = cv2.bitwise_and(img, img, mask = threshold)
    #filtered = increase_brightness(filtered, brightVal)
    filtered[filtered!=0] = 255 #change to value of everything that's not 0 to 255(white)
    filtered = cv2.erode(filtered, np.ones((5,5), np.uint8), iterations = 1)
    filtered = cv2.dilate(filtered, np.ones((5,5), np.uint8), iterations = 1)
    gray_image = cv2.cvtColor(filtered, cv2.COLOR_BGR2GRAY)
    ret,thresh = cv2.threshold(gray_image,127,255,0)
    contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_NONE)
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
        coord = "x:" + str(int(xScaling*(cX - 320))) + " y:" + str(int(yScaling*(480 - cY))) + " z:" +  str(int(zScaling*(zMax - (M["m00"] / zDefaultVal)*zMax))) #str(cv2.contourArea(contours))
        posX = int(axisFilter * xScaling*(cX - 320) + (1-axisFilter) * posX)
        posY = int(axisFilter * yScaling*(480 - cY) + (1-axisFilter) * posY)
        posZ = int(axisFilter * zScaling*(zMax - (M["m00"] / zDefaultVal)*zMax) + (1-axisFilter) * posZ)
    if globalPrint:
        print(coord, end="")
    if showImage:
        stacked = np.hstack((img, filtered))
        cv2.imshow('Windows', cv2.resize(stacked, None, fx=0.7, fy=0.7))


q1_default = 90
q2_default = 0
q3_default = 135
q4_default = 90
q5_default = 90
q6_default = 90

zMax = 300
a, b, Y = 0, -45, 90
posX, posY, posZ = 0.1, 0.1, 0.1
coord = ""

def check_qMinMax():
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
            q = getAngles(posX, posY, posZ, toRadians(alpha), toRadians(beta), Y, posOption, 1, 1, False)
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
                q = getAngles(posX, posY, posZ, toRadians(alpha), toRadians(beta), Y, posOption, 1, 1, False)
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


while True:
    xTemp, yTemp, zTemp = input("Enter X Y Z coord:").split()
    posX, posY, posZ = float(xTemp), float(yTemp), float(zTemp)


    q = getAngles(posX, posY, posZ, a, b, Y, posOption, 1, 1, globalPrint)
    #q3 = toRadians(toDegrees(q3) - 20)
    #q5 -= toRadians(30)
    if not (math.isnan(q1) or
        math.isnan(q2) or
        math.isnan(q3) or
        math.isnan(q4) or
        math.isnan(q5) or
        math.isnan(q6)):
        check_qMinMax()
        #t6 = Thread(target=smoothServo, args=[q6_default - int(round(toDegrees(q6))), s[5], servo[5]])
        #t5 = Thread(target=smoothServo, args=[180 - q5_default - int(round(toDegrees(q5))), s[4], servo[4]])
        #t4 = Thread(target=smoothServo, args=[q4_default + int(round(toDegrees(q4))), s[3], servo[3]])
        #t3 = Thread(target=smoothServo, args=[180 - q3_default - int(round(toDegrees(q3))), s[2], servo[2]])
        #t2 = Thread(target=smoothServo, args=[q2_default + int(round(toDegrees(q2))), s[1], servo[1]])
        #t1 = Thread(target=smoothServo, args=[q1_default - int(round(toDegrees(q1))), s[0], servo[0]])
        #t6.start()
        #t5.start()
        #t4.start()
        #t3.start()
        #t2.start()
        #t1.start()
        #t6.join()
        #t5.join()
        #t4.join()
        #t3.join()
        #t2.join()
        #t1.join()
        #smoothServo(q6_default - int(round(toDegrees(q6))), s[5], servo[5])
        #smoothServo(180 - q5_default - int(round(toDegrees(q5))), s[4], servo[4])
        #smoothServo(q4_default + int(round(toDegrees(q4))), s[3], servo[3])
        #smoothServo(180 - q3_default - int(round(toDegrees(q3))), s[2], servo[2])
        #smoothServo(q2_default + int(round(toDegrees(q2))), s[1], servo[1])
        #smoothServo(q1_default - int(round(toDegrees(q1))), s[0], servo[0])
        s[5] = q6_default - int(round(toDegrees(q6)))
        s[4] = 180 - q5_default - int(round(toDegrees(q5)))
        s[3] = q4_default + int(round(toDegrees(q4)))
        s[2] = 180 - q3_default - int(round(toDegrees(q3)))
        s[1] = q2_default + int(round(toDegrees(q2)))
        s[0] = q1_default - int(round(toDegrees(q1)))

        for x in range(7):
            servo[x-1].angle = s[x-1]
        
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

    # Press Esc key to exit
    if cv2.waitKey(1) == 27:
        break
        
if showImage:
    cv2.destroyAllWindows()
pca.deinit()

