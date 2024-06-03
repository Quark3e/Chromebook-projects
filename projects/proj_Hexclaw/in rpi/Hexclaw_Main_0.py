#!/usr/bin/env python3

#/*
# OpenCV Color Tracking and ADXL345 accelerometer
# "Automatic" Mode where the coordinate and orientation variables are given by opencv color tracking and accelerometer
# uses python threading where cv2::read() is run in a separate thread in a class
#*/

# Import essential libraries
import requests
import cv2 #type: ignore
import numpy as np
import imutils
import time
import math
import adafruit_adxl34x # type: ignore
import sys
from threading import Thread
import RPi.GPIO as GPIO #type: ignore #type: ignore

sys.path.append('/home/pi/Chromebook-projects/projects/proj_Hexclaw')

from IK_module import *


class WebcamVideoStream:
    def __init__(self, src=0):
        # initialize the video camera stream and read the first frame
		# from the stream
        self.stream = cv2.VideoCapture(src)
        (self.grabbed, self.frame) = self.stream.read()
		# initialize the variable used to indicate if the thread should
		# be stopped
        self.stopped = False
    
    def start(self):
		# start the thread to read frames from the video stream
        Thread(target=self.update, args=()).start()
        return self
    def update(self):
		# keep looping infinitely until the thread is stopped
        while True:
			# if the thread indicator variable is set, stop the thread
            if self.stopped:
                return
			# otherwise, read the next frame from the stream
            (self.grabbed, self.frame) = self.stream.read()
    def read(self):
		# return the frame most recently read
        return self.frame
    def stop(self):
		# indicate that the thread should be stopped
        self.stopped = True


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


servo[0].angle = 90
servo[1].angle = 15
servo[2].angle = 150
servo[3].angle = 90
servo[4].angle = 180
servo[5].angle = 90

GPIO.setmode(GPIO.BCM)
ledRelay=23
GPIO.setup(ledRelay,GPIO.OUT)
GPIO.output(ledRelay, True)
sendToServo(servo,[90,115,145,90,125,90],0,mode=2)

# plt.xlabel("x - axis")
# plt.ylabel("y - axis")
# plt.title("xyz value over time")
# xGraph = 100*[0]
# yGraph = 100*[0]
# zGraph = 100*[0]
# plt.plot([-150, -100, -50, 0, 50, 100, 150], [0, 50, 100, 150, 100, 50, 0], [0, 50, 100, 150, 100, 50, 0])

time.sleep(1)

axisFilter = 0.3 #On the new value end
accelFilter = 0.1
xScaling, yScaling, zScaling = 0.8, 0.8, 1.2


cap = WebcamVideoStream(src=0).start()


brightVal = 75
zDefaultVal = 3000000

useDefaultLimit = False
defaultLimitIndex = 0

def nothing(x):
    pass

cv2.namedWindow("Webcam - XY")
cv2.createTrackbar("L - H", "Webcam - XY", 0, 179, nothing)
cv2.createTrackbar("L - S", "Webcam - XY", 0, 255, nothing)
cv2.createTrackbar("L - V", "Webcam - XY", 0, 255, nothing)
cv2.createTrackbar("U - H", "Webcam - XY", 179, 179, nothing)
cv2.createTrackbar("U - S", "Webcam - XY", 255, 255, nothing)
cv2.createTrackbar("U - V", "Webcam - XY", 255, 255, nothing)
while True:
    frame = cap.read()
    frame = cv2.flip(frame, 1 ) 
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    l_h = cv2.getTrackbarPos("L - H", "Webcam - XY")
    l_s = cv2.getTrackbarPos("L - S", "Webcam - XY")
    l_v = cv2.getTrackbarPos("L - V", "Webcam - XY")
    u_h = cv2.getTrackbarPos("U - H", "Webcam - XY")
    u_s = cv2.getTrackbarPos("U - S", "Webcam - XY")
    u_v = cv2.getTrackbarPos("U - V", "Webcam - XY")
    lower_range = np.array([l_h, l_s, l_v])
    upper_range = np.array([u_h, u_s, u_v])
    mask = cv2.inRange(hsv, lower_range, upper_range) 
    res = cv2.bitwise_and(frame, frame, mask=mask)
    mask_3 = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)
    stacked = np.hstack((mask_3,frame,res))    
    cv2.imshow('Webcam - XY',cv2.resize(stacked,None,fx=0.4,fy=0.4)) # type: ignore
    key = cv2.waitKey(1)
    if key == 27:
        sys.exit()    
    if key == ord('s'):
        thearray = [[l_h,l_s,l_v],[u_h, u_s, u_v]]
        L_values, U_values = [l_h,l_s,l_v],[u_h, u_s, u_v]
        print(thearray)        
        np.save('hsv_value',thearray)
        break
    elif key == ord('l'):
        useDefaultLimit = True
        defaultLimitIndex = 0
        break
    elif key == ord('k'):
        useDefaultLimit = True
        defaultLimitIndex = 1
        break

cv2.destroyAllWindows()

if useDefaultLimit:
    if defaultLimitIndex == 0:
        L_values, U_values = [60, 147, 88], [100, 255, 169] #plexgear
    elif defaultLimitIndex == 1:
        L_values, U_values = [68, 207, 92], [112, 255, 238] #trust exis webcam


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

a1, b1 = 0.1, 0.1


def readAccelerometer():
    global X_out, Y_out, Z_out, Roll, Pitch, roll, pitch
    X_out, Y_out, Z_out = accelerometer.acceleration
    #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = math.atan(Y_out / math.sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    roll = math.atan(-1 * X_out / math.sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    #filter
    Roll = (1-accelFilter) * Roll + accelFilter * roll
    Pitch = (1-accelFilter) * Pitch + accelFilter * pitch


zMax = 300
a, b, Y = 0, 0, 0
PP = [0.1]*3
coord = ""

while True:
    #get accelerations, roll and pitch
    readAccelerometer()

    imgTemp = cap.read()
    img = cv2.resize(imgTemp, (640, 480))
    img = cv2.flip(img, 1)
    into_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    #L, U color values: [[62, 108, 0], [85, 238, 249]]
    L_limit = np.array(L_values) # type: ignore
    U_limit = np.array(U_values) # type: ignore
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
        PP[0] = int(axisFilter * xScaling*(cX - 320) + (float(1)-axisFilter) * PP[0])
        PP[1] = int(axisFilter * yScaling*(480 - cY) + (float(1)-axisFilter) * PP[1])
        PP[2] = int(axisFilter * zScaling*(zMax - (M["m00"] / zDefaultVal)*zMax) + (float(1)-axisFilter) * PP[2])
    if globalPrint or printCoord: print(coord)
    if showImage:
        stacked = np.hstack((img, filtered))
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
    isReachable=[True]
    q = getAngles(PP,a,b,Y,'-',positionIsReachable=isReachable)
    servoExceeded = False
    # "under" = given < 0
    # "over" = given < 180
    whichServoExceeded = 6*[False]
    typeOfExceeded = 6*["null"]

    if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)

    # Press Esc key to exit
    if cv2.waitKey(1) == 27:
        break
    # plt.show()
        
if showImage:
    cv2.destroyAllWindows()
sendToServo(servo,[90,15,150,90,180,90],1,mode=2)
GPIO.output(ledRelay, False)
cap.stop()
pca.deinit()

