
#pragma once
#include <unistd.h>
#include <PiPCA9685/PCA9685.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace PiPCA9685;

int SendToServo(
    PCA9685* pcaBoard,
    float new_rotation[6],
    float totalTime = 0,
    bool useDefault = false,
    int mode = 0,
    
    ) {



        return 0;
    }