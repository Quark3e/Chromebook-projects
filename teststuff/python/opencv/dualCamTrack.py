#!/usr/bin/env python3

import cv2
import numpy as np
import os
from time import sleep
import openCV_addon as ad
import math
import matplotlib.pyplot as plt

def nothing(x):
    pass

cam0 = cv2.VideoCapture(0)
cam1 = cv2.VideoCapture(2)

displayToOpenCV = True


#lower, upper values
hsvCam0 = [[0, 0, 255], [179, 9, 255]] #ir webcam
hsvCam1 = [[59, 73, 189], [101, 184, 246]]


if displayToOpenCV:
    cv2.namedWindow("cap0") #xy
    cv2.namedWindow("cap1") #z
    ad.hsv_trackbars("cap0", hsvCam0)
    ad.hsv_trackbars("cap1", hsvCam1)

#   int(zAxis): [cntArea],
values = {
    0: [],
}

prefRes = (640, 480)

kernel = np.ones((5, 5), np.uint8)
font = cv2.FONT_HERSHEY_SIMPLEX

imgTemp = [0, 0]
ret = [0, 0]
contours = 0
cntArea = 0
cntPos = [0, 0]
cntMoments = 0

def processFrame(img, flag, winName):
    global contours, cntMoments, cntPos, cntArea, values
    if flag==0:
        hsvList = hsvCam0
    elif flag==1:
        hsvList = hsvCam0
    frame = cv2.resize(img, prefRes)
    frame = cv2.flip(frame, 1)
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
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

    morphImg = cv2.erode(propMask, kernel, iterations=0)
    morphImg = cv2.dilate(morphImg, kernel, iterations=10)
    morphImg = cv2.erode(morphImg, kernel, iterations=0)

    contours, hierarchy = cv2.findContours(morphImg, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)


while True:
    ret[0], imgTemp[0] = cam0.read()
    ret[1], imgTemp[1] = cam1.read()
    if ret[0]==False or ret[1]==False:
        print("could not capture from cam. returned:", ret)
        break
    processFrame(imgTemp[0], 0, cam0)
    processFrame(imgTemp[1], 1, cam1)

    for i in range(1, len(contours)):
        cntMoments = cv2.moments(contours[0])
        if cntMoments['m00'] != 0:
            cntArea = cv2.contourArea(contours[0])
            cntPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
            if cntPos[1] not in values:
                values.update({cntPos[1]: [cntArea]})
            else:
                values[cntPos[1]].append(int(cntArea))
            if len(values[cntPos[1]]) >= 100:
                values[cntPos[1]] = sum(values[cntPos[1]])/len(values[cntPos[1]])
                
            



