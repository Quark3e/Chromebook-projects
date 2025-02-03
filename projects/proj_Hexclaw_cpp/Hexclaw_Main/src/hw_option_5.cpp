/**
 * @file hw_option_5.cpp
 * @author your name (you@domain.com)
 * @brief tilt telemetry based robot end-effector movement without use of webcam input.
 * @version 0.1
 * @date 2025-02-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "hw_options.hpp"


void HW_option5_orient() {
	simplified_init();
	printFuncLabel(" Running: opt5: Only orientation movement on servo robot");
	if(!_init_status.get("pca").isInit()) ANSI_mvprint(0, 0, "NOTE: pca library has not been able to be initialised: "+_init_status.get("pca").get_callMsg(), true, "abs", "rel");
	// std::cout << "NOTE: pca library has not been able to be initialised: " << _init_status.get("pca").get_callMsg()<<std::endl;

	// std::this_thread::sleep_for(1000ms);

	createTable printTable(8, 2);
	printTable.insertText("q_0", 0, 0);
	printTable.insertText("q_1", 1, 0);
	printTable.insertText("q_2", 2, 0);
	printTable.insertText("q_3", 3, 0);
	printTable.insertText("q_4", 4, 0);
	printTable.insertText("q_5", 5, 0);
	printTable.insertText("alpha", 6, 0);
	printTable.insertText("beta", 7, 0);

	while(true) {
		orientObj.update(true);
		if(HW_KINEMATICS::getAngles(
			new_q, PP, toRadians(orient[0]), toRadians(orient[1]),toRadians(orient[2]), 1
		)) {
			printTable.insertNum(new_q[0], 0, 1, 1);
			printTable.insertNum(new_q[1], 1, 1, 1);
			printTable.insertNum(new_q[2], 2, 1, 1);
			printTable.insertNum(new_q[3], 3, 1, 1);
			printTable.insertNum(new_q[4], 4, 1, 1);
			printTable.insertNum(new_q[5], 5, 1, 1);
			printTable.insertNum(orient[0], 6, 1, 1);
			printTable.insertNum(orient[1], 7, 1, 1);

			printTable.strExport(
				formatNumber(PP[0],6,1)+","+
				formatNumber(PP[1],6,1)+","+
				formatNumber(PP[2],6,1)+"\n"
			);

			if(_init_status.get("pca").isInit()) sendToServo(&pca, new_q, current_q, false);
		}
		else {
			printTable.strExport(
				formatNumber(PP[0],6,1)+","+
				formatNumber(PP[1],6,1)+","+
				formatNumber(PP[2],6,1)+": PP-NOT REACHABLE\n"
			);
		}
		ansiPrint(
			printTable.exportStr,
			static_cast<float>(1),
			static_cast<float>(0.1)
		);
		std::cout.flush();
	}
}
