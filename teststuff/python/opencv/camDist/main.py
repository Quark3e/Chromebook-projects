#!/usr/bin/env python3

import cv2 #type: ignore
import numpy as np
import os
import os.path
from time import sleep
import openCV_addon as ad
import math
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import sys
from datetime import datetime


def nothing(x):
    pass

cam0 = cv2.VideoCapture(2)
cam1 = cv2.VideoCapture(0)

cam0.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1) # Set exposure to manual mode
cam1.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1)

displayToOpenCV = True


#lower, upper values
hsvCam0 = [[0, 0, 255], [179, 9, 255]] #ir webcam
hsvCam1 = [[0, 0, 255], [179, 9, 255]]

camNm = ["cap0 - area", "cap1 - zAxis"]

if displayToOpenCV:
    cv2.namedWindow(camNm[0]) #xy
    cv2.namedWindow(camNm[1]) #z
    ad.hsv_trackbars(camNm[0], hsvCam0)
    ad.hsv_trackbars(camNm[1], hsvCam1)
    cv2.moveWindow(camNm[0], 100, 100)
    cv2.moveWindow(camNm[1], 100+700, 100)

#   int(zAxis): [cntArea],
outFile = open("data/raw_values.dat", "a")
values = {}
xData, yData = [], []

prefRes = (640, 480)

kernel = np.ones((5, 5), np.uint8)
font = cv2.FONT_HERSHEY_SIMPLEX

imgTemp = [0, 0]
morphImg = [0, 0]
thresh = [0, 0]

ret = [0, 0]
contours = [0, 0]
cntArea = 0
cntPos = [0, 0]
cntMoments = 0
frame = [0, 0]

def processFrame(img, flag, winName):
    global contours, cntMoments, cntPos, cntArea, morphImg, frame
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

    morphImg[flag] = cv2.erode(cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR), kernel, 1)
    morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=6)
    #morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=1)
    #morphImg[flag] = cv2.erode(morphImg[flag], kernel, iterations=1)

    _, thresh[flag] = cv2.threshold(morphImg[flag], 127, 255, cv2.THRESH_BINARY)
    contours[flag], hierarchy = cv2.findContours(cv2.cvtColor(thresh[flag], cv2.COLOR_BGR2GRAY), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)


def solveContours(allContours, areaThreshold):
    allPositions = [[], []]
    totArea = 0
    if len(allContours)==0: return [[0,0], 0]
    # print(" len():", len(allContours), end=" ")
    for cnt in allContours:
        CntMoments = cv2.moments(cnt)
        if CntMoments["m00"] != 0:
            area = cv2.contourArea(cnt)
            # print(area, end=" ")
            if area>=areaThreshold:
                allPositions[0].append(CntMoments["m10"]/CntMoments["m00"])
                allPositions[1].append(CntMoments["m01"]/CntMoments["m00"])
                totArea+=area
    avgPos = [
        int(round(sum(allPositions[0])/len(allPositions[0]))),
        int(round(sum(allPositions[1])/len(allPositions[1])))
    ]
    return [avgPos, int(totArea)]


def configure_settings():
    if not displayToOpenCV:
        return
    global imgTemp, ret, contours, cntArea, cntPos, cntMoments
    while True:
        ret[0], imgTemp[0] = cam0.read()
        ret[1], imgTemp[1] = cam1.read()
        if ret[0]==False or ret[1]==False: print("could not capture from cam. returned:", ret)
        else:    
            processFrame(imgTemp[0], 0, camNm[0])
            processFrame(imgTemp[1], 1, camNm[1])
            for flag in range(2):
                for i in range(len(contours[flag])):
                    if cv2.contourArea(contours[flag][0]) > 1000:
                        cntMoments = cv2.moments(contours[flag][0])
                        if cntMoments['m00'] != 0:
                            if flag==0:
                                cntPos_xy = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                                _,cntArea = solveContours(contours[flag],0)
                                if displayToOpenCV:
                                    morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntArea)),(cntPos_xy[0],cntPos_xy[1]),font,1,(255,0,0),2)
                            elif flag==1:
                                cntPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                                if displayToOpenCV:
                                    morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntPos[1])),(cntPos[0],cntPos[1]),font,1,(255,0,0),2)
            print(prefRes[1]-cntPos[1], cntArea)
            # print((morphImg[0]).shape, (frame[0]).shape)
            cv2.imshow(camNm[0], cv2.resize(np.hstack((morphImg[0], frame[0])), None, fx=0.4, fy=0.4)) # type: ignore
            cv2.imshow(camNm[1], cv2.resize(np.hstack((morphImg[1], frame[1])), None, fx=0.4, fy=0.4)) # type: ignore
            key = cv2.waitKey(1)
            if key==27:
                exit()
            elif key==32:
                return

