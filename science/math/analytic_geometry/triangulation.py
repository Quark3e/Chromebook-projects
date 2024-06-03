#!/usr/bin/env python

from math import sqrt, atan, acos, asin, pi, cos, sin, tan

toRadians = lambda degrees: (degrees*pi)/180
toDegrees = lambda radians: (radians*180)/pi

def get_2dDist(A: list, B: list):
    return abs(sqrt((B[0]-A[0])**2+(B[1]-A[1])**2))

def solvePos(
        towers: list,
        radia: list,
        ):
    """Triangulate the position of unidentified object from reference positions(towers) and respective radiuses between each tower and the object

    Args:
        towers (list) {[[x,y], [x,y], [x, y]]}: List of 2D coordinates of each reference point
        radia (list) {[towers[0], towers[1], towers[2]]}: Respective radius between each tower and the object
    Returns:
        solvedPos: 2d list of solved position
        
    """
    solvedPos = [None, None]

    q_a = atan((towers[1][1]-towers[0][1])/(towers[1][0]-towers[0][0]))

    hypot_01 = sqrt((towers[0][0]-towers[1][0])**2+(towers[0][1]-towers[1][1])**2)
    q_p = acos((radia[0]**2+radia[1]**2-hypot_01**2) / (2*radia[0]*radia[1]))
    q_0 = acos((radia[0]**2+hypot_01**2-radia[1]**2) / (2*radia[0]*hypot_01))
    q_1 = acos((radia[1]**2+hypot_01**2-radia[0]**2) / (2*radia[1]*hypot_01))

    testPos = [
        [towers[0][0]+cos(q_a+q_0)*radia[0], towers[0][1]+sin(q_a+q_0)*radia[0]],
        [towers[0][0]+cos(q_a-q_0)*radia[0], towers[0][1]+sin(q_a-q_0)*radia[0]]
    ]
    diff = [abs(get_2dDist(testPos[0], towers[2])-radia[2]), abs(get_2dDist(testPos[1], towers[2])-radia[2])]

    if diff[0]<diff[1]: solvedPos = testPos[0]
    elif diff[0]>diff[1]: solvedPos = testPos[1]
    else: solvedPos = testPos[0]

    return solvedPos