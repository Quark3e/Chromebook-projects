import sys
import time
import os
from math import * #type: ignore

import socket

module_absPath = str(os.path.realpath(__file__)[:len(str(os.path.realpath(__file__)))-len("IK_module.py")])

sys.path.append(module_absPath[:module_absPath.find("projects/proj_Hexclaw")])
from teststuff.python.modules.nodemcu.main_h import *

if "pi" in module_absPath:
    import busio #type: ignore
    from board import SCL, SDA #type: ignore
    from adafruit_motor import servo #type: ignore
    from adafruit_servokit import ServoKit #type: ignore
    from adafruit_pca9685 import PCA9685 #type: ignore

class HC_servoControl(object):
    newRot = [0, 0, 0, 0, 0, 0]
    oldRot = [0, 0, 0, 0, 0, 0]
    
    useDefault = False
    mode = 0
    servoExceeded = False
    whichServoExceeded = [False,False,False,False,False,False]
    typeOfExceeded = ["null","null","null","null","null","null"]
    
    printErrors = True
    printResult = False

    def __init__(self):
        global i2c
        print("ERROR: \"classHC_servoControl\": Could not import necessary modules.")

        i2c = busio.I2C(SCL, SDA)
        self.pca = PCA9685(i2c)
        self.pca.frequency = 50

        pulseWidthRange = (500, 2500)

        self.servo = [
            servo.Servo(self.pca.channels[0]),
            servo.Servo(self.pca.channels[1]),
            servo.Servo(self.pca.channels[2]),
            servo.Servo(self.pca.channels[3]),
            servo.Servo(self.pca.channels[8]),
            servo.Servo(self.pca.channels[5]),
            ]
        for i in range(len(self.servo)):
            self.servo[i].set_pulse_width_range(pulseWidthRange[0],pulseWidthRange[1])
    def oneServo(self, idx, angle):
        self.oldRot[idx] = angle
        self.servo[idx].angle = angle
    def toServo(
        self,
        new_rotation,
        total_time,
        useDefault = False,
        mode = 0,
        printErrors=True,
        printResult=False,
        useCorr=True,
        ):
        """Sends angles in list \"new_rotation\" to servo motors evenly spaced out

        ## Parameters:
            - new_rotation (float/int): [unit: degrees]
                - dictionary/list variable of new rotation commands in [degrees]
            - total_time (float/int): [unit: seconds]
                - Total time spent moving the servo from start to finish in seconds
            - useDefault (bool):
                - if True, value defaults are used
            - mode: what type of functions to use for servo movement
                - 0: Immediate: no split of movement. The new_rotations are sent directly to servo motors
                - 1: new_rotation diff is sent linearly, evenly
                - 2: mp1: motion profile 1: second polynomial
        """
        self.mode = mode
        self.newRot = new_rotation
        self.useDefault = useDefault
        self.printErrors = printErrors
        self.printResult = printResult

        if self.useDefault: self.add_defaults(new_rotation, True)
        if useCorr: self.servoSol()
        self.exceedCheck()
        # print(new_rotation)

        if self.printResult: print("sent:",self.newRot)
        if self.servoExceeded: return
        total_iteration = 135
        if self.mode==0:
            for x in range(6): 
                self.servo[x].angle = self.newRot[x]
                # time.sleep(0.001) #for 6 servo commands it should equal ~10 ms delay
        elif self.mode>0:
            s_diff = []
            s_temp = []
            for i in range(6): s_diff.append(self.newRot[i]-self.servo[i].angle)
            for i in range(6): s_temp.append(self.servo[i].angle)
            total_iteration = round(findVal(s_diff)[0])+45
            for count in range(total_iteration-1):
                for i in range(6):
                    if self.mode==1:
                        val = s_diff[i]/total_iteration
                        if val<180 and val>0: s_temp[i] += val
                        else: print(f"q[{i}] exceeded:",val)
                        self.servo[i].angle = s_temp[i]
                    elif self.mode==2:
                        val = s_temp[i] + s_diff[i]*mp1(count/total_iteration)
                        if val<180 and val>0: self.servo[i].angle = val
                        else: print(f"q[{i}] exceeded:",val)
                if total_time > 0.1: time.sleep(total_time/total_iteration)
        self.oldRot = [i.angle for i in self.servo]
    def add_defaults(self, useMutable=False):
        """Adds default/offset values for servo motors
        
        ### Parameters:
            - useMutable [bool]:
                - True: re-defines class variable "self.newRot"
                - False: stores new values in a new list that is returned
        """
        temp=len(self.newRot)*[0]
        for i in range(len(self.newRot)):
            if useMutable: self.newRot[i] = self.angDef[i](self.newRot[i])
            elif not useMutable:
                temp[i] = self.angDef[i](self.newRot[i])
        if useMutable: return temp
    def servoSol(self, solveIdx=[-1], solveIdx_lst=[]):
        """Apply servo error solutions to each element in self.newRot

        ### Parameters
            - solveIdx [int]:
                - >-1: only apply servo correction/solution to NOT given indexes in solveIdx
                - ==-1: apply correction to all of newRot local variable
        """
        for i in range(6):
            if -1 in solveIdx:
                self.newRot[i] = self.sol[i](self.newRot[i])
            else:
                if i in solveIdx: continue
                else: solveIdx_lst[i] = self.sol[i](solveIdx_lst[i])
    def exceedCheck(self):
        """Check if any of the rotation commands exceeds servo motors limitations [0:180]
        ### Parameters:
            - q: joints 1-6 in unit: NOTE:degrees
        ### Return:
            - True if any joint command exceeded servo limits
            - else returns False (or similar to servoExceeded parameter)
        """
        # print(q)
        for i in range(6):
            if self.newRot[i] < 0:
                self.newRot[i] = 0
                self.servoExceeded = True
                self.whichServoExceeded[i] = True
                self.typeOfExceeded[i] = "under"
            elif self.newRot[i] > 180: 
                self.newRot[i] = 180
                self.servoExceeded = True
                self.whichServoExceeded[i] = True
                self.typeOfExceeded[i] = "over"

        if self.servoExceeded and self.printErrors:
            for i in range(5):
                if self.whichServoExceeded[i]:
                    print(f"\tServo motor: q[{i}] exceeded \"{self.typeOfExceeded[i]}\"")
        return self.servoExceeded
    sol = [
        lambda x: x*constants_q[0]["fixed"],
        lambda x: x*constants_q[1]["fixed"],
        lambda x: x*constants_q[2]["fixed"],
        lambda x: x*constants_q[3]["fixed"],
        lambda x: x*constants_q[4]["fixed"],
        lambda x: x*constants_q[5]["fixed"],
    ]
    angDef = [
        lambda x: default_q[0]+x,
        lambda x: default_q[1]+x,
        lambda x: 180-default_q[2]-x,
        lambda x: default_q[3]+x,
        lambda x: 180-default_q[4]-x,
        lambda x: default_q[5]+x
    ]
    """List of error correction lambda functions for real life servo motor rotation errors
    """



