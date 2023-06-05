#!/usr/bin/env python3

import time
import socket

# for pings in range(10):
while True:
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client_socket.settimeout(1.0)
    message = b'test'
    addr = ("192.168.1.117", 53)
    client_socket.bind(addr)

    start = time.time()
    client_socket.sendto(message, addr)
    try:
        data, server = client_socket.recvfrom(1024)
        end = time.time()
        elapsed = end - start
        print(f'{data} {elapsed}')
    except socket.timeout:
        print('REQUEST TIMED OUT')