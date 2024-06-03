

int button = 2;
int led = 13;
float bright;


void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  pinMode(led, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  
  digitalWrite(led, 255);
  delay(1000);
  digitalWrite(led, 0);
}

void loop() {
  // send data only when you receive data:
  if(digitalRead(button) == LOW) {
    digitalWrite(led, HIGH);
    Serial.println("Pressed");
  }
  else {
    digitalWrite(led, LOW);
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
