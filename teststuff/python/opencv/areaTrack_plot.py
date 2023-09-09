#!/usr/bin/env python3

# zAxis =0.003306(x**(4/6))**2-4.537(x**(4/6))+1580

import numpy as np
import matplotlib.pyplot as plt
import math
import sys

fig, ax = plt.subplots(figsize=(12,6), dpi=100)

polyDeg = 10
dataIndex = 1
values = []
# [
#   xData
#   yData,
# ]


f = open("zAxis-Area_values.dat", "r")
while True:
    line = f.readline()
    # print(line)
    if line == "2D\n":
        values.append([eval(f.readline()), eval(f.readline())])
    if not line: break

print(f"loaded '{len(values)}' data sets")
if dataIndex >= len(values):
    print("dataIndex bigger than available data sets.\n exiting..")
    sys.exit()

xData = values[dataIndex][0]
yData = values[dataIndex][1]

def zAdjFunc(var):
    ans = var
    ans = var
    return ans

xData = [zAdjFunc(i) for i in xData]
xData2, yData2 = xData.copy(), yData.copy()

# for key, val in values.items():
#     xData.append(int(val))
#     yData.append(int(key))


def mean(lst, index, numvalues):
    tempList = [lst[i] for i in range(index-numvalues, index+numvalues+1)]
    meanVar = sum(tempList) / (2*numvalues+1)
    return meanVar

meanGap = 6
for i in range(meanGap, len(xData)-meanGap):
    xData2[i] = mean(xData, i, meanGap)
    yData2[i] = mean(yData, i, meanGap)

def testFits(deg):
    z = np.polyfit(xData, yData, deg)
    polyFunc = np.poly1d(z)
    #print(str(polyFunc))
    return polyFunc

ax.scatter(xData, yData, s=2, label="raw data", )
ax.plot(xData2, yData2, label="mean: "+str(meanGap), color="red", alpha=0.5)

for i in range(8, 10+1, 2):
    ax.plot(xData, testFits(i)(xData), label="fit polyDeg: "+str(i)+" deg", alpha=0.5)

plt.xlabel("contour moment area")
plt.ylabel("z axis")

plt.title("cnt area - webcam distance(z axis height)")

plt.legend()
plt.grid()

plt.show()
