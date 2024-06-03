
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
// #include <Adafruit_Sensor.h>

#include <Wire.h>
#include <Adafruit_ADXL345_U.h>


#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

class tftInclinometer_ST7735 {
    Adafruit_ST7735 *tftPtr;


    uint16_t bgColour = ST7735_BLACK;

    void drawAccel();
    void drawRoll();
    void drawPitch();

    int rollPos[2]  = {120, 45};
    int pitchPos[2] = {120, 109};

    public:
    float offsets_accel[3] = {0, 0, 0};

    float accel[3] = {0.1, 0.1, 0.1}; /* {x, y, z} */
    float orient[2] = {0, 0}; /* {roll, pitch} */

    tftInclinometer_ST7735(Adafruit_ST7735 *tft_pointer);
    void drawText(String text, uint16_t cursorX, uint16_t cursorY, uint16_t colour=ST7735_BLACK, uint8_t textSize=1, bool useBG=false);
    void tftSetup();
    void solveOrients();
    void solveOrients(float x, float y, float z);
    void update();
    void update(float readAccel[3]);
    void update(float xAccel, float yAccel, float zAccel);

    float toRadians(float degrees);
    float toDegrees(float radians);

    void setAccel(float x, float y, float z);


};


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


void displaySensorDetails();
void displayDataRate();
void displayRange(void);


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



