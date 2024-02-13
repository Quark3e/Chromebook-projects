
#include <iostream>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <cmath>

#include "../../../../../teststuff/cpp/useful/useful.hpp"
#include "../../../../../projects/proj_Hexclaw_cpp/in rpi/HW_headers/IR_camTrack.hpp"
#include "../../../../../teststuff/cpp/two_cam_coordinate/two_cam_coordinate.hpp"




int main(int argc, char** argv) {
    bool useCamera = true, useTwoCamClass = true;

    /*
    char array sent to stdout:

    toSend = "[-100.0,-100.0,-100.0,-100.0:-100.0,-100.0,-100.0]\0"
        [cam1_xy[13],cam2_xy[13]:solvedPos_xyz[20]]\0
    maximum length of array incl. null char included: 51
    character array indices/position: 
        - brackets: [0, 49]
        - commas:   [7, 14, 21, 35, 42]
        - colon:    [28]
    */
    char toSend[255];
    char toRecev[255];

    for(int i=0; i<50; i++) toSend[i] = '0';
    toSend[0]   = '['
    toSend[28]  = ':';
    toSend[49]  = ']';
    toSend[50]  = '\0';

    if(argc>1) {
        if(strcmp(argv[1], "0")==0) {
            /*
                NOTE: Should never be the case
            */
            useCamera = false;
            useTwoCamClass = false;
        }
        else if(strcmp(argv[1], "1")==0) {
            useCamera = true;
            useTwoCamClass = false;
        }
        else if(strcmp(argv[1], "2")==0) {
            useCamera = false;
            useTwoCamClass = true;
        }
    }
    
    if(useCamera) {
        int prefSize[2] = {640, 480};
        bool displayToWindow = true;
        bool useAutoBrightne = true;
        bool takePerformance = true;

        IR_camTracking camObj[2] {
            (2, prefSie[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance),
            (0, prefSie[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance)
        };
        
        camObj[0].setup_window();
        camObj[1].setup_window();
    }
    if(useTwoCamClass) {
        float camPosition[2][2] = {{0, 0}, {250, 0}};
        float camAng_offs[2] = {90, 123};

        camTriangle camTri(camPosition, camAng_offs);
    }

    float PP[3] = {0, 0, 0}, camPos[2][2], solvedPos[2], inpPos[2], solvedZ;
    float axisScal[3] = {1, 1, 1};	
    float axisOffset[3] = {0, 0, 0};
    float axisFilter[3] = {1, 1, 1};

    while(true) {
        /*
            [-100.0,-100.0,-100.0,-100.0]\0
            [cam0_x, cam0_y, cam1_x, cam1_y]
            len: 30
        */
        scanf("%s", &toRecev);


        for(int i=0; i<50; i++) toSend[i] = '0';
        toSend[0]   = '['
        toSend[28]  = ':';
        toSend[49]  = ']';
        toSend[50]  = '\0';

        if(useCamera) {
            if(camObj[0].processCam()==-1 || camObj[1].processCam()==-1) {
                return 0;
            }
        }

        if(useCamera && camObj[0].allCnt_pos.size()>0) {
            cv::Size camSize = camObj[0].imgFlipped.size();
            
            camPos[0][0] = camObj[0].totCnt_pos[0];
            camPos[0][1] = camObj[0].totCnt_pos[1];
            if(camObj[0].allCnt_pos.size()>0 && camObj[1].allCnt_pos.size()>0) {
                camPos[1][0] = camObj[1].totCnt_pos[0];
                camPos[1][1] = camObj[1].totCnt_pos[1];
                inpPos[0] = camPos[0][0];
                inpPos[1] = camPos[1][0];

                fillcharArray(camPos[0][0], 1, toSend, 6, 1);
                fillcharArray(camPos[0][1], 8, toSend, 6, 1);
                fillcharArray(camPos[1][0], 15, toSend, 6, 1);
                fillcharArray(camPos[1][1], 22, toSend, 6, 1);
            }
        }
        else if(!useCamera) {
            char nums[2][6];
            for(int i=0; i<6; i++) { nums[0][i] = toRecev[i+1]; }
            for(int i=0; i<6; i++) { nums[1][i] = toRecev[i+15]; }
            inpPos[0] = atof(nums[0]);
            inpPos[1] = atof(nums[1]);
        }

        if(useTwoCamClass) {
            camTri.solvePos(inpPos, solvedPos, false);
            solvedZ = -sin(toRadians((camTri.camRes[0][1]*0.5-camObj[0].totCnt_pos[1])*camTri.camCoef[0][1]))*solvedPos[1];
            PP[0] = axisFilter[0]*float(round(solvedPos[0]*axisScal[0]+axisOffset[0])) + (1-axisFilter[0])*PP[0];
            PP[1] = axisFilter[1]*float(solvedZ*axisScal[1]+axisOffset[1]) + (1-axisFilter[1])*PP[1];
            PP[2] = axisFilter[2]*float(round(solvedPos[1]*axisScal[2]+axisOffset[2])) + (1-axisFilter[2])*PP[2];

            fillcharArray(PP[0], 29, toSend, 6, 1);
            fillcharArray(PP[0], 36, toSend, 6, 1);
            fillcharArray(PP[0], 43, toSend, 6, 1);
        }

        for(int i=7; i<48; i+=7) {
            if(i==28) continue;
            toSend[i] = ',';
        }
        
        printf("%s\n", toSend);
        std::cout.flush();
    }

}