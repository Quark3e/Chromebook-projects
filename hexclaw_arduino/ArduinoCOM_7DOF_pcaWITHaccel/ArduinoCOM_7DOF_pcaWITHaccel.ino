
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

//accelerometer variables
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Outputs
float roll, pitch;
float Roll, Pitch; //filtered roll and pitch
int resolutionOpt = 32;

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

    Serial.println(":");
}

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
    Serial.begin(115200);

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
    delay(1000);
}

void loop() {
    // Drive each servo one at a time using setPWM()
    String s;
    int count;
    uint16_t s1_Degree, s2_Degree, s3_Degree, s4_Degree, s5_Degree, s6_Degree;
    String returnText;
    String input;

    int delayTimer = 0;
 
    readAccelerometer();

    if (Serial.available() > 0) {
        count = 0;
        //format: q1 : q2 : q3 : q4 : q5 : q6 s ServoSpeed t USETIMER(string) \n 
        s1_Degree = (Serial.readStringUntil(':')).toInt();
        s2_Degree = (Serial.readStringUntil(':')).toInt();
        s3_Degree = (Serial.readStringUntil(':')).toInt();
        s4_Degree = (Serial.readStringUntil(':')).toInt();
        s5_Degree = (Serial.readStringUntil(':')).toInt();
        s6_Degree = (Serial.readStringUntil('s')).toInt(); // 's' is the initializer symbol for the servo's speed
        servoSpeed = (Serial.readStringUntil('t')).toInt(); // 't' is the initializer symbol for wether to use a servo speed
        input = (Serial.readStringUntil('\n')).toInt();
        if(input == "true") { useTimer = true; }
        else { useTimer = false; }

        // if(s1_Degree < 0) {s1_Degree = 0;}
        // if(s2_Degree < 0) {s2_Degree = 0;}
        // if(s3_Degree < 0) {s3_Degree = 0;}
        // if(s4_Degree < 0) {s4_Degree = 0;}
        // if(s5_Degree < 0) {s5_Degree = 0;}
        // if(s6_Degree < 0) {s6_Degree = 0;}
        // if(s1_Degree > 180) {s1_Degree = 180;}
        // if(s2_Degree > 180) {s2_Degree = 180;}
        // if(s3_Degree > 180) {s3_Degree = 180;}
        // if(s4_Degree > 180) {s4_Degree = 180;}
        // if(s5_Degree > 180) {s5_Degree = 180;}
        // if(s6_Degree > 180) {s6_Degree = 180;}

        // returnText = "received: q1:" + String(s1_Degree)
        //             + " q2:" + String(s2_Degree)
        //             + " q3:" + String(s3_Degree)
        //             + " q4:" + String(s4_Degree)
        //             + " q5:" + String(s5_Degree)
        //             + " q6:" + String(s6_Degree);
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
