
#include <Servo.h>


int s1 = 10;
int s2 = 11;

Servo myservo1;
Servo myservo2;

void setup() {
    Serial.begin(9600);
    myservo1.attach(s1);
    myservo2.attach(s2);
}

void loop() {
    for(int i=0; i<=180; i+=1) {
        myservo1.write(i);
        for(int n=0; n<=180; n+=1) myservo2.write(180-n);
        for(int n=180; n>=0; n-=1) myservo2.write(180-n);
        delay(20);
    }
    for(int i=180; i>=0; i-=1) {
        myservo1.write(i);
        for(int n=0; n<=180; n+=1) myservo2.write(180-n);
        for(int n=180; n>=0; n-=1) myservo2.write(180-n);
        delay(20);
    }
}