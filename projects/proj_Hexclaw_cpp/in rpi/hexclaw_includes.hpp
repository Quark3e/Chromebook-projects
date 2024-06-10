
#pragma once
#ifndef HPP_HEXCLAW_HEADERS
#define HPP_HEXCLAW_HEADERS



// default/basic headers/includes
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream> //read and write to file (hsv_settings.dat)
#include <algorithm> //ex. find() and erase()
#include <time.h>
#include <stdint.h>
#include <vector>

// path related headers
#include <cstring>
#include <unistd.h>
#include <libgen.h>
#include <linux/limits.h>

// opencv/image tracking
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>


// PCA9685 communication
#include <PiPCA9685/PCA9685.h>

// RPi specific functions
#include <pigpio.h>

// #if useThreads
#include <thread>
#include <mutex>
// #endif


#include <curses.h>
#include <menu.h>
#include <cstdlib>

// imgui related stuff
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_allegro5.h>


// #include "../../../teststuff/cpp/useful/wirelessCOM.hpp"
// #include "HW_headers/IR_camTrack.hpp"
// #include "HW_headers/integ_TFTdisp.hpp"
#include "HW_headers/motion_control/motion_profiles.hpp"

#include <HC_useful/wirelessCOM.hpp>
#include <HC_useful/IR_camTrack.hpp>

#include <HC_useful/createTable.hpp>
#include <HC_useful/useful.hpp>
#include <HC_useful/stringToFunc.hpp>
#include <HC_useful/terminalMenu.hpp>

#include <HC_useful/getPerformance.hpp>

#include <HC_useful/diy_dictionary.hpp>

#include "../../../teststuff/cpp/two_cam_coordinate/two_cam_coordinate.hpp"
#include "../../../teststuff/cpp/opencv/recordVideo/recordFrames.hpp"
// #include "../../../teststuff/cpp/basic/Performance/getPerformance.hpp"

#include "HW_headers/servo_control.hpp"
#include "IK_header.h"
#endif