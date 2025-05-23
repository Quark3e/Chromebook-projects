
#include <cstdlib>
#include <useful.hpp>
#include "constants.hpp"
#include "global_variables.hpp"

std::string absPath;

// PiPCA9685::PCA9685 pca{}; //("/dev/i2c-1", 0x40, false);
// #if _MACHINE__RPI_MAIN

PiPCA9685::PCA9685 pca(false);
// #endif

bool hardExit = false;

// IK related: ik calc variable declaration

// float current_q[6]    = {0,0,0,0,0,0};
// float new_q[6]    	= {0,0,0,0,0,0};

servo_angles_6DOF current_q(0);
servo_angles_6DOF new_q(0);

/**
 * `{yaw, pitch, roll}` variables:
 * unit: degrees
 * */
// float orient[3]    	= {0,0,0};
// float PP[3]    		= {0,150,150};
// float axisScal[3]    = {1, 1, 1};
// float axisOffset[3]    = {0, 100, -200};
// float axisFilter[3]    = {1, 1, 1};

pos3d<float> orient{0, 0, 0};
pos3d<float> PP{-250, 150, 150};
pos3d<float> axisScal{-1, 1, 1};
pos3d<float> axisOffset{0, 100, -100};
// pos3d<float> axisOffset{-100, 0, 0};
pos3d<float> axisFilter{0.1, 0.1, 0.1};



float pitch, roll;

float Pitch=0, Roll=0;

// nodemcu_orient orientObj(orient, "192.168.1.231");
nodemcu_orient orientObj("192.168.1.117", DEFAULT__PORT, false);


// int prefSize[2] = {DEFAULT_PREF_WIDTH, DEFAULT_PREF_HEIGHT};

pos2d<int> prefSize{DEFAULT_PREF_WIDTH, DEFAULT_PREF_HEIGHT};

// bool useAutoBrightne = true;
// bool takeCVTrackPerf = true;

// bool displayToWindow = false;


bool displayTFT = false;


// int l_HSV[3] = {0, 0, 255};
// int u_HSV[3] = {179, 9, 255};
// int HW_HSV[2][3] = {
// 	{0, 0, 255},
// 	{179, 9, 255}
// };

// HSV color range for object tracking
std::vector<std::vector<int>> HW_HSV{
	{0, 0, 255},	//{0, 0, 255},
	{179, 9, 255}	//{0, 0, 255}
};

std::string window_name = "Window";

// IR camtracking header class initialization
// std::vector<IR_camTracking> camObj;


#if _WIN32
	std::vector<int> camID{0, 1};
#else
	std::vector<int> camID{0, 2};
#endif

float opt_camObj__areaLim = 100;

std::vector<CVTRACK::camObjTracker> camObj{
	CVTRACK::camObjTracker(false, camID[0], prefSize[0], prefSize[1], false, true, {-1, 6}, {0, 0, 255}, {179, 9, 255}, opt_camObj__areaLim),
	CVTRACK::camObjTracker(false, camID[1], prefSize[0], prefSize[1], false, true, {-1, 6}, {0, 0, 255}, {179, 9, 255}, opt_camObj__areaLim)
};

TCPTS::TCPThreadedServer<std::vector<uint8_t>, uint8_t*> serverObj(false, TCPThreadedServer__DEFAULT_PORT, &mtx_cout);

// Two_cam_triangle header class initialisation
std::vector<pos2d<double>> camPosition = {{0, 0}, {150, 0}};
std::vector<double> camAng_offs = {90, 120};
camTriangle camTri(camPosition, camAng_offs);



// bool pigpioInitia = false;
int pin_ledRelay = 23;

#if takePerf
//main thread performance measurement
getPerf perfObj[3] {
	{" main thread"},
	{" sub thread[0]"},
	{" sub thread[1]"}
};
#endif

opencv_recorder recObj;


#if takePerf
//sub thread(s) performance measurements

std::mutex mtx_perfObj_threads[2];
// getPerf perfObj[2] {
// 	{" sub-thread[0]"},
// 	{" sub-thread[1]"}
// };
#endif // takePerf

std::mutex mtx_cout;


int subMenuPos[2] = {20, 0};


TUI::termMenu menu__config_options({}, false);
TUI::termMenu menu__init_options({}, false);

