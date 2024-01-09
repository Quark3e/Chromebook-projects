
#pragma once
#ifndef H_CPP_TFTDISP
#define H_CPP_TFTDISP


// TFT display libraries
#include <bcm2835.h> //type: ignore
#include "ST7735_TFT.h"
#include "../../../../teststuff/electronics/tft_display/ST7735_TFT_RPI_test/ST7735_TFT_RPI-1.5/example/include/Bi_Color_Bitmap.h" // Data for test 11 and 12.


// tft display setup
ST7735_TFT myTFT;

int8_t tft_setup(void) {

	cout << "TFT Start!" << endl;
	if(!bcm2835_init())
	{
		cout << "Error : Problem with init bcm2835 library" << endl;
		return -1;
	}
	
// ** USER OPTION 1 GPIO/SPI TYPE HW OR SW **
	int8_t RST_TFT = 24;
	int8_t DC_TFT = 25;
	int8_t SCLK_TFT = -1; // 5, change to GPIO no for sw spi, -1 hw spi
	int8_t SDIN_TFT = -1; // 6, change to GPIO no for sw spi, -1 hw spi
	int8_t CS_TFT = -1 ;  // 8, change to GPIO no for sw spi, -1 hw spi
	myTFT.TFTSetupGPIO(RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);


// ** USER OPTION 2 Screen Setup **
	uint8_t OFFSET_COL = 0;  // 2, These offsets can be adjusted for any issues->
	uint8_t OFFSET_ROW = 0; // 3, with manufacture tolerance/defects
	uint16_t TFT_WIDTH = 128;// Screen width in pixels
	uint16_t TFT_HEIGHT = 160; // Screen height in pixels
	myTFT.TFTInitScreenSize(OFFSET_COL, OFFSET_ROW , TFT_WIDTH , TFT_HEIGHT);


// ** USER OPTION 3 PCB_TYPE + SPI baud rate + SPI_CE_PIN**
	uint32_t SCLK_FREQ =  8000000 ; // HW Spi freq in Hertz , MAX 125 Mhz MIN 30Khz
	uint8_t SPI_CE_PIN = 0; // which HW SPI chip enable pin to use,  0 or 1
	// pass enum to param1 ,4 choices,see README
	if(!myTFT.TFTInitPCBType(TFT_ST7735R_Red, SCLK_FREQ, SPI_CE_PIN))return -1;
	// Note : if using SW SPI you do not have to pass anything for param 2&3, it will do nothing. 

	myTFT.TFTsetRotation(TFT_Degress_90);
	return 0;
}


void matToTFT(cv::Mat threshImg) {
	cv::Size imgSize = threshImg.size();
	cv::resize(threshImg, threshImg, cv::Size(), 128*(imgSize.width/imgSize.height), 128);
	cv::Mat cropImg = threshImg(cv::Rect(160, 128, 128*(imgSize.width/imgSize.height), 128));
	
	size_t pixelSize = 3;
	uint8_t* bmpBuffer = NULL;
	bmpBuffer = (uint8_t*)malloc((160 * 128) * pixelSize);
	if (bmpBuffer == NULL) {
		std::cout << "Error Test14 : MALLOC could not assign memory " << std::endl;
		return;
	}

	//cv::cvtColor(cropImg, bmpBuffer, cv::COLOR_BGR5652RGB);
	//myTFT.TFTdrawBitmap24(0, 0, bmpBuffer, 160, 128);
}





#endif