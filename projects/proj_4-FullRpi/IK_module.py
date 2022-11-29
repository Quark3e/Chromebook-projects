import sys
from math import isnan, pi, sin, cos, tan, asin, acos, atan, sqrt

def toDegrees(radians): return (radians * 180) / pi
def toRadians(degrees): return (degrees * pi) / 180

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
    
    lambdaVar = atan((posZ2 - d1) / sqrt(pow(posX2, 2) + pow(posY2, 2)))
    muVar = atan(((d3 + d4) * sin(q3)) /(d2 + (d3 + d4) * cos(q3)))
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
    global q1, q2, q3, q4, q5, q6, s, servo, servoExceeded, whichServoExceeded, typeOfExceeded
    if not (isnan(q1) or
            isnan(q2) or
            isnan(q3) or
            isnan(q4) or
            isnan(q5) or
            isnan(q6)):
        if q1_default - int(round(toDegrees(q1))) < 0: q1 = toRadians(0 + q1_default); servoExceeded = True; whichServoExceeded[0] = True; typeOfExceeded[0] = "under"
        if q2_default + int(round(toDegrees(q2))) < 0: q2 = toRadians(0 - q2_default); servoExceeded = True; whichServoExceeded[1] = True; typeOfExceeded[1] = "under"
        if q3_default + int(round(toDegrees(q3))) < 0: q3 = toRadians(0 - q3_default); servoExceeded = True; whichServoExceeded[2] = True; typeOfExceeded[2] = "under"
        if q4_default + int(round(toDegrees(q4))) < 0: q4 = toRadians(0 - q4_default); servoExceeded = True; whichServoExceeded[3] = True; typeOfExceeded[3] = "under"
        if q5_default + int(round(toDegrees(q5))) < 0: q5 = toRadians(0 - q5_default); servoExceeded = True; whichServoExceeded[4] = True; typeOfExceeded[4] = "under"
        if q6_default + int(round(toDegrees(q6))) < 0: q6 = toRadians(0 - q6_default); servoExceeded = True; whichServoExceeded[5] = True; typeOfExceeded[5] = "under"
        if q1_default - int(round(toDegrees(q1))) > 180: q1 = toRadians(0 - q1_default); servoExceeded = True; whichServoExceeded[0] = True; typeOfExceeded[0] = "over"
        if q2_default + int(round(toDegrees(q2))) > 180: q2 = toRadians(180 - q2_default); servoExceeded = True; whichServoExceeded[1] = True; typeOfExceeded[1] = "over"
        if q3_default + int(round(toDegrees(q3))) > 180: q3 = toRadians(180 - q3_default); servoExceeded = True; whichServoExceeded[2] = True; typeOfExceeded[2] = "over"
        if q4_default + int(round(toDegrees(q4))) > 180: q4 = toRadians(180 - q4_default); servoExceeded = True; whichServoExceeded[3] = True; typeOfExceeded[3] = "over"
        if q5_default + int(round(toDegrees(q5))) > 180: q5 = toRadians(180 - q5_default); servoExceeded = True; whichServoExceeded[4] = True; typeOfExceeded[4] = "over"
        if q6_default + int(round(toDegrees(q6))) > 180: q6 = toRadians(180 - q6_default); servoExceeded = True; whichServoExceeded[5] = True; typeOfExceeded[5] = "over"
        if servoExceeded:
            for i in range(6):
                if whichServoExceeded[i]:
                    print("\tServo motor: q", i+1, " exceeded \"", typeOfExceeded[i], "\"", sep='')
        s[5] = q6_default - int(round(toDegrees(q6)))
        s[4] = 180 - q5_default - int(round(toDegrees(q5)))
        s[3] = q4_default + int(round(toDegrees(q4)))
        s[2] = 180 - q3_default - int(toDegrees(q3))
        s[1] = q2_default + int(round(toDegrees(q2)))
        s[0] = q1_default - int(round(toDegrees(q1)))
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
                    #     if s[4]<90: servo[4].angle = getServo4Offset(180 - q5_default - s[4])
                    #     else: servo[4].angle = s[4]
                    # else: servo[x].angle = s[x]
                    servo[x].angle = s[x]
    if firstAnglePrint:
            print(
                " read:\n"
                " q1:", toDegrees(q1), 
                " q2:", toDegrees(q2),
                " q3:", toDegrees(q3),
                " q4:", toDegrees(q4),
                " q5:", toDegrees(q5), 
                " q6:", toDegrees(q6),
            )
    if globalPrint or endAnglePrint:
            print(
                " Sent: q1:", servo[0].angle, 
                " q2:", servo[1].angle,
                " q3:", servo[2].angle,
                " q4:", servo[3].angle,
                " q5:", servo[4].angle, 
                " q6:", servo[5].angle,
                # " Roll:", Roll,
                # " Pitch:", Pitch
            )

