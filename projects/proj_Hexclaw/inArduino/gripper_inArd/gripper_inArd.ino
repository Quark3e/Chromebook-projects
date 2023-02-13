#include <Servo.h>

Servo myservo;

int tempVal = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A4, INPUT);
  pinMode(10, OUTPUT);
  myservo.attach(11);
}

void loop() {
  // put your main code here, to run repeatedly:
  float val = analogRead(A4);
  Serial.print(val * 100 / 1024);
  Serial.println("%");
  analogWrite(10, 255 * val / 1024);
  //myservo.write(round(180 * val / 1024));
  if (val<1024/2) {
    myservo.write(90+90*tempVal);
    tempVal=tempVal*-1;
    delay(1500);
  }
}
