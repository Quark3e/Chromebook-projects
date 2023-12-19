from IK_module import *
import math
import os

mov_Patterns = { #NOTE: There cannot be any space in the keys
    "square1": [
        [-250, 250, 250, 45, 0, 0],
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
    ],
    "x-b_test": [
        [0, 150, 150, 0, 0, 0],
        [150, 150, 150, 0, 0, 0],
        [150, 150, 150, 0, -90, 0],
        [125, 150, 150, 0, -75, 0],
        [100, 150, 150, 0, -60, 0],
        [75, 150, 150, 0, -45, 0],
        [50, 150, 150, 0, -30, 0],
        [25, 150, 150, 0, -15, 0],
        [0, 150, 150, 0, 0, 0],
        [-25, 150, 150, 0, -15, 0],
        [-50, 150, 150, 0, -30, 0],
        [-75, 150, 150, 0, -45, 0],
        [-100, 150, 150, 0, -60, 0],
        [-125, 150, 150, 0, -75, 0],
        [-150, 150, 150, 0, -90, 0],
        [-150, 150, 150, 0, 0, 0],
        [0, 150, 150, 0, 0, 0]
    ]

}

def fullAxisTest(servo, diagnostics=[False,[],[[],[],[],[]],None]):
    os.system("clear")
    t_start=0
    if diagnostics[0]: t_start = time.perf_counter()
    isReachable = [True]
    startPos = [0,0,0]
    currentPos = [0,0,0]
    orient = [0,0,0]
    wsRange = [
        [-150, 150],
        [100, 250],
        [100, 350]
    ]
    print(" ---Full axis test---")
    opt = input(" enter start coordinate [x y z]:").split()
    if opt[0]=="exit": return
    startPos = [int(coord) for coord in opt]
    currentPos = startPos.copy()
    opt = input("\n enter orientation [a b Y]:").split()
    if opt[0]=="exit": return
    orient = [toRadians(float(angle)) for angle in opt]

    prevRotation = 6*[0]

    def getAndSend(position):
        nonlocal diagnostics
        currentPos[axis] = position
        q = getAngles(currentPos,orient[0], orient[1], orient[2],
            '-', positionIsReachable=isReachable,
            debug=mod_dict)
        if isReachable[0]:
            sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
            if diagnostics[0]:
                diagnostics[3].write(bytes("get", 'utf-8'))
                diagnostics[1].append(time.perf_counter()-t_start)
                diagnostics[2][3][0].append(0)
                diagnostics[2][3][1].append(sLoadWeight[1]*9.82*(link[1]*math.cos(q[1])+link[2]*math.cos(q[1]+q[2])))
                diagnostics[2][3][2].append(sLoadWeight[2]*9.82*(link[2]+link[3]+link[4]+link[5])*math.cos(q[2]))
                for i in range(3,6): diagnostics[2][3][i].append(0)
                diagnostics[2][2].append(solveListDifference([prevRotation,[toDegrees(joint) for joint in q]]))
                for m in range(6): diagnostics[2][1][m].append(toDegrees(q[m])-prevRotation[m])
                diagnostics[2][0].append(int(diagnostics[3].readline()))
        time.sleep(0.001)

    sendToServo(servo, [toDegrees(joint) for joint in getAngles(startPos,orient[0],orient[1],orient[2],'-')], 1, mode=2, useDefault=True)
    time.sleep(1)
    for axis in range(3):    
        for pos in range(startPos[axis],wsRange[axis][1]):
            getAndSend(pos)
        for pos in range(wsRange[axis][1],wsRange[axis][0],-1):
            getAndSend(pos) 
        for pos in range(wsRange[axis][0],startPos[axis]):
            getAndSend(pos)
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
    for axis in range(2):    
        for angle in range(startOrient[axis],90):
            currentOrient[axis] = angle
            q = getAngles(testPos,
            toRadians(currentOrient[0]),
            toRadians(currentOrient[1]),
            toRadians(currentOrient[2]),
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
            time.sleep(0.001)  

        for angle in range(90,-90,-1):
            currentOrient[axis] = angle
            q = getAngles(testPos,
            toRadians(currentOrient[0]),
            toRadians(currentOrient[1]),
            toRadians(currentOrient[2]),
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
            time.sleep(0.001)   

        for angle in range(-90,startOrient[axis]):
            currentOrient[axis] = angle
            q = getAngles(testPos,
            toRadians(currentOrient[0]),
            toRadians(currentOrient[1]),
            toRadians(currentOrient[2]),
                '-', positionIsReachable=isReachable,
                debug=mod_dict)
            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
            time.sleep(0.001)
    time.sleep(1)

    return

def fullTest(servo):
    os.system("clear")
    isReachable = [True]
    startPos = [0,0,0]
    currentPos = [0,0,0]
    orient = [0,0,0]
    wsRange = [
        [-150, 150],
        [100, 250],
        [100, 350]
    ]
    print(" ---Axis, orientation test test---")
    opt = input(" enter test coordinate [x y z]:").split()
    if opt[0]=="exit": return
    startPos = [int(coord) for coord in opt]
    currentPos = startPos.copy()
    opt = input("\n enter test orientation [a b Y]:").split()
    if opt[0]=="exit": return
    orient = [toRadians(float(angle)) for angle in opt]
    
    sendToServo(servo, [toDegrees(joint) for joint in getAngles(startPos,orient[0],orient[1],orient[2],'-')], 1, mode=2, useDefault=True)
    time.sleep(1)
    for axis in range(3):    
        for pos in range(startPos[axis],wsRange[axis][1]):
            currentPos[axis] = pos
            q = getAngles(currentPos,orient[0], orient[1], orient[2],
                '-', positionIsReachable=isReachable,
                debug=mod_dict,printErrors=False)
            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
            time.sleep(0.002)    
        for pos in range(wsRange[axis][1],wsRange[axis][0],-1):
            currentPos[axis] = pos
            q = getAngles(currentPos,orient[0], orient[1], orient[2],
                '-', positionIsReachable=isReachable,
                debug=mod_dict,printErrors=False)
            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
            time.sleep(0.002)   
        for pos in range(wsRange[axis][0],startPos[axis]+1):
            currentPos[axis] = pos
            q = getAngles(currentPos,orient[0], orient[1], orient[2],
                '-', positionIsReachable=isReachable,
                debug=mod_dict,printErrors=False)
            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
            time.sleep(0.002)
    time.sleep(0.5)
    sendToServo(servo, [toDegrees(joint) for joint in getAngles(startPos, 0, toRadians(-45),0,'-')], 1, mode=2, useDefault=True)
    time.sleep(0.5)
    for angle in range(361):
        # print(math.sin(toRadians(angle))*45,math.cos(toRadians(angle))*-45)
        q = getAngles(startPos,toRadians(math.sin(toRadians(angle))*45),toRadians(math.cos(toRadians(angle))*-45),0,'-',positionIsReachable=isReachable,printErrors=False)
        if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True)
        time.sleep(0.005)

    time.sleep(0.5)
    sendToServo(servo, [toDegrees(joint) for joint in getAngles(startPos,0,0,0,'-')], 1, mode=2, useDefault=True)
    time.sleep(0.5)
    isReachable = [True]
    testPos = startPos
    startOrient = [round(toDegrees(angle)) for angle in orient]
    currentOrient = startOrient.copy()
    for axis in range(2):    
        for angle in range(startOrient[axis],91):
            currentOrient[axis] = angle
            q = getAngles(testPos,
            toRadians(currentOrient[0]),
            toRadians(currentOrient[1]),
            toRadians(currentOrient[2]),
                '-', positionIsReachable=isReachable,
                debug=mod_dict,printErrors=False)
            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
            time.sleep(0.01)  

        for angle in range(90,-91,-1):
            currentOrient[axis] = angle
            q = getAngles(testPos,
            toRadians(currentOrient[0]),
            toRadians(currentOrient[1]),
            toRadians(currentOrient[2]),
                '-', positionIsReachable=isReachable,
                debug=mod_dict,printErrors=False)
            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
            time.sleep(0.01)   

        for angle in range(-90,startOrient[axis]+1):
            currentOrient[axis] = angle
            q = getAngles(testPos,
            toRadians(currentOrient[0]),
            toRadians(currentOrient[1]),
            toRadians(currentOrient[2]),
                '-', positionIsReachable=isReachable,
                debug=mod_dict,printErrors=False)
            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,useDefault=True,mode=0)
            time.sleep(0.01)
    time.sleep(0.5)
    sendToServo(servo, [toDegrees(joint) for joint in getAngles(startPos,0,0,0,'-')], 1, mode=2, useDefault=True)
    time.sleep(0.5)
    return

