#!/usr/bin/env python3

import os
import time

fullRange = [180, 180]
centerPos = [90, 45]
# toDraw = fullRange[0]*[fullRange[1]*[' ']]
# toDraw = [
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '],
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '],
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '],
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '],
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '],
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '],
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '],
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '],
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '],
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '],
#     [' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ']
# ]
toDraw = []
for _ in range(fullRange[0]):
    toDraw.append(fullRange[1]*[' '])


def check(val):
    global toDraw, fullRange
    #print(val)

    if val[0]<=fullRange[0]-1 and val[0]>=0 and val[1]<=fullRange[1]-1 and val[1]>=0:
        toDraw[val[0]][val[1]]='o'
        # os.system("clear")
        # for lst in toDraw:
        #     print(lst)
        # print(val)
        # time.sleep(0.05)
    return

def pathFind(axisRange, startPos):
    """Iterate through 2d array/list in a pattern
    ## Parameters:
    - axisRange [unit: integers]: length of biggest axis
    - startPos [unit: integers]: start position [x, y]
    """
    t1 = time.perf_counter()
    for radius in range(1,axisRange):
        for x in range(startPos[0]-radius, startPos[0]+radius+1):
            check([x, startPos[1]-radius])
            check([x, startPos[1]+radius])
        for y in range(startPos[1]-radius, startPos[1]+radius+1):
            check([startPos[0]-radius, y])
            check([startPos[0]+radius, y])
    t2 = time.perf_counter()
    print(f"Total time: {round((t2-t1)*1000)}ms")
    return


def main():
    toDraw[centerPos[0]][centerPos[1]] = 's'
    # for lst in toDraw:
    #     print(lst)
    pathFind(fullRange[0],centerPos)

    return


if __name__ == "__main__":
    main()
