#!/usr/bin/env python3

#/*
# A script to "attempt" fixing every servo motors
#  error rate by plotting the values on a graph.
# */


import numpy as np
import time
import os
import matplotlib.pyplot as plt
from random import randrange

from IK_module import sendToServo, toDegrees, toRadians, getAngles



sDefault = [90,45,180-135,90,180-0,90]

time.sleep(1)

x_q = 6*[19*[0]]
y_q = 6*[19*[0.0]]
#_q = 6*[18*[0]], 6 lists each with 18 elements

def configure_plots():
    '''
    Plot read value [y] from real life degree disk on given value [x] in degrees.
    '''
    global ax, fig
    fig = plt.figure()
    ax = fig.add_subplot()
    ax.set_xlim(0,180)
    ax.set_ylim(-90,90)
    # ax.set_aspect('equal',adjustable='box')
    ax.set_xlabel("given value [degrees]")
    ax.set_ylabel("read error rate | read-given [degrees]")
    plt.title("Error rate graph")
    ax.grid()


def main():
    configure_plots()
    for q in range(6):
        os.system('clear')
        print("Testing servo:",q)
        # time.sleep(1.5)
        # input(" calibrate degreeDisk (fixate disk 0 with current servo position)\n press enter to continue...")
        print()
        for x in range(0,19,1):
            print(" sent angle:",x*10)
            y_q[q][x] = randrange(-90,90)*0.5
            # y_q[q][x] = round(float(input(" read angle: "))) - x*10
            x_q[q][x] = x*10
        # time.sleep(1)
        ax.plot(x_q[q],y_q[q],linestyle='solid',label=str(q))
    ax.legend()
    
    plt.show()


if __name__ == "__main__":
    main()
    