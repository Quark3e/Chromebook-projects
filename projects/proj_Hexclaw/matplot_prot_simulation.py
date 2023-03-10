#!/usr/bin/env python3

import numpy as np #type:ignore
import matplotlib.pyplot as plt #type: ignore
from matplotlib.animation import FuncAnimation #type: ignore
import math
import sys
import os

from IK_module import *

# xRange = [0-(link[1]+link[2]+link[3]+link[4]+link[5]), (link[1]+link[2]+link[3]+link[4]+link[5])] #[lower/negative_range, upper/positive_range]
# yRange = [0, (link[1]+link[2]+link[3]+link[4]+link[5])]
# zRange = [0, sum(link)] #type: ignore
xRange = [-250, 250]
yRange = [150, 250]
zRange = [100, 300]

PP = 3*[0.0]
orient = 3*[0.0]
q = 6*[0.0]

read_PP = PP
P = 6*PP

animateStartBool = True
useAnimation = True
moveType = "axis_basic"

def animate(n):
    global ax, fig, moveType, PP, orient, q, thePlot, animateStartBool
    if moveType == "axis_basic":
        PP_copy = PP.copy()
        if n>30: PP_copy[0]=-10*(n-30)+150
        else: PP_copy[0]=n*10-150
        
        q = getAngles(PP_copy,orient[0],orient[1],orient[2],'-',printText=False,forShow=False, debug=mod_dict)

        subFrame = getAngles(PP_copy,orient[0],orient[1],orient[2],'-',getSubframe=True, printText=False) #getSubframe(PP,orient[0],orient[1],'-')
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
        if n>0:
            thePlot[0][0].set_data(x_values, y_values) #type: ignore
            thePlot[0][0].set_3d_properties(z_values) #type: ignore
            thePlot[0][1].set_data([P[4][0],PP_copy[0]], [P[4][1],PP_copy[1]]) #type: ignore
            thePlot[0][1].set_3d_properties([P[4][2],PP_copy[2]]) #type: ignore

            thePlot[1][0].set_data([0,-subFrame[0]],[0,subFrame[1]]) #type: ignore
            thePlot[1][0].set_3d_properties([0, subFrame[2]]) #type: ignore
            thePlot[1][1].set_data([0,-subFrame[0],-subFrame[0],-subFrame[0]], [0,subFrame[1],subFrame[1],subFrame[1]]) #type: ignore
            thePlot[1][1].set_3d_properties([0,0,0,subFrame[2]]) #type: ignore
        if animateStartBool:
            thePlot[0][0], = ax[0].plot(x_values, y_values, 'bo', linestyle='solid', zs=z_values, zdir='z', label='Base frame', color='black') #type: ignore
            thePlot[0][1], = ax[0].plot([P[4][0],PP_copy[0]],[P[4][1],PP_copy[1]],[P[4][2],PP_copy[2]], 'bo', linestyle='dashed',color='grey') #type: ignore
            plotAxis_sim(PP_copy)
            thePlot[1][0], = ax[1].plot([0,-subFrame[0]],[0,subFrame[1]], 'bo',zs=[0,subFrame[2]], linestyle='solid', zdir='z', label='sub-frame') #type: ignore
            thePlot[1][1], = ax[1].plot([0,-subFrame[0],-subFrame[0],-subFrame[0]],[0,subFrame[1],subFrame[1],subFrame[1]],'bo',zs=[0,0,0,subFrame[2]], linestyle='dashed',color='red') #type: ignore
            animateStartBool = False

