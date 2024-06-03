#!/usr/bin/env python3


from __future__ import print_function
import cv2 #type: ignore
import numpy as np
import random as rng
import os
import os.path
from time import sleep, perf_counter

useVideo = False

saveVideo = False

displayToTFT = False

if displayToTFT:
    import digitalio #type: ignore
    import board #type: ignore
    from PIL import Image, ImageDraw, ImageOps
    from adafruit_rgb_display import st7735 #type: ignore

    cs_pin = digitalio.DigitalInOut(board.CE0) #type: ignore
    dc_pin = digitalio.DigitalInOut(board.D25) #type: ignore
    reset_pin = digitalio.DigitalInOut(board.D24) #type:ignore

    BAUDRATE = 24_000_000

    spi = board.SPI()
    disp = st7735.ST7735R(spi, rotation=270, cs=cs_pin, dc=dc_pin, rst=reset_pin, baudrate=BAUDRATE)

    if disp.rotation % 180 == 90:
        disp_height = disp.width
        disp_width = disp.height
    else:
        disp_width = disp.width
        disp_height = disp.height

    image = Image.new("RGB", (disp_width, disp_height))
    draw = ImageDraw.Draw(image)


displayToOpenCV = True

# mediaDir = "/home/pi/Chromebook-projects/teststuff/test_media"
mediaDir = str(os.path.realpath(__file__))[:len(str(os.path.realpath(__file__)))-len("python/opencv/basic/HSV_w_canny.py")]+"test_media/"

output_dir = mediaDir
# os.chdir(output_dir)
filename = "threshImg.jpg"

rng.seed(12345)

#trackbar function requires a callback method/function. This is an empty placeholder
def nothing(x):
    pass


def convert_avi_to_mp4(avi_file_path, output_name):
    os.popen("ffmpeg -i '{input}' -ac 2 -b:v 2000k -c:a aac -c:v libx264 -b:a 160k -vprofile high -bf 0 -strict experimental -f mp4 '{output}.mp4'".format(input = avi_file_path, output = output_name))
    return True

if useVideo:
    mp4Src = mediaDir+ "VID_20230511_173648.mp4"
    src = mp4Src
else: src = 2
cap = cv2.VideoCapture(src)

cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1) # Set exposure to manual mode

prefRes = (640, 480)

outFileName = "output.avi"
capSize = (int(cap.get(3)), int(cap.get(4)))
# fourcc = cv2.CV_FOURCC(*'XVID')
if saveVideo: capOut = cv2.VideoWriter(outFileName,cv2.VideoWriter_fourcc(*'MJPG'), 24, (2*prefRes[0], 2*prefRes[1]))
# capOut = cv2.VideoWriter('output.mp4',fourcc, 10, (capSize[0],capSize[1]))


l_pos = [0, 0, 255]
u_pos = [179, 9, 255]


srcWindow = "mainWindow"
if displayToOpenCV:
    cv2.namedWindow(srcWindow)
    cv2.createTrackbar("L - H", srcWindow, l_pos[0], 179, nothing)
    cv2.createTrackbar("L - S", srcWindow, l_pos[1], 255, nothing)
    cv2.createTrackbar("L - V", srcWindow, l_pos[2], 255, nothing)
    cv2.createTrackbar("U - H", srcWindow, u_pos[0], 179, nothing)
    cv2.createTrackbar("U - S", srcWindow, u_pos[1], 255, nothing)
    cv2.createTrackbar("U - V", srcWindow, u_pos[2], 255, nothing)
    cv2.createTrackbar("erode1_iter", srcWindow, 1, 20, nothing)
    cv2.createTrackbar("dilate1_iter", srcWindow, 6, 20, nothing)

    cannyThresh_val = 100
    cannyThresh_val_max = 255
    cv2.createTrackbar('Canny Thresh', srcWindow, cannyThresh_val, cannyThresh_val_max, nothing)

kernel = np.ones((5, 5), np.uint8)
font = cv2.FONT_HERSHEY_SIMPLEX

contours = 0
cntArea = 0
cntPos = [0,0]
cntMoments = 0
areaFilterVal = 0.01

frameLim = 100
prevFrames = []
loadedFrames = 0
framesAreLoaded = None
if loadedFrames < frameLim: framesAreLoaded = False
frameIndex = frameLim #currently shown frame
frameMove = 5

