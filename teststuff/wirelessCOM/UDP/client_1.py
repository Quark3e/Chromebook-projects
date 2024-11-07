#!/usr/bin/env python3

import time
import socket


client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(1.5)
message = b'test'
addr = ("192.168.1.118", 1087)

# for pings in range(10):
while True:
    # client_socket.bind(("100.115.92.198", 53))
    start = time.time()
    client_socket.sendto(message, addr)
    try:
        data, server = client_socket.recvfrom(1024)
        end = time.time()
        elapsed = end - start
        print(f'data:{data.decode()} elapsed:{round(elapsed, 4)}')
    except socket.timeout:
        print('REQUEST TIMED OUT')
    time.sleep(0.01)
