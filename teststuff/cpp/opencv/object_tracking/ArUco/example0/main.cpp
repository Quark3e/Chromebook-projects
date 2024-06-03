/*
By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

                          License Agreement
               For Open Source Computer Vision Library
                       (3-clause BSD License)

Copyright (C) 2013, OpenCV Foundation, all rights reserved.
Third party copyrights are property of their respective owners.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the names of the copyright holders nor the names of the contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

This software is provided by the copyright holders and contributors "as is" and
any express or implied warranties, including, but not limited to, the implied
warranties of merchantability and fitness for a particular purpose are
disclaimed. In no event shall copyright holders or contributors be liable for
any direct, indirect, incidental, special, exemplary, or consequential damages
(including, but not limited to, procurement of substitute goods or services;
loss of use, data, or profits; or business interruption) however caused
and on any theory of liability, whether in contract, strict liability,
or tort (including negligence or otherwise) arising in any way out of
the use of this software, even if advised of the possibility of such damage.
*/


#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include "/home/pi/opencv_contrib/modules/aruco/samples/aruco_samples_utility.hpp"
//! [charucohdr]
#include <opencv2/aruco/charuco.hpp>
//! [charucohdr]
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
// #include "aruco_samples_utility.hpp"

namespace {
const char* about = "A tutorial code on charuco board creation and detection of charuco board with and without camera caliberation";
const char* keys = "{c        |       | Put value of c=1 to create charuco board;\nc=2 to detect charuco board without camera calibration;\nc=3 to detect charuco board with camera calibration and Pose Estimation}";
}

static inline void createBoard()
{
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    //! [createBoard]
    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(5, 7, 0.04f, 0.02f, dictionary);
    cv::Mat boardImage;
    board->draw(cv::Size(600, 500), boardImage, 10, 1);
    //! [createBoard]
    cv::imwrite("BoardImage.jpg", boardImage);
}

//! [detwcp]
static inline void detectCharucoBoardWithCalibrationPose()
{
    cv::VideoCapture inputVideo;
    inputVideo.open(4);
    //! [matdiscoff]
    cv::Mat cameraMatrix, distCoeffs;
    std::string filename = "calib.txt";
    bool readOk = readCameraParameters(filename, cameraMatrix, distCoeffs);
    //! [matdiscoff]
    if (!readOk) {
        std::cerr << "Invalid camera file" << std::endl;
    } else {
        //! [dictboard]
        cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
        cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(5, 7, 0.04f, 0.02f, dictionary);
        cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();
        //! [dictboard]
        while (inputVideo.grab()) {
            //! [inputImg]
            cv::Mat image;
            //! [inputImg]
            cv::Mat imageCopy;
            inputVideo.retrieve(image);
            image.copyTo(imageCopy);
            //! [midcornerdet]
            std::vector<int> markerIds;
            std::vector<std::vector<cv::Point2f> > markerCorners;
            cv::aruco::detectMarkers(image, board->dictionary, markerCorners, markerIds, params);
            //! [midcornerdet]
            // if at least one marker detected
            if (markerIds.size() > 0) {
                cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);
                //! [charidcor]
                std::vector<cv::Point2f> charucoCorners;
                std::vector<int> charucoIds;
                cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, image, board, charucoCorners, charucoIds, cameraMatrix, distCoeffs);
                //! [charidcor]
                // if at least one charuco corner detected
                if (charucoIds.size() > 0) {
                    cv::Scalar color = cv::Scalar(255, 0, 0);
                    //! [detcor]
                    cv::aruco::drawDetectedCornersCharuco(imageCopy, charucoCorners, charucoIds, color);
                    //! [detcor]
                    cv::Vec3d rvec, tvec;
                    //! [pose]
                    bool valid = cv::aruco::estimatePoseCharucoBoard(charucoCorners, charucoIds, board, cameraMatrix, distCoeffs, rvec, tvec);
                    //! [pose]
                    // if charuco pose is valid
                    if (valid)
                        cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvec, tvec, 0.1f);
                }
            }
            cv::imshow("out", imageCopy);
            char key = (char)cv::waitKey(30);
            if (key == 27)
                break;
        }
    }
}
//! [detwcp]

//! [detwc]
static inline void detectCharucoBoardWithoutCalibration()
{
    cv::VideoCapture inputVideo;
    inputVideo.open(2);
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(5, 7, 0.04f, 0.02f, dictionary);

    cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();
    params->cornerRefinementMethod = cv::aruco::CORNER_REFINE_NONE;
    while (inputVideo.grab()) {
        cv::Mat image, imageCopy;
        inputVideo.retrieve(image);
        image.copyTo(imageCopy);
        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f> > markerCorners;
        cv::aruco::detectMarkers(image, board->dictionary, markerCorners, markerIds, params);
        //or
        //cv::aruco::detectMarkers(image, dictionary, markerCorners, markerIds, params);
        // if at least one marker detected
        if (markerIds.size() > 0) {
            cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);
            //! [charidcorwc]
            std::vector<cv::Point2f> charucoCorners;
            std::vector<int> charucoIds;
            cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, image, board, charucoCorners, charucoIds);
            //! [charidcorwc]
            // if at least one charuco corner detected
            if (charucoIds.size() > 0)
                cv::aruco::drawDetectedCornersCharuco(imageCopy, charucoCorners, charucoIds, cv::Scalar(255, 0, 0));
        }
        cv::imshow("out", imageCopy);
        char key = (char)cv::waitKey(30);
        if (key == 27)
            break;
    }
}
//! [detwc]

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);
    if (argc < 2) {
        parser.printMessage();
        return 0;
    }
    int choose = parser.get<int>("c");
    switch (choose) {
    case 1:
        createBoard();
        std::cout << "An image named BoardImg.jpg is generated in folder containing this file" << std::endl;
        break;
    case 2:
        detectCharucoBoardWithoutCalibration();
        break;
    case 3:
        detectCharucoBoardWithCalibrationPose();
        break;
    default:
        break;
    }
    return 0;
}
