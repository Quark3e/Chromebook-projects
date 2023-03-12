
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

uint8_t servonum = 0; // servo counter


void driveServo(int servoNum, int degree) {
    //goes from 0 to 180
    //note: the servos on the "right" side of the robot's perspective must go in reverse.
    // so servo's q3, q5 and q7 must be set so it goes in reverse. This must be done
    // automatically inside this function, meaning you can send driveServo(45, 3) and it'll
    // calculate the correct reverse movement by default
    
    //uint16_t pulselength = SERVOMIN + float(SERVOMAX - SERVOMIN) * (float(degree)/180);

    if(servoNum == 3 || servoNum == 5 || servoNum == 7) {
      degree = 180 - degree;    
    }
    
    uint16_t microsec_dur = USMIN + float(USMAX - USMIN) * (float(degree)/180);
    
    
    Serial.print("moving servo: q");
    Serial.print(servoNum);
    Serial.print(" to ");
    Serial.print(degree);
    Serial.println(" degrees");
    pwm.writeMicroseconds(servoNum - 1, microsec_dur);
    Serial.println(microsec_dur);
    delay(100);
}

void setup() {
    Serial.begin(9600);
    Serial.println("8 channel Servo test!");

    pwm.begin();

    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

    delay(100);

    driveServo(1, 90);
    driveServo(2, 90);
    driveServo(3, 135);
    driveServo(4, 90);
    driveServo(5, 90);
    driveServo(6, 90);
    driveServo(7, 90);
    driveServo(8, 180);
    delay(1000);
    Serial.println("Enter servonum to change it's degree...");
}

void loop() {
    // Drive each servo one at a time using setPWM()
    String input;
    int servoNum;
    int rotation;
  
    while(true) {
        if (Serial.available() > 0) {
            input = Serial.readStringUntil('\n');
            servoNum = input.toInt();
            Serial.print("changing servo #");
            Serial.print(input);
            Serial.print(" rotation to (note: q3 is set to 135degrees when upright): ");
            while(Serial.available() == 0) {}
            input = Serial.readStringUntil('\n');
            rotation = input.toInt();
            Serial.print(rotation);
            Serial.println(" degrees");
            driveServo(servoNum, rotation);
            Serial.println("finished");
            Serial.println("----------------------");
            Serial.println("Enter servonum to change it's degree...");
        }
    }
}
