#!/usr/bin/env python3

import time
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
    plotPara = {
        "label": "",
        "linewidth": 1,
        "alpha": 1,
        "linestyle": "solid",
        "color": "black"
    }

    def __init__(
            self,
            ax=plt.Axes,
            centerPos=[float(), float()],
            arcRadius=1.0,
            startAngle=30.0,
            arcOffset=0.0,
            drawSides=False,
            radiusExtended=0.0,
            res=10,
            plotLabel=plotPara["label"],
            plotLinewidth=plotPara["linewidth"],
            plotAlpha=plotPara["alpha"],
            plotLinestyle=plotPara["linestyle"],
            plotColor=plotPara["color"]
        ):
        """Initialization function of class

        Args:
            ax (<class 'matplotlib.axes._axes.Axes'>): matplotlib axes where the arc will be drawn
            centerPos (list): XY list of where the arc's sides converge/meet, i.e. the center position. Defaults to [0, 0].
            arcRadius (int, float): Radius of the arc. Defaults to 1.
            startAngle (int, float): Angle of the arc [unit:Degrees]. Defaults to 30.
            arcOffset (int, float): Offset angle from x axis [unit:Degrees]. Defaults to 0.
            drawSides (bool): Whether to plot the side lengths of the arc. Defaults to False.
            radiusExtended (int, float): extended length from radius if drawSides==True, Defaults to 0
            res (int): Resolution of drawn arc: how many points to be used. Defaults to 10.
            plotLabel (string): label of the plotted arc. Defaults to "".
            plotLinewidth (int, float): linewdith of plotted arc. Defaults to 1.
            plotAlpha (int, float): alpha value of plotted arc. Defaults to 1.
            plotLinestyle (string,): plot-parameter for linestyle of the plotted arc. Defaults to "solid"
            plotColor (string,): plot color. Default to "black".
        """

        self.ax = ax
        self.arcConvPos = centerPos
        self.cent = centerPos
        
        if arcRadius>0: self.radius = arcRadius
        else: self.throwErr("__init__: 'arcRadius' can't be <=0")

        self.arcAngle = startAngle
        self.arcOffsetAngle = arcOffset
        self.drawRadiuss = drawSides
        
        if res>=1: self.resolution = res
        else: self.throwErr("__init__: 'res' parameter argument too low")

        self.plotPara["label"] = plotLabel
        self.plotPara["linewidth"] = plotLinewidth
        self.plotPara["alpha"] = plotAlpha
        self.plotPara["linestyle"] = plotLinestyle
        self.plotPara["color"] = plotColor
        self.drawRadiusExt = radiusExtended
    def solveValues(self):
        if self.drawRadiuss:
            self.arc_xValues = [
                self.cent[0],
                self.cent[0]+(self.radius+self.drawRadiusExt)*cos(toRadians(self.arcOffsetAngle)),
                self.cent[0]+self.radius*cos(toRadians(self.arcOffsetAngle))
            ]
            self.arc_yValues = [
                self.cent[1],
                self.cent[1]+(self.radius+self.drawRadiusExt)*sin(toRadians(self.arcOffsetAngle)),
                self.cent[1]+self.radius*sin(toRadians(self.arcOffsetAngle))
            ]
        for i in range(self.resolution+1):
            tempAng = (self.arcAngle/self.resolution)*i
            self.arc_xValues.append(self.cent[0]+self.radius*cos(toRadians(self.arcOffsetAngle+tempAng)))
            self.arc_yValues.append(self.cent[1]+self.radius*sin(toRadians(self.arcOffsetAngle+tempAng)))
        if self.drawRadiuss:
            self.arc_xValues.append(self.cent[0]+(self.radius+self.drawRadiusExt)*cos(toRadians(self.arcOffsetAngle+self.arcAngle)))
            self.arc_xValues.append(self.cent[0])
            self.arc_yValues.append(self.cent[1]+(self.radius+self.drawRadiusExt)*sin(toRadians(self.arcOffsetAngle+self.arcAngle)))
            self.arc_yValues.append(self.cent[1])
    def setup(self):
        self.solveValues()
        self.axesPlot, = self.ax.plot(
            self.arc_xValues, self.arc_yValues,
            label=self.plotPara["label"],
            linewidth=self.plotPara["linewidth"],
            alpha=self.plotPara["alpha"],
            linestyle=self.plotPara["linestyle"],
            color=self.plotPara["color"]
        )
        return self.axesPlot
    def update(self, angle: float, offsetAngle=arcOffsetAngle, center=cent):
        self.arcAngle = angle
        self.arcOffsetAngle = offsetAngle
        self.cent = center
        self.solveValues()
        self.axesPlot.set_data(self.arc_xValues, self.arc_yValues)
        return self.axesPlot
    def throwErr(self, text):
        print("Error:", "drawArc:", text)


if __name__=="__main__":
    import matplotlib.animation as animation
    centerPos = [0, 0]
    angles = [0, 90] #startAngle, offsetAngle
    density = 0.1

    def dataStream():
        global angles
        while True:
            for a in range(0,361):
                angles[0] = a
                angles[1] = 2*a
                yield a
    stream = dataStream()
    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)
    ax.set_aspect("equal")
    ax.grid("equal")
    ax.set_xlim((-3, 3))
    ax.set_ylim((-3, 3))
    arc = drawArc(ax, centerPos, 1, angles[0], angles[1], True, 0.5)
    def animSetup():
        global fig, ax, arc, arcPlot
        arcPlot = arc.setup()
        return [arcPlot]
    def animUpdate(i):
        global fig, ax, arc, arcPlot, stream
        next(stream)
        arc.resolution = round(density*(angles[0]))+1
        arc.cent = [cos(toRadians(angles[0])),sin(toRadians(angles[0]))]
        print(f"cent:{str([round(toDegrees(i),2) for i in arc.cent]):<15} res:{arc.resolution}", end="         \r")
        arc.update(angles[0], angles[1], arc.cent)
        return [arcPlot]
    animSetup()
    ani = animation.FuncAnimation(fig, animUpdate, interval=0, blit=True)
    plt.show()