def test_circle(servo):
    os.system("clear")
    circleRadius = 50
    drawPlane = 0 #0 - XY  1 - XZ  2 - YZ
    circleCenter = [0,200,150]
    tempCenter = circleCenter.copy()
    circleOrient = [0,0,0] #in radians
    
    isReachable = [True]
    for plane in range(3):
        if plane==0: circleOrient = [0,0,0]
        elif plane==1: circleOrient = [0,toRadians(-90),0]
        elif plane==2:
            circleOrient = [0,toRadians(-45),0]
            # circleCenter = [0,150,150]
        q = getAngles(circleCenter,circleOrient[0],circleOrient[1],circleOrient[2],'-',positionIsReachable=isReachable)
        if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,mode=2,useDefault=True)
        time.sleep(0.5)
        for angle in range(361):
            if plane==0:
                tempCenter = [
                    circleCenter[0]+circleRadius*sin(toRadians(angle)),
                    circleCenter[1],
                    circleCenter[2]+circleRadius+circleRadius*cos(toRadians(angle-180))
                ]
            if plane==1:
                tempCenter = [
                    circleCenter[0]+circleRadius*sin(toRadians(angle)),
                    circleCenter[1]-circleRadius+circleRadius*cos(toRadians(angle)),
                    circleCenter[2]
                ]
            if plane==2:
                tempCenter = [
                    circleCenter[0],
                    circleCenter[1]+circleRadius*sin(toRadians(angle)),
                    circleCenter[2]+circleRadius+circleRadius*cos(toRadians(angle-180))
                ]
            print(tempCenter,[toDegrees(angle) for angle in circleOrient])
            q = getAngles(tempCenter,circleOrient[0],circleOrient[1],circleOrient[2],'-',positionIsReachable=isReachable)
            if isReachable[0]: sendToServo(servo,[toDegrees(joint) for joint in q],0,mode=0,useDefault=True)
        time.sleep(0.5)
    return

