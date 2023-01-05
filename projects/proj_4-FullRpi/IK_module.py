import sys
import time
# from math import isnan, pi, sin, cos, tan, asin, acos, atan, sqrt
from math import * #type: ignore

d1 = 140; #axial "roll"
d2 = 135; #axial "pitch"
d3 = 70; #axial "pitch"
d4 = 80; #axial "roll"
d5 = 45; #axial "pitch
d6 = 45; #axial "roll" (?)
link = [d1,d2,d3,d4,d5,d6]
default_q = [90, 0, 135, 90, 90, 90]


constants_q = [{
    "default":1.2466666666666668,
    "fixed":0.802139037433155
},
{
    "default":1,
    "fixed":1
},
{
    "default":1,
    "fixed":1
},
{
    "default":1,
    "fixed":1
},
{
    "default":1,
    "fixed":1
},
{
    "default":1,
    "fixed":1
}]


# allCorrect = True
# centerAccurate = [True, True, True, True, True, True] #True if 90 degrees is correct
# notAccurate = [False, False, False, False, False, False] #True if it's not accurate and needs to be fixed
# u_isAccurate, l_isAccurate = notAccurate, notAccurate
# u_correction, l_correction = [1, 1, 1, 1, 1, 1], [1, 1, 1, 1, 1, 1] #1 if it doesnt need correction

def q5Fix(q5):
    '''
    q5 (float/int): q[4] or s[4] [degrees]
    '''
    return (default_q[4]-((default_q[4]-q5)*default_q[4])/135)

def toDegrees(radians): return (radians * 180) / pi
def toRadians(degrees): return (degrees * pi) / 180

def getDistance(p1, p2):
    return sqrt(pow(p2[0]-p1[0], 2)+pow(p2[1]-p1[1], 2)+pow(p2[2]-p1[2], 2))

def custom_sendToServo(servo, new_rotation, total_time, useDefault = False):
    '''
    Args:
        s (float/int): dictionary/list variable that sends to pca board / holds old/current rotation commands
        new_rotation (float/int): dictionary/list variable of new rotation commands
        total_iteration (int): number of loop iterations to complete the movement
        total_time (float/int): Total time spent moving the servo from start to finish in seconds
    '''
    if useDefault:
        new_rotation[5] = default_q[5] + int(round(toDegrees(new_rotation[5])))
        new_rotation[4] = 180 - default_q[4] - int(round(toDegrees(new_rotation[4])))
        new_rotation[3] = default_q[3] + int(round(toDegrees(new_rotation[3])))
        new_rotation[2] = 180 - default_q[2] - int(toDegrees(new_rotation[2]))
        new_rotation[1] = default_q[1] + int(round(toDegrees(new_rotation[1])))
        new_rotation[0] = default_q[0] - int(round(toDegrees(new_rotation[0])))
    
    for q in range(6):
        new_rotation[q] = new_rotation[q] * constants_q[q]["fixed"]

        # for x in range(6):
        #     if notAccurate[x]:
        #         if centerAccurate[x]:
        #             if not u_isAccurate: #90-180 not accurate
        #                 if new_rotation[x]>90: new_rotation[x] = new_rotation[x] * u_correction[x]
        #             if not l_isAccurate: #0-90 not accurate
        #                 if new_rotation[x]<90: new_rotation[x] = new_rotation[x] * l_correction[x]
        #         elif not centerAccurate[x]:
        #             new_rotation[x] = new_rotation[x] * u_correction[x]
            
    total_iteration = 180
    s_diff = {}
    for i in range(6): s_diff[i] = new_rotation[i]-servo[i].angle
    s_temp = []
    for i in range(6): s_temp.append(servo[i].angle)
    for counter in range(total_iteration-1):
        for i in range(6):
            s_temp[i] += s_diff[i]/total_iteration
            servo[i].angle = s_temp[i]
        time.sleep(total_time/total_iteration)


def exceedCheck(q, servoExceeded, whichServoExceeded, typeOfExceeded):
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
            if whichServoExceeded[i]: print("\tServo motor: q", i+1, " exceeded \"", typeOfExceeded[i], "\"", sep='')
    
    return [servoExceeded,whichServoExceeded,typeOfExceeded]


def check_isNaN(q, printText = False):
    '''
    Returns True if any rotation is not a number (isNaN)
        else it returns False
    '''
    if not (isnan(q[0]) or
            isnan(q[1]) or
            isnan(q[2]) or
            isnan(q[3]) or
            isnan(q[4]) or
            isnan(q[5])):
            return False
    else:
        if printText: print("a joint value is NaN")
        return True