TUI::termMenu menu_group__main({
	{" [0]    Intro", 0, 0, '0', HW_option1_intro},

	{" [2]    Main", 0, 2, '2', HW_option0},
	{" [3]    Tracking-telemetry", 0, 3, '3', HW_option3},
	{" [4]    Orient", 0, 4, '4', HW_option5_orient},
	{" [t]    Terminal", 0, 5, 't', HW_option6_terminal},
	
	{" [c]    Calibrate", 0, 7, 'c', HW_group__calibrate},
	
	{" [o]    options", 0, 9, 'o', HW__config_options},
	{" [i]    init-status", 0, 10, 'i', HW__init_options},

	{" [esc]  Exit",	    0, 12,  27, TUI::DEDICATED__exitDriver}
});
TUI::termMenu menu_group__calibrate({
	{" [0]    Webcam object tracking HSV-values", 0, 0, '0', HW_option2},
	{" [1]    Servo motor drift trend-solution"	, 0, 1, '1', HW_option4},
	{" [2]    Accelerometer drift measurement"	, 0, 2, '2', HW_option4_accelOffsets},

	{" [esc] Exit", 0, 4, 27, TUI::DEDICATED__exitDriver}
});


TUI::termMenu opt6_startMenu({
	{" [1]    Control Panel", 0, 0, '1', HW_option6_control_panel},
	{" [2]    Raw input", 	0, 1, '2', HW_option6_rawTerminal},
	{" [3]    Run file", 		0, 2, '3', HW_option6_runFromFile},

	{" [s]    Settings", 		0, 4, 's', HW_option6_settings},

	{" [esc]  Back", 			0, 6, 27, TUI::DEDICATED__exitDriver},
	{" [e]    Exit", 			0, 7, 'e', TUI::DEDICATED__exitDriver}
});
TUI::termMenu opt6_control_panel({
	{formatNumber("x:",8,0,"left"), 1, 1, 'x'},
	{formatNumber("y:",8,0,"left"), 3, 1, 'y'},
	{formatNumber("z:",8,0,"left"), 5, 1, 'z'},

	{formatNumber("a:",8,0,"left"), 1, 3, 'a'},
	{formatNumber("B:",8,0,"left"), 3, 3, 'b'},
	{formatNumber("Y:",8,0,"left"), 5, 3, 'y'},

	{" back", 1, 5, 27},
	{" enter", 5, 5, 10}
}, false);



_initClass::_initClass(
	int (*_init_function)(_initClass_dataStruct*),
	int (*_close_function)(_initClass_dataStruct*),
	bool _init
): _init_func(_init_function), _close_func(_close_function) {
	assert(_init_func || _close_func); //both init and close function can't be invalid (otherwise this serves no purpose)
	if(_init) {
		this->call_init();
	}
}
_initClass::~_initClass() {
	int _returnCode = 0;
	_returnCode = this->call_closing();
	if(_returnCode==0) {
		this->_init = false;
	}
}
int 	_initClass::call_init() {
	int _returnCode = 0;
	// if(this->_init) return -69;
	if(_init_func) _returnCode = _init_func(&this->_callData);
	if(_returnCode==0) this->_init = true;
	return _returnCode;
}
int 	_initClass::call_closing() {
	int _returnCode = 0;
	if(!this->_init) return -69;
	if(_close_func) _returnCode = _close_func(&this->_callData);
	if(_returnCode==0) this->_init = false;
	return _returnCode;
}
const bool _initClass::isInit() {
	return this->_init;
}
std::string _initClass::get_callMsg() {
	return this->_callData._message;
}


DIY::typed_dict<std::string, _initClass> _init_status({
	{"pca", 			_initClass(_init__pca, _close__pca, false)},
	{"camObj", 			_initClass(_init__camObj, _close__camObj, false)},
	{"pigpio", 			_initClass(_init__pigpio, _close__pigpio, false)},
	{"opencv recorder", _initClass(_init__opencv_recorder, _close__opencv_recorder, false)},
	{"orientObj", 		_initClass(_init__orientObj, _close__orientObj, false)},
	{"serverObj",		_initClass(_init__serverObj, _close__serverObj, false)}
});


