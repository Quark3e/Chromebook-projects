#!/usr/bin/env python3

#/*
# Terminal User input
# Manual Mode where the coordinate and orientation variables are input
# Doesn't use accelerometer
#*/

# Import essential libraries
import requests #type: ignore
import numpy as np #type: ignore
import imutils #type: ignore
import time
import math
from datetime import datetime
import adafruit_adxl34x # type: ignore
import sys
import cv2 #type: ignore #type: ignore
import os
import RPi.GPIO as GPIO #type: ignore #type: ignore
import serial #type: ignore
import matplotlib.pyplot as plt #type: ignore

sys.path.append('/home/pi/Chromebook-projects/projects/proj_Hexclaw')

from IK_module import *
from h2_module import *

from board import SCL, SDA # type: ignore
import busio # type: ignore

from adafruit_motor import servo # type: ignore
from adafruit_servokit import ServoKit # type: ignore
from adafruit_pca9685 import PCA9685 # type: ignore

i2c = busio.I2C(SCL, SDA)
pca = PCA9685(i2c)
pca.frequency = 50

servo = [servo.Servo(pca.channels[0]),
         servo.Servo(pca.channels[1]),
         servo.Servo(pca.channels[2]),
         servo.Servo(pca.channels[3]),
         servo.Servo(pca.channels[8]),
         servo.Servo(pca.channels[5]),
         ]



for i in range(6):
    servo[i].set_pulse_width_range(500, 2500)

sendToServo(servo,[135,45,180,45,180,90],0,mode=0)
time.sleep(1)


GPIO.setmode(GPIO.BCM) # GPIO Numbers instead of board numbers
 
ledRelay = 23
GPIO.setup(ledRelay, GPIO.OUT) # GPIO Assign mode
GPIO.output(ledRelay, GPIO.LOW) # out
GPIO.output(ledRelay, GPIO.HIGH) # on

time.sleep(0.75)
sendToServo(servo,[90,115,135,90,115,90],1,mode=2)

if False:
    for _ in range(4):
        GPIO.output(ledRelay, False)
        time.sleep(0.03)
        GPIO.output(ledRelay, True)
        time.sleep(0.03)
    
    time.sleep(1.5)
    GPIO.output(ledRelay, False)
    time.sleep(0.25)
    GPIO.output(ledRelay, True)
    time.sleep(0.5)
    GPIO.output(ledRelay, False)
    time.sleep(0.1)
    GPIO.output(ledRelay, True)
    time.sleep(2)
    GPIO.output(ledRelay, False)

print("------")
time.sleep(2)

GPIO.output(ledRelay, True)

diagnostics = False

if diagnostics:
    ard_port = '/dev/ttyUSB0'
    ser_arduino = serial.Serial(ard_port, 9600, timeout=0.1)
    ser_arduino.reset_input_buffer()
    x_Values = [] #time/seconds passed since start of program

    y0_Values = [] #voltage
    y1_Values = 6*[[]] #absolute rotation difference/jump for each *individual motor*
    y2_Values = [] #total absolute rotation difference (i.e. the "jump" in movement for that iteration)
    y3_Values = 6*[[]] #*estimated* stagnant load on each motor (Nm)
    GraphTitles = ["Voltage","new rotation","total new rotation","torque per motor"]
    Graph_yLabel = ["Voltage [V]","Angle [degrees]","Angle [degrees]","Torque [Nm]"]

axisFilter = 0.7 #On the new value end
xScaling, yScaling, zScaling = 0.8, 0.8, 1.2
brightVal = 75
diffCheck = 100
showImage = False
globalPrint = True
endAnglePrint = False
firstAnglePrint = False
posOption = '-'

q = [0]*6 #NOTE: q = q[0] = servo[0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos

zMax = 300
a, b, Y = toRadians(0), toRadians(-45), toRadians(90)
PP = [0, 200, 200]
coord = ""