def configure_plots(initiate=False):
    '''
    Plot frame X0Y0Z0 and sub-frame X1Y1Z1
    '''
    global ax, fig, thePlot
    
    if initiate:
        fig = plt.figure(figsize=plt.figaspect(0.5)) #type: ignore
        ax = [0,0]
        ax[0] = fig.add_subplot(1,2,1,projection='3d') #type: ignore
        ax[1] = fig.add_subplot(1,2,2,projection='3d') #type: ignore
        thePlot = [[0,0],[0,0]]
        #thePlot[0], = ax[0].plot([], [])
        #thePlot[1], = ax[1].plot([], [])
        thePlot[0][0], = ax[0].plot([], []) #type: ignore
        thePlot[0][1], = ax[0].plot([], []) #type: ignore
        thePlot[1][0], = ax[1].plot([], []) #type: ignore
        thePlot[1][1], = ax[1].plot([], []) #type: ignore

    # Make legend, set axes limits and labels
    fig.legend()
    ax[0].set_xlim(-250, 250) #type: ignore
    ax[0].set_ylim(-100, 400) #type: ignore
    ax[0].set_zlim(0, 400) #type: ignore
    ax[0].set_aspect('equal',adjustable='box') #type: ignore
    ax[0].set_xlabel('X0') #type: ignore
    ax[0].set_ylabel('Y0') #type: ignore
    ax[0].set_zlabel('Z0') #type: ignore
    # Customize the view angle so it's easier to see that the scatter points lie
    # on the plane y=0
    ax[0].view_init(elev=20., azim=135, roll=0) #type: ignore

    ax[1].set_xlim(-50, 50) #type: ignore
    ax[1].set_ylim(0,100) #type: ignore
    ax[1].set_zlim(-50, 50) #type: ignore
    ax[1].set_aspect('equal',adjustable='box') #type: ignore
    ax[1].set_xlabel('X1') #type: ignore
    ax[1].set_ylabel('Y1') #type: ignore
    ax[1].set_zlabel('Z1') #type: ignore
    ax[1].view_init(elev=20., azim=125, roll=0) #type: ignore

def plotAxis_sim(PP, printText=False):
    global ax
    xAxis_values = [[],[],[]] #[[x],[y],[z]], the elements (lists) are what's given to ax[0].plot()
    yAxis_values = [[],[],[]]
    zAxis_values = [[],[],[]]

    posRanges = [xRange,yRange,zRange]
    read_axis_Values = [xAxis_values.copy(),yAxis_values.copy(),zAxis_values.copy()]
    given_axis_Values = [[[],[],[]],[[],[],[]],[[],[],[]]]
    temp_PP = PP.copy()

    for axis in range(3):

        if printText: print(axis, PP, temp_PP, "-> ", end='')
        temp_PP = PP.copy()
        if printText: print(temp_PP)

        for axisPos in range(posRanges[axis][0], posRanges[axis][1], 10):
            temp_PP[axis] = axisPos
            isReachable = [True]
            # print("{:8}".format(round(axisPos)), "orient:{:18}".format(str([toDegrees(angle) for angle in orient])), end=' ')
            if axis==1 and printText: print(temp_PP,[round(toDegrees(angle)) for angle in orient])
            q = getAngles(temp_PP,orient[0],orient[1],orient[2],'-',
            printText=False,printErrors=False,forShow=False,
            positionIsReachable=isReachable, debug=mod_dict)
            if isReachable[0]:
                _, read_PP, _ = FK_solver(q, printText = False)           
                # print(' given:{:18} angles:{:30} read:{:18} orient:{:10}'.format(str([round(temp_PP) for temp_PP in temp_PP]),
                # str([round(toDegrees(q)) for q in q]),
                # str([round(read_PP) for read_PP in read_PP]),
                # str([round(toDegrees(orientation)) for orientation in orient])), end='')
                read_axis_Values[axis][0].append(read_PP[0])
                read_axis_Values[axis][1].append(read_PP[1])
                read_axis_Values[axis][2].append(read_PP[2])
                given_axis_Values[axis][0].append(temp_PP[0])
                given_axis_Values[axis][1].append(temp_PP[1])
                given_axis_Values[axis][2].append(temp_PP[2])
            if printText: print("----------")
        if axis == 0: plotColor_1='red'
        elif axis == 1: plotColor_1='green'
        else: plotColor_1='blue'
        ax[0].plot(read_axis_Values[axis][0],read_axis_Values[axis][1],linestyle='solid',zs=read_axis_Values[axis][2],zdir='z',color=plotColor_1) #type: ignore
        ax[0].plot(given_axis_Values[axis][0],given_axis_Values[axis][1],linestyle='dashed',zs=given_axis_Values[axis][2],zdir='z',color='grey') #type: ignore

