
// #pragma once
// #ifndef H_IR_CAMTRACK
// #define H_IR_CAMTRACK


// #include <iostream>
// #include <vector>
// #include <string>
// // opencv/image tracking
// #include <opencv4/opencv2/opencv.hpp>
// #include <opencv4/opencv2/highgui/highgui.hpp>
// #include <opencv4/opencv2/imgproc/imgproc.hpp>


// using namespace std;

// class IR_camTracking {

//     public:
//     int prefSize[2];

//     int areaLim = 1000;
//     vector<vector<float>> validCnt_pos;
//     vector<vector<float>> totCnt_pos;
//     int validCnt_index = 0;
//     float totCnt_area = 0;

//     vector<cv::VideoCapture> caps;
//     vector<vector<cv::Vec4i>> hierarchy;
//     vector<vector<vector<cv::Point>>> contours;

//     vector<cv::Mat>> imgRaw;
//     vector<cv::Mat>> imgOriginal;
//     vector<cv::Mat>> imgFlipped;
//     vector<cv::Mat>> imgHSV;
//     vector<cv::Mat>> imgThreshold;

//     bool useAutoBrightness = true;
//     bool displayToWindow = false;
//     bool takePerformance = true;

//     int l_HSV[3] = {0, 0, 255};
//     int u_HSV[3] = {179, 9, 255};

//     const char* win_name = "Window";

//     IR_camTracking(
//         vector<int> camIndexes,
//         int prefWidth = 640,
//         int prefHeight = 480,
//         ): caps() {
//             prefSize[0] = prefWidth;
//             prefSize[1] = prefHeight;

//             vector<cv::Vec4i> hTemp;
//             vector<vector<cv::Point>> cTemp;
//             for(auto i: camIndexes) {
//                 caps.emplace_back(cv::VideoCapture(i));
//                 if(!caps.back().isOpened()) {
//                     cout << "error: Cannot open cam: idx:"<< i << endl;
//                     return;
//                 }
//                 if(useAutoBrightness) caps.back().set(cv::CAP_PROP_AUTO_EXPOSURE, 0);
//                 contours.push_back(cTemp);
//                 hierarchy.push_back(hTemp);

//                 // imgRaw
//             }
//             if(displayToWindow) {
//                 cv::namedWindow(win_name, 0);
//                 cv::createTrackbars(win_name);
//                 cv::resizeWindow(win_name, 1280, 960);
//             }
//     }
//     int processFrame(cv::VideoCapture* cap, int idx, bool toDisplay);
//     void createTrackbars(const char* win_name);
//     void updateTrackbarPos(const char* win_name);

// };


// /// @brief Create trackbar
// /// @param win_name pointer to opencv window name
// void IR_camTracking::createTrackbars(const char* win_name) {
// 	cv::createTrackbar("LowH", win_name, &l_HSV[0], 179);
// 	cv::createTrackbar("HighH", win_name, &u_HSV[0], 179);
// 	cv::createTrackbar("LowS", win_name, &l_HSV[1], 255);
// 	cv::createTrackbar("HighS", win_name, &u_HSV[1], 255);
// 	cv::createTrackbar("LowV", win_name, &l_HSV[2], 255);
// 	cv::createTrackbar("HighV", win_name, &u_HSV[2], 255);
// }

// /// @brief set trackbar pos/value
// /// @param win_name pointer to opencv window name
// void IR_camTracking::updateTrackbarPos(const char* win_name) {
// 	cv::setTrackbarPos("LowH", win_name, l_HSV[0]);
// 	cv::setTrackbarPos("LowS", win_name, l_HSV[1]);
// 	cv::setTrackbarPos("LowV", win_name, l_HSV[2]);
// 	cv::setTrackbarPos("HighH", win_name, u_HSV[0]);
// 	cv::setTrackbarPos("HighS", win_name, u_HSV[1]);
// 	cv::setTrackbarPos("HighV", win_name, u_HSV[2]);
// }

