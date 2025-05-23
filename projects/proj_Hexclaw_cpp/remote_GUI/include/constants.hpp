
#pragma once
#ifndef HPP__HEXCLAW_REMOTE_GUI__CONSTANTS
#define HPP__HEXCLAW_REMOTE_GUI__CONSTANTS


#define WIN_WIDTH   600
#define WIN_HEIGHT  600

#define WIN_INPUT_IK_WIDTH  350
#define WIN_INPUT_IK_HEIGHT 100
#define WIN_INPUT_IK_POS  [0, 0]

#define WIN_INPUT_SETTINGS_WIDTH    200
#define WIN_INPUT_SETTINGS_HEIGHT   300
#define WIN_INPUT_SETTINGS_POS  [WIN_INPUT_IK_POS[0]+WIN_INPUT_IK_WIDTH, 0]

#define WIN_OUTPUT_ANGLES_WIDTH     350
#define WIN_OUTPUT_ANGLES_HEIGHT    100
#define WIN_OUTPUT_ANGLES_POS   [0, WIN_INPUT_IK_POS[1]+WIN_INPUT_IK_HEIGHT]

#define WIN_OUTPUT_FK_WIDTH     350
#define WIN_OUTPUT_FK_HEIGHT    100
#define WIN_OUTPUT_FK_POS   [0, WIN_OUTPUT_ANGLES_POS[1]+WIN_INPUT_IK_HEIGHT]

inline size_t __PROGRAM_FPS = 500;

//number of frames to wait after calling a boolean via key shortcut
#define LIM_SHORTCUT_KEYS 100   //50 //8 for ~60fps

#define MAX_HISTORY_SIZE 120 //200 // 10 for ~60fps




inline int __CHOSEN_LAYOUT = 0;

/**
 * ID for how the program was opened/started.
 *  `0` - terminal mode: was opened from the terminal.
 *  `1` - application mode: opened from an application shortcut.
 */
inline int __OPEN_MODE = 0;



#endif