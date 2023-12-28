#!/usr/bin/env python3

import math
import numpy as np

toRadians = lambda degrees: (degrees*np.pi)/180
toDegrees = lambda radians: (radians*180)/np.pi


class camTriangle(object):
    camPos = [[0, 0], [0, 0]]
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
    ang_read = [-1, -1]
    """The changing variable"""

    l_tri = [None, None]

    solved_pos = [-1, -1, -1]

    def __init__(self, camPos):
        self.camPos = camPos
        self.l_delta = [camPos[1][0]-camPos[0][0], camPos[1][1]-camPos[0][1]]
        self.l_hypotenuse = math.sqrt(self.l_delta[0]**2+self.l_delta[1]**2)
        self.ang_d = [toDegrees(math.atan(self.l_delta[1]/self.l_delta[0])), toDegrees(math.atan(self.l_delta[0]/self.l_delta[1]))]
        self.setCamCoef()
    def setCamCoef(self):
        self.camCoef[0][0] = lambda px: self.camFOV[0][0]/self.camRes[0][0]
        self.camCoef[0][1] = lambda px: self.camFOV[0][1]/self.camRes[0][1]
        self.camCoef[1][0] = lambda px: self.camFOV[1][0]/self.camRes[1][0]
        self.camCoef[1][1] = lambda px: self.camFOV[1][1]/self.camRes[1][1]
    def solveAngL(self, readAng):
        self.ang_read = readAng
        self.ang_tri = [
            self.ang_offset[0]-self.ang_d[0]-self.ang_read[0],
            self.ang_offset[1]-self.ang_d[1]-self.ang_read[1]
        ]
        self.ang_p = 180 - self.ang_tri[0] - self.ang_tri[1]
    def solvePos(self, readAng):
        self.solveAngL(readAng=readAng)
        self.l_tri = [
            (self.l_hypotenuse*math.sin(toRadians(self.ang_tri[0])))/math.sin(toRadians(self.ang_p)),
            (self.l_hypotenuse*math.sin(toRadians(self.ang_tri[1])))/math.sin(toRadians(self.ang_p))
        ]
        self.solved_pos = [
                math.cos(toRadians(self.ang_offset[0]+self.ang_read[0]))*self.l_tri[0]+self.camPos[0][0],
                math.sin(toRadians(self.ang_offset[0]+self.ang_read[0]))*self.l_tri[0]+self.camPos[0][1]
        ]
        return self.solved_pos
