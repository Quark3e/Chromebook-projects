
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

const char* ssid = "Telia-47118D";
const char* password = "0C94C28B5D";

WiFiUDP Udp;
unsigned int localUdpPort = 53; // local port to listen on
char incomingPacket[255]; // buffer for incoming packets
char replyPacket[] = "Hi there! Got the message :-)"; // a reply string to send back


float toDegrees(float radians) { return (radians*180)/M_PI; }

void setup() {
    Serial.begin(115200);
      // Try to initialize!
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while(1) {delay(10);}
    }
    Serial.println("MPU6050 Found!");

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    Serial.print("Accelerometer range set to: ");
    switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
        Serial.println("+-2G");
        break;
    case MPU6050_RANGE_4_G:
        Serial.println("+-4G");
        break;
    case MPU6050_RANGE_8_G:
        Serial.println("+-8G");
        break;
    case MPU6050_RANGE_16_G:
        Serial.println("+-16G");
        break;
    }
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    Serial.print("Gyro range set to: ");
    switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
        Serial.println("+- 250 deg/s");
        break;
    case MPU6050_RANGE_500_DEG:
        Serial.println("+- 500 deg/s");
        break;
    case MPU6050_RANGE_1000_DEG:
        Serial.println("+- 1000 deg/s");
        break;
    case MPU6050_RANGE_2000_DEG:
        Serial.println("+- 2000 deg/s");
        break;
    }
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
    Serial.print("Filter bandwidth set to: ");
    switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
        Serial.println("260 Hz");
        break;
    case MPU6050_BAND_184_HZ:
        Serial.println("184 Hz");
        break;
    case MPU6050_BAND_94_HZ:
        Serial.println("94 Hz");
        break;
    case MPU6050_BAND_44_HZ:
        Serial.println("44 Hz");
        break;
    case MPU6050_BAND_21_HZ:
        Serial.println("21 Hz");
        break;
    case MPU6050_BAND_10_HZ:
        Serial.println("10 Hz");
        break;
    case MPU6050_BAND_5_HZ:
        Serial.println("5 Hz");
        break;
    }

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

        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        String resultStr = "{"+String(int(g.gyro.x))+":"+String(int(g.gyro.y))+"}";

        int newPackLen = resultStr.length()+1;
        char newReplyPack[newPackLen];
        resultStr.toCharArray(newReplyPack, newPackLen);

        Udp.beginPacket(Udp.remoteIP(),
        Udp.remotePort()); Udp.write(newReplyPack);
        Udp.endPacket();
    }
}