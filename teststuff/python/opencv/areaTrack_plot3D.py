#!/usr/bin/env python3

# zAxis =0.003306(x**(4/6))**2-4.537(x**(4/6))+1580

import numpy as np
import matplotlib.pyplot as plt
import math
import sys
import time

fig = plt.figure()
ax = fig.add_subplot(projection='3d')

def plt_init():
    ax.set(xlim3d=(-150, 150), xlabel='X')
    ax.set(ylim3d=(-150, 150), ylabel='Y')
    ax.set(zlim3d=(0, 300), zlabel='Z')
    ax.view_init(elev=30, azim=60)


dataIndex = 10
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

print(f"\nloaded '{len(values)}' data sets")
if dataIndex >= len(values):
    print("dataIndex bigger than available data sets.\n exiting..")
    sys.exit()

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

rawData = ax.scatter(xData, yData, zData, c=areaData, cmap="magma", label="raw data")
# ax.plot(xData2, yData2, label="mean: "+str(meanGap), color="red", alpha=0.5)
# ax.plot(xData, polyFunc(xData), label="fit polynomial: "+str(polyDeg)+" deg", color="green")

plt.colorbar(rawData)

plt.title("contour Area - point coordinate")

plt.legend()
plt.grid()

plt.show()