void displaySensorDetails(){
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

void displayDataRate(){ 
    Serial.print("DataRate:");
    switch(accel.getDataRate()) {
        case ADXL345_DATARATE_3200_HZ:
        Serial.print("3200");
        break;
        case ADXL345_DATARATE_1600_HZ:
        Serial.print("1600");
        break;
        case ADXL345_DATARATE_800_HZ:
        Serial.print("800");
        break;
        case ADXL345_DATARATE_400_HZ:
        Serial.print("400");
        break;
        case ADXL345_DATARATE_200_HZ:
        Serial.print("200");
        break;
        case ADXL345_DATARATE_100_HZ:
        Serial.print("100");
        break;
        case ADXL345_DATARATE_50_HZ:
        Serial.print("50");
        break;
        case ADXL345_DATARATE_25_HZ:
        Serial.print("25");
        break;
        case ADXL345_DATARATE_12_5_HZ:
        Serial.print("12.5");
        break;
        case ADXL345_DATARATE_6_25HZ:
        Serial.print("6.25");
        break;
        case ADXL345_DATARATE_3_13_HZ:
        Serial.print("3.13");
        break;
        case ADXL345_DATARATE_1_56_HZ:
        Serial.print("1.56");
        break;
        case ADXL345_DATARATE_0_78_HZ:
        Serial.print("0.78");
        break;
        case ADXL345_DATARATE_0_39_HZ:
        Serial.print("0.39");
        break;
        case ADXL345_DATARATE_0_20_HZ:
        Serial.print("0.20");
        break;
        case ADXL345_DATARATE_0_10_HZ:
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
        case ADXL345_RANGE_16_G: 
        Serial.print("16");
        break;
        case ADXL345_RANGE_8_G:
        Serial.print("8");
        break;
        case ADXL345_RANGE_4_G:
        Serial.print("4");
        break;
        case ADXL345_RANGE_2_G:
        Serial.print("2");
        break;
        default:
        Serial.print("??");
        break;
    }
    Serial.println("g");
}



tftInclinometer_ST7735::tftInclinometer_ST7735(Adafruit_ST7735 *tft_pointer)
    // TFT_RST(D4), TFT_CS(D3), TFT_DC(D2),
    // tftObj(TFT_CS, TFT_DC, TFT_RST) //type: ignore
{
    tftPtr = tft_pointer;
}

/*
    ST7735: dimensions *after rotation*
    width:  160
    height: 128

*/

/// @brief draw Text onto tft screen
/// @param text String() text
/// @param cursorX x position
/// @param cursorY y position
/// @param colour colour of text: BGR565
/// @param textSize size of text
/// @param useBG whether to print background colour
void tftInclinometer_ST7735::drawText(
    String text,
    uint16_t cursorX,
    uint16_t cursorY,
    uint16_t colour,
    uint8_t textSize,
    bool useBG
) {
    if(useBG) tftPtr->setTextColor(colour, bgColour);
    else tftPtr->setTextColor(colour);
    tftPtr->setTextSize(textSize);
    tftPtr->setCursor(cursorX, cursorY);
    tftPtr->print(text);
}

void tftInclinometer_ST7735::drawAccel()
{
    static float prevAccel[3] = {0, 0, 0};
    String accelText;
    accelText = "G_x: "+String(prevAccel[0],2)+"G\n" + " F_x: "+String(9.82*prevAccel[0],2)+"m/s^2";
    drawText(accelText, 6, 14);
    prevAccel[0]=accel[0];
    accelText = "G_x: "+String(prevAccel[0],2)+"G\n" + " F_x: "+String(9.82*prevAccel[0],2)+"m/s^2";
    drawText(accelText, 6, 14, ST7735_GREEN);

    accelText = "G_y: "+String(prevAccel[1],2)+"G\n" + " F_y: "+String(9.82*prevAccel[1],2)+"m/s^2";
    drawText(accelText, 6, 54);
    prevAccel[1]=accel[1];
    accelText = "G_y: "+String(prevAccel[1],2)+"G\n" + " F_y: "+String(9.82*prevAccel[1],2)+"m/s^2";
    drawText(accelText, 6, 54, ST7735_GREEN);

    accelText = "G_z: "+String(prevAccel[2],2)+"G\n" + " F_z: "+String(9.82*prevAccel[2],2)+"m/s^2";
    drawText(accelText, 6, 94);
    prevAccel[2]=accel[2];
    accelText = "G_z: "+String(prevAccel[2],2)+"G\n" + " F_z: "+String(9.82*prevAccel[2],2)+"m/s^2";
    drawText(accelText, 6, 94, ST7735_GREEN);
}
void tftInclinometer_ST7735::drawRoll()
{
    //center: 120:45    radius: 25
    int radius=25;
    static float prevRoll = 0;
    String rollText = "Roll:"+String(prevRoll,1);
    drawText(rollText, rollPos[0]-30, rollPos[1]-30);
    tftPtr->drawLine(
        rollPos[0] +radius*cos(toRadians(prevRoll+180)),
        rollPos[1]  +radius*sin(toRadians(prevRoll+180)),
        rollPos[0] +radius*cos(toRadians(prevRoll)),
        rollPos[1]  +radius*sin(toRadians(prevRoll)),
        bgColour
    );
    prevRoll = orient[0];
    tftPtr->drawLine(
        rollPos[0] +radius*cos(toRadians(prevRoll+180)),
        rollPos[1]  +radius*sin(toRadians(prevRoll+180)),
        rollPos[0] +radius*cos(toRadians(prevRoll)),
        rollPos[1]  +radius*sin(toRadians(prevRoll)),
        ST7735_GREEN
    );
    tftPtr->drawCircle(rollPos[0], rollPos[1], 5, ST7735_GREEN);
    
    rollText = "Roll:"+String(prevRoll,1);
    drawText(rollText, rollPos[0]-30, rollPos[1]-30, ST7735_GREEN);

}
void tftInclinometer_ST7735::drawPitch()
{
    //center: 120:109   radius: 25
    int radius=25;
    static float prevPitch = 0;
    String pitchText = "Pitch:"+String(prevPitch,1);
    drawText(pitchText, pitchPos[0]-30, pitchPos[1]-30);

    tftPtr->drawLine(
        pitchPos[0] +radius*cos(-toRadians(prevPitch+180)),
        pitchPos[1] +radius*sin(-toRadians(prevPitch+180)),
        pitchPos[0] +radius*cos(-toRadians(prevPitch)),
        pitchPos[1] +radius*sin(-toRadians(prevPitch)),
        bgColour
    );
    prevPitch = orient[1];
    tftPtr->drawLine(
        pitchPos[0] +radius*cos(-toRadians(prevPitch+180)),
        pitchPos[1] +radius*sin(-toRadians(prevPitch+180)),
        pitchPos[0] +radius*cos(-toRadians(prevPitch)),
        pitchPos[1] +radius*sin(-toRadians(prevPitch)),
        ST7735_GREEN
    );
    tftPtr->drawCircle(pitchPos[0], pitchPos[1], 5, ST7735_GREEN);

    pitchText = "Pitch:"+String(prevPitch,1);
    drawText(pitchText, pitchPos[0]-30, pitchPos[1]-30, ST7735_GREEN);

}


float tftInclinometer_ST7735::toRadians(float degrees) { return float(degrees*M_PI)/180; }
float tftInclinometer_ST7735::toDegrees(float radians) { return float(radians*180)/M_PI; }

void tftInclinometer_ST7735::setAccel(float x, float y, float z) {
    if(accel[2]==0 && (accel[0]==0 || accel[1]==0)) return;
    accel[0] = x;
    accel[1] = y;
    accel[2] = z;
}

void tftInclinometer_ST7735::tftSetup()
{
    tftPtr->initR(INITR_BLACKTAB);
    delay(500);
    tftPtr->fillScreen(bgColour);
	tftPtr->drawFastHLine(0, 0, tftPtr->width(), ST7735_WHITE);
	tftPtr->drawFastHLine(1, 159, tftPtr->width(), ST7735_WHITE);
	tftPtr->drawFastVLine(0, 0, tftPtr->height(), ST7735_WHITE);
	tftPtr->drawFastVLine(127, 0, tftPtr->height(), ST7735_WHITE);
    delay(3000);
    tftPtr->setRotation(1);
    
}
void tftInclinometer_ST7735::solveOrients()
{
    orient[0]   = toDegrees( atan(accel[1] / sqrt(pow(accel[0],2)+pow(accel[2],2))) ); //degrees
    orient[1]   = toDegrees( atan(-1 * accel[0] / sqrt(pow(accel[1],2)+pow(accel[2],2))) ); //degrees
    if(accel[2]<0) {
        orient[1]=180-orient[1];
    }
}
void tftInclinometer_ST7735::solveOrients(float x, float y, float z) {
    setAccel(x, y, z);
    solveOrients();
}


/// @brief main function to update display with already existing acceleration values (no new values are passed)
void tftInclinometer_ST7735::update()
{
    solveOrients();
    drawAccel();
    drawRoll();
    drawPitch();
}
/// @brief update display with input array of acceleration readings (note: passing
/// array will pass by pointer and risk editing the data)
/// @param readAccel [3] float array of axial acceleration readings
void tftInclinometer_ST7735::update(float readAccel[3])
{
    setAccel(readAccel[0], readAccel[1], readAccel[2]);
    update();
}
/// @brief update display with input array of acceleration readings (note: passing
/// array will pass by pointer and risk editing the data)
/// @param xAccel x axis acceleration reading
/// @param yAccel y axis acceleration reading
/// @param zAccel z axis acceleration reading
void tftInclinometer_ST7735::update(float xAccel, float yAccel, float zAccel)
{
    setAccel(xAccel, yAccel, zAccel);
    update();
}