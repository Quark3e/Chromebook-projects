#!/usr/bin/env python3

#/*
# A script to "attempt" fixing every servo motors
#  error rate (from ADXL345 accelerometer tilt) by plotting the values on a graph.
# */

inpVar = None

servoToTest = [1, 2] #servo[] index's of the ones to test
testedServos = [] #index of servos that were actually tested
plotTitle = None

inpVar = input("enter servo indexes to test: ").split()
servoToTest = [int(n) for n in inpVar]
plotTitle = input("enter plot-title/filename [no space]: ")
useCorr = input("use q_corrections [y/n]?: ")

if useCorr=="y": useCorr = True
elif useCorr=="n": useCorr = False


import numpy as np
import time
import math
import sys
import os
import os.path
absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("plotCal")])
sys.path.append(absPath[:len(absPath)-len("teststuff/python/servo-motor-stuff/")]+"projects/")
sys.path.append("/home/berkhme/github_repo/Chromebook-projects/projects/")

import matplotlib.pyplot as plt
from datetime import datetime

import proj_Hexclaw.IK_module as IK #type: ignore

wifiOrient = IK.nodemcuOrient()
sCntrl = IK.HC_servoControl()

sCntrl.toServo([135,45,180,45,180,90],0,mode=0)
time.sleep(1)
sCntrl.toServo([90,115,135,90,115,90],1,mode=2)



correctionFile = open("corrections.dat", "a")

sDefault = [90,55,180-0,90,180-80,90]

#servo[0].angle = sDefault[0]
#servo[1].angle = sDefault[1]
#servo[2].angle = sDefault[2]
#servo[3].angle = sDefault[3]
#servo[4].angle = sDefault[4]
#servo[5].angle = sDefault[5]
#time.sleep(1)

print(f"testing servos: {servoToTest}")

optAngles = [90, 90, 45, 90, 90, 90]

givPolyFunc = 6*[None]
invPolyFunc = 6*[None]

runInitia = False

if runInitia:
    try:
        time.sleep(0.5)
        for n in range(6):
            servo[n].angle = optAngles[n]
        time.sleep(1)
        for s in servoToTest:
            print(f"- q:[{s}]")
            for a in range(181):
                servo[s].angle = a
                time.sleep(0.02)
            time.sleep(1)
            for a in range(180, -1, -1):
                servo[s].angle = a
                time.sleep(0.02)
            time.sleep(2)
            servo[s].angle = optAngles[s]
    except KeyboardInterrupt:
        inp = input("paused. Want to exit [y/n]?:")
        if inp=="y" or inp=="exit":
            print("exiting...")
            time.sleep(0.5)
            exit()
        elif inp=="n" or inp==" ":
            pass

sPrep = [ #is sent AFTER that angle is finished, so loop[q] and then sPrep[q], so sPrep[q] before loop[q]
    [0,90,45,90,90,90],
    [90,0,45,90,90,90],
    [90,135,0,90,90,90],
    [90,90,135,0,90,90],
    [90,90,135,90,0,90],
    [90,90,135,90,90,0]
]

x_q = 6*[19*[0]]
y_q = 6*[19*[0]]
#_q = 6*[18*[0]], 6 lists each with 18 elements

def configure_plots():
    '''
    Plot read value [y] from real life degree disk on given value [x] in degrees.
    '''
    global ax, fig
    fig, ax = plt.subplots()

    ax.set_aspect('equal',adjustable='box')
    ax.set_xlabel("given value [degrees]")
    ax.set_ylabel("read error rate | read-given [degrees]")
    plt.title(plotTitle)


def main():
    global correctionFile
    configure_plots()
    for q in servoToTest:
        print(f"Testing servo q[{q}]")
        for l in range(6):
            if l<q: tempUseCorr = True
            else: tempUseCorr = False
            servo[l].angle = servoSol(l, sPrep[q][l], tempUseCorr)
        # os.system('clear')
        inp = input(" calibrate degreeDisk (fixate disk 0 with current servo position)\n press options:\n-space and enter to continue\n-\"end\" to ignore rest and plot\n-\"exit\" to exit script\ninput: ")
        if inp=="exit":
            return
        elif inp=="end":
            break
        print()
        for x in range(0,19,1):
            servo[q].angle = servoSol(q, x*10, useCorr)
            print(" sent angle:", useCorr, ": ",servoSol(q, x*10, useCorr), end='', sep='')
            readAccelerometer()
            time.sleep(1)
            inpOpt = pitch
            if x<=8: y_q[q][x] = round(inpOpt)
            elif x>8: y_q[q][x] = 180-round(inpOpt)
            x_q[q][x] = x*10

            time.sleep(0.1)

        givenPoly = np.polyfit(x_q[q], y_q[q], 5)
        inversedPoly = np.polyfit(y_q[q], x_q[q], 5)
        invPolyFunc[q] = np.poly1d(inversedPoly)
        givPolyFunc[q] = np.poly1d(givenPoly)
        print("\n",str(givPolyFunc[q]),"\n", sep='')

        ax.plot(x_q[q], givPolyFunc[q](x_q[q]), label=f"q[{q}] fitPoly.", color="green")
        ax.plot(x_q[q], y_q[q], linestyle='solid',label=f"q[{q}] raw")

        print(f"{q}:",y_q[q])
        time.sleep(0.2)
        testedServos.append(q)
        print(f"finished testing servo q[{q}]")
        print("-----------------")
        input("")

    ax.plot(x_q[q], x_q[q], linestyle='dotted', label="given")	

    plt.grid()
    plt.legend()
    plt.savefig("plot_"+plotTitle, dpi=300)
    plt.show()


    currentDate = str(datetime.now())
    toFile = currentDate + "; testingSolution:" + str(useCorr) + "; "
    tempDict_read = {}
    for q in testedServos: tempDict_read.update({q:y_q[q]})
    toFile += str(tempDict_read) + "\n" #type: ignore
    print("toFile <<",toFile)

    toFile_readable = "{\n"
    for q in testedServos:
        toFile_readable += "\t" + str(y_q[q]) + "\n"
    toFile_readable += "}, "

    toFile_functions = "{\n"
    for q in testedServos:
        toFile_functions += f"read: q[{q}]:\n" + str(givPolyFunc[q]) + "\n"
        toFile_functions += f"inve: q[{q}]:\n" + str(invPolyFunc[q]) + "\n"
    toFile_functions += "};\n"

    print(toFile_readable)
    toFile += toFile_readable + toFile_functions
    correctionFile.write("\n"+toFile)


if __name__ == "__main__":
    main()
    correctionFile.close()
    
