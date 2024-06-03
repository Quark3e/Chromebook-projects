#!/usr/bin/env python3

import time
import board
import adafruit_mpu6050
import math

i2c = board.I2C()
mpu = adafruit_mpu6050.MPU6050(i2c)

def toDegrees(radians):
    return (radians*180)/math.pi

while True:
    sensVals = mpu.acceleration + tuple(toDegrees(i) for i in mpu.gyro) + (mpu.temperature,)
    print( \
        "accel:{:4.1f} {:4.1f} {:4.1f} rotat:{:6.1f} {:6.1f} {:6.1f} temp:{:4.1f}".format(\
            sensVals[0], sensVals[1], sensVals[2],\
            sensVals[3], sensVals[4], sensVals[5],\
            sensVals[6]\
        )\
    )
    time.sleep(0.05)