// int IR_camTracking::processFrame(cv::VideoCapture* cap, int idx, bool toDisplay) {
//     bool test = (cap->read(imgRaw[idx]));
//     if(!test) {
//         printf("error: Cannot read frame from webcam[%d]",idx);
//         cv::destroyAllWindows();
//         return -1;
//     }
//     if(takePerformance) perfObj.add_checkpoint("read");

//     cv::resize(imgRaw[idx], imgOriginal[idx], cv::Size(prefSize[0],prefSize[1]), cv::INTER_LINEAR);
//     if(takePerformance) perfObj.add_checkpoint("resize");

//     // cv::flip(imgOriginal[idx], imgFlipped[idx], 1); //temporarily disabled
//     imgFlipped[idx] = imgOriginal[idx];
//     cv::cvtColor(imgFlipped[idx], imgHSV[idx], cv::COLOR_BGR2HSV);
//     if(takePerformance) perfObj.add_checkpoint("cvtC");

//     cv::inRange(
//         imgHSV[idx],
//         cv::Scalar(l_HSV[0], l_HSV[1], l_HSV[2]),
//         cv::Scalar(u_HSV[0], u_HSV[1], u_HSV[2]),
//         imgThreshold[idx]
//     );
//     if(takePerformance) perfObj.add_checkpoint("inRan");

//     cv::erode(imgThreshold[idx], imgThreshold[idx], cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 1);
//     if(takePerformance) perfObj.add_checkpoint("erode");
//     cv::dilate(imgThreshold[idx], imgThreshold[idx], cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), cv::Point(-1, -1), 6); 
//     if(takePerformance) perfObj.add_checkpoint("dilate");

//     cv::Moments imgMoments = cv::moments(imgThreshold[idx]);
//     double dM01 = imgMoments.m01;
//     double dM10 = imgMoments.m10;
//     double dArea = imgMoments.m00;
//     cv::findContours(imgThreshold[idx], contours.at(idx), hierarchy.at(idx), cv::RETR_TREE,cv::CHAIN_APPROX_NONE);
//     dArea = 0;
//     validCnt_index = 0;
//     totCnt_area = 0;
//     for(unsigned int i=0; i<contours.at(idx).size(); i++) {
//         dArea = cv::contourArea(contours.at(idx)[i]);
//         if(dArea >= areaLim) {
//             cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(contours.at(idx)[i]));
//             int posX = contours.at(idx)[i][0].x, posY = contours.at(idx)[i][0].y+minRect.size.height/2;
//             validCnt_pos[i][0]=posX;
//             validCnt_pos[i][1]=posY;
//             validCnt_index += 1;
//             totCnt_area += dArea;
//         }
//     }
//     if(takePerformance) perfObj.add_checkpoint("cntArea");
//     if(validCnt_index > 0) {
//         getAvg_cntPos(validCnt_pos, validCnt_index, totCnt_pos[idx]);
//         cv::circle(imgFlipped[idx],cv::Point(totCnt_pos[idx][0],totCnt_pos[idx][1]),50,cv::Scalar(0,0,0),2);
//         cv::putText(
//             imgFlipped[idx], "["+to_string(int(totCnt_pos[idx][0]))+","+to_string(int(totCnt_pos[idx][1]))+"]",
//             cv::Point(totCnt_pos[idx][0],totCnt_pos[idx][1]),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,0,0),2,false
//         );
//     }


//     if(toDisplay) {
//         if(takePerformance) perfObj.add_checkpoint("avgPos");
//         cv::cvtColor(imgThreshold[idx], imgThreshold[idx], cv::COLOR_GRAY2BGR);
//         if(takePerformance) perfObj.add_checkpoint("cvtCol");
//         cv::vconcat(imgFlipped[idx], imgThreshold[idx], imgFlipped[idx]);
//         if(takePerformance) perfObj.add_checkpoint("vconc");
//     }
//     return 0;
// }

// #endif