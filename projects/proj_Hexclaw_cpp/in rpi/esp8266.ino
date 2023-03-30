
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
// #include <Adafruit_ADXL345_U.h>
// #include <Adafruit_Sensor.h>

#include <Wire.h>
int ADXL345 = 0x53;
float X_out, Y_out, Z_out;
float roll, pitch, Roll = 0, Pitch = 0;
int resolutionOpt = 32;

const char* ssid = "Telia-47118D";
const char* password = "0C94C28B5D";

WiFiUDP Udp;
unsigned int localUdpPort = 53; // local port to listen on
char incomingPacket[255]; // buffer for incoming packets
char replyPacket[] = "Hi there! Got the message :-)"; // a reply string to send back


float toDegrees(float radians) { return (radians*180)/M_PI; }

void readAccelerometer() {
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
    // pitch = atan(Y_out / sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / PI;
    // roll = atan(-1 * X_out / sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / PI;

    // //filter
    // Roll = 0.8 * Roll + 0.2 * roll;
    // Pitch = 0.8 * Pitch + 0.2 * pitch;

}

void setup() {
    Serial.begin(115200);
    
    Wire.begin();
    Wire.beginTransmission(ADXL345);
    Wire.write(0x2D);
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


    Serial.println();
    Serial.printf("Connecting to %s ", ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected");
    Udp.begin(localUdpPort);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
    pinMode(0, INPUT_PULLUP);
}

void loop() {
    int packetSize = Udp.parsePacket();
    if (packetSize) { // receive incoming UDP packets
        int len = Udp.read(incomingPacket, 255);
        if (len > 0) { incomingPacket[len] = 0; }

        String resultStr = "{"+
        String(X_out)+":"+
        String(Y_out)+":"+
        String(Z_out)+"}";

        int newPackLen = resultStr.length()+1;
        char newReplyPack[newPackLen];
        resultStr.toCharArray(newReplyPack, newPackLen);

        Udp.beginPacket(Udp.remoteIP(),
        Udp.remotePort()); Udp.write(newReplyPack);
        Udp.endPacket();
        readAccelerometer();
    }
}