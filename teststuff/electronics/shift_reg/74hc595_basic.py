#!/usr/bin/env python3

import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)

PIN_DATA = 16
PIN_LATCH = 20
PIN_CLOCK = 21

GPIO.setup(PIN_DATA, GPIO.OUT)
GPIO.setup(PIN_LATCH, GPIO.OUT)
GPIO.setup(PIN_LATCH, GPIO.OUT)


def sendData(strVal):
    GPIO.output(PIN_LATCH, 0)
    for i in range(8):
        GPIO.output(PIN_DATA, int(strVal[i - 1]))
        GPIO.output(PIN_CLOCK, 1)
        GPIO.output(PIN_CLOCK, 0)
    GPIO.output(PIN_LATCH, 1)

for i in range(10):
    sendData("01010101")
    time.sleep(0.5)
    sendData("10101010")
    time.sleep(0.5)

sendData("00000000")

