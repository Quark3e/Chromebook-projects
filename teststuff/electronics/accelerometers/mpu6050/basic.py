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
    print("Acceleration: X:%.2f Y:%.2f Z:%.2f m/s^2" % (mpu.acceleration))
    print("Gyro X:%.2f Y:%.2f Z:%.2f degrees/s" % (toDegrees(mpu.gyro[0]), toDegrees(mpu.gyro[1]), toDegrees(mpu.gyro[2]))) 
    print("Temperature: %.2f C" % mpu.temperature)
    print("")
    time.sleep(1)

