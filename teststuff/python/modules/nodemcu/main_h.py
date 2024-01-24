#!/usr/bin/env python3

import socket
import time
from math import atan, sqrt, pi

class nodemcuClass(object):
    elapsedTime = 0
    server_msg = ""
    addr = ("192.168.1.117", 53)
    def __init__(
            self,
            address = addr[0],
            port = addr[1]
        ):
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.client_socket.settimeout(0.5)
        self.client_msg = b"fromClient"
    def reqToServer(self):
        self.startTime = time.time()
        self.client_socket.sendto(self.client_msg, self.addr)
        try:
            tempData, server = self.client_socket.recvfrom(1024)
            self.endTime = time.time()
            self.elapsedTime = self.endTime - self.startTime
            self.server_msg = tempData.decode()
        except socket.timeout:
            print("- \"espOrient\" request timed out.")


class espOrient(object):
    axis_g  = [0.1, 0.1, 0.1]
    """Raw list of axis acceleration readings \n\n [x, y, z]
    NOTE: Readings are checked. They're limited to range [-1, 1] and can't be 0, meaning if values
    are beyond that then stored values will be modified to within that range.
    \ne.g.
        if read>1: read=1\n
        elif read<-1: read=-1\n
        elif read==0: read=0.1\n
        else: ...
    \nFor un-modified values, use axis_gRaw
        """
    axis_gRaw = [0,1, 0.1, 0.1]
    """Fully raw acceleration readings. Values are Non-modified nor checked"""
    axis_gModified = [0.1, 0.1, 0.1]
    """List of filter modified axis values. Filters are applied to self.axis_g"""
    axis_gFilter = [0.1, 0.1, 0.1]
    """List of filters that are applied to axis_g \n\n [x, y, z]"""
    axis_Err = [0, 0, 0] 
    """Variables of the accelerometers inherent offset errors.
    In correction the read value is *subtracted* with corresponding Err value, so if read=1.1
    and err=read-1=+0.1, then correction will be read-error [x, y, z]"""
    Roll, Pitch = 0.1, 0.1
    """Filtered angle variables"""
    roll, pitch = 0.1, 0.1
    """Un-filtered raw angle variables"""
    tiltFilter = [0.1, 0.1]
    """Filter coefficient for changing tilt [Roll, Pitch]"""
    emptyTest = False
    def __init__(self, emptyTest=False):
        self.emptyTest = emptyTest
        self.espObj = nodemcuClass()
        self.readAccelerometer()
    def readAccelerometer(self, printText=True):
        if self.emptyTest:
            from random import randrange
            self.espObj.server_msg = "{"+f"{randrange(-10,11)/10}:{randrange(-10,10)/10}:{randrange(-10,10)/10}"+"}off;"
        else: self.espObj.reqToServer()
        if len(self.espObj.server_msg)>=1 and \
            (self.espObj.server_msg[:1]=="{" and \
             self.espObj.server_msg[-5]=="}" and \
             self.espObj.server_msg[-1]==";"):
            tempMsg = self.espObj.server_msg.replace(":", ",")
            msgTuple = eval(tempMsg[1:-5])
            self.axis_g[0], self.axis_g[1], self.axis_g[2] = msgTuple
            self.axis_g = [self.axis_g[i]-self.axis_Err[i] for i in range(len(self.axis_g))]
            if printText:
                print(
                    f"| x:{round(self.axis_g[0],1):>4}| y:{round(self.axis_g[1],1):>4}| z:{round(self.axis_g[2],1):>4}| roll:{round(self.roll,1):>6}| pitch:{round(self.pitch,1):>6}| Roll:{round(self.Roll,2):>7}| Pitch:{round(self.Pitch,2):>7}|",
                    end='\r')
        else:
            if printText: print("server_msg length is 0 or smaller")
    
        self.axis_gRaw = self.axis_g.copy()
        for i in range(len(self.axis_g)):
            if self.axis_g[i] > 1: self.axis_g[i] = 1
            elif self.axis_g[i] < -1: self.axis_g[i] = -1
            elif self.axis_g[i] == 0: self.axis_g[i] = 0.01
        self.axis_gModified = [(
            self.axis_g[i]**2*self.axis_gFilter[i] +
            self.axis_gModified[i]**2*(1-self.axis_gFilter[i]))**(1/2)
            for i in range(len(self.axis_gModified))
        ]

        #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
        self.pitch = atan(self.axis_g[1] / sqrt(self.axis_g[0]**2 + self.axis_g[2]**2)) * 180 / pi
        self.roll = atan(-1 * self.axis_g[0] / sqrt(self.axis_g[1]**2 + self.axis_g[2]**2)) * 180 / pi
        #filter
        self.Roll = (1-self.tiltFilter[0]) * self.Roll + self.tiltFilter[0] * self.roll
        self.Pitch = (1-self.tiltFilter[1]) * self.Pitch + self.tiltFilter[1] * self.pitch
        # if printText:
        #     print(f" accel: \
        #         x:{self.axis_g[0]} \
        #         y:{self.axis_g[1]} \
        #         z:{self.axis_g[2]} \
        #         roll:{self.roll} \
        #         pitch:{self.pitch} \
        #         ", end='\r')
        return self.Roll, self.Pitch, self.roll, self.pitch
