/**
 * @file hw_option_3.cpp
 * @author your name (you@domain.com)
 * @brief Similar to main option with hand-target tracking and accelerometer telemetry data but without robot arm control
 * @version 0.1
 * @date 2025-02-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "hw_options.hpp"

void HW_option3() {
	// simplified_init();
	if(!(_init_status.get("camObj").isInit() && _init_status.get("orientObj").isInit())) {
		if(!_init_status.get("camObj").isInit()) 	ANSI_mvprint(0, 0, "ERROR: camObj    has not been initialised (required): "+_init_status.get("camObj").get_callMsg(), true, "abs", "rel");
		if(!_init_status.get("orientObj").isInit()) ANSI_mvprint(0, 0, "ERROR: orientObj has not been initialised (required): "+_init_status.get("orientObj").get_callMsg(), true, "abs", "rel");
		for(int _=0; _<3; _++) {
			// std::cout << " ."; std::cout.flush();
			ANSI_mvprint(0, -1, " .", true, "rel", "rel");
			std::this_thread::sleep_for(std::chrono::milliseconds(1'000));
		}
		return;
	}

	printFuncLabel(" Running: opt3: display opencv but don't move servo");
	

}
