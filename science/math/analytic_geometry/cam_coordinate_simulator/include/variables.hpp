#pragma once
#ifndef HPP__CAM_COORDINATE_SIMULATOR__VARIABLES
#define HPP__CAM_COORDINATE_SIMULATOR__VARIABLES

#include "includes.hpp"
#include "constants.hpp"

/// @brief 2d dimension of the program window
extern pos2d<int> dim__programWindow;
/// @brief 2d dimension of the drawing area
extern pos2d<int> dim__draw_area;

/// @brief 2d position of the drawing area's  top left corner
extern pos2d<int> pos__draw_area;


extern GUINC::imguiwin guiwin;

extern DIY_KBH::keyBind_handler keyBinds;
extern pressed_key__struct guiKeys;


#endif //HPP__CAM_COORDINATE_SIMULATOR__VARIABLES