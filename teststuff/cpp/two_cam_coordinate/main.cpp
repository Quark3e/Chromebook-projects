
#include <iostream>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

// opencv/image tracking
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>

#include "two_cam_coordinate.hpp"

using namespace std;


int prefSize[2] = {640, 480};

int l_HSV[3] = {0, 0, 255};
int u_HSV[3] = {179, 9, 255};

vector<vector<cv::Point>> contours0;
vector<vector<cv::Point>> contours1;
vector<cv::Vec4i> hierarchy0;
vector<cv::Vec4i> hierarchy1;


int areaLim = 1000;
float validCnt_pos[20][2];
int validCnt_index = 0;
float totCnt_pos[2][2];
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



cv::Mat imgRaw[2], imgOriginal[2], imgFlipped[2], imgHSV[2], imgThreshold[2];

int processFrame(cv::VideoCapture* cap, int idx) {
    bool test = (cap->read(imgRaw[idx]));
    if(!test) {
        printf("error: Cannot read frame from webcam[%d]",idx);
        cv::destroyAllWindows();
        return -1;
    }

    cv::resize(imgRaw[idx], imgOriginal[idx], cv::Size(prefSize[0],prefSize[1]), cv::INTER_LINEAR);
    // cv::flip(imgOriginal[idx], imgFlipped[idx], 1); //temporarily disabled
    imgFlipped[idx] = imgOriginal[idx];
    cv::cvtColor(imgFlipped[idx], imgHSV[idx], cv::COLOR_BGR2HSV);
    cv::inRange(
        imgHSV[idx],
        cv::Scalar(l_HSV[0], l_HSV[1], l_HSV[2]),
        cv::Scalar(u_HSV[0], u_HSV[1], u_HSV[2]),
        imgThreshold[idx]
    );

    cv::erode(imgThreshold[idx], imgThreshold[idx], cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 1);
    cv::dilate(imgThreshold[idx], imgThreshold[idx], cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 6); 

    cv::Moments imgMoments = cv::moments(imgThreshold[idx]);
    double dM01 = imgMoments.m01;
    double dM10 = imgMoments.m10;
    double dArea = imgMoments.m00;
    if(idx==0) cv::findContours(imgThreshold[idx], contours0, hierarchy0, cv::RETR_TREE,cv::CHAIN_APPROX_NONE);
    dArea = 0;
    validCnt_index = 0;
    totCnt_area = 0;
    if(idx==0) {
        for(unsigned int i=0; i<contours0.size(); i++) {
            dArea = cv::contourArea(contours0[i]);
            if(dArea >= areaLim) {
                cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(contours0[i]));
                int posX = contours0[i][0].x, posY = contours0[i][0].y+minRect.size.height/2;
                validCnt_pos[i][0]=posX;
                validCnt_pos[i][1]=posY;
                validCnt_index += 1;
                totCnt_area += dArea;
            }
        }
    }
    else if(idx==1) {
        for(unsigned int i=0; i<contours1.size(); i++) {
            dArea = cv::contourArea(contours1[i]);
            if(dArea >= areaLim) {
                cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(contours1[i]));
                int posX = contours1[i][0].x, posY = contours1[i][0].y+minRect.size.height/2;
                validCnt_pos[i][0]=posX;
                validCnt_pos[i][1]=posY;
                validCnt_index += 1;
                totCnt_area += dArea;
            }
        }
    }
    if(validCnt_index > 0) {
        getAvg_cntPos(validCnt_pos, validCnt_index, totCnt_pos[idx]);
        cv::circle(imgFlipped[idx],cv::Point(totCnt_pos[idx][0],totCnt_pos[idx][1]),50,cv::Scalar(0,0,0),2);
        cv::putText(imgFlipped[idx],to_string(int(totCnt_area)),cv::Point(totCnt_pos[idx][0],totCnt_pos[idx][1]),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,0,0),2,false);
    }

    cv::cvtColor(imgThreshold[idx], imgThreshold[idx], cv::COLOR_GRAY2BGR);
    cv::vconcat(imgFlipped[idx], imgThreshold[idx], imgFlipped[idx]);

    return 0;
}

int main(int argc, char* argv[]) {

    cv::VideoCapture cap0(0);
    cv::VideoCapture cap1(2);
    if(!cap0.isOpened() || !cap1.isOpened()) {
        cout << "error: Cannot open one- or both of the webcams." << endl;
        return -1;
    }
    cap0.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
    cap1.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);

    const char* win_name = "Window";
    cv::namedWindow(win_name);
    createTrackbars(win_name);


    float camPosition[2][2] = {{0, 0}, {25, 0}};
    float camAng_offs[2] = {90, 123};
    float inpPos[2];
    camTriangle camObj(camPosition, camAng_offs);

    float solvedPos[2];

    while(true) {
        if(processFrame(&cap0, 0)==-1) return 0; 
        if(processFrame(&cap1, 1)==-1) return 0;

        inpPos[0] = totCnt_pos[0][0];
        inpPos[1] = totCnt_pos[1][0];
        camObj.solvePos(inpPos, solvedPos, true);

        cv::Mat winImg;
        cv::hconcat(imgFlipped[0], imgFlipped[1], winImg);

        int keyInp = cv::waitKey(10);
        cv::imshow(win_name, winImg);

        if(keyInp==27) return 0;
        else if(keyInp==32) break;
    }

    return 0;
}