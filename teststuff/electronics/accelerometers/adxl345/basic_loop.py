#!/usr/bin/env python3

import time
import adafruit_adxl34x
import board
import math

i2c = board.I2C()
accelerometer = adafruit_adxl34x.ADXL345(i2c)

X_out, Y_out, Z_out = accelerometer.acceleration
Roll, Pitch, roll, pitch = 0.1, 0.1, 0.1, 0.1
tiltFilter = 0.1


def readAccelerometer():
    global X_out, Y_out, Z_out, Roll, Pitch, roll, pitch
    X_out, Y_out, Z_out = accelerometer.acceleration
    #x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = math.atan(Y_out / math.sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    roll = math.atan(-1 * X_out / math.sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    #filter
    Roll = (1-tiltFilter) * Roll + tiltFilter * roll
    Pitch = (1-tiltFilter) * Pitch + tiltFilter * pitch

while True:
    readAccelerometer()
    print( \
        "accel:{:4.1f} {:4.1f} {:4.1f} rotat:{:6.1f} {:6.1f}".format(\
            X_out, Y_out, Z_out,\
            roll, pitch\
        )\
    )
    time.sleep(0.05)