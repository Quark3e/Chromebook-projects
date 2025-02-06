
#pragma once
#ifndef HPP_HEXCLAW_OPTIONS
#define HPP_HEXCLAW_OPTIONS


#include "motion_control/motion_profiles.hpp"
#include "constants.hpp"
#include "includes.hpp"
#include "global_variables.hpp"
#include <stringToFunc.hpp>
#include <createTable.hpp>


extern labeledFunction hexclaw_cmdArgs;
extern createTable cmdArgs_output_table;

void printFuncLabel(std::string functionName);
/// @brief display all the command line argument flags with their descriptions
void cmdArgs_info();


/// @brief run intro servo movement
void HW_option1_intro();
/// @brief calibrate HSV values
void HW_option2();

/// @brief mode0: run main/default option
void HW_option0();
/// @brief mode 1: track with opencv and display result but don't control/move servo motors
void HW_option3();
/// @brief mode 2: calibrate servo motors by sending rotation signals to the motors individually and compare them with statically recorded tilt readings
void HW_option4();
/// @brief mode 3: orient mode. Only read and send end-effector orientation from nodemcu device
void HW_option5_orient();
/**
 * mode terminal: control servo motor with terminal input by the user.
 * 
 * ### Functionality:
 * - preset menu: changeable settings like different tweakings for `HW_KINEMATICS::getAngles()`, default values, whether to exit or keep
 * going if value is not found, whether to use `findClosestOrient()`
 * #### input:
 * - run commands from given file:
 * 	-- position commands
 * 	-- orient commands
 * 	-- position commands with given orient
 * - give 3d coordinates which the robot end-effector will try to reach:
 * - give orient which the robot will try to go to for current orientation
 * - give orient which the robot will try to go to from given position
*/
void HW_option6_terminal();

void HW_option6_control_panel();
void HW_option6_rawTerminal();
void HW_option6_settings();
void HW_option6_runFromFile();



/**
 * @brief setup for HW command line arguemtns: (there's a better way. I just haven't gotten around to using it)
*/
void HW_setup_options();




#endif
