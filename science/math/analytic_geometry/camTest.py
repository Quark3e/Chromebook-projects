#!/usr/bin/env python3

import sys
import os.path
import cv2
import matplotlib.pyplot as plt
import matplotlib.animation as animation

absPath = os.path.realpath(__file__)[:-len("realTest.py")]
sys.path.append(absPath[:absPath.find("science")])

import teststuff.python.modules.IR_track_opencv as IR_track

from two_cam_coordinate import camTriangle, toRadians, toDegrees
from teststuff.python.matplotlib.basic.nonFilled_arc import drawArc


class AnimatedPlot(object):
    camPos = [[0, 0, 0], [30, 30, 0]]
    camAng_offset = [90, 180] #degrees
    streamAngle = 0 #degree

    basePos = [0, 30, 0]
    testPos = [15, 15, 0]
    solvedPos = [0, 0, 0]

    saveAnim = True
    def __init__(self):
        self.tri = camTriangle(self.camPos, self.camAng_offset)
        self.tri.solvePos(self.testPos[:2])

        self.IRcams = IR_track.IR_camTrack([0,2])

        self.graphRange = {
            "frame": [
                [
                    min([self.camPos[0][0], self.camPos[1][0]])-15,
                    max([self.camPos[0][0], self.camPos[1][0]])+15
                ],
                [
                    min([self.camPos[0][1], self.camPos[1][1]])-1,
                    max([self.camPos[0][1], self.camPos[1][1]])+30,
                ]
            ]
        }

        self.stream = self.data_stream()
        # self.fig = plt.figure(figsize=(9,4))
        self.fig = plt.figure(figsize=(9,6))
        self.ax = {
            "frame": plt.Axes
        }

        count=1
        for key in self.ax:
            self.ax[key] = self.fig.add_subplot(1, 1, count)
            self.ax[key].grid("equal")
            self.ax[key].title.set_text(key)
            self.ax[key].set_xlim(self.graphRange[key][0])
            self.ax[key].set_ylim(self.graphRange[key][1])
            self.ax[key].set_aspect("equal")
            self.ax[key].grid("equal")
            count+=1


        self.centAlignArc = [
            drawArc(self.ax["frame"], self.tri.camPos[0][:2],7,0,self.camAng_offset[0],True,3,plotColor="gray"),
            drawArc(self.ax["frame"], self.tri.camPos[1][:2],7,0,self.camAng_offset[1],True,3,plotColor="gray"),
        ]

        self.ps_stuff = {}
        """dictionary to hold updating plots/scatter/patches e.t.c.
        {Key: Values}
            "triCornerAngle"    :   [ang_tri[0], ang_tri[1], ang_p] {[string, string, string]}
            "triSideLength"     :   [l_tri[0], l_tri[1], l_hypotenuse] {[float, float, float]}
            "triSideLengthText" :   [l_tri[0], l_tri[1], l_hypotenuse] {[float, float, float]}
            "Pp"                :   [given_point, solved_point] :   "contains all three points"
            "PpText"            :   [given_point, solved_point] :   "contains text for each point"
            "camRelativeAngText":   [cam0, cam1]    :   "contains relative angle between camCenter and length"
            "centerAlignArc     :   [cam0, cam1]    :   "contains center aligned angle arc"
        """
        self.ps_stuff.update({"triCornerAngleText": 3*[0]}) #text
        self.ps_stuff.update({"triSideLength": 3*[0]}) #plotted lengths
        self.ps_stuff.update({"triSideLengthText": 3*[0]})
        self.ps_stuff.update({"Pp": 2*[0]}) #[given, solved]
        self.ps_stuff.update({"PpText": 2*[0]}) #[given, solved]
        self.ps_stuff.update({"camRelativeAngText": 2*[0]})
        self.ps_stuff.update({"centerAlignArc": 2*[0]})

        self.ani = animation.FuncAnimation( \
            self.fig, self.update, interval=1, frames=360, \
            init_func=self.setup_plot, blit=False,  \
        )
    def data_stream(self):
        i=0
        self.testPos = [
            math.sin(toRadians(i))*self.radius+self.basePos[0],
            math.cos(toRadians(i))*self.radius+self.basePos[1],
            self.basePos[2]
        ]
        self.tri.solvePos(self.testPos)
        self.solvedPos = self.tri.solved_pos
        while True:
            if self.IRcams.update() == None: break
            self.tri.solvePos([self.IRcams.tempPos[0][0], self.IRcams.tempPos[1][0])
            #for i in range(360, 0, -1):
            #    self.streamAngle = i
            #    self.testPos = [
            #        math.sin(toRadians(i))*self.radius+self.basePos[0],
            #        math.cos(toRadians(i))*self.radius+self.basePos[1],
            #        self.basePos[2]
            #    ]
            #    toSolveAng = 2*[0]

            #    toSolveAng[0] = toDegrees(math.atan((self.testPos[0]-self.camPos[0][0])/(self.testPos[1]-self.camPos[0][1])))-90+self.tri.ang_offset[0]
            #    xDiff = self.testPos[0]-self.camPos[1][0]
            #    if xDiff<0:      toSolveAng[1] =        toDegrees(-1*math.atan((self.testPos[1]-self.camPos[1][1])/(self.testPos[0]-self.camPos[1][0])))-180+self.tri.ang_offset[1]
            #    elif xDiff>0:    toSolveAng[1] =    180+toDegrees(-1*math.atan((self.testPos[1]-self.camPos[1][1])/(self.testPos[0]-self.camPos[1][0])))-180+self.tri.ang_offset[1]
            #    elif xDiff==0:   toSolveAng[1] =    0
            #    self.tri.solvePos([
            #        toSolveAng[0]/self.tri.camCoef[0][0],
            #        toSolveAng[1]/self.tri.camCoef[1][0]
            #    ])
            #    self.solvedPos = self.tri.solved_pos
            #    yield i
            yield 0
    def setup_plot(self):
        next(self.stream)

        self.ax["frame"].plot(
            [self.tri.camPos[0][0], self.tri.camPos[0][0]+math.cos(toRadians(self.tri.ang_offset[0]))*10],
            [self.tri.camPos[0][1], self.tri.camPos[0][1]+math.sin(toRadians(self.tri.ang_offset[0]))*10],
            linestyle="dashed", color="gray", label="cam0 centerAlign"
        )
        self.ax["frame"].plot(
            [self.tri.camPos[1][0], self.tri.camPos[1][0]+math.cos(toRadians(self.tri.ang_offset[1]))*10],
            [self.tri.camPos[1][1], self.tri.camPos[1][1]+math.sin(toRadians(self.tri.ang_offset[1]))*10],
            linestyle="dashed", color="gray", label="cam1 centerAlign"
        )

        self.ps_stuff["centerAlignArc"] = [
            self.centAlignArc[0].setup(),
            self.centAlignArc[1].setup()
        ]

        self.ps_stuff["camRelativeAngText"] = [
            self.ax["frame"].text(self.tri.camPos[0][0]+math.cos(toRadians(self.tri.ang_offset[0]))*10,
                                  self.tri.camPos[0][1]+math.sin(toRadians(self.tri.ang_offset[0]))*10,
                                  f"{round(self.tri.ang_tri[0],2)}"),
            self.ax["frame"].text(self.tri.camPos[1][0]+math.cos(toRadians(self.tri.ang_offset[1]))*10,
                                  self.tri.camPos[1][1]+math.sin(toRadians(self.tri.ang_offset[1]))*10,
                                  f"{round(self.tri.ang_tri[1],2)}")
        ]

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


        self.ax["frame"].legend(bbox_to_anchor=(1.1, 1.05))

        retur=[]
        for key,val in self.ps_stuff.items():
            for el in val: retur.append(el)
        return retur
    def ps_updateText(self):
        self.ps_stuff["camRelativeAngText"][0].set_text(f"{round(self.tri.ang_read[0],2)}")
        self.ps_stuff["camRelativeAngText"][1].set_text(f"{round(self.tri.ang_read[1],2)}")
        self.ps_stuff["PpText"][0].set_text(f"solv..[{round(self.solvedPos[0],1)},{round(self.solvedPos[1],1)}]")
        self.ps_stuff["PpText"][0].set_x(self.solvedPos[0])
        self.ps_stuff["PpText"][0].set_y(self.solvedPos[1]+2)
        self.ps_stuff["PpText"][1].set_text(f"give.[{round(self.testPos[0],1)},{round(self.testPos[1],1)}]")
        self.ps_stuff["PpText"][1].set_x(self.testPos[0])
        self.ps_stuff["PpText"][1].set_y(self.testPos[1]-2)
        self.ps_stuff["triCornerAngleText"][0].set_text(f"{round(self.tri.ang_tri[0],2)}")
        self.ps_stuff["triCornerAngleText"][0].set_x(self.tri.camPos[0][0])
        self.ps_stuff["triCornerAngleText"][0].set_y(self.tri.camPos[0][1])
        self.ps_stuff["triCornerAngleText"][1].set_text(f"{round(self.tri.ang_tri[1],2)}")
        self.ps_stuff["triCornerAngleText"][1].set_x(self.tri.camPos[1][0])
        self.ps_stuff["triCornerAngleText"][1].set_y(self.tri.camPos[1][1])
        self.ps_stuff["triCornerAngleText"][2].set_text(f"{round(self.tri.ang_p,2)}")
        self.ps_stuff["triCornerAngleText"][2].set_x(self.solvedPos[0])
        self.ps_stuff["triCornerAngleText"][2].set_y(self.solvedPos[1])
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
        for i in range(2):
            self.centAlignArc[i].resolution = round(0.1*(self.tri.ang_tri[0]))+1
            self.centAlignArc[i].update(-self.tri.ang_read[i],self.tri.ang_offset[i],self.tri.camPos[i][:2])
        # self.centAlignArc[1].update(-self.tri.ang_read[1],self.tri.ang_offset[1],self.tri.camPos[1][:2])
        self.ps_updateText()
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
else:
    errMsg = "ERROR: You're running file \"{__file__}\" not as main. Fookin' idiot. check your code"
    print(len(errMsg)*"=")
    print(errMsg)
    print(len(errMsg)*"=")
