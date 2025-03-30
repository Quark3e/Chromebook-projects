/**
 * @file hw_option_3.cpp
 * @author your name (you@domain.com)
 * @brief Similar to main option with hand-target tracking and accelerometer telemetry data but without movement sent to the robot-arm/servo-motors
 * @version 0.1
 * @date 2025-02-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "hw_options.hpp"



void HW_option3() {
	// simplified_init();
	bool init_camObj = _init_status.get("camObj").isInit();
	bool init_orientObj = _init_status.get("orientObj").isInit();
	if(!(init_camObj && init_orientObj)) {
		if(!init_camObj) 	ANSI_mvprint(0, 0, "ERROR: camObj    has not been initialised (required): "+_init_status.get("camObj").get_callMsg(), true, "abs", "rel");
		if(!init_orientObj) ANSI_mvprint(0, 0, "ERROR: orientObj has not been initialised (required): "+_init_status.get("orientObj").get_callMsg(), true, "abs", "rel");
		for(int _=0; _<3; _++) {
			// std::cout << " ."; std::cout.flush();
			ANSI_mvprint(0, -1, " .", true, "rel", "rel");
			std::this_thread::sleep_for(std::chrono::milliseconds(1'000));
		}
		return;
	}

	printFuncLabel(" Running: opt3: display opencv but don't move servo");
	
	orientObj.update(false);
	
	createTable printTable(7, 4);
	printTable.insertText("camPos", 0, 0);
	printTable.insertText("PP", 0, 1);
	printTable.insertText("orient", 0, 2);
	printTable.insertText("servo", 0, 3);

	#if useThreads
	
	#endif //useThreads

}
