
#include <global_variables.hpp>

std::string absPath;

// PiPCA9685::PCA9685 pca{}; //("/dev/i2c-1", 0x40, false);
PiPCA9685::PCA9685 pca(false);

bool hardExit = false;

// IK related: ik calc variable declaration

float current_q[6]	= {0,0,0,0,0,0};

float new_q[6]		= {0,0,0,0,0,0};

/**
 * `{yaw, pitch, roll}` variables:
 * unit: degrees
 * */
float orient[3]		= {0,0,0};

float PP[3]			= {0,150,150};

float axisScal[3]	= {1, 1, 1};

float axisOffset[3]	= {0, 100, -200};

float axisFilter[3]	= {1, 1, 1};

float pitch, roll;

float Pitch=0, Roll=0;

nodemcu_orient orientObj(orient, "192.168.1.231");
//nodemcu_orient orientObj(orient, "192.168.1.177");


int prefSize[2] = {640, 480};

bool useAutoBrightne = true;
bool takeCVTrackPerf = true;

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
std::vector<IR_camTracking> camObj;
// {0, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takeCVTrackPerf},
// {2, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takeCVTrackPerf},


// Two_cam_triangle header class initialisation
float camPosition[2][2] = {{0, 0}, {250, 0}};
float camAng_offs[2] = {90, 123};
float inpPos[2], solvedPos[2];
camTriangle camTri(camPosition, camAng_offs);



// bool pigpioInitia = false;
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


void lock_cout(
	std::mutex &coutMutex,
	string toPrint,
	bool blockingLock,
	bool flushOut,
	bool startClearScr
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


std::mutex mtx_perfObj_threads[2];
// getPerf perfObj[2] {
// 	{"sub-thread[0]"},
// 	{"sub-thread[1]"}
// };
#endif


bool threadsInit[3] = {false, false, false};

cv::Mat flippedImg_main[2];
cv::Mat flippedImg_interm[2];


float camObjPos_main[2][2];
float camObjPos_interm[2][2];


int returCodes_main[2];
int returCodes_interm[2] = {0, 0};


std::vector<float> camProcess_delays[2];

std::vector<std::string> camProcess_delayNames[2];

size_t numContours_main[2] = {0, 0};
size_t numContours_interm[2] = {0, 0};

void transferCamVars(IR_camTracking* camPtr, int t_idx) {
    // if(dispToWindow) {
        flippedImg_interm[t_idx] = (*camPtr).imgFlipped;
    // }
    camObjPos_interm[t_idx][0]  = (*camPtr).totCnt_pos[0];
    camObjPos_interm[t_idx][1]  = (*camPtr).totCnt_pos[1];
    returCodes_interm[t_idx]    = (*camPtr).processReturnCode;
    numContours_interm[t_idx]   = (*camPtr).allCnt_pos.size();
}
void updateCamVars(int t_idx) {
    // if(dispToWindow) {
        flippedImg_main[t_idx] = flippedImg_interm[t_idx];
    // }
    camObjPos_main[t_idx][0]    = camObjPos_interm[t_idx][0];
    camObjPos_main[t_idx][1]    = camObjPos_interm[t_idx][1];
    returCodes_main[t_idx]      = returCodes_interm[t_idx];
    numContours_main[t_idx]     = numContours_interm[t_idx];
}

std::mutex mtx[2];
std::mutex mtx_sync[2];
std::mutex mtx_cout;

// /**mutex for saving frames onto `opencv_recorder` class object*/
// std::mutex mtx_vidRec;

bool exit_thread[2] = {false, false};

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

termMenu opt6_control_panel(false, 7, 7);

void exitFrom_lvl2(bool* driverBool) {
	startMenu.exitDriver = true;
	(*driverBool) = true;
}


_initClass::_initClass(
	int (*_init_function)(),
	int (*_close_function)(),
	bool _init
): _init_func(_init_function), _close_func(_close_function) {
	assert(_init_func || _close_func); //both init and close function can't be invalid (otherwise this serves no purpose)
	if(_init && _init_func) {
		int _returnCode = _init_func();
		if(_returnCode==0) {
			this->_init = true;
		}
	}
}
_initClass::~_initClass() {
	int _returnCode = 0;
	if(_close_func && this->_init) {
		_returnCode = _close_func();
	}
	if(_returnCode==0) {
		this->_init = false;
	}
}
int 	_initClass::call_init() {
	int _returnCode = 0;
	if(this->_init) return -69;
	if(_init_func) _returnCode = _init_func();
	if(_returnCode==0) this->_init = true;
	return _returnCode;
}
int 	_initClass::call_closing() {
	int _returnCode = 0;
	if(!this->_init) return -69;
	if(_close_func) _returnCode = _close_func();
	if(_returnCode==0) this->_init = false;
	return _returnCode;
}
const bool _initClass::isInit() {
	return this->_init;
}


DIY::typed_dict<std::string, _initClass> _init_status(
	{"pca", "camObj", "pigpio", "opencv recorder"},
	{
		_initClass(_init__pca, _close__pca, false),
		_initClass(_init__camObj, _close__camObj, false),
		_initClass(_init__pigpio, _close__pigpio, false),
		_initClass(_init__opencv_recorder, _close__opencv_recorder, false)
	}
);


void simplified_init() {

	if(!_init_status.get("pca").isInit() && _init_status.get("pca").call_init()) {
		std::cout << "ERROR: could not initialise pca library." << std::endl;
	}
	if(!_init_status.get("camObj").isInit() && _init_status.get("camObj").call_init()) {
		std::cout << "ERROR: could not initialise camObj objects." << std::endl;
	}

	if(recordFrames) {
		// recObj = opencv_recorder("Hexclaw_cameraFeeds", prefSize[0]*2, prefSize[1]*2, 15);
		if(!_init_status.get("opencv recorder").isInit() && _init_status.get("opencv recorder").call_init()) {
			std::cout << "ERROR: could not initialise opencv recorder object." << std::endl;
		}
	}
	if(!_init_status.get("pigpio").isInit() && _init_status.get("pigpio").call_init()) {
		std::cout << "ERROR: could not initialise pigpio library." << std::endl;
	}
}

int _init__pca() {
	bool init_success = false;
	try {
		init_success = pca.init();
	} catch (const std::system_error& e) {
		std::cout << e.what() << std::endl;
		std::cout << e.code() << std::endl;
	}
	if(!init_success) return 1;
	pca.set_pwm_freq(50.0);
	return 0;
}
int _init__camObj() {
	camObj.push_back(IR_camTracking(0, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takeCVTrackPerf));
	camObj.push_back(IR_camTracking(2, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takeCVTrackPerf));
	return 0;
}
int _init__pigpio() {
	if(gpioInitialise() < 0) {
		std::cout << "_init__pigpio(): pigpio \"gpioInitialise()\" failed."<<std::endl;
		return -1;
	}
	
	gpioSetMode(pin_ledRelay, PI_OUTPUT);
	gpioWrite(pin_ledRelay, 1);

	return 0;
}
int _init__opencv_recorder() {
	recObj = opencv_recorder("Hexclaw_cameraFeeds", prefSize[0]*2, prefSize[1]*2, 15);
	return 0;
}
int _close__pca() {

	return 0;
}
int _close__camObj() {

	return 0;
}
int _close__pigpio() {
	gpioTerminate();
	return 0;
}
int _close__opencv_recorder() {

}
