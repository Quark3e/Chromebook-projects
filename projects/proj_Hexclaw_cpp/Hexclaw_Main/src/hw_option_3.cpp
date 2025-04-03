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

	bool init_camObj = _init_status.get("camObj").isInit();
	bool init_orientObj = _init_status.get("orientObj").isInit();
	if(!init_camObj && !init_orientObj) {
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
	std::unique_lock<std::mutex> u_lck0(mtx[0], std::defer_lock);
	std::unique_lock<std::mutex> u_lck1(mtx[1], std::defer_lock);
	if(init_camObj) {
		std::unique_lock<std::mutex> u_lck_cout(mtx_cout, std::defer_lock);
	
		std::thread t0(thread_task, &camObj[0], 0);
		std::thread t1(thread_task, &camObj[1], 1);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	#endif //useThreads


	/// Initialise the camera objects and the threads

	// camObj/thread initialisation loop boolean.
	bool loopInit = false;
	while(true) {
		/// Main option loop
		
		#if useThreads
		if(init_camObj) {
			while(!threadsInit[2]) {
				/// While the main thread isn't initialised, keep checking the sub-threads
				/// and update the camera variables
				/// until both the subthreads are initialised and main thread can be declared as initialised.
				u_lck0.lock();
				u_lck1.lock();
				updateCamVars(0);
				updateCamVars(1);
				if(threadsInit[0] && threadsInit[1]) {
					threadsInit[2] = true;
				}
				u_lck1.unlock();
				u_lck0.unlock();
			}

			u_lck0.lock();
			updateCamVars(0);
			u_lck0.unlock();
			u_lck1.lock();
			updateCamVars(1);
			u_lck1.unlock();
			if(returCodes_main[0]==-1) {
				if(threadDebug) lock_cout(mtx_cout,"\ncamObj[0] process error\n",true,true);
				return;
			}
			if(returCodes_main[1]==-1) {
				if(threadDebug) lock_cout(mtx_cout,"\ncamObj[1] process error\n",true,true);
				return;
			}
		}
		#endif //useThreads

		printTable.insertNum(PP[0],1,1,1);
		printTable.insertNum(PP[1],2,1,1);
		printTable.insertNum(PP[2],3,1,1);
		
		try {
			orientObj.update(false);
			printTable.insertNum(orientObj.Roll,1,2,1);
			printTable.insertNum(orientObj.Pitch,2,2,1);
			printTable.insertNum(0,3,2,1);

			orient[0] = orientObj.Roll;
			orient[1] = orientObj.Pitch;
		}
		catch(const std::exception& e) {
			printTable.insertText("ERROR:", 1, 2);
			printTable.insertText(e.what(), 2, 2);
			// std::cerr << e.what() << '\n';
		}
		
		
		#if useThreads
		if(init_camObj) {
			if(numContours_main[0]>0) {
				printTable.insertNum(camObjPos_main[0][0],1,0,1);
				printTable.insertNum(camObjPos_main[0][1],2,0,1);
				printTable.insertNum(camObjPos_main[1][0],3,0,1);
				printTable.insertNum(camObjPos_main[1][1],4,0,1);
			}
			else {
				printTable.insertText("ERROR:no tracked object visible on cams", 1, 1);
			}
		}
		#endif //useThreads

		try {
			if(HW_KINEMATICS::getAngles(new_q, PP, RADIANS(orient[0]), RADIANS(orient[1]), RADIANS(orient[2]), 1)) {
				for(int i=0; i<6; i++) printTable.insertNum(new_q[i],1+i,3,1);
				printTable.strExport("");
			}
			else if(HW_KINEMATICS::findValidOrient(PP, orient, orient, new_q)) {
				printTable.insertNum(orient[0],1,2,1);
				printTable.insertNum(orient[1],2,2,1);
				printTable.insertNum(orient[2],3,2,1);
				for(int i=0; i<6; i++) printTable.insertNum(new_q[i],1+i,3,1);
				printTable.strExport(std::string(21,' ')+"\n");
			}
			else {
				printTable.insertText("ERROR:no valid angles", 1, 3);
			}
		}
		catch(const std::exception& e) {
			printTable.insertText("ERROR:", 1, 3);
			printTable.insertText(e.what(), 2, 3);
			// std::cerr << e.what() << '\n';
		}

		if(init_camObj && _CONFIG_OPTIONS.get("displayToWindow")) {
			cv::Mat winImg;
			cv::hconcat(flippedImg_main[0], flippedImg_main[1], winImg);
			cv::imshow("Main thread window", winImg);
		
			int keyInp = cv::waitKey(5);
		
			bool _exitLoop = false;
			switch (keyInp) {
			case 27: _exitLoop=true; break;
			case 32: _exitLoop=true; break;
			case 114: { //'r'
				std::string inputVar = "";
				int indVar = 0;
				// std::cout << "Enter index in hsv file\ninput: ";
				ANSI_mvprint(0, 0, "Enter index in hsv file", true, "abs", "rel");
				ANSI_mvprint(0, 0, "input: ", true, "abs", "rel");
				std::cin >> inputVar;
				if(inputVar=="exit") exit(1);
				indVar = stoi(inputVar);
				std::cin.clear();
				std::cin.ignore();
				hsv_settingsRead(camObj, HW_HSV[0][0], HW_HSV[0][1], HW_HSV[0][2], HW_HSV[1][0], HW_HSV[1][1], HW_HSV[1][2], window_name, indVar);
			}
			case 115: { //'s'
				hsv_settingsWrite(HW_HSV[0][0], HW_HSV[0][1], HW_HSV[0][2], HW_HSV[1][0], HW_HSV[1][1], HW_HSV[1][2], 0);
				break;
			}
			case 116: { //'t'
				hsv_settingsRead(camObj, HW_HSV[0][0], HW_HSV[0][1], HW_HSV[0][2], HW_HSV[1][0], HW_HSV[1][1], HW_HSV[1][2], window_name, 0); 
			}
			default:
				break;
			}
			if(_exitLoop) break;
			if(keyInp==27) break;
		

		}
		
		ANSI_mvprint(0, 4, printTable.exportStr, true, "abs", "abs");

		loopInit = true;
	}

	if(init_camObj && _CONFIG_OPTIONS.get("displayToWindow")) {
		cv::destroyWindow("Main thread window");
	}

	u_lck0.lock();
	exit_thread[0] = true;
	u_lck0.unlock();
	u_lck1.lock();
	exit_thread[1] = true;
	u_lck1.unlock();

	ANSI_mvprint(0, 0, "Exiting main thread", true, "abs", "rel");
}
