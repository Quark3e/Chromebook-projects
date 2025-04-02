/**
 * @file hw_option_4.cpp
 * @author your name (you@domain.com)
 * @brief Calibrate servo motors by reading error rate and creating correction functions.
 * @version 0.1
 * @date 2025-02-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#include "hw_options.hpp"


TUI::termMenu menu__calibrateMotor_main({
	{"Select a joint/motor to calibrate", 0, 0, -1}, {"Error regr", 1, 0, -1}, {"Regr solution", 2, 0, -1},
	
	{"q[0]", 0, 2, '0'}, {"", 1, 2, -1},
	{"q[1]", 0, 3, '1'}, {"", 1, 3, -1},
	{"q[2]", 0, 4, '2'}, {"", 1, 4, -1},
	{"q[3]", 0, 5, '3'}, {"", 1, 5, -1},
	{"q[4]", 0, 6, '4'}, {"", 1, 6, -1},
	{"q[5]", 0, 7, '5'}, {"", 1, 7, -1},

	{"back", 0, 9, 27}
}, false);

TUI::termMenu menu__calibrateMotor_angles({


	{"  0°", 1,  2, -1}, {"   ", 2,  2, -1},
	{" 10°", 1,  3, -1}, {"   ", 2,  3, -1},
	{" 20°", 1,  4, -1}, {"   ", 2,  4, -1},
	{" 30°", 1,  5, -1}, {"   ", 2,  5, -1},
	{" 40°", 1,  6, -1}, {"   ", 2,  6, -1},
	{" 50°", 1,  7, -1}, {"   ", 2,  7, -1},
	{" 60°", 1,  8, -1}, {"   ", 2,  8, -1},
	{" 70°", 1,  9, -1}, {"   ", 2,  9, -1},
	{" 80°", 1, 10, -1}, {"   ", 2, 10, -1},
	{" 90°", 1, 11, -1}, {"   ", 2, 11, -1},
	{"100°", 1, 12, -1}, {"   ", 2, 12, -1},
	{"110°", 1, 13, -1}, {"   ", 2, 13, -1},
	{"120°", 1, 14, -1}, {"   ", 2, 14, -1},
	{"130°", 1, 15, -1}, {"   ", 2, 15, -1},
	{"140°", 1, 16, -1}, {"   ", 2, 16, -1},
	{"150°", 1, 17, -1}, {"   ", 2, 17, -1},
	{"160°", 1, 18, -1}, {"   ", 2, 18, -1},
	{"170°", 1, 19, -1}, {"   ", 2, 19, -1},
	{"180°", 1, 20, -1}, {"   ", 2, 20, -1},

	{"Solve",1, 21, -1},
	
	{"back", 1, 23, 27},
	{"exit", 1, 24, 27}
}, false);

void tempFunc_updateOrient() {
	orientObj.update(false);
	menu__calibrateMotor_angles.rename_opt(1, 1, formatNumber(orientObj.pitch, 7, 1));
	menu__calibrateMotor_angles.rename_opt(2, 1, formatNumber(orientObj.roll, 7, 1));
}

void HW_option4() {
	// simplified_init();
	if(!(_init_status.get("pca").isInit() || _init_status.get("orientObj").isInit())) {
		if(!_init_status.get("pca").isInit()) 		ANSI_mvprint(0, 0, "ERROR: pca    	 has not been initialised (required): "+_init_status.get("pca").get_callMsg(), true, "abs", "rel");
		if(!_init_status.get("orientObj").isInit()) ANSI_mvprint(0, 0, "ERROR: orientObj has not been initialised (required): "+_init_status.get("orientObj").get_callMsg(), true, "abs", "rel");
		for(int _=0; _<3; _++) {
			// std::cout << " ."; std::cout.flush();
			ANSI_mvprint(0, -1, " .", true, "rel", "rel");
			std::this_thread::sleep_for(std::chrono::milliseconds(1'000));
		}
		return;
	}
	
	bool runMain = true;
	while(runMain) {
		printFuncLabel(" Running: opt4: Calibrate servo motors");
		ANSI_mvprint(0, 0, "Select a joint/motor to calibrate", true, "abs", "rel");

		pos2d<int> pressed_pos = menu__calibrateMotor_main.driver(0, 5, 1, false, TUI::termMenu_nullFunc_void__, false);

		if(pressed_pos==pos2d<int>(0, 9)) break;
		if(pressed_pos.inRegion({0, 2}, {0, 7})) { //check if pressed_pos is in region of the selectable motor options
			ANSI_mvprint(0, 2, "Calibrating motor: q["+formatNumber(pressed_pos[1]-2)+"]", true, "abs", "abs", true);
			std::this_thread::sleep_for(std::chrono::seconds(1));
			
			// calibrate motor
			bool runAngles = true;
			while(runAngles) {
				ANSI_mvprint(0, 0, "Select an angle to calibrate", true, "abs", "abs", true);
				pos2d<int> pressed_pos_angles = menu__calibrateMotor_angles.driver(0, 5, 1, false, tempFunc_updateOrient, false);
				if(pressed_pos_angles==pos2d<int>(0, 9)) break;
				if(pressed_pos_angles.inRegion({1, 2}, {1, 20})) { //check if pressed_pos is in region of the selectable motor options
					if(_init_status.get("pca").isInit()) {
						// set motor to angle
						servo_angles_6DOF newAngles(0);
						int motorIdx = pressed_pos[1]-2;
						float angle = (pressed_pos_angles[1]-2)*10.0f;
						// float rad = toRadians(angle);
						newAngles[motorIdx] = angle;
						if(_init_status.get("pca").isInit()) sendToServo(&pca, newAngles, current_q, false);
					}
					else {
						// ANSI_mvprint(0, 0, "ERROR: pca has not been initialised: "+_init_status.get("pca").get_callMsg(), true, "abs", "rel");
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
					// orientObj.update(false);

					menu__calibrateMotor_angles.rename_opt(2, pressed_pos_angles[1], formatNumber(orientObj.pitch, 7, 1));

					/// Manual input of angles
					// std::string inpValue = menu__calibrateMotor_angles.termInput(0);
					// if(inpValue=="exit") return;
					// else if(inpValue=="back") break;
					// else {
					// 	menu__calibrateMotor_angles.rename_opt(2, pressed_pos[1], inpValue);
					// }
				}
				else if(pressed_pos_angles==pos2d<int>(1, 21)) { // "Solve" button pressed
					// solve servo drift regression
					std::vector<float> refrAngles(19, 0);
					std::vector<float> readAngles(19, 0);
					for(size_t i=0; i<19; i++) {
						refrAngles[i] = i*100;
						try {
							readAngles[i] = std::stof(menu__calibrateMotor_angles.getCellName(2, i+2));
						}
						catch(const std::invalid_argument& e) {
							ANSI_mvprint(0, 0, "ERROR: invalid input: \""+menu__calibrateMotor_angles.getCellName(2, i+2)+"\" "+std::string(e.what()), true, "abs", "rel");
							std::this_thread::sleep_for(std::chrono::seconds(1));
							readAngles[i] = 0;
							// break;
						}
					}

					ANSI_mvprint(0, 0, "refrAngles: "+formatVector(refrAngles, 6, 1), true, "abs", "rel");
					ANSI_mvprint(0, 1, "readAngles: "+formatVector(readAngles, 6, 1), true, "abs", "rel");

					std::vector<float> coeffs 		= HW_KINEMATICS::solveServoDriftRegression(refrAngles, readAngles, 2);
					std::vector<float> coeffs_sol	= HW_KINEMATICS::solveServoDriftRegression(readAngles, refrAngles, 2);
					
					menu__calibrateMotor_main.rename_opt(1, pressed_pos[1], formatVector(coeffs, 6, 2));
					menu__calibrateMotor_main.rename_opt(2, pressed_pos[1], formatVector(coeffs_sol, 6, 2));
					
					ANSI_mvprint(0, 2, "solved coeffs: "+formatVector(coeffs, 6, 1), true, "abs", "rel");
					std::this_thread::sleep_for(std::chrono::seconds(2));
					for(size_t cnt=0; cnt<3; cnt++) {
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
						ANSI_mvprint(0, -1, ".", true, "rel", "abs");
					}
				}
				else if(pressed_pos_angles==pos2d<int>(1, 23)) {
					runAngles = false;
				}
				else if(pressed_pos_angles==pos2d<int>(1, 24)) {
					runAngles = false;
					runMain = false;
				}



			}
		}
	}

}
