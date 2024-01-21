
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//#include<Adafruit_Sensor.h>

#include <Wire.h>
#include <Adafruit_ADXL345_U.h>

 
/// NOTE: TEMPORARILY DEPRECATED: Using nodemcu

/*AssignauniqueIDtothissensoratthesametime*/
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void displaySensorDetails(void){
    sensor_t sensor;
    accel.getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print("Sensor: "); Serial.println(sensor.name);
    Serial.print("DriverVer: "); Serial.println(sensor.version);
    Serial.print("UniqueID: "); Serial.println(sensor.sensor_id);
    Serial.print("MaxValue: "); Serial.print(sensor.max_value); Serial.println("m/s^2");
    Serial.print("MinValue: "); Serial.print(sensor.min_value); Serial.println("m/s^2");
    Serial.print("Resolution: "); Serial.print(sensor.resolution); Serial.println("m/s^2");
    Serial.println("------------------------------------"); 
    Serial.println("");
    delay(500);
} 

void displayDataRate(void){ 
    Serial.print("DataRate:");
    switch(accel.getDataRate()) {
        caseADXL345_DATARATE_3200_HZ:
        Serial.print("3200");
        break;
        caseADXL345_DATARATE_1600_HZ:
        Serial.print("1600");
        break;
        caseADXL345_DATARATE_800_HZ:
        Serial.print("800");
        break;
        caseADXL345_DATARATE_400_HZ:
        Serial.print("400");
        break;
        caseADXL345_DATARATE_200_HZ:
        Serial.print("200");
        break;
        caseADXL345_DATARATE_100_HZ:
        Serial.print("100");
        break;
        caseADXL345_DATARATE_50_HZ:
        Serial.print("50");
        break;
        caseADXL345_DATARATE_25_HZ:
        Serial.print("25");
        break;
        caseADXL345_DATARATE_12_5_HZ:
        Serial.print("12.5");
        break;
        caseADXL345_DATARATE_6_25HZ:
        Serial.print("6.25");
        break;
        caseADXL345_DATARATE_3_13_HZ:
        Serial.print("3.13");
        break;
        caseADXL345_DATARATE_1_56_HZ:
        Serial.print("1.56");
        break;
        caseADXL345_DATARATE_0_78_HZ:
        Serial.print("0.78");
        break;
        caseADXL345_DATARATE_0_39_HZ:
        Serial.print("0.39");
        break;
        caseADXL345_DATARATE_0_20_HZ:
        Serial.print("0.20");
        break;
        caseADXL345_DATARATE_0_10_HZ:
        Serial.print("0.10");
        break;
        default:
        Serial.print("????");
        break;
    }
    Serial.println("Hz");
}

void displayRange(void){
    Serial.print("Range:+/-");

    switch(accel.getRange()) {
        caseADXL345_RANGE_16_G: 
        Serial.print("16");
        break;
        caseADXL345_RANGE_8_G:
        Serial.print("8");
        break;
        caseADXL345_RANGE_4_G:
        Serial.print("4");
        break;
        caseADXL345_RANGE_2_G:
        Serial.print("2");
        break;
        default:
        Serial.print("??");
        break;
    }
    Serial.println("g");
}

const char*ssid="Telia-47118D";
const char*password="0C94C28B5D";

WiFiUDP Udp;
unsigned int localUdpPort=53;//localporttolistenon
char incomingPacket[255];//bufferforincomingpackets
char replyPacket[]="Hi there! Got the message:-)";//areplystringtosendback


float toDegrees(float radians) { return(radians*180)/M_PI; }


void setup() {
    Serial.begin(115200);
    
    pinMode(D8,OUTPUT);
    digitalWrite(D8,LOW);

    if(!accel.begin()) {
        /*TherewasaproblemdetectingtheADXL345...checkyourconnections*/
        Serial.println("Ooops, no ADXL345 detected...Check your wiring!");
        while(1);
    }
    
    /*Settherangetowhateverisappropriateforyourproject*/
    accel.setRange(ADXL345_RANGE_16_G);
    //displaySetRange(ADXL345_RANGE_8_G);
    //displaySetRange(ADXL345_RANGE_4_G);
    //displaySetRange(ADXL345_RANGE_2_G);
    
    /*Displaysomebasicinformationonthissensor*/
    displaySensorDetails();
    
    /*Displayadditionalsettings(outsidethescopeofsensor_t)*/
    displayDataRate();
    displayRange();

    Serial.println();
    Serial.printf("Connecting to %s",ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    int blinkCount=0;
    while(WiFi.status()!=WL_CONNECTED){
        if(blinkCount==4)digitalWrite(D8,HIGH);
        delay(500);
        Serial.print(".");
        if(blinkCount==4){
            digitalWrite(D8,LOW);
            blinkCount=0;
        }
        blinkCount++;
    }
    for(int i=0;i<3;i++){
        digitalWrite(D8,HIGH);
        delay(10);
        digitalWrite(D8,LOW);
        delay(10);
    }
    digitalWrite(D8,HIGH);
    Serial.println("connected");
    Udp.begin(localUdpPort);
    Serial.printf("NowlisteningatIP%s,UDPport%d\n",WiFi.localIP().toString().c_str(),localUdpPort);
    pinMode(0,INPUT_PULLUP);
}

float X_out=0, Y_out=0, Z_out=0;
String filterToggle="on";
  
void loop(){
    int packetSize=Udp.parsePacket();
    if(packetSize){//receiveincomingUDPpackets
        digitalWrite(D8, LOW);
        if(digitalRead(0)==HIGH) filterToggle="off;";
        else filterToggle="on ;";
        int len = Udp.read(incomingPacket,255);
        if(len>0){incomingPacket[len]=0;}

        sensors_event_t event;
        accel.getEvent(&event);

        X_out = event.acceleration.x/10;
        Y_out = event.acceleration.y/10;
        Z_out = event.acceleration.z/10;

        String resultStr="{"+
        String(X_out)+":"+
        String(Y_out)+":"+
        String(Z_out)+"}"+filterToggle;//if"off":toggletiltfilteroff
        
        int newPackLen = resultStr.length()+1;
        charnewReplyPack[newPackLen];
        resultStr.toCharArray(newReplyPack,newPackLen);

        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(newReplyPack);
        Udp.endPacket();
        digitalWrite(D8, HIGH);
    }
}