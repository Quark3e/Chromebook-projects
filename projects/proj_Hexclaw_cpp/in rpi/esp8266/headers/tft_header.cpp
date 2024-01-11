

#include "tft_header.hpp"


TFT_disp::TFT_disp(): tft(TFT_CS, TFT_DC, TFT_RST) {
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