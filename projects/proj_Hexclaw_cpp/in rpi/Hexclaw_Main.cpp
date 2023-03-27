
#include <unistd.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <PiPCA9685/PCA9685.h>
#include <string>
#include <fstream>
#include <algorithm>

#include "IK_header.h"

using namespace std;
// using namespace cv;


float current_q[6] = {0,0,0,0,0,0}; //old_rotation
float new_q[6] = {0,0,0,0,0,0};
float orient[3] = {0,-90,0}; //degrees
float PP[3] = {0,150,150};

float cam_PP_offset[3] = {0,0,0};

int webcamIndex = 0;

bool displayImg = true;
bool calibrateHSV = true;

int l_HSV[3] = {64, 73, 88};
int u_HSV[3] = {103, 213, 255};

int areaLim = 10'000;


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

void hsv_settingsRead(const char* win_name, int indeks=1, string filePath="hsv_settings.dat") {
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
			updateTrackbarPos(win_name);
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
	string fileContents, line;
	int rowLen_2 = count(
		istreambuf_iterator<char>(rFile),
		istreambuf_iterator<char>(), '\n'
	) +1;
	// printf("rowLen_1: %d\n", rowLen);
	while(getline(rFile, line)) {
		fileContents+=line;
		fileContents+="\n";
	} //ex: "row1\nrow2\nrow3"
	rFile.close();
	ofstream wFile(filePath);
	// int rowLen_2 = count(fileContents.begin(),fileContents.end(),'\n');
	printf("total rows:%d\t",rowLen_2);
	if(!overWrite) {rowLen_2++;}
	else {}
	string fileRows[rowLen_2];
	for(int i=0; i<rowLen_2; i++) {
		printf("row %d out of %d rows\n",i,rowLen_2);
		if(i==rowLen_2-1 && !overWrite) {
			fileRows[i] = to_string(i)+";["+
			to_string(l_HSV[0])+","+to_string(l_HSV[1])+","+to_string(l_HSV[2])+":"+
			to_string(u_HSV[0])+","+to_string(u_HSV[1])+","+to_string(l_HSV[2])+"]\n";
		}
		else {
			fileRows[i] = fileContents.substr(0,fileContents.find('\n')+1);
			fileContents.erase(0,fileContents.find('\n')+1);
			if(fileRows[i].substr(0,1)==to_string(indeks-1) && overWrite) {
				i++;
				fileRows[i] = to_string(i)+";["+
				to_string(l_HSV[0])+","+to_string(l_HSV[1])+","+to_string(l_HSV[2])+":"+
				to_string(u_HSV[0])+","+to_string(u_HSV[1])+","+to_string(l_HSV[2])+"]\n";
				fileContents.erase(0,fileContents.find('\n')+1);
			}
		}
		wFile << fileRows[i];
	}


}

int displayFunc(cv::VideoCapture* cap, int mode, PiPCA9685::PCA9685* pcaSrc) {
	/*
	mode:
	- 0: setup/calibrate hsv
	- 1: main color tracking loop with sendToServo
	- 2: main color tracking loop without sendToServo
	- 3: main color tracking loop without display to a window
	*/

	cout << "- In \"displayFunc()\"" << endl;

	const char* win_name = "Window";
	if(mode!=3) {
		cv::namedWindow(win_name);
		createTrackbars(win_name);
	}
	cv::Mat imgOriginal, imgFlipped, imgHSV, imgThreshold;
	
	int fps=0, frames=0, totalDelay=0;
	clock_t t1 = clock();

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
					PP[0] = posX - camSize.width/2;
					PP[1] = camSize.height - posY;
					printf(" x:%d y:%d ",int(PP[0]),int(PP[1]));
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
		float temp = 1000*(clock()-t1)/CLOCKS_PER_SEC;
		if(temp>=1000) {
			totalDelay=temp/frames;
			fps=int(frames/(temp/1000));
			printf(" frames:%d ",frames);
			frames=0;
			t1=clock();
		}
		else frames++;

		printf("fps:%d totalDelay:%dms\n",fps,totalDelay);

		//delay: 6-11ms
		if(mode!=3) {
			cv::putText(
				imgFlipped,"fps:"+to_string(fps)+" totalDelay:"+to_string(totalDelay)
				,cv::Point(50,50),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,0,0),2,false
				);
			cv::imshow(win_name,imgFlipped);
			int keyInp = cv::waitKey(10);
			// printf(" %d ", keyInp);
			if(keyInp==27) return -1; //esc
			else if(keyInp==32) break; //space
			else if(keyInp==115) { //s
				if(mode==0) {
					//save HSV values
					hsv_settingsWrite(0);
				}
			}
			else if(keyInp==114) { //r
				hsv_settingsRead(win_name, 2);
			}
			else if(keyInp==116 && mode==0) {
				hsv_settingsRead(win_name, 0);
			} 
		}

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
	*/
	if(argc<=1) {calibrateHSV=false; displayImg=false;}
	else if(argc>=2) {
		if(argv[1]=="-0") {calibrateHSV=false; displayImg=true;}
		else if(argv[1]=="-1") {calibrateHSV=true; displayImg=false;}
		else if(argv[1]=="-2") {calibrateHSV=true; displayImg=true;}
	}

	PiPCA9685::PCA9685 pca{};
	pca.set_pwm_freq(50.0);
	sendToServo(&pca, new_q, current_q, true, 0);

	cv::VideoCapture cap(webcamIndex);
	if(!cap.isOpened()) {
		cout << "error: Cannot open web cam." << endl;
		return -1;
	}

	if(calibrateHSV) { if(displayFunc(&cap, 0, &pca)==-1) { return 0; } }
	if(displayImg) { if(displayFunc(&cap, 1, &pca)==-1) { return 0; } }
	if(!calibrateHSV && !displayImg) {
		if(displayFunc(&cap, 3, &pca)==-1) { return 0; }
	}

	sendToServo(&pca, new_q, current_q, true, 2, 2, true, true, true);

	return 0;
}
