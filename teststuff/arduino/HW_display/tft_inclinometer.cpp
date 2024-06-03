
#include <tft_inclinometer.hpp>

// Adafruit_ST7735 tftInclinometer_ST7735::tft{TFT_CS, TFT_DC, TFT_RST};


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