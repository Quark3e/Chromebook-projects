
#pragma once
#ifndef HPP_HEXCLAW_OPTIONS
#define HPP_HEXCLAW_OPTIONS


#include "../hexclaw_constants.hpp"
#include "../hexclaw_includes.hpp"
#include "../hexclaw_global.hpp"
#include "../../../../teststuff/cpp/useful/stringToFunc.hpp"
#include "../../../../teststuff/cpp/useful/createTable.hpp"



StringToFunction hexclaw_cmdArgs{};
createTable cmdArgs_output_table;

/// @brief display all the command line argument flags with their descriptions
void cmdArgs_info() {
    cmdArgs_output_table = createTable(3, hexclaw_cmdArgs.getDescriptions().size());
    for(size_t i=0; i<hexclaw_cmdArgs.getDescriptions().size(); i++) {
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getNames()[i][0], 0, 0);
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getNames()[i][1], 1, 0);
        cmdArgs_output_table.insertText(hexclaw_cmdArgs.getDescriptions()[i], 2, 0);
    }
    cmdArgs_output_table.strExport("",false,"\n","\t ");
    std::cout << cmdArgs_output_table.exportStr << std::endl;
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
void HW_option5();

void HW_setup_options() {
	std::cout << "setup"<<std::endl;
    hexclaw_cmdArgs.add_func("-h", cmdArgs_info, "--help", "show help message with info on flags");
    hexclaw_cmdArgs.add_func("-i", HW_option1_intro, "--intro", "run only servo intro movement");
    hexclaw_cmdArgs.add_func("-c", HW_option2, "--cal", "calibrate/setup \"Hue Saturation Value\" -values");
    hexclaw_cmdArgs.add_func("-m", HW_option0, "--main", "run main/default version with servo control, opencv based tracking");
    hexclaw_cmdArgs.add_func("-0", HW_option0, "--mode0");
    hexclaw_cmdArgs.add_func("-1", HW_option3, "--mode1", "track and display with opencv but don't send control movements to robot motors");
    hexclaw_cmdArgs.add_func("-2", HW_option4, "--mode2", "track and control servo motors with opencv, but don't create and display opencv images");
    hexclaw_cmdArgs.add_func("-3", HW_option5, "--mode3", "only read and send end-effector orientation from nodemcu device to hexclaw robot-arm");
	std::cout<<"seutp end" << std::endl;
}


void HW_option1_intro() {
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
		if(pigpioInitia) {
			gpioWrite(pin_ledRelay, 0);
			gpioWrite(pin_ledRelay, 1);
		}
		usleep(750'000);
		printf("\n- section: \"slow start\"\n");
		sendToServo(&pca, new_q, current_q, false, 2, 10);

		printf("intro finished\n");
		usleep(3'000'000);
		if(pigpioInitia) {
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

		if(pigpioInitia) gpioWrite(pin_ledRelay, 1);
		new_q[0] = 45;
		new_q[1] = 0;
		new_q[2] = -45;
		new_q[3] = 89;
		new_q[4] = 89;
		new_q[5] = 0;
		printf("\n- section: \"crash\"\n");
		sendToServo(&pca,new_q,current_q, false);
		usleep(2'000'000);

}
void HW_option2() {

}

void HW_option0() {

	if(displayToWindow) {
		camObj[0].setup_window("Main thread window");
		camObj[1].setup_window("Main thread window");
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
	*/
	createTable printTable(7, 4);
	printTable.insertText("camPos", 0, 0);
	printTable.insertText("PP", 0, 1);
	printTable.insertText("orient", 0, 2);
	printTable.insertText("servo", 0, 3);

	#if takePerf
		printTable.add_row();
		printTable.add_row();
		printTable.insertText("perf.", 0, 6);
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
		#if takePerf
		float delay0, delay1, totDelay;
		perfObj[0].add_checkpoint("loop start");
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
				if(threadsInit[0] && threadsInit[1]) threadsInit[2] = true;
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
		if(numContours_main[0]>0) {
			getCoordinates(2);
			printTable.insertNum(PP[0],1,1,1);
			printTable.insertNum(PP[1],2,1,1);
			printTable.insertNum(PP[2],3,1,1);

			orientObj.update(false);
			printTable.insertNum(orient[0],1,2,1);
			printTable.insertNum(orient[1],2,2,1);
			printTable.insertNum(orient[2],3,2,1);

			if(getAngles(
				new_q, PP,
				toRadians(orient[0]),
				toRadians(orient[1]),
				toRadians(orient[2]),
				1
			)) {
				sendToServo(&pca,new_q,current_q,false);
				for(int i=0; i<6; i++) printTable.insertNum(new_q[i],1+i,3,1);
				printTable.strExport("\n");
			}
			else if (findValidOrient(
				PP, orient, orient, new_q
			)) {
				sendToServo(&pca,new_q,current_q,false);
				printTable.insertNum(orient[0],1,2,1);
				printTable.insertNum(orient[1],2,2,1);
				printTable.insertNum(orient[2],3,2,1);
				for(int i=0; i<6; i++) printTable.insertNum(new_q[i],1+i,3,1);
				printTable.strExport("\n");
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
				static_cast<float>(0.1)
			);
			cout.flush();
			#if useThreads
			u_lck_cout.unlock();
			#endif
		}
		#if takePerf
		totDelay = perfObj[0].delays_ms.at(1);
		printTable.insertNum(totDelay,1,4,1);
		printTable.insertNum(static_cast<float>(1/(totDelay/1000)),2,4,2);
		printTable.add_to_cell("ms",1,4);
		#endif

		// if(displayTFT) matToTFT()

		if(displayToWindow) {
			cv::Mat winImg;
			cv::hconcat(flippedImg_main[0], flippedImg_main[1], winImg);

			int keyInp = cv::waitKey(10);
			#if useThreads
			cv::imshow("Main thread window", winImg);
			#endif
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
		
		}
	}
	if(displayToWindow) cv::destroyWindow("Main thread window");
	u_lck0.lock();
	exit_thread[0] = true;
	u_lck0.unlock();
	u_lck1.lock();
	exit_thread[1] = true;
	u_lck1.unlock();
	std::cout << "\nExit called. Exiting." << std::endl;
}
void HW_option3() {

}
void HW_option4() {

}
void HW_option5() {
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
		if(getAngles(
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
		cout.flush();
	}
}



#endif