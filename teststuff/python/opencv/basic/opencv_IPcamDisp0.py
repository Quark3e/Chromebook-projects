#!/usr/bin/env python3

import urllib.request
import cv2 #type: ignore
import numpy as np
import time
URL = "http://192.168.1.124:8080"
while True:
    img_arr = np.array(bytearray(urllib.request.urlopen(URL).read()),dtype=np.uint8)
    if img_arr is not None:
        img = cv2.imdecode(img_arr,-1)
        cv2.imshow('IPWebcam',img)
    
    if cv2.waitKey(1):
        break
 