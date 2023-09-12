#!/usr/bin/env python3

# Import essential libraries
import requests
import cv2 #type: ignore
import numpy as np
import imutils
  
# Replace the below URL with your own. Make sure to add "/shot.jpg" at last.
url = "http://192.168.1.124:8080/shot.jpg"

cap = cv2.VideoCapture(0)
  
# While loop to continuously fetching data from the Url
while True:
    img_resp = requests.get(url)
    img_arr = np.array(bytearray(img_resp.content), dtype=np.uint8)
    img = cv2.imdecode(img_arr, -1)
    ret, frame = cap.read()
    
    #stacked = np.hstack((img, frame))
    #cv2.imshow("Video feeds from url and webcam", cv2.resize(stacked, None, fx=0.7, fy=0.7))
    cv2.imshow("url", img)
    cv2.imshow("webcam", frame)
    # Press Esc key to exit
    if cv2.waitKey(1) == 27:
        break
 
cv2.destroyAllWindows()
