/**
 * @file hw_option_1.cpp
 * @author your name (you@domain.com)
 * @brief the option that runs the intro script for the hexclaw robot arm.
 * @version 0.1
 * @date 2025-02-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "hw_options.hpp"


void HW_option1_intro() {
	simplified_init();
	if(!_init_status.get("pca").isInit()) {
		ANSI_mvprint(0, 0, std::string("ERROR: pca library failed to initialise:"+_init_status.get("pca").get_callMsg()+". Cannot run intro."), true, "abs", "rel");
		// std::cout << "ERROR: pca library failed to initialise:"<< _init_status.get("pca").get_callMsg() <<". Cannot run intro."<<std::endl;
		return;
	}
	printFuncLabel(" Running: Into sequence");

	current_q[0] = -45;
	current_q[1] = 25;
	current_q[2] = -115;
	current_q[3] = -45;
	current_q[4] = -90;
	current_q[5] = 90;
	add_defaults(current_q);
	new_q[0] = 0;
	new_q[1] = 135;
	new_q[2] = -90;
	new_q[3] = 0;
	new_q[4] = -45;
	new_q[5] = 0;
	// printf("running intro...\n");
	//sendToServo(&pca, current_q, new_q, false, 0, 0);
	std::this_thread::sleep_for(std::chrono::milliseconds(1'000));
	if(_init_status.get("pigpio").isInit()) {
		#if _MACHINE__RPI_MAIN
		gpioWrite(pin_ledRelay, 0);
		gpioWrite(pin_ledRelay, 1);
		#endif
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(750));
	// std::cout<<"\n- section: \"slow start\"\n";
	ANSI_mvprint(0, 0, "\n- section: \"slow start\"\n", true, "abs", "rel");
	sendToServo(&pca, new_q, current_q, false, 2, 10);

	// std::cout<<"intro finished\n";
	ANSI_mvprint(0, 0, "intro finished\n", true, "abs", "rel");
	std::this_thread::sleep_for(std::chrono::milliseconds(3'000));
	if(_init_status.get("pigpio").isInit()) {

		#if _MACHINE__RPI_MAIN
		for(int i=0; i<4; i++) {
			gpioWrite(pin_ledRelay, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			gpioWrite(pin_ledRelay, 1);
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1'500));
		gpioWrite(pin_ledRelay, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		gpioWrite(pin_ledRelay, 1);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		gpioWrite(pin_ledRelay, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		gpioWrite(pin_ledRelay, 1);
		std::this_thread::sleep_for(std::chrono::milliseconds(2'000));
		gpioWrite(pin_ledRelay, 0);
		#endif
	
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(2'000));

	#if _MACHINE__RPI_MAIN
	if(_init_status.get("pigpio").isInit()) gpioWrite(pin_ledRelay, 1);
	#endif
	
	new_q[0] = 45;
	new_q[1] = 0;
	new_q[2] = -45;
	new_q[3] = 89;
	new_q[4] = 89;
	new_q[5] = 0;
	// std::cout<<"\n- section: \"crash\"\n";
	ANSI_mvprint(0, 0, "\n- section: \"crash\"\n", true, "abs", "rel");
	sendToServo(&pca, new_q, current_q, false);
	std::this_thread::sleep_for(std::chrono::milliseconds(2'000));

}