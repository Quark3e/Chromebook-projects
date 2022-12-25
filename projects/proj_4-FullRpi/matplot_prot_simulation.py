#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import math
import sys
import os

from IK_module import toDegrees,toRadians,getAngles,getDistance,getSubframe
from IK_module import link


PP = 3*[0.0]
orient = 3*[0.0]
q = 6*[0.0]

read_PP = PP
P = 6*PP

def configure_plots():
    '''
    Plot frame X0Y0Z0 and sub-frame X1Y1Z1
    '''
    global ax, fig
    
    fig = plt.figure(figsize=plt.figaspect(0.5)) #type: ignore
    ax = [0,0]
    ax[0] = fig.add_subplot(1,2,1,projection='3d') #type: ignore
    ax[1] = fig.add_subplot(1,2,2,projection='3d') #type: ignore
    
    # Make legend, set axes limits and labels
    fig.legend()
    ax[0].set_xlim(-250, 250)
    ax[0].set_ylim(-100, 400)
    ax[0].set_zlim(0, 400) #type: ignore
    ax[0].set_aspect('equal',adjustable='box')
    ax[0].set_xlabel('X0')
    ax[0].set_ylabel('Y0')
    ax[0].set_zlabel('Z0') #type: ignore
    # Customize the view angle so it's easier to see that the scatter points lie
    # on the plane y=0
    ax[0].view_init(elev=20., azim=145, roll=0) #type: ignore

    ax[1].set_xlim(-50, 50)
    ax[1].set_ylim(0,100)
    ax[1].set_zlim(-50, 50) #type: ignore
    ax[1].set_aspect('equal',adjustable='box')
    ax[1].set_xlabel('X1')
    ax[1].set_ylabel('Y1')
    ax[1].set_zlabel('Z1') #type: ignore
    ax[1].view_init(elev=20., azim=145, roll=0) #type: ignore


def plotAxis_sim(PP):
    global ax
    xAxis_values = [[],[],[]] #[[x],[y],[z]], the elements (lists) are what's given to ax[0].plot()
    yAxis_values = [[],[],[]]
    zAxis_values = [[],[],[]]
    xRange = [0-(link[1]+link[2]+link[3]+link[4]+link[5]), (link[1]+link[2]+link[3]+link[4]+link[5])] #[lower/negative_range, upper/positive_range]
    yRange = [0, (link[1]+link[2]+link[3]+link[4]+link[5])]
    zRange = [0, sum(link)]
    xRange = [-250, 250]
    yRange = [0, 400]
    zRange = [0, 400]

    posRanges = [xRange,yRange,zRange]
    axis_Values = [xAxis_values,yAxis_values,zAxis_values]

    temp_PP = PP

    for axis in range(3):
        temp_PP = PP
        for axisPos in range(posRanges[axis][0], posRanges[axis][1], 10):
            temp_PP[axis] = axisPos
            # print("\n",axis, temp_PP)
            # input()
            isReachable = True
            q, isReachable = getAngles(temp_PP,orient[0],orient[1],orient[2],'-', printText=False)
            # print(axis, isReachable)
            # isReachable = True
            if isReachable:
                P, read_PP, _ = FK_solver(q, printText = False)
                # for i in range(6):
                #     axis_Values[axis][0].append(P[i][0]) #type: ignore
                #     axis_Values[axis][1].append(P[i][1]) #type: ignore
                #     axis_Values[axis][2].append(P[i][2]) #type: ignore
                axis_Values[axis][0].append(read_PP[0])
                axis_Values[axis][1].append(read_PP[1])
                axis_Values[axis][2].append(read_PP[2])
                # print(axis_Values[axis])
                
    for axis in range(3):
        if axis == 0: plotColor='red'
        elif axis == 1: plotColor='green'
        else: plotColor='blue'
        ax[0].plot(axis_Values[axis][0],axis_Values[axis][1],linestyle='solid',zs=axis_Values[axis][2],zdir='z',color=plotColor) #type: ignore
                # ax[0].plot([P[4][0],PP[0]],[P[4][1],PP[1]],[P[4][2],PP[2]], 'bo', linestyle='dashed',color='red') #type: ignore



