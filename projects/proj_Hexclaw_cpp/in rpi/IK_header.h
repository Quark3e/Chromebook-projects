
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
float default_q[6] = {90, 0, 135, 90, 90, 90};

void add_defaults(float angles[6]) {
    q[0] = default_q[0] + angles[0];
    q[1] = default_q[1] + angles[1];
    q[2] = default_q[2] + angles[2];
    q[3] = default_q[3] + angles[3];
    q[4] = default_q[4] + angles[4];
    q[5] = default_q[5] + angles[5];
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
    if(printErrors) {
        for(int i=0; i<6; i++) { if(whichExceeded[i]) printf(" servo %d exceeded by: %s",i, typeOfExceeded[i]); }
    }
    if(exceeded) return true;
    else return false;   
}

int SendToServo(
    PCA9685* pcaBoard,
    float new_rotation[6],
    float totalTime = 0,
    bool useDefault = false,
    int mode = 0,
    bool printErrors = true,
    bool printResult = true
    ){
        /*
        */
        if(useDefault) add_defaults(new_rotation);
        q_corrections(new_rotation);
        
   
        return 0;
    }
