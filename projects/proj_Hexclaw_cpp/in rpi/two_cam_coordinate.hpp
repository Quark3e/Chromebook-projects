#pragma once

#include <math.h>
#include <cmath>
#include <numbers>

float toDegrees(float radians) { return (radians*pi)/180; }
float toRadians(float degrees) { return (degrees*180)/pi; }

/// @brief Class to solve object position relative to the position of two webcams via seen pixel positions
class camTriangle {
    public:
    float camPos[2][2];
    float camRes[2][2] = {{640, 480}, {640, 480}};
    float camFOV[2][2] = {{60, 45}, {60, 45}};
    float camCoef[2][2] = {{0, 0}, {0, 0}}; 

    float l_delta[2] = {-1, -1};
    float l_hypotenuse[2] = {-1, -1};
    float l_tri[2] = {-1, -1};

    float ang_offset[2] = {0, 0};
    float ang_d[2] = {0, 0};

    float ang_tri[2] = {0, 0};
    float ang_p = 0;

    float read_pix[2][2] = {{0, 0}, {0, 0}};
    float ang_read[2] = {0, 0};


    float solvedPos[2] = {0, 0};

    /// @brief camTriangle class initializor function
    /// @param camLocation real world location of webcams {{x, y}, {x, y}}
    /// @param camAngleOffset [unit: degrees] webcam offset angle relative to X-axis plane, perpendicular is 90 degrees
    camTriangle(float camLocation[2][2], float camAngleOffset[2]) {
        ang_offset[0] = camAngleOffset[0];
        ang_offset[1] = camAngleOffset[1];

        for(int x=0; x<2; x++) { for(int y=0: y<2; y++) { camPos[x][y] = camLocation[x][y]; } }
        l_delta[0] = camPos[1][0]-camPos[0][0];
        l_delta[1] = camPos[1][1]-camPos[0][1];
    
        l_hypotenuse = sqrt(pow(l_delta[0],2)+pow(l_delta[1],2));

        ang_d[0] = toDegrees(atan(l_delta[1]/l_delta[0]));
        if(l_delta[1]==0) ang_d[1] = 90;
        else ang_d[1] = toDegrees(atan(l_delta[0]/l_delta[1]));
        setCamCoefs();
    }
    void setCamCoefs() {
        camCoef[0][0] = camFOV[0][0]/camRes[0][0];
        camCoef[0][1] = camFOV[0][1]/camRes[0][1];
        camCoef[1][0] = camFOV[1][0]/camRes[1][0];
        camCoef[1][1] = camFOV[1][1]/camRes[1][1];
    }
    void solveAngles() {
        ang_tri[0] = 90-ang_d[0]-ang_read[0];
        ang_tri[1] = 180-ang_offset[1]+ang_d[0]+ang_read[1];
        ang_p = 180 - abs(ang_tri[0]) - abs(ang_tri[1]);
    }
    /// @brief Solve real world positions from given webcam pixel positions
    /// @param rawPos Unchanged pixel positions from both webcams NOTE:(x: left->right; y: up->down)
    /// @param returnArr return array to get solved values
    /// @param printText whether to print solved positions (in newline)
    void solvePos(float rawPos[2], float returnArr[2], bool printText=false) {
        read_pix[0][0] = rawPos[0]-camRes[0][0]*0.5;
        read_pix[1][0] = rawPos[1]-camRes[1][0]*0.5;
        read_pix[0][1] = camRes[0][1]*0.5-rawPos[0];
        read_pix[1][1] = camRes[1][1]*0.5-rawPos[1];

        ang_read[0] = read_pix[0][0]*camCoef[0][0];
        ang_read[1] = read_pix[1][0]*camCoef[1][0];
        solveAngles();

        l_tri[0] = (l_hypotenuse*sin(toRadians(ang_tri[1])))/sin(toRadians(ang_p));
        l_tri[1] = (l_hypotenuse*sin(toRadians(ang_tri[0])))/sin(toRadians(ang_p));

        solvedPos[0] = camPos[0][0]+cos(toRadians(ang_offset[0]+ang_read[0]))*l_tri[0];
        solvedPos[1] = camPos[0][1]+sin(toRadians(ang_offset[0]+ang_read[0]))*l_tri[0];

        solvedPos[0] = camPos[1][0]+cos(toRadians(ang_offset[1]-ang_read[1]))*l_tri[1];
        solvedPos[1] = camPos[1][1]+cos(toRadians(ang_offset[1]-ang_read[1]))*l_tri[1];
        
        returnArr[0] = solvedPos[0];
        returnArr[1] = solvedPos[1];

        if(printText) printf("[%f, %f]     \r", solvedPos[0],solvedPos[1]);
    }
};

