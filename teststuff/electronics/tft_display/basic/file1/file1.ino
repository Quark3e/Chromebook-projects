/*
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
 */

// include TFT and SPI libraries
#include <TFT.h>  
#include <SPI.h>

// pin definition for Arduino UNO
#define cs   10
#define dc   9
#define rst  7


// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

void setup() {

  //initialize the library
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  //set the text size
  TFTscreen.setTextSize(2);
}

int count = 0;

void loop() {
  //generate a random color
  int redRandom = random(0, 255);
  int greenRandom = random (0, 255);
  int blueRandom = random (0, 255);
  
  // set a random font color
  TFTscreen.stroke(redRandom, greenRandom, blueRandom);
  
  // print Hello, World! in the middle of the screen
  TFTscreen.fillRect(0, 0, 160, 128, ST7735_BLACK);
  TFTscreen.text("Hello, World!", 6, count);
  
  // wait 200 miliseconds until change to next color
  delay(20);
  count+=2;
  if(count>=128) count=0;
}
