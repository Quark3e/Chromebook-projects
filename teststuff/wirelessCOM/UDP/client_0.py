#!/usr/bin/env python3

import socket
import time
import select

msgFromClient = "Hello UDP Server"
msgFromClient = "test"
bytesToSend = str.encode(msgFromClient)
#serverAddressPort = ("192.168.1.117", 53)
serverAddressPort = ("192.168.1.231", 53)
bufferSize = 1024

# Create a UDP socket at client side
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# UDPClientSocket.setblocking(0)
# UDPClientSocket.settimeout(10)

time.sleep(5)

while True:
    # Send to server using created UDP socket
    UDPClientSocket.sendto(bytesToSend, serverAddressPort)
    print("Message from Client: \"{}\"".format(msgFromClient))
    
    ready = [True]
    # ready = select.select([UDPClientSocket], [], [], 1)
    if ready[0]:
        msgFromServer = UDPClientSocket.recvfrom(bufferSize)

        msg = "Message from Server: \"{}\"".format(msgFromServer[0])
        print(msg)
    time.sleep(0.01)