d1 = 145; #axial "roll"
d2 = 130; #axial "pitch"
d3 = 75; #axial "pitch"
d4 = 50; #axial "roll"
d5 = 40; #axial "pitch
d6 = 25; #axial "roll" (?)

link = [d1,d2,d3,d4,d5,d6]
sLoadWeight = [0, 0.130, 0.085, 0, 0, 0] #kg
default_q = [90, 0, 135, 90, 90, 90]
debug_mods = []

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


mod_dict = {
    "a1:frame1X": [True, "frame1X = frame1X * cos(b)"], #True
    "a1:a1": [True, "a1 = a1 * cos(b)"], #True
    "q4:default": [False, "q4 = atan(frame1X / frame1Y)"], #False
    "q4:a1": [True, "q4 = a1"], #True
    "q4:a1:b:minus": [True, "if b<0: q4=0-a1; else: q4=a1"], #True
    "q4:a1:b1:minus": [True, "if b1<0: q4=0-a1; else: q4=a1"], #True
    "q5:inPaper": [False, "q5 = atan([...] / ( cos(b1) * cos(a1) ))"], #False
    "q5:default": [False, "q5 = atan([...] / ( frame1X / tan(a1) ))"], #False
    "q5:d5": [False, "q5 = atan([...] / ( d5 ))"], #False
    "exceedState": [True, "if [...]_exceeded: positionIsReachable[0] = False"],
}
"""Dictionary of all the modificaitons used, mostly for debugging the IK equations

## Syntax
    - mod_dict = { str(mod name) : [bool(whether to use), str(description)]}\n

"""

