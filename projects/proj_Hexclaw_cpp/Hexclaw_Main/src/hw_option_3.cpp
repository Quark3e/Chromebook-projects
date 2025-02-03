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
	simplified_init();
	if(!_init_status.get("camObj").isInit()) {
		// std::cout << "ERROR: camObj has not been able to be initialised: " << _init_status.get("camObj").get_callMsg()<<std::endl;
		ANSI_mvprint(0, 0, "ERROR: camObj has not been able to be initialised: "+_init_status.get("camObj").get_callMsg(), true, "abs", "rel");
		for(int _=0; _<3; _++) {
			// std::cout << " ."; std::cout.flush();
			ANSI_mvprint(0, -1, " .", true, "rel", "rel");
			usleep(1'000'000);
		}
		return;
	}
	printFuncLabel(" Running: opt3: display opencv but don't move servo");

}
