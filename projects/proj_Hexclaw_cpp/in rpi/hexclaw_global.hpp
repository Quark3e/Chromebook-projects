
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
nodemcu_orient orientObj(orient, "192.168.1.231");
//nodemcu_orient orientObj(orient, "192.168.1.177");


/// @brief prefered image size to resize/convert/use for all images
int prefSize[2] = {640, 480};

/// @brief whether to use automatic brightness adjustment with opencv cam tracking
bool useAutoBrightne = true;
/// @brief whether to read and measure performance (delays/fps) for `IR_camTracking` class
bool takeCVTrackPerf = true;

/// @brief whether to display `IR_camTracking`'s images
bool displayToWindow = false;


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
	{0, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takeCVTrackPerf},
	{2, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takeCVTrackPerf},
};


// Two_cam_triangle header class initialisation
float camPosition[2][2] = {{0, 0}, {250, 0}};
float camAng_offs[2] = {90, 123};
float inpPos[2], solvedPos[2];
camTriangle camTri(camPosition, camAng_offs);



bool pigpioInitia = false;
int pin_ledRelay = 23;

#if takePerf
//main thread performance measurement
getPerf perfObj[3] {
	{"main thread"},
	{"sub thread[0]"},
	{"sub thread[1]"}
};
#endif

opencv_recorder recObj;



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
void lock_cout(
	std::mutex &coutMutex,
	string toPrint,
	bool blockingLock = true,
	bool flushOut = false,
	bool startClearScr = false
) {
    std::unique_lock<std::mutex> u_lck_cout(coutMutex, std::defer_lock);
    if(blockingLock) {
        u_lck_cout.lock();
		if(startClearScr) std::cout << "\x1B[2J";
        std::cout << toPrint;
		if(flushOut) std::cout.flush();
        u_lck_cout.unlock();
    }
    else {
        if(u_lck_cout.try_lock()) {
			if(startClearScr) std::cout << "\x1B[2J";
            std::cout << toPrint;
			if(flushOut) std::cout.flush();
            u_lck_cout.unlock();
        }
    }
}

#if useThreads
#if takePerf
//sub thread(s) performance measurements

/**
 * sub-thread perfObj mutexes
*/
std::mutex mtx_perfObj_threads[2];
// getPerf perfObj[2] {
// 	{"sub-thread[0]"},
// 	{"sub-thread[1]"}
// };
#endif

/** array to hold initialisation boolean values (true if it's been init, false otherwise)
 * `[0]` = cam0-thread
 * `[1]` = cam1-thread
 * `[2]` = main-thread
*/
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
 * vector of cam processing delays for each webcam
 * in an array.
*/
std::vector<float> camProcess_delays[2];

/**
 * vector of cam processing delay names for each webcam
 * in an array.
*/
std::vector<std::string> camProcess_delayNames[2];

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
 * Syncing purpose mutexes:
 * 
 * Purpose is for a sub-thread to lock its own mutex, then
 * continuously `try_lock()` and `unlock()` the other mutex to check if that sub-thread
 * has reached same point in process.
 * 
 * When both sub-threads have "hard"-locked their mutexes the
 * `try_lock()` will return false, from which each sub-thread can unlock their own mutex and run
 * the rest of the function
 * '[0]' = sub-thread[0]
 * '[1]' = sub-thread[1]
*/
std::mutex mtx_sync[2];

/**
 * mutex for std::iostream::cout
*/
std::mutex mtx_cout;

// /**mutex for saving frames onto `opencv_recorder` class object*/
// std::mutex mtx_vidRec;

bool exit_thread[2] = {false, false};

