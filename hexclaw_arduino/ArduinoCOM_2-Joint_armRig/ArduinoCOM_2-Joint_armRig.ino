
#include <Wire.h>
#include <Servo.h>

Servo servo1; //"yaw"
Servo servo2_1; //joint 1; q_1
Servo servo2_2; //joint 1; q_1
Servo servo3; //joint 2; q_2
Servo servo4;


int LED = 13;

bool xMoving, yMoving, zMoving;
bool xPositive_dir, yPositive_dir, zPositive_dir;

int maxLimit = 3, minLimit = 1;
float timeLimit = 2, timeGap = 0.05;
float xTimer = 0, yTimer = 0, zTimer = 0;
float zOffset = 8.5;

//servo motor variables
int servoPos1 = 90; //to store servo's x position
int servoPos2_1 = 90; //to store servo's y position
int servoPos2_2 = 90;
int servoPos3 = 35; //joint 2 (q2)
int servoPos4 = 90; //arm "head" tilt (roll)

//accelerometer variables
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Outputs
float roll, pitch;
float Roll, Pitch; //filtered roll and pitch

int resolutionOpt = 32; /*personal note: change to 1 to increase output sensitivity to 256*/

String toProgram;


void xAxis_test() {

    if (int(abs(X_out)) > maxLimit) {
        xMoving = true;
        if (X_out < 0) { xPositive_dir = false; }
        else if (X_out > 0) { xPositive_dir = true; }
    }

    if (xMoving && xPositive_dir) {
        xTimer += timeGap;
        if (xTimer > timeLimit || X_out < minLimit) { xMoving = false; }
        Serial.print(":x");
        Serial.print(X_out);
    }
    else if (xMoving && !xPositive_dir) {
        xTimer += timeGap;
        if (xTimer > timeLimit || abs(X_out) < minLimit) { xMoving = false; }
        Serial.print(":x");
        Serial.print(X_out);
    }
    else {
        xTimer = 0;
    }
}
void yAxis_test() {

    if (int(abs(Y_out)) > maxLimit) {
        yMoving = true;
        if (Y_out < 0) { yPositive_dir = false; }
        else if (Y_out > 0) { yPositive_dir = true; }
    }
    if (yMoving && yPositive_dir) {
        yTimer += timeGap;
        if (yTimer > timeLimit || Y_out < minLimit) { yMoving = false; }
        Serial.print(":y");
        Serial.print(Y_out);
    }
    else if (yMoving && !yPositive_dir) {
        yTimer += timeGap;
        if (yTimer > timeLimit || abs(Y_out) < minLimit) { yMoving = false; }
        Serial.print(":y");
        Serial.print(Y_out);
    }
    else {
        yTimer = 0;
    }
}
void zAxis_test() {

    if (int(abs(Z_out) - zOffset) > maxLimit) {
        zMoving = true;
        if (Z_out < 0) { zPositive_dir = false; }
        else if (Z_out > 0) { zPositive_dir = true; }
    }
    if (zMoving && zPositive_dir) {
        zTimer += timeGap;
        Serial.println(zTimer);
        if (zTimer > timeLimit || Z_out - zOffset < minLimit) { zMoving = false; }
        Serial.print(":z");
        Serial.print(Z_out);
    }
    else if (zMoving && !zPositive_dir) {
        zTimer += timeGap;
        if (zTimer > timeLimit || abs(Z_out) < minLimit) { zMoving = false; }
        Serial.print(":z");
        Serial.print(Z_out);
    }
    else {
        zTimer = 0;
        Serial.println("----zMoving is set to off----");
    }
}

