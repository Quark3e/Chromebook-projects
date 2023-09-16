#!/usr/bin/env python3

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import math

import openCV_addon as ad
import cv2 #type: ignore
import time
import os.path
import sys
import socket
from datetime import datetime


def toRadians(degrees):
    return (degrees*np.pi)/180
def toDegrees(radians):
    return (radians*180)/np.pi


def checkfunc(x):
    c = [1.11616931*10**-39, -2.07682935*10**-34, 1.68556962*10**-29, -7.83319285*10**-25,
             2.30122730*10**-20, -4.45491855*10**-16, 5.75203827*10**-12, -4.90909892*10**-8,
             2.68701764*10**-4, -8.89666871*10**-1, 1.61255736*10**3]
    return sum([c[n]*(x**(10-n)) for n in range(len(c))])

def polyTest(xData):
    return [checkfunc(x) for x in xData]



client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(0.5)
client_msg = b"fromClient"
addr = ("192.168.1.117", 53)
server_msg = ""
def reqToServer():
    global server_msg, elapsedTime
    start = time.time()
    client_socket.sendto(client_msg, addr)
    try:
        tempData, server = client_socket.recvfrom(1024)
        end = time.time()
        elapsedTime = end - start
        server_msg = tempData.decode()
    except socket.timeout:
        print("request timed out")


X_out, Y_out, Z_out = 0.1, 0.1, 0.1
Roll, Pitch, roll, pitch = 0.1, 0.1, 0.1, 0.1
tiltFilter = 0.1

def readAccelerometer(printText=True):
    global X_out, Y_out, Z_out, Roll, Pitch, roll, pitch
    reqToServer()
    if len(server_msg)>=1 and (server_msg[:1]=="{" and server_msg[-5]=="}" and server_msg[-1]==";"):
        tempMsg = server_msg.replace(":", ",")
        msgTuple = eval(tempMsg[1:-5])
        X_out, Y_out, Z_out = msgTuple
    
    if printText: print(f" accel: x:{X_out} y:{Y_out} z:{Z_out} roll:{roll} pitch:{pitch}", end='\r')
    
    if X_out > 1: X_out = 1
    if Y_out > 1: Y_out = 1
    if Z_out > 1: Z_out = 1
    if X_out < -1: X_out = -1
    if Y_out < -1: Y_out = -1
    if Z_out < -1: Z_out = -1
    #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = math.atan(Y_out / math.sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    roll = math.atan(-1 * X_out / math.sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    #filter
    Roll = (1-tiltFilter) * Roll + tiltFilter * roll
    Pitch = (1-tiltFilter) * Pitch + tiltFilter * pitch
    if printText: print(f" accel: x:{X_out} y:{Y_out} z:{Z_out} roll:{roll} pitch:{pitch}", end='\r')

reqToServer()
readAccelerometer()

displayToOpenCV = True

cam = [cv2.VideoCapture(2), cv2.VideoCapture(0)]
prefRes = (640, 480)

#cam[0].set(cv2.CAP_PROP_AUTO_EXPOSURE, 1) # Set exposure to manual mode
#cam[1].set(cv2.CAP_PROP_AUTO_EXPOSURE, 1)

#lower, upper values
hsvCam0 = [[0, 0, 255], [179, 9, 255]] #area
hsvCam1 = [[0, 0, 255], [179, 9, 255]] #z

dispWin = ["cam0", "cam1"]
if displayToOpenCV:
    #dispWin = ["cam0", "cam1"]
    cv2.namedWindow(dispWin[0]) #xy
    cv2.namedWindow(dispWin[1]) #z
    ad.hsv_trackbars(dispWin[0], hsvCam0)
    ad.hsv_trackbars(dispWin[1], hsvCam1)
    cv2.moveWindow(dispWin[0], 10, 400)
    cv2.moveWindow(dispWin[1], 10+640, 400)

kernel = np.ones((5, 5), np.uint8)
font = cv2.FONT_HERSHEY_SIMPLEX

imgTemp = [0, 0]
morphImg = [0, 0]
thresh = [0, 0]

PP_read = [0, 0, 0]
raw_read = [0, 0, 0, 0]

ret = [0, 0] # boolean(s) for if .read() succesfully returned an image from VideoCapture object
contours = [0, 0]
cntArea = 0 # contour(area) of tracking as seen from cam[0]
tempPos = [0, 0]
cntPos = [0, 0, 0]
cntPos_z = 0
cntMoments = 0
frame = [0, 0]

