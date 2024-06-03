#!/usr/bin/env python3

import cv2 #type: ignore
import numpy as np
import imutils
import math

file = 0
cascade_scaling = 1.05

def detect_hands(img):
    global cascade_scaling
    if file == 0: hand = cv2.CascadeClassifier('palm.xml') #Cascade Classifier for front-facing hands
    elif file == 1: hand = cv2.CascadeClassifier('/home/pi/Opencv/haarcascade/aGest.xml')
    elif file == 2: hand = cv2.CascadeClassifier('/home/pi/OpenCV/haarcascades/Hand.Cascade.1.xml')
    elif file == 3: hand = cv2.CascadeClassifier('/home/pi/OpenCV/haarcascades/hand.xml') #best one with higher scalingFactor/cascade_scaling
    elif file == 4: hand = cv2.CascadeClassifier('/home/pi/OpenCV/haarcascades/palm.xml')
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) #Converts image to grayscale
    hands = hand.detectMultiScale(gray, cascade_scaling, minNeighbors=6) #type: ignore #All detected hands get put into an array 
    if len(hands) != 0:        
        return hands
    else:
        return False

def getAngle(x,y,x2,y2):
    ''' a1
        /|
       / |
      /__|
    a3   a2
    '''
    a1 = [x2,y2] #Center of hand
    a2 = [x2,(y2+((y/2)-y2))] #x axis of the center frame and y axis of the hand center, forms a right angle
    a3 = [x/2,y/2] #Center of frame

    hypot = math.hypot(a1[0]-a3[0],a1[1]-a3[1]) #distance between the frame center and hand center
    ydistance = math.hypot(a1[0]-a2[0],a1[1]-a2[1]) #distance between the hand center and the right angle 
    ref_angle = math.degrees(math.asin(ydistance/hypot)) #angle from the hand center to the frame center
    plane_angle = 0
    #Top half of plane
    if y2 < y/2:
        if x2 > x/2: #Quadrant 1
            plane_angle = ref_angle
        elif x2 < x/2: #Quadrant 2
            plane_angle = 180 - ref_angle

    #Bottom half of plane
    elif y2 > y/2:
        if x2 < x/2: #Quadrant 3
            plane_angle = 180 + ref_angle
        elif x2 > x/2: #Quadrant 4
            plane_angle = 360 - ref_angle
    
    return int(ref_angle), int(plane_angle)


WIDTH, HEIGHT = (250,250)
# FPS = 30
camera = cv2.VideoCapture(0) #Detects webcamera

camera.set(cv2.CAP_PROP_FRAME_WIDTH,WIDTH);
camera.set(cv2.CAP_PROP_FRAME_HEIGHT,HEIGHT);
# camera.set(cv2.CAP_PROP_FPS,FPS);
    
while camera.isOpened():

    ret, frame = camera.read()
    # print(frame)
    y,x,_ = frame.shape
    
    #frame = cv2.bilateralFilter(frame, 90, 100, 100)  #Smoothing filter
    img = cv2.flip(frame, 1) #Flips the frame horizontally
    hands = detect_hands(img) #Array of coordiantes where hands are located
    
    try:
        #Grid pattern drawn
        cv2.line(img,(int(x/2),0),(int(x/2),y),(0,0,0),1)
        cv2.line(img,(0,int(y/2)),(x,int(y/2)),(0,0,0),1)

        cv2.putText(img,"scalingFactor: "+str(cascade_scaling),(10,20),cv2.FONT_HERSHEY_SIMPLEX,0.5,(255,255,255))
        cv2.putText(img,"w - increase scalingfactor by .05",(10,50),cv2.FONT_HERSHEY_SIMPLEX,0.5,(255,255,255))
        cv2.putText(img,"s - decrease scalingfactor by .05",(10,70),cv2.FONT_HERSHEY_SIMPLEX,0.5,(255,255,255))
        cv2.imshow('frame',img) #Show grid and frame

        if hands.any(): # type: ignore
            for (a,b,w,h) in hands: # type: ignore
                x2, y2 = (int(a+(w/2)),int(b+(h/2))) #Center coords of the hand
                angle, _ = getAngle(x,y,x2,y2) #Returns angle from center of frame to center of hand

                cv2.rectangle(img,(a,b),(a+w,b+h),(255,0,0),1) #Bounding box around the detected hand
                cv2.line(img,(int(x/2),int(y/2)),(x2,y2),(0,255,0),1) #Line to center of hand from center of frame
                cv2.putText(img,str(int(x2-x/2))+" "+str(int(y-y2)),(x2,y2),cv2.FONT_HERSHEY_SIMPLEX,0.5,(255,255,255))
                
            cv2.imshow('frame',img) #Show line to center of hand

            
    except:
        #If no hands were detected nothing happens
        pass
    
    k = cv2.waitKey(10) & 0xff #Detects key presses
    if k == 27:
        #If the esc key is pressed, stop the program
        break
    elif k == 119: cascade_scaling+=0.05
    elif k == 115 and cascade_scaling-0.05!=1: cascade_scaling-=0.05
    elif k == 48: file = 0
    elif k == 49: file = 1
    elif k == 50: file = 2
    elif k == 51: file = 3
    elif k == 52: file = 4

#Required for continous recording
camera.release()
cv2.destroyAllWindows()

