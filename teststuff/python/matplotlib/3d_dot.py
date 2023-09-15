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
        p0, p1 = next(self.stream)
        tempC = np.random.random((1, 2)).T
        self.scat0 = self.ax.scatter([p0[0]],[p0[1]],[p0[2]], s=50, cmap="jet", edgecolor="k", label="dot 0")
        self.scat1 = self.ax.scatter([p1[0]],[p1[1]],[p1[2]], s=50, cmap="magma", edgecolor="k", label="dot 1")
        self.plot0, = self.ax.plot([p0[0], p1[0]],[p0[1], p1[1]],[p0[2], p1[2]])
        self.ax.axis("equal")
        self.ax.set(xlim3d=(-200, 200), xlabel="X")
        self.ax.set(ylim3d=(-200, 200), ylabel="Y")
        self.ax.set(zlim3d=(0, 400), zlabel="Z")
        self.ax.view_init(elev=30, azim=60)
        return self.scat0, self.scat1, self.plot0
    def data_stream(self):
        #global radius, degrees
        print("data_stream accessed")
        p0 = 3*[
            self.radius*math.sin(toRadians(self.degrees)),
            self.radius*math.cos(toRadians(self.degrees)),
            200
        ]
        p1 = 3*[
            0,
            self.radius*math.sin(toRadians(self.degrees)),
            self.radius*math.cos(toRadians(self.degrees))+200
        ]
        s, c = np.random.random((1, 2)).T
        while True:
            p0 = 3*[
                self.radius*math.sin(toRadians(self.degrees)),
                self.radius*math.cos(toRadians(self.degrees)),
                200
            ]
            p1 = 3*[
                0,
                self.radius*math.sin(toRadians(self.degrees)),
                self.radius*math.cos(toRadians(self.degrees))+200
            ]
            self.degrees+=10
            if self.degrees>360: self.degrees=1
            #print(f"deg:{degrees} :{math.sin(toRadians(degrees))} x:{x} y{y}")
            yield p0, p1
    def update(self, i):
        p0, p1 = next(self.stream)
        self.scat0._offsets3d=([p0[0]], [p0[1]], [p0[2]])
        self.scat1._offsets3d=([p1[0]], [p1[1]], [p1[2]])
        # print([p0[0], p1[0]],[p0[1], p1[1]], [p0[2], p1[2]])
        # self.plot0.set_data([p0[0], p1[0]],[p0[1], p1[1]])
        self.plot0.set_data_3d([p0[0], p1[0]],[p0[1], p1[1]], [p0[2], p1[2]])
        return self.scat0, self.scat1, self.plot0


if __name__=="__main__":
    a = AnimatedScatter()
    plt.show()
