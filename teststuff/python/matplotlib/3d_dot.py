#!/usr/bin/env python3

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import math

def toRadians(degrees):
    return (degrees*np.pi)/180
def toDegrees(radians):
    return (radians*180)/np.pi


class AnimatedScatter(object):
    def __init__(self):
        self.radius = 75
        self.degrees = 0
        self.stream = self.data_stream()
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(projection="3d")
        self.ani = animation.FuncAnimation(self.fig, self.update, interval=1, init_func=self.setup_plot, blit=False)
        self.ax.grid()
    def setup_plot(self):
        x, y, z = next(self.stream)
        tempC = np.random.random((1, 2)).T
        self.scat = self.ax.scatter([x],[y],[z], cmap="jet", edgecolor="k", label="dot")
        #self.ax.axis([-150, 150, -150, 150])
        self.ax.set(xlim3d=(-150, 150), xlabel="X")
        self.ax.set(ylim3d=(-150, 150), ylabel="Y")
        self.ax.set(zlim3d=(0, 400), zlabel="Z")
        self.ax.view_init(elev=30, azim=60)
        return self.scat,
    def data_stream(self):
        #global radius, degrees
        print("data_stream accessed")
        x, y = self.radius*math.sin(toRadians(self.degrees)), self.radius*math.cos(toRadians(self.degrees))
        z = 200
        s, c = np.random.random((1, 2)).T
        while True:
            x, y = self.radius*math.sin(toRadians(self.degrees)), self.radius*math.cos(toRadians(self.degrees))
            self.degrees+=10
            if self.degrees>360: self.degrees=1
            #print(f"deg:{degrees} :{math.sin(toRadians(degrees))} x:{x} y{y}")
            yield x, y, z
    def update(self, i):
        x, y, z = next(self.stream)
        self.scat._offsets3d=([x], [y], [z])
        return self.scat,


if __name__=="__main__":
    a = AnimatedScatter()
    plt.show()
