#pragma once
#ifndef HPP__HEXCLAW_MAIN__DATA_PLOTTER__GLOBAL_VARIABLES
#define HPP__HEXCLAW_MAIN__DATA_PLOTTER__GLOBAL_VARIABLES

#include "includes.hpp"


extern GUINC::imguiwin guiwin;

extern DIY_KBH::keyBind_handler keyBinds;
extern pressed_key__struct guiKeys;

extern int keyAction_mouseLeft;
/// @brief width-height size of the total program window [px]
extern pos2d<int> DIM_PROGRAM__WIN;
/// @brief width-height size of the motor selection panel
extern pos2d<int> DIM_PROGRAM__MOTOR_SELECT_PANEL;


#endif //HPP__HEXCLAW_MAIN__DATA_PLOTTER__GLOBAL_VARIABLES