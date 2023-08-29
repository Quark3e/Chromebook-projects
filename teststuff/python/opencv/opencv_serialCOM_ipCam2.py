#!/usr/bin/env python3

import serial
import time
import requests
import cv2 #type: ignore
import numpy as np
import imutils

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
ser.reset_input_buffer()

# Replace the below URL with your own. Make sure to add "/shot.jpg" at last.
url_start = "http://"
url_end = "/shot.jpg"
url = url_start + input('Enter full ip address including port: ') + url_end
minContourVal = 1000

L_values, U_values = [55, 90, 9], [81, 215, 151]

cv2.namedWindow("Windows")
cX, cY, cZ = 320, 200, 150
posX, posY, posZ = cX, cY, cZ
intensityVal = 0.9
area = 1

def increase_brightness(img, value=30):
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    h, s, v = cv2.split(hsv)

    lim = 255 - value
    v[v > lim] = 255
    v[v <= lim] += value

    final_hsv = cv2.merge((h, s, v))
    img = cv2.cvtColor(final_hsv, cv2.COLOR_HSV2BGR)
    return img

# While loop to continuously fetching data from the Url
while True:
    img_resp = requests.get(url)
    img_arr = np.array(bytearray(img_resp.content), dtype=np.uint8)
    img = cv2.imdecode(img_arr, -1)
    img = cv2.flip(img, 0)
    # original image
    #cv2.imshow("Android_cam original", img)

    into_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    #L, U color values: [[62, 108, 0], [85, 238, 249]]
    L_limit = np.array(L_values)
    U_limit = np.array(U_values)
    threshold = cv2.inRange(into_hsv, L_limit, U_limit)
    filtered = cv2.bitwise_and(img, img, mask = threshold)
    # convert image to grayscale image
    filtered = increase_brightness(filtered, 90)
    #cv2.imshow('mask', filtered)
    gray_image = cv2.cvtColor(filtered, cv2.COLOR_BGR2GRAY)
    ##cv2.imshow('grayscale img', gray_image)
    # convert the grayscale image to binary image
    ret,thresh = cv2.threshold(gray_image,127,255,0)
    contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_NONE)
    for c in contours:
        # calculate moments of binary image
        M = cv2.moments(c)
        M = cv2.moments(thresh)
        # calculate x,y coordinate of center
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
        else:
            cX, cY = 0, 0
        # put text and highlight the center
        #if cv2.contourArea(contours[0]) > 0:
        cv2.circle(img, (cX, cY), 5, (255, 255, 255), -1)
        cv2.putText(img, "centroid" + str(cv2.contourArea(contours[0])), (cX - 25, cY - 25),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
        area = cv2.contourArea(contours[0])
        #coord = str(cX - 320) + " " + str(480 - cY) + " " + str(cZ)
        #print(coord)
    # display the image
    stacked = np.hstack((img, filtered))
    cv2.imshow('Windows', cv2.resize(stacked, None, fx=0.7, fy=0.7)) #type: ignore
    # Send coords to robot
    posX, posY, posZ = intensityVal*(0.8*posX + 0.2*(cX - 320)), intensityVal*(0.8*posY + 0.2*(480 - cY)), intensityVal*(cZ)
    print(str(posX) + " " + str(posY) + " " + str(posZ) + " " + str(area))
    data = str(posX) + ":" + str(posY) + ":" + str(posZ) + "\n"
    ser.write(data.encode('utf-8'))
    # Press Esc key to exit
    if cv2.waitKey(1) == 27:
        break 
cv2.destroyAllWindows() 









