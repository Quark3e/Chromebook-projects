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


void HW_option4() {
	simplified_init();
	if(!_init_status.get("pca").isInit()) {
		// std::cout << "ERROR: pca library has not been able to be initialised: " << _init_status.get("pca").get_callMsg()<<std::endl;
		ANSI_mvprint(0, 0, "ERROR: PCA9685 library has not been able to be initialised: "+_init_status.get("pca").get_callMsg(), true, "abs", "rel");
		for(int _=0; _<3; _++) {
			// std::cout << " ."; std::cout.flush();
			ANSI_mvprint(0, -1, " .", true, "abs", "rel");
			std::this_thread::sleep_for(std::chrono::milliseconds(1'000));
		}
		return;
	}
	printFuncLabel(" Running: opt4: Calibrate servo motors");


}
