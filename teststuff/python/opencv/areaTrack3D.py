#!/usr/bin/env python3

#get morphImg[flag]
#flag==0:
#   save xy values
#   save contourArea values
#flag==1:
#   save z values
#
#if xy values exist: append list: xy, contourArea and z values
#else if xy values doesnt exist: update dict with values as element [0]

#in 'zAxis-Area_values.dat' it'll store:
#   [contourArea, ...]
#   [[x,y,z], ...]

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

cam0.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1) # Set exposure to manual mode
cam1.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1)

displayToOpenCV = True


#lower, upper values
hsvCam0 = [[0, 0, 255], [179, 9, 255]] #ir webcam; x y, area
hsvCam1 = [[0, 0, 255], [179, 9, 255]] #z


if displayToOpenCV:
    cv2.namedWindow("cap0") #xy
    cv2.namedWindow("cap1") #z
    ad.hsv_trackbars("cap0", hsvCam0)
    ad.hsv_trackbars("cap1", hsvCam1)
    cv2.moveWindow("cap0", 100, 100)
    cv2.moveWindow("cap1", 100+700, 100)

#   int(zAxis): [cntArea],
outFile = open("zAxis-Area_values.dat", "a")

values = {} 
# temporary holder for values:
#   str([x, y, z]): [cntArea, ...],
#   str([x, y, z]): [cntArea, ...],
#   ...

# xData, yData, zData, areaData = [], [], [], []
data = { # what is sent to .scatter() or .plot()
    "x": [],
    "y": [],
    "z": [],
    "area": []
}

prefRes = (640, 480)

kernel = np.ones((5, 5), np.uint8)
font = cv2.FONT_HERSHEY_SIMPLEX

imgTemp = [0, 0]
morphImg = [0, 0]
thresh = [0, 0]

ret = [0, 0]
contours = [0, 0]
cntArea = 0
tempPos = [0, 0]
cntPos = [0, 0, 0]
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
    morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=1)
    morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=1)
    morphImg[flag] = cv2.erode(morphImg[flag], kernel, iterations=1)

    _, thresh[flag] = cv2.threshold(morphImg[flag], 127, 255, cv2.THRESH_BINARY)
    contours[flag], hierarchy = cv2.findContours(cv2.cvtColor(thresh[flag], cv2.COLOR_BGR2GRAY), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

def configure_settings():
    if not displayToOpenCV:
        return
    global imgTemp, ret, contours, cntArea, cntPos, cntMoments
    while True:
        ret[0], imgTemp[0] = cam0.read()
        ret[1], imgTemp[1] = cam1.read()
        if ret[0]==False or ret[1]==False: print("could not capture from cam. returned:", ret)
        else:    
            processFrame(imgTemp[0], 0, "cap0")
            processFrame(imgTemp[1], 1, "cap1")
            for flag in range(2):
                for i in range(len(contours[flag])):
                    if cv2.contourArea(contours[flag][0]) > 1000:
                        cntMoments = cv2.moments(contours[flag][0])
                        if cntMoments['m00'] != 0:
                            if flag==0:
                                cntPos_xy = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                                cntArea = cv2.contourArea(contours[flag][0])
                                if displayToOpenCV:
                                    morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntArea)),(cntPos_xy[0],cntPos_xy[1]),font,1,(255,0,0),2)
                            elif flag==1:
                                cntPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                                if displayToOpenCV:
                                    morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntPos[1])),(cntPos[0],cntPos[1]),font,1,(255,0,0),2)
            print(prefRes[1]-cntPos[1], cntArea)
            # print((morphImg[0]).shape, (frame[0]).shape)
            cv2.imshow("cap0", cv2.resize(np.hstack((morphImg[0], frame[0])), None, fx=0.4, fy=0.4)) # type: ignore
            cv2.imshow("cap1", cv2.resize(np.hstack((morphImg[1], frame[1])), None, fx=0.4, fy=0.4)) # type: ignore
            key = cv2.waitKey(1)
            if key==27:
                exit()
            elif key==32:
                return

def script_exit():
    global values, data
    print("exit initialized...", end='')

    tempDict = list(values.keys())
    tempDict.sort()
    print("raw:", values)
    print("----------------")
    values = {i: values[i] for i in tempDict}
    print("sorting:")
    print("----------------")
    print("sorted:",values)
    print("----------------")
    for key in data: data[key] = []
    for key in values:
        values[key] = int(sum(values[key])/len(values[key]))
        yData.append(key)
        xData.append(values[key])
        print(key,": ",values[key],sep='')
    print("----------------")
    # outFile.write(str(values)+"\n")
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

fig = plt.figure()
ax = fig.add_subplot(projection='3d')

def plt_init():
    print("check init start")
    ax.set(xlim3d=(-150, 150), xlabel='X')
    ax.set(ylim3d=(-150, 150), ylabel='Y')
    ax.set(zlim3d=(0, 300), zlabel='Z')
    ax.view_init(elev=30, azim=60)
    print("check init end")

def plt_update(n):
    # print("check update")
    global imgTemp, ret, contours, cntArea, cntPos, cntMoments, values, xData, yData
    ret[0], imgTemp[0] = cam0.read()
    ret[1], imgTemp[1] = cam1.read()
    # print("check 2")
    if ret[0]==False or ret[1]==False: print("could not capture from cam. returned:", ret)
    else:    
        processFrame(imgTemp[0], 0, "cap0") #solve contourArea and xy pos
        processFrame(imgTemp[1], 1, "cap1")
        cntPos = [None, None, None]
        tempPos = [None, None]
        for flag in range(2):
            for i in range(len(contours[flag])):
                cntMoments = cv2.moments(contours[flag][0])
                if cntMoments['m00'] != 0:
                    if flag==0:
                        tempPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                        cntPos[0] = round(tempPos[0]/10)*10
                        cntPos[1] = round(tempPos[1]/10)*10
                        cntArea = cv2.contourArea(contours[flag][0])
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntArea)),(tempPos[0],tempPos[1]),font,1,(255,0,0),2)
                    elif flag==1:
                        tempPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                        cntPos[2] = round(tempPos[1]/10)*10
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(tempPos[1])),(tempPos[0],tempPos[1]),font,1,(255,0,0),2)
                        cntPos[2] = prefRes[1] - cntPos[2]
                else: print("cntMoments['m00'] = 0")
        
        if str(cntPos) not in values:
            values.update( { str(cntPos): [int(cntArea)] } )
        else:
            values[str(cntPos)].append(int(cntArea))
        if len(values[str(cntPos)]) >= 100: # check if there are more than 100 cntArea-values stored
            values[str(cntPos)] = sum(values[str(cntPos)])/len(values[str(cntPos)])
            print("solved average area for pos: ", cntPos)

        data["x"]
        data["y"]
        data["z"]
        data["x"]
        for key, val in values.items():
            yData.append(key)
            xData.append(val)
        # ln.set_data(xData, yData)
    if displayToOpenCV:
        cv2.imshow("cap0", cv2.resize(np.hstack((morphImg[0], frame[0])), None, fx=0.4, fy=0.4))
        cv2.imshow("cap1", cv2.resize(np.hstack((morphImg[1], frame[1])), None, fx=0.4, fy=0.4))
        key = cv2.waitKey(1)
        if key==27:
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
configure_settings()

# ani = FuncAnimation(fig, plt_update, 10, init_func=plt_init(), blit=True)

plt_init()
while True:
    if plt_update(0):
        pass
    else:
        break

print("plotting..")

ax.scatter(data["x"], data["y"], data["z"])

plt.show()