def debug_mod_menu(mod_dict):
    """Edit the statuses for different modifications for angle-solving
    ## Parameter
        - mod_dict (dictionary): each mod is separated, only a boolean state for each key/mod
            - keys: mod name
            - value: status for if that key(/mod) is used, True if it is used
    """
    idxDict = {}
    idxDict_rev = {}
    count = 0
    for key in mod_dict:
        idxDict.update({count:key})
        idxDict_rev.update({key:count})
        count+=1
    while True:
        os.system("clear")
        print("--- Debug: mod keys ---")
        print(" to change the \"state\" of a mod, enter key and \"True\" to activate, \"False\" to turn off\n")
        for key, status in mod_dict.items():
            print(f" -[{idxDict_rev[key]}]{'':3}: {key:15}: status:{status[0]:5} description:\"{status[1]:}\"")
        opt = input("\n input:").split()
        if len(opt)==0: continue
        if opt[0] == "exit": return
        if opt[0] not in mod_dict and opt[0] not in [str(i) for i in idxDict]:
            input(f"paused: \"{opt[0]}\" invalid.")
            continue
        if len(opt) > 1: mod_dict[opt[0]][0] = eval(opt[1])
        elif len(opt) == 1:
            if opt[0] in [str(i) for i in idxDict]:
                mod_dict[idxDict[int(opt[0])]][0] = not mod_dict[idxDict[int(opt[0])]][0]
            else: mod_dict[opt[0]][0] = not mod_dict[opt[0]][0]


def getNumFromString(string, sepChar):
    """Get all numbers in a string separated by a character returned in a string

    ## Note: 
    Only spaces are allowed in the string beside the numbers, no other characters than sepChar.
    ## Parameters:
        - string (str): string of text to extract numbers from
            ex: "1:2:3:4"
        - sepChar (str): string/char of character that separates the numbers
    ## Returns:
        - list of extracted numbers as floats
    """
    numList = []
    for i in range(string.count(sepChar)):
        numList.append(float(string[:string.find(sepChar)]))
        string = string[string.find(sepChar)+1:]
    numList.append(float(string))
    return numList

def servoTest(servo, n, totalTime=4):
    """ Run a quick 0-180 degrees servo motor test
    ## Parameters:
    - servo: servo list
    - n: n:th servo to test. Starts at 0
    - totalTime: the time it takes to run the entire test (with slight extra delay)
    """
    for i in range(181):
        servo[n].angle = i
        time.sleep(totalTime/360)
    for i in range(180,-1,-1):
        servo[n].angle = i
        time.sleep(totalTime/360)

def q_corrections(q):
    """Corrects all angle values for their servo
    
    ## Parameters:
        - q (float/int): [unit: degrees]
            - A list of 6 elements where the parameter variable is modified
    ## Returns:
        - q modified
    """
    q[0] = q[0] * constants_q[0]["fixed"]
    q[1] = q[1] * constants_q[1]["fixed"]
    q[2] = q[2] * constants_q[2]["fixed"]
    q[3] = q[3] * constants_q[3]["fixed"]
    # if q[4]<90: q[4] = (default_q[4] - ((default_q[4]-q[4])*default_q[4]) /135)
    # else: 
    q[4] = q[4] * constants_q[4]["fixed"]
    q[5] = q[5] * constants_q[5]["fixed"]
    return q


def toDegrees(radians): return (radians * 180) / pi
def toRadians(degrees): return (degrees * pi) / 180

def getDistance(p1, p2):
    """Solves delta distance between two points in 3D
    """
    return sqrt(pow(p2[0]-p1[0], 2)+pow(p2[1]-p1[1], 2)+pow(p2[2]-p1[2], 2))

