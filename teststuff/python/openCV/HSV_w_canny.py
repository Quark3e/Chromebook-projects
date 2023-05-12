#!/usr/bin/env python3

from __future__ import print_function
import cv2
import numpy as np
import random as rng
import os
from time import sleep

output_dir = r"test_media"
os.chdir(output_dir)
filename = "threshImg.jpg"

rng.seed(12345)

#trackbar function requires a callback method/function. This is an empty placeholder
def nothing(x):
    pass

mp4Src = '/home/berkhme/Chromebook-projects/teststuff/python/openCV/test_media/VID_20230508_173143.mp4'
mp4Src = "/home/berkhme/Chromebook-projects/teststuff/python/openCV/test_media/VID_20230511_173648.mp4"
cap = cv2.VideoCapture(mp4Src)


srcWindow = "mainWindow"
cv2.namedWindow(srcWindow)
cv2.createTrackbar("L - H", srcWindow, 0, 179, nothing)
cv2.createTrackbar("L - S", srcWindow, 0, 255, nothing)
cv2.createTrackbar("L - V", srcWindow, 255, 255, nothing)
cv2.createTrackbar("U - H", srcWindow, 179, 179, nothing)
cv2.createTrackbar("U - S", srcWindow, 9, 255, nothing)
cv2.createTrackbar("U - V", srcWindow, 255, 255, nothing)
cv2.createTrackbar("dilate1_iter", srcWindow, 10, 20, nothing)
cv2.createTrackbar("erode1_iter", srcWindow, 0, 20, nothing)

cannyThresh_val = 100
cannyThresh_val_max = 255
cv2.createTrackbar('Canny Thresh', srcWindow, cannyThresh_val, cannyThresh_val_max, nothing)

kernel = np.ones((5, 5), np.uint8)
font = cv2.FONT_HERSHEY_SIMPLEX

contours = 0
cntArea = 0
cntPos = [0,0]
cntMoments = 0
areaFilterVal = 0.01

def cannyThresh(val, frame):
    global contours, cntMoments, cntPos, cntArea, morphImg
    canny_output = cv2.Canny(cv2.blur(cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY),(3,3)), val, val*2)
    contours, hierarchy = cv2.findContours(canny_output, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    cannyDrawing = np.zeros((canny_output.shape[0], canny_output.shape[1], 3), dtype=np.uint8)
    # print(len(contours))
    for i in range(1,len(contours)):
        # color = (rng.randint(0,256), rng.randint(0,256), rng.randint(0,256))
        color = (255,255,255)
        cv2.drawContours(cannyDrawing, contours, i, color, 2, cv2.LINE_8, hierarchy, 0)
        cntMoments = cv2.moments(contours[0])
        if cntMoments['m00']!=0:
            cntPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
            # cntArea = areaFilterVal * cv2.contourArea(contours[0]) + (1-areaFilterVal) * cntArea
            cntArea = cv2.contourArea(contours[0])
            morphImg = cv2.putText(morphImg, str(int(cntArea)),(cntPos[0],cntPos[1]),font,1,(255,0,0),2)
        else:
            print("'m00' = 0")
    return cannyDrawing

isPressed = False
loopPaused = False

imgTemp = None

while True:

    if not loopPaused:
        ret, imgTemp = cap.read()
        if not ret:
            cap.release()
            cap = cv2.VideoCapture(mp4Src)
            ret, imgTemp = cap.read()
    frame = cv2.resize(imgTemp, (640, 480))
    frame = cv2.flip( frame, 1 ) 
    bitFrame = cv2.resize(frame[0:480, 20:620],(160,128))
    
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    l_h = cv2.getTrackbarPos("L - H", srcWindow)
    l_s = cv2.getTrackbarPos("L - S", srcWindow)
    l_v = cv2.getTrackbarPos("L - V", srcWindow)
    u_h = cv2.getTrackbarPos("U - H", srcWindow)
    u_s = cv2.getTrackbarPos("U - S", srcWindow)
    u_v = cv2.getTrackbarPos("U - V", srcWindow)

    lower_range = np.array([l_h, l_s, l_v])
    upper_range = np.array([u_h, u_s, u_v])
    
    mask = cv2.inRange(hsv, lower_range, upper_range)
    res = cv2.bitwise_and(frame, frame, mask=mask)
    
    mask_3 = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)

    morphImg = cv2.erode(mask_3, kernel, iterations=0)
    morphImg = cv2.dilate(morphImg, kernel, iterations=cv2.getTrackbarPos("dilate1_iter", srcWindow))
    morphImg = cv2.erode(morphImg, kernel, iterations=cv2.getTrackbarPos("erode1_iter", srcWindow))

    cannyImg = cannyThresh(cv2.getTrackbarPos('Canny Thresh', srcWindow),morphImg)

    frame = cv2.putText(frame,"campFrame",(5,50),font,2,(255,255,255),2)
    res = cv2.putText(res,"trackedArea",(5,50),font,2,(255,255,255),2)
    morphImg = cv2.putText(morphImg,"morphed",(5,50),font,2,(255,255,255),2)
    cannyImg = cv2.putText(cannyImg,"cannyThresh",(5,50),font,2,(255,255,255),2)

    stacked = np.hstack((frame, res, morphImg, cannyImg))
    cv2.imshow(srcWindow,cv2.resize(stacked,None,fx=0.4,fy=0.4))
    cv2.imshow("test", cv2.resize(bitFrame, (600,480)))
     
    # If the user presses ESC then exit the program
    key = cv2.waitKey(1)
    if key == 27: #esc
        break
    elif key == ord('s'): 
        thearray = [[l_h,l_s,l_v],[u_h, u_s, u_v]]
        print(thearray)
        np.save('hsv_value',thearray) # Also save this array as penval.npy
        break
    elif key == 97: #a
        cv2.imwrite(filename, stacked)
        sleep(1)

    if key == 32 and not isPressed: #space
        isPressed = True
        loopPaused = not loopPaused
    elif key != 32:
        isPressed = False
    sleep(0.02)
cv2.destroyAllWindows()