mov_Programs = {
    "axisTest": fullAxisTest,
    "orientTest": fullOrientTest,
    "fullTest": fullTest,
    "testCircle": test_circle
}
"""Dictionary of program names and the function

## Parameters:
    - assigned servo[n].angle list

"""


def runFromFile(filePath, servo):
    """Reads movement commands from file and runs them

    ## Parameter:
    - filePath (str): string of path to the file
    ## Syntax:
    - Type of commands (angles or coordinates (and orientation)) declared first in file
    - "type:angle":
        - given angles: "q1:q2:q3:q4:q5:q6" [unit: degrees]
        - ":useDefault":
            - ex. "type:angle:True
    - "type:coord":
        - given coordinates: "x:y:z;a:b:Y"
        - ":orientsGiven"
            - ex. "type:coord:True"  -> "x:y:z;a:b:Y"
            - ex. "type:coord:False" -> "x:y:z"
    - "mode:(n)":
        - the mode to be sent for sendToServo()
    - "sleep:(n)":
        - sleep for n seconds
    """
    os.system("clear")
    
    servoMode = 1
    cmdFile = open(filePath, 'r')
    line1 = cmdFile.readline()
    line2 = cmdFile.readline()
    toUseDefault = False
    orientGiven = False
    readType = line1[5:][:5]
    if readType == "angle" and line1[11:][:4] == "True": toUseDefault = True
    if readType == "coord" and line1[11:][:4] == "True": orientGiven = True
    lineCount=0
    if line2[:5]== "mode:": servoMode = int(line2[5:])
    for line in cmdFile:
        if line[:5]== "mode:":
            servoMode = int(line[5:])
            continue
        if line[0]=="#" or len(line)<=1 or line[0]==" ": continue
        if line[:6]=="sleep:": time.sleep(float(line[6:]))
        elif readType=="coord" and orientGiven:
            coords = line[:line.find(';')]
            orients = line[line.find(';')+1:]
            coordinate = [
                float(coords[:coords.find(':')]),
                float(coords[coords.find(':')+1:][:coords[coords.find(':')+1:].find(':')]),
                float(coords[coords.find(':')+1:][coords[coords.find(':')+1:].find(':')+1:])
            ]
            orientation = [
                int(orients[:orients.find(':')]),
                int(orients[orients.find(':')+1:][:orients[orients.find(':')+1:].find(':')]),
                int(orients[orients.find(':')+1:][orients[orients.find(':')+1:].find(':')+1:orients[orients.find(':')+1:].find(";")])
            ]
            if ";tt" in line:
                if ";" in line[line.find(";tt")+3]: totTime = float(line[line.find(";tt")+3:line.find(";", line.find(";tt")+3)])
                else: totTime = float(line[line.find(";tt")+3:][:line[line.find(";tt")+3:].find(";")])
            else: totTime = 0

            orientation = [toRadians(angle) for angle in orientation]
            isReachable = [True]
            q = getAngles(coordinate,orientation[0],orientation[1],orientation[2],'-',positionIsReachable=isReachable)
            q = [toDegrees(angle) for angle in q]
            print(lineCount, ": ", coordinate, ": ", [round(toDegrees(i)) for i in orientation], ": ", [round(n,1) for n in q], sep="")
            if isReachable[0]: sendToServo(servo,q,totTime,mode=servoMode,useDefault=True)
            else:
                orients = findOrients(coordinate,[orientation[0],orientation[1]])
                if orients==None:
                    print(f"no orientation found for {coordinate}")
                else:
                    orients = [toRadians(var) for var in orients]
                    q = getAngles(coordinate,orients[0],orients[1],orients[2],'-',positionIsReachable=isReachable)
                    q = [toDegrees(angle) for angle in q]
                    if isReachable[0]: sendToServo(servo,q,totTime,mode=servoMode,useDefault=True)
        elif readType=="coord" and not orientGiven:
            coords = line
            coordinate = [
                float(coords[:coords.find(':')]),
                float(coords[coords.find(':')+1:][:coords[coords.find(':')+1:].find(':')]),
                float(coords[coords.find(':')+1:][coords[coords.find(':')+1:].find(':')+1:])
            ]
            orients = findOrients(coordinate,[0,0])
            if orients==None: print(f"no orientation found for {coordinate}")
            else:
                orients = [toRadians(var) for var in orients]
                q = getAngles(coordinate,orients[0],orients[1],orients[2],'-')
                q = [toDegrees(angle) for angle in q]
                # print(q)
                sendToServo(servo,q,1,mode=servoMode,useDefault=True)
        elif readType=="angle":
            angles = getNumFromString(line, ':')
            sendToServo(servo,angles,1,useDefault=toUseDefault,mode=servoMode)
        lineCount+=1
    return

# Permanent modifiers

def createMovPathFile():
    os.system("clear")

    

    return
