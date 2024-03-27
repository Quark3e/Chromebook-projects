
#pragma once
#ifndef HPP_HEXCLAW_GLOBAL
#define HPP_HEXCLAW_GLOBAL

#include "hexclaw_constants.hpp"
#include "hexclaw_includes.hpp"


/// @brief absolute path of the current directory for this program
std::string absPath;

/// @brief pca9685-board libraries obbject
PiPCA9685::PCA9685 pca{};

bool hardExit = false;

// IK related: ik calc variable declaration

/// @brief container array for the current/old servo motors angles
float current_q[6]	= {0,0,0,0,0,0};

/// @brief new servo rotations angles
float new_q[6]		= {0,0,0,0,0,0};

/**
 * `{yaw, pitch, roll}` variables:
 * unit: degrees
 * */
float orient[3]		= {0,0,0};

/// @brief `{x, y, z}` coordinate array container
float PP[3]			= {0,150,150};

/// @brief new axis value scalars
float axisScal[3]	= {1, 1, 1};

/// @brief new axis value offset
float axisOffset[3]	= {0, 100, -200};

/// @brief new axis filter
float axisFilter[3]	= {1, 1, 1};

/// @brief raw tilt values from accelerometer readings
float pitch, roll;

/// @brief filtered tilt values from accelerometer readings
float Pitch=0, Roll=0;

/// @brief Wireless nodemcu udp COM header class object
nodemcu_orient orientObj(orient);



/// @brief prefered image size to resize/convert/use for all images
int prefSize[2] = {640, 480};

/// @brief whether to use automatic brightness adjustment with opencv cam tracking
bool useAutoBrightne = true;
/// @brief whether to read and measure performance (delays/fps) for `IR_camTracking` class
bool takeCVTrackPerf = false;

/// @brief whether to display `IR_camTracking`'s images
bool displayToWindow = true;


bool displayTFT = false;



// int l_HSV[3] = {0, 0, 255};
// int u_HSV[3] = {179, 9, 255};
int HW_HSV[2][3] = {
	{0, 0, 255},
	{179, 9, 255}
};

const char* window_name = "Window";
// IR camtracking header class initialization
IR_camTracking camObj[2] {
	{2, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takeCVTrackPerf},
	{0, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takeCVTrackPerf},
};

// Two_cam_triangle header class initialisation
float camPosition[2][2] = {{0, 0}, {250, 0}};
float camAng_offs[2] = {90, 123};
float inpPos[2], solvedPos[2];
camTriangle camTri(camPosition, camAng_offs);



bool pigpioInitia = false;
int pin_ledRelay = 23;

#if takePerf
getPerf perfObj[1] {{"main thread"}};
#endif

#if recordFrames
opencv_recorder recObj;
#endif



#if useThreads

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

/**
 * simple function for locking a mutex reference and printing `toPrint` to cout buffer
 * NOTE: this function will release after locking but before calling the function *make sure* the mutex object is unlocked,
 * because otherwise it'll give undefined behavious.
 * @param coutMutex `std::mutex` object reference.
 * @param toPrint the string object to print to cout.
 * @param blockingLock whether to use the blocking member function
 * `std::mutex::lock()` or the nonblocking `std::mutex::try_lock()`.
 * @param flushOut whether to flush after printing to cout
*/
void lock_cout(std::mutex &coutMutex, string toPrint, bool blockingLock = true, bool flushOut = false) {
    std::unique_lock<std::mutex> u_lck_cout(coutMutex, std::defer_lock);
    if(blockingLock) {
        u_lck_cout.lock();
        std::cout << toPrint;
		if(flushOut) std::cout.flush();
        u_lck_cout.unlock();
    }
    else {
        if(u_lck_cout.try_lock()) {
            std::cout << toPrint;
			if(flushOut) std::cout.flush();
            u_lck_cout.unlock();
        }
    }
}

#endif



#endif