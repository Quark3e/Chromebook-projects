#!/usr/bin/env python3

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as mpatches
import numpy as np
import math
import itertools
import csv

from matplotlib.patches import Arc

from AngleAnnotation import AngleAnnotation

import os.path
import sys

import time
from datetime import datetime, timedelta

# sys.path.append("../..")
# from modules.useful import printProgBar,progbar_progress,progbar_total

progbar_progress = 1
progbar_total = 1

def printProgBar(symbIdx=4):
    """
    prints a progress bar

    Parameters:
    - total: total number for progress to be at 100%
    - progress: current number for progress (progress = [current/total)*100]
    - symbIdx: indexing integer for symbol to use: ['■', '⬛', '▉', '▉', '█']
    """
    oldProg=0
    percent=0
    speed=0
    count=0
    # checkCount=[0, 3, 0.08]
    img = {
        0: "|",
        1: "/",
        2: "-",
        3: "\\",
    }
    symb = ['■', '⬛', '▉', '▉', '█']
    startDate = datetime.now()
    print(f" {'Start time':<10}:", startDate)
    progBar_t0 = time.perf_counter()
    time.sleep(0.1)
    while progbar_progress<=progbar_total:
        t1 = time.perf_counter()
        speed = (progbar_progress-oldProg)/(t1-progBar_t0)
        if speed==0: speed=1

        percent = (progbar_progress/progbar_total)*100
        formatProgress = f"{progbar_progress:_}"
        if count>=len(img): count=0
        printStr = f" progress: {formatProgress:>10}: {round(percent,2):<5}% \
              |{str(math.floor(percent)*symb[symbIdx]+img[count]):<100}|: \
              {round(speed,1):<6}pt/s: ETA: {timedelta(seconds=(round((progbar_total-oldProg)/speed)))} \
              "
        print(printStr, end="\r")
        count+=1
        oldProg=progbar_progress
        progBar_t0=time.perf_counter()
        yield printStr


absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("main.py")])
dataPath = absPath+"data/0.1/"

toRadians = lambda degrees: (degrees*np.pi)/180
toDegrees = lambda radians: (radians*180)/np.pi


def completeList_2d(inpList, inpLen):
    nPolys = [\
        np.poly1d(np.polyfit([round(n/len(inpList)*inpLen) for n in range(len(inpList))], [n[0] for n in inpList], 20)),
        np.poly1d(np.polyfit([round(n/len(inpList)*inpLen) for n in range(len(inpList))], [n[1] for n in inpList], 20)),
    ]
    returArr = [None for _ in range(inpLen)]
    for n in range(inpLen):
        returArr[n] = [(nPolys[0](n)),(nPolys[1](n))]
    return returArr

def arc_patch(center, radius, theta1, theta2, ax=None, resolution=50, **kwargs):
    if ax is None:
        ax = plt.gca()
    
    theta = np.linspace(toRadians(theta1), np.radians(theta2), resolution)
    points = np.vstack((radius*np.cos(theta) + center[0],
                        radius*np.sin(theta) * center[1]))
    poly = mpatches.Polygon(points.T, closed=True, **kwargs)
    ax.add_patch(poly)
    return poly

sys.path.append(absPath[:absPath.find("teststuff")]+"science/math/geometry/")
import quadrilateral as ql #type: ignore


