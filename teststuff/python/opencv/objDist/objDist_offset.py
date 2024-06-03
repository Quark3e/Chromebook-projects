#!/usr/bin/env python3


import cv2 #type: ignore
import numpy as np
import os.path
import math
import sys
import socket
import pandas
from time import sleep, perf_counter
import time
import openCV_addon as ad

absPath = os.path.realpath(__file__)[:-len("objDist_offset.py")]
sys.path.append(absPath[:absPath.find("teststuff")])

import teststuff.python.modules.nodemcu.main_h as nodCl
nodObj = nodCl.espOrient()

nodObj.readAccelerometer()


def toDegrees(radians): return (radians * 180) / math.pi
def toRadians(degrees): return (degrees * math.pi) / 180


cam = cv2.VideoCapture(2)
if not cam.isOpened():
    print("cam could not be opened. exiting")
    sys.exit()
cam.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1)

displayToOpenCV = True

hsvLim = [[0, 0, 255], [179, 9, 255]]
camWin = "webcam"

if displayToOpenCV:
    cv2.namedWindow(camWin)
    ad.hsv_trackbars(camWin, hsvLim)
    cv2.moveWindow(camWin, 100, 100)

prefRes = (640, 480)
kernel = np.ones((5, 5), np.uint8)
font = cv2.FONT_HERSHEY_SIMPLEX

frame = None
imgTemp = None
morphImg = None
thresh = None
ret = None
contours = None
cntArea = None
cntPos = [0, 0]
cntMoments = 0
cntDistance = None
cntDistance_real = None

obj_height = 40 # real object height [mm]
obj_screen = [None, None]
camFOV = 35 #y axis [degrees]

angPerPx = camFOV / prefRes[1]


def angleCorrections(areaHeight):
    #NOTE: uses height instead of area number
    newArea = 0
    nodObj.readAccelerometer()

    return newArea

#NOTE: change order so getObjDistance is used last and area_angle corrections like tilt and offset are used first

def getObjDistance(scrHeight): # return [mm]
    # use height (y values)
    global cntDistance
    if obj_height == None or camFOV == None: return 0
    ans = (obj_height * 360 * prefRes[1]) / (2 * np.pi * camFOV * scrHeight)
    cntDistance = ans
    return ans

def solveDistOffset(objDist, objPos):
    """
    Calculates axis offset by lens FOV
    ## Parameters:
        - objDist (float/int): "seen"/raw distance
        - objPos (list()) [x, y]: object position from img center going into negative axes
    ## Return:
        - (list()): returns two element list [solved distance, distance offset]
    """
    ans = 0
    ans = objDist * math.cos(toRadians(angPerPx * math.sqrt(objPos[0]**2 + objPos[1]**2)))
    distOffset = objDist - ans
    return [ans, distOffset]

def processFrame(img, winName):
    global contours, cntPos, cntArea, morphImg, frame
    tempHSV = hsvLim
    frame = cv2.resize(img, prefRes)
    frame = cv2.flip(frame, 1)
    hsvImg = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    if displayToOpenCV:    
        tempHSV[0][0] = cv2.getTrackbarPos("L - H", winName)
        tempHSV[0][1] = cv2.getTrackbarPos("L - S", winName)
        tempHSV[0][2] = cv2.getTrackbarPos("L - V", winName)
        tempHSV[1][0] = cv2.getTrackbarPos("U - H", winName)
        tempHSV[1][1] = cv2.getTrackbarPos("U - S", winName)
        tempHSV[1][2] = cv2.getTrackbarPos("U - V", winName)
        mask = cv2.inRange(hsvImg, np.array(tempHSV[0]), np.array(tempHSV[1]))
    else:
        mask = cv2.inRange(hsvImg, np.array(tempHSV[0]), np.array(tempHSV[1]))
    propMask = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)

    morphImg = cv2.erode(cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR), kernel, iterations=1)
    morphImg = cv2.dilate(morphImg, kernel, iterations=5)
    # morphImg = cv2.dilate(morphImg, kernel, iterations=1)
    # morphImg = cv2.erode(morphImg, kernel, iterations=1)

    _, thresh = cv2.threshold(morphImg, 127, 255, cv2.THRESH_BINARY)
    contours, hierarchy = cv2.findContours(cv2.cvtColor(thresh, cv2.COLOR_BGR2GRAY), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)


def main():
    global imgTemp, ret, contours, cntArea, cntPos, cntMoments, cntDistance, obj_screen, morphImg, cntDistance_real
    ret, imgTemp = cam.read()
    if ret==False: print("could not capture from cam")
    else:
        processFrame(imgTemp, camWin)
        if contours==None:
            print("contours==None")
            return
        cntPos = [None, None]
        for i in range(len(contours)):
            cntMoments = cv2.moments(contours[0])
            tempPos = [int(cntMoments['m10']/cntMoments['m00']), int(cntMoments['m01']/cntMoments['m00'])]
            cntPos[0] = round(tempPos[0] - prefRes[0]/2) #type: ignore
            cntPos[1] = round(prefRes[1]/2 - tempPos[1]) #type: ignore
            cntArea = cv2.contourArea(contours[0])
            _, _, obj_screen[0], obj_screen[1] = cv2.boundingRect(contours[0])
            getObjDistance(obj_screen[1])
            cntDistance_real = solveDistOffset(cntDistance, cntPos)
            print(f"object distances: raw:{cntDistance:.2f}mm   solved:{cntDistance_real[0]:.2f}mm   offset:{cntDistance_real[1]:.2f}mm", end="\r")
            if displayToOpenCV:
                morphImg = cv2.putText(morphImg, f"{cntArea:.1f}", (tempPos[0], tempPos[1]), font, 1, (255, 0, 0), 2)
                morphImg = cv2.putText(morphImg, f"raw:  {cntDistance:.2f}mm", (10, 50), font, 1, (255, 0, 0), 2)
                morphImg = cv2.putText(morphImg, f"solv.: {cntDistance_real[0]:.2f}mm", (10, 100), font, 1, (255, 0, 0), 2)
                morphImg = cv2.putText(morphImg, f"offs.: {cntDistance_real[1]:.2f}mm", (10, 150), font, 1, (255, 0, 0), 2)
        if displayToOpenCV:
            cv2.imshow(camWin, np.hstack((morphImg, frame)))
            # cv2.imshow(camWin, cv2.resize(morphImg, None, fx=0.9, fy=0.9))
            key = cv2.waitKey(5)
            if key==27: return False
    return True

if __name__=="__main__":
    while True:
        if main():pass
        else: break



