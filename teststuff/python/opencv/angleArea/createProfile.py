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
import os.path
import time
import openCV_addon as ad
import math
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.animation import FuncAnimation
import sys
import socket
import pandas
import csv

# from sklearn import linear_model
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error

from datetime import datetime

dirPath = {
    "script": "",
    "media": "media/",
    "data": "data/"
}

profilesFile = "profiles.dat"
orient = {
    "azim": -135,
    "elev": 30
}

op2_settings = {
    "thresh_zArea": {"value": 100, "type": type(int()), "func": lambda var: int(var), "description": "minimum area for zCam to recognize z value"},
    "toSaveFig": {"value": True, "type": type(bool()), "func": lambda var: eval(var), "description": "whether to save figures as png images"},
    "thresh_xyLim": {"value": [100, 100], "type": type(list()), "func": lambda var: eval(var), "description": "[x,y]  border square where readings \"outside\" aren't valid"},
    "data_2d_lim": {"value": [[-5, 5], [-5, 5]], "type": type(list()), "func": lambda var: eval(var), "description": "[[x_min,x_max],[y_min,y_max]]  range of points fused into one axis"},
    "polyfitRange": {"value": [2], "type": type(list()), "func": lambda var: eval(var), "description": "[n]  list of degree(s) of polynomial regression models to create"},
    "zPickRange": {"value": [-10, 10], "type": type(list()), "func": lambda var: eval(var), "description": "[z_min,z_max]  range of values outside given z value to also include data points "},
    "show_predict": {"value": False, "type": type(bool()), "func": lambda var: eval(var), "description": "whether to display prediction first or dataSets first"},
    "data_2d_nLim": {"value": [5, 5], "type": type(list()), "func": lambda var: eval(var), "description": "[min,max]  list of minimum number of data points for an axis slice to be made"},
    "zPick": {"value": "auto", "type": type(str()), "func": lambda var: str(var), "description": "what z-axis to pick. If none is given it defaults to \"auto\" (z with most data points)"},
    "useSlice": {"value": True, "type": type(bool()), "func": lambda var: eval(var), "description": "whether create regression model from chosen slice [True] or from entire pf dataSet"},
    "saveCoef": {"value": False, "type": type(bool()), "func": lambda var: eval(var), "description": "whether to write regression model coefficients into coef file"}
}


client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(0.1)
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
# {0.05:0.17:0.88}off;

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
    if X_out==0: X_out=0.01
    if Y_out==0: Y_out=0.01
    if Z_out==0: Z_out=0.01
    #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = math.atan(Y_out / math.sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    roll = math.atan(-1 * X_out / math.sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    #filter
    Roll = (1-tiltFilter) * Roll + tiltFilter * roll
    Pitch = (1-tiltFilter) * Pitch + tiltFilter * pitch
    if printText: print(f" accel: x:{X_out} y:{Y_out} z:{Z_out} roll:{roll} pitch:{pitch}", end='\r')

reqToServer()
readAccelerometer()

def nothing(x):
    pass


displayToOpenCV = True

def opt0_setup():
    global cam0, cam1, dispWin, hsvCam0, hsvCam1
    cam0 = cv2.VideoCapture(2)
    cam1 = cv2.VideoCapture(0)

    cam0.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1) # Set exposure to manual mode
    cam1.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1)

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

values = {}
dataSets = {}

prefRes = (640, 480)

kernel = np.ones((5, 5), np.uint8)
font = cv2.FONT_HERSHEY_SIMPLEX

imgTemp = [0, 0]
morphImg = [0, 0]
thresh = [0, 0]

ret = [0, 0] # boolean(s) for if .read() succesfully returned an image from VideoCapture object
contours = [0, 0]
cntArea = 0 # contour(area) of tracking as seen from cam[0]
tempPos = [0, 0]
cntPos = [0, 0, 0]
cntMoments = 0
frame = [0, 0]