def solveListDifference(listOfLists, mode=1, sumList=[0]):
    """Solves difference of each of the elements between all lists

    ## Parameters
        - listOfLists: [list1, list2], NOTE: len(list1) = len(list2)
        - mode:
            - mode=1: solves total difference: abs(-diff)
            - mode=2: solves normal difference
        - sumList: mutable list where index 0 gives sum of diff values (same as return)

    ## Returns:
        - returns sum of differences of each element
    """
    deltaVal = 0
    sumList = len(listOfLists[0])*[0]
    
    for i in range(len(listOfLists[0])):
        diff = listOfLists[1][i]-listOfLists[0][i]
        if diff<0 and mode==1: sumList[i] = abs(diff)
        else: sumList[i] = diff
    deltaVal = sum(sumList)
    sumList[0] = deltaVal
    return deltaVal

def findVal(listToCheck, mode=0):
    """Goes through list and return the desired element

    ## Parameters:
        - listToCheck [unit: int/float]: the list to go through.
        - mode [unit: int]: declares what to find in the list.
            - 0. biggest value
            - 1. smallest value
        - returnValue [unit: boolean]: whether to return value or element index (true if value)
    ## Returns:
        - returns either value or index of desired element
    """
    try:
        index, val = 0, listToCheck[0]
        for i in range(1,len(listToCheck)):
            if mode==0:
                if listToCheck[i]>val:
                    val=listToCheck[i]
                    index=i
            elif mode==1:
                if listToCheck[i]<val:
                    val=listToCheck[i]
                    index=i
        return [val, index]
    except IndexError:
        print("List is empty")
        return [-1, 0]

def PON(var):
    """find out if a number is Positive Or Negative
    ## Parameters:
        - var [float/int]: value to check
    ## Returns:
        - 1 if var positive
        - -1 if var negative
        - 0 if it's 0
    """
    if var>0: return 1
    elif var<0: return -1
    else: return 0

def mp1(x):
    y=0
    V_max=2
    t3=1
    Pt1=0.5
    Pt2=Pt1
    t2=t3*Pt2
    t1=t3*Pt1

    a3=(0-V_max)/(t3-t2)
    # a2=(V_max-V_max)/(t2-t1)
    a1=(V_max-0)/(t1-0)

    if 0<x and x<=t1: y=(a1*pow(x,2))/2
    elif t1<x and x<t2: y=a1*t1*x-(a1*pow(t1,2))/2
    elif t2<=x and x<=t3: y=(a3*pow(x-t2,2))/2+V_max*x+a1*(t1*t2-pow(t1,2)/2)-V_max*t2

    return y

def sendToServo(
    servo,
    new_rotation,
    total_time,
    useDefault = False,
    mode = 0,
    servoExceeded=False,
    whichServoExceeded=[False,False,False,False,False,False],
    typeOfExceeded=["null","null","null","null","null","null"],
    printErrors=True,
    printResult=False
    ):
    print("  --DEPRECATED: TRY TO NOT USE THIS---")
    """Sends angles in list \"new_rotation\" to servo motors evenly spaced out

    ## Parameters:
        - servo (float/int): [no unit specification needed]
            - dictionary/list variable that sends to pca board / holds old/current rotation commands
        - new_rotation (float/int): [unit: degrees]
            - dictionary/list variable of new rotation commands in [degrees]
        - total_time (float/int): [unit: seconds]
            - Total time spent moving the servo from start to finish in seconds
        - useDefault (bool):
            - if True, value defaults are used
        - mode: what type of functions to use for servo movement
            - 0: Immediate: no split of movement. The new_rotations are sent directly to servo motors
            - 1: new_rotation diff is sent linearly, evenly
            - 2: mp1: motion profile 1: second polynomial
    """
    if useDefault: add_defaults(new_rotation, True)
    
    q_corrections(new_rotation)
    
    servoExceeded = exceedCheck(
        new_rotation,
        servoExceeded,whichServoExceeded,typeOfExceeded,
        printErrors=printErrors
        )
    # print(new_rotation)
    if printResult: print("sent:",new_rotation)
    if servoExceeded: return
    total_iteration = 135
    if mode==0:
        for x in range(6): 
            servo[x].angle = new_rotation[x]
            # time.sleep(0.001) #for 6 servo commands it should equal ~10 ms delay
    elif mode==1 or mode==2:
        s_diff = []
        s_temp = []
        for i in range(6): s_diff.append(new_rotation[i]-servo[i].angle)
        for i in range(6): s_temp.append(servo[i].angle)
        total_iteration = round(findVal(s_diff)[0])+45
        for count in range(total_iteration-1):
            for i in range(6):
                if mode==1:
                    val = s_diff[i]/total_iteration
                    if val<180 and val>0: s_temp[i] += val
                    else: print("exceeded:",val)
                    servo[i].angle = s_temp[i]
                elif mode==2:
                    val = s_temp[i] + s_diff[i]*mp1(count/total_iteration)
                    if val<180 and val>0: servo[i].angle = val
                    else: print("exceeded:",val)
            if total_time > 0.1: time.sleep(total_time/total_iteration)


