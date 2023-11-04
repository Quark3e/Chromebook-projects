#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

# plt.axis([0, 10, 0, 1])

print("1")

for i in range(10):
    y = np.random.random()
    plt.scatter(i, y)
    plt.pause(0.05)

print("2")

plt.show()