isRecVals = False

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
    #morphImg[flag] = cv2.dilate(morphImg[flag], kernel, iterations=1)
    #morphImg[flag] = cv2.erode(morphImg[flag], kernel, iterations=1)

    _, thresh[flag] = cv2.threshold(morphImg[flag], 127, 255, cv2.THRESH_BINARY)
    contours[flag], hierarchy = cv2.findContours(cv2.cvtColor(thresh[flag], cv2.COLOR_BGR2GRAY), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

def script_exit(printText=True, strComments = []):
    #reads from values and appends to file
    global values, dataSets
    if printText: print("exit initialized...", end='')

    if printText: print("raw:")
    if printText: print("- syntax: \"z axis pos: { angle: [cntArea] }\n")
    for key, val in values.items():
        if printText: print(f"- {key}:")
        for key1, val1 in val.items():
            if printText: print(f"- \t{key1}: {val1}")
    if printText: print("----------------")

    if printText: print("processing:")
    for key0, val0 in values.items():
        for key1, val1 in val0.items():
            if type(val1) == type(list()): values[key0][key1] = sum(values[key0][key1])/len(values[key0][key1])
    for key, val in values.items():
        if printText: print(f"- {key}:")
        for key1, val1 in val.items():
            if printText: print(f"- \t{key1}: {val1}")
    if printText: print("----------------")

    if printText: print("filling \"dataSets\":")
    for key0, val0 in values.items():
        if key0 not in dataSets:
            dataSets.update({key0: [[], [], []]})
        for key1, val1 in val0.items():
            angleStr = [int(key1[0:key1.find(":")]), int(key1[key1.find(":")+1:])]
            dataSets[key0][0].append(angleStr[0])
            dataSets[key0][1].append(angleStr[1])
            dataSets[key0][2].append(val1)
            if printText: print(key0,": ", values[key0], sep='')

    if printText: print("----------------values")

    outFile = open(profilesFile, "a")
    # outFile.write(str(values)+"\n")
    outFile.write("profile:1{\n")

    currentDate = str(datetime.now())
    outFile.write("#date:"+currentDate+"\n")

    for cmnt in strComments: outFile.write("#"+cmnt+"\n")

    for z, vals in dataSets.items():
        if z==None or None in vals:
            print(f"warning: found None in \"{z}:{vals}\"")
        else:
            outFile.write(f"z:{z}: {vals}"+"\n")
    outFile.write("}\n\n")
    if printText: print(values)
    if printText: print("----------------data")
    if printText: print(dataSets)
    if printText: print("----------------script_exit() end")
    outFile.close()
    return


def plt_init(printText=True, mode=0):
    global ax, fig, orient
    plt.close()
    if printText: print("check init start")
    if mode==0:
        fig = plt.figure(figsize=(19, 10))
        ax = {
            "raw": 0,
            "slice": 0,
            "roll": 0,
            "pitch": 0
        }
        ax["raw"] = fig.add_subplot(2,2,1,projection='3d') #type: ignore
        ax["slice"] = fig.add_subplot(2,2,2) #type: ignore
        ax["roll"] = fig.add_subplot(2,2,3)
        ax["pitch"] = fig.add_subplot(2,2,4)


        ax["raw"].set(xlim3d=(-90, 90), xlabel="roll")
        ax["raw"].set(ylim3d=(-90, 90), ylabel="pitch")
        ax["raw"].set(zlim3d=(0, prefRes[1]), zlabel="distance")
        ax["raw"].view_init(azim=orient["azim"], elev=orient["elev"])
        ax["raw"].grid(True)

        ax["slice"].axis("equal")
        ax["slice"].set_xlabel("roll")
        ax["slice"].set_ylabel("pitch")
        ax["slice"].set_xlim([-90, 90])
        ax["slice"].set_ylim([-90, 90])
        ax["slice"].grid(True)

        ax["roll"].set_xlabel("roll")
        ax["roll"].set_ylabel("area")
        ax["roll"].set_ylim([1000, 6000])
        ax["roll"].set_xlim([-90, 90])
        ax["roll"].grid(True)

        ax["pitch"].set_xlabel("pitch")
        ax["pitch"].set_ylabel("area")
        ax["pitch"].set_ylim([1000, 6000])
        ax["pitch"].set_xlim([-90, 90])
        ax["pitch"].grid(True)
    elif mode==1:
        fig = plt.figure()
        ax = fig.add_subplot(projection='3d')
        ax.set(xlim3d=(-90, 90), xlabel='roll')
        ax.set(ylim3d=(-90, 90), ylabel='pitch')
        ax.set(zlim3d=(0, 480), zlabel='distance')
        ax.view_init(azim=orient["azim"], elev=orient["elev"])
    if printText: print("check init end")
    # plt.grid()

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

def plt_update(n):
    # print(" Running:  \"update\"")
    global imgTemp, ret, contours, cntArea, cntPos, cntMoments, values, cntPos, isRecVals
    ret[0], imgTemp[0] = cam0.read()
    ret[1], imgTemp[1] = cam1.read()
    # print("check 2")
    if ret[0]==False or ret[1]==False: print("could not capture from cam. returned:", ret)
    else:    
        processFrame(imgTemp[0], 0, dispWin[0]) #solve contourArea and xy pos
        processFrame(imgTemp[1], 1, dispWin[1])
        cntPos[0] = None
        cntPos[1] = None
        tempPos = [None, None]
        cntArea = None
        brdrColor = (255,255,255)
        frame[0] = cv2.rectangle(frame[0], (int(prefRes[0]/2-op2_settings["thresh_xyLim"]["value"][0]), int(prefRes[1]/2-op2_settings["thresh_xyLim"]["value"][1])), (int(prefRes[0]/2+op2_settings["thresh_xyLim"]["value"][0]), int(prefRes[1]/2+op2_settings["thresh_xyLim"]["value"][1])), brdrColor, 2)
        morphImg[0] = cv2.rectangle(morphImg[0], (int(prefRes[0]/2-op2_settings["thresh_xyLim"]["value"][0]), int(prefRes[1]/2-op2_settings["thresh_xyLim"]["value"][1])), (int(prefRes[0]/2+op2_settings["thresh_xyLim"]["value"][0]), int(prefRes[1]/2+op2_settings["thresh_xyLim"]["value"][1])), brdrColor, 2)
        for flag in range(2):
            # for i in range(len(contours[flag])): #type: ignore
            tempPos, cntArea = solveContours(contours[flag], 0)

            if flag==0 and (abs(tempPos[0]-prefRes[0]/2)<op2_settings["thresh_xyLim"]["value"][0] and abs(prefRes[1]/2-tempPos[1])<op2_settings["thresh_xyLim"]["value"][1]):
                cntPos[0] = round((tempPos[0]-prefRes[0]/2)/10)*10 #type: ignore
                cntPos[1] = round((prefRes[1]/2-tempPos[1])/10)*10 #type: ignore
                brdrColor = (255, 255, 255)
                if displayToOpenCV:
                    morphImg[flag] = cv2.putText(morphImg[flag], \
                    str(int(cntArea)), \
                    (tempPos[0],tempPos[1]), \
                    font,1,(255,0,0),2) #type: ignore
                readAccelerometer(printText=False)
            elif flag==0:
                print(" OBJECT POS IS OUTSIDE op2_settings[\"thresh_xyLim\"] ", '\r')
                brdrColor = (255, 0, 0)
                #if displayToOpenCV:
                    #morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntArea)),(tempPos[0],tempPos[1]),font,1,(255,0,0),2) #type: ignore
                    #morphImg[flag] = cv2.copyMakeBorder(morphImg[flag], 20, 20, 20, 20, cv2.BORDER_CONSTANT, value=[255, 0, 0]) #type: ignore
            elif flag==1: # and cntArea >= op2_settings["thresh_zArea"]["value"]:
                tempPos[1] = int(round(tempPos[1]/10)*10) #type: ignore
                if displayToOpenCV:
                    morphImg[flag] = cv2.putText(morphImg[flag], \
                    str(int(tempPos[1])), \
                    (tempPos[0],tempPos[1]), \
                    font,1,(255,0,0),2) #type: ignore
                tempPos[1] = prefRes[1] - tempPos[1] #type: ignore

            if flag==1 and len(contours[flag])<=0:
                tempPos = [None, None]
        if cntArea != None and isRecVals:
            angleStr = f"{round(roll)}:{round(pitch)}"
            print(f"read: angles:{str(round(roll)):<3}:{str(round(pitch)):<3} area:{str(cntArea):<10} z:{str(cntPos[2]):<5}")
            if tempPos[1]==None:
                print("[USING PREVIOUS Z VALUE]", end='')
            else:
                cntPos[2] = tempPos[1]
                print("                        ", end='')
            if not (cntPos[2] in values):
                values.update( {
                        cntPos[2]: {
                            angleStr: [int(cntArea)]
                        }
                    }
                )
            elif not (angleStr in values[cntPos[2]]):
                values[cntPos[2]].update({
                        angleStr: [int(cntArea)]
                    }
                 )
            else:
                values[cntPos[2]][angleStr].append(int(cntArea))
                if len(values[cntPos[2]][angleStr]) >= 10: # check if there are more than 100 cntArea-values stored
                    values[cntPos[2]][angleStr] = [sum(values[cntPos[2]][angleStr])/len(values[cntPos[2]][angleStr])]
                    print(f"average area for z:\"{round(cntPos[2],2)}\" solved", end=' ')
            print("\r", end='')
        # ln.set_data(xData, yData)
    if displayToOpenCV:
        if not isRecVals:
            cv2.putText(morphImg[0], "NOT SAVING DATA", (20,40), font, 1, (0, 0, 255), 2)
        elif isRecVals:
            cv2.putText(morphImg[0], "Rec.", (20,40), font, 1, (0, 255, 0), 2)
        cv2.imshow(dispWin[0], cv2.resize(np.hstack((morphImg[0], frame[0])), None, fx=0.4, fy=0.4))
        cv2.imshow(dispWin[1], cv2.resize(np.hstack((morphImg[1], frame[1])), None, fx=0.4, fy=0.4))
        # print(cv2.getWindowImageRect(dispWin[0]))
        key = cv2.waitKey(5)
        if key==27:
            sys.exit()
        elif key==32:
            script_exit(strComments=["raw"])
            return False
        elif key==115:
            print([hsvCam0],[hsvCam1])
            np.save('hsv_value_cam0',[hsvCam0])
            np.save('hsv_value_cam1',[hsvCam1])
            script_exit(strComments=["raw"])
            return False
        elif key==97:
            cv2.imwrite("cam0_img", np.hstack((morphImg[0], frame[0])))
            cv2.imwrite("cam1_img", np.hstack((morphImg[1], frame[1])))
            time.sleep(1)
        elif key==114:
            isRecVals = True
    # print("check 3")
    return True


def readFile_loadValues(pf_index=1, mode=0):
    global values, allDataSets
    allDataSets = []
    values = {}
    correctPf = False
    nl_pos = 0
    strComments = []
    typeComments = []
    dateComments = []
    with open(profilesFile, "r") as readFile:
        for line in readFile:
            print(f"mode:{mode} reading line: {nl_pos}", end="\r")
            nl_pos+=1
            if mode==0:
                #solve sum of values
                if line[:8]=="profile:" and int(line[8])==pf_index: correctPf = True
                elif line[0:4] =="#sum": correctPf = False
                elif line[0]=="}": correctPf = False
                if correctPf and line[0:2] == "z:":
                    zVar = int(line[2:line.find(":", line.find(":")+1)])
                    lstLine = eval(line[line.find("["):])
                    for i in range(len(lstLine[0])):
                        angleStr = f"{round(lstLine[0][i])}:{round(lstLine[1][i])}"
                        if not (zVar in values):
                            values.update( {
                                    zVar: {
                                        angleStr: [lstLine[2][i]]
                                    }
                                }
                            )
                        elif not (angleStr in values[zVar]):
                            values[zVar].update({
                                    angleStr: [lstLine[2][i]]
                                }
                            )
                        else:
                            values[zVar][angleStr].append(lstLine[2][i])
                            if len(values[zVar][angleStr]) >= 10: # check if there are more than 100 cntArea-values stored
                                values[zVar][angleStr] = [sum(values[zVar][angleStr])/len(values[zVar][angleStr])]
                                # print(f"average area for z:\"{zVar}\" angles:\"{angleStr}\" solved")
                elif correctPf and line[0:1]=="#" and line[1:8] != "profile":
                        if line[1:5] == "date":
                            typeComments.append(" ")
                            strComments.append(" ")
                            dateComments.append(" ")
                            dateComments[len(dateComments)-1] = (line[1:-1])
                        elif line[1:4]=="raw" or line[1:4]=="sum":
                            typeComments[len(typeComments)-1] = (line[1:4])
                        elif not (line[1:5] == "date" or line[1:8] == "profile"):
                            strComments[len(strComments)-1] = (line[1:-1])
            elif mode==1:
                #independantly save values
                if line[:8]=="profile:" and int(line[8])==pf_index:
                    correctPf = True
                    allDataSets.append({})
                elif line[0]=="}": correctPf = False
                if correctPf and line[0:2]=="z:":
                    zVar = int(line[2:line.find(":", line.find(":")+1)])
                    lstLine = eval(line[line.find("["):])
                    allDataSets[len(allDataSets)-1].update({zVar: lstLine})
                elif correctPf and line[0:1]=="#" and line[1:8] != "profile":
                        if line[1:5] == "date":
                            typeComments.append(" ")
                            strComments.append(" ")
                            dateComments.append(" ")
                            dateComments[len(dateComments)-1] = (line[1:-1])
                        elif line[1:4]=="raw" or line[1:4]=="sum":
                            typeComments[len(typeComments)-1] = (line[1:4])
                        elif not (line[1:5] == "date" or line[1:8] == "profile"):
                            strComments[len(strComments)-1] = (line[1:-1])
    return strComments, typeComments, dateComments

def validSaveFig(figToSave, fileName, filePath, imgDpi=300, saveCopies=True, fileExt=".png"):
    if saveCopies:
        for i in range(10000):
            if not os.path.isfile(filePath+fileName+str(i)+fileExt):
                figToSave.savefig(filePath+fileName+str(i)+fileExt, dpi=imgDpi)
                break
    elif not saveCopies:
        figToSave.savefig(filePath+fileName+fileExt, dpi=imgDpi)


def opt0():
    #track and save new data sets from profile 1
    global cntPos, isRecVals
    if displayToOpenCV: isRecVals = False
    elif not displayToOpenCV: isRecVals = True
    cntPos = [None, None, None]
    opt0_setup()
    plt_init(mode=0)
    while True:
        if plt_update(0):
            pass
        else:
            break
    cv2.destroyAllWindows()
    print("plotting..")
    i = 0
    z_pick = 0
    for key,var in dataSets.items():
        if i==0: z_pick = key
        elif len(var[0]) > len(dataSets[z_pick][0]): z_pick = key
        i+=1

    
    # resultGraph = ax.scatter(data[""], data["y"], data["z"], c=data["area"], cmap="magma")
    resultGraph = plt.scatter(dataSets[z_pick][0], dataSets[z_pick][1], c=dataSets[z_pick][2], cmap="magma")

    plt.colorbar(resultGraph)
    plt.title(str(z_pick))

    fileName = "raw_z:"+str(z_pick)+"_"
    validSaveFig(plt, fileName, dirPath["media"])
    # for i in range(1000):
    #     if not os.path.isfile(fileName+str(i)+".png"):
    #         plt.savefig(dirPath["media"]+fileName+str(i)+".png", dpi=300)
    #         break
    plt.show()

def opt1():
    global values
    strComments, typeComments, dateComments = readFile_loadValues(mode=1)
    print(len(strComments), len(typeComments), len(dateComments))
    for i in range(len(typeComments)):
        print(f"-{i:<2}: {typeComments[i]:<5}: {dateComments[i]:<32}: {strComments[i]:<20}")
    print("enter indexes(i) to solved avg for")
    inp = input("input: ")
    if inp=="exit": sys.exit()
    elif inp=="back": return
    else:
        avgList = [int(i) for i in inp.split()]

    for n in avgList:
        for key,val in allDataSets[n].items():
            print(f"{key}: ")
            for i in range(len(val[0])):
                angleStr = f"{round(val[0][i])}:{round(val[1][i])}"
                if not (key in values):
                    values.update( {
                            key: {
                                angleStr: [val[2][i]]
                            }
                        }
                    )
                elif not (angleStr in values[key]):
                    values[key].update( {
                            angleStr: [val[2][i]]
                        }
                    )
                else:
                    values[key][angleStr].append(val[2][i])
                    if len(values[key][angleStr]) >= 10: # check if there are more than 100 cntArea-values stored
                            values[key][angleStr] = [sum(values[key][angleStr])/len(values[key][angleStr])]

    #read and sum all values for same profile (mainly 1)
    print("")
    script_exit(printText=False, strComments=["sum", f"idx:{[i for i in avgList]}"])
    print("plotting..")

    plotMethod = 1
    if plotMethod==0:
        plt_init(mode=0)
        i = 0
        z_pick = 0
        for key,var in dataSets.items():
            if i==0: z_pick = key
            elif len(var[0]) > len(dataSets[z_pick][0]): z_pick = key
            i+=1
        resultGraph = plt.scatter(dataSets[z_pick][0], dataSets[z_pick][1], c=dataSets[z_pick][2], cmap="magma")

        plt.title(str(z_pick))
        fileName = "sum_z:"+str(z_pick)+"_"

    elif plotMethod==1:
        plt_init(mode=1)
        numPoints = 0
        for key, val in dataSets.items():
            resultGraph = ax.scatter(val[0], val[1], len(val[0])*[key], c=val[2], cmap="magma",s=1)
            numPoints+=len(val[0])

        plt.title(f"complete plot: {numPoints} points")

        fileName = f"sum_fullPlot{[i for i in avgList]}_"


    plt.colorbar(resultGraph)

    validSaveFig(plt, fileName, dirPath["media"])
    # for i in range(10000):
    #     if not os.path.isfile(fileName+str(i)+".png"):
    #         plt.savefig(dirPath["media"]+fileName+str(i)+".png", dpi=300)
    #         break
    plt.show()


def ax_addPolyfits(ax, fig, x, y, n, ax_key=None, temp_x = [q for q in range(-90, 91)]):
    # global ax, fig

    # print(x, y)
    # print(f"solving polyfits for \"{ax_key}\"")
    if ax_key!=None:
        ax[ax_key].scatter(x, y, label="data", s=1, c="#fa0000")
        for i in n:
            givenPoly = np.polyfit(x, y, i)
            polyFunc = np.poly1d(givenPoly)
            ax[ax_key].plot(temp_x, polyFunc(temp_x), linestyle="solid", label=f"n:{i}", linewidth=1, alpha=0.7)
    else:
        ax.scatter(x, y, label="data", s=1, c="#fa0000")
        for i in n:
            givenPoly = np.polyfit(x, y, i)
            polyFunc = np.poly1d(givenPoly)
            ax.plot(temp_x, polyFunc(temp_x), linestyle="solid", label=f"n:{i}", linewidth=1, alpha=0.7)

def opt2():
    global orient, ax, fig, op2_settings, df
    strComments, typeComments, dateComments = readFile_loadValues(mode=1)
    chosen_pf = 0
    plotMethod = 0
    op2_settings["zPick"]["value"] = "auto"
    z_pick = 0
    polyfitMeth = 1 #0-use point range; 1-[-90, 90]

    fileName2 = f"polyfit_RollPitch_grid_"
    
    while True:
        zFuse = False
        clearScr = True
        op2_settings["show_predict"]["value"] = False

        while True:
            if clearScr:
                os.system("clear")
                print("\nNum. loaded profiles:", len(allDataSets))
                for i in range(len(typeComments)):
                    print(f"-{i:<2}: {typeComments[i]:<5}: {dateComments[i]:<32}: {strComments[i]:<20}")
            print("")
            pf_opt = input("input: ")
            if pf_opt=="back": return
            elif pf_opt=="exit": sys.exit()
            elif pf_opt[:8]=="settings":
                print(f"{'Name':<25}: {'Value':<25}: {'Type':<25}: Description")
                for key,val in op2_settings.items():
                    # print(f"{key:<20}: {str(val['value']):<20}: {str(val['type']):<20}: {val['description']}")
                    print(f"{key:<25}: ", end='')
                    print(f"{str(val['value']):<25}: ", end='')
                    print(f"{str(val['type']):<25}: ", end='')
                    print(f"{val['description']}")
                clearScr = False
            elif pf_opt[:8]=="savefig=": op2_settings["toSaveFig"]["value"] = eval(pf_opt[8:])
            elif pf_opt[:5]=="azim=": orient["azim"]=int(pf_opt[5:])
            elif pf_opt[:5]=="elev=": orient["elev"]=int(pf_opt[5:])
            elif pf_opt[:5]=="view=": #view=[azim, elev]
                orient["azim"]=int(eval(pf_opt[5:])[0])
                orient["elev"]=int(eval(pf_opt[5:])[1])
            elif pf_opt[:5]=="mode=": plotMethod = int(pf_opt[5])
            elif pf_opt[:7]=="zRange=": op2_settings["zPickRange"]["value"] = eval(pf_opt[7:])
            elif pf_opt[:7]=="pFirst=": op2_settings["show_predict"]["value"] = eval(pf_opt[7:])
            elif pf_opt[:8]=="polyfit=": op2_settings["polyfitRange"]["value"] = eval(pf_opt[8:])
            elif pf_opt[:14]=="polyfitMethod=": polyfitMeth = int(pf_opt[14:])
            elif pf_opt[:8]=="polyLim=": op2_settings['data_2d_lim']["value"] = [eval(pf_opt[8:]), eval(pf_opt[8:])]
            elif pf_opt[:4]=="eval":
                eval(pf_opt[5:])
                input("paused...")
            elif pf_opt[:2]=="z=" or ("=" in pf_opt and pf_opt[:pf_opt.find("=")]=="zPick"):
                if "auto" in pf_opt:
                    op2_settings["zPick"]["value"] = "auto"
                else:
                    op2_settings["zPick"]["value"] = str(round(int(pf_opt[pf_opt.find("=")+1:])/10)*10)
                    z_pick = int(op2_settings["zPick"]["value"])
            elif pf_opt[:4]=="fuse":
                zFuse = True
                chosen_pf = int(pf_opt[5:])
                plotMethod=0
                break
            elif pf_opt=="": pass
            elif pf_opt[0] in [str(i) for i in range(10)]:
                chosen_pf = int(pf_opt)
                clearScr = True
                break
            else:
                if "=" in pf_opt and pf_opt[:pf_opt.find("=")] in op2_settings:
                    op2_settings[pf_opt[:pf_opt.find("=")]]["value"] = op2_settings[pf_opt[:pf_opt.find("=")]]["func"](pf_opt[pf_opt.find("=")+1:])
                    # print(op2_settings[pf_opt[:pf_opt.find("=")]])
                    # input("pause")
                else:
                    print(f"Error: \"{pf_opt}\" is not a valid argument.")
                    time.sleep(1)
                clearScr = True

        if plotMethod==0:
            plt_init(printText=False, mode=0)
            i = 0
            if op2_settings["zPick"]["value"]=="auto":
                for key,var in allDataSets[chosen_pf].items():
                    if i==0: z_pick = key
                    elif len(var[0]) > len(allDataSets[chosen_pf][z_pick][0]): z_pick = key
                    i+=1
                op2_settings["zPick"]["value"]=int(z_pick)


            temp=[[], [], [], []]
            if not zFuse:
                for z in range(z_pick+op2_settings["zPickRange"]["value"][0], z_pick+op2_settings["zPickRange"]["value"][1]+1):
                    if z in allDataSets[chosen_pf]:
                        temp[0] += allDataSets[chosen_pf][z][0]
                        temp[1] += allDataSets[chosen_pf][z][1]
                        temp[2] += allDataSets[chosen_pf][z][2]
                        temp[3] += len(allDataSets[chosen_pf][z][0])*[z]
            elif zFuse:
                for key,val in allDataSets[chosen_pf].items():
                    print(f"fuse: reading z: {key}", end='\r')
                    temp[0] += val[0]
                    temp[1] += val[1]
                    temp[2] += val[2]
                    temp[3] += len(val[0])*[key]
                print()
            tempDict = {
                "roll": temp[0],
                "pitch": temp[1],
                "area": temp[2],
                "z": temp[3]
            }

            df = pandas.DataFrame(tempDict).dropna()
            if op2_settings["useSlice"]["value"]: regrFeed = [df[["roll", "pitch"]], df["area"]]
            elif not op2_settings["useSlice"]["value"]: regrFeed = [df[["roll", "pitch", "z"]], df["area"]]
            poly_model = PolynomialFeatures(degree=2)
            temp_polyXValues = poly_model.fit_transform(regrFeed[0])
            poly_model.fit(temp_polyXValues, regrFeed[1])
            regr = LinearRegression()
            regr.fit(temp_polyXValues, regrFeed[1])

            if op2_settings["saveCoef"]["value"]:
                filename_coef = "model_n2_coef.csv"
                coefFile_content = []

                currentDate = str(datetime.now())

                #z,xrange,yrange,zrange,intercept,a,b,a^2,ab,b^2,date
                with open(dirPath["script"]+filename_coef, "r") as f:
                    read = csv.reader(f)
                    coefFile_content = [row for row in read if len(row)!=0]
                model_coefs = regr.coef_
                model_intercept = regr.intercept_

                checkRow = [
                    str(chosen_pf),
                    str(op2_settings["zPick"]["value"]),
                    str(op2_settings["data_2d_lim"]["value"][0]).replace(", ", ":"),
                    str(op2_settings["data_2d_lim"]["value"][1]).replace(", ", ":"),
                    str(op2_settings["zPickRange"]["value"]).replace(", ", ":"),
                    str(model_intercept),
                    str(model_coefs[1]),
                    str(model_coefs[2]),
                    str(model_coefs[3]),
                    str(model_coefs[4]),
                    str(model_coefs[5]),
                    currentDate
                ]
                if zFuse: checkRow[4]="[0:400]"


                boolChecks = 5*[0]
                # for n in coefFile_content: print(n)

                idx_lists = [[n for n in range(1, len(coefFile_content)) if coefFile_content[n][0]==checkRow[0]]] + 4*[0]
                boolChecks[0] = checkRow[0] in [coefFile_content[n][0] for n in idx_lists[0]]

                print("\n",idx_lists[0],sep='')

                print("--in loop--")
                for d in range(1, 5):
                    idx_lists[d] = [i for i in idx_lists[d-1] if coefFile_content[i][d]==checkRow[d]]
                    # print(idx_lists[d])
                    boolChecks[d] = checkRow[d] in [coefFile_content[i][d] for i in idx_lists[d]]
                # print(" -boolchecks: ", boolChecks)
                # print(" -idx_lists:  ", idx_lists)
                print(checkRow)
                
                fullBreak = False
                print("Checking csv row with existing file...")
                if not False in boolChecks:
                    print(f"Matched n:5 elements: found a copy: overwriting")
                    idx = idx_lists[4][0]
                    coefFile_content[idx][5:] = checkRow[5:]
                else:
                    for rowIdx in range(4, 0, -1):
                        if not False in boolChecks[:rowIdx]:
                            print(f"Matched n:{rowIdx} elements:")
                            for i in idx_lists[rowIdx-1]:
                                if rowIdx in [4, 3]:
                                    if sum([abs(f) for f in eval(checkRow[rowIdx].replace(":", ", "))]) < sum([abs(f) for f in eval(coefFile_content[i][rowIdx].replace(":", ", "))]):
                                        coefFile_content.insert(i, checkRow)
                                        fullBreak=True
                                        break
                                elif rowIdx in [2, 1]:
                                    if int(checkRow[rowIdx]) < int(coefFile_content[i][rowIdx]):
                                        coefFile_content.insert(i, checkRow)
                                        fullBreak=True
                                        break
                            else:
                                coefFile_content.insert(idx_lists[rowIdx-1][-1]+1, checkRow)
                                break
                        if fullBreak: break
                    else:
                        print("No matches")
                        coefFile_content.insert(len(coefFile_content), checkRow)

                print("Writing csv file..")
                with open(dirPath["script"]+filename_coef, "w") as f:
                    write = csv.writer(f)
                    write.writerows(coefFile_content)

            # regr = linear_model.LinearRegression()
            # regr.fit(regrFeed[0].values, regrFeed[1])

            print("creating regr. data...\n")
            regrData = {"roll":[], "pitch":[], "z":[] ,"area":[]}
            
            def polyChangeList(pos):
                return np.squeeze(regr.predict(poly_model.fit_transform(np.array([pos]))))

            #is used to simulate only to the furthest axis value of given points
            #biggestVal = max([max([abs(i) for i in tempDict["roll"]]), max([abs(i) for i in tempDict["pitch"]])])
            biggestVal = 91
            for flip in range(-1, 2, 2):
                for x in range(0, biggestVal, 1):
                    for y in range(0, biggestVal-x, 1):
                        pos = [x*flip, y]
                        if not op2_settings["useSlice"]["value"]: pos.append(int(op2_settings["zPick"]["value"]))
                        print(f"-solving regr. point: {pos}  ", end="\r")
                        regrData["roll"].append(pos[0])
                        regrData["pitch"].append(pos[1])
                        if not op2_settings["useSlice"]["value"]: regrData["z"].append(pos[2])
                        regrData["area"].append(int(polyChangeList(pos)))
                        
            for flip in range(-1, 2, 2):
                for x in range(0, biggestVal, 1):
                    for y in range(0, (0-biggestVal+x), -1):
                        pos = [x*flip, y]
                        if not op2_settings["useSlice"]["value"]: pos.append(int(op2_settings["zPick"]["value"]))
                        print(f"-solving regr. point: {pos}  ", end="\r")
                        regrData["roll"].append(pos[0])
                        regrData["pitch"].append(pos[1])
                        if not op2_settings["useSlice"]["value"]: regrData["z"].append(pos[2])
                        regrData["area"].append(int(polyChangeList(pos)))

            print("scatter plotting data sets:")
            resultGraph1 = ax["slice"].scatter(regrData["roll"], regrData["pitch"], c=regrData["area"], s=1.5, cmap="magma")
            
            ax["roll"].title.set_text(f"lim:{str(op2_settings['data_2d_lim']['value'][0])}")
            ax["pitch"].title.set_text(f"lim:{str(op2_settings['data_2d_lim']['value'][1])}")
            data_2d_strt = [0, 0]
            data_2d_lsts = [[], []]
            data_2d = {
                "roll": [[], []],
                "pitch": [[], []]
            }

            if not op2_settings["show_predict"]["value"]: 
                # resultGraph0 = ax["slice"].scatter(allDataSets[chosen_pf][z_pick][0], allDataSets[chosen_pf][z_pick][1], c=allDataSets[chosen_pf][z_pick][2], s=2, cmap="magma")
                resultGraph0 = ax["slice"].scatter(tempDict["roll"], tempDict["pitch"], c=tempDict["area"], s=2, cmap="magma")
                data_2d_lsts = [
                    [tempDict["roll"], tempDict["area"]],
                    [tempDict["pitch"], tempDict["area"]]
                    ]
            numPoints = 0
            for key, val in allDataSets[chosen_pf].items():
                resultGraph2 = ax["raw"].scatter(val[0], val[1], len(val[0])*[key], c=val[2], cmap="magma", s=1)
                numPoints+=len(val[0])
            
            
            for i in range(len(data_2d_lsts[0][0])):
                # print(data_2d_lsts[1][0][i], "-------------", data_2d_lsts[0][0][i])
                if data_2d_lsts[1][0][i]>(data_2d_strt[1]+op2_settings['data_2d_lim']["value"][1][0]) and data_2d_lsts[1][0][i]<(data_2d_strt[1]+op2_settings['data_2d_lim']["value"][1][1]):
                    data_2d["roll"][0].append(data_2d_lsts[0][0][i])
                    data_2d["roll"][1].append(data_2d_lsts[0][1][i])
                if data_2d_lsts[0][0][i]>(data_2d_strt[0]+op2_settings['data_2d_lim']["value"][0][0]) and data_2d_lsts[0][0][i]<(data_2d_strt[0]+op2_settings['data_2d_lim']["value"][0][1]):
                    data_2d["pitch"][0].append(data_2d_lsts[1][0][i])
                    data_2d["pitch"][1].append(data_2d_lsts[0][1][i])

            tempCheck = [len(val) for val in data_2d["roll"]]+[len(val) for val in data_2d["pitch"]]
            axisFuncs = [0, 0]
            if not 0 in tempCheck:
                saveFigCheck = [True, True]
                ax_addPolyfits(ax, fig, data_2d["roll"][0], data_2d["roll"][1], op2_settings["polyfitRange"]["value"], "roll")
                ax_addPolyfits(ax, fig, data_2d["pitch"][0], data_2d["pitch"][1], op2_settings["polyfitRange"]["value"], "pitch")
                axisFuncs[0] = np.poly1d(np.polyfit(data_2d["roll"][0], data_2d["roll"][1], 2))
                axisFuncs[1] = np.poly1d(np.polyfit(data_2d["pitch"][0], data_2d["pitch"][1], 2))
                ax["roll"].legend()
                ax["pitch"].legend()


                fileName2 = f"polyfit_RollPitch_grid_n{chosen_pf}_"
                if zFuse: fileName2 += "z:FUSED_"
                elif not zFuse: fileName2 += f"z:{z_pick}_"
                fileName2+=f'[{orient["azim"]},{orient["elev"]}]_'+ \
                    f"{op2_settings['zPickRange']['value']}_".replace(" ", "")+ \
                    f"{op2_settings['data_2d_lim']['value']}_".replace(" ","")+ \
                    f"{op2_settings['useSlice']['value']}_".replace(" ", "")
                fig2 = plt.figure(figsize=(19,9))
                ax2 = {
                    "roll": 0,
                    "pitch": 0,
                    "roll2d": 0,
                    "pitch2d": 0,
                    "mixed": 0,
                    "mixed2d": 0,
                    "predict": 0,
                    "predict2d": 0
                }
                ax2["roll"] = fig2.add_subplot(2, 4, 1, projection='3d')
                ax2["pitch"] = fig2.add_subplot(2, 4, 2, projection='3d')
                ax2["mixed"] = fig2.add_subplot(2, 4, 3, projection='3d')
                ax2["predict"]= fig2.add_subplot(2, 4, 4, projection='3d')
                ax2["roll2d"] = fig2.add_subplot(2, 4, 5)
                ax2["pitch2d"] = fig2.add_subplot(2, 4, 6)
                ax2["mixed2d"] = fig2.add_subplot(2, 4, 7)
                ax2["predict2d"] = fig2.add_subplot(2, 4, 8)
                ax2_polyN = op2_settings["polyfitRange"]["value"]
                ax2_polyFuncs = {
                    "roll": [[], [], [], [[], [], []]], #[ [pos,], [func,], [[min, max],], [[], [], []] ]
                    "pitch": [[], [], [], [[], [], []]]
                }
                for key in ax2:
                    if key[-2:] != "2d":
                        ax2[key].set(xlim3d=(-90, 90), xlabel="roll")
                        ax2[key].set(ylim3d=(-90, 90), ylabel="pitch")
                        ax2[key].set(zlim3d=(1, 6000), zlabel="area")
                        ax2[key].view_init(azim=orient["azim"], elev=orient["elev"])
                    elif key[-2:] == "2d":
                        ax2[key].axis("equal")
                        ax2[key].set_xlabel("roll")
                        ax2[key].set_ylabel("pitch")
                        ax2[key].set_xlim([-90, 90])
                        ax2[key].set_ylim([-90, 90])
                    ax2[key].title.set_text(key)
                    ax2[key].grid(True)
                

                print("\nCreating polyfit grid(s):")
                for n in range(-90, 90, abs(op2_settings['data_2d_lim']["value"][0][0])+op2_settings['data_2d_lim']["value"][0][1]):
                    print(f"- roll: n:{n}", end="\r")
                    data_2d_strt[1] = n
                    data_2d["roll"] = [[], []]
                    for i in range(len(data_2d_lsts[0][0])):
                        if data_2d_lsts[1][0][i]>(data_2d_strt[1]+op2_settings['data_2d_lim']["value"][1][0]) \
                        and data_2d_lsts[1][0][i]<(data_2d_strt[1]+op2_settings['data_2d_lim']["value"][1][1]) \
                        and len(data_2d_lsts[0][0])>=op2_settings["data_2d_nLim"]["value"][0]:
                            data_2d["roll"][0].append(data_2d_lsts[0][0][i])
                            data_2d["roll"][1].append(data_2d_lsts[0][1][i])
                    if len(data_2d["roll"][0])>=op2_settings["data_2d_nLim"]["value"][0]:
                        for q in ax2_polyN:
                            givenPoly = np.polyfit(data_2d["roll"][0], data_2d["roll"][1], q)
                            polyFunc = np.poly1d(givenPoly)
                            if polyFunc.coeffs[0]<0:
                                ax2_polyFuncs["roll"][0].append(n)
                                ax2_polyFuncs["roll"][1].append(polyFunc)
                                ax2_polyFuncs["roll"][2].append([min(data_2d["roll"][0]), max(data_2d["roll"][0])])
                                temp_x = [x for x in range(min(data_2d["roll"][0]), max(data_2d["roll"][0]))]
                                ax2["roll"].plot(temp_x, polyFunc(temp_x), zdir='y', zs=n, linestyle="solid", label=f"{n} n:{q}", linewidth=1, c="#32a852")
                                ax2["mixed"].plot(temp_x, polyFunc(temp_x), zdir='y', zs=n, linestyle="solid", label=f"{n} n:{q}", linewidth=1, c="#32a852")
                ax2["roll"].title.set_text(f"roll n:{ax2_polyN}")
                print("")
                for n in range(-90, 90, abs(op2_settings['data_2d_lim']["value"][1][0])+op2_settings['data_2d_lim']["value"][1][1]):
                    print(f"- pitch: n:{n}", end="\r")
                    data_2d_strt[0] = n
                    data_2d["pitch"] = [[], []]
                    for i in range(len(data_2d_lsts[1][0])):
                        if data_2d_lsts[0][0][i]>(data_2d_strt[0]+op2_settings['data_2d_lim']["value"][0][0]) \
                        and data_2d_lsts[0][0][i]<(data_2d_strt[0]+op2_settings['data_2d_lim']["value"][0][1]) \
                        and len(data_2d_lsts[1][0])>=op2_settings["data_2d_nLim"]["value"][1]:
                            data_2d["pitch"][0].append(data_2d_lsts[1][0][i])
                            data_2d["pitch"][1].append(data_2d_lsts[1][1][i])
                    if len(data_2d["pitch"][0])>=op2_settings["data_2d_nLim"]["value"][1]:
                        for q in ax2_polyN:
                            givenPoly = np.polyfit(data_2d["pitch"][0], data_2d["pitch"][1], q)
                            polyFunc = np.poly1d(givenPoly)
                            if polyFunc.coeffs[0]<0:
                                ax2_polyFuncs["pitch"][0].append(n)
                                ax2_polyFuncs["pitch"][1].append(polyFunc)
                                ax2_polyFuncs["pitch"][2].append([min(data_2d["pitch"][0]), max(data_2d["pitch"][0])])
                                temp_x = [x for x in range(min(data_2d["pitch"][0]), max(data_2d["pitch"][0]))]
                                ax2["pitch"].plot(temp_x, polyFunc(temp_x), zdir='x', zs=n, linestyle="solid", label=f"{n} n:{q}", linewidth=1, c="#32a852")
                                ax2["mixed"].plot(temp_x, polyFunc(temp_x), zdir='x', zs=n, linestyle="solid", label=f"{n} n:{q}", linewidth=1, c="#32a852")
                ax2["pitch"].title.set_text(f"pitch n:{ax2_polyN}")
            else:
                print("\nTempCheck = ", tempCheck)
                exit()
                # saveFigCheck = [True, False]

            expResult = [0, 6000]
            for key in ax2_polyFuncs:
                if len(ax2_polyFuncs[key][0])>0:
                    for i in range(len(ax2_polyFuncs[key][0])):
                        print(f"{key:<6}: {i} ", end='')
                        n_val = ax2_polyFuncs[key][0][i]
                        if polyfitMeth==0: temp_x = [x for x in range(ax2_polyFuncs[key][2][i][0], ax2_polyFuncs[key][2][i][1])]
                        elif polyfitMeth==1: temp_x = [x for x in range(-(90-abs(n_val)), 90-abs(n_val)+1)]
                        f_temp = ax2_polyFuncs[key][1][i](temp_x).tolist()
                        # print(ax2_polyFuncs[key][1][i])
                        # print(ax2_polyFuncs[key][2][i])
                        # print(temp_x)
                        # print(f_temp)
                        # print("\n")
                        if len(ax2_polyFuncs[key][2][i])>=2 \
                            and ax2_polyFuncs[key][2][i][0] != ax2_polyFuncs[key][2][i][1] \
                            and min(f_temp)>=expResult[0] \
                            and max(f_temp)<=expResult[1]:
                            ax2_polyFuncs[key][3][0] += temp_x
                            ax2_polyFuncs[key][3][1] += len(temp_x)*[n_val]
                            ax2_polyFuncs[key][3][2] += ax2_polyFuncs[key][1][i](temp_x).tolist()
                        else:
                            print(f"[not included: had values exceeding {expResult} range]")
                        print("", end="\r")
            print("")
            polyfitGraph0 = ax2["roll2d"].scatter(ax2_polyFuncs["roll"][3][0], ax2_polyFuncs["roll"][3][1], c=ax2_polyFuncs["roll"][3][2], s=2, cmap="magma")
            polyfitGraph1 = ax2["pitch2d"].scatter(ax2_polyFuncs["pitch"][3][1], ax2_polyFuncs["pitch"][3][0], c=ax2_polyFuncs["pitch"][3][2], s=2, cmap="magma")
            fig2.colorbar(polyfitGraph0, ax=ax2["roll2d"])
            fig2.colorbar(polyfitGraph1, ax=ax2["pitch2d"])
            
            tempValues = {} # "x:y": z,
            tempLst = [[], [], []]
            for key in ax2_polyFuncs:
                for i in range(len(ax2_polyFuncs[key][3][0])):
                    print(f"checking for avg: i: {i} ", end='')
                    zVal = ax2_polyFuncs[key][3][2][i]
                    if key=="roll": strPos = f"{ax2_polyFuncs[key][3][0][i]}:{ax2_polyFuncs[key][3][1][i]}"
                    if key=="pitch": strPos = f"{ax2_polyFuncs[key][3][1][i]}:{ax2_polyFuncs[key][3][0][i]}"
                    if strPos in tempValues: tempValues[strPos] = round((tempValues[strPos]+zVal)/2); print(" solved avg", end='')
                    elif strPos not in tempValues: tempValues.update({strPos: zVal})
                    print(f"{' ':<36}", end='\r')
                          
            for key,val in tempValues.items():
                tempLst[0].append(int(key[:key.find(":")]))
                tempLst[1].append(int(key[key.find(":")+1:]))
                tempLst[2].append(val)
            ax2["mixed2d"].scatter(tempLst[0], tempLst[1], c=tempLst[2], s=3, cmap="magma")
            
            print("finding value(s) closest to [0, 0]:")
            cntrVal = [tempLst[0][0], tempLst[1][0], tempLst[2][0]]
            for i in range(1, len(tempLst[0])):
                print(f"closest: ", cntrVal, f"{'':10}", end='\r')
                if abs(tempLst[0][i])+abs(tempLst[1][i])<abs(cntrVal[0])+abs(cntrVal[1]):
                    cntrVal = [tempLst[0][i], tempLst[1][i], tempLst[2][i]]
            print("\n")
            
            tempDict = {
                "roll": tempLst[0],
                "pitch": tempLst[1],
                "area": tempLst[2]
            }
            
            def predFunc0(x, y):
                print("\n", axisFuncs[0], '', axisFuncs[1], sep='\n')
                z_0 = [axisFuncs[0](x).tolist(), axisFuncs[1](y).tolist()]
                z_coef = [0, 0]
                z_coef[0] = [round(n/cntrVal[2],2) for n in z_0[0]]
                z_coef[1] = [round(n/cntrVal[2],2) for n in z_0[1]]
                z = []
                for i in range(len(z_coef[0])):
                    z.append(z_coef[0][i]*z_coef[1][i])
                return z
                      
            print("creating polyfit based regr. data...\n")
            
            predData = [[], [], []]
            for i_x in range(-90, 91, 1):
                for i_y in range(-90, 91, 1):
                    print(f"- x:{i_x:<3} y:{i_y:<3}", end='\r')
                    pos = [i_x, i_y]
                    predData[0].append(i_x)
                    predData[1].append(i_y)
            predData[2] = predFunc0(predData[0], predData[1])
            print("")
            print(len(predData[0]))
            # meshX, meshY = np.meshgrid(predData[0], predData[1], sparse=True)
            ax2["predict"].plot_trisurf(predData[0], predData[1], np.array(predData[2]), cmap=cm.coolwarm, antialiased=False)
            ax2["predict"].set(zlim3d=(0, 1), zlabel="coeff")
            #ax2["predict"].set(xticklabels=[], yticklabels=[], zticklabels=[])
            #ax2["predict"].scatter(predData[0], predData[1], predData[2], c=predData[2], s=2)
            #fig2.colorbar(ax2["predict"].collections[0], ax=ax2["predict"])
            dataFileName = f"dataSet_pf{chosen_pf}_fuse-{str(zFuse)}.dat".replace(" ","")
            
            print("\ncreating csv file(s)....")
            csv_fields = ["Roll","Pitch","Area"]
            csv_rows = [[predData[0][n],predData[1][n],predData[2][n]] for n in range(len(predData[0]))]
            with open(dirPath["data"]+"csv_"+dataFileName[:-4]+".csv", "w") as f:
                write = csv.writer(f)
                write.writerow(csv_fields)
                write.writerows(csv_rows)

            temp = [
                df["roll"].tolist(),
                df["pitch"].tolist(),
                df["area"].tolist()
            ]
            csv_rows = [[temp[0][n],temp[1][n],temp[2][n]] for n in range(len(temp[0]))]
            with open(dirPath["data"]+"raw_csv_"+dataFileName[:-4]+".csv", "w") as f:
                write = csv.writer(f)
                write.writerow(csv_fields)
                write.writerows(csv_rows)


            dataFile = open(dirPath["data"]+dataFileName, "w")
            for sets in predData: dataFile.write(str(sets)+"\n")
            dataFile.close()


            print("\nplt plot miscellanious stuff...")
            ax2["predict2d"].scatter(predData[0], predData[1], c=predData[2], s=2, cmap="magma")
            fig2.colorbar(ax2["predict2d"].collections[0], ax=ax2["predict2d"])
            
            
            if not op2_settings["show_predict"]["value"]:
                fig.colorbar(resultGraph0, ax=ax["slice"])
            fig.colorbar(resultGraph1, ax=ax["slice"])
            fig.colorbar(resultGraph2, ax=ax["raw"])

            ax["raw"].title.set_text(f"3D plot: idx:{strComments[chosen_pf]}")
            fileName = f"n{chosen_pf}_"
            if zFuse:
                ax["slice"].title.set_text(f"idx:{chosen_pf} z:FUSED")
                fileName = f"n{chosen_pf}_z:FUSED_"
            elif not zFuse:
                ax["slice"].title.set_text(f"idx:{chosen_pf} z:{z_pick}")
                fileName = f"n{chosen_pf}_z:{z_pick}_"
            fileName+=f'[{orient["azim"]},{orient["elev"]}]_'+ \
                f'{op2_settings["zPickRange"]["value"]}_'.replace(" ", "")+ \
                f"{op2_settings['data_2d_lim']['value']}_".replace(" ","")+ \
                f"{op2_settings['useSlice']['value']}_".replace(" ", "")

        elif plotMethod==1:
            plt_init(printText=False, mode=1)
            numPoints = 0
            for key, val in allDataSets[chosen_pf].items():
                resultGraph = ax.scatter(val[0], val[1], len(val[0])*[key], c=val[2], cmap="magma",s=1)
                numPoints+=len(val[0])

            plt.title(f"idx:{chosen_pf} complete plot: {numPoints} points")
            fileName = f"n{chosen_pf}_"+str(orient["azim"])+":"+str(orient["elev"])+"_"+f"{op2_settings['data_2d_lim']['value']}_".replace(" ","")
            plt.colorbar(resultGraph)
            saveFigCheck = [True, False]
            
        saveEx = False
        if op2_settings["toSaveFig"]["value"]:
            if saveFigCheck[0]:  validSaveFig(fig, fileName, dirPath["media"], imgDpi=fig.dpi+100, saveCopies=saveEx); saveFigCheck[0] = False
            if saveFigCheck[1]:  validSaveFig(fig2, fileName2, dirPath["media"], imgDpi=fig.dpi+100, saveCopies=saveEx); saveFigCheck[1] = False
        
        print("showing plot(s)...")
        plt.show()


def opt3():
    csvFile = pandas.read_csv("model_n2_coef.csv")

    chosen_idx = 0

    xValues = csvFile["z"][1:]

    yValues = [
        csvFile["intercept"][1:],
        csvFile["a"][1:],
        csvFile["b"][1:],
        csvFile["a^2"][1:],
        csvFile["ab"][1:],
        csvFile["b^2"][1:],
    ]
    # print(yValues[2])

    fig = plt.figure(dpi=150)
    ax = fig.subplots()
    ax.plot(xValues, yValues[chosen_idx])
    
    ax_addPolyfits(ax, fig, xValues, yValues[chosen_idx], [n for n in range(1, 5)], temp_x=[x for x in range(round(min(xValues)),round(max(xValues)))])

    plt.legend()

    plt.show()

    # input("paused")
    return


def main():
    while True:
        os.system("clear")
        print("Options:")
        print("0. Track and create new data sets for profile 1")
        print("1. Load and save average of all dataSets for profile 1")
        print("2. Display dataSets from profiles File")
        print("3. Create polfit models of op2 coefs")

        inp = input("input: ")
        if inp=="exit": break
        elif inp=="0": opt0()
        elif inp=="1": opt1()
        elif inp=="2": opt2()
        elif inp=="3": opt3()

if __name__=="__main__":
    main()


