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
	{"q[0]", 0, 2, '0'},
	{"q[1]", 0, 3, '1'},
	{"q[2]", 0, 4, '2'},
	{"q[3]", 0, 5, '3'},
	{"q[4]", 0, 6, '4'},
	{"q[5]", 0, 7, '5'},

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
	{"120°", 1, 13, -1}, {"   ", 2, 13, -1},
	{"130°", 1, 14, -1}, {"   ", 2, 14, -1},
	{"140°", 1, 15, -1}, {"   ", 2, 15, -1},
	{"150°", 1, 16, -1}, {"   ", 2, 16, -1},
	{"160°", 1, 17, -1}, {"   ", 2, 17, -1},
	{"170°", 1, 18, -1}, {"   ", 2, 18, -1},
	{"180°", 1, 19, -1}, {"   ", 2, 19, -1},

	{"Solve",1, 20, -1},
	
	{"back", 1, 22, -1},
	{"exit", 1, 23, -1}
}, false);

void HW_option4() {
	// simplified_init();
	if(!(_init_status.get("pca").isInit() && _init_status.get("orientObj").isInit())) {
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
				ANSI_mvprint(0, 0, "Select an angle to calibrate", true, "abs", "rel");
				pos2d<int> pressed_pos = menu__calibrateMotor_angles.driver(0, 5, 1, false, TUI::termMenu_nullFunc_void__, false);
				if(pressed_pos==pos2d<int>(0, 9)) break;
				if(pressed_pos.inRegion({1, 2}, {1, 19})) { //check if pressed_pos is in region of the selectable motor options
					orientObj.update(false);

					menu__calibrateMotor_angles.rename_opt(2, pressed_pos[1], formatNumber(orient.y, 7, 1));

					/// Manual input of angles
					// std::string inpValue = menu__calibrateMotor_angles.termInput(0);
					// if(inpValue=="exit") return;
					// else if(inpValue=="back") break;
					// else {
					// 	menu__calibrateMotor_angles.rename_opt(2, pressed_pos[1], inpValue);
					// }
				}
				else if(pressed_pos==pos2d<int>(1, 20)) {
					std::vector<float> readAngles(19, 0);
					for(size_t i=0; i<19; i++) {
						try {
							readAngles[i] = std::stof(menu__calibrateMotor_angles.getCellName(2, i+2));
						}
						catch(const std::invalid_argument& e) {
							ANSI_mvprint(0, 0, "ERROR: invalid input", true, "abs", "rel");
							std::this_thread::sleep_for(std::chrono::seconds(1));
							break;
						}
					}


				}
				else if(pressed_pos==pos2d<int>(1, 22)) {
					runAngles = false;
				}
				else if(pressed_pos==pos2d<int>(1, 23)) {
					runAngles = false;
					runMain = false;
				}

			}
		}
	}

}
