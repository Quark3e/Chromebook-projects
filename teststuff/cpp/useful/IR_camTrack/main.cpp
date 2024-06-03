
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstring>
#include <unistd.h>
#include "../../IR_camTrack.hpp"
#include "../../../../../../teststuff/cpp/useful/useful.hpp"
#include "../../../../../../teststuff/cpp/two_cam_coordinate/two_cam_coordinate.hpp"

int prefSize[2] = {640, 480};
float PP[3] = {0, 0, 0};

float inpPos[2] = {0, 0}, solvedPos[2] = {0, 0};
float solvedY = 0;


bool displayToWindow=true;
bool useAutoBrightne=false;
bool takePerformance=false;


int main(int argc, char** argv) {

    float camPosition[2][2] = {{0, 0}, {250, 0}};
    float camAng_offs[2] = {90, 123};

    std::vector<IR_camTracking> camObj;
    camTriangle camTri(camPosition, camAng_offs);

    prefSize[0] = 640;
    prefSize[1] = 480;
    
    camObj.push_back(IR_camTracking(2, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance));
    camObj.push_back(IR_camTracking(0, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance));

    camObj[0].setup_window();
    camObj[1].setup_window();

    while(true) {
        int processResult[2];
        processResult[0] = camObj[0].processCam();
        processResult[1] = camObj[1].processCam();
        if(processResult[0]==-1 || processResult[1]==-1) {
            cout << "ERROR: camObj returned -1: processCam results: {"<<processResult[0]<<", "<<processResult[1]<<"}\n";
            return -1;
        }

        if(camObj[0].allCnt_pos.size()>0 && camObj[1].allCnt_pos.size()>0) {
            // cv::Size camSize = camObj[0].imgFlipped.size();

            inpPos[1] = float(camObj[1].prefSize[0]) - camObj[1].totCnt_pos[0];
            inpPos[0] = float(camObj[0].prefSize[0]) - camObj[0].totCnt_pos[0];

            camTri.solvePos(inpPos, solvedPos, false);

            solvedY = sin(toRadians((camTri.camRes[0][1]*0.5-camObj[0].totCnt_pos[1])*camTri.camCoef[0][1]))*solvedPos[1];

            PP[0] = solvedPos[0];
            PP[1] = solvedY;
            PP[2] = solvedPos[1];

            printf(
                "%5.0f:{%6.1f, %6.1f} %5.0f{%6.1f, %6.1f} | {%6.1f, %6.1f, %6.1f}\n",
                camObj[0].totCnt_area,
                camObj[0].totCnt_pos[0],
                camObj[0].totCnt_pos[1],
                camObj[1].totCnt_area,
                camObj[1].totCnt_pos[0],
                camObj[1].totCnt_pos[1],
                PP[0],
                PP[1],
                PP[2]
            );
        }

        int resultKey = cv::waitKey(10);
        if(resultKey==27) break;
        usleep(1'000);
    }
    camObj[0].close();
    camObj[1].close();
}