#!/usr/bin/env python3

import random
import os
import time


termSize = [os.get_terminal_size().columns, os.get_terminal_size().lines]
print(termSize)


try:
    while True:
        print(f"\x1B[2J\x1B[{random.randint(0,termSize[1])};{random.randint(0,termSize[0])}H*", flush=True, end="")
        #print('\x1B[4;50H*')
        #time.sleep(0.001)
except KeyboardInterrupt:
    print("exiting..")

#print("\033[2JTest")
