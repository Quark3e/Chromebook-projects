#!/usr/bin/env python3

# Brute-Force Matching with ORB Descriptors

import numpy as np
import cv2 as cv
import matplotlib.pyplot as plt

img1 = cv.imread("box.png", cv.IMREAD_GRAYSCALE) #queryImage
img2 = cv.imread("box_in_scene.png", cv.IMREAD_GRAYSCALE) #trainImage

# Initiate ORB detector
orb = cv.ORB_create()

# find the keypoints and descriptors with ORB
kp1, des1 = orb.detectAndCompute(img1, None)
kp2, des2 = orb.detectAndCompute(img2, None)

# create BFMatcher object
bf = cv.BFMatcher(cv.NORM_HAMMING, crossCheck=True)

# match descriptors
matches = bf.match(des1, des2)

# sort them in the order of their distance
matches = sorted(matches, key=lambda x:x.distance)

# draw first 10 matches
img3 = cv.drawMatches(img1, kp1, img2, kp2, matches[:10], None, flag=cv.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS)

plt.imshow(img3),plt.show()
