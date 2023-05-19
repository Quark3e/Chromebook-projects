#!/usr/bin/env python3


import adafruit_adxl34x # type: ignore

from board import SCL, SDA # type: ignore
import busio # type: ignore
import math
import time

i2c = busio.I2C(SCL, SDA)
accelerometer = adafruit_adxl34x.ADXL345(i2c)


accelFilter = 0.1
X_out, Y_out, Z_out = accelerometer.acceleration #acceleration values for each axis
Roll, Pitch = 0.1, 0.1


def readAccelerometer():
    global X_out, Y_out, Z_out, Roll, Pitch, roll, pitch
    X_out, Y_out, Z_out = accelerometer.acceleration
    try:
        pitch = math.atan(Y_out / math.sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / math.pi
        roll = math.atan(-1 * X_out / math.sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / math.pi
    except ZeroDivisionError:
        print("zero error")
    #filter
    Roll = (1-accelFilter) * Roll + accelFilter * roll
    Pitch = (1-accelFilter) * Pitch + accelFilter * pitch


while True:
    readAccelerometer()
    print(int(X_out), int(Y_out), int(Z_out), int(Roll), int(Pitch))
    time.sleep(0.01)