def getAngles(
    PP,a,b,Y,posOption,
    length_scalar=1,coord_scalar=1,
    printText=False,printErrors=True, forShow=False, 
    debug=[
        False,
        "none"
    ], 
    positionIsReachable=[True]
    ):
    '''
    Solves and returns all the rotation values
    also returns if the point is reachable (i.e. no joints is NaN)
    Args:
        PP (float/int) [mm]: The end-effector position in list for
        a (float) [radians]: alpha orientation variable
        b (float) [radians]: beta orientation variable
        Y (float) [radians]: gamma orienation variable
        posOption (str/char): different mode to use
            '-' is q3(/q[2]) above line between P2 and P4/P5;
            '+' is q3(/q[2]) under line between P2 and P4/P5
        debug [boolean(whether to use default or debug), string(what to use)]: [1]="q4" if only q4 mod is used, [1]="q5" if only q5 mod is used, [1]="both" if both is used 
    '''
    positionIsReachable[0] = True

    a1_exceed, b1_exceed = 0, 0
    q=[0]*6
    P5 = [0]*3
    # PP[0] = 0 - PP[0]
    posX = 0 - PP[0]
    # PP[0] = PP[0] * coord_scalar
    # PP[1] = PP[1] * coord_scalar
    # PP[2] = PP[2] * coord_scalar
    link[0] = link[0] * length_scalar
    link[1] = link[1] * length_scalar
    link[2] = link[2] * length_scalar
    link[3] = link[3] * length_scalar
    link[4] = link[4] * length_scalar
    link[5] = link[5] * length_scalar
    l = (link[4]+link[5]) * cos(b)
    P5[0] = posX - l * sin(a)
    P5[1] = PP[1] - l * cos(a)
    P5[2] = PP[2] - (link[4]+link[5]) * sin(b)

    if printText: print(" P5 coords:",[round(pos) for pos in P5], sep='')

    if P5[1] < 0:
        P5[1] = 0
    if P5[1] == 0:
        if P5[0] > 0: q[0] = toRadians(90) # type: ignore
        elif P5[0] < 0: q[0] = toRadians(-90) # type: ignore
        elif P5[0] == 0: q[0] = toRadians(0) # type: ignore
    else: q[0] = atan(-P5[0] / P5[1]) # type: ignore
    a = 0-a
    try:
        if posOption == '+': q[2] = acos((pow(P5[0], 2) + pow(P5[1], 2) + pow(P5[2] - link[0], 2) - pow(link[1], 2) - pow(link[2] + link[3], 2)) /(2 * link[1] * (link[2] + link[3]))) # type: ignore
        elif posOption == '-': q[2] = acos((pow(P5[0], 2) + pow(P5[1], 2) + pow(P5[2] - link[0], 2) - pow(link[1], 2) - pow(link[2] + link[3], 2)) /(2 * link[1] * (link[2] + link[3]))) # type: ignore
    except ValueError: 
        if printErrors: print("domain error triggered")
        positionIsReachable[0] = False
    # print(P5)
    
    lambdaVar, muVar = 0, 0

    try:
        lambdaVar = atan((P5[2] - link[0]) / sqrt(pow(P5[0], 2) + pow(P5[1], 2)))
    except ZeroDivisionError:
        positionIsReachable[0] = False
    muVar = atan(((link[2] + link[3]) * sin(q[2])) /(link[1] + (link[2] + link[3]) * cos(q[2])))
    if printText: print(" lambda:",round(toDegrees(lambdaVar))," mu:",round(toDegrees(muVar)),sep='')
    if posOption == '+': q[1] = lambdaVar - muVar # type: ignore
    elif posOption == '-':
        #if (s[1] < diffCheck and lambdaVar + muVar < 0) or (lambdaVar + muVar > 0):
        if lambdaVar + muVar > 0: q[1] = lambdaVar + muVar # type: ignore #NOTE: the negative muVar value hasnt been solved. so this is a temp. fix 
        else: 
            if printErrors: print("q[1] error occured")
            positionIsReachable[0] = False
        q[2] = 0 - q[2] # type: ignore
    
    a1 = a - q[0]
    b1 = b - (q[1] + q[2])
    if not forShow and (round(toDegrees(b)) == 90 or round(toDegrees(b) == -90)): a1 = 0
    if printText: print(" a1:",round(toDegrees(a1))," b1:",round(toDegrees(b1)),sep='')
    
    if toDegrees(a1)>90: a1_exceed = 1
    elif toDegrees(a1)<-90: a1_exceed = -1
    if toDegrees(b1)>90: b1_exceed = 1
    elif toDegrees(b1)<-90: b1_exceed = -1
    
    if b1_exceed!=0 or a1_exceed!=0:
        if printErrors:
            if a1_exceed!=0: print(" a1 exceeded by", a1_exceed*90, end='')
            if b1_exceed!=0: print(" b1 exceeded by", b1_exceed*90, end='')
            print()
    frame1X = (link[4]+link[5]) * cos(b1) * sin(a1)
    #NOTE: The x and y axis of the X1Y1Z1 frame in the paper was reverse (compared to this. The names need to be changed!)
    #nvm, the frame X0Y0Z0: x axis flipped and then it's flipped in the function
    frame1Z = (link[4]+link[5]) * sin(b1)
    frame1Y = (link[4]+link[5]) * cos(b1) * cos(a1)

    if printText: print(" frame1_x:",round(frame1X)," frame1_y:",round(frame1Y)," frame1_z:",round(frame1Z), sep='')

    if b1 == 0:
        if frame1X > 0: q[3] = toRadians(90)  # type: ignore
        elif frame1X < 0: q[3] = toRadians(-90) # type: ignore
        elif frame1X == 0: q[3] = toRadians(0) # type: ignore
        if printText: print(" b1 was 0 so q4 was adjusted")
    elif b1 < 0 or b1 > 0: q[3] = atan(frame1X / frame1Z) # type: ignore
    
    if debug[0] and (debug[1]=="q4" or debug[1]=="both"):
        q[3] = a1
        print(toDegrees(q[3]))

    try:
        checkVar = asin(sqrt(pow(frame1X, 2) + pow(frame1Z, 2)) / (link[4]+link[5]))
        if isnan(checkVar):
            if printErrors:
                print("asin(sqrt(pow(frame1X, 2) + pow(frame1Z, 2)) / link[4])  is NaN")
            positionIsReachable[0] = False
        else:
            q[4] = asin(sqrt(pow(frame1X, 2) + pow(frame1Z, 2)) / (link[4]+link[5])) # type: ignore
            # q[4] = atan(sqrt(pow(frame1X, 2) + pow(frame1Z, 2)) / (cos(b1)*cos(a1))) # type: ignore
    except ValueError:
        positionIsReachable[0] = False
    
    if debug[0] and (debug[1]=="q5" or debug[1]=="both"):
        q[4] = atan(sqrt(pow(frame1X,2)+pow(frame1Z,2))/(cos(b1)*cos(a1))) #type: ignore
        print(toDegrees(q[4]))

    if frame1Z < 0:
        q[4] = 0-q[4] # type: ignore
        if printText: print(" frame1_z was below 0 so q5 is subtracted from 0")
    if b <= pi / 2 and b >= 0 - pi / 2: q[5] = Y - q[3]
    elif b >= pi / 2 or b <= 0 - pi / 2: q[5] = pi - (Y - q[5])
    if printText: print(" ",[round(toDegrees(n)) for n in q])
    if check_isNaN(q):
        positionIsReachable[0] = False
        print("one joint is NaN")

    return q


