#!/usr/bin/env python3

import numpy as np
import time
import sys
import os
import os.path
absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("nodemcu_accel.py")])
sys.path.append(absPath[:len(absPath)-len("teststuff/python/random/")]+"projects/")

import proj_Hexclaw.IK_module as IK #type: ignore


# def main():
#     wifiOrient = IK.nodemcuOrient()
    
#     while True:
#         wifiOrient.readAccelerometer()
#         time.sleep(0.01)

# if __name__=="__main__":
#     main()


import socket
import math

client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(0.5)
client_msg = b"fromClient"
addr = ("192.168.1.117", 53)
server_msg = ""
def reqToServer():
    global server_msg, elapsedTime
    start = time.time()
    client_socket.sendto(client_msg, addr)
    try:
        tempData, server = client_socket.recvfrom(1024)
        end = time.time()
        elapsedTime = end - start
        server_msg = tempData.decode()
    except socket.timeout:
        print("request timed out")


using_lTemp = True

X_out, Y_out, Z_out = 0.1, 0.1, 0.1
Roll, Pitch, roll, pitch = 0.1, 0.1, 0.1, 0.1
tiltFilter = 0.1

def readAccelerometer(printText=True):
    global X_out, Y_out, Z_out, Roll, Pitch, roll, pitch
    reqToServer()
    if len(server_msg)>=1 and (server_msg[:1]=="{" and server_msg[-5]=="}" and server_msg[-1]==";"):
        tempMsg = server_msg.replace(":", ",")
        msgTuple = eval(tempMsg[1:-5])
        X_out, Y_out, Z_out = msgTuple
    
    # if printText: print(f" accel: x:{X_out} y:{Y_out} z:{Z_out} roll:{roll} pitch:{pitch}", end='\r')
    
    if X_out > 1: X_out = 1
    if Y_out > 1: Y_out = 1
    if Z_out > 1: Z_out = 1
    if X_out < -1: X_out = -1
    if Y_out < -1: Y_out = -1
    if Z_out < -1: Z_out = -1
    if X_out==0: X_out=0.1
    if Y_out==0: Y_out=0.1
    if Z_out==0: Z_out=0.1
    #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = math.atan(Y_out / math.sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / np.pi
    roll = math.atan(-1 * X_out / math.sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / np.pi
    #filter
    Roll = (1-tiltFilter) * Roll + tiltFilter * roll
    Pitch = (1-tiltFilter) * Pitch + tiltFilter * pitch
    if printText:
        print(f"{using_lTemp:<5}  x:{round(X_out,2):>5}  y:{round(Y_out,2):>5}  z:{round(Z_out,2):>5}  roll:{round(roll,2):>5} pitch:{round(pitch,2):>5}           ", end='\r')

while True:
    readAccelerometer()
    time.sleep(0.01)
