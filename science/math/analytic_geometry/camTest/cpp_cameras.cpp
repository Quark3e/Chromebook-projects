
#include <iostream>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <unistd.h>

#include <chrono>
#include <ctime>

#include <thread>

#include "../../../../teststuff/cpp/useful/useful.hpp"
#include "../../../../projects/proj_Hexclaw_cpp/in rpi/HW_headers/IR_camTrack.hpp"
#include "../../../../teststuff/cpp/two_cam_coordinate/two_cam_coordinate.hpp"


/*std::ios::sync_with_stdio(false);*/

// using namespace std;

#define useThreads true
#define threadDebug false

#if useThreads
    void thread_task(IR_camTracking& camRef) {
        camRef.processCam();
    }
#endif


bool logOutput = false;

int main(int argc, char** argv) {
    bool useCamera = true, useTwoCamClass = true;

    /*
    char array sent to stdout:

    // toSend[] =
    //      [  cam1_xy[13]:  cam2_xy[13]:   solvedPos_xyz[20]]\0
    //     "[-100.0,-100.0:-100.0,-100.0:-100.0,-100.0,-100.0]\0"
        
    // maximum length of array incl. null char included: 51
    // character array indices/position: 
    //     - brackets: [0, 49]
    //     - commas:   [7, 21, 35, 42]
    //     - colon:    [14, 28]

               "[  cam1_xy[13]:  cam2_xy[13]:     l_tri[2]:   ang_tri[2]:   solvedPos_xyz[20]]\0"
    toSend[] = "[-100.0,-100.0:-100.0,-100.0:-100.0,-100.0:-100.0,-100.0:-100.0,-100.0,-100.0]\0"
    */
    std::ofstream outLogFile;
    
    if(logOutput) {
        outLogFile.open("output_cpp_cameras.txt", std::ios::app);
        std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        outLogFile << "start: " << std::ctime(&currentTime);
    }
    
    char toSend[255];
    char toRecev[255];

    std::vector<IR_camTracking> camObj;
    if(logOutput) outLogFile << " -created camObj obj vector\n";
    camTriangle* camTri;
    if(logOutput) outLogFile << " -created camTri obj pointer\n";

    for(int i=0; i<78; i++) toSend[i] = '0';
    toSend[0]   = '[';
    toSend[14]  = ':';
    toSend[28]  = ':';
    toSend[42]  = ':';
    toSend[56]  = ':';
    toSend[77]  = ']';
    toSend[78]  = '\0';
    if(logOutput) outLogFile << " -first definition of toSend[] symbols\n";

    if(argc>1) {
        if(strcmp(argv[1], "0")==0) {
            /*
                NOTE: Should never be the case
            */
            std::cout << "tf is wrong with you. You passed false on both booleans and still ran the cpp program" << endl;
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
        bool useAutoBrightne = false;
        bool takePerformance = false;

        // IR_camTracking camObj[2] {

        camObj.push_back(IR_camTracking(2, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance));
        camObj.push_back(IR_camTracking(0, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance));
        if(logOutput) outLogFile << " -push_back() added both cams onto camObj\n";
        
        camObj[0].setup_window();
        if(logOutput) outLogFile << " -camObj[0].setup_window()\n";
        camObj[1].setup_window();
        if(logOutput) outLogFile << " -camObj[1].setup_window()\n";
    }
    if(useTwoCamClass) {
        float camPosition[2][2] = {{0, 0}, {25, 0}};
        float camAng_offs[2] = {90, 123};

        camTri = new camTriangle(camPosition, camAng_offs);
        if(logOutput) outLogFile << " -camTri = new camTriangle(camPosition, camAng_offs)\n";
    }

    float PP[3] = {0, 0, 0}, camPos[2][2], solvedPos[2], inpPos[2], solvedY;
    float axisScal[3] = {1, 1, 1};	
    float axisOffset[3] = {0, 0, 0};
    float axisFilter[3] = {1, 1, 1};

    while(true) {
        /*
            recev = "[-100.0,-100.0,-100.0,-100.0]\0"
            [cam0_x, cam0_y, cam1_x, cam1_y]
            len: 30
        */
        if(logOutput) outLogFile << " -while loop iteration\n";
        scanf("%s", &toRecev);
        if(logOutput) outLogFile << " -scanf();\n";

        char nums[2][6];
        if(useCamera) {
            for(int i=0; i<6; i++) { nums[0][i] = toRecev[i+1]; }
            if(atoi(nums[0]) == 6942)  {
                outLogFile << "--exit called:\n";
                break;
            }
        }
        for(int i=0; i<78; i++) toSend[i] = '0';
        toSend[0]   = '[';
        toSend[14]  = ':';
        toSend[28]  = ':';
        toSend[42]  = ':';
        toSend[56]  = ':';
        toSend[77]  = ']';
        toSend[78]  = '\0';
        if(logOutput) outLogFile << " -while loop definition of toSend[] symbols:" << useCamera << ": "<< useTwoCamClass << "\n";

        if(useCamera) {
            #if useThreads
                std::thread t_cam0(thread_task, std::ref(camObj[0]));
                std::thread t_cam1(thread_task, std::ref(camObj[1]));
                t_cam0.join();
                t_cam1.join();
            #elif !useThreads
                camObj[0].processCam();
                camObj[1].processCam();
            #endif
            if(camObj[0].processReturnCode==-1 || camObj[1].processReturnCode==-1) {
                if(logOutput) outLogFile << " -couldn't processCam: returned -1: \n";
                if(logOutput) outLogFile.close();
                return 0;
            }
            if(logOutput) outLogFile << " -useCamera: processCam\n";
        }

        if(useCamera && camObj[0].allCnt_pos.size()>0) {
            cv::Size camSize = camObj[0].imgFlipped.size();
            
            camPos[1][0] = float(camObj[0].prefSize[0]) - camObj[0].totCnt_pos[0];
            camPos[1][1] = float(camObj[0].prefSize[1]) - camObj[0].totCnt_pos[1];
            if(camObj[0].allCnt_pos.size()>0 && camObj[1].allCnt_pos.size()>0) {

                camPos[0][0] = float(camObj[1].prefSize[0]) - camObj[1].totCnt_pos[0];
                camPos[0][1] = float(camObj[1].prefSize[1]) - camObj[1].totCnt_pos[1];
                inpPos[0] = camPos[0][0];
                inpPos[1] = camPos[1][0];

                fillCharArray(camPos[0][0], 1, toSend, 6, 1);
                fillCharArray(camPos[0][1], 8, toSend, 6, 1);
                fillCharArray(camPos[1][0], 15, toSend, 6, 1);
                fillCharArray(camPos[1][1], 22, toSend, 6, 1);
                if(logOutput) outLogFile << " -useCamera: -camObj[ ].allCnt_pos.size(): -fillCharArray\n";
            }
        }
        else if(!useCamera) {
            for(int i=0; i<6; i++) { nums[0][i] = toRecev[i+1]; }
            for(int i=0; i<6; i++) { nums[1][i] = toRecev[i+15]; }
            inpPos[0] = atof(nums[0]);
            inpPos[1] = atof(nums[1]);
            if(logOutput) outLogFile << " -!useCamera: atof()\n";
        }

        if(useTwoCamClass) {
            camTri->solvePos(inpPos, solvedPos, false);
            if(logOutput) outLogFile << " -useTwoCamClass: -camTri->solvePos()\n";
            solvedY = -sin(toRadians(((*camTri).camRes[0][1]*0.5-camObj[0].totCnt_pos[1])*(*camTri).camCoef[0][1]))*solvedPos[1];
            
            PP[0] = solvedPos[0];
            PP[1] = solvedY;
            PP[2] = solvedPos[1];
            
            // PP[0] = axisFilter[0]*float(round(solvedPos[0]*axisScal[0]+axisOffset[0])) + (1-axisFilter[0])*PP[0];
            // PP[1] = axisFilter[1]*float(solvedY*axisScal[1]+axisOffset[1]) + (1-axisFilter[1])*PP[1];
            // PP[2] = axisFilter[2]*float(round(solvedPos[1]*axisScal[2]+axisOffset[2])) + (1-axisFilter[2])*PP[2];

            if(logOutput) {
                outLogFile << " -useTwoCamClass: -l_hypotenuse=" << (*camTri).l_hypotenuse << "\n"
                           << " -useTwoCamClass: -ang_p=" << (*camTri).ang_p << "\n";
            }
            fillCharArray((*camTri).l_tri[0], 29, toSend, 6, 1);
            fillCharArray((*camTri).l_tri[1], 36, toSend, 6, 1);
            if(logOutput) outLogFile << " -useTwoCamClass: -fillCharArray() (*camTri).l_tri["<<(*camTri).l_tri[0]<<", "<<(*camTri).l_tri[1] <<"]\n";

            fillCharArray((*camTri).ang_tri[0], 43, toSend, 6, 1);
            fillCharArray((*camTri).ang_tri[1], 50, toSend, 6, 1);
            if(logOutput) outLogFile << " -useTwoCamClass: -fillCharArray() (*camTri).ang_tri["<<(*camTri).ang_tri[0]<<", "<<(*camTri).ang_tri[1] <<"]\n";

            fillCharArray(PP[0], 57, toSend, 6, 1);
            fillCharArray(PP[1], 64, toSend, 6, 1);
            fillCharArray(PP[2], 71, toSend, 6, 1);
            if(logOutput) outLogFile << " -useTwoCamClass: -fillCharArray() PP[ ]\n";
        }

        for(int i=7; i<71; i+=7) {
            if(i==14 || i==28 || i==42 || i==56) continue;
            toSend[i] = ',';
        }
        if(logOutput) outLogFile << " -toSend[i] = ','; commas added\n";
        
        printf("%s\n", toSend);
        if(logOutput) outLogFile << " -final printf(\"%s\\n\", toSend);\n";
        std::cout.flush();
        if(logOutput) outLogFile << " -std::cout.flush();\n";
        if(logOutput) logOutput = false;
    }
    if(useCamera) {
        camObj[0].close();
        camObj[1].close();
    }

    if(logOutput) {
        std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        outLogFile << "end: " << ctime(&currentTime);
        outLogFile.close();
    }

}