def getSubframe(PP,a,b,posOption,printText=False):
    '''
    Returns xyz distances in sub-frame 1 and a1, b1

    Args:
        PP (float/int) [mm]: The end-effector position in list for
        a (float) [radians]: alpha orientation variable
        b (float) [radians]: beta orientation variable
        posOption (str/char): different mode to use
            '-' is q3(/q[2]) above line between P2 and P4/P5;
            '+' is q3(/q[2]) under line between P2 and P4/P5
    '''
    global frame1X, frame1Y, frame1Z
    a1_exceed, b1_exceed = 0, 0
    q=[0]*6
    P5 = [0]*3
    # PP[0] = 0 - PP[0]
    posX = 0 - PP[0]
    l = (link[4]+link[5]) * cos(b)
    P5[0] = posX - l * sin(a)
    P5[1] = PP[1] - l * cos(a)
    P5[2] = PP[2] - (link[4]+link[5]) * sin(b)

    if printText: print(" P5 coords:",[round(pos) for pos in P5], sep='')

    if P5[1] < 0:
        P5[1] = 0
    if P5[1] == 0:
        if P5[0] > 0: q[0] = toRadians(90) # type: ignore
        elif P5[0] < 0: q[0] = toRadians(-90) # type: ignore
        elif P5[0] == 0: q[0] = toRadians(0) # type: ignore
    else: q[0] = atan(-P5[0] / P5[1]) # type: ignore
    
    try:
        if posOption == '+': q[2] = acos((pow(P5[0], 2) + pow(P5[1], 2) + pow(P5[2] - link[0], 2) - pow(link[1], 2) - pow(link[2] + link[3], 2)) /(2 * link[1] * (link[2] + link[3]))) # type: ignore
        elif posOption == '-': q[2] = acos((pow(P5[0], 2) + pow(P5[1], 2) + pow(P5[2] - link[0], 2) - pow(link[1], 2) - pow(link[2] + link[3], 2)) /(2 * link[1] * (link[2] + link[3]))) # type: ignore
    except ValueError: print("domain error triggered")
    
    lambdaVar = atan((P5[2] - link[0]) / sqrt(pow(P5[0], 2) + pow(P5[1], 2)))
    muVar = atan(((link[2] + link[3]) * sin(q[2])) /(link[1] + (link[2] + link[3]) * cos(q[2])))
    if printText: print(" lambda:",round(toDegrees(lambdaVar))," mu:",round(toDegrees(muVar)),sep='')
    if posOption == '+': q[1] = lambdaVar - muVar # type: ignore
    elif posOption == '-':
        #if (s[1] < diffCheck and lambdaVar + muVar < 0) or (lambdaVar + muVar > 0):
        if lambdaVar + muVar > 0: q[1] = lambdaVar + muVar # type: ignore #NOTE: the negative muVar value hasnt been solved. so this is a temp. fix 
        else: print("q[1] error occured")
        q[2] = 0 - q[2] # type: ignore
    
    a1 = a - q[0]
    b1 = b - (q[1] + q[2])

    if toDegrees(b) == 90 or toDegrees(b) == -90:
        a1 = 0

    if printText: print(" a1:",round(toDegrees(a1))," b1:",round(toDegrees(b1)),sep='')
    
    if toDegrees(a1)>90: a1_exceed = 1
    elif toDegrees(a1)<-90: a1_exceed = -1
    if toDegrees(b1)>90: b1_exceed = 1
    elif toDegrees(b1)<-90: b1_exceed = -1
    
    if b1_exceed!=0 or a1_exceed!=0:
        if a1_exceed!=0: print(" a1 exceeded by", a1_exceed*90, end='')
        if b1_exceed!=0: print(" b1 exceeded by", b1_exceed*90, end='')
        print()

    frame1X = (link[4]+link[5]) * cos(b1) * sin(a1)
    #NOTE: The x and y axis of the X1Y1Z1 frame in the paper was reverse (compared to this. The names need to be changed!)
    #nvm, the frame X0Y0Z0: x axis flipped and then it's flipped in the function
    frame1Z = (link[4]+link[5]) * sin(b1)
    frame1Y = (link[4]+link[5]) * cos(b1) * cos(a1)
    return [frame1X,frame1Y,frame1Z,a1,b1]


