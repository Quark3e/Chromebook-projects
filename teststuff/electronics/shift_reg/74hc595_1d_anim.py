#!/usr/bin/env python3

import RPi.GPIO as GPIO #type: ignore
import time
import math
from basic_mod import sendData
import sys
sys.path.append("../electronics")
import stepRes_read as pot

GPIO.setmode(GPIO.BCM)

PIN_DATA = 12
PIN_LATCH = 16
PIN_CLOCK = 20

GPIO.setup(PIN_DATA, GPIO.OUT)
GPIO.setup(PIN_LATCH, GPIO.OUT)
GPIO.setup(PIN_CLOCK, GPIO.OUT)

animOpt = 0

setup_anim1 = {
    "spac": 0.05,
    "powScal": 4,
    "delayMS": 0.5,
    "inbDelay": 0,
}
setup_anim2 = {
    "delayMS": 500
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
        for i in range(8):
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
                #var = pot.analog_read()
                #inbDelay = abs(pot.calcRes) / 1000000000
                #print(inbDelay)
                if ledVal[n]*(1/spac)>=i: data = data[:n]+"1"+data[n+1:]
                else: data = data[:n]+"0"+data[n+1:]
                #print(ledVal[n]*(1/spac), end=' ')
                time.sleep(inbDelay)
            #print(data)
            sendData(data, PIN_DATA, PIN_LATCH, PIN_CLOCK)
            time.sleep(delay)

def anim2():
    delay = setup_anim2["delayMS"]/1000
    while True:
        data = "00000000"
        for i in range(127):
            binVar = bin(i)[2:]
            data = data[:7-len(binVar)] + binVar + "0"
            sendData(data, PIN_DATA, PIN_LATCH, PIN_CLOCK)
            time.sleep(delay)

def anim3():
    delay = 0
    while True:
        var = pot.analog_read()
        delay = ((abs(pot.calcRes)/10_000)**4)
        print(f"delay: {delay*1000:<20} ms", end='\r')
        data = "00010000"
        sendData(data, PIN_DATA, PIN_LATCH, PIN_CLOCK)
        time.sleep(delay)
        sendData("00000000", PIN_DATA, PIN_LATCH, PIN_CLOCK)
        time.sleep(delay)

if __name__=="__main__":
    if animOpt==0: anim0()
    elif animOpt==1: anim1()
    elif animOpt==2: anim2()
    elif animOpt==3: anim3()

