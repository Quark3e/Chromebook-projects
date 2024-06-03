
#include <iostream>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#define useThreads  true
#define threadDebug false
#define findPerf    true

#define useAutoBright   true
#define dispToWindow    false
#define takePerf        false


#if useThreads
#include <thread>
#include <chrono>
#include <mutex>
#endif

// opencv/image tracking
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>

#include "../useful/useful.hpp"
#include "../useful/createTable.hpp"
#include "two_cam_coordinate.hpp"
#include "../basic/Performance/getPerformance.hpp"
#include "../../../projects/proj_Hexclaw_cpp/in rpi/HW_headers/IR_camTrack.hpp"

using namespace std;



int prefSize[2] = {640, 480};



// IR_camTracking camObj(0);
IR_camTracking camObj[2] {
    {2, prefSize[0], prefSize[1], useAutoBright, dispToWindow, takePerf},
    {0, prefSize[0], prefSize[1], useAutoBright, dispToWindow, takePerf},
};


bool threadsInit[3] = {false, false, false}; /*{cam0-thread, cam1-thread, main-thread}*/
cv::Mat oldFlippedImg[2], newFlippedImg[2];
float camObjPos_old[2][2], camObjPos_new[2][2];
int oldReturCode[2], returCode[2] = {0, 0};


/// @brief save results from camObj[t_idx] in sub-thread to temporary holders in main-thread
/// @param camPtr pointer to camObj
/// @param t_idx index to what camObjs variables
void transferCamVars(IR_camTracking* camPtr, int t_idx) {
    if(dispToWindow) {
        newFlippedImg[t_idx] = (*camPtr).imgFlipped;
    }
    camObjPos_new[t_idx][0] = (*camPtr).totCnt_pos[0];
    camObjPos_new[t_idx][1] = (*camPtr).totCnt_pos[1];
    returCode[t_idx] = (*camPtr).processReturnCode;
}
/// @brief updates "old" vars with "new". Called by main thread whenever it wishes to refresh result values
/// @param t_idx index to what "cam" objects to refresh
void updateCamVars(int t_idx) {
    if(dispToWindow) {
        oldFlippedImg[t_idx] = newFlippedImg[t_idx];
    }
    camObjPos_old[t_idx][0] = camObjPos_new[t_idx][0];
    camObjPos_old[t_idx][1] = camObjPos_new[t_idx][1];
    oldReturCode[t_idx] = returCode[t_idx];
}


