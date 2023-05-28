#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import json

fig, ax = plt.subplots()
xdata, ydata = [], []

f = open("zAxis-Area_values.dat", "r")
values = eval(f.readline())

xData, yData = [], []

for key, val in values.items():
    xData.append(int(val))
    yData.append(int(key))

ax.plot(xData, yData)

plt.show()