drawing = False # true if mouse is pressed
buttonPressed = False
x1,y1,x2,y2 = -1,-1,-1,-1
windowRes = (600,300)
# mouse callback function
def mouseTrack(event,x,y,flags,param):
    global x1,y1,x2,y2,drawing,buttonPressed,img,temp
    x2,y2 = x,y
    if drawing:
      img = np.zeros((windowRes[1],windowRes[0],3), np.uint8)
    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        x1,y1 = x2,y2
    elif event == cv2.EVENT_MOUSEMOVE:
        buttonPressed = True
        if drawing == True:
            cv2.line(img,(0,y2),(windowRes[0],y2),(255,255,255),1)
            cv2.line(img,(x2,0),(x2,windowRes[1]),(255,255,255),1)
            cv2.circle(img,(x2,y2),10,(0,0,255),1)
            cv2.putText(img,"("+str(int(x2-windowRes[0]*0.5))+","+str(int(windowRes[1]-y2))+")",(x2+10,y2),cv2.FONT_HERSHEY_SIMPLEX,0.5,(255,255,255))
            cv2.circle(img,(int(windowRes[0]*0.5),int(windowRes[1])),50,(255,255,255),1)
            cv2.circle(img,(int(windowRes[0]*0.5),int(windowRes[1])),100,(255,255,255),1)
            cv2.circle(img,(int(windowRes[0]*0.5),int(windowRes[1])),150,(255,255,255),1)
            cv2.circle(img,(int(windowRes[0]*0.5),int(windowRes[1])),200,(255,255,255),1)
            cv2.putText(img,str(PP),(10,20),cv2.FONT_HERSHEY_SIMPLEX,0.5,(255,255,255))
            temp = img
    elif event == cv2.EVENT_LBUTTONUP:
        buttonPressed = False
        drawing = False
        img = temp #type: ignore
        # cv2.line(img,(0,y2),(windowRes[0],y2),(255,255,255),1)
        # cv2.line(img,(x2,0),(x2,windowRes[1]),(255,255,255),1)
        # cv2.circle(img,(x2,y2),10,(0,0,255),1)
        # cv2.putText(img,"("+str(int(x2-windowRes[0]*0.5))+","+str(int(windowRes[1]-y2))+")",(x2+10,y2),cv2.FONT_HERSHEY_SIMPLEX,0.5,(255,255,255))
        

