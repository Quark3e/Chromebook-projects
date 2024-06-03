
#include <Servo.h>

Servo servoX; //yaw
Servo servoY_1; //pitch for one servo
Servo servoY_2;


int xServoPos = 90;
int yServoPos_1 = 0;
int yServoPos_2 = 180;

void setup() {
	Serial.begin(250000);
	servoX.attach(11);
	servoY_1.attach(10);
	servoY_2.attach(9);
	
	servoX.write(xServoPos);
	servoY_1.write(yServoPos_1);
	servoY_2.write(yServoPos_2);
	delay(10);
}

void loop() {
	int xDegree, yDegree;

	if (Serial.available() > 0) {
		xDegree = (Serial.readStringUntil(':')).toInt();
		yDegree = (Serial.readStringUntil('\n')).toInt();

		servoX.write(xDegree);
		delay(10);
        if (yDegree < yServoPos_1 /*&& degree != 0*/) {
            while (yServoPos_1 > yDegree) {
                servoY_1.write(yServoPos_1);
                servoY_2.write(yServoPos_2);
                //delay(2);
                //digitalWrite(LED, HIGH);
                yServoPos_1 -= 1;
                yServoPos_2 += 1; //goes in reverse
            }
            //digitalWrite(LED, LOW);
        }
        else if (yDegree > yServoPos_1) {
            while (yServoPos_1 < yDegree) {
                servoY_1.write(yServoPos_1);
                servoY_2.write(yServoPos_2);
                //delay(2);
                //digitalWrite(LED, HIGH);
                yServoPos_1 += 1;
                yServoPos_2 -= 1; //goes in reverse
            }
            //digitalWrite(LED, LOW);
        }
	}
	delay(10);
}