def processFrame(img, flag, winName):
    global contours, cntPos, cntArea, morphImg, frame
    hsvList = []
    if flag==0:
        hsvList = hsvCam0
    elif flag==1:
        hsvList = hsvCam0
    frame[flag] = cv2.resize(img, prefRes)
    frame[flag] = cv2.flip(frame[flag], 1)
    hsv = cv2.cvtColor(frame[flag], cv2.COLOR_BGR2HSV)
    
    if displayToOpenCV:    
        hsvList[0][0] = cv2.getTrackbarPos("L - H", winName)
        hsvList[0][1] = cv2.getTrackbarPos("L - S", winName)
        hsvList[0][2] = cv2.getTrackbarPos("L - V", winName)
        hsvList[1][0] = cv2.getTrackbarPos("U - H", winName)
        hsvList[1][1] = cv2.getTrackbarPos("U - S", winName)
        hsvList[1][2] = cv2.getTrackbarPos("U - V", winName)
        mask = cv2.inRange(hsv, np.array(hsvList[0]), np.array(hsvList[1]))
    else:
        mask = cv2.inRange(hsv, np.array(hsvList[0]), np.array(hsvList[1]))
    propMask = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)

    # morphImg[flag] = cv2.erode(cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR), kernel, iterations=0)
    # morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=10)
    # morphImg[flag] = cv2.erode(morphImg[flag], kernel, iterations=0)

    morphImg[flag] = cv2.erode(cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR), kernel, iterations=1)
    morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=4)
    # morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=1)
    # morphImg[flag] = cv2.erode(morphImg[flag], kernel, iterations=1)

    _, thresh[flag] = cv2.threshold(morphImg[flag], 127, 255, cv2.THRESH_BINARY)
    contours[flag], hierarchy = cv2.findContours(cv2.cvtColor(thresh[flag], cv2.COLOR_BGR2GRAY), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

def camPos_update():
    global imgTemp, ret, contours, cntArea, cntMoments, cntPos, cntPos_z, raw_read
    ret[0], imgTemp[0] = cam[0].read()
    ret[1], imgTemp[1] = cam[1].read()

    if ret[0]==False or ret[1]==False: print("could not capture from cam. returned:", ret)
    else:
        processFrame(imgTemp[0], 0, dispWin[0])
        processFrame(imgTemp[1], 1, dispWin[1])
        cntPos[0] = None
        cntPos[1] = None
        tempPos = [None, None]
        cntArea = None
        for flag in range(2):
            for i in range(len(contours[flag])):
                cntMoments = cv2.moments(contours[flag][0])
                if cntMoments["m00"] != 0:
                    if flag==0:
                        tempPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                        cntPos[0] = round(tempPos[0]-prefRes[0]/2)
                        cntPos[1] = round(prefRes[1]/2-tempPos[1])
                        cntArea = cv2.contourArea(contours[flag][0])
                        cntPos_z = checkfunc(cntArea)
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntArea)),(tempPos[0],tempPos[1]),font,1, (255,0,0),2)
                        readAccelerometer(False)
                    elif flag==1:
                        tempPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(tempPos[1])),(tempPos[0],tempPos[1]),font,1,(255,0,0),2)
                        tempPos[0] = round(tempPos[0]-prefRes[0]/2)
                        tempPos[1] = round(prefRes[1] - tempPos[1])
                else: print("cntMoments['m00] == 0")
            if flag==1 and len(contours[flag])<=0: tempPos = [None, None]
        if cntArea != None and not (None in tempPos) and not (None in cntPos):
            raw_read = [cntPos[0], cntPos[1], tempPos[0], tempPos[1]]
            cntPos[2] = tempPos[1]
    if displayToOpenCV:
        cv2.imshow(dispWin[0], cv2.resize(np.hstack((morphImg[0], frame[0])), None, fx=0.4, fy=0.4))
        cv2.imshow(dispWin[1], cv2.resize(np.hstack((morphImg[1], frame[1])), None, fx=0.4, fy=0.4))
        key = cv2.waitKey(5)
        if key==27:
            sys.exit()
        elif key==32: return False
    return True


class AnimatedScatter(object):
    def __init__(self):
        self.stream = self.data_stream()
        self.fig = plt.figure(dpi=200)
        self.ax = self.fig.add_subplot(projection="3d")
        self.ani = FuncAnimation(self.fig, self.update,interval=1,init_func=self.setup_plot,blit=False)
    def setup_plot(self):
        p0, p1 = next(self.stream)
        self.scat0 = self.ax.scatter([p0[0]],[p0[1]],[p0[2]], s=20, cmap="jet", edgecolor="k", label="correct")
        self.scat1 = self.ax.scatter([p1[0]],[p1[1]],[p1[2]], s=20, cmap="magma", edgecolor="k", label="svoled")
        self.plot0, = self.ax.plot([p0[0], p1[0]],[p0[1], p1[1]],[p0[2], p1[2]], linestyle="dotted")
        self.ax.axis("equal")
        self.ax.grid()
        self.ax.set(xlim3d=(-200, 200), xlabel="X")
        self.ax.set(ylim3d=(-200, 200), ylabel="Y")
        self.ax.set(zlim3d=(0, 400), zlabel="Z")
        self.ax.view_init(elev=30, azim=60)
        self.ax.legend()
        return self.scat0, self.scat1, self.plot0
    def data_stream(self):
        while True:
            camPos_update()
            if not None in cntPos:
                p0 = cntPos
                p1 = cntPos[:2]+[cntPos_z]
                yield p0, p1
    def update(self, i):
        p0, p1 = next(self.stream)
        self.scat0._offsets3d=([p0[0]], [p0[1]], [p0[2]])
        self.scat1._offsets3d=([p1[0]], [p1[1]], [p1[2]])
        # print([p0[0], p1[0]],[p0[1], p1[1]], [p0[2], p1[2]])
        # self.plot0.set_data([p0[0], p1[0]],[p0[1], p1[1]])
        self.plot0.set_data_3d([p0[0], p1[0]],[p0[1], p1[1]], [p0[2], p1[2]])
        return self.scat0, self.scat1, self.plot0


if __name__=="__main__":
    a = AnimatedScatter()
    plt.show()
