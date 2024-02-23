#!/usr/bin/env python3

from bearlibterminal import terminal
import time

terminal.open()
terminal.printf(1, 1, 'Hello, world!')

for i in range(1, 11):
    terminal.refresh()
    terminal.printf(1, 1, f"count: {i}")
    time.sleep(0.5)


terminal.refresh()

count = 0
while terminal.read() != terminal.TK_CLOSE:
    
    count+=1
    if count==1000: count=0
    time.sleep(0.1)
    terminal.printf(1, 2, f"count: {count}")


terminal.close()
