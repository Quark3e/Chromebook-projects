
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
using namespace cv;

int webcamIndex = 0;

bool displayImg = true;

int lower_HSV[3] = {64, 73, 88};
int upper_HSV[3] = {103, 213, 255};

const char* controlWin = "Control";

void createTrackbars() {
	cvCreateTrackbar("LowH", controlWin, &lower_HSV[0], 179);
	cvCreateTrackbar("HighH", controlWin, &upper_HSV[0], 179);
	cvCreateTrackbar("LowS", controlWin, &lower_HSV[1], 255);
	cvCreateTrackbar("HighS", controlWin, &upper_HSV[1], 255);
	cvCreateTrackbar("LowV", controlWin, &lower_HSV[2], 255);
	cvCreateTrackbar("HighV", controlWin, &upper_HSV[2], 255);
}

int main(int argc, char** argv) {
	if(argc>=2 && argv[1]=="false") displayImg = false;

	PiPCA9685::PCA9685 pca{};
	pca.set_pwm_freq(50.0);

	VideoCapture cap(webcamIndex);
	if(!cap.isOpened()) {
		cout << "Cannot open web cam" << endl;
		return -1;
	}
	namedWindow(controlWin, CV_WINDOW_AUTOSIZE);
	createTrackbars();



	Mat imgOriginal;

	float current_q[6] = {0,0,0,0,0,0}; //old_rotation
	float new_q[6] = {0,0,0,0,0,0};
	float PP[3] = {0, 150, 150};

	sendToServo(&pca, new_q, current_q, true, 0);

	sleep(2);
	if(getAngles(new_q,PP,0,0,0,1)) {	
		printf("angles: ");
		for(int i=0; i<6; i++) { printf(" %f",new_q[i]); }
		printf("\n");
		sendToServo(&pca, new_q, current_q, false, 2, 2, true, true, false);
	}
	else printf("position not reachable\n");
	cout << endl;

	return 0;
}
