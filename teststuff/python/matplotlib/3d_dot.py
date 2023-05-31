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

ax.set(xlim3d=(-1.5, 1.5), xlabel='X')
ax.set(ylim3d=(-1.5, 1.5), ylabel='Y')
ax.set(zlim3d=(-1.5, 1.5), zlabel='Z')
ax.view_init(elev=90, azim=-0)

pointPos = [0, 0, 0]
radius = 1

posPlot, = ax.plot([pointPos[0]], [pointPos[1]], [pointPos[2]], marker='o')

sudoSpeedAdj = 2

def updatePos(n):
    # posPlot._offsets3d = ()
    angle = int(sudoSpeedAdj*n)
    posPlot.set_data(
        np.array([pointPos[0]+radius*math.cos(toRadians(angle))]),
        np.array([pointPos[1]+radius*math.sin(toRadians(angle))])
        )
    posPlot.set_3d_properties(np.array([pointPos[2]+radius*0]))
    return posPlot,
ani = FuncAnimation(fig, updatePos, int(360/sudoSpeedAdj), interval=2, blit=True)

plt.show()

