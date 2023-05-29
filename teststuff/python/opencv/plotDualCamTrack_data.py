#!/usr/bin/env python3

# zAxis =0.003306(x**(4/6))**2-4.537(x**(4/6))+1580

import numpy as np
import matplotlib.pyplot as plt
import math

fig, ax = plt.subplots()

xData, yData = [], []


f = open("zAxis-Area_values.dat", "r")
xData = eval(f.readline())
yData = eval(f.readline())

def zAdjFunc(var):
    ans = var
    ans = var**(4/6)
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

polyDeg = 2
z = np.polyfit(xData, yData, polyDeg)
polyFunc = np.poly1d(z)
print(str(polyFunc))

ax.scatter(xData, yData, s=2, label="raw data", )
ax.plot(xData2, yData2, label="mean: "+str(meanGap), color="red", alpha=0.5)
ax.plot(xData, polyFunc(xData), label="fit polynomial: "+str(polyDeg)+" deg", color="green")

plt.xlabel("contour moment area")
plt.ylabel("z axis")

plt.title("cnt area - webcam distance(z axis height)")

plt.legend()
plt.grid()

plt.show()