#!/usr/bin/env python3

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import math

def toRadians(degrees):
    return (degrees*np.pi)/180
def toDegrees(radians):
    return (radians*180)/np.pi

fig = plt.figure()
ax = fig.add_subplot(projection='3d')

ax.set(xlim3d=(-150, 150), xlabel='X')
ax.set(ylim3d=(-150, 150), ylabel='Y')
ax.set(zlim3d=(0, 300), zlabel='Z')
ax.view_init(elev=30, azim=60)

PP = [0, 0, 150]
radius = 100
sudoSpeedAdj = 2


posPlot, = ax.plot([PP[0]], [PP[1]], [PP[2]], marker='o')


def updatePos(n):
    global PP
    # posPlot._offsets3d = ()
    angle = int(sudoSpeedAdj*n)
    PP[0] = radius*math.cos(toRadians(angle))
    PP[1] = radius*math.sin(toRadians(angle))
    PP[2] = PP[2]
    posPlot.set_data(
        np.array([PP[0]]),
        np.array([PP[1]])
        )
    posPlot.set_3d_properties(np.array([PP[2]]))
    return posPlot,

ani = FuncAnimation(fig, updatePos, int(360/sudoSpeedAdj), interval=20, blit=True)

plt.show()
