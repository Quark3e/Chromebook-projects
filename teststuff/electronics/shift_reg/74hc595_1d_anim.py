#!/usr/bin/env python3

import RPi.GPIO as GPIO #type: ignore
import time
import math
from basic_mod import sendData

GPIO.setmode(GPIO.BCM)

PIN_DATA = 12
PIN_LATCH = 16
PIN_CLOCK = 20

GPIO.setup(PIN_DATA, GPIO.OUT)
GPIO.setup(PIN_LATCH, GPIO.OUT)
GPIO.setup(PIN_CLOCK, GPIO.OUT)

setup_anim1 = {
    "spac": 0.05,
    "powScal": 4,
    "delayMS": 0.5,
    "inbDelay": 0,
}


def toRadians(degrees):
    return (degrees*math.pi)/180

def spac_round(val, spac = 0.25):
    gap = spac/2
    for i in range(0, 100, round(100*spac)):
        if val<(i/100)+gap: return i/100
    return 1


def anim0():
    while True:
        for i in range(7):
            data = "00000000"
            data = data[:i] + "1" + data[i+1:]
            sendData(data, PIN_DATA, PIN_LATCH, PIN_CLOCK)
            time.sleep(0.2)

def anim1():
    spac = setup_anim1["spac"]
    powScal = setup_anim1["powScal"]
    delay = setup_anim1["delayMS"]/1000
    inbDelay = setup_anim1["inbDelay"]/1000

    ledVal = [spac_round(math.sin(toRadians(x))**powScal, spac) for x in range(0, 180, round(180/7))]+[0]
    deg = -180
    while True:
        deg+=1
        if deg>180: deg=-180
        ledVal = [spac_round(math.sin(toRadians(x+deg))**powScal, spac) for x in range(0, 180, round(180/7))]+[0]
        data = "00000000"
        for i in range(1,round(1/spac)+1):
            #print("i:", i, end=' ')
            for n in range(8):
                if ledVal[n]*(1/spac)>=i: data = data[:n]+"1"+data[n+1:]
                else: data = data[:n]+"0"+data[n+1:]
                #print(ledVal[n]*(1/spac), end=' ')
                time.sleep(inbDelay)
            #print(data)
            sendData(data, PIN_DATA, PIN_LATCH, PIN_CLOCK)
            time.sleep(delay)

if __name__=="__main__":
    anim1()

