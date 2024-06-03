int clockPin = 12; //IC Pin 11
int dataPin = 11; //IC Pin 14
int latchPin = 8; //IC Pin 12

int index = 0;
int count = 1;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, B10101010);
  digitalWrite(latchPin, HIGH);
  delay(1000);
  index++;
  if (index >= count){
    index = 0;
  }
}
