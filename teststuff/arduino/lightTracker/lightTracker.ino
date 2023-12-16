
#include <Servo.h>
#include <math.h>

int pR[4] = {A0, A1, A2, A3};
float V_pR[4] = {0, 0, 0, 0};
float filt[4] = {0.2, 0.2, 0.2, 0.2};


int V_src = 5;

void getVolt(int idx) {
    float V_read = V_src*(analogRead(pR[idx])/float(1023));
    float diff = abs(V_read-V_pR[idx]);
    float lim[4] = {0.5, 0.2, 0.3, 0.75};
    if (diff>=lim[0]){ V_pR[idx]+=(1.2)*(V_read-V_pR[idx]); }
    else if (diff>=lim[1]) { V_pR[idx]=V_read; }
    else if (diff>=lim[2]) {
        V_pR[idx] = ((1.0-filt[idx])*V_pR[idx] + filt[idx]*(V_read));
    }
    // else if (diff>=lim[2]) {
    //     V_pR[idx] = 0.99*V_pR[idx] + 0.01*V_read;
    // }

    float scal=0.1;
    V_pR[idx] = float(round(V_pR[idx]/scal)*scal);
    // if(idx==1) V_pR[idx]+=2;
}


Servo servoX, servoY;
Servo myServo[2] = {servoX, servoY};
float servoPos[2] = {90, 90};

void setup() {
    Serial.begin(9600);
    myServo[0].attach(10);
    myServo[1].attach(11);

    myServo[0].write(servoPos[0]);
    myServo[1].write(servoPos[1]);
}

void loop() {
    for(int i=0; i<4; i++) {
        getVolt(i);
        Serial.print(V_pR[i]);
        Serial.print(" ");
    }
    Serial.println("");

    servoPos[0]+=(
        (V_pR[3]+V_pR[1])-(V_pR[2]+V_pR[0])
    )/2;
    servoPos[1]+=(
        (V_pR[2]+V_pR[3])-(V_pR[0]+V_pR[1])
    )/2;

    myServo[0].write(servoPos[0]);
    myServo[1].write(servoPos[1]);
}