class AnimatedScatter(object):
    # def checkIfReachable(self, pos, absAng, c):
    #     beta = 0
    #     if pos[0]>0: beta = 180-toDegrees(math.atan(pos[1]/pos[0]))
    #     elif pos[0]<0: beta = abs(toDegrees(math.atan(pos[1]/pos[0])))
    #     elif pos[0]==0: beta = 90
    #     b = math.sqrt(pos[0]**2 + pos[1]**2)
    #     f = math.sqrt((2*pos[0]+math.cos(toRadians(absAng))*c)**2 + (2*pos[1]+math.sin(toRadians(absAng))*c)**2)
    #     gamma = math.acos((c**2+b**2-f**2)/(2*c*b))
    def solveQuad(self, quadObj, beta, l0, l1, gd, td, eeffec):
            """
            Solve quadrilateral angles and sides and checks if it's possible
            """
            try:
                quadObj.set_a(gd)
                quadObj.set_b(l0)
                quadObj.set_c(td)
                quadObj.set_d(l1)
                quadObj.set_beta(beta)
            except ValueError:
                return None
            tempPos = [
                        [0-gd, 0], #alpha
                        [0, 0], #beta
                        [l0*math.cos(toRadians(180-quadObj.beta)), l0*math.sin(toRadians(180-quadObj.beta))], #gamma
                        [l1*math.cos(toRadians(quadObj.alpha))-gd, l1*math.sin(toRadians(quadObj.alpha))] #delta
                    ]
            if quadObj.isReachable and \
                round(math.sqrt((tempPos[1][0]-tempPos[0][0])**2 + (tempPos[1][1]-tempPos[0][1])**2),3) == gd and \
                round(math.sqrt((tempPos[2][0]-tempPos[1][0])**2 + (tempPos[2][1]-tempPos[1][1])**2),3) == l0 and \
                round(math.sqrt((tempPos[3][0]-tempPos[2][0])**2 + (tempPos[3][1]-tempPos[2][1])**2),3) == td and \
                round(math.sqrt((tempPos[0][0]-tempPos[3][0])**2 + (tempPos[0][1]-tempPos[3][1])**2),3) == l1 and \
                not (tempPos[3][0]<(-l1*0.9) and tempPos[2][0]<(-l1*0.9) and tempPos[3][1]>tempPos[2][1]) and \
                tempPos[3][1]+eeffec*math.sin(toRadians(quadObj.absAng))>=0:
                return [tempPos, quadObj.absAng]
            else:
                return None
    def generateDataSet(self):
        global progbar_total, progbar_progress
        parts = 500
        part = 0
        precis = {
            "l0": range(1, 21, 1),
            "l1": range(1, 21, 1),
            "td": range(1, 21, 1),
            "gd": range(1, 21, 1),
            "eeffec": range(1, 21, 1),
            "beta": range(0, 181, 1)
        }
        temp = [len([i for i in val]) for key,val in precis.items()]
        estComb=1 #estimated number of combinations
        progress=0
        count=0

        for i in temp: estComb*=i
        progbar_total=estComb
        nLenLim = round(estComb/parts)

        csv_fields = ["x","y","beta","leeffec","l0","l1","gd","td","abs"]
        csvResult = []

        checkCount=0
        checkCount_i=500

        quad = ql.quadrilateral()
        quad.printErrors = False

        def saveCsv():
            nonlocal part, csvResult
            print("-saving data into a csv file: part:", part)
            with open(dataPath+f"csv_{part:03}_"+f"{estComb:_}.csv", "w") as f:
                write = csv.writer(f)
                write.writerow(csv_fields)
                write.writerows(csvResult)
            part+=1
            csvResult=[]

        progFunc = printProgBar()

        for beta in precis["beta"]:
            for l_e in precis["eeffec"]:
                l_e/=10
                for l0 in precis["l0"]:
                    l0/=10
                    for l1 in precis["l1"]:
                        l1/=10
                        for td in precis["td"]:
                            td/=10
                            for gd in precis["gd"]:
                                gd/=10
                                try:    
                                    quad.set_a(gd)
                                    quad.set_b(l0)
                                    quad.set_c(td)
                                    quad.set_d(l1)
                                    quad.set_beta(180-beta)
                                except ValueError:
                                    pass

                                funcRet = self.solveQuad(quad, 180-beta, l0, l1, gd, td, l_e)
                                if funcRet!=None:
                                    eePos = [
                                        funcRet[0][3][0]+l_e*math.cos(toRadians(funcRet[1])),
                                        funcRet[0][3][1]+l_e*math.sin(toRadians(funcRet[1]))
                                    ]
                                    csvResult.append([round(eePos[0],1),round(eePos[1],1),beta,l_e,l0,l1,gd,td,quad.absAng])
                                    
                                    if count>=nLenLim:
                                        saveCsv()
                                        count=0
                                    if checkCount>=checkCount_i:
                                        progbar_progress=progress
                                        next(progFunc)
                                        checkCount=0
                                    progress+=1
                                    count+=1
                                    checkCount+=1


    def __init__(self):
        self.quad = ql.quadrilateral()

        self.genAll = False
        if self.genAll:
            self.generateDataSet()
            exit()


        #Notable configs: 1:1.01:0.15:0.2

        self.l0 = 1
        self.l1 = 1.01
        self.gd = 0.11
        self.td = 0.2
        self.end_effecLen = 1.5
        self.end_effecAng = 0

        self.angle = 45

        self.currentIdx = 0
        self.currentInpAng = 0

        self.sideIdxLett = ["a","b","c","d"]

        try:    
            self.quad.set_a(self.gd)
            self.quad.set_b(self.l0)
            self.quad.set_c(self.td)
            self.quad.set_d(self.l1)
            self.quad.set_beta(180-self.angle)
        except ValueError:
            pass

        self.pos = [
            [0-self.gd, 0], #alpha
            [0, 0], #beta
            [self.l0*math.cos(toRadians(180-self.quad.beta)), self.l0*math.sin(toRadians(180-self.quad.beta))],  #gamma
            [self.l1*math.cos(toRadians(self.quad.alpha))-self.gd, self.l1*math.sin(toRadians(self.quad.alpha))] #delta
        ]

        self.allPos = []
        self.allAbsAng = []
        self.allEndEffec_pos = [[], []]
        self.allInpAng = []

        for dir in range(-1, 2, 2):
            for q in range(90+90*(-dir), 90+90*dir+dir, dir):
                    funcRet = self.solveQuad(self.quad, 180-q, self.l0, self.l1, self.gd, self.td, self.end_effecLen)
                    if funcRet != None:
                        self.allPos.append(funcRet[0])
                        self.allInpAng.append(q)
                        
                        self.allAbsAng.append(self.quad.absAng)

                        self.allEndEffec_pos[0].append(funcRet[0][3][0]+self.end_effecLen*math.cos(toRadians(funcRet[1])))
                        self.allEndEffec_pos[1].append(funcRet[0][3][1]+self.end_effecLen*math.sin(toRadians(funcRet[1])))

        print("number of possible positions:", len(self.allPos))
        if len(self.allPos)<=0: exit()

        self.graphRange = {
            "frame": [[-(self.l1+self.gd+0.1), 2],[-2, 2]],
            "coefs": [-2, 2],
        }

        self.stream = self.data_stream()
        self.fig = plt.figure(figsize=(9,4))
        self.ax = {
            "frame": 0, #frame view
            "coefs": 0, #a1/a0 view
        }
        count=1
        for key in self.ax:
            self.ax[key] = self.fig.add_subplot(1, 2, count)
            self.ax[key].grid("equal")
            self.ax[key].title.set_text(key)
            count+=1

        self.ax["frame"].set_xlim(self.graphRange["frame"][0])
        self.ax["frame"].set_ylim(self.graphRange["frame"][1])
        # self.ax["frame"].axis("equal")
        self.ax["frame"].set_aspect("equal")

        self.ps_stuff = {}
        self.ps_stuff.update({"frame": 8*[0]})
        self.ps_stuff.update({"coefs": 0*[0]})
        self.ps_stuff.update({"circle": 0*[0]})
        self.ps_stuff.update({"text": 4*[0]})
        self.ps_stuff.update({"eeffec": 4*[0]}) #end-effector

        self.ani = animation.FuncAnimation( \
            self.fig, self.update, interval=2, \
            init_func=self.setup_plot, blit=True \
        )
    def data_stream(self):
        while True:
            for i in range(len(self.allPos)):
                self.pos=self.allPos[i]
                self.end_effecAng=self.allAbsAng[i]
                self.currentIdx = i
                self.currentInpAng = self.allInpAng[i]
                yield i
    def setup_plot(self):
        next(self.stream)

        self.ax["frame"].plot(self.allEndEffec_pos[0], self.allEndEffec_pos[1], color="r", alpha=0.6,label="path")

        # self.ax["coefs"].plot([i for i in range(round(len(self.allAbsAng)/2))], [self.allAbsAng[i]/self.allInpAng[i] for i in range(round(len(self.allAbsAng)/2))], color="r", linewidth=1)
        self.coefsX = [i for i in range(round(len(self.allAbsAng)/2))]
        self.coefsY = [self.allAbsAng[i]/(self.allInpAng[i]+0.0001) for i in range(round(len(self.allAbsAng)/2))]
        self.ax["coefs"].plot(self.coefsX, self.coefsY, color="r", linewidth=1)
        
        for n in [6]:
            xValues = [i for i in range(round(len(self.allAbsAng)/2))]
            polyModel = np.poly1d(np.polyfit(xValues, self.coefsY, n))
            self.ax["coefs"].plot([i for i in range(min(xValues), max(xValues))], [polyModel(x) for x in range(min(xValues), max(xValues))], label=f"n({n})")

        self.coefsLineCoef = 0.001
        self.ps_stuff["eeffec"][1], = self.ax["coefs"].plot(2*[0], [max(self.coefsY)+self.coefsLineCoef*len(self.coefsY), min(self.coefsY)-self.coefsLineCoef*len(self.coefsY)], label="absAngles", color="g", linewidth=2.5, alpha=0.5)

        self.ps_stuff["frame"][0] = self.ax["frame"].scatter([self.pos[0][0]],[self.pos[0][1]],edgecolor="k",label="alpha")
        self.ps_stuff["frame"][1] = self.ax["frame"].scatter([self.pos[1][0]],[self.pos[1][1]],edgecolor="k",label="beta")
        self.ps_stuff["frame"][2] = self.ax["frame"].scatter([self.pos[2][0]],[self.pos[2][1]],edgecolor="k",label="gamma")
        self.ps_stuff["frame"][3] = self.ax["frame"].scatter([self.pos[3][0]],[self.pos[3][1]],edgecolor="k",label="delta")
        self.ps_stuff["frame"][4], = self.ax["frame"].plot([self.pos[0][0],self.pos[1][0]],[self.pos[0][1],self.pos[1][1]],label="a")
        self.ps_stuff["frame"][5], = self.ax["frame"].plot([self.pos[1][0],self.pos[2][0]],[self.pos[1][1],self.pos[2][1]],label="b")
        self.ps_stuff["frame"][6], = self.ax["frame"].plot([self.pos[2][0],self.pos[3][0]],[self.pos[2][1],self.pos[3][1]],label="c")
        self.ps_stuff["frame"][7], = self.ax["frame"].plot([self.pos[3][0],self.pos[0][0]],[self.pos[3][1],self.pos[0][1]],label="d")

        self.ps_stuff["eeffec"][0], = self.ax["frame"].plot(
            [self.pos[3][0], self.pos[3][0]+self.end_effecLen*math.cos(toRadians(self.end_effecAng))],
            [self.pos[3][1], self.pos[3][1]+self.end_effecLen*math.sin(toRadians(self.end_effecAng))],
            linestyle="-",label="end-effector",color="g"
            )
        self.ps_stuff["eeffec"][2], = self.ax["frame"].plot( \
            [self.pos[3][0], self.pos[3][0]+self.end_effecLen*math.cos(toRadians(self.end_effecAng))], \
            2*[self.pos[3][1]],\
            linestyle="dashed",label="absolute angle ref.", color="gray")
        self.ps_stuff["eeffec"][3] = self.ax["frame"].scatter(
            [self.pos[3][0]+self.end_effecLen*math.cos(toRadians(self.end_effecAng))], \
            [self.pos[3][1]],\
            label="end-effector pos", color="gray"
        )

        def setText(i, plotAx, pos, offset=[0.05, 0.05]):
            return plotAx.text(
                (pos[2]+pos[0])/2+offset[0],
                (pos[3]+pos[1])/2+offset[1],
                f"{self.sideIdxLett[i]}:{round(math.sqrt((pos[2]-pos[0])**2 + (pos[3]-pos[1])**2), 2)}"
            )
        for l in range(3):
            self.ps_stuff["text"][l] = setText(l, self.ax["frame"], [self.pos[l][0],self.pos[l][1],self.pos[l+1][0],self.pos[l+1][1]])
        self.ps_stuff["text"][3] = setText(l, self.ax["frame"], [self.pos[3][0],self.pos[3][1],self.pos[0][0],self.pos[0][1]])
        

        # self.ax["frame"].legend(loc="lower left")
        # self.ax["coefs"].legend()

        retur=[]
        for key,val in self.ps_stuff.items():
            for el in val: retur.append(el)
        return retur
    def ps_updateText(self, idx, pos, offset=[0.05, 0.05]):
        self.ps_stuff["text"][idx].set_text(f"{self.sideIdxLett[idx]}:{round(math.sqrt((pos[2]-pos[0])**2 + (pos[3]-pos[1])**2), 3)}")
        self.ps_stuff["text"][idx].set_x((pos[2]+pos[0])/2+offset[0])
        self.ps_stuff["text"][idx].set_y((pos[3]+pos[1])/2+offset[1])
    def update(self, i):
        next(self.stream)

        # print([self.pos],[self.pos])
        for n in range(4):
            self.ps_stuff["frame"][n].set_offsets([[self.pos[n][0],self.pos[n][1]]])
        self.ps_stuff["frame"][4].set_data([self.pos[0][0],self.pos[1][0]],[self.pos[0][1],self.pos[1][1]])
        self.ps_stuff["frame"][5].set_data([self.pos[1][0],self.pos[2][0]],[self.pos[1][1],self.pos[2][1]])
        self.ps_stuff["frame"][6].set_data([self.pos[2][0],self.pos[3][0]],[self.pos[2][1],self.pos[3][1]])
        self.ps_stuff["frame"][7].set_data([self.pos[3][0],self.pos[0][0]],[self.pos[3][1],self.pos[0][1]])

        self.ps_stuff["eeffec"][0].set_data(
            [self.pos[3][0], self.pos[3][0]+self.end_effecLen*math.cos(toRadians(self.end_effecAng))],
            [self.pos[3][1], self.pos[3][1]+self.end_effecLen*math.sin(toRadians(self.end_effecAng))],
            )
        self.ps_stuff["eeffec"][2].set_data( \
            [self.pos[3][0], self.pos[3][0]+self.end_effecLen*math.cos(toRadians(self.end_effecAng))], \
            2*[self.pos[3][1]]
            )
        self.ps_stuff["eeffec"][3].set_offsets(
            [self.pos[3][0]+self.end_effecLen*math.cos(toRadians(self.end_effecAng)),
            self.pos[3][1]+self.end_effecLen*math.sin(toRadians(self.end_effecAng))]
        )
        
        if self.currentIdx<round(len(self.allAbsAng)/2): coefsTempX = 2*[self.currentIdx]
        elif self.currentIdx>=round(len(self.allAbsAng)/2): coefsTempX = 2*[round(len(self.allAbsAng)) - self.currentIdx]
        self.ps_stuff["eeffec"][1].set_data(coefsTempX, [max(self.coefsY)+self.coefsLineCoef*len(self.coefsY), min(self.coefsY)-self.coefsLineCoef*len(self.coefsY)])

        for i in range(3):
            self.ps_updateText(i, [self.pos[i][0],self.pos[i][1],self.pos[i+1][0],self.pos[i+1][1]])
        self.ps_updateText(3, [self.pos[3][0],self.pos[3][1],self.pos[0][0],self.pos[0][1]], [0.1, -0.1])

        retur=[]
        for key,val in self.ps_stuff.items():
            for el in val: retur.append(el)
        return retur
        # return self.ps_stuff["frame"][0], self.ps_stuff["frame"][1], self.ps_stuff["frame"][2], self.ps_stuff["frame"][3], \
        #     self.ps_stuff["coefs"][0], self.ps_stuff["circle"][0], self.ps_stuff["circle"][1], self.ps_stuff["frame"][4]
    

if __name__=="__main__":
    a = AnimatedScatter()
    plt.show()
