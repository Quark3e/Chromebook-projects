#!/usr/bin/env python3

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.patches as mpatches
import numpy as np
import math
import itertools

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

def arc_patch(center, radius, theta1, theta2, ax=None, resolution=50, **kwargs):
    if ax is None:
        ax = plt.gca()
    
    theta = np.linspace(toRadians(theta1), np.radians(theta2), resolution)
    points = np.vstack((radius*np.cos(theta) + center[0],
                        radius*np.sin(theta) * center[1]))
    poly = mpatches.Polygon(points.T, closed=True, **kwargs)
    ax.add_patch(poly)
    return poly


class quadrilateral(object):
    a=1
    b=1     #constant (stays 1)
    c=1
    d=1    

    alpha=1
    beta=1
    gamma=1
    delta=1

    e=1    
    f=1    

    p=1    
    area=1
    gamma1=1
    gamma2=1

    absAng=0 #absolute angle of side c relative to horizontal axis

    isReachable=True
    def __init__(self):
        self.solveVars()
    def solve_e(self):
        self.e = math.sqrt(self.a**2 + self.b**2 - 2*self.a*self.b*math.cos(toRadians(self.beta)))
        return self.e
    def solve_f(self):
        self.solve_gamma()
        self.f = math.sqrt(self.b**2 + self.c**2 - 2*self.b*self.c*math.cos(toRadians(self.gamma)))
        return self.f
    def solve_gamma1(self):
        try:
            self.gamma1 = toDegrees(math.acos(round((self.b**2 + self.e**2 - self.a**2) / (2*self.b*self.e), 10)))
            return self.gamma1
        except ValueError:
            self.isReachable = False
            print("gamma1 ValueError.")
            return None
    def solve_gamma2(self):
        self.solve_e()
        try:
            self.gamma2 = toDegrees(math.acos(round((self.c**2 + self.e**2 - self.d**2) / (2*self.c*self.e), 10)))
            return self.gamma2
        except ValueError:
            self.isReachable = False
            print(f"gamma2 ValueError: [{(self.c**2 + self.e**2 - self.d**2) / (2*self.c*self.e)}]")
            return None
    def solve_gamma(self):
        self.solve_gamma2()
        self.solve_gamma1()
        self.gamma = self.gamma1 + self.gamma2
        return self.gamma
    def solve_d(self):
        self.solve_gamma2()
        self.d = math.sqrt(self.c**2 + self.e**2 - 2*self.c*self.e*math.cos(toRadians(self.gamma2)))
        return self.d
    def solve_alpha(self):
        self.solve_f()
        try:    
            self.alpha = toDegrees(math.acos((self.a**2 + self.d**2 - self.f**2) / (2*self.a*self.d)))
            return self.alpha
        except ValueError:
            self.isReachable = False
            print("alpha ValueError.")
            return None
    def solve_delta(self):
        self.solve_alpha()
        self.solve_gamma()
        self.delta = 360 - self.alpha - self.beta - self.gamma
        return self.delta
    def solve_perimeter(self):
        self.p = self.a + self.b + self.c + self.d
        return self.p
    def solve_area(self):
        try:
            self.area = math.sqrt(4*self.e**2*self.f**2 - (self.b**2 + self.d**2 - self.a**2 - self.c**2)**2)/4
            return self.area
        except ValueError:
            self.isReachable = False
            print("area ValueError")
            return None
    def solve_absAng(self):
        self.absAng = -(self.beta + self.gamma - 180)

    def solveVars(self):
        self.isReachable = True
        self.solve_e()
        self.solve_f()
        self.solve_delta()
        self.solve_perimeter()
        self.solve_area()
        self.solve_absAng()

    def set_a(self, a):
        self.a = a
        self.solveVars()
    def set_b(self, b):
        self.b = b
        self.solveVars()
    def set_c(self, c):
        self.c = c
        self.solveVars()
    def set_d(self, d):
        self.d = d
        self.solveVars()
    def set_beta(self, beta):
        self.beta = beta
        self.solveVars()
    def set_gamma(self, gamma):
        self.gamma = gamma
        self.solveVars()


