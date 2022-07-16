
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


float d1 = 140; //axial "roll"
float d2 = 135; //axial "pitch"
float d3 = 70; //axial "pitch"
float d4 = 80; //axial "roll"
float d5 = 45; //axial "pitch
float d6 = 30; //axial "roll" (?)

float toDegrees(float radians) {
	return (radians * 180) / PI;
}

float toRadians(float degrees) {
	return (degrees * PI) / 180;
}

void get_Angles(
	// Vector3f PP,
    float posX,
    float posY,
    float posZ,
	float* q1,
	float* q2,
	float* q3,
	float* q4,
	float* q5,
	float* q6,
	float* a1,
	float* b1,
	// Vector3f* P2,
    float* posX2,
    float* posY2,
    float* posZ2,
	float a,
	float b,
	float Y,
	char posOption,
	float length_scalar = 1,
	float coord_scalar = 1,
	bool printText = false
) {
	float lambda, mu;
	float d5x, d5z;

	posX = posX * coord_scalar;
	posY = posY * coord_scalar;
	posZ = posZ * coord_scalar;

	d1 = d1 * length_scalar;
	d2 = d2 * length_scalar;
	d3 = d3 * length_scalar;
	d4 = d4 * length_scalar;
	d5 = d5 * length_scalar;
	d6 = d6 * length_scalar;


	float l = (d5+d6) * cos(b);
	(*posX2) = posX - l * sin(a);
	(*posY2) = posY - l * cos(a);
	(*posZ2) = posZ - (d5+d6) * sin(b);

	if ((*posY2) == 0) {
		(*posY2) = 0.00001;
	}
	else if ((*posY2) < 0) {
		(*posY2) = 0;
	}

	(*q1) = atan((*posX2) / (*posY2));

	if (posOption == '+') {
		(*q3) = acos(
			(pow((*posX2), 2) + pow((*posY2), 2) + pow(*posZ2 - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /
			(2 * d2 * (d3 + d4))
		);
	}
	else if (posOption == '-') {
		(*q3) = acos(
			(pow((*posX2), 2) + pow((*posY2), 2) + pow(*posZ2 - d1, 2) - pow(d2, 2) - pow(d3 + d4, 2)) /
			(2 * d2 * (d3 + d4))
		);
	}
	lambda = atan((*posZ2 - d1) / sqrt(pow((*posX2), 2) + pow((*posY2), 2)));
	mu = atan(
		((d3 + d4) * sin(*q3)) /
		(d2 + (d3 + d4) * cos(*q3))
	);

	//cout << "lambda: " << toDegrees(lambda) << " mu: " << toDegrees(mu) << endl;

	if (posOption == '+')
		(*q2) = lambda - mu;
	else if (posOption == '-') {
		(*q2) = lambda + mu;
		(*q3) = 0 - (*q3);
	}

	(*a1) = a - (*q1);
	(*b1) = b - (*q2 + *q3);

	d5x = (d5+d6) * cos(*b1) * sin(*a1); //NOTE: The x and y axis of the X1Y1Z1 frame in the paper was reverse (compared to this. The names need to be changed!)
	d5z = (d5+d6) * sin(*b1);

	if ((*b1) == 0)
		(*q4) = 0;
	else if ((*b1) < 0 || (*b1) > 0)
		(*q4) = atan(d5x / d5z);

	/*if (cos(*b1) * cos(*a1) == 0) {
		(*q5) = 0;
	}*/
	//else {
	double check = asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / (d5+d6));
	if (isnan(check)) {
		if (printText) {
			Serial.print("asin(sqrt(pow(d5x, 2) + pow(d5z, 2)) / d5)  is NaN\n");
		}
	}
	else {
		if (sqrt(pow(d5x, 2) + pow(d5z, 2)) > 90.0) {
			(*q5) = toRadians(90);
		}
		else {
			(*q5) = asin(
				sqrt(pow(d5x, 2) + pow(d5z, 2)) /
				(d5+d6)
			);
		}
	}


	if (d5z < 0) {
		(*q5) = 0.0 - (*q5);
	}

	if (b <= PI / 2 && b >= 0 - PI / 2)
		(*q6) = Y - (*q4);
	else if (b >= PI / 2 || b <= 0 - PI / 2)
		(*q6) = PI - (Y - (*q6));

    if (printText) {
        Serial.print("q1:");
        Serial.print(*q1);
        Serial.print(" q2:");
        Serial.print(*q2);
        Serial.print(" q3:");
        Serial.print(*q3);
        Serial.print(" q4:");
        Serial.print(*q4);
        Serial.print(" q5:");
        Serial.print(*q5);
        Serial.print(" q6:");
        Serial.println(*q6);

		printf("\nq1:%f q2:%f q3:%f q4:%f q5:%f q6:%f\n",
			toDegrees(*q1),
			toDegrees(*q2),
			toDegrees(*q3),
			toDegrees(*q4),
			toDegrees(*q5),
			toDegrees(*q6)
		);
    }
}   
// note: all angles ares in radian, whether input or output;
// it will not let point P2 be behind the X-axis 
// (i.e have a negative Y value)



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
