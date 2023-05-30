#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import math

fig = plt.figure()
ax = fig.add_subplot(projection='3d')

ax.set_xlim3d(-10, 10)
ax.set_ylim3d(-10, 10)
ax.set_zlim3d(-10, 10)

ax.set_xlabel("X axis")
ax.set_ylabel("Y axis")
ax.set_zlabel("Z axis")

ax.view_init(elev=90, azim=-0)

def toRadians(degree):
    return (degree*np.pi)/180

PP_base = [0, 0, 0]
PP_new = PP_base.copy()

for a in range(0, 360, 1):
    if a-10>=360: a = 0
    PP_new[0] = PP_base[0]+5*math.cos(toRadians(a))
    PP_new[1] = PP_base[1]+5*math.sin(toRadians(a))
    print(a, PP_new)
    ax.scatter([PP_new[0]], [PP_new[1]], [PP_new[2]], marker="o")
    plt.pause(0.000000001)

plt.show()