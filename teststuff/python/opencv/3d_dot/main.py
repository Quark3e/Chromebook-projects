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

displayToOpenCV = True

def toRadians(degrees):
    return (degrees*np.pi)/180
def toDegrees(radians):
    return (radians*180)/np.pi

absPath = os.path.realpath(__file__)[:-len("main.py")]
sys.path.append(absPath[:absPath.find("teststuff")])


#float cppVersion[181][181][401] = #[nodeObj.roll][nodeObj.Pitch][z] = area (solve z by going through each z for closest area)
csvFileChart = [[[None for z in range(401)] for y in range(181)] for x in range(181)] #[180][180][400]=area


def csv_loadData():
    global csvFileChart
    parts = 4
    filePath = str(os.path.realpath(__file__))[:len(str(os.path.realpath(__file__)))-len("3d_dot/main.py")]+"angleArea/data/csv_artif/"
    fileNom = ["csv_[1,1,1]_6568781_p", "_artificial.csv"]

    print("Loading datasets from file(s) to 4d list:")
    for i in range(parts):
        print(f"- p{i}")
        with open(filePath+fileNom[0]+str(i)+fileNom[1], "r") as f:
            next(f, None)
            for line in f:
                nodeObj.roll, nodeObj.Pitch, z, area = [float(var) for var in line.split(",")]
                csvFileChart[round(nodeObj.roll+90)][round(nodeObj.Pitch+90)][round(z)] = area
    print("Finished loading.")

    return



import teststuff.python.modules.nodemcu.main_h as nodeCl
nodeObj = nodeCl.espOrient()

def checkfunc(x):
    global using_lTemp
    # c = [1.11616931*10**-39, -2.07682935*10**-34, 1.68556962*10**-29, -7.83319285*10**-25,
    #          2.30122730*10**-20, -4.45491855*10**-16, 5.75203827*10**-12, -4.90909892*10**-8,
    #          2.68701764*10**-4, -8.89666871*10**-1, 1.61255736*10**3]

    c = [ 1.41350147*10**-32, -7.39588055*10**-28, 1.70543946*10**-23, -2.27903757*10**-19,
        1.95152915*10**-15, -1.11701396*10**-11, 4.32049796*10**-8, -1.11313493*10**-4,
        1.82568080*10**-1, -1.72185711*10**2, 7.15691211*10**4]

    lTemp = [csvFileChart[int(nodeObj.Roll+90)][int(nodeObj.Pitch+90)][i] for i in range(401)]

    # using_lTemp = not None in lTemp
    # print(lTemp.index(None))
    print(f"{using_lTemp} ", end=" \t")


    if using_lTemp: return lTemp.index(min(lTemp, key=lambda r: abs(r-x)))
    else: return sum([c[n]*(x**(10-n)) for n in range(len(c))])

def polyTest(xData):
    return [checkfunc(x) for x in xData]


nodeObj.readAccelerometer()

cam = [cv2.VideoCapture(2), cv2.VideoCapture(0)] # [area/xy, z]
prefRes = (640, 480)

cam[0].set(cv2.CAP_PROP_AUTO_EXPOSURE, 1) # Set exposure to manual mode
cam[1].set(cv2.CAP_PROP_AUTO_EXPOSURE, 1)

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
    cv2.moveWindow(dispWin[0], 200, 420)
    cv2.moveWindow(dispWin[1], 200+512, 420)

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
    morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=6)
    # morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=1)
    # morphImg[flag] = cv2.erode(morphImg[flag], kernel, iterations=1)

    _, thresh[flag] = cv2.threshold(morphImg[flag], 127, 255, cv2.THRESH_BINARY)
    contours[flag], hierarchy = cv2.findContours(cv2.cvtColor(thresh[flag], cv2.COLOR_BGR2GRAY), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)


def solveContours(allContours, areaThreshold=0):
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


def camPos_update():
    global imgTemp, ret, contours, cntArea, cntMoments, cntPos, cntPos_z, raw_read, using_lTemp
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
                        _, cntArea = solveContours(contours[flag])
                        cntPos_z = checkfunc(cntArea)
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntArea)),(tempPos[0],tempPos[1]),font,1, (255,0,0),2)
                        nodeObj.readAccelerometer(printText=True)
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
        elif key==113: using_lTemp=True
        elif key==119: using_lTemp=False
    return True


