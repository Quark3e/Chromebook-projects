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
        self.graphRanges = {"x": (-200, 200), "y":(-200, 200), "z":(0, 400)}
        self.stream = self.data_stream()
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(projection="3d")
        self.ani = animation.FuncAnimation(self.fig, self.update,interval=1,init_func=self.setup_plot,blit=False)
    def setup_plot(self):
        p0, p1 = next(self.stream)
        self.scat0 = self.ax.scatter([p0[0]],[p0[1]],[p0[2]], s=20, cmap="jet", edgecolor="k", label="correct")
        self.scat1 = self.ax.scatter([p1[0]],[p1[1]],[p1[2]], s=20, cmap="magma", edgecolor="k", label="svoled")
        self.plot0, = self.ax.plot([p0[0], p1[0]],[p0[1], p1[1]],[p0[2], p1[2]], linestyle="dotted")
        self.ax.axis("equal")
        self.ax.grid()
        self.ax.set(xlim3d=(-200, 200), xlabel="X")
        self.ax.set(ylim3d=(-200, 200), ylabel="Y")
        self.ax.set(zlim3d=(0, 400), zlabel="Z")
        self.ax.view_init(elev=30, azim=60)
        self.ax.legend()
        return self.scat0, self.scat1, self.plot0
    def data_stream(self):
        while True:
            p0 = [
                self.radius*math.sin(toRadians(self.degrees)),
                self.radius*math.cos(toRadians(self.degrees)),
                200
            ]
            p1 = [
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