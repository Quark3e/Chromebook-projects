#!/usr/bin/env python3

import RPi.GPIO as GPIO #type: ignore #type: ignore
import time

GPIO.setmode(GPIO.BCM)
ledRelay = 23
GPIO.setup(ledRelay, GPIO.OUT)
GPIO.output(ledRelay, True)
time.sleep(1)
GPIO.output(ledRelay, False)
