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


void trackBarCallback(int, void* _callBackBool) {
	std::cout << "trackbar callback" << std::endl;
	// std::cout.flush();

	*reinterpret_cast<bool*>(_callBackBool) = true;
}

void HW_option3() {

	bool init_camObj = _init_status.get("camObj").isInit();
	bool init_orientObj = _init_status.get("orientObj").isInit();
	if(!init_camObj && !init_orientObj) {
		if(!init_camObj) 	ANSI_mvprint(0, 0, "ERROR: camObj    has not been initialised (required): "+_init_status.get("camObj").get_callMsg(), true, "abs", "rel");
		if(!init_orientObj) ANSI_mvprint(0, 0, "ERROR: orientObj has not been initialised (required): "+_init_status.get("orientObj").get_callMsg(), true, "abs", "rel");
		for(int _=0; _<3; _++) {
			// std::cout << " ."; std::cout.flush();
			ANSI_mvprint(0, -1, " .", true, "rel", "rel");
			SHLEEP((1'000));
		}
		return;
	}

	printFuncLabel(" Running: opt3: display opencv but don't move servo");
	
	if(init_orientObj) orientObj.update(false);
	
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
	
	
	std::unique_lock<std::mutex> u_lck_cout;

	
	bool trackBarUpdate = false;
	if(init_camObj) {

		u_lck_cout = std::unique_lock<std::mutex>(mtx_cout, std::defer_lock);
		
		if(_CONFIG_OPTIONS.get("displayToWindow")) {
			cv::namedWindow("Main thread window", cv::WINDOW_AUTOSIZE | cv::WINDOW_GUI_EXPANDED);

			cv::namedWindow("Main thread window",  cv::WINDOW_AUTOSIZE);
			cv::createTrackbar("L_Hue", "Main thread window", &HW_HSV[0][0], 179, trackBarCallback, &trackBarUpdate);
			cv::createTrackbar("L_Sat", "Main thread window", &HW_HSV[0][1], 255, trackBarCallback, &trackBarUpdate);
			cv::createTrackbar("L_Val", "Main thread window", &HW_HSV[0][2], 255, trackBarCallback, &trackBarUpdate);
			cv::createTrackbar("U_Hue", "Main thread window", &HW_HSV[1][0], 179, trackBarCallback, &trackBarUpdate);
			cv::createTrackbar("U_Sat", "Main thread window", &HW_HSV[1][1], 255, trackBarCallback, &trackBarUpdate);
			cv::createTrackbar("U_Val", "Main thread window", &HW_HSV[1][2], 255, trackBarCallback, &trackBarUpdate);
		}
	}

	cv::Mat emptyMat = cv::Mat::zeros(prefSize[1], prefSize[0], CV_8UC3);

	CVTRACK::camObjTrackerData camObjData0;
	CVTRACK::camObjTrackerData camObjData1;

	// camObj/thread initialisation loop boolean.
	bool loopInit = false;
	while(true) {
		/// Main option loop
		if(init_camObj) {
			/// Check if camObj threads have been initialised
			if(_CONFIG_OPTIONS.get("camObj_useThreads") && !(camObj[0].isThreadLoopInit() && camObj[1].isThreadLoopInit())) {
				if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: NOTE: Threads have not been initialised:\n -initialising."); }
				
				while(!(camObj[0].isThreadLoopInit() && camObj[1].isThreadLoopInit())) {
					SHLEEP((500));
				}
				lock_cout(
					mtx_cout, "\n-------------\nSuccessfully initialised both threads:\nstarting tracking:",
					true, true, true
				);
				if(threadDebug) lock_cout(mtx_cout, "\nthread:[2]: sub-threads have been initialised!\n", true, true);
			}
		}

		printTable.insertNum(PP[0],1,1,1);
		printTable.insertNum(PP[1],2,1,1);
		printTable.insertNum(PP[2],3,1,1);
		printTable.setColWidth(1, 6);
		printTable.setColWidth(2, 6);
		printTable.setColWidth(3, 6);
		if(init_orientObj) {
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
		}
		else {
			printTable.insertText("orientObj not init.", 1, 2);
			orient[0] = 0;
			orient[1] = 0;
		}
		
		
		if(init_camObj) {
			/// Update camObjData values
			if(!_CONFIG_OPTIONS.get("camObj_useThreads")) {
				camObjData0 = camObj[1].updateData();
				camObjData1 = camObj[0].updateData();
			}
			else {
				camObjData0 = camObj[1].data();
				camObjData1 = camObj[0].data();
			}
			
			getCoordinates(camObjData0.cnt_pos, camObjData1.cnt_pos, 2, prefSize.cast<float>());

			/// Insert cnt_pos data to termMenu tables
			printTable.insertNum(camObjData0.cnt_pos[0],1,0,1);
			printTable.insertNum(camObjData0.cnt_pos[1],2,0,1);
			printTable.insertNum(camObjData1.cnt_pos[0],3,0,1);
			printTable.insertNum(camObjData1.cnt_pos[1],4,0,1);
			
			/// Insert cnt_area data to termMenu tables
			if(camObjData0.cnt_area==0) { /// cam 0: no object detected
				// printTable.insertText("cam0: no object", 1, 0);
				printTable.insertText("cam0: no object", 1, 4);
			}
			else {
				printTable.insertNum(camObjData0.cnt_area, 1, 4, 1);
				printTable.add_to_cell("px", 1, 4);
			}
			if(camObjData1.cnt_area==0) { /// cam 1: no object detected
				// printTable.insertText("cam1: no object", 2, 0);
				printTable.insertText("cam1: no object", 2, 4);
			}
			else {
				printTable.insertNum(camObjData1.cnt_area, 2, 4, 1);
				printTable.add_to_cell("px", 2, 4);
			}

		}
		
		/// Re-define termMenu column width's
		for(size_t i=0; i<6; i++) {
			printTable.setColWidth(i+1, 6);
		}

		/// Solve angles from PP and orient
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
			}
			else {
				printTable.insertText("ERROR:no valid", 1, 3);
			}
		}
		catch(const std::exception& e) {
			printTable.insertText("ERROR: exc", 1, 3);
			// printTable.insertText(e.what(), 2, 3);
			// std::cerr << e.what() << '\n';
		}
		
		
		u_lck_cout.lock();
		std::cout << formatVector(HW_HSV[0], 3, 0) << " " << formatVector(HW_HSV[1], 3, 0) << std::endl;
		u_lck_cout.unlock();

		if(init_camObj && _CONFIG_OPTIONS.get("displayToWindow")) {
			if(trackBarUpdate) {
				camObj[0].setHSV(HW_HSV[0], HW_HSV[1]);
				camObj[1].setHSV(HW_HSV[0], HW_HSV[1]);
				trackBarUpdate = false;
			}
			
			cv::Mat winImg, concThresh, concFlipped, concHSV;

			cv::cvtColor(camObjData0.imgThreshold, camObjData0.imgThreshold, cv::COLOR_GRAY2BGR);
			cv::cvtColor(camObjData1.imgThreshold, camObjData1.imgThreshold, cv::COLOR_GRAY2BGR);
			
			// cv::cvtColor(camObjData0.imgHSV, camObjData0.imgHSV, cv::COLOR_HSV2BGR);
			// cv::cvtColor(camObjData1.imgHSV, camObjData1.imgHSV, cv::COLOR_HSV2BGR);

			camObjData0.imgThreshold.convertTo(camObjData0.imgThreshold, CV_8UC3);
			camObjData1.imgThreshold.convertTo(camObjData1.imgThreshold, CV_8UC3);

			camObjData0.imgHSV.convertTo(camObjData0.imgHSV, CV_8UC3);
			camObjData1.imgHSV.convertTo(camObjData1.imgHSV, CV_8UC3);
		
			cv::putText(camObjData0.imgFlipped, "Cam 0", cv::Point(25, 25), 0, 1, cv::Scalar(250, 250, 250, 250));
			cv::putText(camObjData1.imgFlipped, "Cam 1", cv::Point(25, 25), 0, 1, cv::Scalar(250, 250, 250, 250));

			cv::circle(camObjData0.imgFlipped, cv::Point(camObjData0.cnt_pos.x, camObjData0.cnt_pos.y), 10, cv::Scalar(200, 100, 100, 250));
			cv::circle(camObjData1.imgFlipped, cv::Point(camObjData1.cnt_pos.x, camObjData1.cnt_pos.y), 10, cv::Scalar(200, 100, 100, 250));
			cv::line(camObjData0.imgFlipped, cv::Point(camObjData0.cnt_pos.x, camObjData0.cnt_pos.y+5), cv::Point(camObjData0.cnt_pos.x, camObjData0.cnt_pos.y-5), cv::Scalar(200, 100, 100, 250));
			cv::line(camObjData0.imgFlipped, cv::Point(camObjData0.cnt_pos.x+5, camObjData0.cnt_pos.y), cv::Point(camObjData0.cnt_pos.x-5, camObjData0.cnt_pos.y), cv::Scalar(200, 100, 100, 250));
			cv::line(camObjData1.imgFlipped, cv::Point(camObjData1.cnt_pos.x, camObjData1.cnt_pos.y+5), cv::Point(camObjData1.cnt_pos.x, camObjData1.cnt_pos.y-5), cv::Scalar(200, 100, 100, 250));
			cv::line(camObjData1.imgFlipped, cv::Point(camObjData1.cnt_pos.x+5, camObjData1.cnt_pos.y), cv::Point(camObjData1.cnt_pos.x-5, camObjData1.cnt_pos.y), cv::Scalar(200, 100, 100, 250));

			cv::hconcat(camObjData0.imgFlipped,   	camObjData1.imgFlipped, 	concFlipped);
			cv::hconcat(camObjData0.imgHSV,     	camObjData1.imgHSV,     	concHSV);
			cv::hconcat(camObjData0.imgThreshold, 	camObjData1.imgThreshold,	concThresh);
			
			cv::vconcat(concFlipped, concHSV, winImg);
			cv::vconcat(winImg, concThresh, winImg);


			cv::imshow("Main thread window", winImg);
			// cv::imshow("HSV controls", winImg);

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
		
		printTable.strExport(std::string(21,' ')+"\n");
		ANSI_mvprint(0, 4, printTable.exportStr, true, "abs", "abs");

		loopInit = true;
	}

	if(init_camObj && _CONFIG_OPTIONS.get("displayToWindow")) {
		// cv::destroyWindow("Main thread window");
		cv::destroyAllWindows();
	}


	if(init_camObj && _CONFIG_OPTIONS.get("camObj_useThreads")) u_lck_cout.lock();
	ANSI_mvprint(0, 0, "Exiting main thread", true, "abs", "rel");
	if(init_camObj && _CONFIG_OPTIONS.get("camObj_useThreads")) u_lck_cout.unlock();

}
