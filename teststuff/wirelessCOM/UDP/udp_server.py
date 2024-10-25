#!/usr/bin/env python3

import socket
import datetime

localIP = "127.0.0.1"
localIP = "192.168.1.112"
localPort = 2001
bufferSize = 1024

msgFromServer = "Hello UDP Client"
bytesToSend = str.encode(msgFromServer)

# Create a datagram socket
UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Bind to address and ip
UDPServerSocket.bind((localIP, localPort))

print("UDP server up and listening")

# Listen for incoming datagrams
while(True):
    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)
    message = bytesAddressPair[0]
    address = bytesAddressPair[1]

    clientMsg = "Message from Client:{}".format(message)
    clientIP  = "Client IP Address:{}".format(address)
    
    # ct stores current time
    ct = datetime.datetime.now()
    print(ct)
    
    print("\t", clientMsg)
    print("\t", clientIP)

    # Sending a reply to client
    UDPServerSocket.sendto(bytesToSend, address)
