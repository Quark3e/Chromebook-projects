

#include "tft_header.hpp"


// TFT_obj::TFT_obj(): tft(TFT_CS, TFT_DC, TFT_RST) {
Adafruit_ST7735 TFT_obj::tft{TFT_CS, TFT_DC, TFT_RST};

// TFT_obj::TFT_obj(Adafruit_ST7735* tft, bool did_init=true) {
//     tftPtr = tft;
//     if(!did_init) scrSetup();


// }

void TFT_obj::scrSetup() {
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST7735_BLACK);

}


/*


_____________________________________________
|
|  Gx:              
|
|
|  Gy:
|
|
|  Gz:
|
|_____________________________________________
*/