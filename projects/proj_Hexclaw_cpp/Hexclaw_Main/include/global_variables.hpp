
#pragma once
#ifndef HPP_HEXCLAW_GLOBAL
#define HPP_HEXCLAW_GLOBAL

// #include <includes.hpp>

#include <constants.hpp>
#include <string>
#include <iostream>

#if _MACHINE__RPI_MAIN
#include <PiPCA9685/PCA9685.h>
#endif

#include <wirelessCOM.hpp>
#include <IR_camTrack.hpp>
#include <terminalMenu.hpp>
#include <diy_dictionary.hpp>
#include <opencv/recordVideo/recordFrames.hpp>
#include <TwoCamCoordinate.hpp>
#include "includes.hpp"


/**
 * @brief settings dict for config options relating to different options within the separate class objects e.t.c e.t.c
 * 
 */
inline DIY::typed_dict<std::string, bool> _CONFIG_OPTIONS({
	{"useAutoBrightness", 	true},
	{"takeCVTrackPerf",		true},
	{"displayToWindow",		true},
	{"camObj_useThreads",	true}
});

/// @brief absolute path of the current directory for this program
extern std::string absPath;

// #if _MACHINE__RPI_MAIN
/// @brief pca9685-board libraries obbject
extern PiPCA9685::PCA9685 pca;
// #endif


extern bool hardExit;

// IK related: ik calc variable declaration

/// @brief container array for the current/old servo motors angles
extern servo_angles_6DOF current_q;
// extern float current_q[6];

/// @brief new servo rotations angles
extern servo_angles_6DOF new_q;
// extern float new_q[6];

/**
 * `{yaw, pitch, roll}` variables:
 * unit: degrees
 * */
extern pos3d<float> orient;
// extern float orient[3];

/// @brief `{x, y, z}` coordinate array container
extern pos3d<float> PP;
// extern float PP[3];

/// @brief new axis value scalars
extern pos3d<float> axisScal;
// extern float axisScal[3];

/// @brief new axis value offset
extern pos3d<float> axisOffset;
// extern float axisOffset[3];

/// @brief new axis filter
extern pos3d<float> axisFilter;
// extern float axisFilter[3];

/// @brief raw tilt values from accelerometer readings
extern float pitch;
extern float roll;

/// @brief filtered tilt values from accelerometer readings
extern float Pitch;
extern float Roll;

/// @brief Wireless nodemcu udp COM header class object
extern nodemcu_orient orientObj;
//nodemcu_orient orientObj(orient, "192.168.1.177");


/// @brief prefered image size to resize/convert/use for all images
extern pos2d<int> prefSize;
// extern int prefSize[2];


extern bool displayTFT;


extern std::vector<std::vector<int>> HW_HSV;

// extern const char* window_name;
extern std::string window_name;

/// @brief ID of the camera for camObj
extern std::vector<int> camID;

/// Cam object tracking related

// extern std::vector<IR_camTracking> camObj;
extern std::vector<CVTRACK::camObjTracker> camObj;


extern TCPTS::TCPThreadedServer<std::vector<uint8_t>, uint8_t*> serverObj;

/// Two_cam_triangle header class initialisation

extern std::vector<pos2d<double>> camPosition;
extern std::vector<double> camAng_offs;
extern camTriangle camTri;


// extern bool pigpioInitia;
extern int pin_ledRelay;

#if takePerf
//main thread performance measurement
extern getPerf perfObj[3];
#endif

extern opencv_recorder recObj;



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
inline void lock_cout(
	std::mutex &coutMutex,
	std::string toPrint,
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

#if takePerf
//sub thread(s) performance measurements


extern std::mutex mtx_perfObj_threads[2];
// getPerf perfObj[2] {
// 	{"sub-thread[0]"},
// 	{"sub-thread[1]"}
// };
#endif // takePerf

extern std::mutex mtx_cout;



/// Terminal menu related

extern TUI::termMenu menu__config_options;
extern TUI::termMenu menu__init_options;

extern TUI::termMenu menu_group__main;
extern TUI::termMenu menu_group__calibrate;


extern TUI::termMenu opt6_startMenu;
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
extern TUI::termMenu opt6_control_panel;


struct _initClass_dataStruct {
	std::string _message = "";

};

class _initClass {
	private:
	bool _init = false;
	int (*_init_func)(_initClass_dataStruct*)	= nullptr;
	int (*_close_func)(_initClass_dataStruct*)	= nullptr;

	_initClass_dataStruct _callData;

	public:
	_initClass() {};
	/**
	 * @brief initialise _initClass object
	 * @param _init_function pointer to the dedicated initialisation function
	 * @param _close_function pointer to the closing function
	 * @param _init whether to call the initialisation function during constructor call
	 */
	_initClass(int (*_init_function)(_initClass_dataStruct*), int (*_close_function)(_initClass_dataStruct*)=nullptr, bool _init=false);
	~_initClass();

	/**
	 * @brief call the stored initialiser function
	 * @return integer return code of the initialiser function [`0` - successful, else - error]
	 * @note no initialiser function could be given (setting it to nullptr) whilst still calling this to "indicate" to this object that calling close is fine
	 */
	int call_init();
	/**
	 * @brief call the stored closing function.
	 * @return integer return code of the intialiser function [`0` - successful, else - error]
	 */
	int call_closing();
	/**
	 * @brief get whether the init variable is set to true or not
	 * @return local _init value
	 */
	const bool isInit();
	/**
	 * @brief get the call message from init or close function/method
	 * @return std::string of the message.
	 */
	std::string get_callMsg();
};


/**
 * dictionary to hold the init/close info and operations for whether the related object/topic is to be initialised/closed
 */
extern DIY::typed_dict<std::string, _initClass> _init_status;


void simplified_init();

/**
 * initialise pca board class object `pca`
 * @return integer code for whether it was successful [`0`] or an error has occurred [`!=0`]
 */
int _init__pca(_initClass_dataStruct *_passData);
/**
 * initialise camObject(s)
 * @return int code for whether it was successful [`0`] or not [`!=0`]
 */
int _init__camObj(_initClass_dataStruct *_passData);
/**
 * initialise rpi4b gpio library
 */
int _init__pigpio(_initClass_dataStruct *_passData);
int _init__opencv_recorder(_initClass_dataStruct *_passData);
int _init__orientObj(_initClass_dataStruct *_passData);
int _init__serverObj(_initClass_dataStruct *_passData);
/**
 * call closing operations related to pca object
 * 
 */
int _close__pca(_initClass_dataStruct *_passData);
int _close__camObj(_initClass_dataStruct *_passData);
int _close__pigpio(_initClass_dataStruct *_passData);
int _close__opencv_recorder(_initClass_dataStruct *_passData);
int _close__orientObj(_initClass_dataStruct *_passData);
int _close__serverObj(_initClass_dataStruct *_passData);

#endif
