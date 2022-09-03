# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

import time

from board import SCL, SDA
import busio

# Import the PCA9685 module. Available in the bundle and here:
#   https://github.com/adafruit/Adafruit_CircuitPython_PCA9685
from adafruit_motor import servo
from adafruit_pca9685 import PCA9685

i2c = busio.I2C(SCL, SDA)

pca = PCA9685(i2c)

# pca = PCA9685(i2c, reference_clock_speed=25630710)
pca.frequency = 50


servo = [servo.Servo(pca.channels[0]),
         servo.Servo(pca.channels[1]),
         servo.Servo(pca.channels[2]),
         servo.Servo(pca.channels[3]),
         servo.Servo(pca.channels[4]),
         servo.Servo(pca.channels[5])]

# We sleep in the loops to give the servo time to move into position.
#for i in range(180):
#    servo[0].angle = i
#    time.sleep(0.03)
#for i in range(180):
#    servo[0].angle = 180 - i
#    time.sleep(0.03)
#for i in range(180):
#    servo[0].angle = i
#    time.sleep(0.03)
    
servo[0].angle = 90
servo[1].angle = 0
servo[2].angle = 180
servo[3].angle = 90
servo[4].angle = 180
servo[5].angle = 180

# You can also specify the movement fractionally.
#fraction = 0.0
#while fraction < 1.0:
#    servo[0].fraction = fraction
#    fraction += 0.01
#    time.sleep(0.03)

pca.deinit()


