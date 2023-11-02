
/*
 * Project Name: Hexclaw  
 * File: main.cpp
 * Description: Robot Arm Control project  
 * Author: Erkhme Byambadorj.
 * Created May 2023
 * Description: See URL for full details.
 * NOTE :: NOT FINISHED
 * URL: 
 */

// nodemcu/esp8266 module udp communication
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <csignal>

#define MAXLINE 2048

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

// TFT display libraries
#include <bcm2835.h>
#include "ST7735_TFT.h"
#include "/home/pi/Chromebook-projects/teststuff/electronics/tft_display/ST7735_TFT_RPI_test/ST7735_TFT_RPI-1.5/example/include/Bi_Color_Bitmap.h" // Data for test 11 and 12.

// RPi specific functions
#include <pigpio.h>

#include "IK_header.h"

using namespace std;

string absPath;

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


// udp com related: udp communication variable declarations
int bind_result;
int sock;
char buffer[MAXLINE];
const char* PORT;
char szIP[100];
sockaddr_in addrListen;
sockaddr_storage addrDest;
const char* toESP_msg;

// tft display setup
ST7735_TFT myTFT;

int8_t tft_setup(void) {

	cout << "TFT Start!" << endl;
	if(!bcm2835_init())
	{
		cout << "Error : Problem with init bcm2835 library" << endl;
		return -1;
	}
	
// ** USER OPTION 1 GPIO/SPI TYPE HW OR SW **
	int8_t RST_TFT = 24;
	int8_t DC_TFT = 25;
	int8_t SCLK_TFT = -1; // 5, change to GPIO no for sw spi, -1 hw spi
	int8_t SDIN_TFT = -1; // 6, change to GPIO no for sw spi, -1 hw spi
	int8_t CS_TFT = -1 ;  // 8, change to GPIO no for sw spi, -1 hw spi
	myTFT.TFTSetupGPIO(RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);


// ** USER OPTION 2 Screen Setup **
	uint8_t OFFSET_COL = 0;  // 2, These offsets can be adjusted for any issues->
	uint8_t OFFSET_ROW = 0; // 3, with manufacture tolerance/defects
	uint16_t TFT_WIDTH = 128;// Screen width in pixels
	uint16_t TFT_HEIGHT = 160; // Screen height in pixels
	myTFT.TFTInitScreenSize(OFFSET_COL, OFFSET_ROW , TFT_WIDTH , TFT_HEIGHT);


// ** USER OPTION 3 PCB_TYPE + SPI baud rate + SPI_CE_PIN**
	uint32_t SCLK_FREQ =  8000000 ; // HW Spi freq in Hertz , MAX 125 Mhz MIN 30Khz
	uint8_t SPI_CE_PIN = 0; // which HW SPI chip enable pin to use,  0 or 1
	// pass enum to param1 ,4 choices,see README
	if(!myTFT.TFTInitPCBType(TFT_ST7735R_Red, SCLK_FREQ, SPI_CE_PIN))return -1;
	// Note : if using SW SPI you do not have to pass anything for param 2&3, it will do nothing. 

	myTFT.TFTsetRotation(TFT_Degress_90);
	return 0;
}


void matToTFT(cv::Mat threshImg) {
	cv::Size imgSize = threshImg.size();
	cv::resize(threshImg, threshImg, cv::Size(), 128*(imgSize.width/imgSize.height), 128);
	cv::Mat cropImg = threshImg(cv::Rect(160, 128, 128*(imgSize.width/imgSize.height), 128));
	
	size_t pixelSize = 3;
	uint8_t* bmpBuffer = NULL;
	bmpBuffer = (uint8_t*)malloc((160 * 128) * pixelSize);
	if (bmpBuffer == NULL) {
		std::cout << "Error Test14 : MALLOC could not assign memory " << std::endl;
		return;
	}

	//cv::cvtColor(cropImg, bmpBuffer, cv::COLOR_BGR5652RGB);
	//myTFT.TFTdrawBitmap24(0, 0, bmpBuffer, 160, 128);
}


// IK related: ik calc variable declaration
float current_q[6] = {0,0,0,0,0,0}; //old_rotation
float new_q[6] = {0,0,0,0,0,0};
float orient[3] = {0,0,0}; //degrees
float PP[3] = {0,150,150};
float axisScal[3] = {0.6, 0.6, 0.9};
float axisOffset[3] = {0, 0, -100};
float axisFilter[3] = {1, 1, 1};


