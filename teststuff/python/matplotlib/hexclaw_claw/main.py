#!/usr/bin/env python3

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import math

from matplotlib.patches import Arc

from AngleAnnotation import AngleAnnotation

toRadians = lambda degrees: (degrees*np.pi)/180
toDegrees = lambda radians: (radians*180)/np.pi

class AnimatedScatter(object):
    def __init__(self):

        self.iterLen = 100
        self.iterSpac=1
        self.Ld = 0
        self.Lambda = 0
        self.Mu = 0
        self.angle = [None, None] #[a0, a1]
        self.arm = [8, 25]
        self.startPos = [self.arm[1]-self.arm[0], 20]
        self.endPos = [self.arm[1]-self.arm[0], -20]
        self.pos = self.startPos.copy()

        self.values = [
            [self.startPos[0]+((self.endPos[0]-self.startPos[0])*n)/self.iterLen, 
            self.startPos[1]+((self.endPos[1]-self.startPos[1])*n)/self.iterLen] \
             for n in range(self.iterLen)
        ]

        self.pos0 = [0, 0]
        self.pos1 = [0, 0]

        self.graphRanges = {
            "frame": (max([self.startPos[0], self.endPos[0]])+5, max([self.startPos[1], self.endPos[1]])+5),
            "coefs": None,
        }
        print(self.graphRanges)
        self.stream = self.data_stream()
        self.fig = plt.figure(figsize=(15,7))
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

        self.ax["frame"].set_xlim([-self.graphRanges["frame"][0]*0.5, self.graphRanges["frame"][0]])
        self.ax["frame"].set_ylim([-self.graphRanges["frame"][1]+0.5, self.graphRanges["frame"][1]])
        # self.ax["frame"].axis("equal")
        self.ax["frame"].set_aspect("equal")

        self.ps_stuff = {}
        self.ps_stuff.update({"frame": 10*[0]})
        self.ps_stuff.update({"coefs": 1*[0]})
        self.ps_stuff.update({"circle": 2*[0]})
        self.ps_stuff.update({"arc": 1*[0]})

        self.ani = animation.FuncAnimation( \
            self.fig, self.update, interval=1, \
            init_func=self.setup_plot, blit=False \
        )
    def getAngles(self, pos):
        angles = [0.1, 0.1]
        try:
            self.Ld = math.sqrt(self.pos[0]**2+self.pos[1]**2)
            angles[1] = math.acos(
                (self.Ld**2-self.arm[0]**2-self.arm[1]**2) / \
                (2*self.arm[0]*self.arm[1])
            )
            # self.Lambda = math.atan(self.pos[1]/self.pos[0])
            self.Lambda = math.asin(self.pos[1]/self.Ld)
            self.Mu = (math.atan(
                (self.arm[1]*math.sin(angles[1])) /
                (self.arm[0] + self.arm[1]*math.cos(angles[1]))
            ))
            if self.Mu<0: self.Mu = np.pi+self.Mu
            angles[1] = -angles[1]
            angles[0] = (self.Lambda + self.Mu)
            # print(f"[ Ld:{round(self.Ld)} lamb:{round(toDegrees(self.Lambda))} mu:{round(toDegrees(self.Mu))} a0:{round(toDegrees(angles[0]))} a1:{round(toDegrees(angles[1]))} ]", end=" | \n")
        except ValueError:
            print("ValueError")
        return angles
    def data_stream(self):
        count = 0
        countDir = -1 #-1, 1
        countRange = round((self.startPos[1]-self.endPos[1])/2)
        # while True:
        #     self.pos[1] = countRange+count
        #     count+=countDir
        #     if abs(count)>=countRange: countDir = countDir*-1
        #     self.angle = self.getAngles(self.pos)
        #     yield self.pos[1]
        findVal = lambda x: x/abs(x)
        # xValues = [self.startPos[0]+x for x in range(self.startPos[0], self.endPos[0], findVal(self.endPos[0]-self.startPos[0]))]
        # yValues = [self.startPos[1]+y for y in range(self.startPos[1], self.endPos[1], findVal(self.endPos[1]-self.startPos[1]))]
        # for n in values: print(n)
        # print(len(values))
        self.nCount = 0
        while True:
            for n in range(0, self.iterLen, self.iterSpac):
                self.pos = self.values[n]
                self.angle = self.getAngles(self.pos)
                self.nCount = n
                yield self.pos[1]
            for n in range(self.iterLen-1, -1, -1*self.iterSpac):
                self.pos = self.values[n]
                self.angle = self.getAngles(self.pos)
                self.nCount = n
                yield self.pos[1]

    def setup_plot(self):
        next(self.stream)

        self.ps_stuff["frame"][5] = self.ax["frame"].scatter([self.pos[0]],[self.pos[1]],label="Pos")

        self.pos0 = [self.arm[0]*math.cos(self.angle[0])], [self.arm[0]*math.sin(self.angle[0])]
        self.pos1 = [self.pos0[0][0]+self.arm[1]*math.cos(self.angle[0]+self.angle[1])], [self.pos0[1][0]+self.arm[1]*math.sin(self.angle[0]+self.angle[1])]

        # self.ps_stuff["frame"][6] = AngleAnnotation( \
        #     (0, 0), \
        #     (self.arm[0], 0), (self.pos0[0][0],self.pos0[1][0]), ax=self.ax["frame"], size=50, text=str(round(toDegrees(self.angle[0]),2)), \
        #     textposition='outside'
        # )

        self.ax["frame"].plot([el[0] for el in self.values],[el[1] for el in self.values],alpha=0.6,color="r",label="path")

        self.ps_stuff["frame"][0] = self.ax["frame"].scatter(self.pos0[0], self.pos0[1], edgecolor="k", label="p0")
        self.ps_stuff["frame"][1] = self.ax["frame"].scatter(self.pos1[0], self.pos1[1], edgecolor="k", label="p1")

        self.ps_stuff["frame"][2], = self.ax["frame"].plot( \
            [0, self.arm[0]*math.cos(self.angle[0])], \
            [0, self.arm[0]*math.sin(self.angle[0])], \
            label="arm0"
        )
        self.ps_stuff["frame"][3], = self.ax["frame"].plot( \
            [self.arm[0]*math.cos(self.angle[0]), self.Ld*math.cos(self.Lambda)], \
            [self.arm[0]*math.sin(self.angle[0]), self.Ld*math.sin(self.Lambda)], \
            label="arm1"
        )
        
        self.ps_stuff["frame"][7], = self.ax["frame"].plot([0, self.pos1[0][0]], [0, self.pos1[1][0]], label="Ld", alpha=0.2)

        self.ps_stuff["arc"] = [
            Arc((0,0), 2, 2, toDegrees(self.angle[0]), toDegrees(2*np.pi-self.angle[0]))
        ]
        self.ax["frame"].add_patch(self.ps_stuff["arc"][0])

        self.ps_stuff["circle"] = [
            plt.Circle((0, 0), self.arm[0], color="b", fill=False),
            plt.Circle((self.arm[0]*math.cos(self.angle[0]), self.arm[0]*math.sin(self.angle[0])), self.arm[1], color="b", fill=False),
        ]
        for _ in self.ps_stuff["circle"]: self.ax["frame"].add_patch(_)


        #print(self.values)
        self.xCoefs = [n for n in range(self.iterLen)]
        self.yCoefs = [] #[self.getAngles(pos)[1]/self.getAngles(pos)[0] for pos in self.values]
        for pos in self.values:
            next(self.stream)
            self.yCoefs.append(self.angle[1]/self.angle[0])
            #print(pos, [round(toDegrees(self.angle[0])),round(toDegrees(self.angle[1]))], self.angle[1]/self.angle[0])

        self.ax["coefs"].scatter(self.xCoefs, self.yCoefs, label="raw")
        self.coefsLim = [min(self.yCoefs)-0.1, max(self.yCoefs)+0.1]
        for i in [2, 3, 4]:
            polyModel = np.poly1d(np.polyfit(self.xCoefs,self.yCoefs,i))
            self.ax["coefs"].plot(self.xCoefs, [polyModel(x) for x in self.xCoefs], label=f"p{i}")
        self.ax["coefs"].legend()

        self.ps_stuff["coefs"][0], = self.ax["coefs"].plot([0, 0],self.coefsLim)


        self.checkLen=0
        try:
            self.checkLen = math.sqrt((self.pos1[1][0]-self.pos0[1][0])/(self.pos1[0][0]-self.pos0[0][0]))
        except ValueError:
            pass
        self.ps_stuff["frame"][4] = self.ax["frame"].text( \
            self.pos1[0][0]-self.pos0[0][0]+5,self.pos1[1][0]-self.pos0[1][0]+5, \
            "len:"+str(round(self.checkLen))
        )

        self.ps_stuff["frame"][8] = self.ax["frame"].text(-5, 0, f"{round(toDegrees(self.angle[0]),2)}")
        self.ps_stuff["frame"][9] = self.ax["frame"].text(self.pos0[0][0]-1, self.pos0[1][0]+2, f"{round(toDegrees(self.angle[1]),2)}")


        self.ax["frame"].legend(bbox_to_anchor=(-0.1, 1.05))

        retur=[]
        for key,val in self.ps_stuff.items():
            for el in val: retur.append(el)
        return retur
    def update(self, i):
        next(self.stream)

        # self.ps_stuff["frame"][6].set_theta(toDegrees(self.angle[0]))
        # self.ps_stuff["frame"][6].update_text()

        self.pos0 = [self.arm[0]*math.cos(self.angle[0])], [self.arm[0]*math.sin(self.angle[0])]
        self.pos1 = [self.pos0[0][0]+self.arm[1]*math.cos(self.angle[0]+self.angle[1])], [self.pos0[1][0]+self.arm[1]*math.sin(self.angle[0]+self.angle[1])]

        # print(self.pos, f" | [{round(self.pos0[0][0])},{round(self.pos0[1][0])}] [{round(self.pos1[0][0])},{round(self.pos1[1][0])}]", end=" | ")
        # print([round(n[0],2) for n in self.pos0],[round(n[0],2) for n in self.pos1], end=" | ")

        try:
            self.checkLen = math.sqrt((self.pos1[1][0]-self.pos0[1][0])**2+(self.pos1[0][0]-self.pos0[0][0])**2)
            # print(round(self.checkLen))
        except ValueError:
            pass

        self.ps_stuff["frame"][7].set_data([0, self.pos1[0][0]], [0, self.pos1[1][0]])

        self.ps_stuff["frame"][5].set_offsets([self.pos[0], self.pos[1]])

        self.ps_stuff["frame"][4].set_text(f"n:{self.nCount}")
        self.ps_stuff["frame"][4].set_x(self.pos1[0][0]-1)
        self.ps_stuff["frame"][4].set_y(self.pos1[1][0]+5)

        self.ps_stuff["frame"][8].set_text(f"{round(toDegrees(self.angle[0]),2)}")
        self.ps_stuff["frame"][9].set_text(f"{round(toDegrees(self.angle[1]),2)}")
        self.ps_stuff["frame"][9].set_x(self.pos0[0][0]-1)
        self.ps_stuff["frame"][9].set_y(self.pos0[1][0]+2)


        self.ps_stuff["frame"][0].set_offsets([self.pos0[0][0], self.pos0[1][0]])
        self.ps_stuff["frame"][1].set_offsets([self.pos1[0][0], self.pos1[1][0]])
        self.ps_stuff["frame"][2].set_data([0, self.arm[0]*math.cos(self.angle[0])], \
                                           [0, self.arm[0]*math.sin(self.angle[0])])
        self.ps_stuff["frame"][3].set_data([self.pos0[0][0], self.pos1[0][0]], \
                                           [self.pos0[1][0], self.pos1[1][0]])

        self.ps_stuff["coefs"][0].set_data([self.nCount, self.nCount],self.coefsLim)

        self.ps_stuff["arc"][0].set(angle=toDegrees(self.angle[0]))
            #Arc((0,0), 2, 2, toDegrees(self.angle[0]), toDegrees(2*np.pi-self.angle[0]))

        self.ps_stuff["circle"][0].center = (0, 0)
        self.ps_stuff["circle"][1].center = (self.arm[0]*math.cos(self.angle[0]), self.arm[0]*math.sin(self.angle[0]))


        retur=[]
        for key,val in self.ps_stuff.items():
            for el in val: retur.append(el)
        return retur
        # return self.ps_stuff["frame"][0], self.ps_stuff["frame"][1], self.ps_stuff["frame"][2], self.ps_stuff["frame"][3], \
        #     self.ps_stuff["coefs"][0], self.ps_stuff["circle"][0], self.ps_stuff["circle"][1], self.ps_stuff["frame"][4]
    

if __name__=="__main__":
    a = AnimatedScatter()
    plt.show()
