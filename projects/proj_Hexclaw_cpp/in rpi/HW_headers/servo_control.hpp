
#pragma once
#ifndef HPP_HEXCLAW_SERVO_CONTROL
#define HPP_HEXCLAW_SERVO_CONTROL


#include "../hexclaw_global.hpp"
#include "../IK_header.h"
#include <PiPCA9685/PCA9685.h>

using namespace HW_KINEMATICS;
using namespace PiPCA9685;


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
int sendToServo(
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
int getCoordinates(
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
