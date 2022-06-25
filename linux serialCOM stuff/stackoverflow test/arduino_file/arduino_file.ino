
char incomingByte = 0;   // for incoming serial data
int led = 11;
int bright;

void setup()
{
    Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
    pinMode(led, OUTPUT);
}

void loop() {
  // send data only when you receive data:
    if (Serial.available() > 0) {
      // read the incoming byte:
        incomingByte = Serial.read();

        switch (incomingByte) {
        case '1':
            bright = 10;
            break;
        case '2':
            bright = 50;
            break;
        case '3':
            bright = 255;
            break;
        default:
            bright = 0;
            break;
        }
      analogWrite(led, bright);
      Serial.println(incomingByte);
    }
}
