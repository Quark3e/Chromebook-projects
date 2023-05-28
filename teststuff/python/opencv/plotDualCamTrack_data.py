#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import json

fig, ax = plt.subplots()

xData, yData = [], []


f = open("zAxis-Area_values.dat", "r")
xData = eval(f.readline())
yData = eval(f.readline())

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

polyDeg = 9
z = np.polyfit(xData, yData, polyDeg)
polyFunc = np.poly1d(z)
print(polyFunc)

ax.scatter(xData, yData, s=2, label="raw data", )
ax.plot(xData2, yData2, label="mean: "+str(meanGap), color="red", alpha=0.5)
ax.plot(xData, polyFunc(xData), label="fit polynomial: "+str(polyDeg)+"th deg.", color="green")

plt.xlabel("contour moment area")
plt.ylabel("z axis")

plt.title("cnt area - webcam distance(z axis height)")

plt.legend()
plt.grid()

plt.show()