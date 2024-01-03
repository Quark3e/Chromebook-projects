#!/usr/bin/env python3

#/*
# A script to "attempt" fixing every servo motors
#  error rate (from ADXL345 accelerometer tilt) by plotting the values on a graph.
# */

inpVar = None

servoToTest = [1, 2] #servo[] index's of the ones to test
testedServos = [] #index of servos that were actually tested
plotTitle = None

ifAuto = True

inp = input("enter whether to read angles automatically(nodemcu) or manually input [y/n]: ")
if inp == "exit": exit()
elif inp=="y": ifAuto = True
elif inp=="n": ifAuto = False

inpVar = input("enter servo indexes to test: ").split()
if inpVar[0] == "exit": exit()
servoToTest = [int(n) for n in inpVar]

plotTitle = input("enter plot-title/filename [no space]: ")
if plotTitle == "exit": exit()

useCorr = input("use q_corrections [y/n]?: ")
if useCorr == "exit": exit()

if useCorr=="y": useCorr = True
elif useCorr=="n": useCorr = False


import numpy as np
import time
import math
import sys
import os
import os.path
absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("plotCal.py")])
sys.path.append(absPath[:absPath.find("teststuff")])
import matplotlib.pyplot as plt
from datetime import datetime

import projects.proj_Hexclaw.IK_module as IK #type: ignore
# import teststuff.python.modules.nodemcu.main_h as nodCl

wifiOrient = IK.espOrient()
sCntrl = IK.HC_servoControl()

sCntrl.toServo([135,45,180,45,180,90],0,mode=0)
time.sleep(1)
sCntrl.toServo([90,115,135,90,115,90],1,mode=2)



correctionFile = open("corrections.dat", "a")

sDefault = [90,55,180-0,90,180-80,90]


print(f"testing servos: {servoToTest}")

optAngles = [90, 90, 45, 90, 90, 90]

givPolyFunc = 6*[None]
invPolyFunc = 6*[None]

runInitia = False

if runInitia:
    try:
        time.sleep(0.5)
        for n in range(6):
            sCntrl.servo[n].angle = optAngles[n]
        time.sleep(1)
        for s in servoToTest:
            print(f"- q:[{s}]")
            for a in range(181):
                sCntrl.servo[s].angle = a
                time.sleep(0.02)
            time.sleep(1)
            for a in range(180, -1, -1):
                sCntrl.servo[s].angle = a
                time.sleep(0.02)
            time.sleep(2)
            sCntrl.servo[s].angle = optAngles[s]
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
        tempRot=sPrep[q].copy()
        sCntrl.servoSol([i for i in range(6) if i<q], tempRot)
        sCntrl.toServo(tempRot, 0, useCorr=False)
        # os.system('clear')
        inp = input(" \
                    calibrate degreeDisk (fixate disk 0 with current servo position)\n \
                    press options:\n-\
                    space and enter to continue\n \
                    -\"end\" to ignore rest and plot\n \
                    -\"exit\" to exit script\n \
                    input: ")
        if inp=="exit":
            return
        elif inp=="end":
            break
        print()
        for x in range(0,19,1):
            sCntrl.servo[q].angle = x*10
            time.sleep(1)
            if ifAuto:
                wifiOrient.readAccelerometer()
                inpOpt = wifiOrient.pitch
            else:
                inpOpt = input("enter angle: ")
                if inpOpt == "exit": return
                inpOpt = int(inpOpt)
            print(f" - sent:{x} read:{round(inpOpt)}")
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
    
