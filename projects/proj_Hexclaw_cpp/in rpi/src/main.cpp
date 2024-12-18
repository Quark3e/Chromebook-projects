
/*
 * Project Name: Hexclaw  
 * File: main.cpp
 * Description: Robot Arm Control project  
 * Author: Erkhme Byambadorj.
 * Created May 2023
 * Description: See URL for full details.
 * NOTE :: NOT FINISHED
 * URL: shit
 */


#include <includes.hpp>
#include <servo_control.hpp>
#include <constants.hpp>
#include <global_variables.hpp>


/// @brief 0-direct function; 1-use 2d coefs; 2-use arificial; 3-use two_cam_triangle
int zSol=3;

void initPaths() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    const char *pathP;
    if (count != -1) {
        pathP = dirname(result);
    }

    char path[100];
    std::string temp = strcat(strcpy(path, pathP), "/");
    absPath = temp.substr(0, temp.find("/projects"))+"/teststuff/python/opencv/angleArea/data/csv_artif/";
}



/// @brief 2d coefficients for a single layer
float angleArea_coef[181][181];
/// @brief 3d artifically pre-generated area results
float artifVal[181][181][401]; //x = [0, 90, 181] = [-90, 0, 01]

void load_csvFile(std::string filePath = "data/csv_dataSet_pf17_fuse-True.csv") {
    printf("Loading csv file:\n");
	std::ifstream csvFile(filePath);
	if(!csvFile.is_open()) {
		printf("error: Cannot open csv file");
		std::cout << filePath << "\"\n";
	}
	std::string line;
	std::getline(csvFile, line);
	while(std::getline(csvFile, line)) {
		int x, y;
		x = stof(line.substr(0, line.find(",")));
		line.erase(0, line.find(",")+1);
		y = stof(line.substr(0, line.find(",")));
		line.erase(0, line.find(",")+1);
        x = int(round(x+90));
        y = int(round(y+90));
        float saveVal = stof(line.substr(0, line.find(",")));
		angleArea_coef[x][y] = saveVal;
        printf("- [%d][%d]: %f\r", x, y, saveVal);
	}
	std::cout << std::endl;
	csvFile.close();

}


/// @brief solve z-axis/cam-distance from area
/// @param area [float]: cntArea
/// @param posX [float]: x coordinate of the object [NOTE: assuming center of screen=[0, 0]]
/// @param posY [float]: y coordinate
/// @return [float]: assumed z-axis value
float zAxisFunc(float area, float posX, float posY) {
	float val=0, ans = 0;

    float c[11] = {
		 1.41350147 * pow(10, -32),
		-7.39588055 * pow(10, -28),
		 1.70543946 * pow(10, -23),
		-2.27903757 * pow(10, -19),
		 1.95152915 * pow(10, -15),
		-1.11701396 * pow(10, -11),
		 4.32049796 * pow(10, -8),
		-1.11313493 * pow(10, -4),
		 1.82568080 * pow(10, -1),
		-1.72185711 * pow(10, 2),
		 7.15691211 * pow(10, 4)
			 };
	for(int i=0; i<11; i++) { val+=c[i]*pow(area, 10-i); }
	if(zSol==0) return val;
	/*
	[-x:x] = [alpha:-alpha]
	[-y:y] = [-beta:beta]
	*/
	// if(zSol==1) {
	// 	ans = val * (1 / angleArea_coef[
	// 			int(round(/*orient[0]*/-(0-posX*angPerPix)))+90
	// 		][
	// 			int(round(/*orient[1]+*/posY*angPerPix))+90
	// 		]);
	// 	return ans;
	// }
	
	if(zSol==3) {
		
	}

	int chosenIdx=0;
	chosenIdx = getClosestValIdx(artifVal[int(Roll)+90][int(Pitch)+90],401,int(area));
	// std::cout << chosenIdx << artifVal[int(Roll)+90][int(Pitch)+90][200]<< "\t";
	return chosenIdx;
}




