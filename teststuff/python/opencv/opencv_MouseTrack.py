import numpy as np
import cv2 #type: ignore
import math

drawing = False # true if mouse is pressed
mode = True # if True, draw rectangle. Press 'm' to toggle to curve
buttonPressed = False
x1,y1,x2,y2 = -1,-1,-1,-1
windowRes = (600,300)
# mouse callback function
def draw_objects(event,x,y,flags,param):
    global x1,y1,x2,y2,drawing,mode,temp,buttonPressed,img
    x2,y2 = x,y 
    if drawing:
      img = np.zeros((windowRes[1],windowRes[0],3), np.uint8)
    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        x1,y1 = x2,y2
    elif event == cv2.EVENT_MOUSEMOVE:
        buttonPressed = True
        if drawing == True:
            if mode == True:
                cv2.rectangle(img,(x1,y1),(x2,y2),(0,255,0),2)
                img = cv2.putText(img, "x"+str(abs(x2-x1)), (int(x1+(x2-x1)/2), int(y2+20)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
                img = cv2.putText(img, "y"+str(abs(y2-y1)), (int(x2+10), int(y1+(y2-y1)/2)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
            else:
                cv2.line(img,(0,y2),(windowRes[0],y2),(255,255,255), 1)
                cv2.line(img,(x2,0),(x2,windowRes[1]),(255,255,255),1)
                cv2.circle(img,(x2,y2),10,(0,0,255),1)
    elif event == cv2.EVENT_LBUTTONUP:
        buttonPressed = False
        drawing = False
        if mode == True:
            cv2.rectangle(img,(x1,y1),(x2,y2),(0,255,0),-1)
            img = cv2.putText(img, "x"+str(abs(x2-x1)), (int(x1+(x2-x1)/2), int(y2+20)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
            img = cv2.putText(img, "y"+str(abs(y2-y1)), (int(x2+10), int(y1+(y2-y1)/2)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
        else:
            cv2.line(img,(0,y2),(windowRes[0],y2),(255,255,255), 1)
            cv2.line(img,(x2,0),(x2,windowRes[1]),(255,255,255),1)
            cv2.circle(img,(x2,y2),10,(0,0,255),1)
        temp = img

img = np.zeros((windowRes[1],windowRes[0],3), np.uint8)
temp = img
cv2.namedWindow('image')
cv2.setMouseCallback('image',draw_objects)
while(1):
    if drawing: cv2.imshow('image',img)
    else: cv2.imshow('image', temp)
    # cv2.imshow('image', img) #use this if the previous drawings are not to be used
    k = cv2.waitKey(1) & 0xFF
    if k == ord('m'): mode = not mode
    elif k == 27: break
    elif k == 99: #c
        img = np.zeros((windowRes[1],windowRes[0],3), np.uint8)
        temp = img
cv2.destroyAllWindows()