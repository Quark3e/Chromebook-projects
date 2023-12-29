#!/usr/bin/env python3

import math
import numpy as np

toRadians = lambda degrees: (degrees*np.pi)/180
toDegrees = lambda radians: (radians*180)/np.pi


class camTriangle(object):
    camPos = [[0, 0, 0], [0, 0, 0]]
    camRes = [[640, 480], [640, 480]]
    camFOV = [[60, 45], [60, 45]]
    """[degrees]"""
    camCoef = [[0, 0], [0, 0]]
    """angles/pixel"""

    l_delta = [None, None]
    """[x2 - x1, y2 -y1]"""
    l_hypotenuse = None

    ang_offset = [90, 90]
    """Ang of which the cam is rotated from its axis. [relative]"""
    ang_d = [None, None]

    ang_tri = [None, None]
    ang_p = -1
    read_pix = [[None, None], [None, None]]
    """input parameter: pixel value [left to right, top to bottom]"""
    ang_read = [-1, -1]
    """The changing variable"""

    l_tri = [None, None]

    solved_pos = [-1, -1, -1]

    def __init__(self, camPos):
        """Initialization of class

        Args:
            camPos (float) [[x,y], [x,y]]: list of relative position of both cameras
        """
        self.camPos = camPos
        self.l_delta = [camPos[1][0]-camPos[0][0], camPos[1][1]-camPos[0][1]]
        self.l_hypotenuse = math.sqrt(self.l_delta[0]**2+self.l_delta[1]**2)
        self.ang_d = [
            toDegrees(math.atan(self.l_delta[1]/self.l_delta[0])),
            toDegrees(math.atan(self.l_delta[0]/self.l_delta[1]))
        ]
        self.setCamCoef()
    def setCamCoef(self):
        self.camCoef[0][0] = self.camFOV[0][0]/self.camRes[0][0]
        self.camCoef[0][1] = self.camFOV[0][1]/self.camRes[0][1]
        self.camCoef[1][0] = self.camFOV[1][0]/self.camRes[1][0]
        self.camCoef[1][1] = self.camFOV[1][1]/self.camRes[1][1]
    def solveAngL(self):
        self.ang_tri = [
            self.ang_offset[0]-self.ang_d[0]-self.ang_read[0],
            self.ang_offset[1]-self.ang_d[1]-self.ang_read[1]
        ]
        self.ang_p = 180 - self.ang_tri[0] - self.ang_tri[1]
    def solvePos(self, rawPos, useAng=False):
        self.read_pix = [
            [rawPos[0]-self.camPos[0][0]*0.5, self.camPos[0][1]*0.5-rawPos[0]],
            [rawPos[1]-self.camPos[1][0]*0.5, self.camPos[1][1]*0.5-rawPos[1]]
        ]
        self.ang_read = [self.read_pix[0][0]*self.camCoef[0][0], self.read_pix[1][0]*self.camCoef[1][0]]
        self.solveAngL()
        # print(rawPos, self.ang_read, end="    ")
        self.l_tri = [
            (self.l_hypotenuse*math.sin(toRadians(self.ang_tri[0])))/math.sin(toRadians(self.ang_p)),
            (self.l_hypotenuse*math.sin(toRadians(self.ang_tri[1])))/math.sin(toRadians(self.ang_p))
        ]
        self.solved_pos = [
                math.cos(toRadians(self.ang_offset[0]+self.ang_read[0]))*self.l_tri[0]+self.camPos[0][0],
                math.sin(toRadians(self.ang_offset[0]+self.ang_read[0]))*self.l_tri[0]+self.camPos[0][1],
                None
        ]
        print(self.ang_tri, end="  ")
        self.solved_pos[2] = self.solved_pos[1]*math.tan(toRadians(self.read_pix[0][1]*self.camCoef[0][1]))+self.camPos[0][2]
        return self.solved_pos

if __name__=="__main__":
    import matplotlib.pyplot as plt
    import matplotlib.animation as animation
    import matplotlib.patches as mpatches


