#!/usr/bin/env python3

#import getpass

import os, time

from pyngrok import ngrok, conf

import digitalio #type: ignore
import board #type: ignore
from PIL import Image, ImageDraw, ImageFont
from adafruit_rgb_display import st7735 #type: ignore

BAUDRATE = 24_000_000

BORDER = 20
FONTSIZE = 12

cs_pin = digitalio.DigitalInOut(board.CE0)
dc_pin = digitalio.DigitalInOut(board.D25)
reset_pin = digitalio.DigitalInOut(board.D24)

spi = board.SPI()
disp = st7735.ST7735R(spi, rotation=270, cs=cs_pin, dc=dc_pin, rst=reset_pin, baudrate=BAUDRATE)

if disp.rotation % 180 == 90:
    height = disp.width
    width = disp.height
else:
    width = disp.width
    height = disp.height

image = Image.new("RGB", (width, height))
draw = ImageDraw.Draw(image)
draw.rectangle((0, 0, width, height), fill=(0, 0, 0))
disp.image(image)
font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", FONTSIZE)


#print("Enter your authtoken, which can be copied from https://dashboard.ngrok.com/auth")
#conf.get_default().auth_token = getpass.getpass()

# Open a TCP ngrok tunnel to the SSH server
connection_string = ngrok.connect("2223", "tcp").public_url

ssh_url, port = connection_string.strip("tcp://").split(":")
print(f"\n * ngrok tunnel available, access with `ssh pi@{ssh_url} -p{port}`")

text = f"ssh pi@{ssh_url}\n -p{port}"#\n{ssh_url}:{port}"

#text= "`ssh pi@168 -p{port}`"
(font_width, font_height) = font.getsize(text)
draw.text(
    (0, 0),#(width // 2 - font_width // 2, height // 2 - font_height // 2),
    text,
    font=font,
    fill=(255, 255, 255),
)

disp.image(image)

ngrok_process = ngrok.get_ngrok_process()

try:
    ngrok_process.proc.wait()
except KeyboardInterrupt:
    print("\n\nShutting down tunnel")
    ngrok.kill()
