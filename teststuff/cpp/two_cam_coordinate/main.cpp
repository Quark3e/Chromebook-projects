
#include <iostream>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

// opencv/image tracking
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>

#include "../useful/useful.hpp"
#include "two_cam_coordinate.hpp"
#include "../basic/Performance/getPerformance.hpp"
#include "../../../projects/proj_Hexclaw_cpp/in rpi/HW_headers/IR_camTrack.hpp"

using namespace std;



int prefSize[2] = {640, 480};


bool useAutoBrightne = true;
bool displayToWindow = true;
bool takePerformance = false;



int main(int argc, char* argv[]) {

    // IR_camTracking camObj(0);
    IR_camTracking camObj[2] {
        {2, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance},
        {0, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance},
    };

    float camPosition[2][2] = {{0, 0}, {25, 0}};
    float camAng_offs[2] = {90, 123};
    float inpPos[2];
    camTriangle camTri(camPosition, camAng_offs);

    float solvedPos[2];


    while(true) {
        //  t1
        if(camObj[0].processCam()==-1) return 0;
        if(camObj[1].processCam()==-1) return 0;
        //  71ms

        //  t1
        inpPos[0] = camObj[0].totCnt_pos[0];
        inpPos[1] = camObj[1].totCnt_pos[0];
        camTri.solvePos(inpPos, solvedPos, false);
        //  0.030ms

        printf(
            "[%6.2f, %6.2f] | [%7.2f, %7.2f]",
            solvedPos[0], solvedPos[1],
            inpPos[0], inpPos[1]
        );

        if(displayToWindow) {
            //  t1
            cv::Mat winImg;
            cv::hconcat(camObj[0].imgFlipped, camObj[1].imgFlipped, winImg);
            //  ~10ms

            //  t1
            int keyInp = cv::waitKey(10);
            cv::imshow(camObj[0].win_name, winImg);
            //  <30ms

            // t1
            if(keyInp==27) return 0;
            else if(keyInp==32) break;
            //  0.01ms
        }
        
        // TOTAL: <110ms with displayToWindow==true
        // if(takePerformance) perfObj.update_totalInfo(true, true, true, ' ','\r');
        printf("\n");
    }

    return 0;
}