def cannyThresh(val, frame):
    global contours, cntMoments, cntPos, cntArea, morphImg
    canny_output = cv2.Canny(cv2.blur(cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY),(3,3)), val, val*2)
    contours, hierarchy = cv2.findContours(canny_output, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    cannyDrawing = np.zeros((canny_output.shape[0], canny_output.shape[1], 3), dtype=np.uint8)
    # print(len(contours))
    for i in range(1,len(contours)):
        # color = (rng.randint(0,256), rng.randint(0,256), rng.randint(0,256))
        color = (255,255,255)
        cv2.drawContours(cannyDrawing, contours, i, color, 2, cv2.LINE_8, hierarchy, 0)
        cntMoments = cv2.moments(contours[0])
        if cntMoments['m00']!=0:
            cntPos = [int(cntMoments['m10']/cntMoments['m00']),int(cntMoments['m01']/cntMoments['m00'])]
            # cntArea = areaFilterVal * cv2.contourArea(contours[0]) + (1-areaFilterVal) * cntArea
            cntArea = cv2.contourArea(contours[0])
            morphImg = cv2.putText(morphImg, str(int(cntArea)),(cntPos[0],cntPos[1]),font,1,(255,0,0),2)
        else:
            print("'m00' = 0")
    return cannyDrawing

isPressed = False
loopPaused = False

imgTemp = None

if not framesAreLoaded:
    print("Loading frames for replay... ", end='')


ret, imgTemp = cap.read()

while True:
    if not loopPaused:
        ret, imgTemp = cap.read()
        if not ret:
            print("could not capture frame")
            cap.release()
            cap = cv2.VideoCapture(mp4Src)
            ret, imgTemp = cap.read()
            break
    frame = cv2.resize(imgTemp, prefRes) #type: ignore
    frame = cv2.flip( frame, 1 )

    if useVideo and not framesAreLoaded:
        prevFrames.append(imgTemp)
        loadedFrames+=1
        if loadedFrames >= frameLim: 
            framesAreLoaded = True
            print(" frames loaded")
    
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    if displayToOpenCV:    
        l_h = cv2.getTrackbarPos("L - H", srcWindow)
        l_s = cv2.getTrackbarPos("L - S", srcWindow)
        l_v = cv2.getTrackbarPos("L - V", srcWindow)
        u_h = cv2.getTrackbarPos("U - H", srcWindow)
        u_s = cv2.getTrackbarPos("U - S", srcWindow)
        u_v = cv2.getTrackbarPos("U - V", srcWindow)
        lower_range = np.array([l_h, l_s, l_v])
        upper_range = np.array([u_h, u_s, u_v])

        mask = cv2.inRange(hsv, lower_range, upper_range)
    else:
        mask = cv2.inRange(hsv, np.array(l_pos), np.array(u_pos))

    res = cv2.bitwise_and(frame, frame, mask=mask)
    
    mask_3 = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)

    morphImg = cv2.erode(mask_3, kernel, iterations=0)
    if displayToOpenCV:
        morphImg = cv2.erode(morphImg, kernel, iterations=cv2.getTrackbarPos("erode1_iter", srcWindow))
        morphImg = cv2.dilate(morphImg, kernel, iterations=cv2.getTrackbarPos("dilate1_iter", srcWindow))
    else:
        morphImg = cv2.erode(morphImg, kernel, iterations=1)
        morphImg = cv2.dilate(morphImg, kernel, iterations=6)


    if displayToOpenCV: cannyImg = cannyThresh(cv2.getTrackbarPos('Canny Thresh', srcWindow),morphImg)
    else: cannyImg = cannyThresh(50, morphImg)

    bitFrame = cv2.resize(morphImg[0:prefRes[1], 20:prefRes[0]-20],(160,128))

    frame = cv2.putText(frame,"campFrame",(5,50),font,2,(255,255,255),2)
    res = cv2.putText(res,"trackedArea",(5,50),font,2,(255,255,255),2)
    morphImg = cv2.putText(morphImg,"morphed",(5,50),font,2,(255,255,255),2)
    cannyImg = cv2.putText(cannyImg,"cannyThresh",(5,50),font,2,(255,255,255),2)


    # stacked = np.hstack((frame, res, morphImg, cannyImg))
    stacked = np.hstack((np.vstack((frame, morphImg)), np.vstack((res, cannyImg))))

    if displayToOpenCV:
        cv2.imshow(srcWindow,cv2.resize(stacked,None,fx=0.4,fy=0.4)) #type: ignore
        cv2.imshow("test", cv2.resize(bitFrame, (250,200)))

    if displayToTFT:
        PIL_img = Image.fromarray(cv2.cvtColor(bitFrame, cv2.COLOR_BGR2RGB))
        # PIL_img = ImageOps.pad(PIL_img.convert("RGB"),(disp_width, disp_height),method=Image.NEAREST,color=(0,0,0),centering=(0.5,0.5))
        
        disp.image(PIL_img)

    # print([stacked.shape[1], stacked.shape[0]])
    if saveVideo: capOut.write(stacked)

    # If the user presses ESC then exit the program
    if displayToOpenCV:
        key = cv2.waitKey(1)
        if key == 27: #esc
            break
        elif key == ord('s'): 
            thearray = [[l_h,l_s,l_v],[u_h, u_s, u_v]] #type: ignore
            print(thearray)
            np.save('hsv_value',thearray) # Also save this array as penval.npy
            break
        elif key == 97: #a
            cv2.imwrite(filename, stacked) #type: ignore
            sleep(1)

        if key == 32 and not isPressed: #space
            isPressed = True
            loopPaused = not loopPaused
        elif key != 32:
            isPressed = False
            prevFrames = prevFrames[1:frameLim] + [imgTemp]

        if key == 81 and framesAreLoaded:
            loopPaused = True
            if frameIndex > frameMove: frameIndex-=frameMove
            else: print("viewing latest frame.")
            imgTemp = prevFrames[frameIndex]
            print("frame -")
        elif key == 83 and framesAreLoaded:
            loopPaused = True
            if frameIndex < frameLim-frameMove: frameIndex+=frameMove
            else: print("viewing oldes frame.")
            imgTemp = prevFrames[frameIndex]
            print("frame +")
    # sleep(0.0002)

if saveVideo: capOut.release()

if saveVideo: convert_avi_to_mp4(outFileName, outFileName)

if displayToOpenCV:
    cap.release()
    cv2.destroyAllWindows()
    print("\n")

