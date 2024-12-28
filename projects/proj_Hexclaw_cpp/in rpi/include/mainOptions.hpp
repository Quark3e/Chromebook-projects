
#pragma once
#ifndef HPP_HEXCLAW_OPTIONS
#define HPP_HEXCLAW_OPTIONS


#include <motion_control/motion_profiles.hpp>
#include <constants.hpp>
#include <includes.hpp>
#include <global_variables.hpp>
#include <stringToFunc.hpp>
#include <createTable.hpp>


labeledFunction hexclaw_cmdArgs{};
createTable cmdArgs_output_table;

void printFuncLabel(std::string functionName) {
	int termDim[2];
	getTermSize(termDim[0], termDim[1]);
	std::cout << std::endl << std::string(termDim[0], '-')<<std::endl;
	std::cout << functionName << std::endl;
	std::cout << std::string(termDim[0], '-')<<std::endl;
}

/// @brief display all the command line argument flags with their descriptions
void cmdArgs_info() {
    cmdArgs_output_table = createTable(3, hexclaw_cmdArgs.getDescriptions().size());

    for(size_t i=0; i<hexclaw_cmdArgs.getDescriptions().size(); i++) {
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getNames()[i][0], 0, i);
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getNames()[i][1], 1, i);
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getDescriptions()[i], 2, i);
    }

    cmdArgs_output_table.strExport("\n",false,"\n","  ");
    std::cout << "\t"<<cmdArgs_output_table.exportStr << std::endl;
	hardExit = true;
}

/// @brief run intro servo movement
void HW_option1_intro();
/// @brief calibrate HSV values
void HW_option2();

/// @brief mode0: run main/default option
void HW_option0();
/// @brief mode 1: track with opencv and display result but don't control/move servo motors
void HW_option3();
/// @brief mode 2: move servo with tracking, don't display in opencv window. just show in terminal
void HW_option4();
/// @brief mode 3: orient mode. Only read and send end-effector orientation from nodemcu device
void HW_option5_orient();
/**
 * mode terminal: control servo motor with terminal input by the user.
 * 
 * ### Functionality:
 * - preset menu: changeable settings like different tweakings for `HW_KINEMATICS::getAngles()`, default values, whether to exit or keep
 * going if value is not found, whether to use `findClosestOrient()`
 * #### input:
 * - run commands from given file:
 * 	-- position commands
 * 	-- orient commands
 * 	-- position commands with given orient
 * - give 3d coordinates which the robot end-effector will try to reach:
 * - give orient which the robot will try to go to for current orientation
 * - give orient which the robot will try to go to from given position
*/
void HW_option6_terminal();

#include "opt6_terminal.hpp"

/**
 * @brief setup for HW command line arguemtns: (there's a better way. I just haven't gotten around to using it)
*/
void HW_setup_options() {
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-h", "--help"}, 		cmdArgs_info, "show help message with info on flags");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-i", "--intro"}, 	HW_option1_intro, "run only servo intro movement");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-c", "--cal"}, 		HW_option2, "calibrate/setup \"Hue Saturation Value\" -values");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-m", "--main"}, 		HW_option0, "run main/default version with servo control, opencv based tracking");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-0", "--mode0"}, 	HW_option0);
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-1", "--mode1"}, 	HW_option3, "track and display with opencv but don't send control movements to robot motors");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-2", "--mode2"}, 	HW_option4, "track and control servo motors with opencv, but don't create and display opencv images");
    hexclaw_cmdArgs.add_func(std::vector<std::string>{"-3", "--mode3"}, 	HW_option5_orient, "only read and send end-effector orientation from nodemcu device to hexclaw robot-arm");
	hexclaw_cmdArgs.add_func(std::vector<std::string>{"-t", "--terminal"},	HW_option6_terminal, "control robot with terminal position inputs");

}


