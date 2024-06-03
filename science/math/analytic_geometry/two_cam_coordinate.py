#!/usr/bin/env python3.9

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

    ang_offset = [90, 200]
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

    classInit = False
    def get_l_hypotenuse(self,):
        self.l_delta = [self.camPos[1][0]-self.camPos[0][0], self.camPos[1][1]-self.camPos[0][1]]
        self.l_hypotenuse = math.sqrt(self.l_delta[0]**2+self.l_delta[1]**2)

    def __init__(self):
        pass
    def setup(self, camPos: float, camAngOffset: float):
        """Setup of class

        Args:
            camPos (float/int): [2][2] list of xy-position of both cameras
            camAngOffset (float/int) [degrees]: [2] list of camera angle offsets in related to ground/base/x-axis. Perpendicular occular axis means 90 degrees
        """
        self.classInit = True
        self.camPos = camPos
        self.ang_offset = camAngOffset
        self.l_delta = [camPos[1][0]-camPos[0][0], camPos[1][1]-camPos[0][1]]
        self.l_hypotenuse = math.sqrt(self.l_delta[0]**2+self.l_delta[1]**2)
        if self.l_delta[1]==0:
            self.ang_d = [toDegrees(math.atan(self.l_delta[1]/self.l_delta[0])), 90]
        else:
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
        #self.ang_tri = [
        #    self.ang_offset[0]-self.ang_d[0]-self.ang_read[0],
        #    270-self.ang_d[1]-(self.ang_offset[1]-self.ang_read[1])
        #    # self.ang_offset[1]-self.ang_d[1]-self.ang_read[1]
        #]
        self.ang_tri = [
            90-self.ang_d[0]-self.ang_read[0],
            180-self.ang_offset[1]+self.ang_d[0]+self.ang_read[1]
            #self.ang_d[0] + self.ang_read[1] + 180-self.ang_offset[1]
        ]
        #print([round(i,1) for i in self.ang_read], "|", [round(i,2) for i in self.ang_tri], end=" | ")
        self.ang_p = 180 - abs(self.ang_tri[0]) - abs(self.ang_tri[1])
    def solvePos(self, rawPos, useAng=False):
        self.read_pix = [
            [rawPos[0]-self.camRes[0][0]*0.5, self.camRes[0][1]*0.5-rawPos[0]],
            [rawPos[1]-self.camRes[1][0]*0.5, self.camRes[1][1]*0.5-rawPos[1]]
        ]
        self.ang_read = [
            self.read_pix[0][0]*self.camCoef[0][0],
            self.read_pix[1][0]*self.camCoef[1][0]
        ]
        # print([round(i) for i in rawPos])
        self.solveAngL()
        self.l_tri = [
            (self.l_hypotenuse*math.sin(toRadians(self.ang_tri[1]))) / math.sin(toRadians(self.ang_p)),
            (self.l_hypotenuse*math.sin(toRadians(self.ang_tri[0]))) / math.sin(toRadians(self.ang_p))
        ]
        self.solved_pos = [
                self.camPos[0][0]+math.cos(toRadians(self.ang_offset[0]+self.ang_read[0]))*self.l_tri[0],
                self.camPos[0][1]+math.sin(toRadians(self.ang_offset[0]+self.ang_read[0]))*self.l_tri[0],
                None
        ]
        self.solved_pos = [
                self.camPos[1][0]+math.cos(toRadians(self.ang_offset[1]-self.ang_read[1]))*self.l_tri[1],
                self.camPos[1][1]+math.sin(toRadians(self.ang_offset[1]-self.ang_read[1]))*self.l_tri[1],
                None
        ]

        self.solved_pos[2] = self.solved_pos[1]*math.tan(toRadians(self.read_pix[0][1]*self.camCoef[0][1]))+self.camPos[0][2]
        return self.solved_pos

