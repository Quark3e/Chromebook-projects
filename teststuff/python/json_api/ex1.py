#!/usr/bin/env python3

import urllib.request, json 
import json

data = {}
useUrl = "https://api.openweathermap.org/data/2.5/weather?q=S%C3%B6dert%C3%A4lje,SE&APPID=7c28bdf205051aa2ce610d67d648c55c"
# useUrl = "https://ddragon.leagueoflegends.com/cdn/14.1.1/data/en_US/champion/Fizz.json"
with urllib.request.urlopen(useUrl) as url:
    data = json.load(url)

# print(data)
# print("---------")

tempCount = True
for key,val in data.items():
    print(f" {key:<10}:", end="")
    if(type(val)==dict):
        for key1,val1 in val.items():
            if(tempCount):
                print(f" {key1}: ", val1, sep="")
                tempCount=False
            else: print(f"{'':<12} {key1}: ", val1, sep="")
        tempCount=True
    elif(type(val)==list):
        for i in val:
            if(tempCount):
                print(f" {i}")
                tempCount=False
            else: print(f"{'':<12} {i}")
        tempCount=True
    else:
        if(tempCount):
            print(f" {val}")