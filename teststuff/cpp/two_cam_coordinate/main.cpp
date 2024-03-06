
#include <iostream>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#define useThreads  true
#define findPerf    true

#define useAutoBrightne true
#define displayToWindow false
#define takePerformance false


#if useThreads
#include <thread>
#include <mutex>
#endif

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



// IR_camTracking camObj(0);
IR_camTracking camObj[2] {
    {2, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance},
    {0, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance},
};


cv::Mat oldFlippedImg[2], newFlippedImg[2];
float camObjPos_old[2][2], camObjPos_new[2][2];
int returCode[2];

void transferCamVars(IR_camTracking* camPtr, int t_idx) {
    if(displayToWindow) {
        newFlippedImg[t_idx] = (*camPtr).imgFlipped;
    }
    camObjPos_new[t_idx][0] = (*camPtr).totCnt_pos[0];
    camObjPos_new[t_idx][1] = (*camPtr).totCnt_pos[1];
    returCode[t_idx] = (*camPtr).processReturnCode;
}
void updateCamVars(int t_idx) {
    if(displayToWindow) {
        oldFlippedImg[t_idx] = newFlippedImg[t_idx];
    }
    camObjPos_old[t_idx][0] = camObjPos_new[t_idx][0];
    camObjPos_old[t_idx][1] = camObjPos_new[t_idx][1];
}

#if useThreads
    mutex mtx[2];
    bool exit_thread[2] = {false, false};


    getPerf perfObj[3] {
        {"cam0 process"},
        {"cam1 process"},
        {"total thread"}
    };

    void thread_task(IR_camTracking* camPtr, int t_idx) {
        unique_lock<mutex> u_lck(mtx[t_idx], defer_lock);
        while(true) {
            camPtr->processCam();
            u_lck.lock();
            transferCamVars(camPtr, t_idx);
            u_lck.unlock();
            if(returCode[t_idx]) break;
        }
    }

#elif !useThreads
    getPerf perfObj[2] {
        {"cam0 process"},
        {"cam1 process"}
    };
#endif

float camPosition[2][2] = {{0, 0}, {25, 0}};
float camAng_offs[2] = {90, 123};
float inpPos[2];
camTriangle camTri(camPosition, camAng_offs);
// camTri.setFilter(0.1, 0.1);

float solvedPos[2];


int main(int argc, char* argv[]) {

    while(true) {
        //  t1
        #if findPerf 
            float delay0, delay1, totDelay;
        #endif
        #if !useThreads
            if(findPerf) perfObj[0].add_checkpoint("cam0 process_start");
            camObj[0].processCam();
            if(camObj[0].processReturnCode==-1) {
                cout << "camObj[0] process error" << endl;
                return 1;
            }

            if(findPerf) {
                perfObj[0].add_checkpoint("cam0 process_end");
                perfObj[0].update_totalInfo(true, false, false);
            }

            if(findPerf) perfObj[1].add_checkpoint("cam1 process_start");

            camObj[1].processCam();
            if(camObj[1].processReturnCode==-1) {
                cout << "camObj[1] process error" << endl;
                return 1;
            }

            if(findPerf) {
                perfObj[1].add_checkpoint("cam1 process_end");
                perfObj[1].update_totalInfo(true, false, false);
                delay0  = perfObj[0].delays_ms.at(1);
                delay1  = perfObj[1].delays_ms.at(1);
                totDelay= delay0+delay1;
                printf(
                    "delays{%7.3fms, %7.3fms}=%7.3f  FPS:%3.0f | ",
                    delay0,
                    delay1,
                    totDelay,
                    1.0/(totDelay/1000)
                );
            }
            //  71ms
        #elif useThreads
            if(findPerf) {
                perfObj[0].add_checkpoint("cam0 process_start");
                perfObj[1].add_checkpoint("cam1 process_start");
                perfObj[2].add_checkpoint("tot thread start");
            }
            
            thread t_cam0(thread_task, &camObj[0]);
            thread t_cam1(thread_task, &camObj[1]);
            t_cam0.join();
            if(camObj[0].processReturnCode==-1) {
                cout << "camObj[0] process error" << endl;
                return 1;
            }
            if(findPerf) perfObj[0].add_checkpoint("cam0 process_end");
            t_cam1.join();
            if(camObj[1].processReturnCode==-1) {
                cout << "camObj[1] process error" << endl;
                return 1;
            }
            if(findPerf) {
                perfObj[0].add_checkpoint("cam2 process end");
                perfObj[1].add_checkpoint("cam1 process_end");
                perfObj[2].add_checkpoint("tot thread end");
                perfObj[0].update_totalInfo(true, false, false);
                perfObj[1].update_totalInfo(true, false, false);
                perfObj[2].update_totalInfo(true, false, false);
                delay0  = perfObj[0].delays_ms.at(1);
                delay1  = perfObj[1].delays_ms.at(1);
                totDelay= delay0+delay1;
                printf(
                    "delays{%7.3fms, %7.3fms}=%7.3f  FPS:%3.0f | ",
                    delay0,
                    delay1,
                    totDelay,
                    1.0/(totDelay/1000)
                );
            }
        #endif
        //  t1
        inpPos[0] = camObj[0].totCnt_pos[0];
        inpPos[1] = camObj[1].totCnt_pos[0];
        camTri.solvePos(inpPos, solvedPos, true);
        //  0.030ms

        printf(
            "solvPos.[%6.2f, %6.2f] | inpCam.[%7.2f, %7.2f]",
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