# def correctionSetup():
#     global allCorrect, centerAccurate, notAccurate
#     global u_isAccurate, l_isAccurate, u_correction, l_correction


#     opt = input(" Does any servo need to be corrected?\n input [y/n]: ")
#     if opt == "exit":
#         sys.exit()
#     elif opt == "n" or opt == "":
#         allCorrect = True
#     elif opt == "y":
#         allCorrect = False
#         print("\n Servo correction: centerAccurate u_isAccurate l_isAccurate u_correction l_correction [y/n y/n y/n float float (1 if y)]: ")
#         print(notAccurate)
#         for i in range(6):
#             opt = 5*[] #opt has 5 elements
#             print("\n servo q", i+1, ": ", sep='', end='')
#             opt = input("").split()
#             if opt[0] == "y": centerAccurate[i] = True
#             elif opt[0] == "n": centerAccurate[i] = False

#             if opt[1] == "y": u_isAccurate[i] = True
#             elif opt[1] == "n": u_isAccurate[i] = False
            
#             if opt[2] == "y": l_isAccurate[i] = True
#             elif opt[2] == "n": l_isAccurate[i] = False

#             if centerAccurate[i] and u_isAccurate[i] and l_isAccurate[i]: notAccurate[i] = False
#             else: notAccurate[i] = True

#             u_correction[i] = float(opt[3]) # type: ignore
#             l_correction[i] = float(opt[4]) # type: ignore
#             # print(notAccurate[i])
#         print(notAccurate)