def script_exit():
    global values, xData, yData
    print("exit initialized...")
    tempDict = list(values.keys())
    tempDict.sort()
    print("raw:", values)
    print("----------------")
    values = {i: values[i] for i in tempDict}
    print("sorting:")
    print("----------------")
    print("sorted:",values)
    print("----------------")
    xData, yData = [], []
    for key in values:
        values[key] = int(sum(values[key])/len(values[key]))
        yData.append(key)
        xData.append(values[key])
        print(key,": ",values[key],sep='')
    print("----------------")
    # outFile.write(str(values)+"\n")
    outFile.write("2D\n")
    outFile.write(str(xData)+"\n")
    outFile.write(str(yData)+"\n")
    outFile.write("\n")
    print(values)
    print("----------------")
    print(xData)
    print(yData)
    print("----------------")
    outFile.close()
    return

fig, ax = plt.subplots()
ln, = plt.plot([], [], 'ro')

def plt_init():
    print("check init start")
    #ax.set_xlim(0, 5000)
    #ax.set_ylim(0, 500)
    print("check init end")


def plt_update(n):
    # print("check update")
    global imgTemp, ret, contours, cntArea, cntPos, cntMoments, values, xData, yData
    ret[0], imgTemp[0] = cam0.read()
    ret[1], imgTemp[1] = cam1.read()
    # print("check 2")
    if ret[0]==False or ret[1]==False: print("could not capture from cam. returned:", ret)
    else:    
        processFrame(imgTemp[0], 0, camNm[0])
        processFrame(imgTemp[1], 1, camNm[1])
        for flag in range(2):
            for i in range(len(contours[flag])):
                cntMoments = cv2.moments(contours[flag][0])
                if cntMoments['m00'] != 0:
                    if flag==0:
                        cntPos_xy = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                        _, cntArea = solveContours(contours[flag],0)
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntArea)),(cntPos_xy[0],cntPos_xy[1]),font,1,(255,0,0),2)
                    elif flag==1:
                        cntPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntPos[1])),(cntPos[0],cntPos[1]),font,1,(255,0,0),2)
                        cntPos[1] = prefRes[1] - cntPos[1]
        
        if cntPos[1] not in values:
            values.update({cntPos[1]: [cntArea]})
        else:
            values[cntPos[1]].append(int(cntArea))
            print(len(values), len(values[cntPos[1]]), end=' ')
        if len(values[cntPos[1]]) >= 100:
            values[cntPos[1]] = sum(values[cntPos[1]])/len(values[cntPos[1]])
            print("averages value:", cntPos[1]  )

        # ln.set_data(xData, yData)
    if displayToOpenCV:
        cv2.imshow(camNm[0], cv2.resize(np.hstack((morphImg[0], frame[0])), None, fx=0.4, fy=0.4))
        cv2.imshow(camNm[1], cv2.resize(np.hstack((morphImg[1], frame[1])), None, fx=0.4, fy=0.4))
        key = cv2.waitKey(1)
        if key==27:
            sys.exit()
        elif key==32:
            script_exit()
            return False
        elif key==ord('s'):
            print([hsvCam0],[hsvCam1])
            np.save('hsv_value_cam0',[hsvCam0])
            np.save('hsv_value_cam1',[hsvCam1])
            script_exit()
        elif key==97:
            cv2.imwrite("cam0_img", np.hstack((morphImg[0], frame[0])))
            cv2.imwrite("cam1_img", np.hstack((morphImg[1], frame[1])))
            sleep(1)
    # print("check 3")
    return True

# if __name__=="__main__":
print("Configuring settings:")
configure_settings()

# ani = FuncAnimation(fig, plt_update, 10, init_func=plt_init(), blit=True)

print("Running main loop:")
while True:
    if plt_update(0):
        pass
    else:
        break

print("plotting..")

plt.plot(xData, yData)

def validSaveFig(figToSave, fileName, filePath, imgDpi=300, saveCopies=True, fileExt=".png"):
    if saveCopies:
        for i in range(10000):
            if not os.path.isfile(filePath+fileName+str(i)+fileExt):
                figToSave.savefig(filePath+fileName+str(i)+fileExt, dpi=imgDpi)
                break
    elif not saveCopies:
        figToSave.savefig(filePath+fileName+fileExt, dpi=imgDpi)

currPath = str(os.path.realpath(__file__))[:len(str(os.path.realpath(__file__)))-len("main.py")]+"media/"

validSaveFig(fig, "RawFig"+str(datetime.now()),currPath,imgDpi=300,saveCopies=True)

plt.show()
