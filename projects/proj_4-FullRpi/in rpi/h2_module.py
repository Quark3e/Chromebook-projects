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
    s = 6*[0]
    for axis in range(3):    
        for pos in range(startPos[axis],wsRange[axis][1]):
            currentPos[axis] = pos
            q = getAngles(currentPos,orient[0], orient[1], orient[2],
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(q,s,servo)
            time.sleep(0.001)    
        for pos in range(wsRange[axis][1],wsRange[axis][0],-1):
            currentPos[axis] = pos
            q = getAngles(currentPos,orient[0], orient[1], orient[2],
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(q,s,servo)
            time.sleep(0.001)   
        for pos in range(wsRange[axis][0],startPos[axis]):
            currentPos[axis] = pos
            q = getAngles(currentPos,orient[0], orient[1], orient[2],
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(q,s,servo)
            time.sleep(0.001)
    time.sleep(1)

    return

mov_Programs = {
    "axisTest": fullAxisTest
}
"""Dictionary of program names and the function

##Parameters:
    - assigned servo[n].angle list

"""


