#!/usr/bin/env python3

#/*
# A script to "attempt" fixing every servo motors
#  error rate by plotting the values on a graph.
# */


import numpy as np
import time
import os
import matplotlib.pyplot as plt
from datetime import datetime
from math import *

from IK_module import sendToServo, correctionSetup, toDegrees, toRadians, getAngles, custom_sendToServo


correctionFile = open("servoCorrections_temp.dat", "a")

sDefault = [90,55,180-0,90,180-80,90]

time.sleep(1)

sTemp = [ #is sent AFTER that angle is finished, so loop[q] and then sTemp[q], so sTemp[q] before loop[q]
    [90,55,45,90,100,90], #before q2, after q1
    [90,90,0,90,90,90],
    [180,90,135,0,90,90],
    [90,90,135,90,0,90],
    [90,90,135,90,90,0],
    [90,55,180,90,90,100,90]
]

x_q = 6*[19*[0]]
y_q = 6*[19*[0]]
y2_q = 6*[19*[0]]
#_q = 6*[18*[0]], 6 lists each with 18 elements

constants_q = 6*[{
    "default":1,
    "fixed":1
}]


def solve_linearConst(allValues, indexToIgnore=None, rawConstList=[]):
    '''
    Note: the elements of "allValues" must be same length
    Solves regression
    '''
    linear_constant = 1

    temp_constants = len(allValues[0])*[0]
    for n in range(1, len(allValues[0])): #starts at index 1 [1] because some parts use "previous" values
        tempList_n = []
        tempList_n_1 = []
        for e in range(len(allValues)-1):
            tempList_n.append(allValues[e][n])
            tempList_n_1.append(allValues[e][n-1])
        temp_constants[n] = (
            (allValues[len(allValues)][n] - allValues[len(allValues)][n-1]) / 
            (sqrt(sum([pow(val,2) for val in tempList_n]))-sqrt(sum([pow(val,2) for val in tempList_n_1])))
            )
    rawConstList = temp_constants.copy()
    linear_constant = sum(temp_constants) / len(temp_constants)

    return linear_constant


def configure_plots():
    '''
    Plot read value [y] from real life degree disk on given value [x] in degrees.
    '''
    global ax, fig
    fig = plt.figure(figsize=(8, 4.8))
    ax = [0,0]
    ax[0] = fig.add_subplot(1,2,1) #type: ignore
    ax[1] = fig.add_subplot(1,2,2) #type: ignore

    ax[0].set_xlim(0,180)
    ax[0].set_ylim(-90,90)
    ax[0].set_aspect('equal',adjustable='box')
    ax[0].set_xlabel("given value [degrees]")
    ax[0].set_ylabel("read error rate | read-given [degrees]")
    ax[0].axhline(y=0.5, color='r', linestyle='-')

    ax[0].grid()

    ax[1].set_xlim(0,180)
    # ax[1].set_ylim(0,90)
    ax[1].set_aspect('equal',adjustable='box')
    ax[1].set_xlabel("given value [degrees]")
    ax[1].set_ylabel("read error rate | read-given [degrees]")
    ax[1].grid()

    plt.title("Error rate graph")

tempVal1 = [
    8,
    21,
    32,
    44,
    58,
    71,
    81,
    91,
    105,
    115,
    128,
    141,
    158,
    170,
    180,
    195,
    205,
    205,
    205,
]


def main():
    global correctionFile
    configure_plots()
    breakVal = False
    for q in range(1):
        for x in range(0,19,1):
            # print(" sent angle:",x*10, end='')
            inpOpt = str(tempVal1[x])
            if inpOpt == "exit":
                breakVal = True
                break
            y_q[q][x] = 0 + round(float(inpOpt) - x*10)
            y2_q[q][x] = round(float(inpOpt))
            x_q[q][x] = x*10
        if breakVal: break
        
        plt.xticks(np.arange(min(x_q[0]),max(x_q[0])+1, 10), rotation=90)
        ax[0].plot(x_q[q],y_q[q],linestyle='solid',label="q"+str(q)) #type: ignore
        ax[1].plot(x_q[q],y2_q[q],linestyle='solid',label="q"+str(q)) #type: ignore
        ax[1].plot(x_q[q],x_q[q],linestyle='solid',label="q"+str(q)) #type: ignore



        # print(y_q)
        time.sleep(0.2)
        print("-----------------")

    fig.legend()
    plt.show()



    #Writing to .dat file
    currentDate = str(datetime.now()) + ";"
    toFile = currentDate
    tempDict_read = {}
    for q in range(6): tempDict_read.update({q:y_q[q]})
    toFile += str(tempDict_read) + "\n" #type: ignore
    toFile_readable = "{\n"
    for q in range(6): toFile_readable += "\t" + str(y_q) + "\n"
    toFile_readable += "}\n"
    # print(toFile_readable)
    toFile += toFile_readable
    correctionFile.write(toFile)


if __name__ == "__main__":
    main()
    correctionFile.close()
    
