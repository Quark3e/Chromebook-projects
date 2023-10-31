#!/usr/bin/env python3

# zAxis =0.003306(x**(4/6))**2-4.537(x**(4/6))+1580

import numpy as np
import matplotlib.pyplot as plt
import math
import sys

fig, ax = plt.subplots(figsize=(12,6), dpi=100)

polyDeg = 10
dataIndex = 5
values = []
# [
#   xData
#   yData,
# ]


def checkfunc(x):
    c = [1.11616931*10**-39, -2.07682935*10**-34, 1.68556962*10**-29, -7.83319285*10**-25,
             2.30122730*10**-20, -4.45491855*10**-16, 5.75203827*10**-12, -4.90909892*10**-8,
             2.68701764*10**-4, -8.89666871*10**-1, 1.61255736*10**3]
    
    return sum([c[n]*(x**(10-n)) for n in range(len(c))])

def polyTest(xData):
    return [checkfunc(x) for x in xData]


f = open("data/raw_values.dat", "r")

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


xData2, yData2 = xData.copy(), yData.copy()

def mean(lst, index, numvalues):
    tempList = [lst[i] for i in range(index-numvalues, index+numvalues+1)]
    meanVar = sum(tempList) / (2*numvalues+1)
    return meanVar

meanGap = 6
for i in range(meanGap, len(xData)-meanGap):
    xData2[i] = mean(xData, i, meanGap)
    yData2[i] = mean(yData, i, meanGap)

def testFits(deg):
    print("-----------polyDeg:", deg)
    z = np.polyfit(xData, yData, deg)
    polyFunc = np.poly1d(z)
    print(str(polyFunc))
    return polyFunc

ax.scatter(xData, yData, s=2, label="raw data", )
ax.plot(xData2, yData2, label="mean: "+str(meanGap), color="green", alpha=0.5)

funcFile = open("data/solved_func.dat", "a")

for i in range(2, 10+1, 2):
    tempFunc = testFits(i)

    # funcFile.write(f"\n----------deg: {i}\n")
    # funcFile.write(str(tempFunc.c)+"\n")
    # funcFile.write(str(tempFunc))
    # funcFile.write("\n")

    ax.plot(xData, tempFunc(xData), label="fit polyDeg: "+str(i)+" deg", alpha=0.95, linewidth=3)

# ax.plot(xData, polyTest(xData), label='check plot', alpha=1, linewidth=2)


plt.xlabel("contour moment area")
plt.ylabel("z axis")

plt.title("cnt area - webcam distance(z axis height)")

plt.legend()
plt.grid()

plt.show()