def FK_solver(q, printText = False):
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
    read_orient[1] = math.asin( ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) / (link[4]+link[5]) )+q[1]+q[2] #type: ignore #isn't it asin?
    # print("read b1", toDegrees(math.asin( ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) / (link[4]+link[5]) ))) #I'm a goddamn moron
    if toDegrees(q[4]) == 90 or toDegrees(q[4]) == -90:
        if ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) == 0: read_orient[0] = toRadians(0) #type: ignore
        if ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) > 0: read_orient[0] = toRadians(90) #type: ignore
        if ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) < 0: read_orient[0] = toRadians(-90) #type: ignore
    else:
        read_orient[0] = q[0]+math.atan(((link[4]+link[5])*math.sin(q[4])*math.sin(q[3])) / ((link[4]+link[5])*math.cos(q[4]))) #type: ignore
    # read_orient[0] = 0 - read_orient[0]
    # read_orient[0] = read_orient[0] / math.cos(read_orient[1]) #type: ignore
    if printText: print(" a_read:",toDegrees(read_orient[0])," b_read:",toDegrees(read_orient[1]),sep='')

    # print("orient:",[toDegrees(a) for a in read_orient])

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
    global PP, orient, q, ax, fig, animateStartBool
    printText = False
    while True:
        os.system('clear')
        opt = input(" enter end-effector position [x y z]: ").split()
        if opt[0] == "exit": break
        
        PP = [float(opt[0]),float(opt[1]),float(opt[2])]
        opt = input(" enter orientation of end-effector [a b Y]: ").split()
        if opt[0] == "exit": break
        orient = [toRadians(float(opt[0])),toRadians(float(opt[1])),toRadians(float(opt[2]))]
        configure_plots(initiate=True)
        if useAnimation:
            animateStartBool = True
            anim = FuncAnimation(fig, animate, interval=0, frames=60, repeat=False) #type: ignore
            #fig.canvas.mpl_connect('button_press_event', onClick)
        elif not useAnimation:
            configure_plots()
            q = getAngles(PP,orient[0],orient[1],orient[2],'-',printText=True,forShow=False, debug=mod_dict)

            subFrame = getAngles(PP,orient[0],orient[1],orient[2],'-',getSubframe=True) #getSubframe(PP,orient[0],orient[1],'-')
            subFrame[0] = 0-subFrame[0]
            if subFrame[2]>=0: ax[1].set_zlim(0,100) #type: ignore
            elif subFrame[2]<0: ax[1].set_zlim(-100,0) #type: ignore
            
            P, read_PP, read_orient = FK_solver(q)


            print("\n{:20} {}".format("given orients:", [round(toDegrees(angle)) for angle in orient]))
            print("{:20} {}".format("calculated orients:", [round(toDegrees(angle)) for angle in read_orient]))

            print("\n{:20} {}".format("correct length:", link[4]+link[5]))
            print("{:20} {}".format("calculated length:", round(getDistance(read_PP,P[4]))),end='\n\n')

            x_values, y_values, z_values = [], [], []

            for i in range(6):
                x_values.append(P[i][0]) #type: ignore
                y_values.append(P[i][1]) #type: ignore
                z_values.append(P[i][2]) #type: ignore
            x_values.append(read_PP[0])
            y_values.append(read_PP[1])
            z_values.append(read_PP[2])
            if printText: print(" x:",x_values,"\n y:",y_values,"\n z:",z_values)
            
            ax[0].plot(x_values, y_values, 'bo', linestyle='solid', zs=z_values, zdir='z', label='Base frame', color='black') #type: ignore
            ax[0].plot([P[4][0],PP[0]],[P[4][1],PP[1]],[P[4][2],PP[2]], 'bo', linestyle='dashed',color='grey') #type: ignore

            # plotAxis_sim(PP)

            if printText: print(subFrame[0],subFrame[1],subFrame[2])
            ax[1].plot([0,-subFrame[0]],[0,subFrame[1]], 'bo',zs=[0,subFrame[2]], linestyle='solid', zdir='z', label='sub-frame') #type: ignore
            ax[1].plot([0,-subFrame[0],-subFrame[0],-subFrame[0]],[0,subFrame[1],subFrame[1],subFrame[1]],'bo',zs=[0,0,0,subFrame[2]], linestyle='dashed',color='red') #type: ignore
            
            
        plt.show()


if __name__ == "__main__":
    main()
