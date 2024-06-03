#!/usr/bin/env python3

#/*
# Terminal User input
# "Manually" control the servo motors with either different motions like:
#   - sweeps
#   - direct rotation commands
#   - move end-effector in a given pattern
#       -pre-set pattern
#       -from given .dat file (where user input file-path)
#*/

# Import essential libraries
import requests
import numpy as np
import imutils
import time
import math
# import adafruit_adxl34x
import sys

sys.path.append('/home/pi/Chromebook-projects/projects/proj_Hexclaw')

from IK_module import sendToServo, toRadians, toDegrees, getAngles


from board import SCL, SDA # type: ignore
import busio # type: ignore

from adafruit_motor import servo # type: ignore
from adafruit_servokit import ServoKit # type: ignore
from adafruit_pca9685 import PCA9685 # type: ignore

i2c = busio.I2C(SCL, SDA)
pca = PCA9685(i2c)
# accelerometer = adafruit_adxl34x.ADXL345(i2c)

pca.frequency = 50

servo = [servo.Servo(pca.channels[0]),
         servo.Servo(pca.channels[1]),
         servo.Servo(pca.channels[2]),
         servo.Servo(pca.channels[3]),
         servo.Servo(pca.channels[4]),
         servo.Servo(pca.channels[5])]
for i in range(6):
    servo[i].set_pulse_width_range(500, 2500)

# servo[0].angle = 90
# servo[1].angle = 15
# servo[2].angle = 150
# servo[3].angle = 90
# servo[4].angle = 180
# servo[5].angle = 90

def getServo4Offset(degrees): #s4 = q5
    return 90-(90/130)*degrees
#0-90 is inaccurate where the actual angle is 130 for the given 90 degrees
#Note: Only use this function if the rotation value is 

#servo[0].angle = 0
time.sleep(0.5)

diagnostics = False


axisFilter = 0.7 #On the new value end
xScaling, yScaling, zScaling = 0.8, 0.8, 1.2
brightVal = 75
zDefaultVal = 3000000
L_values, U_values = [36, 72, 8], [86, 240, 204]
#L_values, U_values = input("Enter L- and U-values without comma: "), input()
diffCheck = 100
showImage = False
globalPrint = False
endAnglePrint = False
firstAnglePrint = False
posOption = '-'

# X_out, Y_out, Z_out = accelerometer.acceleration #acceleration values for each axis
# Roll, Pitch = 0.1, 0.1

q = [0]*6 #NOTE: q = q[0] = servo[0]
s = [0, 0, 0, 0, 0, 0, 0] #The variables that are sent to the servos


# def readAccelerometer():
#     global X_out, Y_out, Z_out, Roll, Pitch, roll, pitch
#     X_out, Y_out, Z_out = accelerometer.acceleration
#     #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
#     pitch = math.atan(Y_out / math.sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / math.pi
#     roll = math.atan(-1 * X_out / math.sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / math.pi
#     #filter
#     Roll = 0.8 * Roll + 0.2 * roll
#     Pitch = 0.8 * Pitch + 0.2 * pitch


zMax = 300
a, b, Y = toRadians(0), toRadians(-45), toRadians(90)
PP = [float(0)]*3
PP[0], PP[1], PP[2] = 0.1, 0.1, 0.1
coord = ""

servoExceeded = False
whichServoExceeded = [False, False, False, False, False, False]
typeOfExceeded = ["none", "none", "none", "none", "none", "none"]


square = [
    [-150, 150, 150],
    [-50, 150, 150],
    [-50, 50, 150],
    [-150, 50, 150],
    [-150, 150, 150],
    [-150, 150, 50],
    [-50, 150, 50],
    [-50, 50, 50],
    [-150, 50, 50],
    [0, 150, 150],
    [150, 150, 150],
    [50, 150, 150],
    [50, 50, 150],
    [150, 50, 150],
    [150, 150, 150],
    [150, 150, 50],
    [50, 150, 50],
    [50, 50, 50],
    [150, 50, 150],
]