float x_accel, y_accel, z_accel, pitch, roll, Pitch=0, Roll=0;

/// @brief 2d coefficients for a single layer
float angleArea_coef[181][181];
/// @brief 3d artifically pre-generated area results
int artifVal[181][181][401]; //x = [0, 90, 181] = [-90, 0, 01]

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


/// @brief prefered image size to convert/use for all images
int prefSize[2] = {640, 480};

/// @brief FOV of webcam in degrees
float camFOV[2] = {round(43*(640/480)), 43};
/// @brief Number of angles for each pixel
float angPerPix = camFOV[1]/prefSize[1];


/// @brief Find index to closest value in arr
/// @param arr array to find closest index for
/// @param pick value to find index of
/// @param printVar whether to print results
/// @return index of result (closest value)
int getClosestValIdx(int arr[401], int pick=2000, bool printVar=false) {
    int minArr[401];

    for(int i=0; i<401; i++) minArr[i] = abs(arr[i]-pick);

    int minVar[2] = {0, minArr[0]};
    if(printVar) cout << "Picked: " << pick << endl;
    if(printVar) printf("\n[%d] %3d|%3d - %3d\n", 0, minVar[1], minArr[0], arr[0]);
    for(int i=1; i<401; i++) {
        if(printVar) printf("[%d] %3d|%3d - %3d\n", i, minVar[1], minArr[i], arr[i]);
        if(minVar[1]>minArr[i]) {
            minVar[0]=i;
            minVar[1]=minArr[i];
        }
    }
    if(printVar) printf("\nClosest value to %d: index:%d element:%d\n", pick, minVar[0], arr[minVar[0]]);

    return minVar[0];
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
	/*
	[-x:x] = [alpha:-alpha]
	[-y:y] = [-beta:beta]
	*/
	// ans = val * (1 / angleArea_coef[
	// 		int(round(/*orient[0]*/-(0-posX*angPerPix)))+90
	// 	][
	// 		int(round(/*orient[1]+*/posY*angPerPix))+90
	// 	]);
	// return ans;

	int chosenIdx=0;
	chosenIdx = getClosestValIdx(artifVal[int(Roll)+90][int(Pitch)+90],int(area));
	cout << chosenIdx << artifVal[int(Roll)+90][int(Pitch)+90][200]<< "\t";
	return chosenIdx;
}


/// @brief index of webcam
int webcamIndex = 2;


bool displayImg = true;
bool calibrateHSV = false;
bool displayTFT = false;

bool mode_orients = false;
bool mode_intro = false;


int l_HSV[3] = {0, 0, 255};
int u_HSV[3] = {179, 9, 255};

/// @brief minimum limit for area to be recognised
int areaLim = 1000;


bool useFilter = false;
float accelFilter = 0.1;

/// @brief variable for contours
vector<vector<cv::Point>> contours;
vector<cv::Vec4i> hierarchy;

float validCnt_pos[20][2]; //array of contours above threshold; form [x, y]
int validCnt_index = 0; //index of biggest validCnt_pos sent (so =1 means there is one elemment)
float totCnt_pos[2];
float totCnt_area = 0;

/// @brief Get avg xy coordinates from list of coordinates
/// @param allCnt array of coordinates
/// @param cntIndex number of elements in array
/// @param totCntPos_ptr "pointer" array to hold "returned" result/xy_coordinate
void getAvg_cntPos(float allCnt[20][2], int cntIndex, float totCntPos_ptr[2]) {
	float xTot=0, yTot = 0;
	for(int i=0; i<cntIndex; i++) {
		xTot += allCnt[i][0];
		yTot += allCnt[i][1];
	}
	totCntPos_ptr[0] = xTot / cntIndex;
	totCntPos_ptr[1] = yTot / cntIndex;
}


bool pigpioInitia = false;
int pin_ledRelay = 23;

