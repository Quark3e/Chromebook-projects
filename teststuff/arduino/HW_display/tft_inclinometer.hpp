#pragma once
#ifndef H_TFT_INCLINOMETER
#define H_TFT_INCLINOMETER


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


#endif