/**
 * @brief Multithread function for processing and storing image/webcam processing results
 * @param camRef `IR_camTracking` object pointer
 * @param t_idx `int` value for thread indexing/naming
*/
void thread_task(IR_camTracking* camPtr, int t_idx) {
	std::unique_lock<std::mutex> u_lck(mtx[t_idx], std::defer_lock);
	std::unique_lock<std::mutex> u_lck_cout(mtx_cout, std::defer_lock);
	std::unique_lock<std::mutex> u_lck_sync(mtx_sync[t_idx], std::defer_lock);
	std::unique_lock<std::mutex> u_lck_syncCheck(mtx_sync[abs(t_idx-1)], std::defer_lock);
	#if takePerf
	std::unique_lock<std::mutex> u_lck_threadPerfObj(mtx_perfObj_threads[t_idx], std::defer_lock);
	#endif
	// #if recordFrames
	// std::unique_lock<std::mutex> u_lck_rec(mtx_vidRec, std::defer_lock);
	// #endif

	if(threadDebug) {
		u_lck_cout.lock();
		std::cout << "\nthread " << t_idx << " initialised" << std::endl;
		u_lck_cout.unlock();
	}
	(*camPtr).printAll = false;
	while(true) {
		/*Synchronisation between sub-threads*/
		// u_lck_sync.lock();
		// while(true) {
		// 	if(u_lck_syncCheck.try_lock()) u_lck_syncCheck.unlock();
		// 	else break;
		// 	std::this_thread::sleep_for(10ms);
		// }
		// if(threadDebug) lock_cout(mtx_cout, "\n["+to_string(t_idx)+"]: sub-threads have been synced",true,true);
		// u_lck_sync.unlock();

		#if takePerf
		u_lck_threadPerfObj.lock();
		perfObj[t_idx+1].add_checkpoint("thread-syncing"); //c1
		u_lck_threadPerfObj.unlock();
		#endif

		/*Webcam reading and processing*/
		camPtr->processCam();
		#if takePerf
		u_lck_threadPerfObj.lock();

		if(!threadsInit[t_idx]) {
			for(size_t i=1; i<(*camPtr).perfObj.names.size(); i++) {
				camProcess_delayNames[t_idx].push_back(
					(*camPtr).perfObj.names.at(i).substr(3)
				);
				camProcess_delays[t_idx].push_back(
					(*camPtr).perfObj.delays_ms.at(i)
				);
			}
		}
		else {
			for(size_t i=1; i<(*camPtr).perfObj.names.size(); i++) {
				camProcess_delays[t_idx][i-1] = (*camPtr).perfObj.delays_ms.at(i);
			}
		}

		// if(t_idx==0) {
		// 	u_lck_cout.lock();
		// 	std::cout << "\x1B[H"<<std::endl;
		// 	for(size_t i=0; i<(*camPtr).perfObj.names.size(); i++) {
		// 		std::cout<<"\n|"<<std::setw(10)<<(*camPtr).perfObj.names.at(i)<<"|: "<<(*camPtr).perfObj.delays_ms.at(i);
		// 	}
		// 	std::cout<<"\n-----------"<<std::endl;
		// 	if(threadsInit[t_idx]) {
		// 		for(size_t i=0; i<camProcess_delayNames[t_idx].size(); i++) {
		// 			std::cout<<"\n|"<<std::setw(10)<<camProcess_delayNames[t_idx].at(i)<<"|: "<<camProcess_delays[t_idx].at(i);
		// 		}
		// 	}
		// 	u_lck_cout.unlock();
		// }

		perfObj[t_idx+1].add_checkpoint("cam-processing"); //c2
		u_lck_threadPerfObj.unlock();
		#endif

		/*Transfer of cam processing results from local thread-affected variables to
		independant main-thread variables*/
		u_lck.lock();
		transferCamVars(camPtr, t_idx);
		if(!threadsInit[t_idx]) threadsInit[t_idx] = true;
		if(threadDebug) {
			lock_cout(mtx_cout, "\n\tthread:["+to_string(t_idx)+"]: transferCamVars called",true,false);
		}
		if(exit_thread[t_idx] || returCodes_interm[t_idx]==-1) {
			u_lck_cout.lock();
			if(returCodes_interm[t_idx]==-1) std::cout << "\nERROR: IR_camTracking error:";
			std::cout << "\n\tthread:["<<t_idx<<"]: exit called. exiting...";
			u_lck_cout.unlock();
			break;
		}
		u_lck.unlock();
		#if takePerf
		u_lck_threadPerfObj.lock();
		perfObj[t_idx+1].add_checkpoint("transf_to_main_var"); //c3
		perfObj[t_idx+1].update_totalInfo(true,false,false);
		u_lck_threadPerfObj.unlock();
		#endif

	}
}


#endif


termMenu startMenu(true, 1, 6);
termMenu modeMenu(true, 1, 6);


termMenu opt6_startMenu(true, 1, 6);

/**
 *  _ _ _ _ _ _ _
 *  _ 0 _ 0 _ 0 _
 *  _ _ _ _ _ _ _
 *  _ 0 _ 0 _ 0 _
 *  _ _ _ _ _ _ _
 *  _ x _ _ _ e _
 *  _ _ _ _ _ _ _
 * 
*/
termMenu opt6_control_panel(false, 7, 7);

void exitFrom_lvl2(bool* driverBool) {
	startMenu.exitDriver = true;
	(*driverBool) = true;
}

#endif