class AnimatedScatter(object):
    def __init__(self):
        self.quad = quadrilateral()

        self.l0 = 1
        self.l1 = 1.1
        self.gd = 0.1
        self.td = 0.1
        self.end_effecLen = 1
        self.end_effecAng = 0

        self.angle = 45

        self.currentIdx = 0

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

        for dir in range(-1, 2, 2):
            for q in range(90+90*(-dir), 90+90*dir+dir, dir):
                self.quad.set_beta(180-q)
                tempPos = [
                            [0-self.gd, 0], #alpha
                            [0, 0], #beta
                            [self.l0*math.cos(toRadians(180-self.quad.beta)), self.l0*math.sin(toRadians(180-self.quad.beta))], #gamma
                            [self.l1*math.cos(toRadians(self.quad.alpha))-self.gd, self.l1*math.sin(toRadians(self.quad.alpha))] #delta
                        ]
                #pos = [self.pos[2][0], self.pos[2][1], self.pos[3][0], self.pos[3][1]]
                #round(math.sqrt((pos[2]-pos[0])**2 + (pos[3]-pos[1])**2), 2)
                # print(round(math.sqrt((tempPos[3][0]-tempPos[2][0])**2 + (tempPos[3][1]-tempPos[2][1])**2),1))
                
                # if round(math.sqrt((tempPos[1][0]-tempPos[0][0])**2 + (tempPos[1][1]-tempPos[0][1])**2),1) == self.gd and \
                #     round(math.sqrt((tempPos[2][0]-tempPos[1][0])**2 + (tempPos[2][1]-tempPos[1][1])**2),1) == self.l0 and \
                #     round(math.sqrt((tempPos[3][0]-tempPos[2][0])**2 + (tempPos[3][1]-tempPos[2][1])**2),1) == self.td and \
                #     round(math.sqrt((tempPos[0][0]-tempPos[3][0])**2 + (tempPos[0][1]-tempPos[3][1])**2),1) == self.l1: print("is reachable:", end=" ")
                if self.quad.isReachable and \
                    round(math.sqrt((tempPos[1][0]-tempPos[0][0])**2 + (tempPos[1][1]-tempPos[0][1])**2),3) == self.gd and \
                    round(math.sqrt((tempPos[2][0]-tempPos[1][0])**2 + (tempPos[2][1]-tempPos[1][1])**2),3) == self.l0 and \
                    round(math.sqrt((tempPos[3][0]-tempPos[2][0])**2 + (tempPos[3][1]-tempPos[2][1])**2),3) == self.td and \
                    round(math.sqrt((tempPos[0][0]-tempPos[3][0])**2 + (tempPos[0][1]-tempPos[3][1])**2),3) == self.l1 and \
                    not (tempPos[3][0]<(-self.l1*0.9) and tempPos[2][0]<(-self.l1*0.9) and tempPos[3][1]>tempPos[2][1]) and \
                    tempPos[3][1]+self.end_effecLen*math.sin(toRadians(self.quad.absAng))>=0:
                    self.allPos.append(tempPos)
                    
                    self.allAbsAng.append(self.quad.absAng)

                    self.allEndEffec_pos[0].append(tempPos[3][0]+self.end_effecLen*math.cos(toRadians(self.quad.absAng)))
                    self.allEndEffec_pos[1].append(tempPos[3][1]+self.end_effecLen*math.sin(toRadians(self.quad.absAng)))

        print("number of possible positions:", len(self.allPos))
        if len(self.allPos)<=0: exit()

        self.graphRange = {
            "frame": [[-(self.l1+self.gd+0.1), 2],[-2, 2]],
            "coefs": [-2, 2],
        }

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
        self.ps_stuff.update({"frame": 8*[0]})
        self.ps_stuff.update({"coefs": 0*[0]})
        self.ps_stuff.update({"circle": 0*[0]})
        self.ps_stuff.update({"text": 4*[0]})
        self.ps_stuff.update({"eeffec": 3*[0]}) #end-effector

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
                yield i
    def setup_plot(self):
        next(self.stream)

        self.ax["frame"].plot(self.allEndEffec_pos[0], self.allEndEffec_pos[1], color="r", alpha=0.6,label="path")
        self.ax["coefs"].plot([i for i in range(round(len(self.allAbsAng)/2))], self.allAbsAng[:round(len(self.allAbsAng)/2)], color="r", linewidth=1)
        self.ps_stuff["eeffec"][1], = self.ax["coefs"].plot(2*[0], [max(self.allAbsAng)+1, min(self.allAbsAng)-1], label="absAngles", color="g", linewidth=2.5, alpha=0.5)

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
        
        if self.currentIdx<round(len(self.allAbsAng)/2): coefsTempX = 2*[self.currentIdx]
        elif self.currentIdx>=round(len(self.allAbsAng)/2): coefsTempX = 2*[round(len(self.allAbsAng)) - self.currentIdx]
        self.ps_stuff["eeffec"][1].set_data(coefsTempX, [max(self.allAbsAng)+1, min(self.allAbsAng)-1])

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
