#!/usr/bin/env python3
#""" PY READER FROM ESP """
# Written by Junicchi - https://github.com/Kebablord

from time import sleep
import urllib.request
url = "http://192.168.1.117"  # ESP's IP, ex: http://192.168.102/ (Check serial console while uploading the ESP code, the IP will be printed)

def get_data():
	global data

	n = urllib.request.urlopen(url).read() # get the raw html data in bytes (sends request and warn our esp8266)
	n = n.decode("utf-8") # convert raw html bytes format to string :3

	data = n
#	data = n.split() #<optional> split datas we got. (if you programmed it to send more than one value) It splits them into seperate list elements.
#	data = list(map(int, data)) #<optional> turn datas to integers, now all list elements are integers.

# Example usage
while True:
	get_data()
	print("Your data(s) which we received from ESP: "+data)
	#sleep(0.01)
