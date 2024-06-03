#!/usr/bin/env python3

#finding hsv range of target object(pen)
import requests
import cv2 #type: ignore
import numpy as np
import imutils
import time
import os
from time import sleep

# output_dir = r"test_media"
# os.chdir(output_dir)
filename = "threshImg.jpg"

# cap = cv2.VideoCapture(0)

# A required callback method that goes into the trackbar function.
def nothing(x):
    pass

mode = 0

# Initializing the webcam feed.
if mode==0: cap = cv2.VideoCapture(2)
elif mode==1:
    mp4Src = '/home/berkhme/Chromebook-projects/teststuff/python/openCV/test_media/VID_20230508_173143.mp4'
    cap = cv2.VideoCapture(mp4Src)
#cap.set(3,1280)
#cap.set(4,720)

# Create a window named trackbars.
cv2.namedWindow("Trackbars")

# Now create 6 trackbars that will control the lower and upper range of 
# H,S and V channels. The Arguments are like this: Name of trackbar, 
# window name, range,callback function. For Hue the range is 0-179 and
# for S,V its 0-255.
cv2.createTrackbar("L - H", "Trackbars", 0, 179, nothing)
cv2.createTrackbar("L - S", "Trackbars", 0, 255, nothing)
cv2.createTrackbar("L - V", "Trackbars", 255, 255, nothing)
cv2.createTrackbar("U - H", "Trackbars", 179, 179, nothing)
cv2.createTrackbar("U - S", "Trackbars", 9, 255, nothing)
cv2.createTrackbar("U - V", "Trackbars", 255, 255, nothing)

kernel = np.ones((5, 5), np.uint8)
font = cv2.FONT_HERSHEY_SIMPLEX

while True:
    ret, imgTemp = cap.read()
    # imgTemp = cv2.imread("/home/berkhme/Chromebook-projects/teststuff/python/openCV/test_media/IMG_20230508_173123.jpg")
    if not ret:
        if mode==0:
            print("could not open camera")
            break
        elif mode==1:
            cap.release()
            cap = cv2.VideoCapture(mp4Src)
            ret, imgTemp = cap.read()
    frame = cv2.resize(imgTemp, (640, 480))

    # Flip the frame horizontally (Not required)
    frame = cv2.flip( frame, 1 ) 
    
    # Convert the BGR image to HSV image.
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # Get the new values of the trackbar in real time as the user changes 
    # them
    l_h = cv2.getTrackbarPos("L - H", "Trackbars")
    l_s = cv2.getTrackbarPos("L - S", "Trackbars")
    l_v = cv2.getTrackbarPos("L - V", "Trackbars")
    u_h = cv2.getTrackbarPos("U - H", "Trackbars")
    u_s = cv2.getTrackbarPos("U - S", "Trackbars")
    u_v = cv2.getTrackbarPos("U - V", "Trackbars")
 
    # Set the lower and upper HSV range according to the value selected
    # by the trackbar
    lower_range = np.array([l_h, l_s, l_v])
    upper_range = np.array([u_h, u_s, u_v])
    
    # Filter the image and get the binary mask, where white represents 
    # your target color
    mask = cv2.inRange(hsv, lower_range, upper_range)
 
    # You can also visualize the real part of the target color (Optional)
    res = cv2.bitwise_and(frame, frame, mask=mask)
    
    # Converting the binary mask to 3 channel image, this is just so 
    # we can stack it with the others
    mask_3 = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)
    
    morphImg = cv2.erode(mask_3, kernel, iterations=0)
    morphImg = cv2.dilate(morphImg, kernel, iterations=10)
    morphImg = cv2.erode(morphImg, kernel, iterations=1)

    # mask_3 = cv2.putText(mask_3,"bin2_3channImg",(5,50),font,2,(255,255,255),2)
    frame = cv2.putText(frame,"campFrame",(5,50),font,2,(255,255,255),2)
    res = cv2.putText(res,"trackedArea",(5,50),font,2,(255,255,255),2)
    morphImg = cv2.putText(morphImg,"morphed",(5,50),font,2,(255,255,255),2)


    # stack the mask, orginal frame and the filtered result
    stacked = np.hstack((frame, res, morphImg))
    
    # Show this stacked frame at 40% of the size.
    cv2.imshow('Trackbars',cv2.resize(stacked,None,fx=0.4,fy=0.4)) #type: ignore
    
    # If the user presses ESC then exit the program
    key = cv2.waitKey(1)
    if key == 27:
        break
    
    # If the user presses `s` then print this array.
    if key == ord('s'): 
        thearray = [[l_h,l_s,l_v],[u_h, u_s, u_v]]
        print(thearray)
        
        # Also save this array as penval.npy
        np.save('hsv_value',thearray)
        break
    elif key == 32:
        cv2.imwrite(filename, stacked)
        sleep(1)
    sleep(0.02)
# Release the camera & destroy the windows.    
cv2.destroyAllWindows()