int resolvehelper(const char* hostname, int family, const char* service, sockaddr_storage* pAddr) {
    int result;
    addrinfo* result_list = NULL;
    addrinfo hints = {};
    hints.ai_family = family;
    hints.ai_socktype = SOCK_DGRAM; // without this flag, getaddrinfo will return 3x the number of addresses (one for each socket type).
    result = getaddrinfo(hostname, service, &hints, &result_list);
    if (result == 0)
    {
        //ASSERT(result_list->ai_addrlen <= sizeof(sockaddr_in));
        memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
        freeaddrinfo(result_list);
    }

    return result;
}

// initialize udp "settings"
void nodemcu_udp_setup() {
	bind_result = 0;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	fcntl(sock, F_SETFL, O_NONBLOCK);
	PORT = "53";
	addrListen = {};
	addrListen.sin_family = AF_INET;
	bind_result = bind(sock, (sockaddr*)&addrListen, sizeof(addrListen));
	if(bind_result==-1) {
		int lasterror = errno;
		cout << "bind() error:" << lasterror;
		exit(1);
	}
	addrDest = {};
	bind_result = resolvehelper("192.168.1.117", AF_INET, PORT, &addrDest);
	if(bind_result!=0) {
		int lasterror = errno;
		cout << "resolvehelper error:" << lasterror;
		exit(1);
	}
	toESP_msg = "1";
}


// request to nodemcu board, receive {axis}_accel values, solve orient[0,1] variables
void updateOrients(bool printResult) {
	size_t msg_length = strlen(toESP_msg);
	bind_result = sendto(
		sock, toESP_msg, msg_length, 0,
		(sockaddr*)&addrDest, sizeof(addrDest)
		);
	socklen_t len;
	int n = recvfrom(
		sock, (char*)buffer, MAXLINE, MSG_WAITALL,
		(struct sockaddr*)&addrDest, &len);

	buffer[n] = '\0';
	if(printResult) {
		// printf("\tSent %d bytes\t",bind_result);
		printf("\tRead from server: \"%s\"\t",buffer);
	}
	cout /*<< " msg_waitall:" << MSG_WAITALL */<< " errno:" << errno << " ";

	// cout << "[n=" << n << " 0:\"" <<buffer[0] << "\" n-1:\"" << buffer[n-1] << "\" ]";
	string temp = "";
	if(buffer[0]=='{' && buffer[n-1]==';') { //{x:y:z}
		// printf("is in\n");
		// if(printResult) cout << buffer << "\t";
		for(int i=0; i<n-1; i++) temp+=buffer[i];
		x_accel = stof(temp.substr(1, temp.find(':')));
		temp.erase(0, temp.find(':')+1);
		y_accel = stof(temp.substr(0, temp.find(':')));
		temp.erase(0, temp.find(':')+1);
		z_accel = stof(temp.substr(0, temp.find('}')));
		temp.erase(0, temp.find(':')+1);
		string filtBoolStr = temp.substr(0, temp.find(';'));
		if(filtBoolStr == "off") useFilter = false;
		else if(filtBoolStr == "on ") useFilter = true;

		// if(x_accel>1 || y_accel>1 || z_accel>1 || x_accel<-1 || y_accel<-1 || z_accel<-1) return;
		if(x_accel>1) x_accel = 0.99;
		if(y_accel>1) y_accel = 0.99;
		if(z_accel>1) z_accel = 0.99;
		// if(printResult) printf("x_acc:%d\ty_acc:%d\tz_acc:%d\t",int(x_accel),int(y_accel),int(z_accel));

		pitch = atan(y_accel / sqrt(pow(x_accel,2)+pow(z_accel,2))) * 180 / M_PI; //degrees
		roll = atan(-1 * x_accel / sqrt(pow(y_accel,2)+pow(z_accel,2))) * 180 / M_PI; //degrees
		//pitch = -pitch;
		//roll = -roll;

		if(useFilter) {
			Pitch = (1-accelFilter) * Pitch + accelFilter * pitch;
			Roll = (1-accelFilter) * Roll + accelFilter * roll;
		}
		else {
			Pitch = pitch;
			Roll = roll;
		}
		int bPos = -1;
    	if(Pitch <= 90 and Pitch >= -90) {
			if(useFilter) orient[1] = Pitch * accelFilter + orient[1] * (1-accelFilter);
			else orient[1] = Pitch;
			if(orient[1] < 0) bPos = -1;
			if(orient[1] > 0) bPos = 1;
		}
    	if(Roll <= 90 and Roll >= -90) {
			if(useFilter) orient[0] = Roll * accelFilter + orient[0] * (1-accelFilter);
			else orient[0] = Roll;
			orient[0] = orient[0] * bPos;
		}
		if(printResult) printf("a:%3d b:%3d  Roll:%3d Pitch:%3d", 
		int(orient[0]), int(orient[1]), int(Roll), int(Pitch));
	}
}