/// @brief main function to read, display and control the robot
/// @param cap [cv::VideoCapture object pointer]: for main camera
/// @param mode [integer]: -0: setup/calibrate hsv; -1: w. sendToServo; -2: without sendToServo; -3: w. sendToServo without cv disp.
/// @param pcaSrc [PiPCA9682 object pointer]: for pca9685 board
/// @return [integer]: 0 - normal function finish; -1 - exit program entirely
/**
int displayFunc(int mode, PiPCA9685::PCA9685* pcaSrc) {
	
	//mode:
	//- 0: setup/calibrate hsv
	//- 1: main color tracking loop with sendToServo
	//- 2: main color tracking loop without sendToServo
	//- 3: main color tracking loop with sendToServo without display to a window

	std::cout << "- In \"displayFunc()\"" << std::endl;

	if(mode!=3) {
		camObj[0].setup_window();
		camObj[1].setup_window();
	}

	while(true) {
		#if useThreads
			thread t_cam0(cam_thread_task, ref(camObj[0]));
			thread t_cam1(cam_thread_task, ref(camObj[1]));
			t_cam0.join();
			t_cam1.join();
		#elif !useThreads
			if(camObj[0].processCam()==-1) return 0;
			if(zSol==3 && camObj[1].processCam()==-1) return 0;
		#endif
		if(camObj[0].processReturnCode==-1) {
			std::cout << "ERROR: displayFunc(): camObj[0].processCam() returned -1." << std::endl;
			return 1;
		}
		else if(camObj[1].processReturnCode==-1) {
			std::cout << "ERROR: displayFunc(): camObj[1].processCam() returned -1." << std::endl;
			return 1;
		}

		std::cout << "zSol=" << zSol << ": ";
		if(mode >= 1 && camObj[0].allCnt_pos.size()>0) {
			if(mode==1 || mode==3) {
				cv::Size camSize = camObj[0].imgFlipped.size();
				float camPos[2][2], inpPos[2], solvedZ;
				camPos[0][0] = camObj[0].totCnt_pos[0];
				camPos[0][1] = camObj[0].totCnt_pos[1];
				if(zSol==3 && camObj[1].allCnt_pos.size()>0 && camObj[0].allCnt_pos.size()>0) {
					camPos[1][0] = camObj[1].totCnt_pos[0];
					camPos[1][1] = camObj[1].totCnt_pos[1];
					inpPos[0] = camPos[0][0];
					inpPos[1] = camPos[1][0];
					camTri.solvePos(inpPos, solvedPos, false);
					solvedZ = -sin(toRadians((camTri.camRes[0][1]*0.5-camObj[0].totCnt_pos[1])*camTri.camCoef[0][1]))*solvedPos[1];
					PP[0] = axisFilter[0]*float(round(solvedPos[0]*axisScal[0]+axisOffset[0])) + (1-axisFilter[0])*PP[0];
					PP[1] = axisFilter[1]*float(solvedZ*axisScal[1]+axisOffset[1]) + (1-axisFilter[1])*PP[1];
					PP[2] = axisFilter[2]*float(round(solvedPos[1]*axisScal[2]+axisOffset[2])) + (1-axisFilter[2])*PP[2];
				}
				else {
					inpPos[0] = camPos[0][0];
					inpPos[1] = camPos[0][1];
					PP[0] = axisFilter[0]*float(round(float(inpPos[0] - camSize.width/2)*axisScal[0]) + axisOffset[0]) + (1-axisFilter[0])*PP[0];
					PP[1] = axisFilter[1]*float(round(float(camSize.height - inpPos[1])*axisScal[1]) + axisOffset[1]) + (1-axisFilter[1])*PP[1];
					PP[2] = round((axisFilter[2]*float(axisScal[2]*zAxisFunc(camObj[0].totCnt_area, inpPos[0], inpPos[1]) + axisOffset[2]) + (1-axisFilter[2])*PP[2])/10)*10;
				}

				printf(" x:%3d y:%3d z:%3d",int(PP[0]),int(PP[1]), int(PP[2]));
				orientObj.update(true);
				if(HW_KINEMATICS::getAngles(new_q,PP,toRadians(orient[0]),toRadians(orient[1]),toRadians(orient[2]),1)) { sendToServo(pcaSrc,new_q,current_q,false); }
				else if(HW_KINEMATICS::findValidOrient(PP, orient, orient, new_q)) { sendToServo(pcaSrc,new_q,current_q,false); }
				else printf("valid orient for (%d, %d, %d) not found\n", int(PP[0]), int(PP[1]), int(PP[2]));
			}
		}


		cv::Mat fusedImg;
		if(mode!=3) {
			if(zSol==3) { cv::hconcat(camObj[0].imgFlipped,camObj[1].imgFlipped,fusedImg); }
			else { fusedImg = camObj[0].imgFlipped; }
		}
		if(displayTFT) {
			matToTFT(camObj[0].imgThreshold);
		}
		

		//delay: 6-11ms
		if(mode!=3) {
			int keyInp = cv::waitKey(10);
			cv::imshow(window_name, fusedImg);
			// printf(" %d ", keyInp);
			if(keyInp==27) return -1; //'esc'
			else if(keyInp==32) break; //'space'
			else if(keyInp==115) {
				//'s'
				if(mode==0) {
					//save HSV values/
					hsv_settingsWrite(HW_HSV, 0);
				}
			}
			else if(keyInp==114) { //'r'
				std::string inputVar = "";
				int indVar = 0;
				std::cout << "input: ";
				cin >> inputVar;
				if(inputVar=="exit") exit(1);
				indVar = stoi(inputVar);
				cin.clear();
				cin.ignore();
				hsv_settingsRead(camObj, HW_HSV, window_name, indVar);
			}
			else if(keyInp==116 && mode==0) {
				//'t'
				hsv_settingsRead(camObj, HW_HSV, window_name, 0);
			}
		}
		printf("\n");
	}
	if(mode!=3) cv::destroyWindow(window_name);
	return 0;
}
*/