def findOrients(pos, startOrient=[0,0],Y=0):
    """To find an alpha:beta combination that gives a reachable answer: NOTE: It includes exceedCheck of the servo motors.
        
    ## Parameters:
    - pos [unit: mm]: XYZ coordinate to find a valid orientation for.
    - startOrient [unit: degrees]: Orientation to find a valid orientation from. This orientation is used at the start.
    ## Returns:
    - [unit: degrees], [a, b, Y]: "closest" valid orientation from startOrient in terms of difference *IF* an orient is found
    - else returns None
    # 
    """
    # I need to specify the likely reachable range of orientation combinations by
    # using pre-defined rules
    isReachable = [False, False]
    tempOrient = [val+90 for val in startOrient.copy()]
    def checkOrient(orient):
        if orient[0]<=180-1 and orient[0]>=0 and orient[1]<=180-1 and orient[1]>=0:
            q = getAngles(pos,
            toRadians(orient[0]),toRadians(orient[1]),toRadians(Y),
            '-',positionIsReachable=isReachable,printErrors=False)
            if isReachable[0]: isReachable[1]=True
    for x in range(181):
        for y in range(181):
            checkOrient([tempOrient[0]+x, tempOrient[1]+y])
            if isReachable[1]: return [tempOrient[0]+x-90, tempOrient[1]+y-90, toDegrees(Y)]
            checkOrient([tempOrient[0]+x, tempOrient[1]-y])
            if isReachable[1]: return [tempOrient[0]+x-90, tempOrient[1]-y-90, toDegrees(Y)]
            checkOrient([tempOrient[0]-x, tempOrient[1]+y])
            if isReachable[1]: return [tempOrient[0]-x-90, tempOrient[1]+y-90, toDegrees(Y)]
            checkOrient([tempOrient[0]-x, tempOrient[1]-y])
            if isReachable[1]: return [tempOrient[0]-x-90, tempOrient[1]-y-90, toDegrees(Y)]
    # for radius in range(0,180):
    #     for x in range(tempOrient[0]-radius,tempOrient[0]+radius+1):
    #         checkOrient([x,tempOrient[1]-radius])
    #         if isReachable[1]: return [x-90, tempOrient[1]-radius-90, toDegrees(Y)]
    #         checkOrient([x,tempOrient[1]+radius])
    #         if isReachable[1]: return [x-90, tempOrient[1]+radius-90, toDegrees(Y)]
    #     for y in range(tempOrient[1]-radius,tempOrient[1]+radius+1):
    #         checkOrient([tempOrient[0]-radius,y])
    #         if isReachable[1]: return [tempOrient[0]-radius-90, y-90, toDegrees(Y)]
    #         checkOrient([tempOrient[0]+radius,y])
    #         if isReachable[1]: return [tempOrient[0]+radius-90, y-90, toDegrees(Y)]

    return None

def exceedCheck(q, servoExceeded=False, whichServoExceeded=[False,False,False,False,False,False], typeOfExceeded=["null","null","null","null","null","null"], printErrors=True):
    """
    ### Parameters:
        - q: joints 1-6 in unit: NOTE:degrees
    ### Returns:
        - True if any joint command exceeded servo limits
        - else returns False (or similar to servoExceeded parameter)
    """
    # print(q)
    for i in range(5):
        if q[i] < 0: q[i] = 0; servoExceeded = True; whichServoExceeded[i] = True; typeOfExceeded[i] = "under"
        elif q[i] > 180: q[i] = 180; servoExceeded = True; whichServoExceeded[i] = True; typeOfExceeded[i] = "over"

    if servoExceeded and printErrors:
        for i in range(5):
            if whichServoExceeded[i]: print("\tServo motor: q", i+1, " exceeded \"", typeOfExceeded[i], "\"", sep='')
    
    return servoExceeded


