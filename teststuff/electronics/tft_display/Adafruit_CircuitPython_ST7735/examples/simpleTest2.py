#!/usr/bin/env python3

import time
import busio
import digitalio
from board import SCK, MOSI, MISO, CE0, D24, D25

from adafruit_rgb_display import color565
from adafruit_rgb_display.st7735 import ST7735


# Configuration for CS and DC pins:
CS_PIN = CE0
DC_PIN = D25
RESET_PIN = D24
BAUDRATE = 24000000

# Setup SPI bus using hardware SPI:
spi = busio.SPI(clock=SCK, MOSI=MOSI, MISO=MISO)

# Create the ST7789 display:
display = ST7735(
    spi,
    rotation=90,
    width=128,
    height=160,
    x_offset=53,
    y_offset=40,
    baudrate=BAUDRATE,
    cs=digitalio.DigitalInOut(CS_PIN),
    dc=digitalio.DigitalInOut(DC_PIN),
    rst=digitalio.DigitalInOut(RESET_PIN))

# Main loop: same as above
while True:
    # Clear the display
    display.fill(0)
    # Draw a red pixel in the center.
    display.pixel(100, 80, color565(255, 0, 0))
    # Pause 2 seconds.
    time.sleep(2)
    # Clear the screen blue.
    display.fill(color565(0, 0, 255))
    # Pause 2 seconds.
    time.sleep(2)