def sendToServo(q, s, servo, servoExceeded, whichServoExceeded, typeOfExceeded):
    '''
    Purpose: sends given rotation commands directly, one motor at a time, to the servomotors with default_q values set in functions

    Args:
        q (float/int): dictionary/list variable of new rotation commands
        s (float/int): empty/0 dictionary/list that are only used in this function (too lazy to edit every file that uses this function)
        servo (float/int): dictionary/list variable that sends to pca board / holds old/current rotation commands
        servoExceeded (boolean): dictionary/list
        whichServoExceeded (boolean): dictionary/list
        typeOfExceeded (string): dictionary/list
    '''
    
    if not check_isNaN(q):
        servoExceeded, whichServoExceeded, typeOfExceeded = exceedCheck(q,servoExceeded,whichServoExceeded,typeOfExceeded)

        s[5] = default_q[5] + int(round(toDegrees(q[5])))
        s[4] = 180 - default_q[4] - int(round(toDegrees(q[4])))
        s[3] = default_q[3] + int(round(toDegrees(q[3])))
        s[2] = 180 - default_q[2] - int(toDegrees(q[2]))
        s[1] = default_q[1] + int(round(toDegrees(q[1])))
        s[0] = default_q[0] - int(round(toDegrees(q[0])))

        # print(notAccurate)
        # print(u_correction, l_correction)
        for x in range(6):
            # print("q",x+1,": u_corr:",u_correction[x]," l_corr:",l_correction[x],sep='',end='')
            # print(" angle:", s[x],sep='',end='')
            # print(" notAccurate[x]:",notAccurate[x], " centerAccurate[x]:", centerAccurate[x],sep='',end='')
            if x==4: servo[4].angle = q5Fix(s[4])
            else: servo[x].angle = s[x] * constants_q[x]["fixed"]


def getP1():
    return [0, 0, 0]
def getP2():
    return (0, 0, d1)
def getP3(q1, q2): #in radians
    return [
        d2*cos(q2)*sin(q1),
        d2*cos(q2)*cos(q1),
        d1+d2*sin(q2)
        ]
def getP4(P3, q1, q2, q3): #in radians
    return [
        P3[0]+d3*cos(q2+q3)*sin(q1),
        P3[1]+d3*cos(q2+q3)*cos(q1),
        P3[2]+d3*sin(q2+q3)
        ]
def getP5(P3, q1, q2, q3): #in radians
    return [
        P3[0]+(d3+d4)*cos(q2+q3)*sin(q1),
        P3[1]+(d3+d4)*cos(q2+q3)*cos(q1),
        P3[2]+(d3+d4)*sin(q2+q3)
        ]

def getPP(P5, q1, q2, q3, q4, q5): #in radians
    global a_read, b_read
    # return [
    #     P5[0]+(d5+d6)*cos(b)*sin(a),
    #     P5[1]+(d5+d6)*cos(b)*cos(a),
    #     P5[2]+(d5+d6)*sin(b)
    # ]
    b_read = asin(((d5+d6)*sin(q5)*cos(q4))/(d5+d6))+q2+q3
    if int(toDegrees(q5)) == 90 or int(toDegrees(q5)) == -90:
        if ((d5+d6)*sin(q5)*cos(q4)) == 0:
            a_read = 0
        elif ((d5+d6)*sin(q5)*cos(q4)) > 0:
            a_read = toRadians(90)
        elif ((d5+d6)*sin(q5)*cos(q4)) < 0:
            a_read = toRadians(-90)
    else:
        a_read = q1-atan(
            ((d5+d6)*sin(q5)*sin(q4)) / ((d5+d6)*cos(q5))
            )
    print("calculated: alpha: ", int(round(toDegrees(a_read))), " beta:", int(round(toDegrees(b_read))), 
    "   \tsolved y:", int(round((d5+d6)*cos(b_read)*cos(a_read))), 
    " \tgiven: q4:", int(round(toDegrees(q4))), " \tgiven: q5:", int(round(toDegrees(q5))), sep='')
    return [
        P5[0]+(d5+d6)*cos(b_read)*sin(a_read),
        P5[1]+(d5+d6)*cos(b_read)*cos(a_read),
        P5[2]+(d5+d6)*sin(b_read)
        ]

def getQ4fromQ5(a, b, q1, q2, q3, q5): #returns two answers: [0] from alpha equation, [1] from beta equation
    return [
        acos(tan(a-q1)/tan(q5)),
        acos(sin(b-q2-q3)/sin(q5))
        ]
def getQ5fromQ4(a, b, q1, q2, q3, q4):
    return [
        atan(tan(a-q1)/cos(q4)),
        asin(sin(b-q2-q3)/cos(q4))
        ]