/// @brief Create trackbar
/// @param win_name pointer to opencv window name
void createTrackbars(const char* win_name) {
	cv::createTrackbar("LowH", win_name, &l_HSV[0], 179);
	cv::createTrackbar("HighH", win_name, &u_HSV[0], 179);
	cv::createTrackbar("LowS", win_name, &l_HSV[1], 255);
	cv::createTrackbar("HighS", win_name, &u_HSV[1], 255);
	cv::createTrackbar("LowV", win_name, &l_HSV[2], 255);
	cv::createTrackbar("HighV", win_name, &u_HSV[2], 255);
}

/// @brief set trackbar pos/value
/// @param win_name pointer to opencv window name
void updateTrackbarPos(const char* win_name) {
	cv::setTrackbarPos("LowH", win_name, l_HSV[0]);
	cv::setTrackbarPos("LowS", win_name, l_HSV[1]);
	cv::setTrackbarPos("LowV", win_name, l_HSV[2]);
	cv::setTrackbarPos("HighH", win_name, u_HSV[0]);
	cv::setTrackbarPos("HighS", win_name, u_HSV[1]);
	cv::setTrackbarPos("HighV", win_name, u_HSV[2]);
}

void hsv_settingsRead(const char* win_name = "", int indeks=1, string filePath="hsv_settings.dat", bool displayWin=true) {
	//read HSV values from file with given indeks and change global l_HSV/u_HSV variables
	ifstream hsvFile(filePath);
	if(!hsvFile.is_open()) {
		printf("can't open file \"%s\"",filePath);
		return;
	}
	string line;
	int tempVal[6];
	while(getline(hsvFile,line)) {
		if(line.substr(0,1)==to_string(indeks)) {
			line.erase(0,3);
			//lower HSV
			tempVal[0]=stoi(line.substr(0,line.find(',')));
			line.erase(0,line.find(',')+1);
			tempVal[1]=stoi(line.substr(0,line.find(',')));
			line.erase(0,line.find(',')+1);
			tempVal[2]=stoi(line.substr(0,line.find(':')));
			line.erase(0,line.find(':')+1);
			//upper HSV
			tempVal[3]=stoi(line.substr(0,line.find(',')));
			line.erase(0,line.find(',')+1);
			tempVal[4]=stoi(line.substr(0,line.find(',')));
			line.erase(0,line.find(',')+1);
			tempVal[5]=stoi(line.substr(0,line.find(']')));

			for(int i=0; i<6; i++) { printf(" %d", tempVal[i]); }
			l_HSV[0] = tempVal[0];
			l_HSV[1] = tempVal[1];
			l_HSV[2] = tempVal[2];
			u_HSV[0] = tempVal[3];
			u_HSV[1] = tempVal[4];
			u_HSV[2] = tempVal[5];
			if(displayWin) updateTrackbarPos(win_name);
			hsvFile.close();
			return;
		}
	}
	printf("index not found\n");
}