class AnimatedScatter(object):
    def __init__(self):
        self.radius = 75
        self.degrees = 0
        self.graphRanges = {"x": (-200, 200), "y":(-200, 200), "z":(0, 500)}
        self.stream = self.data_stream()
        self.fig = plt.figure(dpi=150)
        self.ax = {
            "3d": 0, #3d view
            "xz": 0, #front view
            "yz": 0, #side view
            "xy": 0  #top view
        }
        count = 0
        for key in self.ax:
            count+=1
            if key=="3d": self.ax[key] = self.fig.add_subplot(2,2,count,projection="3d")
            else: self.ax[key] = self.fig.add_subplot(2,2,count)
            self.ax[key].grid()
            self.ax[key].axis("equal")
            self.ax[key].title.set_text(key)
        self.ani = FuncAnimation(self.fig, self.update, interval=1, init_func=self.setup_plot, blit=True)
    def setup_plot(self):
        p0, p1 = next(self.stream)
        tempC = np.random.random((1, 2)).T
        self.ps_Stuff = {}
        for key in self.ax: self.ps_Stuff.update({key: 3*[0]})

        # self.ps_Stuff["3d"][0] = self.ax["3d"].scatter([p0["x"]],[p0["y"]],[p0["z"]], edgecolor="k", label="dot 0")
        # self.ps_Stuff["3d"][1] = self.ax["3d"].scatter([p1["x"]],[p1["y"]],[p1["z"]], edgecolor="k", label="dot 1")
        # print("------", p0)
        self.ps_Stuff["3d"][0], = self.ax["3d"].plot([p0["x"], p0["x"]], [p0["y"], p0["y"]], [p0["z"], p0["z"]], linewidth=7)
        self.ps_Stuff["3d"][1], = self.ax["3d"].plot([p1["x"], p1["x"]], [p1["y"], p1["y"]], [p1["z"], p1["z"]], linewidth=7)
        self.ps_Stuff["3d"][2], = self.ax["3d"].plot([p0["x"], p1["x"]], [p0["y"], p1["y"]], [p0["z"], p1["z"]], linestyle="dotted")

        for key in self.ax:
            if key=="3d": continue
            self.ps_Stuff[key][0] = self.ax[key].scatter([p0[key[:1]]],[p0[key[1:2]]], edgecolor="k", label="dot 0")
            self.ps_Stuff[key][1] = self.ax[key].scatter([p1[key[:1]]],[p1[key[1:2]]], edgecolor="k", label="dot 1")
            self.ps_Stuff[key][2], = self.ax[key].plot([p0[key[:1]],p1[key[:1]]],[p0[key[1:2]],p1[key[1:2]]], linestyle="dotted")

        self.ax["3d"].set(xlim3d=self.graphRanges["x"], xlabel="X")
        self.ax["3d"].set(ylim3d=self.graphRanges["y"], ylabel="Y")
        self.ax["3d"].set(zlim3d=(self.graphRanges["z"]), zlabel="Z")
        self.ax["3d"].view_init(elev=30, azim=60)
        for key in self.ax:
            if key=="3d": continue
            self.ax[key].set_xlabel(key[:1].upper())
            self.ax[key].set_ylabel(key[1:2].upper())
            self.ax[key].set_xlim(list(self.graphRanges[key[:1]]))
            self.ax[key].set_ylim(list(self.graphRanges[key[1:2]]))
        return self.ps_Stuff["3d"][0], self.ps_Stuff["3d"][1], self.ps_Stuff["3d"][2] \
        , self.ps_Stuff["xz"][0], self.ps_Stuff["xz"][1], self.ps_Stuff["xz"][2] \
        , self.ps_Stuff["yz"][0], self.ps_Stuff["yz"][1], self.ps_Stuff["yz"][2] \
        , self.ps_Stuff["xy"][0], self.ps_Stuff["xy"][1], self.ps_Stuff["xy"][2]
    def data_stream(self):
        p0 = {"x":0, "y":0, "z":0}
        p1 = {"x":0, "y":0, "z":0}
        while True:
            camPos_update()
            if not None in cntPos:
                p0 = {"x":cntPos[0], "y":cntPos[1], "z":cntPos[2]}
                zVals = cntPos[:2]+[cntPos_z]
                p1 = {"x": zVals[0], "y":zVals[1], "z":zVals[2]}
            yield p0, p1
    def update(self, i):
        p0, p1 = next(self.stream)
        # self.ps_Stuff["3d"][0]._offsets3d=([p0["x"]], [p0["y"]], [p0["z"]])
        # self.ps_Stuff["3d"][1]._offsets3d=([p1["x"]], [p1["y"]], [p1["z"]])
        self.ps_Stuff["3d"][0].set_data_3d([p0["x"], p0["x"]+1], [p0["y"], p0["y"]+1], [p0["z"], p0["z"]+1])
        self.ps_Stuff["3d"][1].set_data_3d([p1["x"], p1["x"]+1], [p1["y"], p1["y"]+1], [p1["z"], p1["z"]+1])
        self.ps_Stuff["3d"][2].set_data_3d([p0["x"], p1["x"]], [p0["y"], p1["y"]], [p0["z"], p1["z"]])
        for key in self.ax:
            if key=="3d": continue
            self.ps_Stuff[key][0].set_offsets([p0[key[:1]], p0[key[1:2]]])
            self.ps_Stuff[key][1].set_offsets([p1[key[:1]], p1[key[1:2]]])
            self.ps_Stuff[key][2].set_data([[p0[key[:1]],p1[key[:1]]],[p0[key[1:2]],p1[key[1:2]]]])
        return self.ps_Stuff["3d"][0], self.ps_Stuff["3d"][1], self.ps_Stuff["3d"][2] \
        , self.ps_Stuff["xz"][0], self.ps_Stuff["xz"][1], self.ps_Stuff["xz"][2] \
        , self.ps_Stuff["yz"][0], self.ps_Stuff["yz"][1], self.ps_Stuff["yz"][2] \
        , self.ps_Stuff["xy"][0], self.ps_Stuff["xy"][1], self.ps_Stuff["xy"][2]


if __name__=="__main__":
    csv_loadData()

    a = AnimatedScatter()
    plt.show()
