
#include <Servo.h>

Servo servoX; //"yaw"
Servo servoY_1; //"pitch
Servo servoY_2; //"pitch" two

int LED = 13;

int servoPosX = 90; //to store servo's x position
int servoPosY_1 = 90; //to store servo's y position
int servoPosY_2 = 90;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(250000);
    pinMode(LED, OUTPUT);
    servoX.attach(11);
    servoY_1.attach(10);
    servoY_2.attach(9);
    servoX.write(servoPosX);
    servoY_1.write(servoPosY_1);
    servoY_2.write(servoPosY_2);
}

void loop() {
    //put your main code here, to run repeatedly:
    //delay(500);
    String s;
    String return_text;
    int count;
    int xDegree, yDegree;
 
    if (Serial.available() > 0) {
        String dataIn;
        count = 0;
        xDegree = (Serial.readStringUntil(':')).toInt();
        yDegree = (Serial.readStringUntil('\n')).toInt();
        
        //return_text = "info variable string is: " + dataIn;
        //Serial.println(yDegree, DEC);

        if (yDegree < servoPosY_1 /*&& degree != 0*/) {
            while (servoPosY_1 > yDegree) {
                servoY_1.write(servoPosY_1);
                servoY_2.write(servoPosY_2);
                //delay(2);
                //digitalWrite(LED, HIGH);
                servoPosY_1 -= 1;
                servoPosY_2 += 1; //goes in reverse
            }
            //digitalWrite(LED, LOW);
        }
        else if (yDegree > servoPosY_1) {
            while (servoPosY_1 < yDegree) {
                servoY_1.write(servoPosY_1);
                servoY_2.write(servoPosY_2);
                //delay(2);
                //digitalWrite(LED, HIGH);
                servoPosY_1 += 1;
                servoPosY_2 -= 1; //goes in reverse
            }
            //digitalWrite(LED, LOW);
        }

        if (xDegree < servoPosX /*&& degree != 0*/) {
            for (0; servoPosX > xDegree; servoPosX -= 1) {
                servoX.write(servoPosX);
                //delay(2);
            }
        }
        else if (xDegree > servoPosX) {
            for (0; servoPosX < xDegree; servoPosX += 1) {
                servoX.write(servoPosX);
                //delay(2);
            }
        }
        //delay(100);

        //Serial.print("x: ");
        //Serial.print(servoPosX);
        //Serial.print(" y: ");
        //Serial.println(servoPosY);

        digitalWrite(LED, HIGH);
        //delay(100);
        digitalWrite(LED, LOW);

        //Serial.println("");
        //Serial.print("Servo position: ");
        //Serial.println(servoPos);

        //for (int i = 0; i < 64; i++) { Serial.read(); }
        while (Serial.read() != -1) { count++; }
        //Serial.print("the \"Serial.read()\" has repeated: ");
        //Serial.print(count);
        //Serial.println(" times");
    }
}