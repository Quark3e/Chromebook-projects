
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
// #include <Adafruit_Sensor.h>

#include <Wire.h>
#include <Adafruit_ADXL345_U.h>


#include <tft_inclinometer.hpp>

bool useAccel   = true;
bool useWiFi    = false;
bool useTFTscr  = false;

#define TFT_RST		D4		// TFT RST	pin is connected to NodeMCU pin D4 (GPIO02)
#define TFT_CS		D3		// TFT CS	pin is connected to NodeMCU pin D3 (GPIO00)
#define TFT_DC		D0 //EXPERIMENTAL: WORKS: 2		// TFT DC	pin is connected to NodeMCU pin D2 (GPIO04)

Adafruit_ST7735 tftObj = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

tftInclinometer_ST7735 inclinometer = tftInclinometer_ST7735(&tftObj);

/*Assign an unique ID to this sensor at the same time*/
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

#include <displayFunc.hpp>

const char*ssid="Telia-47118D";
const char*password="0C94C28B5D";

// const char *ssid="Stockholms_stadsbibliotek";
// const char *password="stockholm";

WiFiUDP Udp;
unsigned int localUdpPort=53;//localporttolistenon
char incomingPacket[255];//bufferforincomingpackets
char replyPacket[]="Hi there! Got the message:-)";//areplystringtosendback


float toDegrees(float radians) { return(radians*180)/M_PI; }

unsigned long t1;
String oldStr0, oldStr1;
int frames;


void setup() {
    Serial.begin(115200);
    Serial.flush();
    // Wire.setClock(400000); //experimental

    pinMode(D8,OUTPUT);
    digitalWrite(D8,LOW);
    
    if(useAccel && !accel.begin()) {
        /*TherewasaproblemdetectingtheADXL345...checkyourconnections*/
        Serial.println("Ooops, no ADXL345 detected...Check your wiring!");
        while(1);
    
        
        /*Settherangetowhateverisappropriateforyourproject*/
        accel.setRange(ADXL345_RANGE_16_G);
        
        /*Displaysomebasicinformationonthissensor*/
        displaySensorDetails();
        
        /*Displayadditionalsettings(outsidethescopeofsensor_t)*/
        displayDataRate();
        displayRange();
    }

    Serial.println();
    if(useWiFi) {
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
        Serial.println("connected");
        Udp.begin(localUdpPort);
        Serial.printf("Now listening at IP %s, UDPport %d\n",WiFi.localIP().toString().c_str(), localUdpPort);
    }
    digitalWrite(D8,HIGH);
    pinMode(0,INPUT_PULLUP);
    // inclinometer = tftInclinometer_ST7735(&tftObj);
    if(useTFTscr) inclinometer.tftSetup();

    t1=millis();
}

float X_out=0.1, Y_out=0.1, Z_out=0.1;
String filterToggle="on";
int angle=0;

String totStr;

void readAccelerometer() {
    sensors_event_t event;

    accel.getEvent(&event);

    X_out = event.acceleration.x/10;
    Y_out = event.acceleration.y/10;
    Z_out = event.acceleration.z/10;

    totStr = "read ["+String(X_out,2)+" "+String(Y_out,2)+" "+String(Z_out,2)+"]";

    // Serial.print("read; [");
    // Serial.print(String(X_out, 2)); Serial.print(" ");
    // Serial.print(String(Y_out, 2)); Serial.print(" ");
    // Serial.print(String(Z_out, 2)); Serial.println("]");
    Serial.println(totStr);

}


void loop() {
  
    if(useAccel) readAccelerometer();
    
    if(!useAccel && useTFTscr) {
        inclinometer.update(
            sin(radians(45)*sin(radians(angle))),
            sin(radians(45)*sin(radians(angle+90))),
            cos(radians(45))
        );
    }
    if(useAccel && useTFTscr)
        inclinometer.update(X_out, Y_out, Z_out);


    if(useTFTscr) {
        inclinometer.drawText(oldStr0, 0, 0, ST7735_BLACK, 1);
        inclinometer.drawText(oldStr1, 70, 0, ST7735_BLACK, 1);

        oldStr0 = "FPS:"+String(float(frames*1000)/(millis()-t1),2);
        oldStr1 = String((millis()-t1))+"ms";

        inclinometer.drawText(oldStr0, 0, 0, ST7735_RED, 1, false);
        inclinometer.drawText(oldStr1, 70, 0, ST7735_RED, 1, false);
        
        t1 = millis();
        frames=1;
    }
    // else frames++;


    if(useWiFi) {
        int packetSize=Udp.parsePacket();
        if(packetSize) {//receiveincomingUDPpackets
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
            char newReplyPack[newPackLen];
            resultStr.toCharArray(newReplyPack,newPackLen);

            Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
            Udp.write(newReplyPack);
            Udp.endPacket();
            digitalWrite(D8, HIGH);
        }
    }
    if(!useAccel) {
        angle+=10;
        if(angle>360) angle=0;
    }
}

