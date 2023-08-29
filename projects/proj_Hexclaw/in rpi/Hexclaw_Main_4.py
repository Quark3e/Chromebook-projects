#!/usr/bin/env python3

#/*
# OpenCV HSV-range and ADXL345 accelerometer with two video feeds (webcam[0] and webcam[1])
# "Automatic" Mode where the coordinate and orientation variables are given by opencv color tracking and accelerometer
#*/

# Import essential libraries
import cv2 #type: ignore
import numpy as np
import imutils
import math
import adafruit_adxl34x # type: ignore
import sys
import requests
from time import perf_counter, sleep
from threading import Thread

sys.path.append('/home/pi/Chromebook-projects/projects/proj_Hexclaw')

from IK_module import sendToServo, toRadians, toDegrees, getAngles


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


class IpVideoStream:
    def __init__(self, ipaddress="192.168.1.124:8080"):
        # Replace the below URL with your own. Make sure to add "/shot.jpg" at last.
        url_start = "http://"
        url_end = "/shot.jpg"
        self.address = url_start + ipaddress + url_end
        
        img_resp = requests.get(self.address)
        img_arr = np.array(bytearray(img_resp.content), dtype=np.uint8)
        self.frame = cv2.imdecode(img_arr, -1)

        self.stopped = False
    def start(self):
        Thread(target=self.update_ip, args=()).start()
        return self
    def update_ip(self):
        while True:
            if self.stopped:
                return
            img_temp = requests.get(self.address)
            img_array = np.array(bytearray(img_temp.content), dtype=np.uint8)
            self.frame = cv2.imdecode(img_array, -1)
    def read(self):
        return self.frame
    def stop(self):
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

sleep(1)

axisFilter = 1 #On the new value end
accelFilter = 0.1
xScaling, yScaling, zScaling = 0.8, 0.6, 0.5


camOption = input("Which video source to use as second webcam:\n 1.Exis webcam\n 2.ip webcam\n input:")

if camOption == "exit":
    sys.exit()
elif int(camOption) == 1: webcam = (WebcamVideoStream(src=0).start(), WebcamVideoStream(src=2).start())
elif int(camOption) == 2:
    tempAddress = input("Enter ip address: ")
    if tempAddress == "default": webcam = (WebcamVideoStream(src=0).start(), IpVideoStream().start())
    else: webcam = (WebcamVideoStream(src=0).start(), IpVideoStream(ipaddress=tempAddress).start())


brightVal = 75
zDefaultVal = 3000000
zOffset = 100


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
    frame = webcam[0].read() # type: ignore
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
    cv2.imshow('Webcam - XY',cv2.resize(stacked,None,fx=0.4,fy=0.4))   # type: ignore  
    key = cv2.waitKey(1)
    if key == 27:
        sys.exit()    
    if key == ord('s'):
        thearray = [[l_h,l_s,l_v],[u_h, u_s, u_v]]
        L_values0, U_values0 = [l_h,l_s,l_v],[u_h, u_s, u_v]
        print(thearray)        
        np.save('hsv_value',thearray)
        break
    if key == ord('k'):
        L_values0, U_values0 = [44, 165, 0], [102, 255, 121]
        break
cv2.destroyAllWindows()
# L_values0, U_values0 = [48, 152, 19], [94, 249, 77]


cv2.namedWindow("Webcam - YZ")
cv2.createTrackbar("L - H", "Webcam - YZ", 0, 179, nothing)
cv2.createTrackbar("L - S", "Webcam - YZ", 0, 255, nothing)
cv2.createTrackbar("L - V", "Webcam - YZ", 0, 255, nothing)
cv2.createTrackbar("U - H", "Webcam - YZ", 179, 179, nothing)
cv2.createTrackbar("U - S", "Webcam - YZ", 255, 255, nothing)
cv2.createTrackbar("U - V", "Webcam - YZ", 255, 255, nothing)
while True:
    frame = webcam[1].read() # type: ignore
    frame = cv2.flip(frame, 1 ) 
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    l_h = cv2.getTrackbarPos("L - H", "Webcam - YZ")
    l_s = cv2.getTrackbarPos("L - S", "Webcam - YZ")
    l_v = cv2.getTrackbarPos("L - V", "Webcam - YZ")
    u_h = cv2.getTrackbarPos("U - H", "Webcam - YZ")
    u_s = cv2.getTrackbarPos("U - S", "Webcam - YZ")
    u_v = cv2.getTrackbarPos("U - V", "Webcam - YZ")
    lower_range = np.array([l_h, l_s, l_v])
    upper_range = np.array([u_h, u_s, u_v])
    mask = cv2.inRange(hsv, lower_range, upper_range) 
    res = cv2.bitwise_and(frame, frame, mask=mask)
    mask_3 = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)    
    stacked = np.hstack((mask_3,frame,res))    
    cv2.imshow('Webcam - YZ',cv2.resize(stacked,None,fx=0.4,fy=0.4)) # type: ignore
    key = cv2.waitKey(1)
    if key == 27:
        sys.exit()    
    if key == ord('s'):
        thearray = [[l_h,l_s,l_v],[u_h, u_s, u_v]]
        L_values1, U_values1 = [l_h,l_s,l_v],[u_h, u_s, u_v]
        print(thearray)        
        np.save('hsv_value',thearray)
        break
    if key == ord('k'):
        L_values1, U_values1 = [56, 93, 30], [93, 255, 237]