void loadData_csvArtif(bool printVar=true) {
	if (printVar) std::cout << "Starting to load the data\n";

	for(int x=0; x<181; x++) {
		for(int y=0; y<181; y++) {
			for(int z=0; z<401; z++) {
				artifVal[x][y][z]=-1;
			}
		}
	}

	std::string filename;
	std::string filenom[2] = {"csv_[1,1,1]_6568781_p", "_artificial.csv"};

	int columns=4;
	int parts=4;
	char temp[16]="0123456789;,.- ";
	int rowCount=0;
	float tempArr[4];
	bool fullBreak;
	std::string line;


	for(int part=0; part<parts; part++) {
		std::cout << "- p" << part << std::endl;
		rowCount=0;
		std::fstream csvFile;

		filename = absPath+filenom[0]+std::to_string(part)+filenom[1];
		csvFile.open(filename, ios::in);
		if(!csvFile.is_open()) std::cout << "error: file \""<< filename <<"\" could not be opened" << std::endl;
		
		std::getline(csvFile, line);
		while(std::getline(csvFile, line)) {
			int idx=0;
			fullBreak=false;
			for(int n=0; n<100; n++) {
				for(int i=0; i<sizeof(temp)/sizeof(temp[0]); i++) {
					if(line[n] == temp[i]) break;
					else if(i>=sizeof(temp)/sizeof(temp[0])-1) {
						idx=n;
						fullBreak=true;
						break;
					}
				}
				if(fullBreak) break;
			}
			splitString(line.substr(0, idx), ",", tempArr, columns, false);
			if(printVar) {
				printf(
					"- p%d   %7d |%36s| x:%3d y:%3d z:%4d area:%0.4f        \r",
					part, rowCount, line.substr(0, idx).c_str(),
					int(tempArr[0]), int(tempArr[1]), int(tempArr[2]), tempArr[3]
				);
			}
			artifVal[int(tempArr[0])+90][int(tempArr[1])+90][int(tempArr[2])] = round(tempArr[3]);
			rowCount++;
			
		}
		csvFile.close();
	}
	if(printVar) std::cout << "\nFinished loading the data: Total rows:" << rowCount << std::endl;
	std::cout << artifVal[90][90][200] << std::endl;
	// std::cout << "paused:";
	// std::string inp;
	// cin >> inp;
	// cin.ignore();
	// cin.clear();
}



