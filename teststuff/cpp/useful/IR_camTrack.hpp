
#pragma once
#ifndef H_IR_CAMTRACK
#define H_IR_CAMTRACK


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
// opencv/image tracking
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>


#include <HC_useful/getPerformance.hpp>

using namespace std;



/// @brief class for tracking and reading screen pixel coordinate from tracked object
/// @param camIndex index to camera for cv::VideoCapture to create a object with
/// @param prefWidth prefered width for which to scale read image to. Useful for 
/// reducing processing/tracking time
/// @param prefHeight prefered width for which to scale read image to. Useful for
/// reducing processing/tracking time
/// @param setAutoBright whether to use webcams inbuilt automatic brightness adjusting.
///  If false it'll set cv::CAP_PROP_AUTO_EXPOSURE to 0
/// @param useWindow whether to display everything in a window
/// @param readDelays whether to take performance of everything with the performance header
/// @param windowName window name to display img's on
class IR_camTracking {


    public:
    int camIdx;
    getPerf perfObj;
    
    int prefSize[2];

    int areaLim = 1000;
    vector<vector<float>> allCnt_pos;

    /// @brief final/total/avg contour position
    float totCnt_pos[2];
    /// @brief total sum of contour areas
    float totCnt_area = 0;

    cv::VideoCapture cap;
    vector<cv::Vec4i> hierarchy;
    vector<vector<cv::Point>> contours;

    cv::Mat imgRaw, imgOriginal, imgFlipped, imgHSV, imgThreshold;

    bool useAutoBrightness = true;
    bool displayToWindow = false;
    bool takePerformance = true;
    bool printAll = true;

    int l_HSV[3] = {0, 0, 255};
    int u_HSV[3] = {179, 9, 255};

    const char* win_name = "Window";

    /** Return code holder variable incase `IR_camTracking::processCam()` is called without access to return value or by some error
     *  0 = normal/successful
     * -1 = error
    */
    int processReturnCode = 0;

    IR_camTracking(
        int camIndex,
        int prefWidth = 640,
        int prefHeight = 480,
        bool setAutoBright = true,
        bool useWindow = false,
        bool readDelays = true,
        const char* windowName = "Window"
    );
    IR_camTracking(
        int camIndex,
        const char* windowName = "Window",
        int prefWidth = 640,
        int prefHeight = 480
    );

    /// @brief create a window for the class. If `windowName` is already used then it won't create a new window
    /// @param windowName name of the window
    /// @param width_resize new width for window
    /// @param height_resize new height for window
    void setup_window(
        const char* windowName = "Window",
        int width_resize = 1280,
        int height_resize = 960
    );

    void getAvg_cntPos();
    int processCam();
    void createTrackbars(const char* win_name);
    void updateTrackbarPos(const char* win_name);
    void close();

};

void hsv_settingsRead(
    std::vector<IR_camTracking>& camObject,
    int argHSV[2][3],
    const char* window_name = "",
    int indeks=1,
    string filePath="hsv_settings.dat",
    bool displayWin=true
);
void hsv_settingsWrite(
    int argHSV[2][3],
    int indeks=0,
    bool overWrite=false,
    string filePath="hsv_settings.dat"
);


#endif