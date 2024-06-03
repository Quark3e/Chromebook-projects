

from IK_module import toDegrees, toRadians
from math import cos, sin, atan, asin, acos, sqrt, isnan, pi


d1 = 140 #axial "roll"
d2 = 135 #axial "pitch"
d3 = 70 #axial "pitch"
d4 = 80 #axial "roll"
d5 = 45 #axial "pitch
d6 = 45 #axial "roll" (?)


q1, q2, q3, q4, q5, q6 = 0, 0, 0, 0, 0, 0

def getAngles(posX, posY, posZ, a, b, Y, posOption, length_scalar = 1, coord_scalar = 1, printText = False):
    global d1, d2, d3, d4, d5, d6, q1, q2, q3, q4, q5, q6, q7, posX2, posY2, posZ2

    posX = posX * coord_scalar
    posY = posY * coord_scalar
    posZ = posZ * coord_scalar
    d1 = d1 * length_scalar
    d2 = d2 * length_scalar
    d3 = d3 * length_scalar
    d4 = d4 * length_scalar
    d5 = d5 * length_scalar
    d6 = d6 * length_scalar
    l = (d5+d6) * cos(b)
    posX2 = posX - l * sin(a)
    posY2 = posY - l * cos(a)
    posZ2 = posZ - (d5+d6) * sin(b)

    if posY2 < 0:
        posY2 = 0
    if posY2 == 0:
        if posX2 > 0: q1 = toRadians(90)
        elif posX2 < 0: q1 = toRadians(-90)
        elif posX2 == 0: q1 = toRadians(0)
    else: q1 = atan(posX2 / posY2)
    
    try:
        if posOption == '+': q3 = acos((pow(posX2, 2) + pow(posY2, 2) + pow(posZ2 - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /(2 * d2 * (d3 + d4)))
        elif posOption == '-': q3 = acos((pow(posX2, 2) + pow(posY2, 2) + pow(posZ2 - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /(2 * d2 * (d3 + d4)))
    except ValueError: print("domain error triggered")
    
    print(posX2, posY2, posZ2)
    print(((d3+d4)*sin(q3)))

    lambdaVar = atan((posZ2 - d1) / sqrt(pow(posX2, 2) + pow(posY2, 2)))
    muVar = atan(((d3 + d4) * sin(q3)) / (d2 + (d3 + d4) * cos(q3)))
    
    print(toDegrees(muVar), toDegrees(lambdaVar))

    if posOption == '+': q2 = lambdaVar - muVar
    elif posOption == '-':
        #if (s[1] < diffCheck and lambdaVar + muVar < 0) or (lambdaVar + muVar > 0):
        if lambdaVar + muVar > 0: q2 = lambdaVar + muVar #NOTE: the negative muVar value hasnt been solved. so this is a temp. fix 
        else: print("q2 error occured")
        q3 = 0 - q3
    
    a1 = a - q1
    b1 = b - (q2 + q3)
    d5x = (d5+d6) * cos(b1) * sin(a1)
    #NOTE: The x and y axis of the X1Y1Z1 frame in the paper was reverse (compared to this. The names need to be changed!)
    d5z = (d5+d6) * sin(b1)

    if b1 == 0:
        if d5x > 0: q4 = toRadians(90)
        elif d5x < 0: q4 = toRadians(-90)
        elif d5x == 0: q4 = toRadians(0)
    elif b1 < 0 or b1 > 0: q4 = atan(d5x / d5z)
    checkVar = asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
    if isnan(checkVar):
        if printText: print("asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / d5)  is NaN")
    else:
        if sqrt(pow(d5x, 2) + pow(d5z, 2)) > 90: q5 = toRadians(90)
        else: q5 = asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
    if d5z < 0: q5 = 0-q5
    if b <= pi / 2 and b >= 0 - pi / 2: q6 = Y - q4
    elif b >= pi / 2 or b <= 0 - pi / 2: q6 = pi - (Y - q6)
    if printText:
        print(
            " q1: ", toDegrees(q1),
            " q2: ", toDegrees(q2),
            " q3: ", toDegrees(q3),
            " q4: ", toDegrees(q4),
            " q5: ", toDegrees(q5),
            " q6: ", toDegrees(q6),
        )

getAngles(0, 200, 200, 0, 0, 0, '-', 1)

print(toDegrees(q1), toDegrees(q2), toDegrees(q3), toDegrees(q4), toDegrees(q5), toDegrees(q6))