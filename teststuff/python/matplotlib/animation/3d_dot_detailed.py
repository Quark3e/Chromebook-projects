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
        self.fig = plt.figure(figsize=(15,7))
        self.ax = {
            "3d": 0, #3d view
            "xz": 0, #front view
            "yz": 0, #side view
            "xy": 0  #top view
        }
        count = 0
        for key in self.ax:
            count+=1
            if key=="3d": self.ax[key] = self.fig.add_subplot(2,2,count,projection="3d")
            else: self.ax[key] = self.fig.add_subplot(2,2,count)
            self.ax[key].grid()
            self.ax[key].axis("equal")
            self.ax[key].title.set_text(key)
        self.ani = animation.FuncAnimation(self.fig, self.update, interval=1, init_func=self.setup_plot, blit=True)
    def setup_plot(self):
        p0, p1 = next(self.stream)
        tempC = np.random.random((1, 2)).T
        self.ps_Stuff = {}
        for key in self.ax: self.ps_Stuff.update({key: 3*[0]})

        # self.ps_Stuff["3d"][0] = self.ax["3d"].scatter([p0["x"]],[p0["y"]],[p0["z"]], edgecolor="k", label="dot 0")
        # self.ps_Stuff["3d"][1] = self.ax["3d"].scatter([p1["x"]],[p1["y"]],[p1["z"]], edgecolor="k", label="dot 1")
        # print("------", p0)
        self.ps_Stuff["3d"][0], = self.ax["3d"].plot([p0["x"], p0["x"]], [p0["y"], p0["y"]], [p0["z"], p0["z"]], linewidth=7)
        self.ps_Stuff["3d"][1], = self.ax["3d"].plot([p1["x"], p1["x"]], [p1["y"], p1["y"]], [p1["z"], p1["z"]], linewidth=7)
        self.ps_Stuff["3d"][2], = self.ax["3d"].plot([p0["x"], p1["x"]], [p0["y"], p1["y"]], [p0["z"], p1["z"]], linestyle="dotted")

        for key in self.ax:
            if key=="3d": continue
            self.ps_Stuff[key][0] = self.ax[key].scatter([p0[key[:1]]],[p0[key[1:2]]], edgecolor="k", label="dot 0")
            self.ps_Stuff[key][1] = self.ax[key].scatter([p1[key[:1]]],[p1[key[1:2]]], edgecolor="k", label="dot 1")
            self.ps_Stuff[key][2], = self.ax[key].plot([p0[key[:1]],p1[key[:1]]],[p0[key[1:2]],p1[key[1:2]]], linestyle="dotted")

        self.ax["3d"].set(xlim3d=self.graphRanges["x"], xlabel="X")
        self.ax["3d"].set(ylim3d=self.graphRanges["y"], ylabel="Y")
        self.ax["3d"].set(zlim3d=(self.graphRanges["z"]), zlabel="Z")
        self.ax["3d"].view_init(elev=30, azim=60)
        for key in self.ax:
            if key=="3d": continue
            self.ax[key].set_xlabel(key[:1].upper())
            self.ax[key].set_ylabel(key[1:2].upper())
            self.ax[key].set_xlim(list(self.graphRanges[key[:1]]))
            self.ax[key].set_ylim(list(self.graphRanges[key[1:2]]))
        return self.ps_Stuff["3d"][0], self.ps_Stuff["3d"][1], self.ps_Stuff["3d"][2] \
        , self.ps_Stuff["xz"][0], self.ps_Stuff["xz"][1], self.ps_Stuff["xz"][2] \
        , self.ps_Stuff["yz"][0], self.ps_Stuff["yz"][1], self.ps_Stuff["yz"][2] \
        , self.ps_Stuff["xy"][0], self.ps_Stuff["xy"][1], self.ps_Stuff["xy"][2]
    def data_stream(self):
        #global radius, degrees
        print("data_stream accessed")
        while True:
            p0 = {
                "x": self.radius*math.sin(toRadians(self.degrees)),
                "y": self.radius*math.cos(toRadians(self.degrees)),
                "z": 200
            }
            p1 = {
                "x": 0,
                "y": self.radius*math.sin(toRadians(self.degrees)),
                "z": self.radius*math.cos(toRadians(self.degrees))+200
            }
            self.degrees+=10
            if self.degrees>360: self.degrees=1
            yield p0, p1
    def update(self, i):
        p0, p1 = next(self.stream)
        # self.ps_Stuff["3d"][0]._offsets3d=([p0["x"]], [p0["y"]], [p0["z"]])
        # self.ps_Stuff["3d"][1]._offsets3d=([p1["x"]], [p1["y"]], [p1["z"]])
        self.ps_Stuff["3d"][0].set_data_3d([p0["x"], p0["x"]+1], [p0["y"], p0["y"]+1], [p0["z"], p0["z"]+1])
        self.ps_Stuff["3d"][1].set_data_3d([p1["x"], p1["x"]+1], [p1["y"], p1["y"]+1], [p1["z"], p1["z"]+1])
        self.ps_Stuff["3d"][2].set_data_3d([p0["x"], p1["x"]], [p0["y"], p1["y"]], [p0["z"], p1["z"]])
        for key in self.ax:
            if key=="3d": continue
            self.ps_Stuff[key][0].set_offsets([p0[key[:1]], p0[key[1:2]]])
            self.ps_Stuff[key][1].set_offsets([p1[key[:1]], p1[key[1:2]]])
            self.ps_Stuff[key][2].set_data([[p0[key[:1]],p1[key[:1]]],[p0[key[1:2]],p1[key[1:2]]]])
        return self.ps_Stuff["3d"][0], self.ps_Stuff["3d"][1], self.ps_Stuff["3d"][2] \
        , self.ps_Stuff["xz"][0], self.ps_Stuff["xz"][1], self.ps_Stuff["xz"][2] \
        , self.ps_Stuff["yz"][0], self.ps_Stuff["yz"][1], self.ps_Stuff["yz"][2] \
        , self.ps_Stuff["xy"][0], self.ps_Stuff["xy"][1], self.ps_Stuff["xy"][2]


if __name__=="__main__":
    a = AnimatedScatter()
    plt.show()