#if useThreads
    mutex mtx[2], mtx_cout;

    bool exit_thread[2] = {false, false};

    getPerf perfObj[1] {
        // {"cam0 process"},
        // {"cam1 process"},
        {"total thread"}
    };

    void thread_task(IR_camTracking* camPtr, int t_idx) {
        unique_lock<mutex> u_lck(mtx[t_idx], defer_lock);
        unique_lock<mutex> u_lck_cout(mtx_cout, defer_lock);
        u_lck_cout.lock();
        cout << "\nthread " << t_idx << " initialised"<<endl;
        u_lck_cout.unlock();
        while(true) {
            camPtr->processCam();
            u_lck.lock();
            
            transferCamVars(camPtr, t_idx);
            if(!threadsInit[t_idx]) threadsInit[t_idx]=true;
            if(threadDebug) {
                u_lck_cout.lock();
                cout << "\n    thread:[" << t_idx << "]: transferCamVars called";// << endl;
                u_lck_cout.unlock();
            }
            u_lck.unlock();
            if(exit_thread[t_idx]) {
                u_lck_cout.lock();
                cout << "\n    thread:[" << t_idx << "] exit called. exiting..";
                u_lck_cout.unlock();
                break;
            }
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

    #if useThreads
        unique_lock<mutex> u_lck0(mtx[0], defer_lock);
        unique_lock<mutex> u_lck1(mtx[1], defer_lock);
        unique_lock<mutex> u_lck_cout(mtx_cout, defer_lock);

        thread t_cam0(thread_task, &camObj[0], 0);
        thread t_cam1(thread_task, &camObj[1], 1);
        this_thread::sleep_for(1000ms);
    #endif

    createTable printTable(3, 2);
    printTable.insertText("solvedPos", 0, 0);
    printTable.insertText("camPos", 0, 1);
    if(findPerf) {
        printTable.add_row();
        printTable.insertText("perf:", 0, 2);
    }

    while(true) {
        //  t1
        #if findPerf 
            float delay0, delay1, totDelay;
        #endif
        #if !useThreads
            if(findPerf) perfObj[0].add_checkpoint("cam0 process_start");
            camObj[0].processCam();
            transferCamVars(&camObj[0], 0);
            updateCamVars(0);
            if(oldReturCode[0]==-1) {
                cout << "camObj[0] process error" << endl;
                return 1;
            }

            if(findPerf) {
                perfObj[0].add_checkpoint("cam0 process_end");
                perfObj[0].update_totalInfo(true, false, false);
            }

            if(findPerf) perfObj[1].add_checkpoint("cam1 process_start");

            camObj[1].processCam();
            transferCamVars(&camObj[1], 1);
            updateCamVars(1);
            if(oldReturCode[1]==-1) {
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
                // perfObj[0].add_checkpoint("cam0 process_start");
                // perfObj[1].add_checkpoint("cam1 process_start");
                perfObj[0].add_checkpoint("tot thread start");
            }
            // thread t_cam0(thread_task, &camObj[0]);
            // thread t_cam1(thread_task, &camObj[1]);
            
            // t_cam0.join();

            if(!threadsInit[2]) {
                u_lck_cout.lock();
                cout << "\nthread:[2]: NOTE: Threads have not been initialised:\n -initialising.";
                u_lck_cout.unlock();
                while(!threadsInit[2]) {
                    //// Apparently it's on C++14. Too afraid to update. need to update
                    //std::scoped_lock scope_lock(mtx[0], mtx[1]);
                    u_lck0.lock();
                    if(threadDebug) {
                        u_lck_cout.lock();
                        cout << "\nthread:[2]: -u_lck0 locked.";
                        u_lck_cout.unlock();
                        this_thread::sleep_for(100ms);
                    }
                    u_lck1.lock();
                    if(threadDebug) {
                        u_lck_cout.lock();
                        cout << "\nthread:[2]: -u_lck1 locked.";
                        u_lck_cout.unlock();
                        this_thread::sleep_for(100ms);
                    }
                    updateCamVars(0);
                    if(threadDebug) {
                        u_lck_cout.lock();
                        cout << "\nthread:[2]:  -updateCamVars(0).";
                        u_lck_cout.unlock();
                    }
                    updateCamVars(1);
                    if(threadDebug) {
                        u_lck_cout.lock();
                        cout << "\nthread:[2]:  -updateCamVars(1).";
                        u_lck_cout.unlock();
                    }
                    if(threadsInit[0] && threadsInit[1]) threadsInit[2] = true;
                    cout << ".";
                    cout.flush();
                    this_thread::sleep_for(500ms);
                    u_lck1.unlock();
                    u_lck0.unlock();
                }
                u_lck_cout.lock();
                cout << "\nthread:[2]: Initialised!" << endl;
                u_lck_cout.unlock();
            }
            else {
                if(true/*u_lck0.try_lock()*/) {
                    u_lck0.lock();
                    updateCamVars(0);
                    u_lck0.unlock();
                }
                if(true/*u_lck1.try_lock()*/) {
                    u_lck1.lock();
                    updateCamVars(1);
                    u_lck1.unlock();
                }
            }
            if(oldReturCode[0]==-1) {
                u_lck_cout.lock();
                cout << "camObj[0] process error" << endl;
                u_lck_cout.unlock();
                return 1;
            }
            if(findPerf) perfObj[0].add_checkpoint("cam0 process_end");
            // t_cam1.join();
            if(oldReturCode[1]==-1) {
                u_lck_cout.lock();
                cout << "camObj[1] process error" << endl;
                u_lck_cout.unlock();
                return 1;
            }
            if(findPerf) {
                // perfObj[0].add_checkpoint("cam2 process end");
                // perfObj[1].add_checkpoint("cam1 process_end");
                perfObj[0].add_checkpoint("tot thread end");
                // perfObj[0].update_totalInfo(true, false, false);
                // perfObj[1].update_totalInfo(true, false, false);
                perfObj[0].update_totalInfo(true, false, false);
                // delay0  = perfObj[0].delays_ms.at(1);
                // delay1  = perfObj[1].delays_ms.at(1);
                // totDelay= perfObj[2].delays_ms.at(1);
                // printf(
                //     "delays{%7.3fms, %7.3fms}=%7.3f  FPS:%3.0f | ",
                //     delay0,
                //     delay1,
                //     totDelay,
                //     1.0/(totDelay/1000)
                // );

                totDelay = perfObj[0].delays_ms.at(1);

                printTable.insertNum(totDelay, 1, 2);
                printTable.insertNum(static_cast<float>(1.0/(totDelay/1000)), 2, 2);
                printTable.add_to_cell("ms", 1, 2);
            }
        #endif
        //  t1
        inpPos[0] = camObjPos_old[0][0];
        inpPos[1] = camObjPos_old[1][0];
        camTri.solvePos(inpPos, solvedPos, true);
        //  0.030ms

        printTable.insertNum(solvedPos[0],1,0,2);
        printTable.insertNum(solvedPos[1],2,0,2);

        printTable.insertNum(inpPos[0],1,1,2);
        printTable.insertNum(inpPos[1],2,1,2);
        printTable.strExport();

        #if useThreads
        u_lck_cout.lock();
        #endif
        ansiPrint(
            printTable.exportStr,
            static_cast<float>(1.0),
            static_cast<float>(0.1)
        );
        cout.flush();
        #if useThreads
        u_lck_cout.unlock();
        #endif

        if(dispToWindow) {
            //  t1
            cv::Mat winImg;
            cv::hconcat(oldFlippedImg[0], oldFlippedImg[1], winImg);
            //  ~10ms

            //  t1
            int keyInp = cv::waitKey(10);
            #if !useThreads
            cv::imshow(camObj[0].win_name, winImg);
            #elif useThreads
            cv::imshow("Main thread window", winImg);
            #endif
            //  <30ms

            // t1
            if(keyInp==27) return 0;
            else if(keyInp==32) break;
            //  0.01ms
        }
        
        // TOTAL: <110ms with displayToWindow==true
        // if(takePerformance) perfObj.update_totalInfo(true, true, true, ' ','\r');
        
        // #if useThreads
        // u_lck_cout.lock();
        // #endif
        // printf("\n");
        // #if useThreads
        // u_lck_cout.unlock();
        // #endif
        this_thread::sleep_for(10ms);
    }

    #if useThreads
    
    t_cam0.join();
    t_cam1.join();
    #endif

    return 0;
}