while True:
    servo[0].angle = 90
    servo[1].angle = 45
    servo[2].angle = 180 - 0
    servo[3].angle = 90
    servo[4].angle = 90
    servo[5].angle = 90

    print(" Options:")
    print(" 0.Calibrate given servo motor")
    print(" 1.Sweep given joint/servo motor")
    print(" 2.Send direct rotation to servo motors")
    # print(" 3.Move the end-effector in a pattern")
    option = input(" input: ")
    print()
    breakVal_0 = False
    if option == "exit":
        sys.exit()
    elif option == "0":
        while True:
            toCheck = input(" -Enter what servo to calibrate/check [q(n)]: n=")
            correction, read, given, isCorrect_90, bothSidesEqual = 0, 0, 0, True, True
            if toCheck == "exit": break
            servo[int(toCheck)-1].angle = 90
            time.sleep(0.1)
            servo[int(toCheck)-1].angle = 180-90
            time.sleep(0.9)
            for i in range(0, 181, 45):
                servo[int(toCheck)-1].angle = i
                time.sleep(1.5)
            for i in range(180, -1, -45):
                servo[int(toCheck)-1].angle = i
                time.sleep(1.5)
            time.sleep(2)
            
            temp = input(" -was 90° correct? [y/n]: ")
            if temp == "y": isCorrect_90 = True
            elif temp == "n": isCorrect_90 = False
            
            temp = input(" -are both sides (0-90 and 90-180) equal? [y/n]: ")
            if temp == "y": bothSidesEqual = True
            elif temp == "n": bothSidesEqual = False
            
            print()
            while True:
                print(" Note: The rotation values are between 0-180")
                print(" 1. enter read value to get the correction factor")
                print(" 2. send rotation value to given servo")
                print(" 3. change given/chosen servo motor (to check/calibrate a different motor)")
                print(" 4. exit this option")
                opt = input(" input: ")
                print()
                if opt == "1":
                    given = input(" enter rotation to send input for (degrees): ")
                    servo[int(toCheck)-1].angle = int(given)
                    read = input(" enter the read angle: ")
                    if isCorrect_90:
                        correction = (90-float(given)) / (90-float(read))
                    elif not isCorrect_90:
                        correction = float(given) / float(read)
                    if bothSidesEqual: print("\n\t Correction factor for servo motor q", toCheck, " = ", correction, ": applies on \"both sides\" of the rotation span", sep='')
                    elif not bothSidesEqual: print("\n\t Correction factor for servo motor q", toCheck, " = ", correction, ": does NOT apply on \"both sides\" of the rotation span", sep='')
                    print()
                elif opt == "2":
                    print(" enter rotation to send to servo motor q", toCheck, ": ", sep='', end='')
                    given = input("")
                    servo[int(toCheck)-1].angle = int(given)
                elif opt == "3": break
                elif opt == "4":
                    breakVal_0 = True
                    break
            if breakVal_0:
                break
    elif option == "1":
        while True:
            toSweep = input(" Enter what servo to sweep [q(n)]: n=")
            print()
            if toSweep == "exit":
                break
            servo[int(toSweep)-1].angle = 180
            time.sleep(0.75)
            servo[int(toSweep)-1].angle = 135
            time.sleep(0.75)
            servo[int(toSweep)-1].angle = 90
            time.sleep(0.75)
            servo[int(toSweep)-1].angle = 45
            time.sleep(0.75)
            servo[int(toSweep)-1].angle = 0
            time.sleep(1)
            
            for i in range(181):
                servo[int(toSweep)-1].angle = i
                time.sleep(0.01)
            time.sleep(1)
            for i in range(180,-1, -1):
                servo[int(toSweep)-1].angle = i
                time.sleep(0.01)
            time.sleep(1.5)
            break
    elif option == "2":
        while True:
            toMove = input(" Enter what servo to move [q(n)]: n=")
            print()
            if toMove == "exit":
                break
            moveAmount = input(" Enter servo rotation to move:")
            if moveAmount == "exit":
                break
            if int(toMove) == 3 or int(toMove) == 5:
                servo[int(toMove)-1].angle = 180 - int(moveAmount)
            elif int(toMove) == 1 or int(toMove) == 2 or int(toMove) == 4 or int(toMove) == 6:
                servo[int(toMove)-1].angle = int(moveAmount)
    elif option == "3":
        while True:
            print(" Movement patterns:")
            print(" 1.square (corners)")
            print(" 2.cuboid(/3d rectangle) (sides)")
            print(" 3.x-axis test")
            print(" 4.z-axis test")
            print(" 5.orientation test")
            option = input(" enter what pattern [number]:")
            print()
            if option == "exit":
                break
            if option == "1":
                for i in range(len(square)):
                    q = getAngles(square[i], 0, toRadians(-90), 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(1)
                break
            if option == "2":
                for i in range(-150, 150):
                    q = getAngles([i, 150, 150], 0, -90, 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.004)
                time.sleep(1)
                for i in range(150, 50, -1):
                    q = getAngles([150, i, 150], 0, -90, 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.01)
                time.sleep(1)
                for i in range(150, -150, -1):
                    q = getAngles([i, 50, 150], 0, -90, 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.004)
                time.sleep(1)
                for i in range(50, 150):
                    q = getAngles([-150, i, 150], 0, -90, 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.01)
                time.sleep(1)
                for i in range(150, 50, -1):
                    q = getAngles([-150, 150, i], 0, -90, 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.01)
                time.sleep(1)
                for i in range(-150, 150):
                    q = getAngles([i, 150, 50], 0, -90, 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.004)
                time.sleep(1)
                for i in range(150, 50, -1):
                    q = getAngles([150, i, 50], 0, -90, 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.01)
                time.sleep(1)
                for i in range(150, -150, -1):
                    q = getAngles([i, 50, 50], 0, -90, 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.004)
                time.sleep(1)
                for i in range(50, 150):
                    q = getAngles([-150, i, 50], 0, -90, 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.01)
                time.sleep(1)
                break
            if option == "3":
                coord = input(" enter yz coord to run axis test on [y z]: ").split()
                if coord[0] == "exit":
                    break
                angles = input(" enter a b orientation values for test [a b [degrees]]: ").split()
                print()
                for i in range(-200, 200):
                    q = getAngles([i, int(coord[0]), int(coord[1])], toRadians(int(angles[0])), toRadians(int(angles[1])), 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.005)
                for i in range(200, -200, -1):
                    q = getAngles([i, int(coord[0]), int(coord[1])], toRadians(int(angles[0])), toRadians(int(angles[1])), 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.005)
            if option == "4":
                coord = input(" enter xy coord to run z-axis test on [x y]: ").split()
                if coord[0] == "exit":
                    break
                angles = input(" enter a b orientation values for test [a b [degrees]]: ").split()
                print()
                for i in range(50, 300):
                    q = getAngles([int(coord[0]), int(coord[1]), i], toRadians(int(angles[0])), toRadians(int(angles[1])), 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.02)
                    print("z:", i, sep='')
                for i in range(300, 50, -1):
                    q = getAngles([int(coord[0]), int(coord[1]), i], toRadians(int(angles[0])), toRadians(int(angles[1])), 0, '-')
                    sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                    time.sleep(0.02)
                    print("z:", i, sep='')
            if option == "5":
                while True:
                    option = input(" which orientation variable to test: [a/b/Y]: ")
                    if option == "exit":
                        break
                    coord = input(" enter coord to run the test on [x y z]: ").split()
                    for i in range(-90, 90):
                        if option == "a":
                            q = getAngles([int(coord[0]), int(coord[1]), int(coord[2])], toRadians(i), 0, 0, '-', 1, 1, True)
                        elif option == "b":
                            q = getAngles([int(coord[0]), int(coord[1]), int(coord[2])], 0, toRadians(i), 0, '-', 1, 1, True)
                        elif option == "Y":
                            q = getAngles([int(coord[0]), int(coord[1]), int(coord[2])], 0, 0, toRadians(i), '-', 1, 1, True)
                        
                        sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
                        time.sleep(0.05)

pca.deinit()

