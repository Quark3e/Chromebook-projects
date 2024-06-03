//negative
int clockPin2 = 12; //IC Pin 11, Yellow Jumper
int dataPin2 = 11; //IC Pin 14 (and 13), Blue Jumper
int latchPin2 = 10; //IC Pin 12, Green Jumper

//positive
int clockPin = 7;
int dataPin = 6;
int latchPin = 5;

int mainDelay = 0;
int frameDelay = 1;

bool useIntermediary = false;
int interDelay = 1;

int frame = 0;
int count = 0;
int countLim = 50;

byte x0[] {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};

byte x1[] {
  B00000000,
  B00011000,
  B00100100,
  B01000010,
  B01000010,
  B00100100,
  B00011000,
  B00000000
};

byte x2[] {
  B00000000,
  B01000010,
  B00100100,
  B00011000,
  B00011000,
  B00100100,
  B01000010,
  B00000000
};

byte pattern1[5][8] {
  {
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000,
  },{
    B00000000,
    B00000000,
    B00111100,
    B00111100,
    B00111100,
    B00111100,
    B00000000,
    B00000000,
  },{
    B00000000,
    B01111110,
    B01111110,
    B01111110,
    B01111110,
    B01111110,
    B01111110,
    B00000000,
  },{
    B00000000,
    B01111110,
    B01111110,
    B01100110,
    B01100110,
    B01111110,
    B01111110,
    B00000000,
  },{
    B00000000,
    B01111110,
    B01000010,
    B01000010,
    B01000010,
    B01000010,
    B01111110,
    B00000000,
  }
};

byte alphabes[26][8] {
  {
    B00000000,
    B00011000,
    B00111100,
    B00100100,
    B01100110,
    B01111110,
    B01000010,
    B00000000
  }, {
    B00000000,
    B01110000,
    B01001000,
    B01110000,
    B01001000,
    B01000100,
    B01111000,
    B00000000
  }, {
    B00000000,
    B00111100,
    B01100110,
    B01000000,
    B01000000,
    B01100110,
    B00111100,
    B00000000
  }, {
    B00000000,
    B01111000,
    B01001100,
    B01000010,
    B01000010,
    B01001100,
    B01111000,
    B00000000
  }, {
    B00000000,
    B01111110,
    B01000000,
    B01000000,
    B01111000,
    B01000000,
    B01111110,
    B00000000
  }, {
    B00000000,
    B01111100,
    B01000000,
    B01000000,
    B01110000,
    B01000000,
    B01000000,
    B00000000
  }, {
    B00000000,
    B00111100,
    B01100010,
    B01000000,
    B01001110,
    B01100010,
    B00111110,
    B00000000
  }, {
    B00000000,
    B01100110,
    B01100110,
    B01111110,
    B01111110,
    B01100110,
    B01100110,
    B00000000
  }, {
    B00000000,
    B00111100,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00111100,
    B00000000
  }, {
    B00000000,
    B01111100,
    B00000100,
    B00000100,
    B00000100,
    B01100100,
    B00111000,
    B00000000
  }, {
    B00000000,
    B00100100,
    B00101000,
    B00110000,
    B00101000,
    B00100100,
    B00100010,
    B00000000
  }, {
    B00000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B01000000,
    B01111100,
    B00000000
  }, {
    B00000000,
    B01000010,
    B01100110,
    B01011010,
    B01000010,
    B01000010,
    B01000010,
    B00000000
  }, {
    B00000000,
    B01000010,
    B01100010,
    B01010010,
    B01001010,
    B01000110,
    B01000010,
    B00000000
  }, {
    B00000000,
    B00111100,
    B01000010,
    B01000010,
    B01000010,
    B01000010,
    B00111100,
    B00000000
  }, {
    B00000000,
    B01111000,
    B01000100,
    B01111000,
    B01000000,
    B01000000,
    B01000000,
    B00000000
  }, {
    B00000000,
    B00011000,
    B00100100,
    B01000010,
    B01001010,
    B00101100,
    B00011010,
    B00000000
  }, {
    B00000000,
    B01111000,
    B01000100,
    B01111000,
    B01010000,
    B01001000,
    B01000100,
    B00000000
  }, {
    B00000000,
    B00111100,
    B01000110,
    B01100000,
    B00011100,
    B01000110,
    B00111100,
    B00000000
  }, {
    B00000000,
    B01111110,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00000000
  }, {
    B00000000,
    B01000010,
    B01000010,
    B01000010,
    B01000010,
    B01000010,
    B00111100,
    B00000000
  }, {
    B00000000,
    B01000010,
    B01000010,
    B01100110,
    B00100100,
    B00111100,
    B00011000,
    B00000000
  }, {
    B00000000,
    B01001010,
    B01001010,
    B01001010,
    B01001010,
    B01001010,
    B00110100,
    B00000000
  }, {
    B00000000,
    B01000010,
    B00100100,
    B00011000,
    B00011000,
    B00100100,
    B01000010,
    B00000000
  }, {
    B00000000,
    B00100010,
    B00010100,
    B00001000,
    B00001000,
    B00001000,
    B00001000,
    B00000000,
  }, {
    B00000000,
    B01111110,
    B00000100,
    B00001000,
    B00010000,
    B00100000,
    B01111110,
    B00000000
  }
};

