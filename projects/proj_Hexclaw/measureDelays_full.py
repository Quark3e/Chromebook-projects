#!/usr/bin/env python3

#/*
# Measure the delays for the fullprocess of Hexclaw_Main_0 and 1
# prob will use a boolean to switch from thread use to none
# */


import cv2 #type: ignore
import numpy as np
import matplotlib.pyplot as plt
import time
import math
import adafruit_adxl34x #type: ignore
import sys
from threading import Thread
from datetime import datetime
import RPi.GPIO as GPIO #type: ignore #type: ignore

sys.path.append('home/py/Chromebook-projects/projects/proj_Hexclaw')

from IK_module import *

class WebcamVideoStream:
    """Pepega    
    """
    def __init__(self, src=0):
        self.stream = cv2.VideoCapture(src)
        (self.grabbed, self.frame) = self.stream.read()
        self.stopped = False
    def start(self):
        Thread(target=self.update, args=()).start()
        return self
    def update(self):
        while True:
            if self.stopped:
                return
            (self.grabbed, self.frame) = self.stream.read()
    def read(self):
        return self.grabbed, self.frame
    def stop(self):
        self.stopped = True

from board import SCL, SDA #type: ignore
import busio #type: ignore

from adafruit_motor import servo #type: ignore
from adafruit_servokit import ServoKit #type: ignore
from adafruit_pca9685 import PCA9685 #type: ignore

i2c = busio.I2C(SCL, SDA)
pca = PCA9685(i2c)
accelerometer = adafruit_adxl34x.ADXL345(i2c)
pca.frequency = 50
servo = [
    servo.Servo(pca.channels[0]),
    servo.Servo(pca.channels[1]),
    servo.Servo(pca.channels[2]),
    servo.Servo(pca.channels[3]),
    servo.Servo(pca.channels[4]),
    servo.Servo(pca.channels[5])
]
for i in range(6):
    servo[i].set_pulse_width_range(500, 2500)

servo[0].angle = 108
servo[1].angle = 15
servo[2].angle = 180 - 25
servo[3].angle = 45
servo[4].angle = 180 - 0
servo[5].angle = 90


GPIO.setmode(GPIO.BCM)
ledRelay=23
GPIO.setup(ledRelay,GPIO.OUT)
GPIO.output(ledRelay, True)
sendToServo(servo,[90,115,145,90,125,90],0,mode=2)


#delay tracking configurations
useThread = True

#variables
axisFilter = 0.3
accelFilter = 0.1
xScaling, yScaling, zScaling = 0.8, 0.8, 1.2

brightVal = 75
maxArea = 35_000
zMax = 400
zOffset = 100


if useThread: cap = WebcamVideoStream(src=0).start()
elif not useThread: cap = cv2.VideoCapture(0)

showImage = True
globalPrint = False
printCoord = False

posOption = '-'

if showImage: cv2.namedWindow("Windows")

axisVals = [0,0,0]
axisVals[0],axisVals[1],axisVals[2] = accelerometer.acceleration
Roll, Pitch = 0.1, 0.1
roll, pitch = Roll, Pitch
tiltVals = [0.1,0.1,0.1,0.1]
q = 6*[0]
a,b,Y = 0, 0, 0 #unit: radians
PP = [0,200,150]
coord=""

#time measuring definitions
timeResults = {
    "readAccel": [],
    "processImg": {
        "pullImg": [],
        "resizeImg": [],
        "useHSV_limits": [],
        "makeFiltered": [],
        "morphErode": [],
        "morphDilate": [],
        "getThresh": [],
        "findContours": [],
        "findBiggestContour": [],
        "contourLoop": []
    },
    "solveAngles": [],
    "sendAngles": []
}
testKeyNames = []

for key,_ in timeResults.items():
    testKeyNames.append(key)

xRange = 100