void hsv_settingsWrite(int indeks=0, bool overWrite=false, string filePath="hsv_settings.dat") {
	//write global HSV values from l_HSV and u_HSV into the file
	//overwrite:	if indeks already exists the new one will be written over the old one if overWrite is true,
	//				otherwise it'll add the values at the end of the files
	ifstream rFile(filePath);
	if(!rFile.is_open()) {
		printf("error: Cannot open file\"");
		cout << filePath << "\"\n";
	}
	string fileContents="", line;
	// printf("rowLen_1: %d\n", rowLen);
	while(getline(rFile, line)) {
		fileContents+=line;
		fileContents+="\n";
	} //ex: "row1\nrow2\nrow3"
	rFile.close();
	ofstream wFile(filePath);
	int rowLen_2 = count(fileContents.begin(),fileContents.end(),'\n');
	// printf("total rows:%d\n",rowLen_2);
	if(!overWrite) {rowLen_2++;}
	else {}
	string fileRows[rowLen_2];
	for(int i=0; i<rowLen_2; i++) {
		// printf("row %d out of %d rows: ",i,rowLen_2);
		// cout << fileContents.substr(0,fileContents.find('\n')+1) << endl;
		if(i==rowLen_2-1 && !overWrite) {
			fileRows[i] = to_string(i-2)+";["+
			to_string(l_HSV[0])+","+to_string(l_HSV[1])+","+to_string(l_HSV[2])+":"+
			to_string(u_HSV[0])+","+to_string(u_HSV[1])+","+to_string(u_HSV[2])+"]\n";
		}
		else {
			fileRows[i] = fileContents.substr(0,fileContents.find('\n')+1);
			fileContents.erase(0,fileContents.find('\n')+1);
			if(fileRows[i].substr(0,1)==to_string(indeks-1) && overWrite) {
				i++;
				fileRows[i] = to_string(i)+";["+
				to_string(l_HSV[0])+","+to_string(l_HSV[1])+","+to_string(l_HSV[2])+":"+
				to_string(u_HSV[0])+","+to_string(u_HSV[1])+","+to_string(u_HSV[2])+"]\n";
				fileContents.erase(0,fileContents.find('\n')+1);
			}
		}
	}
	string totText = "";
	for(int i=0; i<rowLen_2; i++) {
		// printf("fileRows[%d]: \"", i);
		// cout << fileRows[i] << "\"" << endl;
		totText+=fileRows[i];
		}
	wFile << totText;
	printf("File written\n");
	wFile.close();
}

