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
    value = f"[{ii:<1}" + "this:is:a:test:test]\n"
    print("sent: ", value[:-1])
    value = value.encode("utf-8")
    #value = bytes(value, 'UTF-8')  # Needed in Python 3.
    p.stdin.write(value)

    p.stdin.flush()
    result = p.stdout.readline()#.strip()
    print("recev:", result.decode("utf-8"), end="\n\n")