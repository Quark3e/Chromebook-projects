#!/usr/bin/env python3




def check(val):
    return

def pathFind(axisRange, startPos):
    """Iterate through 2d array/list in a pattern
    ## Parameters:

    """
    for radius in range(axisRange):
        for x in range(startPos[0]-radius, startPos[0]+radius):
            check([x, startPos[1]-radius])
            check([x, startPos[1]+radius])
        for y in range(startPos[1]-radius, startPos[1]+radius):
            check([])
    return

def main():


    return



if __name__ == "__main__":
    main()