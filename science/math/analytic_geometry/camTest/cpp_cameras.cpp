
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


#define useThreads true
#define threadDebug false

#define useAutoBright   false
#define dispToWindow    true

#define takePerf        true
#define recordFrames    true

#if useThreads
#include <thread>
#include <mutex>
#endif

#include "../../../../teststuff/cpp/useful/useful.hpp"
#include "../../../../projects/proj_Hexclaw_cpp/HexclawMain/HW_headers/IR_camTrack.hpp"
#include "../../../../teststuff/cpp/two_cam_coordinate/two_cam_coordinate.hpp"
#include "../../../../teststuff/cpp/basic/Performance/getPerformance.hpp"
#include "../../../../teststuff/cpp/opencv/recordVideo/recordFrames.hpp"


/*std::ios::sync_with_stdio(false);*/

// using namespace std;


int prefSize[2] = {640, 480};

#if recordFrames
opencv_recorder recObj(
    "cpp_cameras_feed.mp4",
    prefSize[0]*2,  //using hconcat
    prefSize[1]*2,   //imgFlipped is by default vconcat
    15
);
#endif

/// array to hold initialisation boolean values (true if it's been init, false otherwise)
/// `[0]` = cam0-thread
/// `[1]` = cam1-thread
/// `[2]` = main-thread
bool threadsInit[3] = {false, false, false};

/// variable/container for cv::Mat display objects/img's that's used in the main thread
cv::Mat flippedImg_main[2];
/// @brief variable/container for intermediary cv::Mat images; temporarily holds new values from sub threads
cv::Mat flippedImg_interm[2];

/** main camera tracked object center position holder/container array;
 * `[0][0, 1]` = cam/thread 0;
 * `[1][0, 1]` = cam/thread 1;
*/
float camObjPos_main[2][2];
/**
 * intermediary container/holder array for tracked-object-center-positions received from sub-threads
 * `[0][0, 1]` = sub-thread[0]
 * `[1][0, 1]` = sub-thread[1]
*/
float camObjPos_interm[2][2];

/**
 * main-thread container/holder array for error exit codes from threads
 * `[0]` = sub-thread[0]
 * `[1]` = sub-thread[1]
*/
int returCodes_main[2];
/**
 * sub-thread container/holder array for error exit codes
 * `[0]` = sub-thread[0]
 * `[1]` = sub-thread[1]
*/
int returCodes_interm[2] = {0, 0};

/**
 * container array for number of contours detected, to be used in main thread
 * `[0]` = sub-thread[0]
 * `[1]` = sub-thread[1]
*/
size_t numContours_main[2] = {0, 0};
/**
 * container array for number of contours detected, to be used in sub threads
 * `[0]` = sub-thread[0]
 * `[1]` = sub-thread[1]
*/
size_t numContours_interm[2] = {0, 0};

/**
 * @brief transfer results from sub thread[`t-idx`] to `[..]_interm[t_idx]` variables
 * @param camPtr `IR_camTracking` object pointer
 * @param t_idx `int` value of which thread to use function on
*/
void transferCamVars(IR_camTracking* camPtr, int t_idx) {
    // if(dispToWindow) {
        flippedImg_interm[t_idx] = (*camPtr).imgFlipped;
    // }
    camObjPos_interm[t_idx][0]  = (*camPtr).totCnt_pos[0];
    camObjPos_interm[t_idx][1]  = (*camPtr).totCnt_pos[1];
    returCodes_interm[t_idx]    = (*camPtr).processReturnCode;
    numContours_interm[t_idx]   = (*camPtr).allCnt_pos.size();
}
/**
 * @brief update main-thread `[..]_main[t-idx]` variables with sub-thread `[..]_interm[t_idx]` variables
 * @param t_idx `int` value of which thread to use function on
*/
void updateCamVars(int t_idx) {
    // if(dispToWindow) {
        flippedImg_main[t_idx] = flippedImg_interm[t_idx];
    // }
    camObjPos_main[t_idx][0]    = camObjPos_interm[t_idx][0];
    camObjPos_main[t_idx][1]    = camObjPos_interm[t_idx][1];
    returCodes_main[t_idx]      = returCodes_interm[t_idx];
    numContours_main[t_idx]     = numContours_interm[t_idx];
}

