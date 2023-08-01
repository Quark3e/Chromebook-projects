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
from matplotlib.animation import FuncAnimation
import sys
import socket
import pandas
from sklearn import linear_model
from datetime import datetime

profilesFile = "angleArea_profiles.dat"
orient = {
    "azim": 0,
    "elev": 0
}

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
# {0.05:0.17:0.88}off;

X_out, Y_out, Z_out = 0.1, 0.1, 0.1
Roll, Pitch, roll, pitch = 0.1, 0.1, 0.1, 0.1
tiltFilter = 0.1

def readAccelerometer():
    global X_out, Y_out, Z_out, Roll, Pitch, roll, pitch
    reqToServer()
    if len(server_msg)>=1 and (server_msg[:1]=="{" and server_msg[-5]=="}" and server_msg[-1]==";"):
        tempMsg = server_msg.replace(":", ",")
        msgTuple = eval(tempMsg[1:-5])
        X_out, Y_out, Z_out = msgTuple

    print(f" accel: x:{X_out} y:{Y_out} z:{Z_out} roll:{roll} pitch:{pitch}", end='\r')
    
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
    print(f" accel: x:{X_out} y:{Y_out} z:{Z_out} roll:{roll} pitch:{pitch}", end='\r')

reqToServer()
readAccelerometer()

def nothing(x):
    pass


displayToOpenCV = True

def opt0_setup():
    global cam0, cam1, dispWin, hsvCam0, hsvCam1
    cam0 = cv2.VideoCapture(0)
    cam1 = cv2.VideoCapture(2)

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
        cv2.moveWindow(dispWin[0], 100, 500)
        cv2.moveWindow(dispWin[1], 100+700, 500)

#   int(zAxis): [cntArea],

values = {} 
# temporary holder for values where each "angle" has several data values
#   z:
#   {
#       angle: [area,],
#       angle: [area,],
#   }
#   z:
#   {
#       angle: [area,],
#       angle: [area,],
#   }
#   ...


dataSets = {}
# "permanet" holder for values in data sets where each angle has a single corresponding area value
#   z: [[angle_roll,], [angle_pitch,], [area,]],
#   z: [[angle_roll,], [angle_pitch,], [area,]],
#   ---

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
        fig, ax = plt.subplots()
        plt.xlim(-90, 90)
        plt.ylim(-90, 90)
        plt.xlabel("roll")
        plt.ylabel("pitch")
    elif mode==1:
        fig = plt.figure()
        ax = fig.add_subplot(projection='3d')
        ax.set(xlim3d=(-90, 90), xlabel='roll')
        ax.set(ylim3d=(-90, 90), ylabel='pitch')
        ax.set(zlim3d=(0, 480), zlabel='distance')
        ax.view_init(azim=orient["azim"], elev=orient["elev"])
    if printText: print("check init end")
    plt.grid()

def plt_update(n):
    # print("check update")
    global imgTemp, ret, contours, cntArea, cntPos, cntMoments, values, cntPos
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

        for flag in range(2):
            for i in range(len(contours[flag])): #type: ignore
                cntMoments = cv2.moments(contours[flag][0]) #type: ignore
                if cntMoments['m00'] != 0:
                    if flag==0:
                        tempPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                        cntPos[0] = round((tempPos[0]-prefRes[0]/2)/10)*10 #type: ignore
                        cntPos[1] = round((prefRes[1]/2-tempPos[1])/10)*10 #type: ignore
                        cntArea = cv2.contourArea(contours[flag][0]) #type: ignore
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(cntArea)),(tempPos[0],tempPos[1]),font,1,(255,0,0),2) #type: ignore
                    elif flag==1:
                        readAccelerometer()
                        tempPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
                        tempPos[1] = round(tempPos[1]/10)*10 #type: ignore
                        #cntPos[2] = round(tempPos[1]/10)*10 #type: ignore
                        if displayToOpenCV:
                            morphImg[flag] = cv2.putText(morphImg[flag],str(int(tempPos[1])),(tempPos[0],tempPos[1]),font,1,(255,0,0),2) #type: ignore
                        tempPos[1] = prefRes[1] - tempPos[1] #type: ignore
                        #cntPos[2] = prefRes[1] - cntPos[2] #type: ignore
                else: print("cntMoments['m00'] = 0")
        if cntArea != None:
            if tempPos[1]==None:
                print(f"read: {'area:':<8}: {cntArea} {'z:':<8}:{cntPos[2]} [USING PREVIOUS Z VALUE]", end='\r')
            else:
                cntPos[2] = tempPos[1]
                print(f"read: {'area:':<8}: {cntArea} {'z:':<8}:{cntPos[2]:<25} ", end='\r')
            angleStr = f"{round(roll)}:{round(pitch)}"
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
                    print(f"average area for z:\"{cntPos[2]}\" angles:\"{angleStr}\" solved")

        # ln.set_data(xData, yData)
    if displayToOpenCV:
        cv2.imshow(dispWin[0], cv2.resize(np.hstack((morphImg[0], frame[0])), None, fx=0.4, fy=0.4))
        cv2.imshow(dispWin[1], cv2.resize(np.hstack((morphImg[1], frame[1])), None, fx=0.4, fy=0.4))
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
    with open(profilesFile, "r") as readFile:
        for line in readFile:
            print(f"mode:{mode} reading line: {nl_pos}", end="\r")
            if line[0:1]=="#":
                if line[1:4]=="raw" or line[1:4]=="sum": typeComments.append(line[1:4])
                strComments.append(line[1:-1])
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
    return strComments, typeComments

