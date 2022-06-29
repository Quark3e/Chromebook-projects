

int button = 2;
int led = 11;
float bright;


void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  
  analogWrite(led, 255);
  delay(1000);
  analogWrite(led, 0);
}

void loop() {
  // send data only when you receive data:
  if(digitalRead(button) == LOW) {
    analogWrite(led, 255);
    Serial.println("Pressed");
  }
  else {
    analogWrite(led, 0);
  }
  delay(20);
}

void blinkLED(int numberOfTimes) {
  for (int i = 0; i < numberOfTimes; i++) {
    analogWrite(led, 255);
    delay(100);
    analogWrite(led, 0);
    delay(100);
  }
}
