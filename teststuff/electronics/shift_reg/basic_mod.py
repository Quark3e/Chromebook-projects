#!/usr/bin/env python3

import RPi.GPIO as GPIO #type: ignore
GPIO.setmode(GPIO.BCM)

def sendData(strVal, dataPin, latchPin, clockPin):
    GPIO.output(latchPin, 0)
    for i in range(8):
        GPIO.output(dataPin, int(strVal[i]))
        GPIO.output(clockPin, 1)
        GPIO.output(clockPin, 0)
    GPIO.output(latchPin, 1)
