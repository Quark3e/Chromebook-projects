
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

class IR_camTracking {

    public:
    int prefSize[2];

    int areaLim = 1000;
    vector<vector<float>> validCnt_pos;
    vector<vector<float>> totCnt_pos;
    int validCnt_index = 0;
    float totCnt_area = 0;

    vector<cv::VideoCapture> caps;
    vector<vector<cv::Vec4i>> hierarchy;
    vector<vector<vector<cv::Point>>> contours;

    IR_camTracking(
        vector<int> camIndexes,
        int prefWidth = 640,
        int prefHeight = 480,
        ): caps() {
            prefSize[0] = prefWidth;
            prefSize[1] = prefHeight;

            vector<cv::Vec4i> hTemp;
            vector<vector<cv::Point>> cTemp;
            for(auto i: camIndexes) {
                caps.emplace_back(cv::VideoCapture(i));
                contours.push_back(cTemp);
                hierarchy.push_back(hTemp);
            }
    }


};


#endif