def FK_solver(q, printText = True):
    '''
    F(orward)K(inematics)_solver

    '''
    P = 6*[3*[0.0]]

    P[0] = [0, 0, 0] #type: ignore
    P[1] = [0, 0, link[0]] #type: ignore
    P[2] = [link[1]*math.cos(q[1])*math.sin(q[0]), link[1]*math.cos(q[1])*math.cos(q[0]), link[0]+link[1]*math.sin(q[1])]
    P[3] = [P[2][0]+link[2]*math.cos(q[1]+q[2])*math.sin(q[0]), P[2][1]+link[2]*math.cos(q[1]+q[2])*math.cos(q[0]), P[2][2]+link[2]*math.sin(q[1]+q[2])]
    P[4] = [P[2][0]+(link[2]+link[3])*math.cos(q[1]+q[2])*math.sin(q[0]), P[2][1]+(link[2]+link[3])*math.cos(q[1]+q[2])*math.cos(q[0]), P[2][2]+(link[2]+link[3])*math.sin(q[1]+q[2])]
    
    read_orient = [0,0,0]
    read_orient[1] = math.sin( ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) / (link[4]+link[5]) )+q[1]+q[2] #type: ignore
    print("read b1", toDegrees(math.sin( ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) / (link[4]+link[5]) )))
    if toDegrees(q[4]) == 90 or toDegrees(q[4]) == -90:
        if ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) == 0: orient[0] = toRadians(0)
        if ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) > 0: orient[0] = toRadians(90)
        if ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) < 0: orient[0] = toRadians(-90)
    else:
        read_orient[0] = q[0]+math.atan(((link[4]+link[5])*math.sin(q[4])*math.sin(q[3])) / ((link[4]+link[5])*math.cos(q[4]))) #type: ignore
    if printText: print(" a_read:",toDegrees(read_orient[0])," b_read:",toDegrees(read_orient[1]),sep='')

    P[5] = [ #type: ignore
        P[4][0]+(link[4])*math.sin(read_orient[0])*math.sin(read_orient[1]),
        P[4][1]+(link[4])*math.cos(read_orient[1])*math.cos(read_orient[0]),
        P[4][2]+(link[4])*math.sin(read_orient[1])
        ]
    read_PP = [
        P[4][0]+(link[4]+link[5])*math.sin(read_orient[0])*math.sin(read_orient[1]),
        P[4][1]+(link[4]+link[5])*math.cos(read_orient[1])*math.cos(read_orient[0]),
        P[4][2]+(link[4]+link[5])*math.sin(read_orient[1])
        ]
    return P, read_PP, read_orient


def main():
    global PP, orient, q
    while True:
        configure_plots() #NOTE: must call this first before any use of the variable 'ax'
        os.system('clear')
        opt = input(" enter end-effector position [x y z]: ").split()
        if opt[0] == "exit": break
        
        PP = [float(opt[0]),float(opt[1]),float(opt[2])]
        opt = input(" enter orientation of end-effector [a b Y]: ").split()
        orient = [toRadians(float(opt[0])),toRadians(float(opt[1])),toRadians(float(opt[2]))]
        q, _ = getAngles(PP,orient[0],orient[1],orient[2],'-',printText=True)
        
        subFrame = getSubframe(PP,orient[0],orient[1],'-')
        subFrame[0] = 0-subFrame[0]
        if subFrame[2]>=0: ax[1].set_zlim(0,100) #type: ignore
        elif subFrame[2]<0: ax[1].set_zlim(-100,0) #type: ignore
        
        P, read_PP, read_orient = FK_solver(q)

        x_values, y_values, z_values = [], [], []

        for i in range(6):
            x_values.append(P[i][0]) #type: ignore
            y_values.append(P[i][1]) #type: ignore
            z_values.append(P[i][2]) #type: ignore
        x_values.append(read_PP[0])
        y_values.append(read_PP[1])
        z_values.append(read_PP[2])
        print(" x:",x_values,"\n y:",y_values,"\n z:",z_values)
        
        
        ax[0].plot(x_values, y_values, 'bo', linestyle='solid', zs=z_values, zdir='z', label='Base frame', color='black') #type: ignore
        ax[0].plot([P[4][0],PP[0]],[P[4][1],PP[1]],[P[4][2],PP[2]], 'bo', linestyle='dashed',color='grey') #type: ignore

        # plotAxis_sim(PP)

        print(subFrame[0],subFrame[1],subFrame[2])
        ax[1].plot([0,subFrame[0]],[0,subFrame[1]], 'bo',zs=[0,subFrame[2]], linestyle='solid', zdir='z', label='sub-frame') #type: ignore
        ax[1].plot([0,subFrame[0],subFrame[0],subFrame[0]],[0,subFrame[1],subFrame[1],subFrame[1]],'bo',zs=[0,0,0,subFrame[2]], linestyle='dashed',color='red') #type: ignore
        plt.show()


if __name__ == "__main__":
    main()
