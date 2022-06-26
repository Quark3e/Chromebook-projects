
int incomingByte = 0;   // for incoming serial data
String input;
int led = 11;
float bright;
int cmds[10];

void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  pinMode(led, OUTPUT);

  analogWrite(led, 255);
  delay(1000);
  analogWrite(led, 0);
}

void loop() {
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:

    input = Serial.readStringUntil('\n');

    
    if (input.toInt() == 69) {
      blinkLED(12);
    }

    if (input.toInt() <= 10) {
      bright = 255 * (float(input.toInt())/10);
    }

    analogWrite(led, bright);
    delay(100);
  }
}

void blinkLED(int numberOfTimes) {
  for (int i = 0; i < numberOfTimes; i++) {
    analogWrite(led, 255);
    delay(100);
    analogWrite(led, 0);
    delay(100);
  }
}