#if useThreads
    /**
     * general variable mutexes
     * `[0]` = sub-thread[0]
     * `[1]` = sub-thread[1]
    */
    std::mutex mtx[2];
    /**
     * mutex for std::iostream::cout
    */
    std::mutex mtx_cout;

    // /**mutex for saving frames onto `opencv_recorder` class object*/
    // std::mutex mtx_vidRec;

    bool exit_thread[2] = {false, false};

    /**
     * @brief thread function
     * @param camRef `IR_camTracking` object pointer
     * @param t_idx `int` value for thread indexing/naming
    */
    void thread_task(IR_camTracking* camPtr, int t_idx) {
        std::unique_lock<std::mutex> u_lck(mtx[t_idx], std::defer_lock);
        std::unique_lock<std::mutex> u_lck_cout(mtx_cout, std::defer_lock);

        // #if recordFrames
        // std::unique_lock<std::mutex> u_lck_rec(mtx_vidRec, std::defer_lock);
        // #endif

        if(threadDebug) {
            u_lck_cout.lock();
            std::cout << "\nthread " << t_idx << " initialised" << std::endl;
            u_lck_cout.unlock();
        }
        while(true) {
            camPtr->processCam();

            u_lck.lock();
            transferCamVars(camPtr, t_idx);
            if(!threadsInit[t_idx]) threadsInit[t_idx] = true;
            if(threadDebug) {
                u_lck_cout.lock();
                std::cout << "\n\tthread:["<<t_idx<<"]: transferCamVars called";
                u_lck_cout.unlock();
            }
            if(exit_thread[t_idx] || returCodes_interm[t_idx]==-1) {
                u_lck_cout.lock();
                if(returCodes_interm[t_idx]==-1) std::cout << "\nERROR: IR_camTracking error:";
                std::cout << "\n\tthread:["<<t_idx<<"]: exit called. exiting...";
                u_lck_cout.unlock();
                break;
            }
            u_lck.unlock();

        }
    }
#endif

/**
 * simple function for locking a mutex reference and printing `toPrint` to cout buffer
 * NOTE: this function will release after locking but before calling the function *make sure* the mutex object is unlocked,
 * because otherwise it'll give undefined behavious.
 * @param coutMutex `std::mutex` object reference.
 * @param toPrint the string object to print to cout.
 * @param blockingLock whether to use the blocking member function
 * `std::mutex::lock()` or the nonblocking `std::mutex::try_lock()`.
*/
void lock_cout(std::mutex &coutMutex, string toPrint, bool blockingLock = true) {
    std::unique_lock<std::mutex> u_lck_cout(coutMutex, std::defer_lock);
    if(blockingLock) {
        u_lck_cout.lock();
        std::cout << toPrint;
        u_lck_cout.unlock();
    }
    else {
        if(u_lck_cout.try_lock()) {
            std::cout << toPrint;
            u_lck_cout.unlock();
        }
    }
}


bool logOutput = false;

getPerf localPerfObj {"start"};

