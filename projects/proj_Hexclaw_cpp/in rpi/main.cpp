
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


// default/basic headers/includes
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream> //read and write to file (hsv_settings.dat)
#include <algorithm> //ex. find() and erase()
#include <time.h>
#include <stdint.h>
#include <vector>

// path related headers
#include <cstring>
#include <unistd.h>
#include <libgen.h>
#include <linux/limits.h>

// opencv/image tracking
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>


// PCA9685 communication
#include <PiPCA9685/PCA9685.h>

// RPi specific functions
#include <pigpio.h>


// Personal 
#include "hexclaw_includes.hpp"


using namespace std;


// thread stuff
#define useThreads true



#if useThreads
#include <thread>
#include <mutex>




/// @brief thread intermediary function so class member function is passable to thread object.
/// Function calls IR_camTracking::processCam() member function
/// @param camRef reference to IR_camTracking class object
void cam_thread_task(IR_camTracking& camRef) {
	camRef.processCam();
}

#endif


string absPath;

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
    string temp = strcat(strcpy(path, pathP), "/");
    absPath = temp.substr(0, temp.find("/projects"))+"/teststuff/python/opencv/angleArea/data/csv_artif/";
}


// IK related: ik calc variable declaration
float current_q[6]	= {0,0,0,0,0,0}; //old_rotation
float new_q[6]		= {0,0,0,0,0,0};
float orient[3]		= {0,0,0}; //degrees
float PP[3]			= {0,150,150};
float axisScal[3]	= {1, 1, 1};	
float axisOffset[3]	= {0, 100, -200};
float axisFilter[3]	= {1, 1, 1};

float pitch, roll, Pitch=0, Roll=0;
// Wireless nodemcu udp COM header class initialization
nodemcu_orient orientObj(orient);


/// @brief prefered image size to convert/use for all images
int prefSize[2] = {640, 480};


bool useAutoBrightne = true;
bool takePerformance = false;


/// @brief index of webcam
int webcamIndex = 2;


bool displayToWindow = true;
bool calibrateHSV = false;
bool displayTFT = false;

bool mode_orients = false;
bool mode_intro = false;


// int l_HSV[3] = {0, 0, 255};
// int u_HSV[3] = {179, 9, 255};
int HW_HSV[2][3] = {
	{0, 0, 255},
	{179, 9, 255}
};

const char* window_name = "Window";
// IR camtracking header class initialization
IR_camTracking camObj[2] {
	{2, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance},
	{0, prefSize[0], prefSize[1], useAutoBrightne, displayToWindow, takePerformance},
};

// Two_cam_triangle header class initialisation
float camPosition[2][2] = {{0, 0}, {250, 0}};
float camAng_offs[2] = {90, 123};
float inpPos[2], solvedPos[2];
camTriangle camTri(camPosition, camAng_offs);


/// @brief 2d coefficients for a single layer
float angleArea_coef[181][181];
/// @brief 3d artifically pre-generated area results
float artifVal[181][181][401]; //x = [0, 90, 181] = [-90, 0, 01]

void load_csvFile(string filePath = "data/csv_dataSet_pf17_fuse-True.csv") {
    printf("Loading csv file:\n");
	ifstream csvFile(filePath);
	if(!csvFile.is_open()) {
		printf("error: Cannot open csv file");
		cout << filePath << "\"\n";
	}
	string line;
	getline(csvFile, line);
	while(getline(csvFile, line)) {
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
	cout << endl;
	csvFile.close();

}



bool useCSV=true;

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
	// cout << chosenIdx << artifVal[int(Roll)+90][int(Pitch)+90][200]<< "\t";
	return chosenIdx;
}



bool pigpioInitia = false;
int pin_ledRelay = 23;


#include "HW_options/mainOptions.hpp"

/// @brief main function to read, display and control the robot
/// @param cap [cv::VideoCapture object pointer]: for main camera
/// @param mode [integer]: -0: setup/calibrate hsv; -1: w. sendToServo; -2: without sendToServo; -3: w. sendToServo without cv disp.
/// @param pcaSrc [PiPCA9682 object pointer]: for pca9685 board
/// @return [integer]: 0 - normal function finish; -1 - exit program entirely
int displayFunc(int mode, PiPCA9685::PCA9685* pcaSrc) {
	/*
	mode:
	- 0: setup/calibrate hsv
	- 1: main color tracking loop with sendToServo
	- 2: main color tracking loop without sendToServo
	- 3: main color tracking loop with sendToServo without display to a window
	*/

	cout << "- In \"displayFunc()\"" << endl;

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
			cout << "ERROR: displayFunc(): camObj[0].processCam() returned -1." << endl;
			return 1;
		}
		else if(camObj[1].processReturnCode==-1) {
			cout << "ERROR: displayFunc(): camObj[1].processCam() returned -1." << endl;
			return 1;
		}

		cout << "zSol=" << zSol << ": ";
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
				if(getAngles(new_q,PP,toRadians(orient[0]),toRadians(orient[1]),toRadians(orient[2]),1)) { sendToServo(pcaSrc,new_q,current_q,false); }
				else if(findValidOrient(PP, orient, orient, new_q)) { sendToServo(pcaSrc,new_q,current_q,false); }
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
			else if(keyInp==115) { /*'s'*/ if(mode==0) { /*save HSV values*/ hsv_settingsWrite(HW_HSV, 0); } }
			else if(keyInp==114) { //'r'
				string inputVar = "";
				int indVar = 0;
				cout << "input: ";
				cin >> inputVar;
				if(inputVar=="exit") exit(1);
				indVar = stoi(inputVar);
				cin.clear();
				cin.ignore();
				hsv_settingsRead(camObj, HW_HSV, window_name, indVar);
			}
			else if(keyInp==116 && mode==0) { /*'t'*/ hsv_settingsRead(camObj, HW_HSV, window_name, 0); }
		}
		printf("\n");
	}
	if(mode!=3) cv::destroyWindow(window_name);
	return 0;
}