void readAccelerometer() {
    // === Read acceleromter data === //
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
    X_out = (Wire.read() | Wire.read() << 8); // X-axis value
    X_out = X_out / resolutionOpt; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
    Y_out = (Wire.read() | Wire.read() << 8); // Y-axis value
    Y_out = Y_out / resolutionOpt;
    Z_out = (Wire.read() | Wire.read() << 8); // Z-axis value
    Z_out = Z_out / resolutionOpt;

    //x is roll and y is pitch (it's switched so the servo can be fit to the servo robot arm)
    pitch = atan(Y_out / sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / PI;
    roll = atan(-1 * X_out / sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / PI;

    //filter
    Roll = 0.8 * Roll + 0.2 * roll;
    Pitch = 0.8 * Pitch + 0.2 * pitch;

    /*Serial.print("Xa= ");
    Serial.print(X_out);
    Serial.print("   Ya= ");
    Serial.print(Y_out);
    Serial.print("   Za= ");
    Serial.print(Z_out);*/
    Serial.print(":r");
    Serial.print(Roll);
    Serial.print(":p");
    Serial.print(Pitch);

    xAxis_test();
    yAxis_test();
    zAxis_test();

    Serial.println(":");
} /*
  The serial monitor will hold each sent character
  with a letter of either the coord axis letter or
  first letter of tilt name, ending with a colon symbol.
  */


void setup() {
    // put your setup code here, to run once:
    Serial.begin(250000);
    pinMode(LED, OUTPUT);
    servo1.attach(11);
    servo2_1.attach(10);
    servo2_2.attach(9);
    servo3.attach(6);
    servo4.attach(5);

    //define servo's
    servo1.write(servoPos1);
    servo2_1.write(servoPos2_1);
    servo2_2.write(servoPos2_2);
    servo3.write(servoPos3);
    servo4.write(servoPos4);

    xMoving = false;
    yMoving = false;
    zMoving = false;
    xPositive_dir = false;
    yPositive_dir = false;
    zPositive_dir = false;

    Wire.begin(); // Initiate the Wire library
    // Set ADXL345 in measuring mode
    Wire.beginTransmission(ADXL345); // Start communicating with the device 
    Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
    // Enable measurement
    Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
    Wire.endTransmission();
    delay(10);

    //offset calibration
    //X-axis
    Wire.beginTransmission(ADXL345);
    Wire.write(0x1E);
    Wire.write(-6);
    Wire.endTransmission();
    delay(10);
    //Y-axis
    Wire.beginTransmission(ADXL345);
    Wire.write(0x1F);
    Wire.write(-4);
    Wire.endTransmission();
    delay(10);
    //Z-axis
    Wire.beginTransmission(ADXL345);
    Wire.write(0x20);
    Wire.write(-5);
    Wire.endTransmission();
    delay(10);
}

void loop() {
    //put your main code here, to run repeatedly:
    //delay(500);
    String s;
    String return_text;
    int count;
    int s1_Degree, s2_Degree, s3_Degree, s4_Degree;

    readAccelerometer();

    if (Serial.available() > 0) {
        String dataIn;
        count = 0;
        s1_Degree = (Serial.readStringUntil(':')).toInt();
        s2_Degree = (Serial.readStringUntil(':')).toInt();
        s3_Degree = (Serial.readStringUntil('\n')).toInt();
        s4_Degree = 90 + Roll;

        //return_text = "info variable string is: " + dataIn;
        //Serial.println(yDegree, DEC);
        if (s4_Degree < servoPos4 /*&& degree != 0*/) {
            for (0; servoPos4 > s4_Degree; servoPos4 -= 1) { servo4.write(servoPos4); }
        }
        else if (s4_Degree > servoPos4) {
            for (0; servoPos4 < s4_Degree; servoPos4 += 1) { servo4.write(servoPos4); }
        }

        if (s3_Degree < servoPos3 /*&& degree != 0*/) {
            for (0; servoPos3 > s3_Degree; servoPos3 -= 1) {
                servo3.write(servoPos3);
                //delay(2);
            }
        }
        else if (s3_Degree > servoPos3) {
            for (0; servoPos3 < s3_Degree; servoPos3 += 1) {
                servo3.write(servoPos3);
                //delay(2);
            }
        }

        if (s2_Degree < servoPos2_1 /*&& degree != 0*/) {
            while (servoPos2_1 > s2_Degree) {
                servo2_1.write(servoPos2_1);
                servo2_2.write(servoPos2_2);
                //delay(2);
                //digitalWrite(LED, HIGH);
                servoPos2_1 -= 1;
                servoPos2_2 += 1; //goes in reverse
            }
            //digitalWrite(LED, LOW);
        }
        else if (s2_Degree > servoPos2_1) {
            while (servoPos2_1 < s2_Degree) {
                servo2_1.write(servoPos2_1);
                servo2_2.write(servoPos2_2);
                //delay(2);
                //digitalWrite(LED, HIGH);
                servoPos2_1 += 1;
                servoPos2_2 -= 1; //goes in reverse
            }
            //digitalWrite(LED, LOW);
        }

        if (s1_Degree < servoPos1 /*&& degree != 0*/) {
            for (0; servoPos1 > s1_Degree; servoPos1 -= 1) {
                servo1.write(servoPos1);
                //delay(2);
            }
        }
        else if (s1_Degree > servoPos1) {
            for (0; servoPos1 < s1_Degree; servoPos1 += 1) {
                servo1.write(servoPos1);
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