if __name__=="__main__":
    useCPP_cam = False

    import matplotlib.pyplot as plt
    import matplotlib.animation as animation
    import matplotlib.patches as mpatches
    import sys
    import os.path
    absPath = os.path.realpath(__file__)[:-len("two_cam_coordinate.py")]
    sys.path.append(absPath[:absPath.find("science")])
    from teststuff.python.matplotlib.basic.nonFilled_arc import drawArc

    
    class AnimatedPlot(object):
        camPos = [[0, 0, 0], [25, 0, 0]]
        camAng_offset = [90, 120] #degrees
        streamAngle = 0 #degree
        radius = 5
        basePos = [0, 15, 0]
        testPos = [15, 15, 0]
        solvedPos = [0, 0, 0]

        saveAnim = False
        def __init__(self):
            self.tri = camTriangle()
            self.tri.setup(self.camPos, self.camAng_offset)
            try:
                self.tri.solvePos(self.testPos[:2])
            except ZeroDivisionError:
                print("__init__: zero division error")
                self.tri.l_tri = [1, 1]

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
                drawArc(
                    self.ax["frame"],
                    self.tri.camPos[0][:2],
                    7,
                    0,
                    self.camAng_offset[0],
                    True,
                    3,
                    plotColor="gray",
                    plotAlpha = 1
                ),
                drawArc(
                    self.ax["frame"],
                    self.tri.camPos[1][:2],
                    7,
                    0,
                    self.camAng_offset[1],
                    True,
                    3,
                    plotColor="gray",
                    plotAlpha = 1
                ),
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
                for i in range(360, 0, -1):
                    self.streamAngle = i
                    self.testPos = [
                        math.sin(toRadians(i))*self.radius+self.basePos[0],
                        math.cos(toRadians(i))*self.radius+self.basePos[1],
                        self.basePos[2]
                    ]
                    toSolveAng = 2*[0]
                    
                    toSolveAng[0] = toDegrees(math.atan((self.testPos[0]-self.camPos[0][0])/(self.testPos[1]-self.camPos[0][1])))-90+self.tri.ang_offset[0]
                    xDiff = self.testPos[0]-self.camPos[1][0]
                    if xDiff<0:      toSolveAng[1] =        toDegrees(-1*math.atan((self.testPos[1]-self.camPos[1][1])/(self.testPos[0]-self.camPos[1][0])))-180+self.tri.ang_offset[1]
                    elif xDiff>0:    toSolveAng[1] =    180+toDegrees(-1*math.atan((self.testPos[1]-self.camPos[1][1])/(self.testPos[0]-self.camPos[1][0])))-180+self.tri.ang_offset[1]
                    elif xDiff==0:   toSolveAng[1] =    0
                    self.tri.solvePos([
                        toSolveAng[0]/self.tri.camCoef[0][0]+self.tri.camRes[0][0]*0.5,
                        toSolveAng[1]/self.tri.camCoef[1][0]+self.tri.camRes[1][0]*0.5
                    ])
                    # print([
                    #     toSolveAng[0]/self.tri.camCoef[0][0]+self.tri.camRes[0][0]*0.5,
                    #     toSolveAng[1]/self.tri.camCoef[1][0]+self.tri.camRes[1][0]*0.5
                    # ])
                    self.solvedPos = self.tri.solved_pos
                    print(
                        f"ang:{i:>3} | solved pos: [{round(self.solvedPos[0],1):>4}:{round(self.solvedPos[1]):>4}]", " | ",
                        end="\r")
                    yield i
        def setup_plot(self):
            next(self.stream)


            camFov_arc = [
                drawArc(self.ax["frame"], self.tri.camPos[0][:2],10,self.tri.camFOV[0][0],self.camAng_offset[0]-self.tri.camFOV[0][0]/2,True,20,plotLinestyle="dotted",plotAlpha=0.2),
                drawArc(self.ax["frame"], self.tri.camPos[1][:2],10,self.tri.camFOV[1][0],self.camAng_offset[1]-self.tri.camFOV[1][0]/2,True,20,plotLinestyle="dotted",plotAlpha=0.2),
            ]
            camFov_arc[0].setup()
            camFov_arc[1].setup()

            self.ax["frame"].plot( \
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
            self.ps_stuff["camRelativeAngText"][0].set_text(f"camRel.{round(self.tri.ang_read[0],2)}")
            self.ps_stuff["camRelativeAngText"][1].set_text(f"camRel.{round(self.tri.ang_read[1],2)}")
            self.ps_stuff["PpText"][0].set_text(f"solv..[{round(self.solvedPos[0],1)},{round(self.solvedPos[1],1)}]")
            self.ps_stuff["PpText"][0].set_x(self.solvedPos[0])
            self.ps_stuff["PpText"][0].set_y(self.solvedPos[1]+2)
            self.ps_stuff["PpText"][1].set_text(f"give.[{round(self.testPos[0],1)},{round(self.testPos[1],1)}]")
            self.ps_stuff["PpText"][1].set_x(self.testPos[0])
            self.ps_stuff["PpText"][1].set_y(self.testPos[1]-2)
            self.ps_stuff["triCornerAngleText"][0].set_text(f"tAng[0]:{round(self.tri.ang_tri[0],2)}")
            self.ps_stuff["triCornerAngleText"][0].set_x(self.tri.camPos[0][0])
            self.ps_stuff["triCornerAngleText"][0].set_y(self.tri.camPos[0][1])
            self.ps_stuff["triCornerAngleText"][1].set_text(f"tAng[1]:{round(self.tri.ang_tri[1],2)}")
            self.ps_stuff["triCornerAngleText"][1].set_x(self.tri.camPos[1][0])
            self.ps_stuff["triCornerAngleText"][1].set_y(self.tri.camPos[1][1])
            self.ps_stuff["triCornerAngleText"][2].set_text(f"tAng[2]{round(self.tri.ang_p,2)}")
            self.ps_stuff["triCornerAngleText"][2].set_x(self.solvedPos[0])
            self.ps_stuff["triCornerAngleText"][2].set_y(self.solvedPos[1])
            self.ps_stuff["triSideLengthText"][0].set_text(f"l[0]:{round(self.tri.l_tri[0],2)}")
            self.ps_stuff["triSideLengthText"][0].set_x((self.tri.camPos[0][0]+self.solvedPos[0])/2+2)
            self.ps_stuff["triSideLengthText"][0].set_y((self.tri.camPos[0][1]+self.solvedPos[1])/2+2)
            self.ps_stuff["triSideLengthText"][1].set_text(f"l[1]:{round(self.tri.l_tri[1],2)}")
            self.ps_stuff["triSideLengthText"][1].set_x((self.solvedPos[0]+self.tri.camPos[1][0])/2+2)
            self.ps_stuff["triSideLengthText"][1].set_y((self.solvedPos[1]+self.tri.camPos[1][1])/2+2)
            self.ps_stuff["triSideLengthText"][2].set_text(f"l[2]:{round(self.tri.l_hypotenuse,2)}")
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


    a = AnimatedPlot()

    fileName = "cam0.gif"
    # To save the animation using Pillow as a gif
    if AnimatedPlot.saveAnim:
        writer = animation.PillowWriter(fps=15,
                                    metadata=dict(artist='Me'),
                                    bitrate=1800)
        a.ani.save(absPath+fileName, writer=writer)
    plt.show()
