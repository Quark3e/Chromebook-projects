#!/usr/bin/env python3

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import cv2 #type: ignore
import numpy as np
import time
import math


P = [
    [0, 0], #const
    [0.25, 0.10],
    [0.75, 0.90],
    [1, 1], #const
]

def nothing(x):
    pass

# cv2.namedWindow("Trackbars")
# cv2.createTrackbar("P1[0]", "Trackbars", 25, 100, nothing)
# cv2.createTrackbar("P1[1]", "Trackbars", 25, 100, nothing)
# cv2.createTrackbar("P2[0]", "Trackbars", 75, 100, nothing)
# cv2.createTrackbar("P2[1]", "Trackbars", 75, 100, nothing)


def update(n):
    P[1] = [cv2.getTrackbarPos("P1[0]", "Trackbars"), cv2.getTrackbarPos("P1[1]", "Trackbars")]
    P[2] = [cv2.getTrackbarPos("P2[0]", "Trackbars"), cv2.getTrackbarPos("P2[1]", "Trackbars")]



    cv2.imshow("Trackbars")

    key = cv2.waitKey(1)
    if key == 27:
        return
    if key == 32:
        pass
    time.sleep(0.02)

xTemp = np.arange(0, 1, 0.01)
x_values = [P[p][0] for p in range(4)]
y_values = [P[p][1] for p in range(4)]

fitPoly = np.polyfit(x_values, y_values, 3)
polyFunc = np.poly1d(fitPoly)

print(polyFunc)

plt.scatter(x_values, y_values)
plt.plot(xTemp, polyFunc(xTemp))

plt.legend()
plt.grid()

plt.show()