def configure_plots():
    global ax, fig
    fig = plt.figure(figsize=(19, 6))
    ax = [0,0,0,0]
    
    for i in range(len(ax)):
        ax[i] = fig.add_subplot(1,4,i+1) #type: ignore
        ax[i].set_xlim(0,xRange) #type: ignore
        # ax[i].set_ylim(0,2) #type: ignore
        ax[i].set_title(testKeyNames[i]) #type: ignore
        ax[i].set_xlabel("iterations") #type: ignore
        ax[i].set_ylabel("read delay [milliseconds]") #type: ignore
        ax[i].grid() #type: ignore

    # plt.title("Delay measurements")


#functions
def nothing(x):
    pass


def getValues(varLists=[False,1]):
    """Read and configure hsv values for colour tracking

    ## Parameters:
    - cap: cv2.VideoCapture variable
    - varLists: lists of mutable variables
        - [0] = useDefaultLimit
        - [1] = defaultLimitIndex
    ## Returns:
    - [L_values, U_values]
    """
    global cap
    cv2.namedWindow("Webcam - XY")
    cv2.createTrackbar("L - H", "Webcam - XY", 0, 179, nothing)
    cv2.createTrackbar("L - S", "Webcam - XY", 0, 255, nothing)
    cv2.createTrackbar("L - V", "Webcam - XY", 0, 255, nothing)
    cv2.createTrackbar("U - H", "Webcam - XY", 179, 179, nothing)
    cv2.createTrackbar("U - S", "Webcam - XY", 255, 255, nothing)
    cv2.createTrackbar("U - V", "Webcam - XY", 255, 255, nothing)
    while True:
        ret, frame = cap.read()
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
            varLists[0] = True
            varLists[1] = 0
            break
        elif key == ord('k'):
            varLists[0] = True
            varLists[1] = 1
            break

    cv2.destroyAllWindows()

    if varLists[0]:
        if varLists[1] == 0: L_values, U_values = [60, 147, 88], [100, 255, 169] #plexgear
        elif varLists[1] == 1: L_values, U_values = [68, 207, 92], [112, 255, 238] #trust exis webcam

    return L_values, U_values #type: ignore

def readAccelerometer():
    """
    ## Parameters:
    - axisVals = [X_out, Y_out, Z_out]
    - tiltVals = [roll, pitch, Roll, Pitch] [unit:radians]
    """
    global axisVals, tiltVals
    axisVals[0], axisVals[1], axisVals[2] = accelerometer.acceleration
    tiltVals[1] = math.atan(axisVals[1]/math.sqrt(pow(axisVals[0],2)+pow(axisVals[2],2)))*180/math.pi
    tiltVals[0] = math.atan(-1*axisVals[0]/math.sqrt(pow(axisVals[1],2)+pow(axisVals[2],2)))*180/math.pi
    #filter
    tiltVals[2] = (1-accelFilter)*tiltVals[2] + accelFilter*tiltVals[0]
    tiltVals[3] = (1-accelFilter)*tiltVals[3] + accelFilter*tiltVals[1]
    return axisVals, tiltVals

