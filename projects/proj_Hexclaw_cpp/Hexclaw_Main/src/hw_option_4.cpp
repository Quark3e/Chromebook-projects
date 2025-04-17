/**
 * @file hw_option_4.cpp
 * @author your name (you@domain.com)
 * @brief Calibrate servo motors by reading error rate and creating correction functions. (NOTE: will be changes so that in the future option 4 contains all the calibration methods/functions for various things, HSV, servo error rate e.t.)
 * @version 0.1
 * @date 2025-02-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#include "hw_options.hpp"


TUI::termMenu menu__calibrateMotor_main({
	{" Select a joint/motor to calibrate", 0, 0, TUI::CELLOPTSPEC_TEXT}, {" Error regr", 1, 0, TUI::CELLOPTSPEC_TEXT}, {" Regr solution", 2, 0, TUI::CELLOPTSPEC_TEXT},
	
	{" q[0]", 0, 2, '0'}, {" ", 1, 2, -1},
	{" q[1]", 0, 3, '1'}, {" ", 1, 3, -1},
	{" q[2]", 0, 4, '2'}, {" ", 1, 4, -1},
	{" q[3]", 0, 5, '3'}, {" ", 1, 5, -1},
	{" q[4]", 0, 6, '4'}, {" ", 1, 6, -1},
	{" q[5]", 0, 7, '5'}, {" ", 1, 7, -1},

	{" back", 0, 9, 27}
}, false);

TUI::termMenu menu__calibrateMotor_angles({


	{"   0", 1,  2, -1}, {"    ", 2,  2, TUI::CELLOPTSPEC_TEXT},
	{"  10", 1,  3, -1}, {"    ", 2,  3, TUI::CELLOPTSPEC_TEXT},
	{"  20", 1,  4, -1}, {"    ", 2,  4, TUI::CELLOPTSPEC_TEXT},
	{"  30", 1,  5, -1}, {"    ", 2,  5, TUI::CELLOPTSPEC_TEXT},
	{"  40", 1,  6, -1}, {"    ", 2,  6, TUI::CELLOPTSPEC_TEXT},
	{"  50", 1,  7, -1}, {"    ", 2,  7, TUI::CELLOPTSPEC_TEXT},
	{"  60", 1,  8, -1}, {"    ", 2,  8, TUI::CELLOPTSPEC_TEXT},
	{"  70", 1,  9, -1}, {"    ", 2,  9, TUI::CELLOPTSPEC_TEXT},
	{"  80", 1, 10, -1}, {"    ", 2, 10, TUI::CELLOPTSPEC_TEXT},
	{"  90", 1, 11, -1}, {"    ", 2, 11, TUI::CELLOPTSPEC_TEXT},
	{" 100", 1, 12, -1}, {"    ", 2, 12, TUI::CELLOPTSPEC_TEXT},
	{" 110", 1, 13, -1}, {"    ", 2, 13, TUI::CELLOPTSPEC_TEXT},
	{" 120", 1, 14, -1}, {"    ", 2, 14, TUI::CELLOPTSPEC_TEXT},
	{" 130", 1, 15, -1}, {"    ", 2, 15, TUI::CELLOPTSPEC_TEXT},
	{" 140", 1, 16, -1}, {"    ", 2, 16, TUI::CELLOPTSPEC_TEXT},
	{" 150", 1, 17, -1}, {"    ", 2, 17, TUI::CELLOPTSPEC_TEXT},
	{" 160", 1, 18, -1}, {"    ", 2, 18, TUI::CELLOPTSPEC_TEXT},
	{" 170", 1, 19, -1}, {"    ", 2, 19, TUI::CELLOPTSPEC_TEXT},
	{" 180", 1, 20, -1}, {"    ", 2, 20, TUI::CELLOPTSPEC_TEXT},

	{" Solve",1, 21, -1},
	
	{" back", 1, 23, 27},
	{" exit", 1, 24, 27}
}, false);


void HW_option4() {
	// simplified_init();
	if(!(_init_status.get("pca").isInit() || _init_status.get("orientObj").isInit())) {
		if(!_init_status.get("pca").isInit()) 		ANSI_mvprint(0, 0, "ERROR: pca    	 has not been initialised (required): "+_init_status.get("pca").get_callMsg(), true, "abs", "rel");
		if(!_init_status.get("orientObj").isInit()) ANSI_mvprint(0, 0, "ERROR: orientObj has not been initialised (required): "+_init_status.get("orientObj").get_callMsg(), true, "abs", "rel");
		for(int _=0; _<3; _++) {
			// std::cout << " ."; std::cout.flush();
			ANSI_mvprint(0, -1, " .", true, "rel", "rel");
			SHLEEP((1'000));
		}
		return;
	}
	
	bool runMain = true;
	while(runMain) {
		printFuncLabel(" Running: opt4: Calibrate servo motors");
		// ANSI_mvprint(0, 0, "Select a joint/motor to calibrate", true, "abs", "rel", true);

		pos2d<int> pressed_pos = menu__calibrateMotor_main.driver(1, 5, 1, true, _mainDriver_updateFunc__orientObj, true);

		if(pressed_pos==pos2d<int>(0, 9)) break;
		if(pressed_pos.inRegion({0, 2}, {0, 7})) { //check if pressed_pos is in region of the selectable motor options
			ANSI_mvprint(0, 2, "Calibrating motor: q["+formatNumber(pressed_pos[1]-2)+"]", true, "abs", "abs", false);
			
			// calibrate motor
			bool runAngles = true;
			while(runAngles) {
				ANSI_mvprint(0, 0, "Select an angle to calibrate", true, "abs", "abs", true);
				pos2d<int> pressed_pos_angles = menu__calibrateMotor_angles.driver(1, 4, 1, false, _mainDriver_updateFunc__orientObj, false);
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
					SHLEEP((500));
					// orientObj.update(false);

					menu__calibrateMotor_angles.rename_opt(2, pressed_pos_angles[1], formatNumber(orientObj.Pitch+90, 7, 1));

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
						SHLEEP((1000));
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


TUI::termMenu menu__calibrateAccel_main({
	{" Adjust accel board and select an axis to calibrate", 0, 0, TUI::CELLOPTSPEC_TEXT},
	
	{"reset", 0, 2, -1},

	{" X_neg: "	, 0, 4, -1}, {" X_pos: ", 2, 4, -1},
	{" Y_neg: "	, 0, 5, -1}, {" Y_pos: ", 2, 5, -1},
	{" Z_neg: "	, 0, 6, -1}, {" Z_pos: ", 2, 6, -1},

	{" apply offset", 0, 8, -1},

	{"back"	, 0, 10, 27}
}, false);

void _updateFunc_opt4_accelOffset(TUI::termMenu* ptr_menu) {
	static std::string prevStr = "";
	assert(_init_status.get("orientObj").isInit());
	std::string textCell_text;
	try {
		orientObj.update(false);
		ptr_menu->addTextCell(orientObj.accel, 2, 0);
	}
	catch(const std::exception& e) {
		ptr_menu->addTextCell(e.what(), 2, 0);
	}
	
	if(prevStr!=textCell_text) {
		prevStr = textCell_text;
	}
	ptr_menu->updateTable();
}

void HW_option4_accelOffsets() {
	if(!_init_status.get("orientObj").isInit()) {
		ANSI_mvprint(0, 0, "ERROR: orientObj has not been initialised (required): "+_init_status.get("orientObj").get_callMsg(), true, "abs", "rel");
		for(size_t _=0; _<3; _++) {
			ANSI_mvprint(0, -1, " .", true, "rel", "rel");
			SHLEEP(1000);
		}
		return;
	}

	size_t numValuesToMeasure = 100;
	std::vector<pos2d<float>> endValues(3, pos2d<float>(-1, 1));

	bool runMain = true;
	while(runMain) {
		for(size_t axis=0; axis<3; axis++) {
			for(size_t limID=0; limID<2; limID++) {
				menu__calibrateAccel_main.addTextCell(formatNumber(endValues.at(axis)[limID], 4, 2), limID*2+1, axis+4);
			}
		}
		
		try {
			pos2d<int> pressed_pos = menu__calibrateAccel_main.driver(1, 3, 5, true, _updateFunc_opt4_accelOffset, true);
			
			if(pressed_pos==pos2d<int>{0, 2}) endValues = std::vector<pos2d<float>>(3, pos2d<float>(-1, 1));
			else if(pressed_pos==pos2d<int>{0, 8}) {
				orientObj.accel_offs[0] = endValues.at(0)[1] - std::abs(endValues.at(0)[0]);
				orientObj.accel_offs[1] = endValues.at(1)[1] - std::abs(endValues.at(1)[0]);
				orientObj.accel_offs[2] = endValues.at(2)[1] - std::abs(endValues.at(2)[0]);
			}
			else if(pressed_pos==pos2d<int>{0, 10}) break;
			else if(pressed_pos.inRegion({0, 4}, {2, 6})) {
				/**
				 * The accelerometer axis value to find the max(abs()) value of.
				 *  - [0]: min/max
				 *  - [1]: x/y/z
				 * 
				 */
				pos2d<int> valIdx = {pressed_pos[0]/2, (pressed_pos[1]-4)};
				float furthestVal = 0;
				for(size_t i=0; i<numValuesToMeasure; i++) {
					try {
						orientObj.update(false);
					}
					catch(const std::exception& e) {
						continue;
					}
					float val = std::abs(orientObj.accel[valIdx[1]]);
					if(val>furthestVal) {
						furthestVal = val;
						endValues.at(valIdx[1])[valIdx[0]] = orientObj.accel[valIdx[1]];
					}
				}
	
	
			}
		}
		catch(const std::exception& e) {
			std::cout << e.what() << '\n';
			SHLEEP(1000);
			exit(1);
		}
		
	}

}
