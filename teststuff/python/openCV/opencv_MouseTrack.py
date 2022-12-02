import numpy as np
import cv2
drawing = False # true if mouse is pressed
buttonPressed = False
x1,y1,x2,y2 = -1,-1,-1,-1
# mouse callback function
def draw_circle(event,x,y,flags,param):
    global x1,y1,x2,y2,drawing,temp, buttonPressed, img
    x2,y2 = x,y
    if drawing:
      img = np.zeros((512,512,3), np.uint8)
    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        x1,y1 = x2,y2
    elif event == cv2.EVENT_MOUSEMOVE:
        buttonPressed = True
        if drawing == True:
            cv2.rectangle(img,(x1,y1),(x2,y2),(0,255,0),2)
            img = cv2.putText(img, "x"+str(abs(x2-x1)), (int(x1+(x2-x1)/2), int(y2+20)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
            img = cv2.putText(img, "y"+str(abs(y2-y1)), (int(x2+10), int(y1+(y2-y1)/2)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
    elif event == cv2.EVENT_LBUTTONUP:
        buttonPressed = False
        drawing = False
        cv2.rectangle(img,(x1,y1),(x2,y2),(0,255,0),-1)
        img = cv2.putText(img, "x"+str(abs(x2-x1)), (int(x1+(x2-x1)/2), int(y2+20)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
        img = cv2.putText(img, "y"+str(abs(y2-y1)), (int(x2+10), int(y1+(y2-y1)/2)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)

img = np.zeros((512,512,3), np.uint8)
temp = img
cv2.namedWindow('image')
cv2.setMouseCallback('image',draw_circle)
while(1):
    cv2.imshow('image',img)

    k = cv2.waitKey(1) & 0xFF

    if k == 27:
        break
    elif k == 99:
        img = np.zeros((512,512,3), np.uint8)
cv2.destroyAllWindows()