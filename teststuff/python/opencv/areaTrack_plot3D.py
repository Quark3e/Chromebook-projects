#!/usr/bin/env python3

# zAxis =0.003306(x**(4/6))**2-4.537(x**(4/6))+1580

import numpy as np
import matplotlib.pyplot as plt
import math
import sys
import time
import argparse

parser = argparse.ArgumentParser(
    prog="areaTrack_plot3D",
    description="scatter plot 3D data sets",

)
parser.add_argument('setIndex', type=str, nargs='?', help='optional integer argument for data set index')
# parser.add_argument('')

args = parser.parse_args()

fig = plt.figure()
ax = fig.add_subplot(projection='3d')

def plt_init():
    ax.set(xlim3d=(-250, 250), xlabel='X')
    ax.set(ylim3d=(-250, 250), ylabel='Y')
    ax.set(zlim3d=(0, 400), zlabel='Z')
    ax.view_init(elev=30, azim=60)

values = []
# xData, yData = [], []

f = open("zAxis-Area_values.dat", "r")
print("loading data sets:\n")
i = 0
while True:
    i+=1
    print(f"\rreading line: {i}", end='')
    line = f.readline()
    # print(line)
    if line == "3D\n":
        values.append([eval(f.readline()), eval(f.readline()), eval(f.readline()), eval(f.readline())])
        i+=4
    if not line: break

indexSpec = False
dataIndex = 0
if args.setIndex != None:
    if args.setIndex == "all": indexSpec = True
    else: dataIndex = int(args.setIndex)


print(f"\nloaded '{len(values)}' data sets")
if indexSpec == False and dataIndex >= len(values):
    print("dataIndex bigger than available data sets.")
    sys.exit()

areaData = []
xData = []
yData = []
zData = []
if indexSpec:
    for i in range(len(values)):
        areaData += values[i][0]
        xData += values[i][1]
        yData += values[i][2]
        zData += values[i][3]
else:
    areaData = values[dataIndex][0]
    xData = values[dataIndex][1]
    yData = values[dataIndex][2]
    zData = values[dataIndex][3]

# def zAdjFunc(var):
#     ans = var
#     ans = var**(4/6)
#     return ans
# def mean(lst, index, numvalues):
#     tempList = [lst[i] for i in range(index-numvalues, index+numvalues+1)]
#     meanVar = sum(tempList) / (2*numvalues+1)
#     return meanVar
# meanGap = 6
# for i in range(meanGap, len(xData)-meanGap):
#     xData2[i] = mean(xData, i, meanGap)
#     yData2[i] = mean(yData, i, meanGap)

# polyDeg = 2
# z = np.polyfit(xData, yData, polyDeg)
# polyFunc = np.poly1d(z)
# print(str(polyFunc))

plt_init()

rawData = ax.scatter(xData, yData, zData, c=areaData, s=5, cmap="magma", label="raw data")
# ax.plot(xData2, yData2, label="mean: "+str(meanGap), color="red", alpha=0.5)
# ax.plot(xData, polyFunc(xData), label="fit polynomial: "+str(polyDeg)+" deg", color="green")

plt.colorbar(rawData)

plt.title("contour Area - point coordinate")

plt.legend()
plt.grid()

plt.show()