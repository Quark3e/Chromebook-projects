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
        self.fig, self.ax = plt.subplots()
        self.ani = animation.FuncAnimation(self.fig, self.update, interval=5, init_func=self.setup_plot, blit=True)
        self.ax.grid()
    def setup_plot(self):
        x, y = next(self.stream)
        tempC = np.random.random((1, 2)).T
        self.scat = self.ax.scatter([x],[y], vmin=-150, vmax=1, cmap="jet", edgecolor="k")
        self.ax.axis([-150, 150, -150, 150])
        return self.scat,
    def data_stream(self):
        #global radius, degrees
        print("data_stream accessed")
        x, y = self.radius*math.sin(toRadians(self.degrees)), self.radius*math.cos(toRadians(self.degrees))
        s, c = np.random.random((1, 2)).T
        while True:
            x, y = self.radius*math.sin(toRadians(self.degrees)), self.radius*math.cos(toRadians(self.degrees))
            self.degrees+=2
            if self.degrees>360: self.degrees=1
            #print(f"deg:{degrees} :{math.sin(toRadians(degrees))} x:{x} y{y}")
            yield x, y
    def update(self, i):
        x, y = next(self.stream)
        #print(x, y, end='-------------\n')

        self.scat.set_offsets([x, y])
        return self.scat,


if __name__=="__main__":
    a = AnimatedScatter()
    plt.show()
