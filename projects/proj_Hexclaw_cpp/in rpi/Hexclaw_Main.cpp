
#include <unistd.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <PiPCA9685/PCA9685.h>
#include <string>

#include "IK_header.h"

using namespace std;
// using namespace cv;


float current_q[6] = {0,0,0,0,0,0}; //old_rotation
float new_q[6] = {0,0,0,0,0,0};
float PP[3] = {0, 150, 150};

int webcamIndex = 0;

bool displayImg = false;
bool calibrateHSV = false;

int l_HSV[3] = {64, 73, 88};
int u_HSV[3] = {103, 213, 255};

int areaLim = 1000;


void createTrackbars(const char* win_name) {
	cv::createTrackbar("LowH", win_name, &l_HSV[0], 179);
	cv::createTrackbar("HighH", win_name, &u_HSV[0], 179);
	cv::createTrackbar("LowS", win_name, &l_HSV[1], 255);
	cv::createTrackbar("HighS", win_name, &u_HSV[1], 255);
	cv::createTrackbar("LowV", win_name, &l_HSV[2], 255);
	cv::createTrackbar("HighV", win_name, &u_HSV[2], 255);
}

int displayFunc(cv::VideoCapture* cap, int mode, PiPCA9685::PCA9685* pcaSrc) {
	/*
	mode:
	- 0: setup/calibrate hsv
	- 1: main color tracking loop with sendToServo
	- 2: main color tracking loop without sendToServo
	*/
	const char* win_name = "Window";
	cv::namedWindow(win_name);
	createTrackbars(win_name);
	cv::Mat imgOriginal, imgFlipped, imgHSV, imgThreshold;

	while(true) {
		if(!(cap->read(imgOriginal))) {
			printf("error: Cannot read frame");
			cv::destroyAllWindows();
			return -1;
		}
		cv::flip(imgOriginal, imgFlipped, 1);
		cv::cvtColor(imgFlipped, imgHSV, cv::COLOR_BGR2HSV);

		cv::inRange(imgHSV,
		cv::Scalar(l_HSV[0], l_HSV[1], l_HSV[2]),
		cv::Scalar(u_HSV[0], u_HSV[1], u_HSV[2]),
		imgThreshold);
		
		cv::erode(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
		cv::dilate(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) ); 

		cv::dilate(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) ); 
		cv::erode(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );

		if(mode==1 || mode==2) {
			cv::Moments imgMoments = cv::moments(imgThreshold);
			double dM01 = imgMoments.m01;
			double dM10 = imgMoments.m10;
			double dArea = imgMoments.m00;

			if(dArea>=areaLim) {
				int posX = dM10/dArea, posY = dM01/dArea;
				cv::circle(imgFlipped,cv::Point(posX,posY),50,cv::Scalar(0,0,0),2);
				if(mode==2) {
					if(getAngles(new_q,PP,0,0,0,1)) {
						sendToServo(pcaSrc,new_q,current_q,false);
					}
				}
			}
		}
		cv::hconcat(imgFlipped,imgThreshold,imgFlipped);
		cv::imshow(win_name,imgFlipped);
		if(cv::waitKey(10)==27) break;
	}
	cv::destroyWindow(win_name);
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

	sendToServo(&pca, new_q, current_q, true, 2, 2, true, true, true);

	return 0;
}
