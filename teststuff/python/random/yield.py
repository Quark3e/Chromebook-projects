#!/bin/env python3

import time

def test(start=True):
    if start: print("start")
    start=False
    while i<=101:
        print("test", i, end="\r")
        time.sleep(0.02)
        yield i**2

temp = test()

for i in range(102):
    next(temp)
    # print(f"{next(temp(i)):_}")

input("paused")