/// @brief main function to read, display and control the robot
/// @param cap [cv::VideoCapture object pointer]: for main camera
/// @param mode [integer]: -0: setup/calibrate hsv; -1: w. sendToServo; -2: without sendToServo; -3: w. sendToServo without cv disp.
/// @param pcaSrc [PiPCA9682 object pointer]: for pca9685 board
/// @return [integer]: 0 - normal function finish; -1 - exit program entirely
int displayFunc(cv::VideoCapture* cap, int mode, PiPCA9685::PCA9685* pcaSrc) {
	/*
	mode:
	- 0: setup/calibrate hsv
	- 1: main color tracking loop with sendToServo
	- 2: main color tracking loop without sendToServo
	- 3: main color tracking loop with sendToServo without display to a window
	*/

	cout << "- In \"displayFunc()\"" << endl;

	const char* win_name = "Window";
	if(mode!=3) {
		cv::namedWindow(win_name);
		createTrackbars(win_name);
	}
	cv::Mat imgRaw, imgOriginal, imgFlipped, imgHSV, imgThreshold;
	// int fps=0, frames=0, totalDelay=0;
	// clock_t t1 = clock();

	while(true) {
		//delay: 4-5ms
		if(!(cap->read(imgRaw))) {
			printf("error: Cannot read frame");
			cv::destroyAllWindows();
			return -1;
		}

		//delay: unknown
		cv::resize(imgRaw, imgOriginal, cv::Size(prefSize[0], prefSize[1]), cv::INTER_LINEAR);
		// cout << imgOriginal.cols << "x" << imgOriginal.rows;
		
		//delay: 4-7ms
		cv::flip(imgOriginal, imgFlipped, 1);
		//delay: 3-5ms
		cv::cvtColor(imgFlipped, imgHSV, cv::COLOR_BGR2HSV);

		//delay: 1-4ms
		cv::inRange(imgHSV,
		cv::Scalar(l_HSV[0], l_HSV[1], l_HSV[2]),
		cv::Scalar(u_HSV[0], u_HSV[1], u_HSV[2]),
		imgThreshold);
		
		//delay: 2-3ms
		cv::erode(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 1);
		cv::dilate(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 6); 

		// cv::dilate(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 1); 
		// cv::erode(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 1);

		//delay: 1-2ms
		if(mode >= 1) {
			cv::Moments imgMoments = cv::moments(imgThreshold);
			double dM01 = imgMoments.m01;
			double dM10 = imgMoments.m10;
			double dArea = imgMoments.m00;
			cv::findContours(imgThreshold, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
			dArea = 0;
			validCnt_index = 0;
			totCnt_area = 0;
			for(unsigned int i=0; i<contours.size(); i++) {
				dArea = cv::contourArea(contours[i]);
				// cout << contours[0][0].x << endl;
				if(dArea >= areaLim) {
					cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(contours[i]));
					int posX = contours[i][0].x /*	+ minRect.size.width/4*/, posY = contours[i][0].y + minRect.size.height/2;
					validCnt_pos[i][0] = posX;
					validCnt_pos[i][1] = posY;
					validCnt_index += 1;
					totCnt_area += dArea;
				}
			}
			if(validCnt_index > 0) {
				getAvg_cntPos(validCnt_pos, validCnt_index, totCnt_pos);
				if(mode != 3) {
					cv::circle(imgFlipped,cv::Point(totCnt_pos[0],totCnt_pos[1]),50,cv::Scalar(0,0,0),2);
					cv::putText(imgFlipped,to_string(int(totCnt_area)),cv::Point(totCnt_pos[0],totCnt_pos[1]),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,0,0),2,false);
				}
				if(mode != 2) {
					cv::Size camSize = imgFlipped.size();
                    float camPos[2] = {totCnt_pos[0]-camSize.width/2, camSize.height-totCnt_pos[1]};
					PP[0] = axisFilter[0] * float(round(float(totCnt_pos[0] - camSize.width/2)*axisScal[0]) + axisOffset[0]) + (1-axisFilter[0])*PP[0];
					PP[1] = axisFilter[1] * float(round(float(camSize.height - totCnt_pos[1])*axisScal[1]) + axisOffset[1]) + (1-axisFilter[1])*PP[1];
					PP[2] = axisFilter[2] * float(axisScal[2]*zAxisFunc(totCnt_area, camPos[0], camPos[1]) + axisOffset[2]) + (1-axisFilter[2])*PP[2];
					printf("num. cnt:%2d total dArea:%6d", validCnt_index, int(totCnt_area));
					printf(" x:%3d y:%3d z:%3d",int(PP[0]),int(PP[1]), int(PP[2]));
					updateOrients(false);
					if(getAngles(new_q,PP,toRadians(orient[0]),toRadians(orient[1]),toRadians(orient[2]),1)) {
						sendToServo(pcaSrc,new_q,current_q,false);
					}
					else if(findValidOrient(PP, orient, orient, new_q)) {
						sendToServo(pcaSrc,new_q,current_q,false);
					}
					else printf("valid orient for (%d, %d, %d) not found\n", int(PP[0]), int(PP[1]), int(PP[2]));
				}
			}
		}
		//delay: 9-13ms
		if(mode!=3) {
			cv::cvtColor(imgThreshold,imgThreshold,cv::COLOR_GRAY2BGR);
			cv::hconcat(imgFlipped,imgThreshold,imgFlipped); //merge imgThreshold and imgFlipped horizontally
		}
		if(displayTFT) {
			matToTFT(imgThreshold);
		
		}
		
		//delay: 0ms
		// float temp = 1000*(clock()-t1)/CLOCKS_PER_SEC;
		// if(temp>=1000) {
		// 	totalDelay=temp/frames;
		// 	fps=int(frames/(temp/1000));
		// 	printf(" frames:%d ",frames);
		// 	frames=0;
		// 	t1=clock();
		// }
		// else frames++;

		// printf("fps:%d totalDelay:%dms\n",fps,totalDelay);

		//delay: 6-11ms
		if(mode!=3) {
			// cv::putText(
			// 	imgFlipped,"fps:"+to_string(fps)+" totalDelay:"+to_string(totalDelay)
			// 	,cv::Point(50,50),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,0,0),2,false
			// 	);
			int keyInp = cv::waitKey(10);
			cv::imshow(win_name,imgFlipped);
			// printf(" %d ", keyInp);
			if(keyInp==27) return -1; //'esc'
			else if(keyInp==32) break; //'space'
			else if(keyInp==115) { /*'s'*/ if(mode==0) { /*save HSV values*/ hsv_settingsWrite(0); } }
			else if(keyInp==114) { //'r'
				string inputVar = "";
				int indVar = 0;
				cout << "input: ";
				cin >> inputVar;
				if(inputVar=="exit") exit(1);
				indVar = stoi(inputVar);
				cin.clear();
				cin.ignore();
				hsv_settingsRead(win_name, indVar);
			}
			else if(keyInp==116 && mode==0) { /*'t'*/ hsv_settingsRead(win_name, 0); }
		}
		printf("\n");
	}
	if(mode!=3) cv::destroyWindow(win_name);
	return 0;
}