def opt0():
    #track and save new data sets from profile 1
    global cntPos
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

    fileName = "objDist_angleArea_media/raw_z:"+str(z_pick)+"_"
    for i in range(1000):
        if not os.path.isfile(fileName+str(i)+".png"):
            plt.savefig(fileName+str(i)+".png", dpi=300)
            break
    plt.show()

def opt1():


    readFile_loadValues()
    #read and sum all values for same profile (mainly 1)
    print("")
    script_exit(printText=False, strComments=["sum"])
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
        fileName = "objDist_angleArea_media/sum_z:"+str(z_pick)+"_"

    elif plotMethod==1:
        plt_init(mode=1)
        numPoints = 0
        for key, val in dataSets.items():
            resultGraph = ax.scatter(val[0], val[1], len(val[0])*[key], c=val[2], cmap="magma",s=1)
            numPoints+=len(val[0])

        plt.title(f"complete plot: {numPoints} points")

        fileName = "objDist_angleArea_media/sum_fullPlot_"


    plt.colorbar(resultGraph)

    for i in range(10000):
        if not os.path.isfile(fileName+str(i)+".png"):
            plt.savefig(fileName+str(i)+".png", dpi=300)
            break
    plt.show()

def opt2():
    global orient
    strComments, typeComments = readFile_loadValues(mode=1)
    chosen_pf = 0
    plotMethod = 0
    print("\nNum. loaded profiles:", len(allDataSets))
    for i in range(len(typeComments)):
        print(f"-{i}: {typeComments[i]}")
    while True:
        while True:
            pf_opt = input("input: ")
            if pf_opt=="back": return
            elif pf_opt=="exit": sys.exit()
            elif pf_opt[:5]=="azim=": orient["azim"]=int(pf_opt[5:])
            elif pf_opt[:5]=="elev=": orient["elev"]=int(pf_opt[5:])
            elif pf_opt[:5]=="mode=": plotMethod = int(pf_opt[5])
            elif pf_opt=="": pass
            else:
                chosen_pf = int(pf_opt)
                break

        if plotMethod==0:
            plt_init(printText=False, mode=0)
            i = 0
            z_pick = 0
            for key,var in allDataSets[chosen_pf].items():
                if i==0: z_pick = key
                elif len(var[0]) > len(allDataSets[chosen_pf][z_pick][0]): z_pick = key
                i+=1

            tempDict = {
                "roll": allDataSets[chosen_pf][z_pick][0],
                "pitch": allDataSets[chosen_pf][z_pick][1],
                "area": allDataSets[chosen_pf][z_pick][2]
            }
            df = pandas.DataFrame(tempDict)
            regrFeed = [
                df[["roll", "pitch"]],
                df["area"]
            ]
            regr = linear_model.LinearRegression()
            regr.fit(regrFeed[0].values, regrFeed[1])

            print("creating regr. data...")
            regrData = {"roll":[], "pitch":[], "area":[]}
            
            #is used to simulate only to the furthest axis value of given points
            #biggestVal = max([max([abs(i) for i in tempDict["roll"]]), max([abs(i) for i in tempDict["pitch"]])])
            biggestVal = 91
            for flip in range(-1, 2, 2):
                for x in range(0, biggestVal, 1):
                    for y in range(0, biggestVal-x, 1):
                        pos = [x*flip, y]
                        print(f"-creating point: {pos}  ", end="\r")
                        regrData["roll"].append(pos[0])
                        regrData["pitch"].append(pos[1])
                        regrData["area"].append(int(regr.predict([pos])))
                        
            for flip in range(-1, 2, 2):
                for x in range(0, biggestVal, 1):
                    for y in range(0, (0-biggestVal+x), -1):
                        pos = [x*flip, y]
                        print(f"-creating point: {pos}  ", end="\r")
                        regrData["roll"].append(pos[0])
                        regrData["pitch"].append(pos[1])
                        regrData["area"].append(int(regr.predict([pos])))

            print("scatter plotting data sets:")
            resultGraph1 = plt.scatter(regrData["roll"], regrData["pitch"], c=regrData["area"], s=1.5, cmap="YlGn")
            resultGraph0 = plt.scatter(allDataSets[chosen_pf][z_pick][0], allDataSets[chosen_pf][z_pick][1], c=allDataSets[chosen_pf][z_pick][2], s=2, cmap="RdPu")

            plt.colorbar(resultGraph0)
            plt.colorbar(resultGraph1)

            plt.title(f"z:{z_pick}")
            fileName = f"objDist_angleArea_media/{typeComments[chosen_pf]}_n{chosen_pf}_z:{z_pick}_"

        elif plotMethod==1:
            plt_init(printText=False, mode=1)
            numPoints = 0
            for key, val in allDataSets[chosen_pf].items():
                resultGraph = ax.scatter(val[0], val[1], len(val[0])*[key], c=val[2], cmap="magma",s=1)
                numPoints+=len(val[0])

            plt.title(f"complete plot: {numPoints} points")
            fileName = f"objDist_angleArea_media/{typeComments[chosen_pf]}_n{chosen_pf}_"+str(orient["azim"])+":"+str(orient["elev"])+"_"
            plt.colorbar(resultGraph)

        for i in range(10000):
            if not os.path.isfile(fileName+str(i)+".png"):
                plt.savefig(fileName+str(i)+".png", dpi=300)
                break
        plt.show()


def main():
    while True:
        os.system("clear")
        print("Options:")
        print("0. Track and create new data sets for profile 1")
        print("1. Load and save average of all dataSets for profile 1")
        print("2. Display dataSets from profiles File")

        inp = input("input: ")
        if inp=="exit": break
        elif inp=="0": opt0()
        elif inp=="1": opt1()
        elif inp=="2": opt2()

if __name__=="__main__":
    main()


