#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import math
import os
import time

from IK_module import *

from board import SCL, SDA #type: ignore
import busio #type: ignore

from adafruit_motor import servo #type: ignore
# from adafruit_servokit import
from adafruit_pca9685 import PCA9685 #type: ignore

i2c = busio.I2C(SCL, SDA)
pca = PCA9685(i2c)
pca.frequency = 50

servo = [servo.Servo(pca.channels[0]),
         servo.Servo(pca.channels[1]),
         servo.Servo(pca.channels[2]),
         servo.Servo(pca.channels[3]),
         servo.Servo(pca.channels[4]),
         servo.Servo(pca.channels[5]),
         ]

for i in range(6): servo[i].set_pulse_width_range(500, 2500)
servo[0].angle = 108
servo[1].angle = 15
servo[2].angle = 180 - 25
servo[3].angle = 45
servo[4].angle = 180 - 0
servo[5].angle = 90