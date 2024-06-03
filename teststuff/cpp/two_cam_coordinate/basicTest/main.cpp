
#include <iostream>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>


#include "../two_cam_coordinate.hpp"
#include "../../basic/Performance/getPerformance.hpp"

using namespace std;

getPerf perfObj;


int main(int argc, char* argv[]) {
    float camPosition[2][2] = {{0, 0}, {25, 0}};
    float camAng_offs[2] = {90, 123};
    float givenPos[2], solvedPos[2];
    float toSolvePixels[2];
    float testRadius = 5;

    camTriangle camObj(camPosition, camAng_offs);

    //14|30|
    printf("| given        | solved       |\n");
    while(true) {
        for(int a=0; a<360; a++) {
            givenPos[0] = sin(toRadians((float)a))*testRadius+0;
            givenPos[1] = cos(toRadians((float)a))*testRadius+15;

            float toSolveAng[2];
            toSolveAng[0] = toDegrees(atan((givenPos[0]-camPosition[0][0])/(givenPos[1]-camPosition[0][1])))-90+camObj.ang_offset[0];
            float xDiff = givenPos[0]-camPosition[1][0];
            if(xDiff<0) toSolveAng[1] = toDegrees(-1*atan((givenPos[1]-camPosition[1][1])/(givenPos[0]-camPosition[1][0])))-180+camObj.ang_offset[1];
            else if(xDiff>0) toSolveAng[1] = 180+toDegrees(-1*atan((givenPos[1]-camPosition[1][1])/(givenPos[0]-camPosition[1][0])))-180+camObj.ang_offset[1];
            else if (xDiff==0) toSolveAng[1] = 0;
            
            toSolvePixels[0] = toSolveAng[0]/camObj.camCoef[0][0]+camObj.camRes[0][0]*0.5;
            toSolvePixels[1] = toSolveAng[1]/camObj.camCoef[1][0]+camObj.camRes[1][0]*0.5;
            camObj.solvePos(toSolvePixels, solvedPos, true);
            printf(
                "| [%4d, %4d] | [%4d, %4d] | px:%4d %4d",
                int(round(givenPos[0])), int(round(givenPos[1])),
                int(round(solvedPos[0])), int(round(solvedPos[1])),
                int(round(toSolvePixels[0])), int(round(toSolvePixels[1]))
            );
            usleep(10'000);
            printf("\n");
        }
    }

    return 0;
}