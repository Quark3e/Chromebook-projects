#!/usr/bin/env python3

import cv2
import numpy as np
import os
from time import sleep
import openCV_addon as ad
import math
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

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
morphImg = [0, 0]
ret = [0, 0]
contours = [0, 0]
cntArea = 0
cntPos = [0, 0]
cntMoments = 0
frame = [0, 0]

def processFrame(img, flag, winName):
    global contours, cntMoments, cntPos, cntArea, values, morphImg, frame
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

    morphImg[flag] = cv2.erode(propMask, kernel, iterations=0)
    morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=10)
    morphImg[flag] = cv2.erode(morphImg[flag], kernel, iterations=0)

    contours[flag], hierarchy = cv2.findContours(morphImg[flag], cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

def configure_settings():
    if not displayToOpenCV:
        return
    global imgTemp, ret, contours, cntArea, cntPos, cntMoments, values
    while True:
        ret[0], imgTemp[0] = cam0.read()
        ret[1], imgTemp[1] = cam1.read()
        if ret[0]==False or ret[1]==False: print("could not capture from cam. returned:", ret)
        else:    
            processFrame(imgTemp[0], 0, cam0)
            processFrame(imgTemp[1], 1, cam1)
            for flag in range(1):
                for i in range(1, len(contours)):
                    cntMoments = cv2.moments(contours[0])
                    if cntMoments['m00'] != 0:
                        if flag==0:
                            cntPos_xy = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                            cntArea = cv2.contourArea(contours[0])
                            if displayToOpenCV:
                                morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntArea)),(cntPos_xy[0],cntPos_xy[1]),font,1,(255,0,0),2)
                        elif flag==1:
                            cntPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                            if displayToOpenCV:
                                morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntPos[1])),(cntPos[0],cntPos[1]),font,1,(255,0,0),2)

            cv2.imshow("cap0", cv2.resize(np.hstack((morphImg[0], frame[0])), None, fx=0.4, fy=0.4))
            cv2.imshow("cap1", cv2.resize(np.hstack((morphImg[1], frame[1])), None, fx=0.4, fy=0.4))
            key = cv2.waitKey(1)
            if key==27:
                return

def script_exit():
    global values
    print("exit initialized...", end='')
    tempDict = list(values.keys()).sort()
    values = {i: values[i] for i in tempDict}
    print("sorting:")
    for key in values:
        print(key,": ",values[key],sep='')
        values[key] = sum(values[key])/len(values[key])
    print(values)
    exit()

fig, ax = plt.subplots()
xData, yData = [], []
ln, = plt.plot([], [], 'ro')

def plt_init():
    ax.set_xlim(0, 5000)
    ax.set_ylim(0, 500)
    return ln,

def plt_update():
    global imgTemp, ret, contours, cntArea, cntPos, cntMoments, values, xData, yData
    ret[0], imgTemp[0] = cam0.read()
    ret[1], imgTemp[1] = cam1.read()
    if ret[0]==False or ret[1]==False: print("could not capture from cam. returned:", ret)
    else:    
        processFrame(imgTemp[0], 0, cam0)
        processFrame(imgTemp[1], 1, cam1)
        for flag in range(1):
            for i in range(1, len(contours)):
                cntMoments = cv2.moments(contours[0])
                if cntMoments['m00'] != 0:
                    if flag==0:
                        cntPos_xy = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                        cntArea = cv2.contourArea(contours[0])
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntArea)),(cntPos_xy[0],cntPos_xy[1]),font,1,(255,0,0),2)
                    elif flag==1:
                        cntPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntPos[1])),(cntPos[0],cntPos[1]),font,1,(255,0,0),2)
        
        if cntPos[1] not in values:
            values.update({cntPos[1]: [cntArea]})
        else:
            values[cntPos[1]].append(int(cntArea))
        if len(values[cntPos[1]]) >= 100:
            values[cntPos[1]] = sum(values[cntPos[1]])/len(values[cntPos[1]])

        tempDict = list(values.keys()).sort()
        values = {i: values[i] for i in tempDict}
        xData, yData = [], []
        for key, val in values.items():
            yData.append(key)
            xData.append(val)
        ln.set_data(xData, yData)
    if displayToOpenCV:
        cv2.imshow("cap0", cv2.resize(np.hstack((morphImg[0], frame[0])), None, fx=0.4, fy=0.4))
        cv2.imshow("cap1", cv2.resize(np.hstack((morphImg[1], frame[1])), None, fx=0.4, fy=0.4))
        key = cv2.waitKey(1)
        if key==27:
            script_exit()
        elif key==ord('s'):
            print([hsvCam0],[hsvCam1])
            np.save('hsv_value_cam0',[hsvCam0])
            np.save('hsv_value_cam1',[hsvCam1])
            script_exit()
        elif key==97:
            cv2.imwrite("cam0_img", np.hstack((morphImg[0], frame[0])))
            cv2.imwrite("cam1_img", np.hstack((morphImg[1], frame[1])))
            sleep(1)

    return ln,


if __name__=="__main__":
    configure_settings()

    ani = FuncAnimation(fig, plt_update,  init_func=plt_init(), blit=True)

    plt.show()