def processImage(cValues,axisFilter,axisScal,maxArea,zMax,newSize=(640,480),testT=False,offsets=[0,0,0]):
    if testT:
        global timeResults
        t1 = time.perf_counter()
        readAccelerometer()
        t2 = time.perf_counter()
        timeResults["readAccel"].append(round((t2-t1)*1000))
        t1 = time.perf_counter()
    cX,cY = 0,0
    PP = [0,0,0]
    ret, imgTemp = cap.read()
    if testT:
        t2 = time.perf_counter()
        timeResults["processImg"]["pullImg"].append(round((t2-t1)*1000)) #type: ignore
        t1 = time.perf_counter()

    img = cv2.resize(imgTemp,newSize)
    img = cv2.flip(img,1)
    into_hsv = cv2.cvtColor(img,cv2.COLOR_BGR2HSV)
    if testT:
        t2 = time.perf_counter()
        timeResults["processImg"]["resizeImg"].append(round((t2-t1)*1000)) #type: ignore
        t1 = time.perf_counter()

    L_limit = np.array(cValues[0])
    U_limit = np.array(cValues[1])
    threshold = cv2.inRange(into_hsv,L_limit,U_limit)
    if testT:
        t2 = time.perf_counter()
        timeResults["processImg"]["useHSV_limits"].append(round((t2-t1)*1000)) #type: ignore
        t1 = time.perf_counter()

    filtered = cv2.bitwise_and(img,img,mask=threshold)
    filtered[filtered!=0] = 255
    if testT:
        t2 = time.perf_counter()
        timeResults["processImg"]["makeFiltered"].append(round((t2-t1)*1000)) #type: ignore
        t1 = time.perf_counter()

    # filtered = cv2.erode(filtered,np.ones((5,5),np.uint8),iterations=1)
    if testT:
        t2 = time.perf_counter()
        timeResults["processImg"]["morphErode"].append(round((t2-t1)*1000)) #type: ignore
        t1 = time.perf_counter()

    filtered = cv2.dilate(filtered,np.ones((5,5),np.uint8),iterations=1)
    if testT:
        t2 = time.perf_counter()
        timeResults["processImg"]["morphDilate"].append(round((t2-t1)*1000)) #type: ignore
        t1 = time.perf_counter()

    gray_image = cv2.cvtColor(filtered,cv2.COLOR_BGR2GRAY)
    ret,thresh = cv2.threshold(gray_image,127,255,0)
    if testT:
        t2 = time.perf_counter()
        timeResults["processImg"]["getThresh"].append(round((t2-t1)*1000)) #type: ignore
        t1 = time.perf_counter()

    contours,hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_NONE)
    if testT:
        t2 = time.perf_counter()
        timeResults["processImg"]["findContours"].append(round((t2-t1)*1000)) #type: ignore
        t1 = time.perf_counter()

    #contours loop-start
    # for c in contours:
    c = contours[findVal([cv2.contourArea(contour) for contour in contours])[1]] #return index of element with biggest contourArea() in contours
    if testT:
        t2 = time.perf_counter()
        timeResults["processImg"]["findBiggestContour"].append(round((t2-t1)*1000)) #type: ignore
        t1 = time.perf_counter()

    M = cv2.moments(c)
    # M = cv2.moments(thresh)
    if M["m00"]!=0:
        cX = int(M["m10"]/M["m00"])
        cY = int(M["m01"]/M["m00"])
    else:
        # cX, cY
        pass
    cv2.circle(img,(cX,cY),5,(255,255,255),-1)
    cv2.putText(img,"center"+str(cv2.contourArea(c)),(cX-25,cY-25),cv2.FONT_HERSHEY_SIMPLEX,0.5,(255,255,255),2)
    print("area:",cv2.contourArea(c))
    PP[0] = axisFilter*axisScal[0]*(cX-newSize[0]*0.5+offsets[0])+(1-axisFilter)*PP[0]
    PP[1] = axisFilter*axisScal[1]*(newSize[1]-cY+offsets[1])+(1-axisFilter)*PP[1]
    PP[2] = axisFilter*axisScal[2]*(zMax-(cv2.contourArea(c)/maxArea)*zMax+offsets[2]) + (1-axisFilter)*PP[2]
    print(PP[2])
    #contours loop-end

    if testT:
        t2 = time.perf_counter()
        timeResults["processImg"]["contourLoop"].append(round((t2-t1)*1000)) #type: ignore

    if showImage:
        stacked = np.hstack((img,filtered))
        cv2.imshow('Windows',cv2.resize(stacked,None,fx=0.7,fy=0.7)) #type: ignore

    return PP

