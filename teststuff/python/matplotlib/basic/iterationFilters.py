#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import math

iterRange = 100
realRange = 0

xValues = [*range(0,iterRange)]
yValues = []
yValues2 = []

new = 100
curr = 0

def f1():
    global curr
    filterVar = 0.01
    curr = filterVar*new + (1-filterVar)*curr
    return curr


while True:
    yValues.append(f1())
    realRange+=1
    if curr>=new*0.99: break

xValues = [*range(0,realRange)]

print(len(xValues), len(yValues))

plt.plot(xValues, yValues)
plt.plot(xValues, yValues2)
plt.show()