void HW_option1_intro() {
	simplified_init();
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
	usleep(1'000'000);
	if(_init_status.get("pigpio").isInit()) {
		gpioWrite(pin_ledRelay, 0);
		gpioWrite(pin_ledRelay, 1);
	}
	usleep(750'000);
	std::cout<<"\n- section: \"slow start\"\n";
	sendToServo(&pca, new_q, current_q, false, 2, 10);

	std::cout<<"intro finished\n";
	usleep(3'000'000);
	if(_init_status.get("pigpio").isInit()) {
		for(int i=0; i<4; i++) {
			gpioWrite(pin_ledRelay, 0);
			usleep(30'000);
			gpioWrite(pin_ledRelay, 1);
			usleep(30'000);
		}
		usleep(1'500'000);
		gpioWrite(pin_ledRelay, 0);
		usleep(250'000);
		gpioWrite(pin_ledRelay, 1);
		usleep(500'000);
		gpioWrite(pin_ledRelay, 0);
		usleep(100'000);
		gpioWrite(pin_ledRelay, 1);
		usleep(2'000'000);
		gpioWrite(pin_ledRelay, 0);
	}
	usleep(2'000'000);

	if(_init_status.get("pigpio").isInit()) gpioWrite(pin_ledRelay, 1);
	new_q[0] = 45;
	new_q[1] = 0;
	new_q[2] = -45;
	new_q[3] = 89;
	new_q[4] = 89;
	new_q[5] = 0;
	std::cout<<"\n- section: \"crash\"\n";
	sendToServo(&pca,new_q,current_q, false);
	usleep(2'000'000);

}
void HW_option2() {
	simplified_init();
	printFuncLabel(" Running: opt2: calibrate HSV values");

}

void HW_option0() {
	simplified_init();
	printFuncLabel(" Running: opt0: Main loop with opencv and servo control");

	//create windows regardless because i need a way to properly exit the loop
	camObj[0].setup_window("Main thread window");
	camObj[1].setup_window("Main thread window");
	if(!displayToWindow) {
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

		#if useThreads
			delayTable.insertText("thread-sync",2,1);
			delayTable.insertText("cam-process",2,2);
			delayTable.insertText("main-sub_tr",2,3);

			printTable.add_row();
			printTable.add_row();
			printTable.insertText("perf_t0", 0, 5);
			printTable.insertText("perf_t1", 0, 6);
			std::unique_lock<std::mutex> u_lck_tPerfObj0(mtx_perfObj_threads[0], std::defer_lock);
			std::unique_lock<std::mutex> u_lck_tPerfObj1(mtx_perfObj_threads[1], std::defer_lock);
		#endif
	#endif

    #if useThreads
		std::cout << " Initialising local `std::unique_lock<std::mutex>` and variables starting threads"<<endl;
        std::unique_lock<std::mutex> u_lck0(mtx[0], std::defer_lock);
        std::unique_lock<std::mutex> u_lck1(mtx[1], std::defer_lock);
        std::unique_lock<std::mutex> u_lck_cout(mtx_cout, std::defer_lock);

        std::thread t_cam0(thread_task, &camObj[0], 0);
        std::thread t_cam1(thread_task, &camObj[1], 1);
        std::this_thread::sleep_for(1000ms);
    #endif

	while(true) {
		static bool loopInit=false;

		#if takePerf
		float delay0, delay1, totDelay;
		// perfObj[0].add_checkpoint("loop start");
		#endif

		#if useThreads
		if(!threadsInit[2]) {
			if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: NOTE: Threads have not been initialised:\n -initialising."); }

			while(!threadsInit[2]) {
				u_lck0.lock();
				if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: -u_lck0 locked."); std::this_thread::sleep_for(100ms); }
				u_lck1.lock();
				if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: -u_lck1 locked."); std::this_thread::sleep_for(100ms); }
				updateCamVars(0);
				if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: -updateCamVars(0)."); }
				updateCamVars(1);
				if(threadDebug) { lock_cout(mtx_cout, "\nthread:[2]: -updateCamVars(1)."); }
				if(threadsInit[0] && threadsInit[1]) {
					// u_lck_cout.lock();
					// std::cin.get();
					// u_lck_cout.unlock();
					lock_cout(
						mtx_cout, "\n-------------\nSuccessfully initialised both threads:\nstarting tracking:",
						true, true, true
					);
					threadsInit[2] = true;
				}
				if(threadDebug) lock_cout(mtx_cout, ".", true, true);
				std::this_thread::sleep_for(500ms);
				u_lck1.unlock();
				u_lck0.unlock();
			}
			if(threadDebug) lock_cout(mtx_cout, "\nthread:[2]: Initialised!\n",true,true);
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
		#endif
		#if takePerf
		perfObj[0].add_checkpoint("upd.Cam"); /*c1*/
		#endif
		if(numContours_main[0]>0) {
			getCoordinates(2);
			printTable.insertNum(PP[0],1,1,1);
			printTable.insertNum(PP[1],2,1,1);
			printTable.insertNum(PP[2],3,1,1);

			orientObj.update(false);
			printTable.insertNum(orient[0],1,2,1);
			printTable.insertNum(orient[1],2,2,1);
			printTable.insertNum(orient[2],3,2,1);

			printTable.insertNum(camObjPos_main[0][0],1,0,1);
			printTable.insertNum(camObjPos_main[0][1],2,0,1);
			printTable.insertNum(camObjPos_main[1][0],3,0,1);
			printTable.insertNum(camObjPos_main[1][1],4,0,1);

			#if takePerf
			perfObj[0].add_checkpoint("getCoords"); /*c2*/
			#endif

			if(HW_KINEMATICS::getAngles(
				new_q, PP,
				toRadians(orient[0]),
				toRadians(orient[1]),
				toRadians(orient[2]),
				1
			)) {
				sendToServo(&pca,new_q,current_q,false);
				for(int i=0; i<6; i++) printTable.insertNum(new_q[i],1+i,3,1);
				printTable.strExport(std::string(21,' ')+"\n");
			}
			else if (HW_KINEMATICS::findValidOrient(
				PP, orient, orient, new_q
			)) {
				sendToServo(&pca,new_q,current_q,false);
				printTable.insertNum(orient[0],1,2,1);
				printTable.insertNum(orient[1],2,2,1);
				printTable.insertNum(orient[2],3,2,1);
				for(int i=0; i<6; i++) printTable.insertNum(new_q[i],1+i,3,1);
				printTable.strExport(std::string(21,' ')+"\n");
			}
			else {
				printTable.strExport("ERROR:no valid orient\n");
			}
			#if useThreads
			u_lck_cout.lock();
			#endif
			ansiPrint(
				printTable.exportStr,
				static_cast<float>(1),
				static_cast<float>(0.1),
				false,
				false,
				"\n","\n",false
			);
			ansiPrint(
				delayTable.exportStr,
				static_cast<float>(1),
				static_cast<float>(0.3),
				false,
				false,
				"\n","\n",false
			);
			std::cout.flush();
			#if useThreads
			u_lck_cout.unlock();
			#endif
			#if takePerf
			perfObj[0].add_checkpoint("toServo"); /*c3*/
			#endif
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
		float c0_1, c0_2, c0_3, c0_total;
		float c1_1, c1_2, c1_3, c1_total;
		if(u_lck_tPerfObj0.try_lock()) {
			// lock_cout(mtx_cout, "\nT[0]: tPerfObj locked",true,true);
			static bool init_delayTable_name = false;
			if(!init_delayTable_name && camProcess_delayNames[0].size()>1) {
				lock_cout(mtx_cout,to_string(camProcess_delayNames[0].size()));
				// u_lck_cout.lock();
				// std::cin.get();
				// u_lck_cout.unlock();
				for(int i=0; i<static_cast<int>(camProcess_delayNames[0].size())-static_cast<int>(delayTable.table.size()-4); i++) {
					delayTable.add_row();
				}
				for(size_t i=0; i<camProcess_delayNames[0].size(); i++) {
					delayTable.insertText(
						camProcess_delayNames[0].at(i),
						2, i+4
					);
				}
				init_delayTable_name = true;
			}
			for(size_t i=0; i<camProcess_delays[0].size(); i++) {
				delayTable.insertNum(
					camProcess_delays[0].at(i),
					3, i+4, 1
				);
			}
			c0_1 = perfObj[1].delays_ms.at(1);
			c0_2 = perfObj[1].delays_ms.at(2);
			c0_3 = perfObj[1].delays_ms.at(3);
			for(size_t i=0; i<3; i++) { delayTable.insertNum(perfObj[1].delays_ms.at(i+1),3,i+1); }
			u_lck_tPerfObj0.unlock();

			u_lck_tPerfObj1.lock();
			c1_1 = perfObj[2].delays_ms.at(1);
			c1_2 = perfObj[2].delays_ms.at(2);
			c1_3 = perfObj[2].delays_ms.at(3);
			for(size_t i=0; i<3; i++) { delayTable.insertNum(perfObj[2].delays_ms.at(i+1),4,i+1); }
			
			for(size_t i=0; i<camProcess_delays[1].size(); i++) {
				delayTable.insertNum(
					camProcess_delays[1].at(i),
					4, i+4, 1
				);
			}
			delayTable.strExport("");
			u_lck_tPerfObj1.unlock();
		}

		c0_total = c0_1+c0_2+c0_3;
		c1_total = c1_1+c1_2+c1_3;

		printTable.insertNum(c0_1,1,5,1);
		printTable.insertNum(c0_2,2,5,1);
		printTable.insertNum(c0_3,3,5,1);
		printTable.insertNum(c0_total,4,5,1);
		printTable.add_to_cell("ms",4,5);
		printTable.insertNum(c0_1,1,6,1);
		printTable.insertNum(c0_2,2,6,1);
		printTable.insertNum(c0_3,3,6,1);
		printTable.insertNum(c0_total,4,6,1);
		printTable.add_to_cell("ms",4,6);
		#endif

		#endif

		// if(displayTFT) matToTFT()

		if(recordFrames) {
			cv::Mat concatImg;
			cv::hconcat(flippedImg_main[0], flippedImg_main[1], concatImg);
			recObj.addFrame(concatImg);
		}

		if(displayToWindow) {
			cv::Mat winImg;
			cv::hconcat(flippedImg_main[0], flippedImg_main[1], winImg);
			cv::imshow("Main thread window", winImg);
		}
		int keyInp = cv::waitKey(5);
		if(keyInp==27) break; //'esc'
		else if(keyInp==32) break; //'space'
		else if(keyInp==115) { /*'s'*/
			/*save HSV values*/
			hsv_settingsWrite(HW_HSV, 0);
		}
		else if(keyInp==114) { //'r'
			string inputVar = "";
			int indVar = 0;
			std::cout << "Enter index in hsv file\ninput: ";
			std::cin >> inputVar;
			if(inputVar=="exit") exit(1);
			indVar = stoi(inputVar);
			cin.clear();
			cin.ignore();
			hsv_settingsRead(camObj, HW_HSV, window_name, indVar);
		}
		else if(keyInp==116) { /*'t'*/ hsv_settingsRead(camObj, HW_HSV, window_name, 0); }
		


		loopInit=true;
		// lock_cout(mtx_cout, "\x1B[2J\n",true,true);
	}
	// if(displayToWindow)
	cv::destroyWindow("Main thread window");

	if(recordFrames) recObj.releaseVideo();

	u_lck0.lock();
	exit_thread[0] = true;
	u_lck0.unlock();
	u_lck1.lock();
	exit_thread[1] = true;
	u_lck1.unlock();
	std::cout << "\nExit called. Exiting." << std::endl;
}
void HW_option3() {
	simplified_init();
	printFuncLabel(" Running: opt3: display opencv but don't move servo");

}
void HW_option4() {
	simplified_init();
	printFuncLabel(" Running: opt4: move servo but don't display opencv im on window");

}
void HW_option5_orient() {
	simplified_init();
	printFuncLabel(" Running: opt5: Only orientation movement on servo robot");

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

			sendToServo(&pca, new_q, current_q, false);
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



#endif
