
#pragma once
#ifndef HPP_HEXCLAW_OPTIONS
#define HPP_HEXCLAW_OPTIONS


#include "motion_control/motion_profiles.hpp"
#include "constants.hpp"
#include "includes.hpp"
#include "global_variables.hpp"
#include <stringToFunc.hpp>
#include <createTable.hpp>


labeledFunction hexclaw_cmdArgs{};
createTable cmdArgs_output_table;

void printFuncLabel(std::string functionName) {
	int termDim[2];
	getTermSize(termDim[0], termDim[1]);
	// std::cout << std::endl << std::string(termDim[0], '-')<<std::endl;
	// std::cout << functionName << std::endl;
	// std::cout << std::string(termDim[0], '-')<<std::endl;
	ANSI_mvprint(0, 0, std::string(termDim[0], '-'));
	ANSI_mvprint(0, 0, functionName, true, "abs", "rel");
	ANSI_mvprint(0, 0, std::string(termDim[0], '-'), true, "abs", "rel");
}

/// @brief display all the command line argument flags with their descriptions
void cmdArgs_info() {
    cmdArgs_output_table = createTable(3, hexclaw_cmdArgs.getDescriptions().size());

    for(size_t i=0; i<hexclaw_cmdArgs.getDescriptions().size(); i++) {
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getNames()[i][0], 0, i);
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getNames()[i][1], 1, i);
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getDescriptions()[i], 2, i);
    }

    cmdArgs_output_table.strExport("\n",false,"\n","  ");
    std::cout << "\t"<<cmdArgs_output_table.exportStr << std::endl;
	hardExit = true;
}


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

#include "opt6_terminal.hpp"

/**
 * @brief setup for HW command line arguemtns: (there's a better way. I just haven't gotten around to using it)
*/
void HW_setup_options() {
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-h", "--help"}, cmdArgs_info, "show help message with info on flags");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-i", "--intro"}, HW_option1_intro, "run only servo intro movement");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-c", "--cal"}, HW_option2, "calibrate/setup \"Hue Saturation Value\" -values");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-0", "-m", "--main"}, HW_option0, "run main/default version with servo control, opencv based tracking");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-3"}, HW_option3, "track and display with opencv but don't send control movements to robot motors");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-4"}, HW_option4, "calibrate servo motors by reading error rate and solving reverse correction function coefficients");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-5"}, HW_option5_orient, "only read and send end-effector orientation from nodemcu device to hexclaw robot-arm");
	hexclaw_cmdArgs.add_func(std::vector<std::string>{"-6", "-t", "--terminal"}, HW_option6_terminal, "control robot with terminal position inputs");

}






#endif
