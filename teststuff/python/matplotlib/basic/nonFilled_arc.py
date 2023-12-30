#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
from math import sin, cos, tan, asin, acos, atan

toRadians = lambda degrees: (degrees*np.pi)/180
toDegrees = lambda radians: (radians*180)/np.pi

class drawArc(object):
    """Draw an arc on plot axis/figure
    """

    cent = [0, 0]
    """Point position of where arc sides converge"""
    radius = 1
    """Arc side length"""
    arcAngle = 30
    arcOffsetAngle = 0
    drawRadiuss = False
    drawRadiusExt = 1

    resolution = 10

    arc_xValues = []
    arc_yValues = []
    axesPlot = None
    ax = None
    label=""

    def __init__(
            self,
            ax=plt.Axes,
            centerPos=float([float(), float()]),
            arcRadius=1.0,
            startAngle=30.0,
            arcOffset=0.0,
            drawSides=False,
            res=10,
            label="",
            radiusExtended=0.0,
        ):
        """Initialization function of class

        Args:
            ax (<class 'matplotlib.axes._axes.Axes'>): matplotlib axes where the arc will be drawn
            centerPos (list): XY list of where the arc's sides converge/meet, i.e. the center position. Defaults to [0, 0].
            arcRadius (int, float): Radius of the arc. Defaults to 1.
            startAngle (int, float): Angle of the arc [unit:Degrees]. Defaults to 30.
            arcOffset (int, float): Offset angle from x axis [unit:Degrees]. Defaults to 0.
            drawSides (bool): Whether to plot the side lengths of the arc. Defaults to False.
            res (int): Resolution of drawn arc: how many points to be used. Defaults to 10.
            label (string): label of the plotted arc. Defaults to "".
            radiusExtended (int, float): extended length from radius if drawSides==True, Defaults to 0
        """

        self.ax = ax
        self.arcConvPos = centerPos
        self.radius = arcRadius
        self.arcAngle = startAngle
        self.arcOffsetAngle = arcOffset
        self.drawRadiuss = drawSides
        self.resolution = res
        self.label = label
        self.drawRadiusExt = radiusExtended
        self.setup()
    def solveValues(self):
        if self.drawRadiuss:
            self.arc_xValues = [
                self.cent[0],
                self.cent[0]+self.radius*cos(toRadians(self.arcOffsetAngle))
            ]
            self.arc_yValues = [
                self.cent[1],
                self.cent[1]+self.radius*sin(toRadians(self.arcOffsetAngle))
            ]
            for i in range(self.resolution):
                tempAng = (self.arcAngle/self.resolution)*i
                self.arc_xValues.append(self.cent[0]+self.radius*cos(toRadians(self.arcOffsetAngle+tempAng)))
                self.arc_yValues.append(self.cent[1]+self.radius*sin(toRadians(self.arcOffsetAngle+tempAng)))


    def setup(self):
        self.solveValues()
        self.axesPlot = self.ax.plot(
            []
        )

    def update(self, angle, offsetAngle=cent):
        self.arcAngle = angle
        self.cent = offsetAngle
        self.solveValues()

        
