
#pragma once
#ifndef HPP_HEXCLAW_SERVO_CONTROL
#define HPP_HEXCLAW_SERVO_CONTROL


#include <global_variables.hpp>
#include "../IK_header.hpp"
#include <PiPCA9685/PCA9685.h>

using namespace PiPCA9685;


/**Angle offset to apply to given angles to make sure angle-origo/0 is paralllel along the arm axis*/
inline float offset_q[6] = {90, 0, 135, 90, 90, 90};

/**Angles to send to the servo motors at startup to hold a pose*/
inline float startup_q[6] = {0, 115, -90, 0, -25, 00};


/// @brief Apply real-world servo motor offsets to input angles
/// @param angles float()[6] values to add defaults onto: NOTE: original var will be modified
inline void add_defaults(float angles[6]) {
    angles[0] = offset_q[0] + angles[0];
    angles[1] = offset_q[1] + angles[1];
    angles[2] = 180 - (offset_q[2] + angles[2]);
    angles[3] = offset_q[3] + angles[3];
    angles[4] = 180 - (offset_q[4] + angles[4]);
    angles[5] = offset_q[5] + angles[5];
}

/// @brief apply correction function values for each servos error rate
/// @param angles float array of rotation values to correct
/// @return nothing. Modifies parameter variable
inline void q_corrections(float angles[6]) {
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

/// @brief Check if any angle in {angles} is exceeding servo motors range
/// @param angles angles to check
/// @param printErrors whether to print out any exceedings onto the terminal
/// @return boolean of whether any angles have exceeded
inline bool exceedCheck(float angles[6], bool printErrors=true) {
    bool exceeded = false;
    bool whichExceeded[6] = {false, false, false, false, false, false};
    string typeOfExceeded[6] = {"null", "null", "null", "null", "null", "null"};

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


/// @brief Sends rotation commands to servo motors
/// @param pcaBoard is a PCA9685 class object pointer
/// @param new_rotation 6 element float() array of new rotations
/// @param old_rotation 6 element float() array of old/previous/current rotations
/// @param servoInitialize *true* if it's the first time this function is called; *false* otherwise
/// @param mode *0*-rotation values sent directly; *1*-sent linearly over *totalTime* duration; *2*-sent according to mp1 over *totalTime* duration
/// @param totalTime time it takes for mode [1, 2] to finish
/// @param useDefault *true*-include IK-offsets in angles BEFORE writing to servo motors; *false*-do not include IK-offsets
/// @param printErrors whether to print any errors
/// @param printResult whether to print any results/checkpoints/non-essential-info
/// @return `0`- if successful; `-1` otherwise
/// @note if `useDefault` is true then it'll modify `new_rotation` accordingly to default value addition
inline int sendToServo(
    PCA9685* pcaBoard,
    float new_rotation[6],
    float old_rotation[6],
    bool servoInitialize,
    int mode = 0,
    float totalTime = 0,
    bool useDefault = true,
    bool printErrors = true,
    bool printResult = false
){/*
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
            pcaBoard->set_pwm(q, 0, round(400*(float(new_rotation[q])/180))+100);
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
                    pcaBoard->set_pwm(q, 0, round(400*(s_temp[q]/180))+100);
                }
                else if(mode==2) { 
                    val = s_temp[q] + s_diff[q]*mp1(float(count)/total_iteration);
                    pcaBoard->set_pwm(q, 0, round(400*(val/180))+100);
                }
                old_rotation[q] = val;
                if(printResult) printf("%3d ",int(round(val)));
            }
            if(printResult) printf("----------\n");
            if(totalTime>0.1) usleep(int(totalTime/total_iteration*1'000'000));
        }
    }
    if(printResult) {
        printf("received: ");
        for(int i=0; i<6; i++) {printf("%d ",int(old_rotation[i]));}
        printf("\n");
    }
    return returnCode;
}



/**
 * @brief solve/get the three axis coordinates
 * @param mode 
 * different modes/alternatives for how to get the three coordinate values
 * `0` - {cam0.x, cam0.y, cam0.cntArea*scal}
 * `1` - {cam0.x, cam0.y, cam1.y}
 * `2` - {camTri.x, camTri.solvedZ, camTri.y}
 * @return `0`- if successful; `1`- if an error occured
*/
inline int getCoordinates(
    int mode
) {
    if(mode==0) {

    }
    else if(mode==1) {

    }
    else if(mode==2) {
        inpPos[0] = static_cast<float>(prefSize[0]) - camObjPos_main[0][0];
        inpPos[1] = static_cast<float>(prefSize[1]) - camObjPos_main[1][0];
        camTri.solvePos(inpPos, solvedPos, false);
	    float solvedZ = -sin(
            toRadians(
                (
                    camTri.camRes[0][1]*0.5-(static_cast<float>(prefSize[1])-camObjPos_main[0][1])) * camTri.camCoef[0][1]
                )
        )*solvedPos[1];
        
        PP[0] = axisFilter[0]*float(round(solvedPos[0]*axisScal[0]+axisOffset[0])) + (1-axisFilter[0])*PP[0];
        PP[1] = axisFilter[1]*float(solvedZ*axisScal[1]+axisOffset[1]) + (1-axisFilter[1])*PP[1];
        PP[2] = axisFilter[2]*float(round(solvedPos[1]*axisScal[2]+axisOffset[2])) + (1-axisFilter[2])*PP[2];
        
    }

    return 0;
}



#endif