class AnimatedPlot(object):
    camPos = [[0, 0, 0], [30, 20, 0]]
    basePos = [15, 25, 0]
    testPos = [15, 15, 0]
    solvedPos = [0, 0, 0]
    def __init__(self):
        self.tri = camTriangle(self.camPos)
        self.tri.solvePos(self.testPos[:2])

        self.graphRange = {
            "frame": [
                [
                    min([self.camPos[0][0], self.camPos[1][0]])-10,
                    max([self.camPos[0][0], self.camPos[1][0]])+10
                ],
                [
                    min([self.camPos[0][1], self.camPos[1][1]])-10,
                    max([self.camPos[0][1], self.camPos[1][1]])+20,
                ]
            ]
        }

        self.stream = self.data_stream()
        self.fig = plt.figure(figsize=(9,4))
        self.ax = {
            "frame": 0
        }
        count=1
        for key in self.ax:
            self.ax[key] = self.fig.add_subplot(1, 2, count)
            self.ax[key].grid("equal")
            self.ax[key].title.set_text(key)
            self.ax[key].set_xlim(self.graphRange[key][0])
            self.ax[key].set_ylim(self.graphRange[key][1])
            self.ax[key].set_aspect("equal")
            count+=1

        self.ps_stuff = {}
        """dictionary to hold updating plots/scatter/patches e.t.c.
        {Key: Values}
            "triCornerAngle"    :   [ang_tri[0], ang_tri[1], ang_p] {[string, string, string]}
            "triSideLength"     :   [l_tri[0], l_tri[1], l_hypotenuse] {[float, float, float]}
            "triSideLengthText" :   [l_tri[0], l_tri[1], l_hypotenuse] {[float, float, float]}
            "Pp"                :   [given_point, solved_point] :   "contains all three points"
            "PpText"            :   [given_point, solved_point] :   "contains text for each point"
        """
        self.ps_stuff.update({"triCornerAngleText": 3*[0]}) #text
        self.ps_stuff.update({"triSideLength": 3*[0]}) #plotted lengths
        self.ps_stuff.update({"triSideLengthText": 3*[0]})
        self.ps_stuff.update({"Pp": 2*[0]}) #[given, solved]
        self.ps_stuff.update({"PpText": 2*[0]}) #[given, solved]

        self.ani = animation.FuncAnimation( \
            self.fig, self.update, interval=2, \
            init_func=self.setup_plot, blit=False \
        )
    def data_stream(self):
        self.streamAngle = 0 #degree
        self.radius = 10
        i=0
        self.testPos = [
            math.sin(toRadians(i))*self.radius+self.basePos[0],
            math.cos(toRadians(i))*self.radius+self.basePos[1],
            self.basePos[2]
        ]
        self.tri.solvePos(self.testPos)
        self.solvedPos = self.tri.solved_pos
        while True:
            for i in range(360):
                self.streamAngle = i
                self.testPos = [
                    math.sin(toRadians(i))*self.radius+self.basePos[0],
                    math.cos(toRadians(i))*self.radius+self.basePos[1],
                    self.basePos[2]
                ]
                self.tri.solvePos([
                    toDegrees(math.atan(self.testPos[0]/self.testPos[1]))/self.tri.camCoef[0][0],
                    toDegrees(math.atan(self.testPos[1]/self.testPos[0]))/self.tri.camCoef[1][0]
                ])
                self.solvedPos = self.tri.solved_pos
                yield i
    def setup_plot(self):
        next(self.stream)

        self.ps_stuff["Pp"][0] = self.ax["frame"].scatter(
            [self.tri.camPos[0][0],self.tri.camPos[1][0],self.solvedPos[0]],
            [self.tri.camPos[0][1],self.tri.camPos[1][1],self.solvedPos[1]],
            edgecolor="k", label="given"
        )
        self.ps_stuff["Pp"][1] = self.ax["frame"].scatter(
            [self.tri.camPos[0][0],self.tri.camPos[1][0],self.testPos[0]],
            [self.tri.camPos[0][1],self.tri.camPos[1][1],self.testPos[1]],
            edgecolor="k", label="solved"
        )
        self.ps_stuff["PpText"][0] = self.ax["frame"].text(self.testPos[0],
                                                           self.testPos[1]+2,
                                                           f"[{round(self.testPos[0],1)},{round(self.testPos[1],1)}]")
        self.ps_stuff["PpText"][1] = self.ax["frame"].text(self.solvedPos[0],
                                                           self.solvedPos[1]+2,
                                                           f"[{round(self.solvedPos[0],1)},{round(self.solvedPos[1],1)}]")
        self.ps_stuff["triCornerAngleText"] = [
            self.ax["frame"].text(self.tri.camPos[0][0],
                                  self.tri.camPos[0][1],
                                  f"{round(self.tri.ang_tri[0],2)}"),
            self.ax["frame"].text(self.tri.camPos[1][0],
                                  self.tri.camPos[1][1],
                                  f"{round(self.tri.ang_tri[1],2)}"),
            self.ax["frame"].text(self.solvedPos[0],
                                  self.solvedPos[1],
                                  f"{round(self.tri.ang_p,2)}"),
        ]
        self.ps_stuff["triSideLength"] = 3*[0]

        self.ps_stuff["triSideLength"][0], = self.ax["frame"].plot([self.tri.camPos[0][0],self.solvedPos[0]], [self.tri.camPos[0][1],self.solvedPos[1]],
                                  label="l_tri[0]",linewidth=2,alpha=0.75)
        self.ps_stuff["triSideLength"][1], = self.ax["frame"].plot([self.solvedPos[0],self.tri.camPos[1][0]], [self.solvedPos[1],self.tri.camPos[1][1]], 
                                  label="l_tri[1]",linewidth=2,alpha=0.75)
        self.ps_stuff["triSideLength"][2], = self.ax["frame"].plot([self.tri.camPos[1][0],self.tri.camPos[0][0]], [self.tri.camPos[1][1],self.tri.camPos[0][1]],
                                  label="l_hyp.",linewidth=2,alpha=0.75)
        
        self.ps_stuff["triSideLengthText"] = [
            self.ax["frame"].text((self.tri.camPos[0][0]+self.solvedPos[0])/2+2,
                                  (self.tri.camPos[0][1]+self.solvedPos[1])/2+2,
                                  f"{round(self.tri.l_tri[0],2)}"),
            self.ax["frame"].text((self.solvedPos[0]+self.tri.camPos[1][0])/2+2,
                                  (self.solvedPos[1]+self.tri.camPos[1][1])/2+2,
                                  f"{round(self.tri.l_tri[1],2)}"),
            self.ax["frame"].text((self.tri.camPos[1][0]+self.tri.camPos[0][0])/2+2,  #x
                                  (self.tri.camPos[1][1]+self.tri.camPos[0][1])/2+2,  #y
                                  f"{round(self.tri.l_hypotenuse,2)}")                #len number
        ]

        retur=[]
        for key,val in self.ps_stuff.items():
            for el in val: retur.append(el)
        return retur
    def ps_updateText(self):
        self.ps_stuff["PpText"][0].set_text(f"[{round(self.solvedPos[0],1)},{round(self.solvedPos[1],1)}]")
        self.ps_stuff["PpText"][0].set_x(self.solvedPos[0])
        self.ps_stuff["PpText"][0].set_y(self.solvedPos[1]+2)
        self.ps_stuff["PpText"][1].set_text(f"[{round(self.testPos[0],1)},{round(self.testPos[1],1)}]")
        self.ps_stuff["PpText"][1].set_x(self.testPos[0])
        self.ps_stuff["PpText"][1].set_y(self.testPos[1]+2)
        self.ps_stuff["triCornerAngleText"][0].set_text(f"{round(self.tri.ang_tri[0],2)}")
        self.ps_stuff["triCornerAngleText"][0].set_x(self.tri.camPos[0][0])
        self.ps_stuff["triCornerAngleText"][0].set_y(self.tri.camPos[0][1])
        self.ps_stuff["triCornerAngleText"][1].set_text(f"{round(self.tri.ang_tri[1],2)}")
        self.ps_stuff["triCornerAngleText"][1].set_x(self.tri.camPos[1][0])
        self.ps_stuff["triCornerAngleText"][1].set_y(self.tri.camPos[1][1])
        self.ps_stuff["triCornerAngleText"][2].set_text(f"{round(self.tri.ang_p,2)}")
        self.ps_stuff["triCornerAngleText"][2].set_x(self.solvedPos[0][0])
        self.ps_stuff["triCornerAngleText"][2].set_y(self.solvedPos[0][1])
        self.ps_stuff["triSideLengthText"][0].set_text(f"{round(self.tri.l_tri[0],2)}")
        self.ps_stuff["triSideLengthText"][0].set_x((self.tri.camPos[0][0]+self.solvedPos[0])/2+2)
        self.ps_stuff["triSideLengthText"][0].set_y((self.tri.camPos[0][1]+self.solvedPos[1])/2+2)
        self.ps_stuff["triSideLengthText"][1].set_text(f"{round(self.tri.l_tri[1],2)}")
        self.ps_stuff["triSideLengthText"][1].set_x((self.solvedPos[0]+self.tri.camPos[1][0])/2+2)
        self.ps_stuff["triSideLengthText"][1].set_y((self.solvedPos[1]+self.tri.camPos[1][1])/2+2)
        self.ps_stuff["triSideLengthText"][2].set_text(f"{round(self.tri.l_hypotenuse,2)}")
        self.ps_stuff["triSideLengthText"][2].set_x((self.tri.camPos[1][0]+self.tri.camPos[0][0])/2+2)
        self.ps_stuff["triSideLengthText"][2].set_y((self.tri.camPos[1][1]+self.tri.camPos[0][1])/2+2)
    def update(self, i):
        next(self.stream)
        print(self.solvedPos)
        self.ps_stuff["Pp"][0].set_offsets(
            [[self.tri.camPos[0][0],self.tri.camPos[0][1]],[self.tri.camPos[1][0],self.tri.camPos[1][1]],[self.solvedPos[0],self.solvedPos[1]]])
        self.ps_stuff["Pp"][1].set_offsets(
            [[self.tri.camPos[0][0],self.tri.camPos[0][1]],[self.tri.camPos[1][0],self.tri.camPos[1][1]],[self.testPos[0],self.testPos[1]]])
        self.ps_stuff["triSideLength"][0].set_data(
            [self.tri.camPos[0][0],self.solvedPos[0]], [self.tri.camPos[0][1],self.solvedPos[1]])
        self.ps_stuff["triSideLength"][1].set_data(
            [self.solvedPos[0],self.tri.camPos[1][0]], [self.solvedPos[1],self.tri.camPos[1][1]])
        self.ps_stuff["triSideLength"][2].set_data(
            [self.tri.camPos[1][0],self.tri.camPos[0][0]], [self.tri.camPos[1][1],self.tri.camPos[0][1]])
        
        retur=[]
        for key,val in self.ps_stuff.items():
            for el in val: retur.append(el)
        return retur


if __name__=="__main__":
    a = AnimatedPlot()
    plt.show()