#!/usr/bin/env python3

import os, time

from pyngrok import ngrok


# Open a HTTP tunnel on the default port 80
# <NgrokTunnel: "https://<public_sub>.ngrok.io" -> "http://localhost:80">
http_tunnel = ngrok.connect()

# Open a SSH tunnel
# <NgrokTunnel: "tcp://0.tcp.ngrok.io:12345" -> "localhost:22">
ssh_tunnel = ngrok.connect("2223", "tcp")

# Open a named tunnel from the config file
named_tunnel = ngrok.connect(name="my_tunnel_name")
print(named_tunnel)

ngrok_process = ngrok.get_ngrok_process()

try:
    ngrok_process.proc.wait()
except KeyboardInterrupt:
    print("Shutting down tunnel")
    ngrok.kill()
