#!/usr/bin/env python3

import RPi.GPIO as GPIO #type: ignore
import time

GPIO.setmode(GPIO.BCM)

PIN_DATA = 12
PIN_LATCH = 16
PIN_CLOCK = 20

GPIO.setup(PIN_DATA, GPIO.OUT)
GPIO.setup(PIN_LATCH, GPIO.OUT)
GPIO.setup(PIN_CLOCK, GPIO.OUT)

strNum = {
    0: "11111100",
    1: "01100000",
    2: "11011010",
    3: "11110010",
    4: "01100110",
    5: "10110110",
    6: "10111110",
    7: "11100000",
    8: "11111110",
    9: "11110110"
}

def sendData(strVal):
    GPIO.output(PIN_LATCH, 0)
    for i in range(8):
        GPIO.output(PIN_DATA, int(strVal[i]))
        GPIO.output(PIN_CLOCK, 1)
        GPIO.output(PIN_CLOCK, 0)
    GPIO.output(PIN_LATCH, 1)

while True:
    i = 0
    data = "00000000"
    inp = input("enter index between 0-7: ")
    if inp=="exit": break
    if len(inp)>0:
        if len(inp)<=1 and int(inp)<10 and int(inp)>=0: data = strNum[int(inp)]
        if len(inp)==8: data = inp 
    sendData(data)
    print("sent:", data, end='\n\n')

sendData("00000000")

