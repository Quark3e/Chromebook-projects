#!/usr/bin/env python3

import cv2
import numpy as np
import os
from time import sleep
import openCV_addon as ad
import math
import matplotlib.pyplot as plt

def nothing(x):
    pass

cam0 = cv2.VideoCapture(0)
cam2 = cv2.VideoCapture(2)

cv2.namedWindow("cap0")
cv2.namedWindow("cap1")

ad.hsv_trackbars("cap0")
ad.hsv_trackbars("cap1")


values = {
    "area": [],
    "zAxis": []
}

l_pos = [0, 0, 255]
u_pos = [179, 9, 255]



