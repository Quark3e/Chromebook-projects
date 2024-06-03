#!/usr/bin/env python3

import sys
import os.path
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as mpatches
import math
import time
from random import randrange

absPath = os.path.realpath(__file__)[:-len("towerTriangulate")]
sys.path.append(absPath[:absPath.find("science")])

import science.math.analytic_geometry.triangulation as trig
from teststuff.python.matplotlib.basic.nonFilled_arc import drawArc

class cellTower(object):
    name = None
    pos = None
    radiusToObj = None
    def __init__(
            self,
            pos: list,
            name: str,
        ):
        self.name = name
        self.pos = pos


class AnimatedPlot(object):

    c = 299_792_458 #speed of light in meters/second
    c = 299_702_547 #speed of light in air

    towers = 3*[cellTower]
    towerObjDist = 3*[None]
    testObjPos = 2*[None]
    solvedObjPos = 2*[None]

    circRadAnimTimer = 3*[None]
    animIdx = 0
    def __init__(self):

        self.graphRange = {
            "frame": [
                [0, 2000],
                [0, 2000]
            ]
        }
        self.stream = self.data_stream()
        
        self.fig = plt.figure(figsize=(9, 6))
        self.ax = {
            "frame": plt.Axes
        }

        count=1
        for key in self.ax:
            self.ax[key] = self.fig.add_subplot(1, 1, count)
            self.ax[key].grid("equal")
            self.ax[key].set_title(key)
            self.ax[key].set_xlim(self.graphRange[key][0])
            self.ax[key].set_ylim(self.graphRange[key][1])
            self.ax[key].set_aspect("equal")
            self.ax[key].grid("equal")
            count+=1
        
        self.ps_stuff = {
            "towerRadius": 3*[0], #towerRadius circle
            "towerRadiusText": 3*[0], #towerRadius text
        }

        self.ani = animation.FuncAnimation(
            self.fig, self.update, interval=1, frames=300, 
            init_func=self.setup_plot, blit=False, repeat=True, repeat_delay=0
        )
    def data_stream(self):
        pass
    def setup_plot(self):
        print("setup called")
        for key in self.ax:
            self.ax[key].clear()
            self.ax[key].grid("equal")
            self.ax[key].set_title(key)
            self.ax[key].set_xlim(self.graphRange[key][0])
            self.ax[key].set_ylim(self.graphRange[key][1])
            self.ax[key].set_aspect("equal")
            self.ax[key].grid("equal")


        self.towers = [
            cellTower([randrange(2000),randrange(2000)], "A"),
            cellTower([randrange(2000),randrange(2000)], "B"),
            cellTower([randrange(2000),randrange(2000)], "C")
        ]
        self.testObjPos = [randrange(500, 1500),randrange(500,1500)]
        self.towerObjDist = [trig.get_2dDist(tower.pos, self.testObjPos) for tower in self.towers]
        self.solvedObjPos = trig.solvePos([tower.pos for tower in self.towers], self.towerObjDist)
        print(f" -corr. pos:{[round(i) for i in self.testObjPos]} | solv. pos:{[round(i) for i in self.solvedObjPos]}")

        self.circRadAnimTimer = [round(300*(i/sum(self.towerObjDist))) for i in self.towerObjDist]

        for tower in self.towers:
            self.ax["frame"].scatter([tower.pos[0]],[tower.pos[1]], s=3, color="b")
            self.ax["frame"].text(tower.pos[0], tower.pos[1], tower.name)

        self.ax["frame"].plot([tower.pos[0] for tower in self.towers]+[self.towers[0].pos[0]],[tower.pos[1] for tower in self.towers]+[self.towers[0].pos[1]],alpha=0.5,linestyle="dashed",color="gray")

        self.ax["frame"].scatter([self.testObjPos[0]],[self.testObjPos[1]],s=5,marker="o", color="g")
        self.ax["frame"].text(self.testObjPos[0],self.testObjPos[1]+30, "corr. position")
        self.ax["frame"].scatter([self.solvedObjPos[0]],[self.solvedObjPos[1]],s=5,marker="o", color="r")
        self.ax["frame"].text(self.solvedObjPos[0],self.solvedObjPos[1]-30, "solv. position")

        self.ps_stuff["towerRadius"] = [
            mpatches.Circle(tuple(self.towers[0].pos),1,fill=False),
            mpatches.Circle(tuple(self.towers[1].pos),1,fill=False),
            mpatches.Circle(tuple(self.towers[2].pos),1,fill=False),
        ]
        for c in self.ps_stuff["towerRadius"]:
            self.ax["frame"].add_artist(c)

    def update(self, i):
        self.animIdx = i
        print(f"{self.circRadAnimTimer} | {i:>3}", end="\r")
        if i<self.circRadAnimTimer[0]:
            self.ps_stuff["towerRadius"][0].set(radius=round((i/self.circRadAnimTimer[0])*self.towerObjDist[0]))
        elif i<sum(self.circRadAnimTimer[:2]):
            self.ps_stuff["towerRadius"][1].set(radius=round(((i-self.circRadAnimTimer[0])/self.circRadAnimTimer[1])*self.towerObjDist[1]))
        elif i<sum(self.circRadAnimTimer):
            self.ps_stuff["towerRadius"][2].set(radius=round(((i-sum(self.circRadAnimTimer[:2]))/self.circRadAnimTimer[2])*self.towerObjDist[2]))


if __name__=="__main__":
    a = AnimatedPlot()
    
    plt.show()
