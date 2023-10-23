#!/usr/bin/env python3

from time import sleep

img = {
    0: "|",
    1: "/",
    2: "-",
    3: "\\",
}

symb = '█'

print("")

while True:
    for _,val in img.items():
        print("   ", val, symb*_, "     ", end="\r")

        sleep(0.5)
