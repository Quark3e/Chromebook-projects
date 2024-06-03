#!/usr/bin/env python3
# Import packages
import cv2 #type: ignore

img = cv2.imread('/home/berkhme/github_repo/Chromebook-projects/teststuff/test_media/color-bars.webp') # load a dummy image
img = cv2.resize(img, (320, 180))

cv2.namedWindow("temp")

while(1):
    cv2.imshow("temp",img)
    k = cv2.waitKey(0)
    if k==27:    # Esc key to stop
        break
    elif k==-1:  # normally -1 returned,so don't print it
        continue
    else:
        print(k) # else print its value 
