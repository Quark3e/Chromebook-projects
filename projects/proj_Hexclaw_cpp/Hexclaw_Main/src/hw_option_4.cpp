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

	{"  0°", 1,  2, -1}, {"   ", 2, 2, -1},
	{" 10°", 1,  2, -1}, {"   ", 2, 2, -1},
	{" 20°", 1,  2, -1}, {"   ", 2, 2, -1},
	{" 30°", 1,  2, -1}, {"   ", 2, 2, -1},
	{" 40°", 1,  2, -1}, {"   ", 2, 2, -1},
	{" 50°", 1,  2, -1}, {"   ", 2, 2, -1},
	{" 60°", 1,  2, -1}, {"   ", 2, 2, -1},
	{" 70°", 1,  2, -1}, {"   ", 2, 2, -1},
	{" 80°", 1,  2, -1}, {"   ", 2, 2, -1},
	{" 90°", 1,  2, -1}, {"   ", 2, 2, -1},
	{"100°", 1,  2, -1}, {"   ", 2, 2, -1},
	{"120°", 1,  2, -1}, {"   ", 2, 2, -1},
	{"130°", 1,  2, -1}, {"   ", 2, 2, -1},
	{"140°", 1,  2, -1}, {"   ", 2, 2, -1},
	{"150°", 1,  2, -1}, {"   ", 2, 2, -1},
	{"160°", 1,  2, -1}, {"   ", 2, 2, -1},
	{"170°", 1,  2, -1}, {"   ", 2, 2, -1},
	{"180°", 1,  2, -1}, {"   ", 2, 2, -1}
});

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
		if(pressed_pos.inRegion({0, 2}, {0, 7})) {
			ANSI_mvprint(0, 2, "Calibrating motor: q["+formatNumber(pressed_pos[1]-2)+"]", true, "abs", "abs", true);
			std::this_thread::sleep_for(std::chrono::seconds(1));
			
		}
	}

}
