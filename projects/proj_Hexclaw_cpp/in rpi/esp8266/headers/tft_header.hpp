
#pragma once
#ifndef INO_H_TFT_HEADER
#define INO_H_TFT_HEADER

#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library

#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>


// #define TFT_RST     D4
// #define TFT_CS      D3
// #define TFT_DC      D2


class TFT_obj {
    public:
    TFT_RST = D4;
    TFT_CS  = D3;
    TFT_DC  = D2;
    static Adafruit_ST7735 tft;
    // Adafruit_ST7735 tft;
    // TFT_obj(Adafruit_ST7735* tft, bool did_init);
    void scrSetup();
    
};


#endif