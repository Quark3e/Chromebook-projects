#!/usr/bin/env python3

from subprocess import Popen, PIPE
import os.path
import sys

absPath = os.path.realpath(__file__)[:-len("main.py")]

p = Popen(
    [absPath+'cppEXE'],
    shell=True,
    stdout=PIPE,
    stdin=PIPE
)

for ii in range(10):
    value = str(ii) + '\n'
    value = value.encode("utf-8")
    #value = bytes(value, 'UTF-8')  # Needed in Python 3.
    p.stdin.write(value)
    p.stdin.flush()
    result = p.stdout.readline().strip()
    print(result.decode("utf-8"))