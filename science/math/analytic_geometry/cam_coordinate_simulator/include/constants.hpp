#pragma once
#ifndef HPP__CAM_COORDINATE_SIMULATOR__CONSTANTS
#define HPP__CAM_COORDINATE_SIMULATOR__CONSTANTS

#include "includes.hpp"

#ifndef M_PI
#define M_PI  3.1415926535
#endif


#ifndef toDEGREES
#define toDEGREES(RAD) ((RAD)*180/M_PI)
#endif
#ifndef toRADIANS
#define toRADIANS(DEG) ((DEG)*M_PI/180.0)
#endif




#define DEFAULT__WINDOW_WIDTH   1200
#define DEFAULT__WINDOW_HEIGHT   700




#endif //HPP__CAM_COORDINATE_SIMULATOR__CONSTANTS