#include <mainOptions.hpp>


int subMenuPos[2] = {20, 0};
// void subMenu_exit() {
// 	hardExit = true;
// 	modeMenu.exitDriver = true;
// 	startMenu.exitDriver = true;
// }
void subMenu_mode() {

	modeMenu.addOpt("0. Main", 0, 0, -1, HW_option0);
	modeMenu.addOpt("1. Servo motor disconnected", 0, 1, -1, HW_option3);
	modeMenu.addOpt("2. Don't display opencv", 0, 2, -1, HW_option4);
	modeMenu.addOpt("3. Orient movement", 0, 3, -1, HW_option5_orient);
	modeMenu.addOpt("Back", 0, 4, 27, bool_true, &modeMenu.exitDriver);
	modeMenu.addOpt("Exit", 0, 5, 'e', exitFrom_lvl2, &modeMenu.exitDriver);

	modeMenu.driver(subMenuPos[0], subMenuPos[1], 0, false);
}



int main(int argc, char* argv[]) {

	std::cout<<"\x1B[2J"<<std::endl;

	int termSize[2] = {0, 0};
	getTermSize(termSize[0], termSize[1]);

	
	startMenu.addOpt("[1]   Intro    ", 0, 0, '1', HW_option1_intro);
	startMenu.addOpt("[2]   Calibrate", 0, 1, '2', HW_option2);
	startMenu.addOpt("[3]   Main     ", 0, 2, '3', HW_option0);
	startMenu.addOpt("[t]   Terminal ", 0, 3, 't', HW_option6_terminal);
	startMenu.addOpt("[m]   mode:    ", 0, 4, 'm', subMenu_mode);
	startMenu.addOpt("[esc] Exit     ", 0, 5, 27, bool_true, &startMenu.exitDriver);


	// startMenu.setButtonWidth(10);


	//pca9685 board setup
	pca.set_pwm_freq(50.0);

	if(recordFrames) {
		recObj = opencv_recorder(
			"Hexclaw_cameraFeeds",
			prefSize[0]*2,
			prefSize[1]*2,
			15
		);
	}


	if(gpioInitialise()<0) {
		std::cout << "pigpio \"gpioInitialise()\" failed\n";
		pigpioInitia = false;
	}
	else {
		gpioSetMode(pin_ledRelay, PI_OUTPUT);
		pigpioInitia = true;
		gpioWrite(pin_ledRelay, 1);
	}

	sendToServo(&pca, new_q, current_q, true);

	HW_setup_options();




	if(argc==2) {
		if(hexclaw_cmdArgs.call_func(argv[1])==1) {
			std::cout << "ERROR: no matching flag or argument input"<<std::endl;
			return 1;
		}
	}
	else {
		std::cout<<"\x1B[2J"<<std::endl;
		startMenu.driver(1, 1, 0, false);
	}



	if(startMenu.exitDriver) {
		std::cout << "\x1B[H"<<"\x1B[2J";
		std::cout.flush();
		return 0;
	}

	for(int n=0; n<6; n++) new_q[n] = startup_q[n];
	printf("\n- section: \"closing\"\n");
	sendToServo(&pca, new_q, current_q, false, 2, 2);
	if(pigpioInitia) {
		gpioWrite(pin_ledRelay, 0);
		gpioTerminate();
	}

	std::cout << "\x1B[H";
	std::cout.flush();
	return 0;
}
