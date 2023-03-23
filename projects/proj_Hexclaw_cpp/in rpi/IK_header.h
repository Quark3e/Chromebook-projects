
#pragma once
#include <unistd.h>
#include <PiPCA9685/PCA9685.h>
#include <iostream>
#include <string>
#include <math.h>

using namespace std;
using namespace PiPCA9685;


float link[6] = {145, 130, 75, 50, 25, 25};
float sLoadWeight[6] = {0, 0.130, 0.085, 0, 0, 0}; //kg
float offset_q[6] = {90, 0, 135, 90, 90, 90};
float startup_q[6] = {90, 115, 135, 90, 115, 90};

void add_defaults(float angles[6]) {
    q[0] = offset_q[0] + angles[0];
    q[1] = offset_q[1] + angles[1];
    q[2] = offset_q[2] + angles[2];
    q[3] = offset_q[3] + angles[3];
    q[4] = offset_q[4] + angles[4];
    q[5] = offset_q[5] + angles[5];
}

void q_corrections(float angles[6]) {
    float error_Consts[6] = {
        0.802139037433155,
        1,
        1,
        1,
        1,
        1
    };
    angles[0] = angles[0] * error_Consts[0];
    angles[1] = angles[1] * error_Consts[1];
    angles[2] = angles[2] * error_Consts[2];
    angles[3] = angles[3] * error_Consts[3];
    angles[4] = angles[4] * error_Consts[4];
    angles[5] = angles[5] * error_Consts[5];
}

bool exceedCheck(float angles[6], bool printErrors=false) {
    bool exceeded = false;
    bool whichExceeded[6] = {false, false, false, false, false, false};
    string typeOfExceeded[6] = {"null", "null", "null", "null", "null", "null"};

    for(int i=0; i<6; i++) {
        if(angles[i]<0) {exceeded=true; whichExceeded[i]=true;typeOfExceeded[i]="under";}
        if(angles[i]>180) {exceeded=true; whichExceeded[i]=true;typeOfExceeded[i]="over";}
    }
    if(exceeded && printErrors) {
        for(int i=0; i<6; i++) { if(whichExceeded[i]) printf(" servo %d exceeded by: %s",i, typeOfExceeded[i]); }
    }
    if(exceeded) return true;
    else return false;   
}

float findVal(float arrToCheck[], int mode=0) {
    /*
    mode:
    - 0 - biggest, value
    - 1 - smallest, value
    - 2 - biggest, index
    - 3 - smallest, index
    */
    float val = arrToCheck[0];
    int index = 0;
    for(int i=0; i<sizeof(arrToCheck)/sizeof(int); i++) {
        if(mode==0 || mode==2) if(arrToCheck[i]>val) { val=arrToCheck[i]; index=i; }
        else if(mode==1 || mode==3) if(arrToCheck[i]<val) {val=arrToCheck[i]; index=i}
    }
    if(mode==0 || mode==1) return val;
    else if(mode==2 || mode==3) return index;
    else return -1
}


float mp1(float x) {
    float y=0, V_max=2, t3=1;
    float Pt1=0.5;
    float Pt2=Pt1;
    float t2=t3*Pt2;
    float t1=t3*Pt1;
    float a3 = (0-V_max)/(t3-t2);
    float a1 = (V_Max-0)/(t1-0);
    if(x>0 && x<=t1) y=(a1*pow(x,2))/2;
    else if(x>t1 && x<t2) y=a1*t1*x-(a1*pow(t1,2))/2;
    else if(x>=t2 && x<=t3) y=(a3*pow(x-t2,2))/2+V_max*x+a1*(t1*t2-pow(t1,2)/2)-V_max*t2;
    return y;
}

int sendToServo(
    PCA9685* pcaBoard,
    float new_rotation[6],
    float old_rotation[6],
    bool servoInitialize,
    float totalTime = 0,
    bool useDefault = false,
    int mode = 0,
    bool printErrors = true,
    bool printResult = true
    ){
        /*
        When the function is ran for the first time and servoInitialize is one, an empty array
        needs to be entered that will be used in *EVERY* call of sendToServo
        */
        if(useDefault) add_defaults(new_rotation);
        q_corrections(new_rotation);
        if(exceedCheck(new_rotation, true)) return;
        if(printResult) {
            printf("sent: ");
            for(int i=0; i<6; i++) { printf("%f", new_rotation[i]); }
        }

        int total_iteration = 135;

        if(mode==1) {
            for(int q=0; q<6; q++) {
                (*pcaBoard).set_pwm(q, 0, round(400*(float(new_rotation[q])/180))+100);
            }
        }
        if(mode==2 || mode==3) {
            float s_diff[6];
            float s_temp[6];
            if(!serovInitialize) {
                for(int i=0; i<6; i++) {
                    s_diff[i] = new_rotation[i]-old_rotation[i];
                    s_temp[i] = old_rotation[i];
                }
            }
            total_iteration = int(findVal(s_diff,1));
            for(int count=0; count<total_iteration-1; count++) {
                float val;
                for(int q=0; q<6; q++) {
                    if(mode==1) {
                        val = s_diff[q]/total_iteration;
                        if(val<180 && val>0) s_temp[q]+=val;
                        else if(printErrors) printf("servo %d exceeded:%f", q, val);
                        (*pcaBoard).set_pwm(q, 0, round(400*(s_temp[q]/180))+100);
                    }
                    else if(mode==2) {
                        val = s_temp[q] + s_diff[q]*mp1(float(count)/total_iteration);
                        if(val<180 && val>0) (*pcaBoard).set_pwm(q, 0, round(400*(val/180))+100);
                        else if(printErrors) printf("servo %d exceeded:%f", q, val);
                    }
                }
                if(total_time>0.1) usleep(int(total_time/total_iteration)*1'000'000);
            }
        }
   
        return 0;
    }



