
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
// #include <Adafruit_Sensor.h>

#include <Wire.h>
#include <Adafruit_ADXL345_U.h>

 
/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void displaySensorDetails(void) {
    sensor_t sensor;
    accel.getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print("Sensor: "); Serial.println(sensor.name);
    Serial.print("Driver Ver: "); Serial.println(sensor.version);
    Serial.print("Unique ID: "); Serial.println(sensor.sensor_id);
    Serial.print("Max Value: "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
    Serial.print("Min Value: "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
    Serial.print("Resolution: "); Serial.print(sensor.resolution); Serial.println(" m/s^2"); 
    Serial.println("------------------------------------");
    Serial.println("");
    delay(500);
}

void displayDataRate(void) {
    Serial.print("Data Rate: "); 
    switch(accel.getDataRate()) {
    case ADXL345_DATARATE_3200_HZ:
    Serial.print("3200 "); 
    break;
    case ADXL345_DATARATE_1600_HZ:
    Serial.print("1600 "); 
    break;
    case ADXL345_DATARATE_800_HZ:
    Serial.print("800 "); 
    break;
    case ADXL345_DATARATE_400_HZ:
    Serial.print("400 "); 
    break;
    case ADXL345_DATARATE_200_HZ:
    Serial.print("200 "); 
    break;
    case ADXL345_DATARATE_100_HZ:
    Serial.print("100 "); 
    break;
    case ADXL345_DATARATE_50_HZ:
    Serial.print("50 "); 
    break;
    case ADXL345_DATARATE_25_HZ:
    Serial.print("25 "); 
    break;
    case ADXL345_DATARATE_12_5_HZ:
    Serial.print("12.5 "); 
    break;
    case ADXL345_DATARATE_6_25HZ:
    Serial.print("6.25 "); 
    break;
    case ADXL345_DATARATE_3_13_HZ:
    Serial.print("3.13 "); 
    break;
    case ADXL345_DATARATE_1_56_HZ:
    Serial.print("1.56 "); 
    break;
    case ADXL345_DATARATE_0_78_HZ:
    Serial.print("0.78 "); 
    break;
    case ADXL345_DATARATE_0_39_HZ:
    Serial.print("0.39 "); 
    break;
    case ADXL345_DATARATE_0_20_HZ:
    Serial.print("0.20 "); 
    break;
    case ADXL345_DATARATE_0_10_HZ:
    Serial.print("0.10 "); 
    break;
    default:
    Serial.print("???? "); 
    break;
    } 
    Serial.println(" Hz"); 
}

void displayRange(void) {
    Serial.print("Range: +/- "); 

    switch(accel.getRange()) {
    case ADXL345_RANGE_16_G:
    Serial.print("16 "); 
    break;
    case ADXL345_RANGE_8_G:
    Serial.print("8 "); 
    break;
    case ADXL345_RANGE_4_G:
    Serial.print("4 "); 
    break;
    case ADXL345_RANGE_2_G:
    Serial.print("2 "); 
    break;
    default:
    Serial.print("?? "); 
    break;
    } 
    Serial.println(" g"); 
}

const char* ssid = "Telia-47118D";
const char* password = "0C94C28B5D";

WiFiUDP Udp;
unsigned int localUdpPort = 53; // local port to listen on
char incomingPacket[255]; // buffer for incoming packets
char replyPacket[] = "Hi there! Got the message :-)"; // a reply string to send back


float toDegrees(float radians) { return (radians*180)/M_PI; }


void setup() {
    Serial.begin(115200);
    
    if(!accel.begin()) {
        /* There was a problem detecting the ADXL345 ... check your connections */
        Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
        while(1);
    }
    
    /* Set the range to whatever is appropriate for your project */
    accel.setRange(ADXL345_RANGE_16_G);
    // displaySetRange(ADXL345_RANGE_8_G);
    // displaySetRange(ADXL345_RANGE_4_G);
    // displaySetRange(ADXL345_RANGE_2_G);
    
    /* Display some basic information on this sensor */
    displaySensorDetails();
    
    /* Display additional settings (outside the scope of sensor_t) */
    displayDataRate();
    displayRange();


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
    /* Get a new sensor event */ 
    sensors_event_t event; 
    accel.getEvent(&event);

    /* Display the results (acceleration is measured in m/s^2) */
    Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print(" ");
    Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print(" ");
    Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print(" ");Serial.println("m/s^2 ");
    delay(500);

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