def main():
    global PP, a, b, Y
    mode = 0


    while True:
        backBreak = False
        print(" Different modes for tracking/moving:")
        print(" 0. Run movement commands from .dat file")
        print(" 1. Enter position and orientation in terminal")
        print(" 2. Mouse position tracking on window")
        print(" 3. Move end-effector in a pattern")
        option = input(" input: ")
        if option == "exit": return
        elif option == "debug": debug_mod_menu(mod_dict)
        else: mode = int(option)


        start_time = time.time()
        x = 1 # displays the frame rate every 1 second
        counter = 0


        while True:
            # os.system("clear")
            print("\n ---Enter mode_(n) to change mode to (n)--- ")
            print(" - \"debug\" to enter debug-mod menu")
            print(" - \"mode_(n)\" to change mode to n")
            PP = [0, 200, 200]
            while True:
                isReachable = [True]
                if mode==0:
                    tempInput_1 = input("Enter file path:")
                    if tempInput_1 == "exit": return
                    elif tempInput_1 == "back":
                        backBreak=True
                        break
                    elif tempInput_1[:4] == "mode": mode=int(tempInput_1[5:])
                    elif tempInput_1[:5] == "debug": debug_mod_menu(mod_dict)
                    else:
                        basePath = "movementFiles/HC_2/"
                        movCommPath = basePath+tempInput_1
                        runFromFile(movCommPath, servo)
                    break
                if mode==1:
                    tempInput_1 = input("Enter coordinates [x y z] in mm: ").split()
                    if tempInput_1[0] == "exit": return
                    elif tempInput_1[0] == "back":
                        backBreak=True
                        break
                    elif tempInput_1[0][:4] == "mode": mode=int(tempInput_1[0][5:])
                    elif tempInput_1[0] == "debug": debug_mod_menu(mod_dict)
                    else:
                        PP[0] = (float(tempInput_1[0])) # type: ignore
                        PP[1] = (float(tempInput_1[1])) # type: ignore
                        PP[2] = (float(tempInput_1[2])) # type: ignore
                        break
                if mode==2:
                    tempInput_1 = input("Enter z-value in mm: ")
                    if tempInput_1 == "exit": return
                    elif tempInput_1 == "back":
                        backBreak=True
                        break
                    elif tempInput_1[:4] == "mode": mode=int(tempInput_1[5:])
                    elif tempInput_1 == "debug": debug_mod_menu(mod_dict)
                    else:
                        PP[2] = float(tempInput_1) # type: ignore
                        break
                if mode==3:
                    patternOpt = 1
                    print("Options:")
                    print(" 0.run a custom program/course of motions")
                    print(" 1.choose a pre-defined pattern from a dictionary")
                    print(" 2.move end-effector along an axis")
                    print(" 3.move end-effector orientation with a fixed position")
                    tempInput_1 = input("input: ")
                    if tempInput_1 == "exit": return
                    elif tempInput_1 == "back":
                        backBreak=True
                        break
                    elif tempInput_1[:4] == "mode": mode=int(tempInput_1[5:])
                    elif tempInput_1 == "debug": debug_mod_menu(mod_dict)
                    else:
                        patternOpt = int(tempInput_1)
                        break
            if backBreak: break
            if mode==1:
                tempInput_2 = input("Enter orientation values [a b Y] in degrees: ").split()
                if tempInput_2[0] == "exit": return
                elif tempInput_2[0] == "back": break
                a,b,Y = toRadians(float(tempInput_2[0])), toRadians(float(tempInput_2[1])), toRadians(float(tempInput_2[2]))
                if diagnostics: print("x:", PP[0], " y:", PP[1], " z:", PP[2], " a:", toDegrees(a), " b:", toDegrees(b), " Y:", toDegrees(Y), sep='')
                q = getAngles(PP,a,b,Y,'-', debug=mod_dict, positionIsReachable=isReachable)
                # print(q)
                print("read:",[toDegrees(q) for q in q], "posIsReachable:", isReachable)
                if isReachable[0]:
                    # custom_sendToServo(servo,[toDegrees(angle) for angle in q],0,True)
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=2,printResult=True)
            elif mode==2:
                tempInput_2 = input("Enter orientation values [a b Y] in degrees: ").split()
                if tempInput_2[0] == "exit": return
                elif tempInput_2[0] == "back": break
                a,b,Y = toRadians(float(tempInput_2[0])), toRadians(float(tempInput_2[1])), toRadians(float(tempInput_2[2]))
                img = np.zeros((windowRes[1],windowRes[0],3), np.uint8)
                cv2.namedWindow('tracking_window')
                cv2.setMouseCallback('tracking_window',mouseTrack)
                temp = img
                print("\n 'Esc' - change z-value and orientation\n")
                while True:
                    cv2.imshow('tracking_window', img) #use this if the previous drawings are not to be used
                    k = cv2.waitKey(1) & 0xFF
                    if k == 27: break
                    elif k == 119: PP[2]+=10 #type: ignore
                    elif k == 115: PP[2]-=10 #type: ignore
                    if drawing:
                        PP[0], PP[1] = x2-windowRes[0]*0.5,windowRes[1]-y2 # type: ignore
                        q = getAngles(PP,a,b,Y,'-',positionIsReachable=isReachable, debug=mod_dict)
                        # print(q)
                        if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    counter+=1
                    if (time.time() - start_time) > x :
                        print("FPS: ", counter / (time.time() - start_time))
                        counter = 0
                        start_time = time.time()
                cv2.destroyAllWindows()
            elif mode==3:
                if patternOpt==0: #type: ignore
                    print("Pick any of these programs")
                    for key,_ in mov_Programs.items(): print(" - \"",key,"\"", sep='')
                    key = input("input the program name: ")
                    if key=="exit": break
                    elif tempInput_1 == "back":
                        backBreak=True
                        break
                    presetAngles = 6*[0]
                    for joint in range(6): presetAngles[joint] = servo[joint].angle / constants_q[joint]["fixed"]
                    if key=="axisTest" and diagnostics:
                        mov_Programs[key](
                            servo,
                            [diagnostics,x_Values,
                            [y0_Values,y1_Values,y2_Values,y3_Values],ser_arduino]
                            ) 
                    else: mov_Programs[key](servo)
                    sendToServo(servo,presetAngles,0,mode=2)
                    if diagnostics and key=="axisTest":
                        fig = plt.figure(figsize=(19, 6))
                        ax = [0,0,0,0]
                        y_Values = [y0_Values,y1_Values,y2_Values,y3_Values]
                        for axis in range(len(ax)):
                            ax[axis] = fig.add_subplot(1,4,axis+1) #type: ignore
                            ax[axis].set_xlim(0,round(x_Values[-1])) #type: ignore
                            ax[axis].set_title(GraphTitles[axis]) #type: ignore
                            ax[axis].set_xlabel("time [seconds]") #type: ignore
                            ax[axis].set_ylabel(Graph_yLabel[axis]) #type: ignore
                            ax[axis].grid() #type: ignore
                        
                            if axis==1 or axis==3:
                                for j in range(6):
                                    ax[axis].plot(x_Values,y_Values[axis][j],linestyle='solid',label='q'+str(j+1)) #type: ignore
                            else:
                                ax[axis].plot(x_Values,y_Values[axis],linestyle='solid') #type: ignore
                                if axis==0:
                                    ax[axis].axhline( #type: ignore
                                        y=(sum(y_Values[axis])/len(y_Values[axis])),
                                        color='red',linestyle='-',
                                        label=f"avg:{round(sum(y_Values[axis])/len(y_Values[axis]))}V"
                                        )
                            ax[axis].legend(loc='upper right',framealpha=0.3) #type: ignore
                        fig.tight_layout(pad=5.0)
                        fig.suptitle("")

                        currentDate = str(datetime.now()).replace(" ", ";")
                        relativePath = "/home/pi/Chromebook-projects/projects/proj_Hexclaw/hexclaw_files/voltage_readings/media/"
                        fileTitle = "mov_Voltage_"
                        plt.savefig(relativePath+fileTitle+currentDate+".png") #type: ignore
                        plt.show()
                        
                if patternOpt==1: #type: ignore
                    print("Pick any of these patterns")
                    for key,_ in mov_Patterns.items(): print(" - \"",key,"\"", sep='')
                    key = input("input a key:")
                    if key=="exit": break
                    elif tempInput_1 == "back":
                        backBreak=True
                        break
                    for i in range(len(mov_Patterns[key])):
                        q = getAngles(
                            [mov_Patterns[key][i][0],mov_Patterns[key][i][1],mov_Patterns[key][i][2]],
                            toRadians(mov_Patterns[key][i][3]),
                            toRadians(mov_Patterns[key][i][4]),
                            toRadians(mov_Patterns[key][i][5]),
                            '-',
                            positionIsReachable=isReachable,
                            debug=mod_dict
                            )
                        print(mov_Patterns[key][i])
                        if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=2)
                        time.sleep(1)
                elif patternOpt==2: #type: ignore
                    axis = input("\nEnter what axis to move [x, y or z] [unit: mm]:")
                    if axis=="exit": break
                    elif tempInput_1 == "back":
                        backBreak=True
                        break
                    orientToUse = input("\nEnter orientation for axis test [a, b and Y]:").split()
                    fullPos = [0,link[4]+link[5]+120,150]
                    presetAngles = [0,0,0,0,0,0]
                    for joint in range(6): presetAngles[joint] = servo[joint].angle / constants_q[joint]["fixed"]
                    prevSent=False
                    for direction in range(1, -2, -2):
                        for pos in range(-200, 200):
                            if axis == "x": fullPos[0] = direction*pos*0.75  #type: ignore #300 
                            if axis == "y": fullPos[1] = direction*pos*0.5+100 #type: ignore #200
                            if axis == "z": fullPos[2] = direction*pos*0.5+200 #type: ignore #200
                            q = getAngles(
                                fullPos,
                                toRadians(int(orientToUse[0])),toRadians(int(orientToUse[1])),toRadians(int(orientToUse[2])),
                                '-', positionIsReachable=isReachable,
                                debug=mod_dict
                            )
                            if isReachable[0]:
                                if prevSent: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                                else:
                                    sendToServo(servo,[toDegrees(joint) for joint in q],2,useDefault=True,mode=2)
                                    time.sleep(1)
                                    prevSent=True
                            if axis == "x": time.sleep(0.005)
                            else: time.sleep(0.001)
                    time.sleep(1.5)
                    sendToServo(servo,presetAngles,0,mode=2)
                elif patternOpt==3: #type: ignore
                    orientToUse = input("\nEnter what orientation to test [a, b or Y] [unit: degrees]:")
                    posToUse = input("\nEnter coordinate for position test [x, y and z]:").split()
                    for _ in range(3): posToUse[_] = int(posToUse[_]) #type: ignore
                    fullOrient = [0,0,0]
                    presetAngles = [0,0,0,0,0,0]
                    for joint in range(6): presetAngles[joint] = servo[joint].angle / constants_q[joint]["fixed"]
                    for direction in range(1, -2, -2):
                        for angle in range(-90, 90):
                            if orientToUse == "a": fullOrient[0] = direction*angle
                            if orientToUse == "b": fullOrient[1] = direction*angle
                            if orientToUse == "Y": fullOrient[2] = direction*angle
                            q = getAngles(posToUse,toRadians(fullOrient[0]),toRadians(int(fullOrient[1])),toRadians(int(fullOrient[2])),
                            '-',positionIsReachable=isReachable,
                            debug=mod_dict
                            )
                            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                            time.sleep(0.01)
                    time.sleep(1.5)
                    sendToServo(servo,presetAngles,0,mode=2)
        # input("\npaused. Press enter to continue...")


if __name__ == "__main__":
    main()
    sendToServo(servo,[135,45,180,45,180,90],1,mode=2)
    GPIO.output(ledRelay, False)
    pca.deinit()

