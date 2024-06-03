
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// #define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
// #define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

uint8_t servonum = 0; // servo counter

int servoSpeed = 5; // the increase steps in the for loop
bool useTimer = false;

uint16_t s[7]; // the previous rotation value


void driveServo(int servoNum, int degree, bool useTimer = false) {
    //goes from 0 to 180
    //note: the servos on the "right" side of the robot's perspective must go in reverse.
    // so servo's q3, q5 and q7 must be set so it goes in reverse. This must be done
    // automatically inside this function, meaning you can send driveServo(45, 3) and it'll
    // calculate the correct reverse movement by default
    
    if(servoNum == 3 || servoNum == 5 || servoNum == 7) {
      degree = 180 - degree;    
    }
    
    uint16_t microsec_dur = USMIN + float(USMAX - USMIN) * (float(degree)/180);
    
    if(useTimer) {
        if(s[servoNum-1] <= microsec_dur) {
            for(int i=s[servoNum-1]; i < microsec_dur; i+= servoSpeed) { pwm.writeMicroseconds(servoNum-1, i); }
            pwm.writeMicroseconds(servoNum-1, microsec_dur);
        }
        else if(s[servoNum-1] >= microsec_dur) {
            for(int i=s[servoNum-1]; i > microsec_dur; i-= servoSpeed) { pwm.writeMicroseconds(servoNum-1, i); }
            pwm.writeMicroseconds(servoNum-1, microsec_dur);
        }
    }
    else {
        pwm.writeMicroseconds(servoNum-1, microsec_dur);
    }
    s[servoNum - 1] = microsec_dur;
    
    delay(10);
}


void global_driveServo(
    int s1_Degree,
    int s2_Degree,
    int s3_Degree,
    int s4_Degree,
    int s5_Degree,
    int s6_Degree
) {
    uint16_t g_sDur[6]; // the new rotation value
    g_sDur[0] = USMIN + float(USMAX - USMIN) * (float(s1_Degree)/180);
    g_sDur[1] = USMIN + float(USMAX - USMIN) * (float(s2_Degree)/180);
    g_sDur[2] = USMIN + float(USMAX - USMIN) * (float(180 - s3_Degree)/180);
    g_sDur[3] = USMIN + float(USMAX - USMIN) * (float(s4_Degree)/180);
    g_sDur[4] = USMIN + float(USMAX - USMIN) * (float(180 - s5_Degree)/180);
    g_sDur[5] = USMIN + float(USMAX - USMIN) * (float(s6_Degree)/180);
    while(
        g_sDur[0] != s[0] ||
        g_sDur[1] != s[1] ||
        g_sDur[2] != s[2] ||
        g_sDur[3] != s[3] ||
        g_sDur[4] != s[4] ||
        g_sDur[5] != s[5]
        ) {
            for(int i=0; i<6; i++) {
                if(s[i] < g_sDur[i]) {
                    s[i] += servoSpeed;
                    pwm.writeMicroseconds(i, s[i]);
                    delay(5);
                    if(s[i] > g_sDur[i]) {
                        pwm.writeMicroseconds(0, g_sDur[i]);
                        s[i] = g_sDur[i];
                    }
                }
                if(s[i] > g_sDur[i]) {
                    s[i] -= servoSpeed;
                    pwm.writeMicroseconds(i, s[i]);
                    delay(5);
                    if(s[i] < g_sDur[i]) {
                        pwm.writeMicroseconds(i, g_sDur[i]);
                        s[i] = g_sDur[i];
                    }
                }
            }
        }
}


void setup() {
    Serial.begin(250000);

    pwm.begin();

    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

    s[0] = USMIN + float(USMAX - USMIN) * (float(90)/180);
    s[1] = USMIN + float(USMAX - USMIN) * (float(45)/180);
    s[2] = USMIN + float(USMAX - USMIN) * (float(180 - 180)/180);
    s[3] = USMIN + float(USMAX - USMIN) * (float(90)/180);
    s[4] = USMIN + float(USMAX - USMIN) * (float(180 - 0)/180);
    s[5] = USMIN + float(USMAX - USMIN) * (float(90)/180);
    s[6] = USMIN + float(USMAX - USMIN) * (float(180 - 0)/180);

    for(int i=0; i<7; i++) {
        pwm.writeMicroseconds(i, s[i]);
        delay(10);
    }
    
    // driveServo(8, 180);

    delay(1000);
}

void loop() {
    // Drive each servo one at a time using setPWM()
    String s;
    int count;
    uint16_t s1_Degree, s2_Degree, s3_Degree, s4_Degree, s5_Degree, s6_Degree;
    String returnText;
    String input;

    int delayTimer = 1;
 

    if (Serial.available() > 0) {
        count = 0;
        s1_Degree = (Serial.readStringUntil(':')).toInt();
        s2_Degree = (Serial.readStringUntil(':')).toInt();
        s3_Degree = (Serial.readStringUntil(':')).toInt();
        s4_Degree = (Serial.readStringUntil(':')).toInt();
        s5_Degree = (Serial.readStringUntil(':')).toInt();
        s6_Degree = (Serial.readStringUntil('s')).toInt(); // 's' is the initializer symbol for the servo's speed
        servoSpeed = (Serial.readStringUntil('t')).toInt(); // 't' is the initializer symbol for wether to use a servo speed
        input = (Serial.readStringUntil('\n')).toInt();
        if(input == "true") { useTimer = true; }

        if(s1_Degree < 0) {s1_Degree = 0;}
        if(s2_Degree < 0) {s2_Degree = 0;}
        if(s3_Degree < 0) {s3_Degree = 0;}
        if(s4_Degree < 0) {s4_Degree = 0;}
        if(s5_Degree < 0) {s5_Degree = 0;}
        if(s6_Degree < 0) {s6_Degree = 0;}
        if(s1_Degree > 180) {s1_Degree = 180;}
        if(s2_Degree > 180) {s2_Degree = 180;}
        if(s3_Degree > 180) {s3_Degree = 180;}
        if(s4_Degree > 180) {s4_Degree = 180;}
        if(s5_Degree > 180) {s5_Degree = 180;}
        if(s6_Degree > 180) {s6_Degree = 180;}

        returnText = "received: q1:" + String(s1_Degree)
                    + " q2:" + String(s2_Degree)
                    + " q3:" + String(s3_Degree)
                    + " q4:" + String(s4_Degree)
                    + " q5:" + String(s5_Degree)
                    + " q6:" + String(s6_Degree);

        // Serial.println(returnText);

        driveServo(6, s6_Degree, useTimer); delay(delayTimer);
        driveServo(5, s5_Degree, useTimer); delay(delayTimer);
        driveServo(4, s4_Degree, useTimer); delay(delayTimer);
        driveServo(3, s3_Degree, useTimer); delay(delayTimer);
        driveServo(2, s2_Degree, useTimer); delay(delayTimer);
        driveServo(1, s1_Degree, useTimer);
        // global_driveServo(
        //     s1_Degree,
        //     s2_Degree,
        //     s3_Degree,
        //     s4_Degree,
        //     s5_Degree,
        //     s6_Degree
        // );
        driveServo(7, 0 - (s2_Degree + s3_Degree + s5_Degree), useTimer);
        // driveServo(8, s2_Degree);
    }
}
