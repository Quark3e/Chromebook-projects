
#include <servo_control.hpp>


servo_angles_6DOF offset_q{90, 0, 135, 90, 90, 90};
// float offset_q[6] = {90, 0, 135, 90, 90, 90};

servo_angles_6DOF startup_q{0, 115, -90, 0, -25, 0};
// float startup_q[6] = {0, 115, -90, 0, -25, 00};


std::vector<size_t> pcaBoard_motorIndices{
    0, 1, 2, 3, 4, 5
};

// void add_defaults(float angles[6]) {
void add_defaults(servo_angles_6DOF& angles) {
    angles[0] = offset_q[0] + angles[0];
    angles[1] = offset_q[1] + angles[1];
    angles[2] = 180 - (offset_q[2] + angles[2]);
    angles[3] = offset_q[3] + angles[3];
    angles[4] = 180 - (offset_q[4] + angles[4]);
    angles[5] = offset_q[5] + angles[5];
}

// void q_corrections(float angles[6]) {
void q_corrections(servo_angles_6DOF& angles) {
    float error_Consts[6] = {
        0.802139037433155,
        0.7538,
        0.8772,
        1.0345,
        1,
        1
    };
    angles[0] = angles[0] * error_Consts[0];
    // angles[1] = angles[1] * error_Consts[1];
    angles[1] =
        1.751 * pow(10, -9) * pow(angles[1], 5)
        -7.693 * pow(10, -7) * pow(angles[1], 4)
        +0.000117 * pow(angles[1], 3)
        -0.006447 * pow(angles[1], 2)
        +0.71 * angles[1]
        +25.66;
    angles[2] = 
        4.605 * pow(10, -9) * pow(angles[2], 5)
        -2.178 * pow(10, -6) * pow(angles[2], 4)
        +0.0003815 * pow(angles[2],3)
        -0.02938 * pow(angles[2],2)
        +1.776 * angles[2]
        +4.114;
    angles[3] = angles[3] * error_Consts[3];
    angles[4] = angles[4] * error_Consts[4];
    angles[5] = angles[5] * error_Consts[5];
}

// bool exceedCheck(float angles[6], bool printErrors) {
bool exceedCheck(servo_angles_6DOF& angles, bool printErrors) {
    bool exceeded = false;
    bool whichExceeded[6] = {false, false, false, false, false, false};
    std::vector<std::string> typeOfExceeded{"null", "null", "null", "null", "null", "null"};

    for(int i=0; i<5; i++) {
        if(int(angles[i])<0) {
            exceeded=true;
            whichExceeded[i]=true;
            typeOfExceeded[i]="under";
            angles[i]=0; 
        }
        if(int(angles[i])>180) {
            exceeded=true;
            whichExceeded[i]=true;
            typeOfExceeded[i]="over";
            angles[i]=180;
        }
    }
    if(exceeded && printErrors) {
        printf("\n");
        for(int i=0; i<6; i++) { if(whichExceeded[i]) printf("\t-servo q[%d] exceeded: %s\n",i, typeOfExceeded[i].c_str()); }
    }
    // printf("\n");
    if(exceeded) return true;
    else return false;   
}


