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
from math import * #type: ignore

# from IK_module import sendToServo, toDegrees, toRadians, getAngles
from IK_module import *

relativePath = "projects/proj_Hexclaw/hexclaw_files/servoCal_files/"

correctionFile = open(relativePath+"data/"+"servoCorrections_temp.dat", "a")


x_q = 6*[19*[0]]
y_q = 6*[19*[0]]
y2_q = 6*[19*[0]]
#_q = 6*[18*[0]], 6 lists each with 18 elements

constants_q = [{
    "default":1,
    "fixed":1
},{
    "default":1,
    "fixed":1
},{
    "default":1,
    "fixed":1
},{
    "default":1,
    "fixed":1
},{
    "default":1,
    "fixed":1
},{
    "default":1,
    "fixed":1
}]


def solve_linearConst(allValues, indexToIgnore, rawConstList=[]):
    """Solves linear regression: solves constant between points in linear set
    
    ## Parameters:
    - allValues: list of lists with the values: [list1(axis 1), list2(axis 2), list3(axis 3)]
        - Note: the elements of "allValues" must be same length
    - indexToIgnore: list of first axis elements to ignore and not include in the calculation
    - rawConstList: empty list to get raw data from
    """
    linear_constant = 1

    temp_constants = []
    for n in range(1, len(allValues[0])): #starts at index 1 [1] because some parts use "previous" values
        dontIgnore = True
        for index in indexToIgnore:
            if index/10==n: dontIgnore=False
        if dontIgnore:
            tempList_n = []
            tempList_n_1 = []
            for e in range(len(allValues)-1):
                tempList_n.append(allValues[e][n])
                tempList_n_1.append(allValues[e][n-1])
            temp_constants.append(
                (allValues[len(allValues)-1][n] - allValues[len(allValues)-1][n-1]) / 
                (sqrt(sum([pow(val,2) for val in tempList_n]))-sqrt(sum([pow(val,2) for val in tempList_n_1])))
                )
    for n in range(len(temp_constants)):
        rawConstList.append(temp_constants[n])
    linear_constant = sum(temp_constants) / len(temp_constants)

    return linear_constant


def configure_plots():
    '''
    Plot read value [y] from real life degree disk on given value [x] in degrees.
    '''
    global ax, fig
    fig = plt.figure(figsize=(11, 6))
    ax = [0,0]
    ax[0] = fig.add_subplot(1,2,1) #type: ignore
    ax[1] = fig.add_subplot(1,2,2) #type: ignore

    ax[0].set_xlim(0,180) #type: ignore
    ax[0].set_ylim(-90,90)
    ax[0].set_aspect('equal',adjustable='box')
    ax[0].set_xlabel("given value [degrees]")
    ax[0].set_ylabel("read error-rate [degrees]")
    ax[0].axhline(y=0, color='black', linestyle='-')

    ax[0].grid()

    ax[1].set_xlim(0,180)
    # ax[1].set_ylim(0,90)
    ax[1].set_aspect('equal',adjustable='box')
    ax[1].set_xlabel("given value [degrees]")
    ax[1].set_ylabel("read value [degrees]")
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
    global correctionFile, constants_q, fig
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
        
        constants_q[q]["default"] = solve_linearConst([x_q[q], y2_q[q]],indexToIgnore=[160,170,180]) #type: ignore
        constants_q[q]["fixed"] = 1 / constants_q[q]["default"] #type: ignore

        regression_graph = [[],[]]
        modified_graph = [[],[]]
        for x in x_q[q]:
            regression_graph[0].append(x)
            regression_graph[1].append(constants_q[q]["default"]*x+8)
            modified_graph[0].append(x)
            modified_graph[1].append(constants_q[q]["fixed"]*regression_graph[1][int(x/10)]-8)

        # for x in y2_q[q]:   


        plt.xticks(np.arange(min(x_q[0]),max(x_q[0])+1, 10), rotation=90)
        
        ax[0].plot(x_q[q],y_q[q],linestyle='solid',label="q[{:1}] error-value".format(q)) #type: ignore
        ax[1].plot(x_q[q],y2_q[q],linestyle='solid',label="q[{:1}] non modified".format(q)) #type: ignore
        ax[1].plot(x_q[q],x_q[q],linestyle='solid',label="q[{:1}] expected".format(q)) #type: ignore
        
        ax[1].plot(regression_graph[0],regression_graph[1],linestyle='dashed',color="r",label="q[{:1}] regression k:{:4.4f}".format(q,constants_q[q]["default"])) #type: ignore
        ax[1].plot(modified_graph[0],modified_graph[1],linestyle='dashed',color="g",label="q[{:1}] regression k:{:4.4f}".format(q, constants_q[q]["fixed"])) #type: ignore
        ax[1].axvline(x=160,color="grey", linestyle="-") #type: ignore

        # print(y_q)
        time.sleep(0.2)
        print("-----------------")

    fig.legend(loc=2)
    # plt.savefig(relativePath+"media/"+"q1_errorGraph.png")
    # plt.savefig(relativePath+"media/"+"q1_errorGraph.pdf")

    plt.show()


    # #Writing to .dat file
    # currentDate = str(datetime.now()) + ";"
    # toFile = currentDate
    # tempDict_read = {}
    # for q in range(6): tempDict_read.update({q:y_q[q]})
    # toFile += str(tempDict_read) + "\n" #type: ignore
    # toFile_readable = str(constants_q)+"{\n"
    # for q in range(6): toFile_readable += "\t" + str(y_q) + "\n"
    # toFile_readable += "}\n"
    # # print(toFile_readable)
    # toFile += toFile_readable
    # correctionFile.write(toFile)


if __name__ == "__main__":
    main()
    correctionFile.close()
    