void loadData_csvArtif(bool printVar=true) {
	if (printVar) cout << "Starting to load the data\n";

	for(int x=0; x<181; x++) {
		for(int y=0; y<181; y++) {
			for(int z=0; z<401; z++) {
				artifVal[x][y][z]=-1;
			}
		}
	}

	string filename;
	string filenom[2] = {"csv_[1,1,1]_6568781_p", "_artificial.csv"};

	int columns=4;
	int parts=4;
	char temp[16]="0123456789;,.- ";
	int rowCount=0;
	float tempArr[4];
	bool fullBreak;
	string line;


	for(int part=0; part<parts; part++) {
		cout << "- p" << part << endl;
		rowCount=0;
		fstream csvFile;

		filename = absPath+filenom[0]+to_string(part)+filenom[1];
		csvFile.open(filename, ios::in);
		if(!csvFile.is_open()) cout << "error: file \""<< filename <<"\" could not be opened" << endl;
		
		getline(csvFile, line);
		while(getline(csvFile, line)) {
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
	if(printVar) cout << "\nFinished loading the data: Total rows:" << rowCount << endl;
	cout << artifVal[90][90][200] << endl;
	// cout << "paused:";
	// string inp;
	// cin >> inp;
	// cin.ignore();
	// cin.clear();
}


int main(int argc, char* argv[]) {
	/*
	argv codes:
	- argc <= 1:		calibrateHSV = false;	displayToWindow = false; #default
	- argv[1] == "-0":	calibrateHSV = false;	displayToWindow = true;
	- argv[1] == "-1":	calibrateHSV = true;	displayToWindow = false;
	- argv[1] == "-2":	calibrateHSV = true;	displayToWindow = true;
	- argv[1] == "-3":	special mode: no display and no tracking or reading of position.
						where given pos is stationary and only reads orient var from esp
	*/

	// signal(SIGINT	,signal_handler);

	//nodemcu udp communication setup/initialization
	// nodemcu_udp_setup();


	if(zSol==1) load_csvFile();
	initPaths();
	
	if(zSol==2) loadData_csvArtif(false);

	if(gpioInitialise()<0) {
		cout << "pigpio \"gpioInitialise()\" failed\n";
		pigpioInitia = false;
	}
	else {
		gpioSetMode(pin_ledRelay, PI_OUTPUT);
		pigpioInitia = true;
		gpioWrite(pin_ledRelay, 1);
	}

	if(argc<=1) {calibrateHSV=false; displayToWindow=false;}
	else if(argc>=2) {
		if(strcmp(argv[1], "-0")==0) {calibrateHSV=false; displayToWindow=true;}
		else if(strcmp(argv[1], "-1")==0) {calibrateHSV=true; displayToWindow=false;}
		else if(strcmp(argv[1], "-2")==0) {calibrateHSV=true; displayToWindow=true;printf("argv[1]==\"-2\"\n");}
		else if(strcmp(argv[1], "-k")==0) {calibrateHSV=false; displayToWindow=false; mode_orients=true; printf("orient mode is set to true. only reading orient commands from nodemcu unit\n");}
		else if(strcmp(argv[1], "-c")==0) {calibrateHSV=false; displayToWindow=false; mode_orients=false; mode_intro=true; printf("running intro sequence\n");}
	}

	printf("\n- section: \"initialisation\"\n");
	//pca9685 board setup
	PiPCA9685::PCA9685 pca{};
	pca.set_pwm_freq(50.0);



	sendToServo(&pca, new_q, current_q, true);

	if(!mode_orients && !mode_intro) {
		printf("special mode not on\n");

		hsv_settingsRead(camObj, HW_HSV, "",5,"hsv_settings.dat",false);

		if(calibrateHSV) { if(displayFunc(0, &pca)==-1) { return 0; } }
		if(displayToWindow) { if(displayFunc(1, &pca)==-1) { return 0; } }
		if(!calibrateHSV && !displayToWindow && !mode_intro) {
			if(displayFunc(3, &pca)==-1) { return 0; }
		}
	}
	else if(mode_orients){
		while(true) {
			usleep(10'000);
			// printf("\tx:%d y:%d z:%d a:%d b:%d\n",int(PP[0]),int(PP[1]),int(PP[2]),int(orient[0]),int(orient[1]));
			orientObj.update(true);
			if(getAngles(new_q,PP,toRadians(orient[0]),toRadians(orient[1]),toRadians(orient[2]),1)) {
				printf("a:%d\tb:%d", int(orient[0]), int(orient[1]));
				printf("\tangles:\t%d\t%d\t%d\t%d\t%d\t%d",
				int(new_q[0]),int(new_q[1]),int(new_q[2]),int(new_q[3]),int(new_q[4]),int(new_q[5]));
				sendToServo(&pca,new_q,current_q,false);
			}
			else {printf("\tPP-orient not reachable");}
			printf("\n");
		}
	}
	else if(mode_intro) {
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

	for(int n=0; n<6; n++) new_q[n] = startup_q[n];
	printf("\n- section: \"closing\"\n");
	sendToServo(&pca, new_q, current_q, false, 2, 2);
	if(pigpioInitia) {
		gpioWrite(pin_ledRelay, 0);
		gpioTerminate();
	}
	return 0;
}
