/*
 Name:		accelerometer_test.ino
 Created:	12/31/2021 4:28:59 PM
 Author:	aa82637
*/



// the setup function runs once when you press reset or power the board

#include <Servo.h>
#include <Wire.h>


int xLED = 3, yLED = 5, zLED = 7;
bool xMoving, yMoving, zMoving;
bool xPositive_dir, yPositive_dir, zPositive_dir;

int maxLimit = 3, minLimit = 1;
float timeLimit = 2, timeGap = 0.05;
float xTimer = 0, yTimer = 0, zTimer = 0;
float zOffset = 8.5;

int test = 12;

int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Outputs
float roll, pitch;
float Roll, Pitch; //filtered roll and pitch
int resolutionOpt = 32;


void xAxis_test() {

	if (int(abs(X_out)) > maxLimit) {
		xMoving = true;
		if (X_out < 0) { xPositive_dir = false; }
		else if (X_out > 0) { xPositive_dir = true; }
	}

	if (xMoving && xPositive_dir) {
		xTimer += timeGap;
		digitalWrite(xLED, HIGH);
		if (xTimer > timeLimit || X_out < minLimit) { xMoving = false; }
	}
	else if (xMoving && !xPositive_dir) {
		xTimer += timeGap;
		digitalWrite(xLED, HIGH);
		if (xTimer > timeLimit || abs(X_out) < minLimit) { xMoving = false; }
	}
	else {
		digitalWrite(xLED, LOW);
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
		digitalWrite(yLED, HIGH);
		if (yTimer > timeLimit || Y_out < minLimit) { yMoving = false; }
	}
	else if (yMoving && !yPositive_dir) {
		yTimer += timeGap;
		digitalWrite(yLED, HIGH);
		if (yTimer > timeLimit || abs(Y_out) < minLimit) { yMoving = false; }
	}
	else {
		digitalWrite(yLED, LOW);
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
		digitalWrite(zLED, HIGH);
		Serial.println(zTimer);
		if (zTimer > timeLimit || Z_out - zOffset < minLimit) { zMoving = false; }
	}
	else if (zMoving && !zPositive_dir) {
		zTimer += timeGap;
		digitalWrite(zLED, HIGH);
		if (zTimer > timeLimit || abs(Z_out) < minLimit) { zMoving = false; }
	}
	else {
		digitalWrite(zLED, LOW);
		zTimer = 0;
		Serial.println("----zMoving is set to off----");
	}


}

void setup() {
	Serial.begin(9600); // Initiate serial communication for printing the results on the Serial monitor
	pinMode(test, OUTPUT);
	pinMode(xLED, OUTPUT);
	pinMode(yLED, OUTPUT);
	pinMode(zLED, OUTPUT);

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
	Roll = 0.6 * Roll + 0.4 * roll;
	Pitch = 0.6 * Pitch + 0.4 * pitch;

	// Serial.print("Xa= ");
	// Serial.print(X_out);
	// Serial.print("   Ya= ");
	// Serial.print(Y_out);
	// Serial.print("   Za= ");
	// Serial.print(Z_out);
	Serial.print(" roll= ");
	Serial.print(Roll);
	Serial.print(" pitch= ");
	Serial.println(Pitch);
	// Serial.println("");

	// xAxis_test();
	// yAxis_test();
	// zAxis_test();

}