int sendToServo(
    PCA9685* pcaBoard,
    servo_angles_6DOF& new_rotation,
    servo_angles_6DOF& old_rotation,
    bool servoInitialize,
    int mode,
    float totalTime,
    bool useDefault,
    bool printErrors,
    bool printResult
) {
    /*
    When the function is ran for the first time and servoInitialize is one, an empty array
    needs to be entered that will be used in *EVERY* call of sendToServo
    */

    int returnCode = 0;
    
    if(printResult) { printf("\tNEW SENDTOSERVO()\n"); }
    if(servoInitialize) {
        for(int i=0; i<6; i++) {new_rotation[i] = startup_q[i];}
    }

    if(useDefault) add_defaults(new_rotation);
    if(printResult && useDefault) {
        printf(
            "angles with offset added: %d %d %d %d %d %d\n",
            int(new_rotation[0]),
            int(new_rotation[1]),
            int(new_rotation[2]),
            int(new_rotation[3]),
            int(new_rotation[4]),
            int(new_rotation[5])
            );
    }

    q_corrections(new_rotation);

    if(exceedCheck(new_rotation, printErrors)) {}//return -1;
    if(printResult) {
        printf("mode: %d  totalTime[sec]:%.2f\n", mode, totalTime);
        printf(" new: ");
        for(int i=0; i<6; i++) { printf("%.2f ", new_rotation[i]); }
        printf("\n old: ");
        for(int i=0; i<6; i++) { printf("%.2f ", old_rotation[i]); }
        printf("\n");
    }

    int total_iteration = 135;
    if(mode==0) {
        for(int q=0; q<6; q++) {
            pcaBoard->set_pwm(pcaBoard_motorIndices[q], 0, round(400*(float(new_rotation[q])/180))+100);
            old_rotation[q] = new_rotation[q];
        }
    }
    if(mode==1 || mode==2) {
        float s_diff[6];
        float s_temp[6];
        for(int i=0; i<6; i++) {
            if(!servoInitialize) {
                if(printResult) printf(" - new:%4d   old:%4d   ", int(new_rotation[i]), int(old_rotation[i]));
                s_diff[i] = new_rotation[i] - old_rotation[i];
                s_temp[i] = old_rotation[i];
            }
            else if(servoInitialize) {
                old_rotation[i] = startup_q[i];
                s_diff[i] = new_rotation[i]-old_rotation[i];
                s_temp[i] = old_rotation[i];
            }
            if(printResult) { printf("s_diff[%d]: %6.2f\n",i, s_diff[i]); }
        }
        total_iteration = int(findVal(s_diff, sizeof(s_diff)/sizeof(int),0));
        if(printResult) printf("total_iteration:%d\n",total_iteration);
        for(int count=0; count<total_iteration-1; count++) {
            float val;
            for(int q=0; q<6; q++) {
                if(mode==1) {
                    val = s_diff[q]/total_iteration;
                    s_temp[q]+=val;
                    pcaBoard->set_pwm(pcaBoard_motorIndices[q], 0, round(400*(s_temp[q]/180))+100);
                }
                else if(mode==2) { 
                    val = s_temp[q] + s_diff[q]*mp1(float(count)/total_iteration);
                    pcaBoard->set_pwm(pcaBoard_motorIndices[q], 0, round(400*(val/180))+100);
                }
                old_rotation[q] = val;
                if(printResult) printf("%3d ",int(round(val)));
            }
            if(printResult) printf("----------\n");
            if(totalTime>0.1) std::this_thread::sleep_for(std::chrono::milliseconds(int(totalTime/total_iteration*1'000)));
        }
    }
    if(printResult) {
        printf("received: ");
        for(int i=0; i<6; i++) {printf("%d ",int(old_rotation[i]));}
        printf("\n");
    }
    return returnCode;
}



int getCoordinates(
    pos2d<float> cam0_pos,
    pos2d<float> cam1_pos,
    int mode,
    pos2d<float> camDim
) {
    if(mode==0) {

    }
    else if(mode==1) {

    }
    else if(mode==2) {
        camTri.camRes[0] = prefSize.cast<double>();
        camTri.camRes[1] = prefSize.cast<double>();

        camTri.solvedPos_filter = {axisFilter[0], axisFilter[1], axisFilter[2]};
        camTri.setcamScalars();

        static std::vector<pos2d<double>> camTri_inputPos(2, pos2d<double>(0, 0));
        camTri_inputPos[0] = cam0_pos.cast<double>();
        camTri_inputPos[1] = cam1_pos.cast<double>();
        
        camTri.solvePos(camTri_inputPos, false);
        
        float newPP[3] = {
            axisScal[0]*(camTri.solvedPos[0])+axisOffset[0],
            axisScal[1]*(camTri.solvedPos[2])+axisOffset[1],
            axisScal[2]*(camTri.solvedPos[1])+axisOffset[2]
        };

        PP[0] = newPP[0];
        PP[1] = newPP[1];
        PP[2] = newPP[2];

        // PP[0] = axisFilter[0]*newPP[0] + (1.0-axisFilter[0])*PP[0];
        // PP[1] = axisFilter[1]*newPP[1] + (1.0-axisFilter[1])*PP[1];
        // PP[2] = axisFilter[2]*newPP[2] + (1.0-axisFilter[2])*PP[2];

        // PP[0] = axisFilter[0]*float(round(solvedPos[0]*axisScal[0]+axisOffset[0])) + (1-axisFilter[0])*PP[0];
        // PP[1] = axisFilter[1]*float(solvedZ*axisScal[1]+axisOffset[1]) + (1-axisFilter[1])*PP[1];
        // PP[2] = axisFilter[2]*float(round(solvedPos[1]*axisScal[2]+axisOffset[2])) + (1-axisFilter[2])*PP[2];
        
    }

    return 0;
}
