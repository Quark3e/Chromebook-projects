#!/usr/bin/env python3

import RPi.GPIO as GPIO #type: ignore #type: ignore
import time
import math

GPIO.setmode(GPIO.BCM)

pin_a = 18
pin_b = 24

totTime = 0
calcRes = 0

parts = {
    "R": 1000,
    "C": 0.22*10**-6,
    "V_B": 3.3,
    "V_trigg": 1.8
}

def R_t(t):
    return -(t) / ( parts["C"] * math.log(-(parts["V_trigg"]-parts["V_B"])/parts["V_B"]) ) - parts["R"]

def discharge():
    GPIO.setup(pin_a, GPIO.IN)
    GPIO.setup(pin_b, GPIO.OUT)
    GPIO.output(pin_b, False)
    time.sleep(0.004)

def charge_time():
    global totTime, calcRes
    t1 = time.time()
    GPIO.setup(pin_b, GPIO.IN)
    GPIO.setup(pin_a, GPIO.OUT)
    count = 0
    GPIO.output(pin_a, True)
    while not GPIO.input(pin_b):
        count = count + 1
    totTime = (time.time() - t1)
    calcRes = R_t(totTime)
    return count

def analog_read():
    discharge()
    return charge_time()

if __name__=="__main__":
    while True:
        print(f"delay:{round(totTime*1000, 2)}ms:    calc_Res.:{round(calcRes):<5}    analog inp.:{analog_read()}")
        time.sleep(0.01)