void splitString(string line, string delimiter, float returnArr[4], int numVar=4, bool printVar=false) {
    if(printVar) cout << "--- \"" << line << "\"\n";
    size_t pos = 0;
    for(int i=0; i<numVar; i++) {
        if(i<(numVar-1)) pos = line.find(delimiter);
        if(printVar) cout << "- pos:" << pos << " :" << line.substr(0, pos) << "\n";
        returnArr[i] = stof(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
    }
    if(printVar) cout << "---";
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
			artifVal[int(tempArr[0])+90][int(tempArr[1])+90][int(tempArr[2])] = int(round(tempArr[3]));
			rowCount++;
			
		}
		csvFile.close();
	}
	if(printVar) cout << "\nFinished loading the data: Total rows:" << rowCount << endl;
	cout << artifVal[90][90][200] << endl;
	cout << "paused:";
	string inp;
	cin >> inp;
	cin.ignore();
	cin.clear();
}


int main(int argc, char** argv) {
	/*
	argv codes:
	- argc <= 1:		calibrateHSV = false;	displayImg = false; #default
	- argv[1] == "-0":	calibrateHSV = false;	displayImg = true;
	- argv[1] == "-1":	calibrateHSV = true;	displayImg = false;
	- argv[1] == "-2":	calibrateHSV = true;	displayImg = true;
	- argv[1] == "-3":	special mode: no display and no tracking or reading of position.
						where given pos is stationary and only reads orient var from esp
	*/

	// signal(SIGINT	,signal_handler);

	//nodemcu udp communication setup/initialization
	nodemcu_udp_setup();
	load_csvFile();
	initPaths();
	
	loadData_csvArtif(false);

	if(gpioInitialise()<0) {
		cout << "pigpio \"gpioInitialise()\" failed\n";
		pigpioInitia = false;
	}
	else {
		gpioSetMode(pin_ledRelay, PI_OUTPUT);
		pigpioInitia = true;
		gpioWrite(pin_ledRelay, 1);
	}

	if(argc<=1) {calibrateHSV=false; displayImg=false;}
	else if(argc>=2) {
		if(argv[1]=="-0") {calibrateHSV=false; displayImg=true;}
		else if(argv[1]=="-1") {calibrateHSV=true; displayImg=false;}
		else if(argv[1]=="-2") {calibrateHSV=true; displayImg=true;printf("argv[1]==\"-2\"\n");}
		else if(argv[1]=="-k") {calibrateHSV=false; displayImg=false; mode_orients=true; printf("orient mode is set to true. only reading orient commands from nodemcu unit\n");}
		else if(argv[1]=="-c") {calibrateHSV=false; displayImg=false; mode_orients=false; mode_intro=true; printf("running intro sequence\n");}
	}

	//pca9685 board setup
	PiPCA9685::PCA9685 pca{};
	pca.set_pwm_freq(50.0);
	//initialization command send to pca-board
	printf("\n- section: \"initialisation\"\n");
	sendToServo(&pca, new_q, current_q, true);

	if(!mode_orients && !mode_intro) {
		printf("special mode not on\n");
		cv::VideoCapture cap(webcamIndex);
		if(!cap.isOpened()) {
			cout << "error: Cannot open web cam." << endl;
			return -1;
		}
		cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1); //change to 0 if output is not same as createProfile.py
		hsv_settingsRead("",5,"hsv_settings.dat",false);

		if(calibrateHSV) { if(displayFunc(&cap, 0, &pca)==-1) { return 0; } }
		if(displayImg) { if(displayFunc(&cap, 1, &pca)==-1) { return 0; } }
		if(!calibrateHSV && !displayImg && !mode_intro) {
			if(displayFunc(&cap, 3, &pca)==-1) { return 0; }
		}
	}
	else if(mode_orients){
		while(true) {
			usleep(10'000);
			// printf("\tx:%d y:%d z:%d a:%d b:%d\n",int(PP[0]),int(PP[1]),int(PP[2]),int(orient[0]),int(orient[1]));
			updateOrients(false);
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