void simplified_init() {

	if(!_init_status.get("pca").isInit() && _init_status.get("pca").call_init()) {
		// std::cout << "ERROR: could not initialise pca library: "<< _init_status.get("pca").get_callMsg() << std::endl;
		ANSI_mvprint(0, 0, "ERROR: could not initialise pca library: "+_init_status.get("pca").get_callMsg(), true, "abs", "rel");
	}
	if(!_init_status.get("camObj").isInit() && _init_status.get("camObj").call_init()) {
		// std::cout << "ERROR: could not initialise camObj objects: "<<_init_status.get("camObj").get_callMsg() << std::endl;
		ANSI_mvprint(0, 0, "ERROR: could not initialise camObj objects: "+_init_status.get("camObj").get_callMsg(), true, "abs", "rel");
	}

	if(recordFrames) {
		// recObj = opencv_recorder("Hexclaw_cameraFeeds", prefSize[0]*2, prefSize[1]*2, 15);
		if(!_init_status.get("opencv recorder").isInit() && _init_status.get("opencv recorder").call_init()) {
			// std::cout << "ERROR: could not initialise opencv recorder object:"<<_init_status.get("opencv recorder").get_callMsg() << std::endl;
			ANSI_mvprint(0, 0, "ERROR: could not initialise opencv recorder object: "+_init_status.get("opencv recorder").get_callMsg(), true, "abs", "rel");
		}
	}
	// std::cout << "Initialising pigpio..."<<std::endl;
	if(!_init_status.get("pigpio").isInit() && _init_status.get("pigpio").call_init()) {
		// std::cout << "ERROR: could not initialise pigpio library: "<<_init_status.get("pigpio").get_callMsg() << std::endl;
		ANSI_mvprint(0, 0, "ERROR: could not intialise pigpio library: "+_init_status.get("pigpio").get_callMsg(), true, "abs", "rel");
	}
	if(!_init_status.get("orientObj").isInit() && _init_status.get("orientObj").call_init()) {
		ANSI_mvprint(0, 0, "ERROR: could not initialise nodemcu_orient object \"orientObj\": "+_init_status.get("orientObj").get_callMsg(), true, "abs", "rel");
	}
}

