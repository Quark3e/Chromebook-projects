from math import atan, pi

def mathTest(inp):
    return atan(float(inp))*180/pi


test = "HELLO WORLD"

def testFunc(inp):
    if inp == "YES": print("IT WORKED")
    elif inp == "NO": print("IT DIDNT READ")
    else: print("printing test: ", test)

def globalTest(x):
    global itWorks
    itWorks = x*2