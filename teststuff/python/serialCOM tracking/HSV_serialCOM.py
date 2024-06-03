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
brightVal = 85
circleScale = 1.5

L_values, U_values = [62, 150, 35], [90, 255, 149]

cv2.namedWindow("Windows")
cX, cY, cZ = 320, 200, 150
posX, posY, posZ = cX, cY, cZ #pos(axis name) is what is sent to he arduino, and the final variable
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
    into_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    L_limit = np.array(L_values)
    U_limit = np.array(U_values)
    threshold = cv2.inRange(into_hsv, L_limit, U_limit)
    filtered = cv2.bitwise_and(img, img, mask = threshold)
    filtered = increase_brightness(filtered, brightVal)
    gray_image = cv2.cvtColor(filtered, cv2.COLOR_BGR2GRAY)
    ret,thresh = cv2.threshold(gray_image,127,255,0)
    contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_NONE)
    for c in contours:
        M = cv2.moments(thresh)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
        else:
            cX, cY = 0, 0
        cv2.circle(img, (cX, cY), 5, (255, 255, 255), -1)
        cv2.putText(img, "centroid" + str(cv2.contourArea(contours[0])), (cX - 25, cY - 25),cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
        area = cv2.contourArea(contours[0])
    cv2.circle(img, (320, 480), int(50*circleScale), (255, 255, 255), 1)
    cv2.circle(img, (320, 480), int(100*circleScale), (255, 255, 255), 1)
    cv2.circle(img, (320, 480), int(150*circleScale), (255, 255, 255), 1)
    cv2.circle(img, (320, 480), int(200*circleScale), (255, 255, 255), 1)
    stacked = np.hstack((img, filtered))
    cv2.imshow('Windows', cv2.resize(stacked, None, fx=0.7, fy=0.7))
    
    posX, posY, posZ = intensityVal*(0.8*posX + 0.2*(cX - 320)), intensityVal*(0.8*posY + 0.2*(480 - cY)), intensityVal*(cZ)
    print(str(posX) + " " + str(posY) + " " + str(posZ) + " " + str(area))
    data = str(posX) + ":" + str(posY) + ":" + str(posZ) + "\n"
    ser.write(data.encode('utf-8'))
    if cv2.waitKey(1) == 27:
        break
cv2.destroyAllWindows()


