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
cam1 = cv2.VideoCapture(2)

cv2.namedWindow("cap0")
cv2.namedWindow("cap1")

ad.hsv_trackbars("cap0")
ad.hsv_trackbars("cap1")


values = {
    "area": [],
    "zAxis": []
}

prefRes = (640, 480)

l_pos = [0, 0, 255]
u_pos = [179, 9, 255]
kernel = np.ones((5, 5), np.uint8)
font = cv2.FONT_HERSHEY_SIMPLEX

imgTemp = [0, 0]
ret = [0, 0]
contours = 0
cntArea = 0
cntMoments = 0

def processFrame(img, flag):
    frame = cv2.resize(img, prefRes)
    frame = cv2.flip(frame, 1)
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    

while True:
    ret[0], imgTemp[0] = cam0.read()
    ret[1], imgTemp[1] = cam1.read()
    if ret[0]==False or ret[1]==False:
        print("could not capture from cam. returned:", ret)
        break




