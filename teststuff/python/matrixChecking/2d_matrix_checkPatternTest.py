#!/usr/bin/env python3

import os
import os.path
import time
import math

import sys
absPath = str(os.path.realpath(__file__)[:os.path.realpath(__file__)-len("2d_matrix_checkPatternTest.py")])
sys.path.append(absPath[:absPath.find("teststuff")]+"projects/proj_Hexclaw/HexclawMain/")
import IK_module as IK #type: ignore

fullRange = [18, 18]
centerPos = [4, 4]

testingMethod = 0
lst = []
printDraw = False

def toDraw_init():
    global toDraw
    toDraw = []
    for _ in range(fullRange[0]):
        # toDraw.append(fullRange[1]*[' '])
        toDraw.append(fullRange[1]*"■")

def addSpace(string):
    temp=""
    for i in range(len(string)-1):
        temp+=string[i:][:1]+" "
    temp+=string[len(string)-1:][:1]
    return temp

def removeCopies(oldList, occurance={}):
    """Iterates through a list and removes all subsequent copies/iterations
    ## Parameters:
    - oldList: list of items to remove copies from
    - occurance (optional): mutable dictionary stating how many times iterations of copies occur
        - `Note`: the first iteration is counted, so [1, 1] would returns 1: 2
    ## Returns:
    - cleaned list without copies
    """
    temp = []
    for a in range(len(oldList)):
        print(a)
        base = oldList[a]
        for b in range(len(temp)):
            if temp[b]==base:
                # if base in occurance.keys(): occurance[base]+=1
                # else: occurance[base]=2
                break
        else:
            temp.append(base)
    return temp


def check(val):
    global toDraw, fullRange
    # print(val)

    if val[0]<=fullRange[0]-1 and val[0]>=0 and val[1]<=fullRange[1]-1 and val[1]>=0:
        # toDraw[val[0]][val[1]]='o'
        toDraw[val[1]]=toDraw[val[1]][:val[0]]+" "+toDraw[val[1]][val[0]+1:]
        if printDraw: os.system("clear")
        for lst in toDraw:
            retStr = addSpace(lst)
            if printDraw: print(retStr)
        if printDraw:
            print(val)
            print("method:", testingMethod)
            time.sleep(0.01)
        # if val == [4,1]: input("point reached. paused...")
    return

def pathFind(axisRange, startPos, method=0):
    global testingMethod
    """Iterate through 2d array/list in a pattern
    ## Parameters:
    - axisRange [unit: integers]: length of biggest axis
    - startPos [unit: integers]: start position [x, y]
    - method [unit: integers]:
        - 0:
    """
    testingMethod = method
    t1 = time.perf_counter()
    # check(startPos)
    if method==0:
        for x in range(axisRange):
            for y in range(axisRange):
                check([x,y])
    elif method==1:
        for radius in range(1,axisRange):
            for x in range(radius-1):
                for y in range(radius-1):
                    check([startPos[0]-round(radius/2)+x, startPos[1]-round(radius/2)+y])
    elif method==2:
        # for radius in range(1,axisRange):
        #     for x in range(startPos[0]-radius, startPos[0]+radius+1):
        #         check([x, startPos[1]-radius])
        #         check([x, startPos[1]+radius])
        #     for y in range(startPos[1]-radius, startPos[1]+radius+1):
        #         check([startPos[0]-radius, y])
        #         check([startPos[0]+radius, y])

        for radius in range(1,axisRange):
            # for x in range(startPos[0]-radius, startPos[0]+radius+1):
            for x in range(radius+1):
                check([startPos[0]+x, startPos[1]-radius])
                check([startPos[0]-x, startPos[1]-radius])
                check([startPos[0]+x, startPos[1]+radius])
                check([startPos[0]-x, startPos[1]+radius])
            # for y in range(startPos[1]-radius, startPos[1]+radius+1):
            for y in range(radius+1):
                check([startPos[0]-radius, startPos[1]+y])
                check([startPos[0]+radius, startPos[1]+y])
                check([startPos[0]-radius, startPos[1]-y])
                check([startPos[0]+radius, startPos[1]-y])

    elif method==3:
        for radius in range(1,axisRange):
            for angle in range(0,360,1): #not viable
                check([
                    startPos[0]+round(math.sin(IK.toRadians(angle))*radius),
                    startPos[1]+round(math.cos(IK.toRadians(angle))*radius)
                ])
                # lst1.append([
                #     startPos[0]+round(math.sin(IK.toRadians(angle))*radius),
                #     startPos[1]+round(math.cos(IK.toRadians(angle))*radius)
                # ])
    elif method==4:
        # print(len(lst1))
        for _ in lst1:
            check(_)
    elif method==5:
        for x in range(axisRange+1):
            for y in range(axisRange+1):
                check([startPos[0]+x, startPos[1]+y])
                check([startPos[0]+x, startPos[1]-y])
                check([startPos[0]-x, startPos[1]+y])
                check([startPos[0]-x, startPos[1]-y])
    elif method==6:
        if startPos[1]<=round(fullRange[1]/2): B_dir=-1
        else: B_dir=1
        for x in range(axisRange+1):
            for y in range(axisRange+1):
                check([startPos[0]+x, startPos[1]+y*B_dir])
                check([startPos[0]-x, startPos[1]+y*B_dir])
            for y in range(axisRange+1):
                check([startPos[0]+x, startPos[1]+y*(-B_dir)])
                check([startPos[0]-x, startPos[1]+y*(-B_dir)])
    elif method==7:
        B_dir = [0, 0]
        for axis in range(len(startPos)):
            if startPos[axis]<=0: B_dir[axis]=-1
            else: B_dir[axis]=1
        
    else:
        return
    t2 = time.perf_counter()
    print(f"Total time: {round((t2-t1)*1000)}ms")
    return

def readOrientPath(listFile):
    global lst1

    lst1 = eval(listFile.readline())

def writeOrientPath(listFile):
    global lst1
    print(len(lst1))
    listFile.write(str(removeCopies(lst1)))
    print(len(removeCopies(lst1)))
    print("finished writing")
    return

def main():
    # # toDraw[centerPos[0]][centerPos[1]] = 's'
    # toDraw[centerPos[1]]=toDraw[centerPos[1]][:centerPos[0]]+"o"+toDraw[centerPos[1]][centerPos[0]+1:]
    global lst1, printDraw
    while True:
        toDraw_init()
        inp = input("enter mode:")
        if inp=="exit": break
        tempMode = int(inp)
        inp = input("print result? [y/n]:")
        if inp=="exit": break
        elif inp=="y": printDraw=True
        elif inp=="n": printDraw=False
        # lst1=[]
        # for lst in toDraw:
        #     print(lst)
        pathListFile = open("findOrientPath.dat", "r")
        readOrientPath(pathListFile)
        pathFind(fullRange[0],centerPos, tempMode)
        # # writeOrientPath(pathListFile)
        # print(IK.getNumFromString("150.233123123 6.23 420.21\n"," "))
        print()

    return


if __name__ == "__main__":
    toDraw_init()
    main()
