#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

plt.axis([0, 10, 0, 1])
plt.grid()

xValues = [x for x in range(10)]


y = np.random.random(10)
# plt.scatter(i, y)
plt.plot(xValues, y)

print("1")


for i in range(20):
    y = np.random.random(10)
    print(y)
    # plt.scatter(i, y)
    # plt.plot(xValues, y)
    plt.pause(0.5)

print("2")

plt.show()