byte numbers[10][8] {
  {
    B00000000,
    B00011100,
    B00100010,
    B00100010,
    B00100010,
    B00100010,
    B00011100,
    B00000000
  }, {
    B00000000,
    B00011000,
    B00101000,
    B00001000,
    B00001000,
    B00001000,
    B00111110,
    B00000000
  }, {
    B00000000,
    B00011100,
    B00100010,
    B00000010,
    B00011100,
    B00100000,
    B00111110,
    B00000000
  }, {
    B00000000,
    B00011100,
    B00100010,
    B00001100,
    B00000010,
    B00100010,
    B00011100,
    B00000000
  }, {
    B00000000,
    B00001100,
    B00010100,
    B00100100,
    B00111100,
    B00000100,
    B00000100,
    B00000000
  }, {
    B00000000,
    B00111110,
    B00100000,
    B00111100,
    B00000110,
    B00100110,
    B00011100,
    B00000000
  }, {
    B00000000,
    B00011100,
    B00100010,
    B00111000,
    B00100110,
    B00100110,
    B00011100,
    B00000000
  }, {
    B00000000,
    B00111110,
    B00100010,
    B00000100,
    B00001000,
    B00001000,
    B00001000,
    B00000000
  }, {
    B00000000,
    B00011100,
    B00100010,
    B00011100,
    B00100010,
    B00100010,
    B00011100,
    B00000000
  }, {
    B00000000,
    B00011100,
    B00100010,
    B00110010,
    B00001110,
    B00100010,
    B00011100,
    B00000000
  }
};

int index = 0;

void setup() {
  Serial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(latchPin2, OUTPUT);
  pinMode(clockPin2, OUTPUT);
  pinMode(dataPin2, OUTPUT);
}

byte negativeAxis = B00000000;
byte row = B00000000;


void loop() {
  negativeAxis = B11111111;

  row = numbers[count][index];

  if (useIntermediary) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, B00000000);
    digitalWrite(latchPin, HIGH);
    delay(interDelay);
  }

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, row);
  digitalWrite(latchPin, HIGH);


  if (row > 0) negativeAxis = (~(1 << index)) - 129;
  else negativeAxis = B11111110;

  digitalWrite(latchPin2, LOW);
  shiftOut(dataPin2, clockPin2, MSBFIRST, negativeAxis);
  digitalWrite(latchPin2, HIGH);

  delay(frameDelay);
  index++;
  if (index >= 8) {
    index = 0;

    delay(mainDelay);
    //>0.008s between each frame
    frame++;
    if (frame > countLim) {
      count++;
      if (count >= 10) count = 0;
      frame = 0;
    }
  }
}