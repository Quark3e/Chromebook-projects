#!/usr/bin/env python3

import sys
import os.path
import cv2
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import math
import time

absPath = os.path.realpath(__file__)[:-len("camTest.py")]
sys.path.append(absPath[:absPath.find("science")])

import teststuff.python.modules.IR_track_opencv as IR_track
import teststuff.python.modules.useful as useful

from science.math.analytic_geometry.two_cam_coordinate import camTriangle, toRadians, toDegrees
from teststuff.python.matplotlib.basic.nonFilled_arc import drawArc



from subprocess import Popen, PIPE


anim_use        = True
anim_display    = False
anim_update     = False

display_graph   = True # note sure how to exactly integrate this properly into the animate class
measure_perf    = True

recordVideo     = True


template_perf_ms = {
    "value" : 0,
    "unit"  : "",
    "prefix": ""
}


class AnimatedPlot(object):
    camPos = [[0, 0, 0], [25, 0, 0]]
    camAng_offset = [90, 123] #degrees
    streamAngle = 0 #degree

    basePos = [0, 0, 0]
    testPos = [0, 0, 0]
    solvedPos = [0, 0, 0]

    saveAnim = True

    timeDelta = 2*[0]

    perfObj = useful.basic_perf([
                    "plt_total",
                    "plt_00",
                    "plt_01",
                    "plt_02",
                    "plt_1",
                    "plt_2",
                    "camPos.from.py",
                    "cpp_total",
                    "cpp_enc",
                    "cpp_write",
                    "cpp_flush",
                    "cpp_read",
                    "get_camPos",
                    "get_realPos",
                    "pltAnim",
                    "stream_total",
                    "update_total"
                ], 1)
    perf_deltaFilter = lambda x: 0.5*x

    
    tempCount = 0

    winToDisp = {
        "opencv": True,
        "pyplot": True
    }

    CPP_opts = {
        "useCPP": False,
        "useCamera": False,
        "useTrigClass": False
    }
    to_cppEXE = "[0000.0,0000.0,0000.0,0000.0]"
    from_cppEXE = ""


    def cpp_update(self):
        if measure_perf:
            self.perfObj.tA("cpp_total")
            self.perfObj.tA("cpp_enc")

        self.to_cppEXE+="\n"
        value = self.to_cppEXE.encode("utf-8")
        if measure_perf:
            self.perfObj.tB("cpp_enc")
            self.perfObj.tA("cpp_write")

        self.cpp_P.stdin.write(value)
        
        if measure_perf:
            self.perfObj.tB("cpp_write")
            self.perfObj.tA("cpp_flush")

        self.cpp_P.stdin.flush()

        if measure_perf:
            self.perfObj.tB("cpp_flush")
            self.perfObj.tA("cpp_read")

        self.from_cppEXE = self.cpp_P.stdout.readline().decode("utf-8")

        if measure_perf:
            self.perfObj.tB("cpp_read")
            self.perfObj.tB("cpp_total")

        print("from C++ exe received:", self.from_cppEXE)
    def cpp_closeCams(self):
        self.to_cppEXE = "[6942.0,6942.0,6942.0,6942.0]"
        self.cpp_update()
        time.sleep(2)

    def __init__(
            self,
            useCPP_prog     = False,
            useCPP_useCam   = False,
            useCPP_trigClass= False,
        ):
        if useCPP_prog:
            self.CPP_opts["useCPP"] = useCPP_prog
            self.CPP_opts["useCamera"] = useCPP_useCam
            self.CPP_opts["useTrigClass"] = useCPP_trigClass
            cmdArgsList = [absPath+"build/cppEXE"]
            if self.CPP_opts["useCamera"] and not self.CPP_opts["useTrigClass"]: cmdArgsList.append("1")
            elif self.CPP_opts["useTrigClass"] and not self.CPP_opts["useCamera"]: cmdArgsList.append("2")
            elif not self.CPP_opts["useCamera"] and not self.CPP_opts["useTrigClass"] and self.CPP_opts:
                print("You've got brain damage.")
                exit()

            self.cpp_P = Popen(
                cmdArgsList,
                shell=False,
                stdout=PIPE,
                stdin=PIPE
            )

        sys.stdout.write("\x1B[2J")
        print(useCPP_prog, useCPP_useCam, useCPP_trigClass)  
        print(self.CPP_opts)

        self.timeDelta[0] = time.perf_counter()

        self.tri = camTriangle()
        #things used from camTri:
        #   .l_tri[2]       #dynamic
        #   .solved_pos[2]  #dynamic
        #   .camPos[2][3]   #static
        #   .camFOV[2][2]   #static
        #   .ang_offset[2]  #static
        #   .ang_tri[2]     #dynamic

        self.IRcams = IR_track.IR_camTrack()



        if not self.CPP_opts["useTrigClass"]:
            self.tri.setup(self.camPos, self.camAng_offset)
            try:
                self.tri.solvePos(self.testPos[:2])
            except ZeroDivisionError:
                print(f"Error: camTest.py: ZeroDivisionError")
                self.tri.l_tri = [1, 1]
        else:
            self.tri.camPos = self.camPos
            self.tri.ang_offset = self.camAng_offset
            self.tri.get_l_hypotenuse()

        if not self.CPP_opts["useCamera"]:
            self.IRcams.setup([2, 0], displayWindows=self.winToDisp["opencv"])


        if self.winToDisp["pyplot"]:
            self.graphRange = {
                "frame": [
                    [
                        min([self.camPos[0][0], self.camPos[1][0]])-15,
                        max([self.camPos[0][0], self.camPos[1][0]])+15
                    ],
                    [
                        min([self.camPos[0][1], self.camPos[1][1]])-1,
                        max([self.camPos[0][1], self.camPos[1][1]])+50,
                    ]
                ]
            }


        self.stream = self.data_stream()


        if not self.winToDisp["pyplot"]:
            self.timeDelta[0] = time.perf_counter()
            try:
                while True:
                    next(self.stream)
                    # self.timeDelta[1] = time.perf_counter()
                    # if self.IRcams.update() == None:
                    #     print("NOTE: IRcams.update() returned None: Exiting")
                    #     break
                    # try: self.tri.solvePos([self.IRcams.tempPos[0][0], self.IRcams.tempPos[2][0]])
                    # except ZeroDivisionError: print("Error: camTest.py: ZeroDivisionError")

                    # self.solvedPos = self.tri.solved_pos
                    # print(
                    #     f"solved pos: [{round(self.solvedPos[0],1):>4}:{round(self.solvedPos[1]):>4}]", " | ",
                    #     f"fps: {round(1/(self.timeDelta[1]-self.timeDelta[0]),4):>6}",
                    #     end="\r")
                    # self.timeDelta[0] = self.timeDelta[1]
            except KeyboardInterrupt:
                print("KeyboardInterrupt: Exiting...")
            return

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
                plotColor="gray"
            ),
            drawArc(
                self.ax["frame"],
                self.tri.camPos[1][:2],
                7,
                0,
                self.camAng_offset[1],
                True,
                3,
                plotColor="gray"
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

        if anim_use:
            self.ani = animation.FuncAnimation( \
                self.fig, self.update, interval=1, frames=360, \
                init_func=self.setup_plot, blit=False,  \
            )
    def data_stream(self):
        self.testPos = 3*[0]

        while True:
            #from end of loop iteration to here is ~25ms (~40±10 fps)
            
                # self.perfObj.tA("\"stream\"_total")

            if measure_perf:
                self.perfObj.tA("camPos.from.py")
                
            if not self.CPP_opts["useCamera"]:
                if self.IRcams.update() == None:
                    print("\nNOTE: IRcams.update() returned None: Exiting..\n")
                    break
                print(f" inp:[{self.IRcams.tempPos[0][0]},{self.IRcams.tempPos[2][0]}] ", end="")
                if self.CPP_opts["useCPP"] and self.CPP_opts["useTrigClass"]:
                    self.to_cppEXE = ("["+
                                 f"{self.IRcams.tempPos[0][0]:6.1f},"+
                                 f"{self.IRcams.tempPos[0][1]:6.1f},"+
                                 f"{self.IRcams.tempPos[2][0]:6.1f},"+
                                 f"{self.IRcams.tempPos[2][1]:6.1f}]")
            if measure_perf:
                self.perfObj.tB("camPos.from.py")
                
            if self.CPP_opts["useCPP"]: self.cpp_update()

            if measure_perf:
                self.perfObj.tA("get_camPos")

            if self.CPP_opts["useCPP"] and self.CPP_opts["useCamera"]:
                self.IRcams.tempPos[0][0] = float(self.from_cppEXE[1:7])
                self.IRcams.tempPos[0][1] = float(self.from_cppEXE[8:14])
                self.IRcams.tempPos[2][0] = float(self.from_cppEXE[15:21])
                self.IRcams.tempPos[2][1] = float(self.from_cppEXE[22:28])
                
            if measure_perf:
                self.perfObj.tB("get_camPos")
                self.perfObj.tA("get_realPos")

            if not self.CPP_opts["useTrigClass"]:
                try:
                    self.tri.solvePos([self.IRcams.tempPos[0][0], self.IRcams.tempPos[2][0]])
                except ZeroDivisionError: 
                    print("Error: camTest.py: ZeroDivisionError")
                print(f" l_hyp:{self.tri.l_hypotenuse} ", end="")
                print(f" l_tri:[{round(self.tri.l_tri[0],1):>3}, {round(self.tri.l_tri[1],1):>3}] ", end="")
                print(f" ang_tri:[{round(self.tri.ang_tri[0],1):>3}, {round(self.tri.ang_tri[1],1):>3}] ", end="")

                self.solvedPos = self.tri.solved_pos
            else:
                self.tri.l_tri[0] = float(self.from_cppEXE[29:35])
                self.tri.l_tri[1] = float(self.from_cppEXE[36:42])

                self.tri.ang_tri[0] = float(self.from_cppEXE[43:49])
                self.tri.ang_tri[1] = float(self.from_cppEXE[50:56])

                self.tri.solved_pos[0] = float(self.from_cppEXE[57:63])
                self.tri.solved_pos[2] = float(self.from_cppEXE[64:70])
                self.tri.solved_pos[1] = float(self.from_cppEXE[71:77])
                self.solvedPos = self.tri.solved_pos
            
            if measure_perf: self.perfObj.tB("get_realPos")

            toPrintString = (
                f"solved pos: [{round(self.solvedPos[0],1):>4}: {round(self.solvedPos[1],1):>4}]" + " | \n")


            if measure_perf:
                # self.perfObj.tB("\"stream\"_total")
                toPrintString+=self.perfObj.print(["total"])
            #     toPrintString += "\nperf:\n"
            #     sumDelay = 0
            #     for key,value in self.perf.items():
            #         toPrintString += f"  -{key:<16}: {round(value['delay']['value']*1000,2):>6}ms\n"

            #         if key[-5:]!="total":
            #             sumDelay += value["delay"]["value"]
            #     toPrintString += f"total: [{round(sumDelay*1000,2):>6}ms] [fps:{round(1/sumDelay,1):>6}]\n"
            
            # # print(toPrintString)
            useful.simpleANSIWrite(toPrintString, ["right", 5])
            # sys.stdout.write(
            # "\x1B[2J"+#\x1B[H"+
            #     "\x1B[5;0H"+
            #     toPrintString
            # )

            # if self.tempCount>10:
            #     for key,val in self.perfObj.perfDict.items():
            #         print(f" -{key:<20}: {val}")
            #     input("paused")
            #     self.tempCount=0
            # self.tempCount+=1

            yield self.IRcams.tempPos
    def setup_plot(self):
        next(self.stream)

        camFov_arc = [
            drawArc(self.ax["frame"], self.tri.camPos[0][:2],10,self.tri.camFOV[0][0],self.camAng_offset[0]-self.tri.camFOV[0][0]/2,True,20,plotLinestyle="dotted"),
            drawArc(self.ax["frame"], self.tri.camPos[1][:2],10,self.tri.camFOV[1][0],self.camAng_offset[1]-self.tri.camFOV[1][0]/2,True,20,plotLinestyle="dotted"),
        ]

        self.ax["frame"].plot([self.tri.camPos[0][0], self.tri.camPos[0][0]+math.cos(toRadians(self.tri.ang_offset[0]))*10],
                              [self.tri.camPos[0][1], self.tri.camPos[0][1]+math.sin(toRadians(self.tri.ang_offset[0]))*10],
                              linestyle="dashed", color="gray", label="cam0 centerAlign")
        self.ax["frame"].plot([self.tri.camPos[1][0], self.tri.camPos[1][0]+math.cos(toRadians(self.tri.ang_offset[1]))*10],
                              [self.tri.camPos[1][1], self.tri.camPos[1][1]+math.sin(toRadians(self.tri.ang_offset[1]))*10],
                              linestyle="dashed", color="gray", label="cam1 centerAlign")

        self.ps_stuff["centerAlignArc"] = [self.centAlignArc[0].setup(), self.centAlignArc[1].setup()]

        self.ps_stuff["camRelativeAngText"] = [
            self.ax["frame"].text(self.tri.camPos[0][0] + math.cos(toRadians(self.tri.ang_offset[0]))*10,
                                  self.tri.camPos[0][1] + math.sin(toRadians(self.tri.ang_offset[0]))*10,
                                  f"{round(self.tri.ang_tri[0],2)}"),
            self.ax["frame"].text(self.tri.camPos[1][0] + math.cos(toRadians(self.tri.ang_offset[1]))*10,
                                  self.tri.camPos[1][1] + math.sin(toRadians(self.tri.ang_offset[1]))*10,
                                  f"{round(self.tri.ang_tri[1],2)}")
        ]

        self.ps_stuff["Pp"][0] = self.ax["frame"].scatter([self.tri.camPos[0][0],self.tri.camPos[1][0],self.solvedPos[0]],
                                                          [self.tri.camPos[0][1],self.tri.camPos[1][1],self.solvedPos[1]],
                                                          edgecolor="k", label="given")
        self.ps_stuff["Pp"][1] = self.ax["frame"].scatter([self.tri.camPos[0][0],self.tri.camPos[1][0],self.testPos[0]],
                                                          [self.tri.camPos[0][1],self.tri.camPos[1][1],self.testPos[1]],
                                                          edgecolor="k", label="solved")
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
        if measure_perf:
            self.perfObj.tB("pltAnim")
            self.perfObj.tA("stream_total")

        next(self.stream)


        if measure_perf:
            self.perfObj.tB("stream_total")
            self.perfObj.tA("plt_total")
            self.perfObj.tA("plt_00")

        for i in range(2):
            self.centAlignArc[i].resolution = round(0.1*(self.tri.ang_tri[0]))+1

        # self.centAlignArc[1].update(-self.tri.ang_read[1],self.tri.ang_offset[1],self.tri.camPos[1][:2])

        if measure_perf:
            self.perfObj.tB("plt_00")
            self.perfObj.tA("plt_01")

        # for i in range(2):
        #     self.centAlignArc[i].update(-self.tri.ang_read[i],self.tri.ang_offset[i],self.tri.camPos[i][:2])
        # self.centAlignArc[0].update(-self.tri.ang_read[0],self.tri.ang_offset[0],self.tri.camPos[0][:2])


        if measure_perf:
            self.perfObj.tB("plt_01")
            self.perfObj.tA("plt_02")

        self.ps_updateText()

        if measure_perf:
            self.perfObj.tB("plt_02")
            self.perfObj.tA("plt_1")

        self.ps_stuff["Pp"][0].set_offsets([[self.tri.camPos[0][0],self.tri.camPos[0][1]],[self.tri.camPos[1][0],self.tri.camPos[1][1]],[self.solvedPos[0],self.solvedPos[1]]])
        self.ps_stuff["Pp"][1].set_offsets([[self.tri.camPos[0][0],self.tri.camPos[0][1]],[self.tri.camPos[1][0],self.tri.camPos[1][1]],[self.testPos[0],self.testPos[1]]])
        self.ps_stuff["triSideLength"][0].set_data([self.tri.camPos[0][0],self.solvedPos[0]], [self.tri.camPos[0][1],self.solvedPos[1]])
        self.ps_stuff["triSideLength"][1].set_data([self.solvedPos[0],self.tri.camPos[1][0]], [self.solvedPos[1],self.tri.camPos[1][1]])
        self.ps_stuff["triSideLength"][2].set_data([self.tri.camPos[1][0],self.tri.camPos[0][0]], [self.tri.camPos[1][1],self.tri.camPos[0][1]])
        
        if measure_perf:
            self.perfObj.tB("plt_1")
            self.perfObj.tA("plt_2")

        if anim_display:
            retur=[]
            for key,val in self.ps_stuff.items():
                for el in val: retur.append(el)
            
        if measure_perf:
            self.perfObj.tB("plt_2")
            self.perfObj.tB("plt_total")
            self.perfObj.tB("update_total")
            self.perfObj.tA("pltAnim")
            self.perfObj.tA("update_total")

        if anim_display:
            return retur


if __name__=="__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Optioal app description")
    parser.add_argument("-m", "--masterCPP", type=int, choices=[0, 1],
                        help="\"master key\" for whether to use cpp or not:\n  0-turn off\n  1-turn on")
    parser.add_argument("-1", "--useCPP", action="store_true", #type=bool, nargs="?", const=False,
                        help="boolean whether to use CPP program's tracking instead")
    parser.add_argument("-2", "--CPP_useCamera", action="store_true", #type=bool, nargs="?", const=False,
                        help="boolean of whether to use the cameras on cpp program instead of python's")
    parser.add_argument("-3", "--CPP_useClass", action="store_true", #type=bool, nargs="?", const=False,
                        help="boolean of whether to use CPP's trigonometric class instead of pythons")
    parser.add_argument("-4", "--useAnim", action="store_false",# type=bool, nargs="?", const=True,
                        help="boolean of whether to use the matplotlib.animation")

    args = parser.parse_args()

    print("\x1B[2J\x1B[H")

    print(args)


    if args.masterCPP==0:
        args.useCPP = False
        args.CPP_useCamera  = False
        args.CPP_useClass   = False
    elif args.masterCPP==1:
        args.useCPP = True
        args.CPP_useCamera  = True
        args.CPP_useClass   = True

    # exit()

    # if args.useCPP==None: args.useCPP = False
    # if args.CPP_useCamera==None: args.CPP_useCamera = False
    # if args.CPP_useClass==None: args.CPP_useClass = False


    anim_use = args.useAnim

    a = AnimatedPlot(
        args.useCPP,
        args.CPP_useCamera,
        args.CPP_useClass
    )

    if not anim_use:
        try:
            while True:
                next(a.stream)
        except KeyboardInterrupt:
            print("\nexiting..")
    else:
        plt.show()
    # print("\x1B[2J")
    if not args.useCPP: a.IRcams.close()
    if args.useCPP and args.CPP_useCamera:
        print("\n Closing\n")
        a.cpp_closeCams()
else:
    errMsg = "ERROR: You're running file \"{__file__}\" not as main. Fookin' idiot. check your code"
    print(len(errMsg)*"=")
    print(errMsg)
    print(len(errMsg)*"=")
