#!/usr/bin/env python3

import matplotlib.pyplot as plt
import matplotlib.animation as animation
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
            self.gamma1 = toDegrees(math.acos((self.b**2 + self.e**2 - self.a**2) / (2*self.b*self.e)))
            return self.gamma1
        except ValueError:
            print("gamma1 ValueError.")
            return None
    def solve_gamma2(self):
        self.solve_e()
        try:
            self.gamma2 = toDegrees(math.acos((self.c**2 + self.e**2 - self.d**2) / (2*self.c*self.e)))
            return self.gamma2
        except ValueError:
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
            print("area ValueError")
            return None
    def solveVars(self):
        self.solve_e()
        self.solve_f()
        self.solve_delta()
        self.solve_perimeter()
        self.solve_area()

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
        self.gd = 0.5
        self.td = 0.5

        self.angle = 90
        
        self.quad.set_a(self.gd)
        self.quad.set_b(self.l0)
        self.quad.set_c(self.td)
        self.quad.set_d(self.l1)
        self.quad.set_beta(180-self.angle)

        self.pos = [
            [0-self.gd, 0], #alpha
            [0, 0], #beta
            [self.l0*math.cos(toRadians(180-self.quad.beta)), self.l0*math.sin(toRadians(180-self.quad.beta))],  #gamma
            [self.l1*math.cos(toRadians(self.quad.alpha))-self.gd, self.l1*math.sin(toRadians(self.quad.alpha))] #delta
        ]

        self.allPos = []


        for dir in range(-1, 2, 2):
            for q in range(45+45*(-dir), 45+45*dir+dir, dir):
                self.quad.set_beta(180-q)
                if not None in [self.quad.beta, self.quad.alpha, self.quad.area]:
                    self.allPos.append(
                        [
                            [0-self.gd, 0], #alpha
                            [0, 0], #beta
                            [self.l0*math.cos(toRadians(180-self.quad.beta)), self.l0*math.sin(toRadians(180-self.quad.beta))], #gamma
                            [self.l1*math.cos(toRadians(self.quad.alpha))-self.gd, self.l1*math.sin(toRadians(self.quad.alpha))] #delta
                        ]
                    )
                    # print(self.quad.alpha)

        self.graphRange = {
            "frame": [[-2, 2],[-2, 2]],
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

        self.ani = animation.FuncAnimation( \
            self.fig, self.update, interval=1, \
            init_func=self.setup_plot, blit=True \
        )
    def data_stream(self):
        while True:
            for pos in self.allPos:
                self.pos=pos
                yield pos
    def setup_plot(self):
        next(self.stream)

        self.ps_stuff["frame"][0] = self.ax["frame"].scatter([self.pos[0][0]],[self.pos[0][1]],edgecolor="k",label="alpha")
        self.ps_stuff["frame"][1] = self.ax["frame"].scatter([self.pos[1][0]],[self.pos[1][1]],edgecolor="k",label="beta")
        self.ps_stuff["frame"][2] = self.ax["frame"].scatter([self.pos[2][0]],[self.pos[2][1]],edgecolor="k",label="gamma")
        self.ps_stuff["frame"][3] = self.ax["frame"].scatter([self.pos[3][0]],[self.pos[3][1]],edgecolor="k",label="delta")
        self.ps_stuff["frame"][4], = self.ax["frame"].plot([self.pos[0][0],self.pos[1][0]],[self.pos[0][1],self.pos[1][1]],label="a")
        self.ps_stuff["frame"][5], = self.ax["frame"].plot([self.pos[1][0],self.pos[2][0]],[self.pos[1][1],self.pos[2][1]],label="b")
        self.ps_stuff["frame"][6], = self.ax["frame"].plot([self.pos[2][0],self.pos[3][0]],[self.pos[2][1],self.pos[3][1]],label="c")
        self.ps_stuff["frame"][7], = self.ax["frame"].plot([self.pos[3][0],self.pos[0][0]],[self.pos[3][1],self.pos[0][1]],label="d")

        # self.ax["frame"].legend(loc="lower left")
        # self.ax["coefs"].legend()

        retur=[]
        for key,val in self.ps_stuff.items():
            for el in val: retur.append(el)
        return retur
    def update(self, i):
        next(self.stream)

        # print([self.pos],[self.pos])
        self.ps_stuff["frame"][0].set_offsets([[self.pos[0][0],self.pos[0][1]]])
        self.ps_stuff["frame"][1].set_offsets([[self.pos[1][0],self.pos[1][1]]])
        self.ps_stuff["frame"][2].set_offsets([[self.pos[2][0],self.pos[2][1]]])
        self.ps_stuff["frame"][3].set_offsets([[self.pos[3][0],self.pos[3][1]]])
        self.ps_stuff["frame"][4].set_data([self.pos[0][0],self.pos[1][0]],[self.pos[0][1],self.pos[1][1]])
        self.ps_stuff["frame"][5].set_data([self.pos[1][0],self.pos[2][0]],[self.pos[1][1],self.pos[2][1]])
        self.ps_stuff["frame"][6].set_data([self.pos[2][0],self.pos[3][0]],[self.pos[2][1],self.pos[3][1]])
        self.ps_stuff["frame"][7].set_data([self.pos[3][0],self.pos[0][0]],[self.pos[3][1],self.pos[0][1]])

        retur=[]
        for key,val in self.ps_stuff.items():
            for el in val: retur.append(el)
        return retur
        # return self.ps_stuff["frame"][0], self.ps_stuff["frame"][1], self.ps_stuff["frame"][2], self.ps_stuff["frame"][3], \
        #     self.ps_stuff["coefs"][0], self.ps_stuff["circle"][0], self.ps_stuff["circle"][1], self.ps_stuff["frame"][4]
    

if __name__=="__main__":
    a = AnimatedScatter()
    plt.show()
