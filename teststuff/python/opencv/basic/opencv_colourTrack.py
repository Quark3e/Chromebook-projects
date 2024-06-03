#!/usr/bin/env python3

# Import essential libraries
import requests
import cv2 #type: ignore
import numpy as np
import imutils
import os
import sys

cap = cv2.VideoCapture(0)

def nothing(x):
    pass



useDefaultLimit = False
defaultLimitIndex = 0
L_values = 3*[0]
U_values = 3*[0]

cv2.namedWindow("Webcam - XY")
cv2.createTrackbar("L - H", "Webcam - XY", 0, 179, nothing)
cv2.createTrackbar("L - S", "Webcam - XY", 0, 255, nothing)
cv2.createTrackbar("L - V", "Webcam - XY", 0, 255, nothing)
cv2.createTrackbar("U - H", "Webcam - XY", 179, 179, nothing)
cv2.createTrackbar("U - S", "Webcam - XY", 255, 255, nothing)
cv2.createTrackbar("U - V", "Webcam - XY", 255, 255, nothing)
while True:
    ret, frame = cap.read()
    frame = cv2.flip(frame, 1 ) 
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    l_h = cv2.getTrackbarPos("L - H", "Webcam - XY")
    l_s = cv2.getTrackbarPos("L - S", "Webcam - XY")
    l_v = cv2.getTrackbarPos("L - V", "Webcam - XY")
    u_h = cv2.getTrackbarPos("U - H", "Webcam - XY")
    u_s = cv2.getTrackbarPos("U - S", "Webcam - XY")
    u_v = cv2.getTrackbarPos("U - V", "Webcam - XY")
    lower_range = np.array([l_h, l_s, l_v])
    upper_range = np.array([u_h, u_s, u_v])
    mask = cv2.inRange(hsv, lower_range, upper_range) 
    res = cv2.bitwise_and(frame, frame, mask=mask)
    mask_3 = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)
    stacked = np.hstack((mask_3,frame,res))    
    cv2.imshow('Webcam - XY',cv2.resize(stacked,None,fx=0.4,fy=0.4)) #type: ignore
    key = cv2.waitKey(1)
    if key == 27:
        sys.exit()    
    if key == ord('s'):
        thearray = [[l_h,l_s,l_v],[u_h, u_s, u_v]]
        L_values, U_values = [l_h,l_s,l_v],[u_h, u_s, u_v]
        print(thearray)        
        np.save('hsv_value',thearray)
        break
    elif key == ord('l'):
        useDefaultLimit = True
        defaultLimitIndex = 0
        break
    elif key == ord('k'):
        useDefaultLimit = True
        defaultLimitIndex = 1
        break

cv2.destroyAllWindows()

if useDefaultLimit:
    if defaultLimitIndex == 0:
        L_values, U_values = [59, 184, 139], [116, 255, 255]#plexgear
    elif defaultLimitIndex == 1:
        L_values, U_values = [68, 207, 92], [112, 255, 238] #trust exis webcam


brightVal = 80
zDefaultVal = 3000000

cv2.namedWindow("Windows")

cv2.createTrackbar("erode iteration", "Windows", 0, 10, nothing)
cv2.createTrackbar("dilate iteration", "Windows", 0, 10, nothing)
# While loop to continuously fetching data from the Url
while True:
    # Get the image from ipwebcam
    ret, imgTemp = cap.read()
    img = cv2.resize(imgTemp, (640, 480))
    img = cv2.flip(img, 1)

    into_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    L_limit = np.array(L_values)
    U_limit = np.array(U_values)
    threshold = cv2.inRange(into_hsv, L_limit, U_limit)
    filtered = cv2.bitwise_and(img, img, mask = threshold)
    # convert image to grayscale image
    filtered[filtered!=0] = 255
    filtered = cv2.erode(filtered, np.ones((5,5), np.uint8), iterations=cv2.getTrackbarPos("erode iteration", "Windows"))
    filtered = cv2.dilate(filtered, np.ones((5,5), np.uint8), iterations=cv2.getTrackbarPos("dilate iteration", "Windows"))
    gray_image = cv2.cvtColor(filtered, cv2.COLOR_BGR2GRAY)
    ##cv2.imshow('grayscale img', gray_image)
  # convert the grayscale image to binary image
    ret,thresh = cv2.threshold(gray_image,127,255,0)
    contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_NONE)
    # os.system('clear')
    print("Number of contours:", len(contours))
    for c in contours:
        # calculate moments of binary image
        M = cv2.moments(c)
        # M = cv2.moments(thresh)
        # calculate x,y coordinate of center
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
        else:
            cX, cY = 0, 0
        cv2.circle(img, (cX, cY), 5, (255, 255, 255), -1)
        cv2.putText(img, "centroid" + str(cv2.contourArea(contours[0])), (cX - 25, cY - 25),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)

        # coord = "x:"+str(cX - 320)+" y:"+str(480 - cY)+" z:"+str((M["m00"] / zDefaultVal)*150) #str(cv2.contourArea(contours))
        # print(coord)
    # display the image
    stacked = np.hstack((img, filtered))
    cv2.imshow('Windows', cv2.resize(stacked, None, fx=0.7, fy=0.7)) #type: ignore
    # Press Esc key to exit
    if cv2.waitKey(1) == 27:
        break
cv2.destroyAllWindows()


