int clockPin = 12; //IC Pin 11, Yellow Jumper
int dataPin = 11; //IC Pin 14, Blue Jumper
int latchPin = 8; //IC Pin 12, Green Jumper

int mainDelay = 40;

bool useIntermediary = true;
int interDelay = 1;

byte patterns_0[58] = {
//B00000001, mainDelay,
//B00000010, mainDelay,
B00000100, mainDelay,
B00001000, mainDelay,
B00010000, mainDelay,
B00100000, mainDelay,
B01000000, mainDelay,
B10000000, mainDelay,
B01000000, mainDelay,
B00100000, mainDelay,
B00010000, mainDelay,
B00001000, mainDelay,
B00000100, mainDelay,
B00000010, mainDelay,
};

byte patterns_1[58] = {
B10000000, mainDelay,
B01000000, mainDelay,
B00100000, mainDelay,
B00010000, mainDelay,
B00001000, mainDelay,
B00000100, mainDelay,
B10000010, mainDelay,
};

byte patterns_2[100] = {
B10000110, mainDelay,
B11000000, mainDelay,
B11100000, mainDelay,
B11110000, mainDelay,
B11111000, mainDelay,
B11111100, mainDelay,
B11111110, mainDelay,
B11111110, mainDelay,
B01111110, mainDelay,
B00111110, mainDelay,
B00011110, mainDelay,
B00001110, mainDelay
};

byte patterns_3[58] = {
B11000000, mainDelay,
B01100000, mainDelay,
B00110000, mainDelay,
B00011000, mainDelay,
B00001100, mainDelay,
B00000110, mainDelay,
B00000010, mainDelay,
B10000000, mainDelay
};

int index = 0;
int count = sizeof(patterns_3) / 2;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  if(useIntermediary) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, B00000000);
    digitalWrite(latchPin, HIGH);
    delay(interDelay);
  }
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, patterns_3[index * 2]);
  digitalWrite(latchPin, HIGH);
  delay(patterns_3[(index * 2) + 1]);
  index++;
  if (index >= count){
    index = 0;
  }
}
