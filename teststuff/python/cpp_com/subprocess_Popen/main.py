#!/usr/bin/env python3

from subprocess import Popen, PIPE
import os.path
import sys

absPath = os.path.realpath(__file__)[:-len("main.py")]


cmdArgsList = [absPath+"cppEXE"]

for i in sys.argv[1:]:
    print("appended:", i)
    cmdArgsList.append(i)

print("full cmd arguments:", cmdArgsList)

p = Popen(
    cmdArgsList,
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
    resultStr = result.decode("utf-8")
    print("type:", type(resultStr), " recev:", resultStr, end="\n\n")