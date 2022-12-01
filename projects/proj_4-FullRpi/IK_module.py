import sys
from math import isnan, pi, sin, cos, tan, asin, acos, atan, sqrt


def toDegrees(radians): return (radians * 180) / pi
def toRadians(degrees): return (degrees * pi) / 180

def getDistance(p1, p2):
    return sqrt(pow(p2[0]-p1[0], 2)+pow(p2[1]-p1[1], 2)+pow(p2[2]-p1[2], 2))

def getAngles(posX, posY, posZ, a, b, Y, posOption, length_scalar = 1, coord_scalar = 1, printText = False):
    global d1, d2, d3, d4, d5, d6, posX2, posY2, posZ2, a1_exceed, b1_exceed
    q=[0]*6

    posX = 0 - posX

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
        if posX2 > 0: q[0] = toRadians(90)
        elif posX2 < 0: q[0] = toRadians(-90)
        elif posX2 == 0: q[0] = toRadians(0)
    else: q[0] = atan(-posX2 / posY2)
    
    try:
        if posOption == '+': q[2] = acos((pow(posX2, 2) + pow(posY2, 2) + pow(posZ2 - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /(2 * d2 * (d3 + d4)))
        elif posOption == '-': q[2] = acos((pow(posX2, 2) + pow(posY2, 2) + pow(posZ2 - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /(2 * d2 * (d3 + d4)))
    except ValueError: print("domain error triggered")
    
    lambdaVar = atan((posZ2 - d1) / sqrt(pow(posX2, 2) + pow(posY2, 2)))
    muVar = atan(((d3 + d4) * sin(q[2])) /(d2 + (d3 + d4) * cos(q[2])))
    if posOption == '+': q[1] = lambdaVar - muVar
    elif posOption == '-':
        #if (s[1] < diffCheck and lambdaVar + muVar < 0) or (lambdaVar + muVar > 0):
        if lambdaVar + muVar > 0: q[1] = lambdaVar + muVar #NOTE: the negative muVar value hasnt been solved. so this is a temp. fix 
        else: print("q[1] error occured")
        q[2] = 0 - q[2]
    
    a1 = a - q[0]
    b1 = b - (q[1] + q[2])

    if toDegrees(a1)>90: a1_exceed = 1
    elif toDegrees(a1)<90: a1_exceed = -1
    if toDegrees(b1)>90: b1_exceed = 1
    elif toDegrees(b1)<90: b1_exceed = -1


    d5x = (d5+d6) * cos(b1) * sin(a1)
    #NOTE: The x and y axis of the X1Y1Z1 frame in the paper was reverse (compared to this. The names need to be changed!)
    #nvm, the frame X0Y0Z0: x axis flipped and then itäs flipped in the function
    d5z = (d5+d6) * sin(b1)

    if b1 == 0:
        if d5x > 0: q[3] = toRadians(90)
        elif d5x < 0: q[3] = toRadians(-90)
        elif d5x == 0: q[3] = toRadians(0)
    elif b1 < 0 or b1 > 0: q[3] = atan(d5x / d5z)
    checkVar = asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
    if isnan(checkVar):
        if printText: print("asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / d5)  is NaN")
    else:
        if sqrt(pow(d5x, 2) + pow(d5z, 2)) > 90: q[4] = toRadians(90)
        else: q[4] = asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6))
    if d5z < 0: q[4] = 0-q[4]
    if b <= pi / 2 and b >= 0 - pi / 2: q[5] = Y - q[3]
    elif b >= pi / 2 or b <= 0 - pi / 2: q[5] = pi - (Y - q[5])
    if printText:
        print(
            " q[0]: ", toDegrees(q[0]),
            " q[1]: ", toDegrees(q[1]),
            " q[2]: ", toDegrees(q[2]),
            " q[3]: ", toDegrees(q[3]),
            " q[4]: ", toDegrees(q[4]),
            " q[5]: ", toDegrees(q[5]),
        )
    return q



def correctionSetup():
    global allCorrect, centerAccurate, notAccurate
    global u_isAccurate, l_isAccurate, u_correction, l_correction

    allCorrect = True
    centerAccurate = [True, True, True, True, True, True] #True if 90 degrees is correct
    notAccurate = [False, False, False, False, False, False] #True if it's not accurate and needs to be fixed
    u_isAccurate, l_isAccurate = notAccurate, notAccurate
    u_correction, l_correction = [1, 1, 1, 1, 1, 1], [1, 1, 1, 1, 1, 1] #1 if it doesnt need correction

    opt = input(" Does any servo need to be corrected?\n input [y/n]: ")
    if opt == "exit":
        sys.exit()
    elif opt == "n" or opt == "":
        allCorrect = True
    elif opt == "y":
        allCorrect = False
        print("\n Servo correction: centerAccurate u_isAccurate l_isAccurate u_correction l_correction [y/n y/n y/n float float (1 if y)]: ")
        for i in range(6):
            print("\n servo q", i+1, ": ", sep='', end='')
            opt = input("").split()
            if opt[0] == "y": centerAccurate[i] = True
            elif opt[0] == "n": centerAccurate[i] = False; notAccurate[i] = True

            if opt[1] == "y": u_isAccurate[i] = True
            elif opt[1] == "n": u_isAccurate[i] = False; notAccurate[i] = True
            
            if opt[2] == "y": l_isAccurate[i] = True
            elif opt[2] == "n": l_isAccurate[i] = False; notAccurate[i] = True

            u_correction[i] = float(opt[3])
            l_correction[i] = float(opt[4])


def sendToServo():
    global q, default_q, s, servo, servoExceeded, whichServoExceeded, typeOfExceeded
    if not (isnan(q[0]) or
            isnan(q[1]) or
            isnan(q[2]) or
            isnan(q[3]) or
            isnan(q[4]) or
            isnan(q[5])):

        if default_q[0] - int(round(toDegrees(q[0]))) < 0: q[0] = toRadians(0 + default_q[0]); servoExceeded = True; whichServoExceeded[0] = True; typeOfExceeded[0] = "under"
        if default_q[1] + int(round(toDegrees(q[1]))) < 0: q[1] = toRadians(0 - default_q[1]); servoExceeded = True; whichServoExceeded[1] = True; typeOfExceeded[1] = "under"
        if default_q[2] + int(round(toDegrees(q[2]))) < 0: q[2] = toRadians(0 - default_q[2]); servoExceeded = True; whichServoExceeded[2] = True; typeOfExceeded[2] = "under"
        if default_q[3] + int(round(toDegrees(q[3]))) < 0: q[3] = toRadians(0 - default_q[3]); servoExceeded = True; whichServoExceeded[3] = True; typeOfExceeded[3] = "under"
        if default_q[4] + int(round(toDegrees(q[4]))) < 0: q[4] = toRadians(0 - default_q[4]); servoExceeded = True; whichServoExceeded[4] = True; typeOfExceeded[4] = "under"
        if default_q[5] + int(round(toDegrees(q[5]))) < 0: q[5] = toRadians(0 - default_q[5]); servoExceeded = True; whichServoExceeded[5] = True; typeOfExceeded[5] = "under"
        if default_q[0] - int(round(toDegrees(q[0]))) > 180: q[0] = toRadians(0 - default_q[0]); servoExceeded = True; whichServoExceeded[0] = True; typeOfExceeded[0] = "over"
        if default_q[1] + int(round(toDegrees(q[1]))) > 180: q[1] = toRadians(180 - default_q[1]); servoExceeded = True; whichServoExceeded[1] = True; typeOfExceeded[1] = "over"
        if default_q[2] + int(round(toDegrees(q[2]))) > 180: q[2] = toRadians(180 - default_q[2]); servoExceeded = True; whichServoExceeded[2] = True; typeOfExceeded[2] = "over"
        if default_q[3] + int(round(toDegrees(q[3]))) > 180: q[3] = toRadians(180 - default_q[3]); servoExceeded = True; whichServoExceeded[3] = True; typeOfExceeded[3] = "over"
        if default_q[4] + int(round(toDegrees(q[4]))) > 180: q[4] = toRadians(180 - default_q[4]); servoExceeded = True; whichServoExceeded[4] = True; typeOfExceeded[4] = "over"
        if default_q[5] + int(round(toDegrees(q[5]))) > 180: q[5] = toRadians(180 - default_q[5]); servoExceeded = True; whichServoExceeded[5] = True; typeOfExceeded[5] = "over"

        if servoExceeded:
            for i in range(6):
                if whichServoExceeded[i]:
                    print("\tServo motor: q", i+1, " exceeded \"", typeOfExceeded[i], "\"", sep='')
        s[5] = default_q[5] - int(round(toDegrees(q[5])))
        s[4] = 180 - default_q[4] - int(round(toDegrees(q[4])))
        s[3] = default_q[3] + int(round(toDegrees(q[3])))
        s[2] = 180 - default_q[2] - int(toDegrees(q[2]))
        s[1] = default_q[1] + int(round(toDegrees(q[1])))
        s[0] = default_q[0] - int(round(toDegrees(q[0])))

        for x in range(6):
            if notAccurate[i]:
                if centerAccurate[i]:
                    if not u_isAccurate: #90-180 not accurate
                        if s[x]>90: servo[x].angle = s[x] * u_correction
                        else: servo[x].angle = s[x]
                    if not l_isAccurate: #0-90 not accurate
                        if s[x]<90: servo[x].angle = s[x] * l_correction
                        else: servo[x].angle = s[x]
                elif not centerAccurate[i]:
                    servo[x].angle = s[x] * u_correction
            elif not notAccurate[i]:
                    # if x == 4:
                    #     if s[4]<90: servo[4].angle = getServo4Offset(180 - default_q[4] - s[4])
                    #     else: servo[4].angle = s[4]
                    # else: servo[x].angle = s[x]
                    servo[x].angle = s[x]


