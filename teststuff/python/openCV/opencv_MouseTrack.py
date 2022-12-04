import numpy as np
import cv2
import math

drawing = False # true if mouse is pressed
mode = True # if True, draw rectangle. Press 'm' to toggle to curve
buttonPressed = False
x1,y1,x2,y2 = -1,-1,-1,-1
# mouse callback function
def draw_objects(event,x,y,flags,param):
    global x1,y1,x2,y2,drawing,mode,temp,buttonPressed,img
    x2,y2 = x,y
    if drawing:
      img = np.zeros((360,640,3), np.uint8)
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
                img = drawCircles(img,(x1,y1), [int(math.sqrt(pow(x2-x1,2)+pow(y2-y1,2))),int(math.sqrt(pow(x2-x1,2)+pow(y2-y1,2))*0.75),int(math.sqrt(pow(x2-x1,2)+pow(y2-y1,2))*0.5),int(math.sqrt(pow(x2-x1,2)+pow(y2-y1,2))*0.25)])
                img = cv2.putText(img, "radius:"+str(int(round(math.sqrt(pow(x2-x1,2)+pow(y2-y1,2))))), (x2+5,y2+5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
    elif event == cv2.EVENT_LBUTTONUP:
        buttonPressed = False
        drawing = False
        if mode == True:
            cv2.rectangle(img,(x1,y1),(x2,y2),(0,255,0),-1)
            img = cv2.putText(img, "x"+str(abs(x2-x1)), (int(x1+(x2-x1)/2), int(y2+20)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
            img = cv2.putText(img, "y"+str(abs(y2-y1)), (int(x2+10), int(y1+(y2-y1)/2)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
        else:
            img = drawCircles(img,(x1,y1), [int(math.sqrt(pow(x2-x1,2)+pow(y2-y1,2))),int(math.sqrt(pow(x2-x1,2)+pow(y2-y1,2))*0.75),int(math.sqrt(pow(x2-x1,2)+pow(y2-y1,2))*0.5),int(math.sqrt(pow(x2-x1,2)+pow(y2-y1,2))*0.25)])
            img = cv2.putText(img, "radius:"+str(int(round(math.sqrt(pow(x2-x1,2)+pow(y2-y1,2))))), (x2+5,y2+5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
        temp = temp + img

def drawCircles(img, center, spacings, atCenter=True, line_colour=(255,255,255), line_thickness=1): #NOTE: Enter array in "spacings"
    height, width = img.shape[:2]
    for i in range(0, len(spacings)): cv2.circle(img, center, spacings[i], line_colour, line_thickness)
    return img

img = np.zeros((360,640,3), np.uint8)
base = cv2.imread("/home/berkhme/Chromebook-projects/teststuff/python/openCV/img1.jpg")
base = cv2.resize(base, (640,360))
temp = img
cv2.namedWindow('image')
cv2.setMouseCallback('image',draw_objects)
while(1):
    if drawing: cv2.imshow('image',img)
    else: cv2.imshow('image', temp)
    k = cv2.waitKey(1) & 0xFF
    if k == ord('m'):
        mode = not mode
    elif k == 27:
        break
    elif k == 99:
        img = np.zeros((360,640,3), np.uint8)
        temp = img
cv2.destroyAllWindows()