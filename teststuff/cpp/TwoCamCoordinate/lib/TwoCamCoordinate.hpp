#pragma once
#ifndef H_TWO_CAM_COORDINATE
#define H_TWO_CAM_COORDINATE

#include <math.h>
#include <cmath>
#include <numbers>
#include <time.h>
#include <iostream>
#include <iomanip>

#ifndef M_PI
#define M_PI  3.1415926535
#endif


#ifndef toDEGREES
#define toDEGREES(RAD) ((RAD)*180/M_PI)
#endif
#ifndef toRADIANS
#define toRADIANS(DEG) ((DEG)*M_PI/180.0)
#endif


/// @brief Class to solve object position relative to the position of two webcams via seen pixel positions
class camTriangle {
    public:
    float camPos[2][2];
    float camRes[2][2] = {{640, 480}, {640, 480}};
    float camFOV[2][2] = {{60, 45}, {60, 45}};
    float camCoef[2][2] = {{0, 0}, {0, 0}}; 

    float l_delta[2] = {-1, -1};
    float l_hypotenuse = -1;
    float l_tri[2] = {-1, -1};

    float ang_offset[2] = {0, 0};
    float ang_d[2] = {0, 0};

    float ang_tri[2] = {0, 0};
    float ang_p = 0;

    float read_pix[2][2] = {{0, 0}, {0, 0}};
    float ang_read[2] = {0, 0};


    float solvedPos[2] = {0, 0};
    float filter_solvedPos[2] = {1, 1};

    int fps = 0;
    int frames = 0;
    int framesLim = 1;
    float totalDelay = 0;
    clock_t t1, t2;

    /// @brief camTriangle class initializor function
    /// @param camLocation real world location of webcams {{x, y}, {x, y}}
    /// @param camAngleOffset [unit: degrees] webcam offset angle relative to X-axis plane, perpendicular is 90 degrees
    camTriangle(float camLocation[2][2], float camAngleOffset[2]);
    void setCamCoefs();
    void solveAngles();
    void solvePos(float rawPos[2], float returnArr[2], bool printText);
    void setFilter(float x_filter, float y_filter);
};



#endif //H_TWO_CAM_COORDINATE