cv2.destroyAllWindows()

# L_values0, U_values0 = [63, 163, 36], [83, 255, 171]
# L_values1, U_values1 = [145, 125, 40], [176, 255, 255]

diffCheck = 100
showImage = True
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
    Roll = (1-accelFilter) * Roll + accelFilter * roll
    Pitch = (1-accelFilter) * Pitch + accelFilter * pitch


zMax = 300
a, b, Y = 90, 0.1, 90
PP = [float(0)]*3
coord = ""
PP[0], PP[1], PP[2] = 0, 200, 150

def getCoords(img, valueSrc, resizeImg = False): #Returns [x, y, z, frame/img, filtered]
    global coord, PP
    # PP[0], PP[1], PP[2] = 0.1, 0.1, 0.1
     #Read frame from webcam
    if True:
        img = cv2.resize(img, (640, 480))
    img = cv2.flip(img, 1)
    into_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    #L, U color values: [[62, 108, 0], [85, 238, 249]]
    if valueSrc == 0:
        L_limit = np.array(L_values0)
        U_limit = np.array(U_values0)
    elif valueSrc == 1:
        L_limit = np.array(L_values1)
        U_limit = np.array(U_values1)
    threshold = cv2.inRange(into_hsv, L_limit, U_limit) # type: ignore
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
        if valueSrc == 0:
            # print(cX)
            PP[0] = int(axisFilter * xScaling*(cX - 320) + (float(1)-axisFilter) * PP[0])
            PP[1] = int(axisFilter * yScaling*(480 - cY) + (float(1)-axisFilter) * PP[1])
        elif valueSrc == 1:
            PP[2] = int(axisFilter * zScaling*(480 - cY) + (float(1)-axisFilter) * PP[2])
        # PP[2] = int(axisFilter * zScaling*(zMax - (M["m00"] / zDefaultVal)*zMax) + (float(1)-axisFilter) * PP[2])
        # coord = "x:" + str(PP[0]) + " y:" + str(PP[1]) + " z:" + str(PP[2])
    return [PP[0], PP[1], PP[2], img, filtered]

def threadTest(valueSrc, boolVal):
    if valueSrc == 0:
        img = webcam[0].read() # type: ignore
    elif valueSrc == 1:
        img = webcam[1].read() # type: ignore

    getCoords(img, valueSrc, boolVal) # type: ignore

getTime = False

while True:
    readAccelerometer()
    #get accelerations, roll and pitch
    if cv2.waitKey(1) == 32:
        getTime = True
    
    # if getTime:
    #     start_time = perf_counter()
    # readAccelerometer()
    # if getTime:
    #     end_time = perf_counter()
    #     print(" readAccelerometer:", end_time-start_time)
    #     start_time = perf_counter()
    # ret0, temp0 = webcam[0].read()
    # ret1, temp1 = webcam[1].read()
    # if getTime:
    #     end_time = perf_counter()
    #     print(" webcam.read():", end_time-start_time)
    
    # source0 = getCoords(temp0, 0, True) #X Y, [0] [1]
    # source1 = getCoords(temp1, 1, False) #Y Z, [1]

    if getTime:
        start_time = perf_counter()

    threadTest(0, True)
    threadTest(1, False)

    if getTime:
        end_time = perf_counter()
        print(" getCoords(), both threads:", end_time-start_time) # type: ignore

    if globalPrint or printCoord:
        print("x:", PP[0], " y:", PP[1], " z:", PP[2], sep='')

    showImage = False

    if showImage:
        stacked = np.hstack((source0[3], source1[3], source0[4], source1[4])) # type: ignore
        cv2.imshow('Windows', cv2.resize(stacked, None, fx=0.7, fy=0.7)) # type: ignore
    
    if cv2.waitKey(1) == 32:
        showImage = not showImage

    if getTime:
        start_time = perf_counter()
    # Calculate the servo rotations
    bPos = False
    if Pitch <= 90 and Pitch >= -90:
        b = toRadians(Pitch * 0.9 + toDegrees(b) * 0.1)
        if b < 0: bPos = False
        if b > 0: bPos = True
    if Roll <= 90 and Roll >= -90:
        if not bPos: a = toRadians(0 - (Roll * 0.9 + toDegrees(a) * 0.1))
        elif bPos: a = toRadians(Roll * 0.9 + toDegrees(a) * 0.1)
    # getAngles(source0[0], source0[1], source1[2], a, b, Y, posOption, 1, 1, globalPrint)
    q = getAngles(PP,a,b,Y,'-')

    if getTime:
        end_time = perf_counter()
        print(" read accel, calc getAngles():", end_time-start_time) # type: ignore

    servoExceeded = False
    # "under" = given < 0
    # "over" = given < 180
    whichServoExceeded = 6*[False]
    typeOfExceeded = 6*["null"]

    if getTime:
        start_time = perf_counter()

    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)

    # Press Esc key to exit
    if cv2.waitKey(1) == 27:
        break

    if getTime:
        end_time = perf_counter()
        print(" send coords to pca board:", end_time-start_time) # type: ignore
        sys.exit()        
if showImage:
    cv2.destroyAllWindows()
webcam[0].stop() # type: ignore
webcam[1].stop() # type: ignore
pca.deinit()

