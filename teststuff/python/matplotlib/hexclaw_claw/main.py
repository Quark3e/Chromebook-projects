#!/usr/bin/env python3

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import math

from matplotlib.patches import Arc

from AngleAnnotation import AngleAnnotation

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


class AnimatedScatter(object):
    def __init__(self):

        self.iterLen = 50
        self.iterSpac=1
        self.Ld = 0
        self.Lambda = 0
        self.Mu = 0
        self.angle = [None, None] #[a0, a1]
        self.arm = [8,25]

        simpleOff = [[3,3], [0,0]]

        self.startPos = [self.arm[1]-self.arm[0]+simpleOff[0][0], 20+simpleOff[1][0]]
        self.endPos = [self.arm[1]-self.arm[0]+simpleOff[0][1], 0+simpleOff[1][1]]
        self.pos = self.startPos.copy()

        #self.values = []

        self.values = [
            [self.startPos[0]+((self.endPos[0]-self.startPos[0])*n)/self.iterLen, 
            self.startPos[1]+((self.endPos[1]-self.startPos[1])*n)/self.iterLen] \
             for n in range(self.iterLen)
        ]
        self.values = completeList_2d(self.values, 60)


        #self.values = []
        self.maxSquare = math.sqrt((sum(self.arm)**2)/2)
        self.minCoef = ( abs(self.arm[1]-self.arm[0]) / self.maxSquare)
        self.maxCoef = sum(self.arm)/self.maxSquare


        #self.shape_square(sideCoef=[math.ceil((self.arm[1]-self.arm[0])/sum(self.arm)), 1, math.ceil((self.arm[1]-self.arm[0])/sum(self.arm)), 0])
        #self.shape_circle(sideCoef=[self.minCoef, self.maxCoef-0.001, self.minCoef, self.maxCoef-0.001])
        #self.shape_circle(sideCoef=[self.maxCoef-0.001, self.minCoef, self.maxCoef-0.001, self.minCoef])

        #for q in range(180):
        #    self.values.append([math.sin(toRadians(q))*25,math.cos(toRadians(q))*25])
        #self.values = completeList_2d(self.values, 100)
        self.iterLen = len(self.values)

        self.pos0 = [0, 0]
        self.pos1 = [0, 0]


        #(min([n[0] for n in self.values if n[0]<sum(self.arm)])-min(self.arm)
        self.graphRange = {
            "frame": [ \
                (-min(self.arm), max([n[0] for n in self.values if n[0]<sum(self.arm)])+min(self.arm)), \
                (min([n[1] for n in self.values if n[1]<sum(self.arm)])-min(self.arm), max([n[1] for n in self.values if n[1]<sum(self.arm)])+min(self.arm)) \
            ],
            "coefs": [self.iterLen, 10],
        }
        print(self.graphRange)
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

        self.ax["frame"].set_xlim(self.graphRange["frame"][0])
        self.ax["frame"].set_ylim(self.graphRange["frame"][1])
        # self.ax["frame"].axis("equal")
        self.ax["frame"].set_aspect("equal")

        self.ps_stuff = {}
        self.ps_stuff.update({"frame": 10*[0]})
        self.ps_stuff.update({"coefs": 2*[0]})
        self.ps_stuff.update({"circle": 4*[0]})
        self.ps_stuff.update({"arc": 1*[0]})

        self.ani = animation.FuncAnimation( \
            self.fig, self.update, interval=1, \
            init_func=self.setup_plot, blit=False \
        )
    def shape_square(self, sideCoef=[1,1,1,1]):
        #bool[-x:x, y:-y, x:-x, -y:y] bool[y, x, y, x]
        maxAx = math.floor(math.sqrt((sum(self.arm)**2)/2))
        if sideCoef[0]>0:
            for x in range(-round(sideCoef[3]*maxAx),round(sideCoef[1]*maxAx)+1, 1): self.values.append([x, round(maxAx*sideCoef[0])])
        if sideCoef[1]>0:
            for y in range(round(sideCoef[0]*maxAx),-round(sideCoef[2]*maxAx)-1,-1): self.values.append([round(maxAx*sideCoef[1]), y])
        if sideCoef[2]>0:
            for x in range(round(sideCoef[1]*maxAx),-round(sideCoef[3]*maxAx)-1,-1): self.values.append([x,-round(maxAx*sideCoef[0])])
        if sideCoef[3]>0:
            for y in range(round(sideCoef[2]*maxAx),-round(sideCoef[0]*maxAx)-1,-1): self.values.append([-round(maxAx*sideCoef[1]),y])
    def shape_circle(self, sideCoef=[1,1,1,1], check=[1,1,0,0]):
        #[0:90, 90:180, 180:270, 270:360]
        maxAx = math.sqrt((sum(self.arm)**2)/2)
        for q in range(360):
            if q>=0 and q<90 and check[0]:    self.values.append([math.sin(toRadians(q))*maxAx*sideCoef[1],math.cos(toRadians(q))*maxAx*sideCoef[0]])
            if q>=90 and q<180 and check[1]:  self.values.append([math.sin(toRadians(q))*maxAx*sideCoef[1],math.cos(toRadians(q))*maxAx*sideCoef[0]])
            if q>=180 and q<270 and check[2]: self.values.append([math.sin(toRadians(q))*maxAx*sideCoef[3],math.cos(toRadians(q))*maxAx*sideCoef[2]])
            if q>=270 and q<360 and check[3]: self.values.append([math.sin(toRadians(q))*maxAx*sideCoef[3],math.cos(toRadians(q))*maxAx*sideCoef[2]])

    def getAngles(self, pos):
        angles = [0.1, 0.1]
        try:
            self.Ld = math.sqrt(pos[0]**2+pos[1]**2)
            angles[1] = -math.acos(
                (self.Ld**2-self.arm[0]**2-self.arm[1]**2) / \
                (2*self.arm[0]*self.arm[1])
            )
            # self.Lambda = math.atan(self.pos[1]/self.pos[0])
            self.Lambda = math.asin(pos[1]/self.Ld)
            self.Mu = (math.atan(
                (self.arm[1]*math.sin(-angles[1])) /
                (self.arm[0] + self.arm[1]*math.cos(-angles[1]))
            ))
            if self.Mu<0: self.Mu = np.pi+self.Mu
            angles[1] = angles[1]
            angles[0] = (self.Lambda + self.Mu)
            #print(f"[ Ld:{round(self.Ld)} lamb:{round(toDegrees(self.Lambda))} mu:{round(toDegrees(self.Mu))} a0:{round(toDegrees(angles[0]))} a1:{round(toDegrees(angles[1]))} ]", end=" | \n")
        except ValueError:
            print("ValueError")
        return angles
    def data_stream(self):
        findVal = lambda x: x/abs(x)
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
            plt.Circle((0, 0), self.arm[0], color="b", fill=False, alpha=0.5),
            plt.Circle((self.arm[0]*math.cos(self.angle[0]), self.arm[0]*math.sin(self.angle[0])), self.arm[1], color="b", fill=False, alpha=0.5),
            plt.Circle((0, 0), sum(self.arm), color="g", fill=False, alpha=0.25),
            plt.Circle((0, 0), self.arm[1]-self.arm[0], color="g", fill=False, alpha=0.25),
        ]
        for _ in self.ps_stuff["circle"]: self.ax["frame"].add_patch(_)


        #print(self.values)
        self.xCoefs = [n for n in range(self.iterLen)]
        self.yCoefs = [] #[self.getAngles(pos)[1]/self.getAngles(pos)[0] for pos in self.values]
        for pos in self.values:
            self.angle = self.getAngles(pos=pos)
            try:
                coef = toDegrees(self.angle[1])/toDegrees(self.angle[0]+90)
            except ZeroDivisionError:
                print("zero division error:", toDegrees(self.angle[0]+90))
                coef = 0
            if coef>100: coef=0
            self.yCoefs.append(coef)

            #print(pos, [round(toDegrees(self.angle[0])),round(toDegrees(self.angle[1]))], self.angle[1]/self.angle[0])

        # for i in range(len(self.xCoefs)):
        #     print(self.xCoefs[i], self.yCoefs[i])

        # temp = np.poly1d(np.polyfit(self.xCoefs, self.yCoefs, 10))
        # self.yCoefs = [temp(n) for n in self.xCoefs]
        self.ps_stuff["coefs"][1] = self.ax["coefs"].scatter(self.xCoefs, self.yCoefs, label="raw")
        self.coefsLim = [min(self.yCoefs)-0.01, max(self.yCoefs)+0.01]
        for i in [2, 3, 6, 8, 10]:
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


        # self.ax["frame"].legend(bbox_to_anchor=(-0.1, 1.05))
        self.ax["frame"].legend(loc="lower left")

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
