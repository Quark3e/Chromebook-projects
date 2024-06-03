//negative
int clockPin2 = 12; //IC Pin 11, Yellow Jumper
int dataPin2 = 11; //IC Pin 14 (and 13), Blue Jumper
int latchPin2 = 10; //IC Pin 12, Green Jumper

//positive
int clockPin = 7;
int dataPin = 6;
int latchPin = 5;

int mainDelay = 50;

bool useIntermediary = false;
int interDelay = 1;

byte patterns_t[] = {
B10101010,
B01010100,
};

byte patterns_0[] = {
B00000010,
B00000100,
B00001000,
B00010000,
B00100000,
B01000000,
B10000000,
B01000000,
B00100000,
B00010000,
B00001000,
B00000100,
};

byte patterns_1[] = {
B00000000,
B01000000,
B00100000,
B00010000,
B00001000,
B00000100,
B00000010,
};

byte patterns_2[] = {
B10000110,
B11000010,
B11100000,
B11110000,
B01111000,
B00111100,
B00011110,
B00001110
};

byte patterns_3[] = {
B11000000,
B01100000,
B00110000,
B00011000,
B00001100,
B00000110,
B00000010,
B10000000
};

byte patterns_4[] = {
B00000000,
B10000000,
B11000000,
B11100000,
B11110000,
B11111000,
B11111100,
B11111110,
B01111110,
B00111110,
B00011110,
B00001110,
B00000110,
B00000010,
};


byte patt_2d_0[] = {
B00000000
};

int index = 0;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  pinMode(latchPin2, OUTPUT);
  pinMode(clockPin2, OUTPUT);
  pinMode(dataPin2, OUTPUT);
  
  //digitalWrite(latchPin2, LOW);
  //shiftOut(dataPin2, clockPin2, MSBFIRST, B10101010);
  //digitalWrite(latchPin2, HIGH);
}

void loop() {
  int count = sizeof(patterns_4);
  
  if(useIntermediary) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, B00000000);
    digitalWrite(latchPin, HIGH);
    delay(interDelay);
  }

  digitalWrite(latchPin2, LOW);
  shiftOut(dataPin2, clockPin2, MSBFIRST, patterns_4[index]);
  digitalWrite(latchPin2, HIGH);
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, patterns_4[index]);
  digitalWrite(latchPin, HIGH);
  delay(mainDelay);
  index++;
  if (index >= count){
    index = 0;
  }
}
