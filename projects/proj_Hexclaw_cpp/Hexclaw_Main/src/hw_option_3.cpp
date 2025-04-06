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
	
	/**
	 * output print table:
	 *  
	 * 
	 *  | camPos	| x0| y0| x1| y1|
	 *  | PP		| x	| y	| z	|
	 *  | tilt		| a	| b	| Y	|
	 *  | servo		| 0	| 1	| 2 | 3 | 4 | 5 |
	 *  | objArea 	| 0 | 1 |
	 */
	createTable printTable(7, 5);
	printTable.insertText("camPos", 0, 0);
	printTable.insertText("PP", 0, 1);
	printTable.insertText("tilt", 0, 2);
	printTable.insertText("servo", 0, 3);
	printTable.insertText("objArea", 0, 4);

	#if useThreads
	if(init_camObj) {
		std::unique_lock<std::mutex> u_lck_cout(mtx_cout, std::defer_lock);
	
		// std::thread t0(thread_task, &camObj[0], 0);
		// std::thread t1(thread_task, &camObj[1], 1);
		// std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	#endif //useThreads

	/// Initialise the camera objects and the threads

	// camObj/thread initialisation loop boolean.
	bool loopInit = false;
	while(true) {
		/// Main option loop
		
		#if useThreads
		if(init_camObj) {
			if(!(camObj[0].isThreadLoopInit() && camObj[1].isThreadLoopInit())) {
				if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: NOTE: Threads have not been initialised:\n -initialising."); }
	
				while(!(camObj[0].isThreadLoopInit() && camObj[1].isThreadLoopInit())) {
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
				}
				lock_cout(
					mtx_cout, "\n-------------\nSuccessfully initialised both threads:\nstarting tracking:",
					true, true, true
				);
				if(threadDebug) lock_cout(mtx_cout, "\nthread:[2]: sub-threads have been initialised!\n", true, true);
			}
		}
		#endif //useThreads

		printTable.insertNum(PP[0],1,1,1);
		printTable.insertNum(PP[1],2,1,1);
		printTable.insertNum(PP[2],3,1,1);
		printTable.setColWidth(1, 6);
		printTable.setColWidth(2, 6);
		printTable.setColWidth(3, 6);
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
			printTable.insertNum(camObj[0].data().cnt_pos[0],1,0,1);
			printTable.insertNum(camObj[0].data().cnt_pos[1],2,0,1);
			printTable.insertNum(camObj[1].data().cnt_pos[0],3,0,1);
			printTable.insertNum(camObj[1].data().cnt_pos[1],4,0,1);

			if(camObj[0].data().cnt_area==0) { /// cam 0: no object detected
				// printTable.insertText("cam0: no object", 1, 0);
				printTable.insertText("cam0: no object", 1, 4);
			}
			else {
				printTable.insertNum(camObj[0].data().cnt_area,1,4,1);
				printTable.add_to_cell("px",1,4);
			}
			if(camObj[1].data().cnt_area==0) { /// cam 1: no object detected
				// printTable.insertText("cam1: no object", 2, 0);
				printTable.insertText("cam1: no object", 2, 4);
			}
			else {
				printTable.insertNum(camObj[1].data().cnt_area,2,4,1);
				printTable.add_to_cell("px",1,4);
			}
		}
		#endif //useThreads

		for(size_t i=0; i<6; i++) {
			printTable.setColWidth(i+1, 6);
		}
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
			cv::Mat winImg, concThresh, concFlipped;

			CVTRACK::camObjTrackerData camObjData0 = camObj[0].data();
			CVTRACK::camObjTrackerData camObjData1 = camObj[1].data();

			cv::resize(camObjData0.imgFlipped, camObjData0.imgFlipped, cv::Size(), 0.5, 0.5);
			cv::resize(camObjData1.imgFlipped, camObjData1.imgFlipped, cv::Size(), 0.5, 0.5);
			cv::resize(camObjData0.imgThreshold, camObjData0.imgThreshold, cv::Size(), 0.5, 0.5);
			cv::resize(camObjData1.imgThreshold, camObjData1.imgThreshold, cv::Size(), 0.5, 0.5);

			std::cout << "flip0: " << camObjData0.imgFlipped.size().aspectRatio() << std::endl;
			std::cout << "flip1: " << camObjData1.imgFlipped.size().aspectRatio() << std::endl;
			std::cout << "thre0: " << camObjData0.imgThreshold.size().aspectRatio() << std::endl;
			std::cout << "thre1: " << camObjData1.imgThreshold.size().aspectRatio() << std::endl;

			std::cout << "size0: x:" << camObjData0.imgFlipped.size().width << " y:" << camObjData0.imgFlipped.size().height << std::endl;
			std::cout << "size1: x:" << camObjData1.imgFlipped.size().width << " y:" << camObjData1.imgFlipped.size().height << std::endl;
			std::cout << "size0: x:" << camObjData0.imgThreshold.size().width << " y:" << camObjData0.imgThreshold.size().height << std::endl;
			std::cout << "size1: x:" << camObjData1.imgThreshold.size().width << " y:" << camObjData1.imgThreshold.size().height << std::endl;

			cv::hconcat(camObjData0.imgFlipped,   camObjData1.imgFlipped, concFlipped);
			cv::hconcat(camObjData0.imgThreshold, camObjData1.imgThreshold, concThresh);
			
			// std::cout << concFlipped.size.dims() << std::endl;
			// std::cout << concThresh.size.dims() << std::endl;

			
			// cv::vconcat(concFlipped, concThresh, winImg);

			cv::imshow("Main thread window", concFlipped);
			// cv::imshow("Main thread window", winImg);
			cv::imshow("Main thread window: threshold", concThresh);
		
			int keyInp = cv::waitKey(5);
		
			bool _exitLoop = false;
			switch (keyInp) {
				case 27: _exitLoop=true; break;
				case 32: _exitLoop=true; break;
				// case 114: { //'r'
				// 	std::string inputVar = "";
				// 	int indVar = 0;
				// 	// std::cout << "Enter index in hsv file\ninput: ";
				// 	ANSI_mvprint(0, 0, "Enter index in hsv file", true, "abs", "rel");
				// 	ANSI_mvprint(0, 0, "input: ", true, "abs", "rel");
				// 	std::cin >> inputVar;
				// 	if(inputVar=="exit") exit(1);
				// 	indVar = stoi(inputVar);
				// 	std::cin.clear();
				// 	std::cin.ignore();
				// 	hsv_settingsRead(camObj, HW_HSV[0][0], HW_HSV[0][1], HW_HSV[0][2], HW_HSV[1][0], HW_HSV[1][1], HW_HSV[1][2], window_name, indVar);
				// }
				// case 115: { //'s'
				// 	hsv_settingsWrite(HW_HSV[0][0], HW_HSV[0][1], HW_HSV[0][2], HW_HSV[1][0], HW_HSV[1][1], HW_HSV[1][2], 0);
				// 	break;
				// }
				// case 116: { //'t'
				// 	hsv_settingsRead(camObj, HW_HSV[0][0], HW_HSV[0][1], HW_HSV[0][2], HW_HSV[1][0], HW_HSV[1][1], HW_HSV[1][2], window_name, 0); 
				// }
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
		// cv::destroyWindow("Main thread window");
		cv::destroyAllWindows();
	}



	ANSI_mvprint(0, 0, "Exiting main thread", true, "abs", "rel");
}