int _init__pca(_initClass_dataStruct *_passData) {
#if _MACHINE__RPI_MAIN
	bool init_success = false;
	try {
		init_success = pca.init();
	} catch (const std::system_error& e) {
		_passData->_message = e.what();
		// std::cout << e.what() << std::endl;
	}
	if(!init_success) return 1;
	_passData->_message = "";
	pca.set_pwm_freq(50.0);
	return 0;
#else
	_passData->_message = "NOTE: _init__pca: _MACHINE__RPI_MAIN==false";
	// return 1;
#endif

	return 0;
}
int _init__camObj(_initClass_dataStruct *_passData) {
	// camObj = std::vector<IR_camTracking>(2);
	camObj = std::vector<CVTRACK::camObjTracker>{
		CVTRACK::camObjTracker(false, camID[0], prefSize[0], prefSize[1], false, _CONFIG_OPTIONS.get("camObj_useThreads"), {-1, 6}, {0, 0, 255}, {179, 9, 255}, opt_camObj__areaLim),
		CVTRACK::camObjTracker(false, camID[1], prefSize[0], prefSize[1], false, _CONFIG_OPTIONS.get("camObj_useThreads"), {-1, 6}, {0, 0, 255}, {179, 9, 255}, opt_camObj__areaLim)
	};
	

	// camObj.reserve(2);
	// IR_camTracking* camObj_ptr = nullptr;
	CVTRACK::camObjTracker* camObj_ptr = nullptr;
	try {
		// camObj.push_back(IR_camTracking(0, prefSize[0], prefSize[1], _CONFIG_OPTIONS.get("useAutoBrightness"), _CONFIG_OPTIONS.get("displayToWindow"), _CONFIG_OPTIONS.get("takeCVTrackPerf")));
		camObj_ptr = &camObj.at(0);
		new (camObj_ptr) CVTRACK::camObjTracker(false, camID[0], prefSize[0], prefSize[1], false, _CONFIG_OPTIONS.get("camObj_useThreads"), {-1, 6}, HW_HSV[0], HW_HSV[1], opt_camObj__areaLim);
		camObj[0].setConsolePrintMutex(&mtx_cout);
		if(!camObj.at(0).init()) {
			throw std::runtime_error("_init__camObj[0]: init() failed");
		}
		
		_passData->_message = "";
	} catch (const std::exception& e) {
		_passData->_message = 	"cam0:"+std::string(e.what());
		return 1;
	}
	try {
		// camObj.push_back(IR_camTracking(2, prefSize[0], prefSize[1], _CONFIG_OPTIONS.get("useAutoBrightness"), _CONFIG_OPTIONS.get("displayToWindow"), _CONFIG_OPTIONS.get("takeCVTrackPerf")));
		camObj_ptr = &camObj.at(1);
		new (camObj_ptr) CVTRACK::camObjTracker(false, camID[1], prefSize[0], prefSize[1], false, _CONFIG_OPTIONS.get("camObj_useThreads"), {-1, 6}, HW_HSV[0], HW_HSV[1], opt_camObj__areaLim);
		camObj[1].setConsolePrintMutex(&mtx_cout);
		if(!camObj.at(1).init()) {
			throw std::runtime_error("_init__camObj[1]: init() failed");
		}
		
		_passData->_message = "";
	} catch (const std::exception& e) {
		_passData->_message = "cam1:"+std::string(e.what());
		return 1;
	}

	camObj[0].setAreaLim(opt_camObj__areaLim);
	camObj[1].setAreaLim(opt_camObj__areaLim);

	return 0;
}
int _init__pigpio(_initClass_dataStruct *_passData) {
#if _MACHINE__RPI_MAIN
	// std::cout << "before pigpio init..."<<std::endl;
	try {
		if(gpioInitialise() < 0) {
			std::cout << "_init__pigpio(): pigpio \"gpioInitialise()\" failed."<<std::endl;
			return -1;
		}
		gpioSetMode(pin_ledRelay, PI_OUTPUT);
		gpioWrite(pin_ledRelay, 1);
		_passData->_message = "";
	} catch(const std::exception& e) {
		_passData->_message = e.what();
		return 1;
	}

	return 0;
#else
	_passData->_message = "_init__pigpio: _MACHINE__RPI_MAIN=false";
	return 1;
#endif

}
int _init__opencv_recorder(_initClass_dataStruct *_passData) {
	try {
		recObj = opencv_recorder("Hexclaw_cameraFeeds", prefSize[0]*2, prefSize[1]*2, 15);
	} catch (const std::exception &e) {
		_passData->_message = e.what();
		return 1;
	}
	return 0;
}
int _init__orientObj(_initClass_dataStruct *_passData) {
	int init_success = -1;
	try	{
		init_success = orientObj.connectObj.init();
		_passData->_message = "";
	}
	catch(const std::exception& e)
	{
		_passData->_message = e.what();
		return 1;
	}
	bool _connected = false;
	for(size_t i=0; i<3; i++) {
		try {
			orientObj.update(false);
			_connected = true;
			_passData->_message = "";
			break;
		}
		catch(const std::exception& e) {

		}
	}
	if(!_connected) {
		_passData->_message = "failed to establish connection.";
		init_success = -1;
	}

	return init_success;
}
int _init__serverObj(_initClass_dataStruct *_passData) {
	// serverObj = TCPTS::TCPThreadedServer<std::vector<uint8_t>>(true, DEFAULT__PORT);
	try {
		if(!serverObj.start()) {
			throw std::runtime_error("::start()==false");
		}
	} catch (const std::exception& e) {
		_passData->_message = e.what();
		return 1;
	}
	return 0;
}
int _close__pca(_initClass_dataStruct *_passData) {
	(&pca)->~PCA9685();
	new (&pca) PiPCA9685::PCA9685(false);

	return 0;
}
int _close__camObj(_initClass_dataStruct *_passData) {
	if(_init_status.get("camObj").isInit()) {
		(&camObj[0])->~camObjTracker();
		(&camObj[1])->~camObjTracker();
	}
	
	return 0;
}
int _close__pigpio(_initClass_dataStruct *_passData) {
#if _MACHINE__RPI_MAIN
	gpioTerminate();
#else

#endif
	return 0;
}
int _close__opencv_recorder(_initClass_dataStruct *_passData) {
	(&recObj)->~opencv_recorder();
	return 0;
}
int _close__orientObj(_initClass_dataStruct *_passData) {
	(&orientObj)->~nodemcu_orient();
	new (&orientObj) nodemcu_orient("192.168.1.117", DEFAULT__PORT, false);
	return 0;
}
int _close__serverObj(_initClass_dataStruct *_passData) {
	(&serverObj)->~TCPThreadedServer<std::vector<uint8_t>, uint8_t*>();
	new (&serverObj) TCPTS::TCPThreadedServer<std::vector<uint8_t>, uint8_t*>(false, TCPThreadedServer__DEFAULT_PORT, &mtx_cout);
	return 0;
}
