
def sendToServo():
    global q1, q2, q3, q4, q5, q6, s, servo, servoExceeded, whichServoExceeded, typeOfExceeded
    if not (math.isnan(q1) or
            math.isnan(q2) or
            math.isnan(q3) or
            math.isnan(q4) or
            math.isnan(q5) or
            math.isnan(q6)):
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
            if x == 4:
                if s[4]<90:
                    servo[4].angle = getServo4Offset(180 - q5_default - s[4])
                else:
                    servo[4].angle = s[4]
            else:
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