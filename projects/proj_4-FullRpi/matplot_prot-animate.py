
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import math
import sys
import os

from IK_module import toDegrees, toRadians, getAngles, getDistance, link


PP = 3*[0.0]
orient = 3*[0.0]
q = 6*[0.0]

P = 6*[PP]
read_PP = PP


def configure_plot():
    global ax, fig

    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')
    # Make legend, set axes limits and labels
    ax.legend()
    ax.set_xlim(-250, 250)
    ax.set_ylim(-100, 400)
    ax.set_zlim(0, 400) #type: ignore
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z') #type: ignore
    # Customize the view angle so it's easier to see that the scatter points lie
    # on the plane y=0
    ax.view_init(elev=20., azim=145, roll=0) #type: ignore


def FK_solver():
    '''
    F(orward)K(inematics)_solver

    '''
    global P, read_PP, read_orient
    P[0] = [0, 0, 0] #type: ignore
    P[1] = [0, 0, link[0]] #type: ignore
    P[2] = [link[1]*math.cos(q[1])*math.sin(q[0]), link[1]*math.cos(q[1])*math.cos(q[0]), link[0]+link[1]*math.sin(q[1])]
    P[3] = [P[2][0]+link[2]*math.cos(q[1]+q[2])*math.sin(q[0]), P[2][1]+link[2]*math.cos(q[1]+q[2])*math.cos(q[0]), P[2][2]+link[2]*math.sin(q[1]+q[2])]
    P[4] = [P[2][0]+(link[2]+link[3])*math.cos(q[1]+q[2])*math.sin(q[0]), P[2][1]+(link[2]+link[3])*math.cos(q[1]+q[2])*math.cos(q[0]), P[2][2]+(link[2]+link[3])*math.sin(q[1]+q[2])]
    
    read_orient = [0,0,0]
    read_orient[1] = math.sin(((link[4]+link[5])*math.sin(q[4])*math.cos(q[3]))/(link[4]+link[5]))+q[1]+q[2] #type: ignore
    if toDegrees(q[4]) == 90 or toDegrees(q[4]) == -90:
        if ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) == 0: orient[0] = toRadians(0)
        if ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) > 0: orient[0] = toRadians(90)
        if ((link[4]+link[5])*math.sin(q[4])*math.cos(q[3])) < 0: orient[0] = toRadians(-90)
    else:
        read_orient[0] = q[0]+math.atan(((link[4]+link[5])*math.sin(q[4])*math.sin(q[3])) / ((link[4]+link[5])*math.cos(q[4]))) #type: ignore
    print(toDegrees(read_orient[0]),toDegrees(read_orient[1]))

    P[5] = [
        P[4][0]+(link[4])*math.sin(read_orient[0])*math.sin(read_orient[1]),
        P[4][1]+(link[4])*math.cos(read_orient[1])*math.cos(read_orient[0]),
        P[4][2]+(link[4])*math.sin(read_orient[1])
        ]
    read_PP = [
        P[4][0]+(link[4]+link[5])*math.sin(read_orient[0])*math.sin(read_orient[1]),
        P[4][1]+(link[4]+link[5])*math.cos(read_orient[1])*math.cos(read_orient[0]),
        P[4][2]+(link[4]+link[5])*math.sin(read_orient[1])
        ]


def main():
    global PP, orient, q
    while True:
        configure_plot() #NOTE: must call this first before any use of the variable 'ax'
        os.system('clear')
        opt = input(" enter end-effector position [x y z]: ").split()
        if opt[0] == "exit": break
        PP = [float(opt[0]),float(opt[1]),float(opt[2])]
        opt = input(" enter orientation of end-effector [a b Y]: ").split()
        orient = [toRadians(float(opt[0])),toRadians(float(opt[1])),toRadians(float(opt[2]))]
        q = getAngles(PP,orient[0],orient[1],orient[2],'-')
        print([round(toDegrees(q)) for q in q])
        FK_solver()

        x_values, y_values, z_values = [], [], []
        for i in range(6):
            x_values.append(P[i][0])
            y_values.append(P[i][1])
            z_values.append(P[i][2])
        x_values.append(read_PP[0])
        y_values.append(read_PP[1])
        z_values.append(read_PP[2])
        print(x_values,y_values,z_values,sep="\n")
        ax.plot(x_values, y_values, 'bo', linestyle='solid', zs=z_values, zdir='z', label='curve in (x, y)')
        ax.plot([P[4][0],-PP[0]],[P[4][1],PP[1]],[P[4][2],PP[2]], 'bo', linestyle='dashed',color='red')

        plt.show()


if __name__ == "__main__":
    main()
