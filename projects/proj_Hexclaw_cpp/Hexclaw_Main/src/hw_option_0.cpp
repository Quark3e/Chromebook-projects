/**
 * @file hw_option_0.cpp
 * @author your name (you@domain.com)
 * @brief Main default hw_option which moves the robot arm with webcam trig positioning and telemetry reading.
 * @version 0.1
 * @date 2025-02-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "hw_options.hpp"


void HW_option0() {
	printFuncLabel(" Running: opt0: Main loop with opencv and servo control");
	simplified_init();

	if(!_init_status.get("pca").isInit()) { ANSI_mvprint(0, 0, "NOTE: PCA9685 library has not been successfully initialised: "+_init_status.get("pca").get_callMsg(), true, "abs", "rel"); }
	if(!_init_status.get("camObj").isInit()) { ANSI_mvprint(0, 0, "ERROR: camObj has not been successfully initialised: "+_init_status.get("camObj").get_callMsg(), true, "abs", "rel"); }
	if(!(_init_status.get("pca").isInit() && _init_status.get("camObj").isInit())) {
		ANSI_mvprint(0, 0, "Exiting \"opt0: Main loop\"", true, "abs", "rel");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		return;
	}
	ANSI_mvprint(
		0, 0,
		formatNumber(_init_status.get("pca").isInit(), 5, 0, "left")+" "+formatNumber(_init_status.get("camObj").isInit(), 5, 0, "left"),
		true, "abs", "rel"
	);
	std::this_thread::sleep_for(std::chrono::seconds(5));



	//create windows regardless because i need a way to properly exit the loop
	// camObj[0].setup_window("Main thread window");
	// camObj[1].setup_window("Main thread window");
	
	
	if(!_CONFIG_OPTIONS.get("displayToWindow")) {
		cv::namedWindow("Main thread window", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
		cv::resizeWindow("Main thread window", 100, 100);
	}

	/**
	 * output print table:
	 * 
	 * | cam| x0| y0| x1| y1|
	 * | PP	| x	| y	| z	|
	 * | ori| a	| b	| Y	|
	 * | q_	| 0	| 1	| 2 | 3 | 4 | 5 |
	 * 
	 * |perf| ms|fps|
	 * 
	 * |p_t0| c1| c2| c3|tot|	{ms}
	 * |p_t1| c1| c2| c3|tot|	{ms}
	*/
	createTable printTable(7, 4);
	printTable.insertText("camPos", 0, 0);
	printTable.insertText("PP", 0, 1);
	printTable.insertText("orient", 0, 2);
	printTable.insertText("servo", 0, 3);

	/**
	 * |    |main|    |sub0|sub1|
	 * |c1  |    |tSyn|    |    |
	 * |c2  |    |camP|    |    |
	 * |c3  |    |tran|    |    |
	 * 
	 * |    |    |read|    |    |
	 * |    |    |resi|    |    |
	 * |    |    |cvtC|    |    |
	 * |    |    |inRa|    |    |
	 * |    |    |erod|    |    |
	 * |    |    |dila|    |    |
	 * |    |    |cAre|    |    |
	 * |    |    |avgP|    |    |
	 * 
	 * |    |    |cvtC|    |    |
	 * |    |    |conc|    |    |
	*/
	createTable delayTable(5, 14);
	delayTable.insertText("main",1,0);
	delayTable.insertText("sub0",3,0);
	delayTable.insertText("sub1",4,0);


	#if takePerf
		printTable.add_row();
		printTable.insertText("perf.", 0, 4);

		// #if useThreads
			// delayTable.insertText("thread-sync",2,1);
			// delayTable.insertText("cam-process",2,2);
			// delayTable.insertText("main-sub_tr",2,3);

			// printTable.add_row();
			// printTable.add_row();
			// printTable.insertText("perf_t0", 0, 5);
			// printTable.insertText("perf_t1", 0, 6);
			// std::unique_lock<std::mutex> u_lck_tPerfObj0(mtx_perfObj_threads[0], std::defer_lock);
			// std::unique_lock<std::mutex> u_lck_tPerfObj1(mtx_perfObj_threads[1], std::defer_lock);
		// #endif
	#endif

    // #if useThreads
	// 	// std::cout << " Initialising local `std::unique_lock<std::mutex>` and variables starting threads"<<endl;
	// 	ANSI_mvprint(0, 0, " Initialising local `std::unqiue_lock<std::mutex>` and variables starting threads", true, "abs", "rel");
    //     std::unique_lock<std::mutex> u_lck0(mtx[0], std::defer_lock);
    //     std::unique_lock<std::mutex> u_lck1(mtx[1], std::defer_lock);
	std::unique_lock<std::mutex> u_lck_cout(mtx_cout, std::defer_lock);

    //     std::thread t_cam0(thread_task, &camObj[0], 0);
    //     std::thread t_cam1(thread_task, &camObj[1], 1);
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // #endif


	while(true) {
		static bool loopInit=false;

		#if takePerf
		float delay0, delay1, totDelay;
		// perfObj[0].add_checkpoint("loop start");
		#endif

		if(_CONFIG_OPTIONS.get("camObj_useThreads") && !(camObj[0].isThreadLoopInit() && camObj[1].isThreadLoopInit())) {
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

		#if takePerf
		perfObj[0].add_checkpoint("upd.Cam"); /*c1*/
		#endif
		{
			// getCoordinates(camObjPos_main[0], camObjPos_main[1], 2);
			printTable.insertNum(PP[0],1,1,1);
			printTable.insertNum(PP[1],2,1,1);
			printTable.insertNum(PP[2],3,1,1);

			orientObj.update(false);
			printTable.insertNum(orient[0],1,2,1);
			printTable.insertNum(orient[1],2,2,1);
			printTable.insertNum(orient[2],3,2,1);

			printTable.insertNum(camObj[0].data().cnt_pos[0],1,0,1);
			printTable.insertNum(camObj[0].data().cnt_pos[1],2,0,1);
			printTable.insertNum(camObj[1].data().cnt_pos[0],3,0,1);
			printTable.insertNum(camObj[1].data().cnt_pos[1],4,0,1);

			#if takePerf
			perfObj[0].add_checkpoint("getCoords"); /*c2*/
			#endif

			if(HW_KINEMATICS::getAngles(new_q, PP, toRadians(orient[0]), toRadians(orient[1]), toRadians(orient[2]), 1)) {
				if(_init_status.get("pca").isInit()) sendToServo(&pca,new_q,current_q,false);
				for(int i=0; i<6; i++) printTable.insertNum(new_q[i],1+i,3,1);
				printTable.strExport(std::string(21,' ')+"\n");
			}
			else if (HW_KINEMATICS::findValidOrient(PP, orient, orient, new_q)) {
				if(_init_status.get("pca").isInit()) sendToServo(&pca,new_q,current_q,false);
				printTable.insertNum(orient[0],1,2,1);
				printTable.insertNum(orient[1],2,2,1);
				printTable.insertNum(orient[2],3,2,1);
				for(int i=0; i<6; i++) printTable.insertNum(new_q[i],1+i,3,1);
				printTable.strExport(std::string(21,' ')+"\n");
			}
			else {
				printTable.strExport("ERROR:no valid orient\n");
			}

			u_lck_cout.lock();

			ansiPrint(printTable.exportStr, 1.1, 0.1, false, false, "\n", "\n", false);
			ansiPrint(delayTable.exportStr, 1.0, 0.3, false, false, "\n", "\n", false);
			std::cout.flush();

			u_lck_cout.unlock();

			#if takePerf
			perfObj[0].add_checkpoint("toServo"); /*c3*/
			#endif // takePerf
		}
		#if takePerf
		perfObj[0].update_totalInfo(true,false,false);
		totDelay = perfObj[0].delays_ms.at(1);
		printTable.insertNum(totDelay,1,4,1);
		printTable.insertNum(static_cast<float>(1/(totDelay/1000)),2,4,2);
		printTable.add_to_cell("ms",1,4);

		for(size_t i=1; i<perfObj[0].names.size(); i++) {
			delayTable.insertText(perfObj[0].names.at(i),0,i);
			delayTable.insertNum(perfObj[0].delays_ms.at(i),1,i,1);
		}

		#if useThreads
		// float c0_1, c0_2, c0_3, c0_total;
		// float c1_1, c1_2, c1_3, c1_total;
		// if(u_lck_tPerfObj0.try_lock()) {
		// 	// lock_cout(mtx_cout, "\nT[0]: tPerfObj locked",true,true);
		// 	static bool init_delayTable_name = false;
		// 	if(!init_delayTable_name && camProcess_delayNames[0].size()>1) {
		// 		lock_cout(mtx_cout, std::to_string(camProcess_delayNames[0].size()));
		// 		// u_lck_cout.lock();
		// 		// std::cin.get();
		// 		// u_lck_cout.unlock();
		// 		for(int i=0; i<static_cast<int>(camProcess_delayNames[0].size())-static_cast<int>(delayTable.table.size()-4); i++) {
		// 			delayTable.add_row();
		// 		}
		// 		for(size_t i=0; i<camProcess_delayNames[0].size(); i++) {
		// 			delayTable.insertText(
		// 				camProcess_delayNames[0].at(i),
		// 				2, i+4
		// 			);
		// 		}
		// 		init_delayTable_name = true;
		// 	}
		// 	for(size_t i=0; i<camProcess_delays[0].size(); i++) {
		// 		delayTable.insertNum(
		// 			camProcess_delays[0].at(i),
		// 			3, i+4, 1
		// 		);
		// 	}
		// 	c0_1 = perfObj[1].delays_ms.at(1);
		// 	c0_2 = perfObj[1].delays_ms.at(2);
		// 	c0_3 = perfObj[1].delays_ms.at(3);
		// 	for(size_t i=0; i<3; i++) { delayTable.insertNum(perfObj[1].delays_ms.at(i+1),3,i+1); }
		// 	u_lck_tPerfObj0.unlock();

		// 	u_lck_tPerfObj1.lock();
		// 	c1_1 = perfObj[2].delays_ms.at(1);
		// 	c1_2 = perfObj[2].delays_ms.at(2);
		// 	c1_3 = perfObj[2].delays_ms.at(3);
		// 	for(size_t i=0; i<3; i++) { delayTable.insertNum(perfObj[2].delays_ms.at(i+1),4,i+1); }
			
		// 	for(size_t i=0; i<camProcess_delays[1].size(); i++) {
		// 		delayTable.insertNum(
		// 			camProcess_delays[1].at(i),
		// 			4, i+4, 1
		// 		);
		// 	}
		// 	delayTable.strExport("");
		// 	u_lck_tPerfObj1.unlock();
		// }

		// c0_total = c0_1+c0_2+c0_3;
		// c1_total = c1_1+c1_2+c1_3;

		// printTable.insertNum(c0_1,1,5,1);
		// printTable.insertNum(c0_2,2,5,1);
		// printTable.insertNum(c0_3,3,5,1);
		// printTable.insertNum(c0_total,4,5,1);
		// printTable.add_to_cell("ms",4,5);
		// printTable.insertNum(c0_1,1,6,1);
		// printTable.insertNum(c0_2,2,6,1);
		// printTable.insertNum(c0_3,3,6,1);
		// printTable.insertNum(c0_total,4,6,1);
		// printTable.add_to_cell("ms",4,6);
		#endif // useThreads

		#endif // takePerf

		// if(displayTFT) matToTFT()

		if(recordFrames) {
			cv::Mat concatImg;
			cv::hconcat(camObj[0].data().imgFlipped, camObj[1].data().imgFlipped, concatImg);
			recObj.addFrame(concatImg);
		}

		if(_CONFIG_OPTIONS.get("displayToWindow")) {
			cv::Mat winImg;
			cv::hconcat(camObj[0].data().imgFlipped, camObj[1].data().imgFlipped, winImg);
			cv::imshow("Main thread window", winImg);
		}

		int keyInp = cv::waitKey(5);		
		bool breakLoop = false;
		switch (keyInp) {
			case 27: //'esc'
				breakLoop = true;
				break;
			case 32: //'space'
				breakLoop = true;
				break;
			case 115: //'s'
				/*save HSV values*/
				hsv_settingsWrite(HW_HSV[0][0], HW_HSV[0][1], HW_HSV[0][2], HW_HSV[1][0], HW_HSV[1][1], HW_HSV[1][2], 0);
				break;
			case 114: //'r'
				// std::string inputVar = "";
				// int indVar = 0;
				// // std::cout << "Enter index in hsv file\ninput: ";
				// ANSI_mvprint(0, 0, "Enter index in hsv file", true, "abs", "rel");
				// ANSI_mvprint(0, 0, "input: ", true, "abs", "rel");
				// std::cin >> inputVar;
				// if(inputVar=="exit") exit(1);
				// indVar = stoi(inputVar);
				// std::cin.clear();
				// std::cin.ignore();
				// hsv_settingsRead(camObj, HW_HSV[0][0], HW_HSV[0][1], HW_HSV[0][2], HW_HSV[1][0], HW_HSV[1][1], HW_HSV[1][2], window_name, indVar);
				break;
			case 116: //'t'
				// hsv_settingsRead(camObj, HW_HSV[0][0], HW_HSV[0][1], HW_HSV[0][2], HW_HSV[1][0], HW_HSV[1][1], HW_HSV[1][2], window_name, 0);
				break;	
			default:
				break;
		}
		if(breakLoop) break;


		loopInit=true;
		// lock_cout(mtx_cout, "\x1B[2J\n",true,true);
	}
	// if(_CONFIG_OPTIONS.get("displayToWindow"))
	cv::destroyWindow("Main thread window");

	if(recordFrames) recObj.releaseVideo();

	// std::cout << "\nExit called. Exiting." << std::endl;
	ANSI_mvprint(0, 2, "Exit called. Exiting.", true, "abs", "rel");
}