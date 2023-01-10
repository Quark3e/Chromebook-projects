from IK_module import *
import os

mov_Patterns = { #NOTE: There cannot be any space in the keys
    "square1": [
    [-150, 250, 250, 0, 0, 0],
    [-50, 250, 250, 0, 0, 0],
    [-50, 50, 250, 0, -90, 0],
    [-150, 50, 250, 0, -90, 0],
    [-150, 250, 250, 0, 0, 0],
    [-150, 250, 50, 0, 0, 0],
    [-50, 250, 50, 0, 0, 0],
    [-50, 50, 50, 0, -90, 0],
    [-150, 50, 50, 0, -90, 0],
    [0, 250, 250, 0, 0, 0],
    [150, 250, 250, 0, 0, 0],
    [50, 250, 250, 0, 0, 0],
    [50, 50, 250, 0, -90, 0],
    [150, 50, 250, 0, -90, 0],
    [150, 250, 250, 0, 0, 0],
    [150, 250, 50, 0, 0, 0],
    [50, 250, 50, 0, 0, 0],
    [50, 50, 50, 0, -90, 0],
    [150, 50, 250, 0, -90, 0],
]
}

def fullAxisTest(servo):
    os.system("clear")
    isReachable = [True]
    startPos = [0,0,0]
    currentPos = [0,0,0]
    orient = [0,0,0]
    wsRange = [
        [-300, 300],
        [10, 400],
        [10, 400]
    ]
    print(" ---Full axis test---")
    opt = input(" enter start coordinate [x y z]:").split()
    if opt[0]=="exit": return
    startPos = [int(coord) for coord in opt]
    currentPos = startPos.copy()
    opt = input("\n enter orientation [a b Y]:").split()
    if opt[0]=="exit": return
    orient = [toRadians(float(angle)) for angle in opt]
    for axis in range(3):    
        for pos in range(startPos[axis],wsRange[axis][1]):
            currentPos[axis] = pos
            q = getAngles(currentPos,orient[0], orient[1], orient[2],
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(q,servo)
            time.sleep(0.001)    
        for pos in range(wsRange[axis][1],wsRange[axis][0],-1):
            currentPos[axis] = pos
            q = getAngles(currentPos,orient[0], orient[1], orient[2],
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(q,servo)
            time.sleep(0.001)   
        for pos in range(wsRange[axis][0],startPos[axis]):
            currentPos[axis] = pos
            q = getAngles(currentPos,orient[0], orient[1], orient[2],
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(q,servo)
            time.sleep(0.001)
    time.sleep(1)

    return

def fullOrientTest(servo):
    os.system("clear")
    isReachable = [True]
    testPos = [0,200,150]
    startOrient = [0,0,0]
    print(" ---Full axis test---")
    opt = input(" enter test coordinate [x y z]:").split()
    if opt[0]=="exit": return
    testPos = [int(coord) for coord in opt]

    opt = input("\n enter start orientation [a b Y]:").split()
    if opt[0]=="exit": return
    startOrient = [int(angle) for angle in opt]
    currentOrient = startOrient.copy()
    s = 6*[0]
    for axis in range(3):    
        for angle in range(startOrient[axis],90):
            currentOrient[axis] = angle
            q = getAngles(testPos,
            toRadians(currentOrient[0]),
            toRadians(currentOrient[1]),
            toRadians(currentOrient[2]),
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(q,servo)
            time.sleep(0.01)  

        for angle in range(90,-90,-1):
            currentOrient[axis] = angle
            q = getAngles(testPos,
            toRadians(currentOrient[0]),
            toRadians(currentOrient[1]),
            toRadians(currentOrient[2]),
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(q,servo)
            time.sleep(0.01)   

        for angle in range(-90,startOrient[axis]):
            currentOrient[axis] = angle
            q = getAngles(testPos,
            toRadians(currentOrient[0]),
            toRadians(currentOrient[1]),
            toRadians(currentOrient[2]),
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(q,servo)
            time.sleep(0.01)
    time.sleep(1)

    return


mov_Programs = {
    "axisTest": fullAxisTest,
    "orientTest": fullOrientTest
}
"""Dictionary of program names and the function

##Parameters:
    - assigned servo[n].angle list

"""