def check_isNaN(q, printText = False):
    """
    Returns True if any rotation is not a number (isNaN)
        else it returns False
    ## Returns:
        - True if isnan(q[i])==True
        - else: False
    """
    for i in q:
        if isnan(i):
            if printText: print(f"joint q{i} is NaN")
            return True
    else:
        return False

def add_defaults(q, useMutable=False):
    if useMutable:
        q[5] = default_q[5] + q[5]
        q[4] = 180 - default_q[4] - q[4]
        q[3] = default_q[3] + q[3]
        q[2] = 180 - default_q[2] - q[2]
        q[1] = default_q[1] + q[1]
        q[0] = default_q[0] + q[0]
        return
    elif not useMutable:
        temp=len(q)*[0]
        temp[5] = default_q[5] + q[5]
        temp[4] = 180 - default_q[4] - q[4]
        temp[3] = default_q[3] + q[3]
        temp[2] = 180 - default_q[2] - q[2]
        temp[1] = default_q[1] + q[1]
        temp[0] = default_q[0] + q[0]
        return temp 

def getAngles(
    PP,a,b,Y,posOption,
    length_scalar=1,coord_scalar=1,
    printText=False,printErrors=True, 
    debug=mod_dict,
    positionIsReachable=[True],
    getSubframe=False
    ):
    """ Solves and returns all the rotation values

    ## Parameters:
        - PP (float/int): [unit: mm]
            - The end-effector position in list for
        - a (float): [unit: radians]
            - alpha orientation variable
        - b (float): [unit: radians]
            - beta orientation variable
        - Y (float): [unit: radians]
            - gamma orienation variable
        - posOption (str/char):  
            -different mode to use
                - '-' is q3(/q[2]) above line between P2 and P4/P5
                - '+' is q3(/q[2]) under line between P2 and P4/P5
        - debug [boolean, string(code)]:
            - [1]="q4" if only q4 mod is used, [1]="q5" if only q5 mod is used, [1]="both" if both is used 
        - positionIsReachable (boolean): a mutable list for if position is reachable.
            - positionIsReachable[0]=True if reachable, False if otherwise 

    ## Returns:
        - List of solved rotations for each joint in [radians].
            - Note: \"if positionIsReachable[0]==False\" then some elements are NaN
    """

    positionIsReachable[0] = True

    a1_exceed, b1_exceed = 0, 0
    q=[0]*6
    P5 = [0]*3
    # posX = 0 - PP[0] #if it doesn't work: reset default_q[0] + to minux
    PP[0] = PP[0] * coord_scalar
    PP[1] = PP[1] * coord_scalar
    PP[2] = PP[2] * coord_scalar
    link[0] = link[0] * length_scalar
    link[1] = link[1] * length_scalar
    link[2] = link[2] * length_scalar
    link[3] = link[3] * length_scalar
    link[4] = link[4] * length_scalar
    link[5] = link[5] * length_scalar
    l = (link[4]+link[5]) * cos(b)
    P5[0] = PP[0] - l * sin(a)
    P5[1] = PP[1] - l * cos(a)
    P5[2] = PP[2] - (link[4]+link[5]) * sin(b)

    if printText: print(" P5 coords:",[round(pos) for pos in P5], sep='')

    if P5[1] < 0: P5[1] = 0
    if P5[1] == 0:
        if P5[0] > 0: q[0] = toRadians(90) # type: ignore
        elif P5[0] < 0: q[0] = toRadians(-90) # type: ignore
        elif P5[0] == 0: q[0] = toRadians(0) # type: ignore
    else: q[0] = atan(-P5[0] / P5[1]) # type: ignore
    a = 0-a
    try:
        if posOption == '+': q[2] = acos( #type: ignore
                (pow(P5[0], 2) + pow(P5[1], 2) + pow(P5[2] - link[0], 2) - pow(link[1], 2) - pow(link[2] + link[3], 2)) / 
                (2 * link[1] * (link[2] + link[3]))
            ) # type: ignore
        elif posOption == '-': q[2] = acos( #type: ignore
                (pow(P5[0],2) + pow(P5[1],2) + pow(P5[2]-link[0],2) - pow(link[1],2) - pow(link[2]+link[3],2)) /
                (2 * link[1] * (link[2] + link[3]))
            ) # type: ignore
    except ValueError: 
        if printErrors: print("domain error triggered")
        positionIsReachable[0] = False
    
    lambdaVar, muVar = 0, 0

    try: lambdaVar = atan((P5[2] - link[0]) / sqrt(pow(P5[0], 2) + pow(P5[1], 2)))
    except ZeroDivisionError: positionIsReachable[0] = False

    muVar = atan( \
            ((link[2] + link[3]) * sin(q[2])) / 
            (link[1] + (link[2] + link[3]) * cos(q[2])) \
        )
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
    if debug["a1:a1"][0]: a1 = a1 * cos(b)

    # if not forShow and (round(toDegrees(b)) == 90 or round(toDegrees(b) == -90)): a1 = 0
    
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
        if debug["exceedState"][0]: positionIsReachable[0] = False
    frame1X = (link[4]+link[5]) * cos(b1) * sin(a1)

    if debug["a1:frame1X"][0]: frame1X = frame1X*cos(b)

    #NOTE: The x and y axis of the X1Y1Z1 frame in the paper was reverse (compared to this. The names need to be changed!)
    #nvm, the frame X0Y0Z0: x axis flipped and then it's flipped in the function
    frame1Z = (link[4]+link[5]) * sin(b1)
    frame1Y = (link[4]+link[5]) * cos(b1) * cos(a1)
    if getSubframe:
        return [frame1X,frame1Y,frame1Z,a1,b1]

    if printText: print(" frame1_x:",round(frame1X)," frame1_y:",round(frame1Y)," frame1_z:",round(frame1Z), sep='')

    if b1 == 0:
        if frame1X > 0: q[3] = toRadians(90)  # type: ignore
        elif frame1X < 0: q[3] = toRadians(-90) # type: ignore
        elif frame1X == 0: q[3] = toRadians(0) # type: ignore
        if printText: print(" b1 was 0 so q4 was adjusted")
    elif b1 < 0 or b1 > 0: q[3] = atan(frame1X / frame1Z) # type: ignore
    
    if debug["q4:default"][0]: #false
        q[3] = atan(frame1X/frame1Z) #type: ignore
    if debug["q4:a1"][0]: #false
        q[3] = a1
    if debug["q4:a1:b:minus"][0]: #false
        if b<0: q[3] = 0 - a1
        else: q[3] = a1
    if debug["q4:a1:b1:minus"][0]: #true
        if b1<0: q[3] = 0-a1
        else: q[3] = a1
    q[3]=-q[3]

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

    try:    
        if debug["q5:inPaper"][0]: #false
            q[4] = atan(sqrt(pow(frame1X,2)+pow(frame1Z,2))/(cos(b1)*cos(a1))) #type: ignore
        if debug["q5:default"][0]: #false
            q[4] = atan(sqrt(pow(frame1X,2)+pow(frame1Z,2))/(frame1X/tan(a1))) #type: ignore
        if debug["q5:d5"][0]: #true
            q[4] = asin(sqrt(pow(frame1X, 2) + pow(frame1Z, 2)) / (link[4]+link[5])) # type: ignore

        if frame1Z < 0:
            q[4] = 0-q[4] # type: ignore
            if printText: print(" frame1_z was below 0 so q5 is subtracted from 0")
        if b <= pi / 2 and b >= 0 - pi / 2: q[5] = Y - q[3]
        elif b >= pi / 2 or b <= 0 - pi / 2: q[5] = pi - (Y - q[3])
        if printText: print(" ",[round(toDegrees(n)) for n in q])
        if check_isNaN(q):
            positionIsReachable[0] = False
            print("one joint is NaN")
    except ValueError:
        positionIsReachable[0] = False

    return q


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