def solveAngles(PP,Roll,Pitch,a,b,testT=False):
    if testT:
        global timeResults
        t1 = time.perf_counter()
    bPos = False
    if Pitch <= 90 and Pitch >= -90:
        b = toRadians(Pitch * 0.9 + toDegrees(b) * 0.1)
        if b < 0: bPos = False
        if b > 0: bPos = True
    if Roll <= 90 and Roll >= -90:
        if not bPos: a = toRadians(0 - (Roll * 0.9 + toDegrees(a) * 0.1))
        elif bPos: a = toRadians(Roll * 0.9 + toDegrees(a) * 0.1)
    q = getAngles(PP,a,b,Y,'-',printErrors=False)
    if testT:
        t2 = time.perf_counter()
        timeResults["solveAngles"].append(round((t2-t1)*1000)) #type: ignore
    return q


#delay tracking
def main():
    global timeResults, fig
    configure_plots()

    xValues = [x for x in range(xRange)]
    # L_values, U_values = getValues()
    L_values, U_values = [45, 133, 68], [95, 255, 248] #getValues()
    toTest = True
    #loop starts here
    i=0
    for i in range(xRange):
    # while True:
        # print(i,end='')
        if i>=0: toTest=True
        #     print(" testing...",end='')
        PP = processImage(
            [L_values,U_values],axisFilter,[xScaling,yScaling,zScaling],
            maxArea,zMax,testT=toTest,offsets=[0,0,zOffset]
            )
        print(" ",[round(axis) for axis in PP],end='')
        q = solveAngles(PP,tiltVals[2],tiltVals[3],a,b,testT=toTest)
        if toTest:
            t1 = time.perf_counter()
        sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0,printErrors=False)    
        if toTest:
            t2 = time.perf_counter()
            timeResults["sendAngles"].append(round((t2-t1)*1000)) #type: ignore
        if cv2.waitKey(1) == 27:
            return
        print()
    if showImage:
        cv2.destroyAllWindows()
    #loop ends here
    totalTime, sumTime = 0, 0
    if toTest:
        n=0
        for key,val in timeResults.items():
            if key=="processImg":
                for key2,val2 in timeResults["processImg"].items():
                    ax[n].plot(xValues,val2,linestyle='solid',label=key2) #type: ignore
                    # ax[n].axhline(
                    #     y=(sum(val2)/len(val2)),color="red",linestyle='-',
                    #     label=f"avg:{round(sum(val2)/len(val2))}ms"
                    #     ) #type: ignore
                    sumTime+=sum(val2)/len(val2)
                ax[n].text( #type: ignore
                    0.05, 0.95, str(round(sumTime))+"ms", #type: ignore
                    transform=ax[n].transAxes, fontsize=14, verticalalignment='top') #type: ignore
                totalTime+=sumTime
            else:
                ax[n].plot(xValues,val,linestyle='solid',label="raw") #type: ignore
                ax[n].axhline(y=(sum(val)/len(val)),color="red",linestyle='-',label=f"avg:{round(sum(val)/len(val))}ms") #type: ignore
                totalTime+=sum(val)/len(val)
            ax[n].legend(loc='upper right',framealpha=0.3) #type: ignore
            n+=1
        fig.tight_layout(pad=5.0)
        fig.suptitle("with findVal(), with only dilate(); total delay: {:1.2f}ms".format(totalTime))

        currentDate = str(datetime.now()).replace(" ",";")
        relativePath = "/home/pi/Chromebook-projects/projects/proj_Hexclaw/hexclaw_files/measureDelays_files/media/"
        if useThread: fileTitle = "HM0_delays_"
        elif not useThread: fileTitle = "HM1_delays_"
        plt.savefig(relativePath+fileTitle+currentDate+".png") #type: ignore
        plt.show()
            
    return



if __name__=="__main__":
    main()
    sendToServo(servo,[135,15,155,45,180,90],1,mode=2)
    GPIO.output(ledRelay,False)
    if useThread: cap.stop() #type: ignore
    pca.deinit()
