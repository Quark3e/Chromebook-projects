#!/usr/bin/env python3

import os
import math
import time

from numpy import radians

termSize = os.get_terminal_size()

print("\033[2J")

circCenter = [termSize.columns/2, termSize.lines/2]
circRadius = min([termSize.columns, termSize.lines])*0.5

esc = "\033["

for a in range(360):
    # print("\033[2J") #clear screen
    print((
        esc+
        f"{circCenter[0]+round(math.cos(radians(a))*circRadius)};"+
        f"{circCenter[1]+round(math.sin(radians(a))*circRadius)}H"+
        esc+"0J X"
    ), end="\n")
    # print("o", end="")
    time.sleep(0.1)

