#!/usr/bin/env python3

#import getpass

import os, time

from pyngrok import ngrok, conf

#print("Enter your authtoken, which can be copied from https://dashboard.ngrok.com/auth")
#conf.get_default().auth_token = getpass.getpass()

# Open a TCP ngrok tunnel to the SSH server
connection_string = ngrok.connect("2223", "tcp").public_url

ssh_url, port = connection_string.strip("tcp://").split(":")
print(f" * ngrok tunnel available, access with `ssh pi@{ssh_url} -p{port}`")

ngrok_process = ngrok.get_ngrok_process()

try:
    ngrok_process.proc.wait()
except KeyboardInterrupt:
    print("\n\nShutting down tunnel")
    ngrok.kill()
