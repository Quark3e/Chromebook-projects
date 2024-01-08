
#pragma once
#ifndef H_IR_CAMTRACK
#define H_IR_CAMTRACK


#include <iostream>
#include <vector>
#include <string>
// opencv/image tracking
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>


using namespace std;

/// @brief class for tracking and reading screen pixel coordinate from tracked object
/// @param camIndex index to camera for cv::VideoCapture to create a object with
/// @param prefWidth prefered width for which to scale read image to. Useful for 
/// reducing processing/tracking time
/// @param prefHeight prefered width for which to scale read image to. Useful for
/// reducing processing/tracking time
/// @param setAutoBright whether to use webcams inbuilt automatic brightness adjusting.
///  If false it'll set cv::CAP_PROP_AUTO_EXPOSURE to 0
/// @param 
class IR_camTracking {

    public:
    int prefSize[2];

    int areaLim = 1000;
    vector<vector<float>> validCnt_pos;
    vector<vector<float>> totCnt_pos;
    int validCnt_index = 0;
    float totCnt_area = 0;

    cv::VideoCapture cap;
    vector<cv::Vec4i> hierarchy;
    vector<vector<cv::Point>> contours;

    cv::mat imgRaw, imgOriginal, imgFlipped, imgHSV, imgThreshold;

    bool useAutoBrightness = true;
    bool displayToWindow = false;
    bool takePerformance = true;

    int l_HSV[3] = {0, 0, 255};
    int u_HSV[3] = {179, 9, 255};

    const char* win_name = "Window";

    IR_camTracking(
        int camIndex,
        int prefWidth = 640,
        int prefHeight = 480,
        bool setAutoBright = true;
        bool useWindow = false;
        bool readDelays = true;
        ): cap(camIndex) {
            prefSize[0] = prefWidth;
            prefSize[1] = prefHeight;


            if(!useAutoBrightness) cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
            if(displayToWindow) {
                cv::namedWindow(win_name, 0);
                cv::createTrackbars(win_name);
                cv::resizeWindow(win_name, 1280, 960);
            }
    }
    int processFrame();
    void createTrackbars(const char* win_name);
    void updateTrackbarPos(const char* win_name);

};


/// @brief Create trackbar
/// @param win_name pointer to opencv window name
void IR_camTracking::createTrackbars(const char* win_name) {
	cv::createTrackbar("LowH", win_name, &l_HSV[0], 179);
	cv::createTrackbar("HighH", win_name, &u_HSV[0], 179);
	cv::createTrackbar("LowS", win_name, &l_HSV[1], 255);
	cv::createTrackbar("HighS", win_name, &u_HSV[1], 255);
	cv::createTrackbar("LowV", win_name, &l_HSV[2], 255);
	cv::createTrackbar("HighV", win_name, &u_HSV[2], 255);
}

/// @brief set trackbar pos/value
/// @param win_name pointer to opencv window name
void IR_camTracking::updateTrackbarPos(const char* win_name) {
	cv::setTrackbarPos("LowH", win_name, l_HSV[0]);
	cv::setTrackbarPos("LowS", win_name, l_HSV[1]);
	cv::setTrackbarPos("LowV", win_name, l_HSV[2]);
	cv::setTrackbarPos("HighH", win_name, u_HSV[0]);
	cv::setTrackbarPos("HighS", win_name, u_HSV[1]);
	cv::setTrackbarPos("HighV", win_name, u_HSV[2]);
}


int IR_camTracking::processFrame() {
    bool test = (cap.read(imgRaw));
    if(!test) {
        printf("error: Cannot read frame from webcam[%d]",idx);
        cv::destroyAllWindows();
        return -1;
    }
    if(takePerformance) perfObj.add_checkpoint("read");

    cv::resize(imgRaw, imgOriginal, cv::Size(prefSize[0],prefSize[1]), cv::INTER_LINEAR);
    if(takePerformance) perfObj.add_checkpoint("resize");

    // cv::flip(imgOriginal, imgFlipped, 1); //temporarily disabled
    imgFlipped = imgOriginal;
    cv::cvtColor(imgFlipped, imgHSV, cv::COLOR_BGR2HSV);
    if(takePerformance) perfObj.add_checkpoint("cvtC");

    cv::inRange(
        imgHSV,
        cv::Scalar(l_HSV[0], l_HSV[1], l_HSV[2]),
        cv::Scalar(u_HSV[0], u_HSV[1], u_HSV[2]),
        imgThreshold
    );
    if(takePerformance) perfObj.add_checkpoint("inRan");

    cv::erode(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 1);
    if(takePerformance) perfObj.add_checkpoint("erode");
    cv::dilate(imgThreshold, imgThreshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 6); 
    if(takePerformance) perfObj.add_checkpoint("dilate");

    cv::Moments imgMoments = cv::moments(imgThreshold);
    double dM01 = imgMoments.m01;
    double dM10 = imgMoments.m10;
    double dArea = imgMoments.m00;
    cv::findContours(imgThreshold, contours.at(idx), hierarchy.at(idx), cv::RETR_TREE,cv::CHAIN_APPROX_NONE);
    dArea = 0;
    validCnt_index = 0;
    totCnt_area = 0;
    for(unsigned int i=0; i<contours.at(idx).size(); i++) {
        dArea = cv::contourArea(contours.at(idx)[i]);
        if(dArea >= areaLim) {
            cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(contours.at(idx)[i]));
            int posX = contours.at(idx)[i][0].x, posY = contours.at(idx)[i][0].y+minRect.size.height/2;
            validCnt_pos[i][0]=posX;
            validCnt_pos[i][1]=posY;
            validCnt_index += 1;
            totCnt_area += dArea;
        }
    }
    if(takePerformance) perfObj.add_checkpoint("cntArea");
    if(validCnt_index > 0) {
        getAvg_cntPos(validCnt_pos, validCnt_index, totCnt_pos);
        cv::circle(imgFlipped,cv::Point(totCnt_pos[0],totCnt_pos[1]),50,cv::Scalar(0,0,0),2);
        cv::putText(
            imgFlipped, "["+to_string(int(totCnt_pos[0]))+","+to_string(int(totCnt_pos[1]))+"]",
            cv::Point(totCnt_pos[0],totCnt_pos[1]),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,0,0),2,false
        );
    }


    if(toDisplay) {
        if(takePerformance) perfObj.add_checkpoint("avgPos");
        cv::cvtColor(imgThreshold, imgThreshold, cv::COLOR_GRAY2BGR);
        if(takePerformance) perfObj.add_checkpoint("cvtCol");
        cv::vconcat(imgFlipped, imgThreshold, imgFlipped);
        if(takePerformance) perfObj.add_checkpoint("vconc");
    }
    return 0;
}


/// @brief Get avg xy coordinates from list of coordinates
/// @param allCnt array of coordinates
/// @param cntIndex number of elements in array
/// @param totCntPos_ptr "pointer" array to hold "returned" result/xy_coordinate
void IR_camTracking::getAvg_cntPos(float allCnt[20][2], int cntIndex, float totCntPos_ptr[2]) {
	float xTot=0, yTot = 0;
	for(int i=0; i<cntIndex; i++) {
		xTot += allCnt[i][0];
		yTot += allCnt[i][1];
	}
	totCntPos_ptr[0] = xTot / cntIndex;
	totCntPos_ptr[1] = yTot / cntIndex;
}

#endif