int main(int argc, char** argv) {
    if(takePerf) {
        // localPerfObj = getPerf("[start]");
        localPerfObj.csv_setup("csv_files/cpp_camera_csv", true);
    }


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

    #if useThreads
        std::unique_lock<std::mutex> u_lck0, u_lck1, u_lck_cout;
        if(logOutput) outLogFile << " -declared std::unique_lock<std::mutex>> objects\n";

        std::thread t_cam0, t_cam1;
        if(logOutput) outLogFile << " -declared std::thread objects\n";
    #endif

    if(useCamera) {
        // IR_camTracking camObj[2] {

        /// Create and setup IR_camTracking objects
        camObj.push_back(IR_camTracking(2, prefSize[0], prefSize[1], useAutoBright, dispToWindow, false));
        camObj.push_back(IR_camTracking(0, prefSize[0], prefSize[1], useAutoBright, dispToWindow, false));
        if(logOutput) outLogFile << " -push_back() added both cams onto camObj\n";
        
        camObj[0].setup_window();
        if(logOutput) outLogFile << " -camObj[0].setup_window()\n";
        camObj[1].setup_window();
        if(logOutput) outLogFile << " -camObj[1].setup_window()\n";

        #if useThreads
            // /// sub-thread [0] mutex
            // std::unique_lock<std::mutex> u_lck0(mtx[0], std::defer_lock);
            // /// sub-thread [1] mutex
            // std::unique_lock<std::mutex> u_lck1(mtx[1], std::defer_lock);
            // /// local cout mutex obj
            // std::unique_lock<std::mutex> u_lck_cout(mtx_cout, std::defer_lock);

            // std::thread t_cam0(thread_task, &camObj[0], 0);
            // std::thread t_cam1(thread_task, &camObj[1], 1);

            u_lck0      = std::unique_lock<std::mutex>(mtx[0], std::defer_lock);
            u_lck1      = std::unique_lock<std::mutex>(mtx[1], std::defer_lock);
            u_lck_cout  = std::unique_lock<std::mutex>(mtx_cout, std::defer_lock);
            if(logOutput) outLogFile << " -defined unique_lock()'s\n";
            
            t_cam0  = std::thread(thread_task, &camObj[0], 0);
            t_cam1  = std::thread(thread_task, &camObj[1], 1);
            if(logOutput) outLogFile << " -defined std::thread objects\n";
            std::this_thread::sleep_for(1000ms);
        #endif
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
        if(takePerf) { localPerfObj.add_checkpoint("scanf()"); }

        char nums[2][6];
        if(useCamera) {
            for(int i=0; i<6; i++) { nums[0][i] = toRecev[i+1]; }
            if(atoi(nums[0]) == 6942)  {
                outLogFile << "--exit called:\n";
                u_lck0.lock();
                exit_thread[0] = true;
                u_lck0.unlock();
                u_lck1.lock();
                exit_thread[1] = true;
                u_lck1.unlock();
                std::cout << "exit called" << std::endl;
                break;
            }
        }
        if(takePerf) { localPerfObj.add_checkpoint("checkExit"); }
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
                // std::thread t_cam0(thread_task, std::ref(camObj[0]));
                // std::thread t_cam1(thread_task, std::ref(camObj[1]));
                // t_cam0.join();
                // t_cam1.join();

                if(!threadsInit[2]) {
                    if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: NOTE: Threads have not been initialised:\n -initialising."); }

                    if(logOutput) outLogFile << " -threads not initialised: initialising\n";

                    while(!threadsInit[2]) {
                        u_lck0.lock();
                        if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: -u_lck0 locked."); std::this_thread::sleep_for(100ms); }
                        u_lck1.lock();
                        if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: -u_lck1 locked."); std::this_thread::sleep_for(100ms); }
                        updateCamVars(0);
                        if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: -updateCamVars(0)."); }
                        updateCamVars(1);
                        if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: -updateCamVars(1)."); }
                        if(threadsInit[0] && threadsInit[1]) threadsInit[2] = true;
                        if(threadDebug) { 
                            u_lck_cout.lock();
                            std::cout << ".";
                            cout.flush();
                            u_lck_cout.unlock();
                        }
                        std::this_thread::sleep_for(500ms);
                        u_lck1.unlock();
                        u_lck0.unlock();
                    }
                    if(logOutput) outLogFile << " -threads initialised\n";
                    if(threadDebug) {
                        u_lck_cout.lock();
                        std::cout << "\nthread:[2]: Initialised!" << std::endl;
                        u_lck_cout.unlock();
                    }
                }
                else {
                    u_lck0.lock();
                    updateCamVars(0);
                    u_lck0.unlock();
                    u_lck1.lock();
                    updateCamVars(1);
                    u_lck1.unlock();
                    if(logOutput) outLogFile << " -thread: initiated loop iteration\n";
                    if(returCodes_main[0]==-1) {
                        if(threadDebug) {
                            u_lck_cout.lock();
                            std::cout << "\ncamObj[0] process error" << std::endl;
                            u_lck_cout.unlock();
                        }
                        return 1;
                    }
                    if(returCodes_main[1]==-1) {
                        if(threadDebug) {
                            u_lck_cout.lock();
                            std::cout << "\ncamObj[1] process error" << std::endl;
                            u_lck_cout.unlock();
                        }
                        return 1;
                    }
                }
            #elif !useThreads
                camObj[0].processCam();
                camObj[1].processCam();
                transferCamVars(&camObj[0], 0);
                transferCamVars(&camObj[1], 1);
                updateCamVars(0);
                updateCamVars(1);
            #endif
            if(returCodes_main[0]==-1 || returCodes_main[1]==-1) {
                if(logOutput) outLogFile << " -couldn't processCam: returned -1: \n";
                if(logOutput) outLogFile.close();
                return 0;
            }
            if(logOutput) outLogFile << " -useCamera: processCam\n";
        }
        if(takePerf) { localPerfObj.add_checkpoint("loopIter"); }


        if(useCamera && numContours_main[0]>0) {
            // cv::Size camSize = camObj[0].imgFlipped.size();
            
            camPos[1][0] = float(prefSize[0]) - camObjPos_main[0][0];
            camPos[1][1] = float(prefSize[1]) - camObjPos_main[0][1];
            if(numContours_main[0]>0 && numContours_main[1]>0) {

                camPos[0][0] = float(prefSize[0]) - camObjPos_main[1][0];
                camPos[0][1] = float(prefSize[1]) - camObjPos_main[1][1];
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
        if(takePerf) { localPerfObj.add_checkpoint("fillChar"); }
        if(useTwoCamClass) {
            camTri->solvePos(inpPos, solvedPos, false);
            if(logOutput) outLogFile << " -useTwoCamClass: -camTri->solvePos()\n";
            solvedY = -sin(toRadians(((*camTri).camRes[0][1]*0.5-camObjPos_main[0][1])*(*camTri).camCoef[0][1]))*solvedPos[1];
            if(takePerf) { localPerfObj.add_checkpoint("solvePos"); }
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
        if(takePerf) { localPerfObj.add_checkpoint("filltoSend"); }

        for(int i=7; i<71; i+=7) {
            if(i==14 || i==28 || i==42 || i==56) continue;
            toSend[i] = ',';
        }
        if(logOutput) outLogFile << " -toSend[i] = ','; commas added\n";
        
        printf("%s\n", toSend);
        if(takePerf) { localPerfObj.add_checkpoint("sent"); }
        if(logOutput) outLogFile << " -final printf(\"%s\\n\", toSend);\n";
        std::cout.flush();
        if(logOutput) outLogFile << " -std::cout.flush();\n";
        if(logOutput) logOutput = false;
        // if(takePerf) { localPerfObj.add_checkpoint("after_flush"); }

        if(useCamera) {
            #if recordFrames
            cv::Mat concatImg;
            cv::hconcat(flippedImg_main[0], flippedImg_main[1], concatImg);
            recObj.addFrame(concatImg);
            #endif
        }
        if(takePerf) { localPerfObj.update_totalInfo(true,false,false); }
    }
    if(useCamera) {
        #if recordFrames
            recObj.releaseVideo();
        #endif
        #if useThreads
            t_cam0.join();
            t_cam1.join();
        #endif

        camObj[0].close();
        camObj[1].close();
    }
    if(takePerf) {
        localPerfObj.close();
    }

    if(logOutput) {
        std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        outLogFile << "end: " << ctime(&currentTime);
        outLogFile.close();
    }

}
