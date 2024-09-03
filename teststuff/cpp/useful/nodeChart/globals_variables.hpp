
#pragma once
#ifndef HPP__GUI_NODECHART__CONSTANTS
#define HPP__GUI_NODECHART__CONSTANTS

#define _DEBUG false

#include "globals_includes.hpp"


extern ImVec2 dim__main;
extern ImVec2 dim__menu__node_detail;

extern bool lockMove_screen;
extern bool lockMove_node;

//state checks
/**
 * Left_mouse click decay lock
 */
extern int mouseClick_left;
/**
 * Right_mouse click decay lock
 */
extern int mouseClick_right;

/**
 * Global variable to hold the current mouse action:
 *  `-1` default: no action
 *  ` 0` drag mouse: move screen
 *  ` 1` left mouse: move node
 *  ` 2` left mouse: drag node connection
 */
extern int mouseAction;


/// @brief rate at which the mouse lock value decays per iteration/frame 
extern int mouseTimer_decay;



#endif