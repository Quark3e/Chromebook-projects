
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

//nodemcu/esp8266 module udp communication
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

//default headers/includes
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream> //read and write to file (hsv_settings.dat)
#include <algorithm> //ex. find() and erase()
#include <unistd.h>

//opencv/image tracking
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>

//pca9685 communication
#include <PiPCA9685/PCA9685.h>

#include "IK_header.h"

using namespace std;
// using namespace cv;

//udp com related: udp communication variable declarations
int bind_result;
int sock;
char buffer[MAXLINE];
const char* PORT;
char szIP[100];
sockaddr_in addrListen;
sockaddr_storage addrDest;
const char* toESP_msg;

//IK related: ik calc variable declaration
float current_q[6] = {0,0,0,0,0,0}; //old_rotation
float new_q[6] = {0,0,0,0,0,0};
float orient[3] = {0,0,0}; //degrees
float PP[3] = {0,150,150};
float axisScal[3] = {0.7, 0.7, 1};

float cam_PP_offset[3] = {0,0,0};

int webcamIndex = 0;

bool displayImg = true;
bool calibrateHSV = false;

bool mode_orients = false;
bool mode_intro = false;

int l_HSV[3] = {0, 0, 255};
int u_HSV[3] = {179, 9, 255};

int areaLim = 10'000;

float x_accel, y_accel, z_accel, pitch, roll, Pitch=0, Roll=0;

bool useFilter = false;
float accelFilter = 0.1;

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
		printf("\tSent %d bytes\t",bind_result);
		printf("Read from server: \"%s\"\t",buffer);
	}
	string temp = "";
	
	if(buffer[0]=='{' && buffer[n-1]==';') { //{x:y:z}
		if(printResult) cout << buffer << "\t";
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
		if(printResult) printf("x_acc:%f\ty_acc:%f\tz_acc:%f\t",x_accel,y_accel,z_accel);

		pitch = atan(y_accel / sqrt(pow(x_accel,2)+pow(z_accel,2))) * 180 / M_PI; //degrees
		roll = atan(-1 * x_accel / sqrt(pow(y_accel,2)+pow(z_accel,2))) * 180 / M_PI; //degrees
		pitch = 0-pitch;
		roll = 0-roll;

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
		if(printResult) printf("a:%d \tb:%d  \tRoll:%d  \tPitch:%d", 
		int(orient[0]), int(orient[1]), int(Roll), int(Pitch));
	}
}


void createTrackbars(const char* win_name) {
	cv::createTrackbar("LowH", win_name, &l_HSV[0], 179);
	cv::createTrackbar("HighH", win_name, &u_HSV[0], 179);
	cv::createTrackbar("LowS", win_name, &l_HSV[1], 255);
	cv::createTrackbar("HighS", win_name, &u_HSV[1], 255);
	cv::createTrackbar("LowV", win_name, &l_HSV[2], 255);
	cv::createTrackbar("HighV", win_name, &u_HSV[2], 255);
}

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
	cv::Mat imgOriginal, imgFlipped, imgHSV, imgThreshold;
	
	// int fps=0, frames=0, totalDelay=0;
	// clock_t t1 = clock();

	while(true) {
		//delay: 4-5ms
		if(!(cap->read(imgOriginal))) {
			printf("error: Cannot read frame");
			cv::destroyAllWindows();
			return -1;
		}

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
		cv::erode(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
		cv::dilate(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) ); 

		cv::dilate(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) ); 
		cv::erode(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );

		//delay: 1-2ms
		if(mode>=1) {
			cv::Moments imgMoments = cv::moments(imgThreshold);
			double dM01 = imgMoments.m01;
			double dM10 = imgMoments.m10;
			double dArea = imgMoments.m00;

			if(dArea>=areaLim) {
				int posX = dM10/dArea, posY = dM01/dArea;
				if(mode!=3) {
					cv::circle(imgFlipped,cv::Point(posX,posY),50,cv::Scalar(0,0,0),2);
					cv::putText(imgFlipped,to_string(int(dArea)),cv::Point(posX,posY),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,0,0),2,false);
				}
				if(mode!=2) {
					cv::Size camSize = imgFlipped.size();
					PP[0] = int(ceil(float(posX - camSize.width/2)*axisScal[0]));
					PP[1] = int(ceil(float(camSize.height - posY)*axisScal[1]));
					printf(" x:%d y:%d ",int(PP[0]),int(PP[1]));
					updateOrients(true);
					if(getAngles(new_q,PP,toRadians(orient[0]),toRadians(orient[1]),toRadians(orient[2]),1)) {
						sendToServo(pcaSrc,new_q,current_q,false);
					}
				}
			}
		}
		//delay: 9-13ms
		if(mode!=3) {
			cv::cvtColor(imgThreshold,imgThreshold,cv::COLOR_GRAY2BGR);
			cv::hconcat(imgFlipped,imgThreshold,imgFlipped);
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
			else if(keyInp==115) { //'s'
				if(mode==0) {
					//save HSV values
					hsv_settingsWrite(0);
				}
			}
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
			else if(keyInp==116 && mode==0) { //'t'
				hsv_settingsRead(win_name, 0);
			} 
		}
		printf("\n");
	}
	if(mode!=3) cv::destroyWindow(win_name);
	return 0;
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
	sendToServo(&pca, new_q, current_q, true, 0);


	if(!mode_orients) {
		printf("special mode not on\n");
		cv::VideoCapture cap(webcamIndex);
		if(!cap.isOpened()) {
			cout << "error: Cannot open web cam." << endl;
			return -1;
		}
		hsv_settingsRead("",5,"hsv_settings.dat",false);

		if(calibrateHSV) { if(displayFunc(&cap, 0, &pca)==-1) { return 0; } }
		if(displayImg) { if(displayFunc(&cap, 1, &pca)==-1) { return 0; } }
		if(!calibrateHSV && !displayImg) {
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
		new_q[0] = 0;
		new_q[1] = 135;
		new_q[2] = 115;
		new_q[3] = 0;
		new_q[4] = -20;
		new_q[5] = 0;
		printf("running intro...\t");
		sendToServo(&pca, new_q, current_q, false, 2, 10);
		printf("intro finished\n");
		usleep(3'000'000);
		new_q[0] = 45;
		new_q[1] = 0;
		new_q[2] = -45;
		new_q[3] = 90;
		new_q[4] = 90;
		new_q[5] = 0;
		sendToServo(&pca,new_q,current_q,false);
	}


	sendToServo(&pca, new_q, current_q, true, 2, 2, true, true, true);

	return 0;
}
