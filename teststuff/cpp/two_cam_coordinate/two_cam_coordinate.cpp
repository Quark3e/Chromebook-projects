
#include "two_cam_coordinate.hpp"


camTriangle::camTriangle(float camLocation[2][2], float camAngleOffset[2]) {
    // t1 = clock();

    ang_offset[0] = camAngleOffset[0];
    ang_offset[1] = camAngleOffset[1];

    for(int x=0; x<2; x++) { for(int y=0; y<2; y++) { camPos[x][y] = camLocation[x][y]; } }
    l_delta[0] = camPos[1][0]-camPos[0][0];
    l_delta[1] = camPos[1][1]-camPos[0][1];

    l_hypotenuse = float(sqrt(pow(l_delta[0],2)+pow(l_delta[1],2)));

    ang_d[0] = toDEGREES(atan(l_delta[1]/l_delta[0]));
    if(l_delta[1]==0) ang_d[1] = 90;
    else ang_d[1] = toDEGREES(atan(l_delta[0]/l_delta[1]));
    setCamCoefs();
}

/// @brief re-define filter variables for solvedPos
/// @param x_filter xPos
/// @param y_filter yPos
void camTriangle::setFilter(float x_filter=1, float y_filter=1) {
    filter_solvedPos[0] = x_filter;
    filter_solvedPos[1] = y_filter;
}
void camTriangle::setCamCoefs() {
    camCoef[0][0] = camFOV[0][0]/camRes[0][0];
    camCoef[0][1] = camFOV[0][1]/camRes[0][1];
    camCoef[1][0] = camFOV[1][0]/camRes[1][0];
    camCoef[1][1] = camFOV[1][1]/camRes[1][1];
}
void camTriangle::solveAngles() {
    ang_tri[0] = 90-ang_d[0]-ang_read[0];
    ang_tri[1] = 180-ang_offset[1]+ang_d[0]+ang_read[1];
    ang_p = 180 - abs(ang_tri[0]) - abs(ang_tri[1]);
}
/// @brief Solve real world positions from given webcam pixel positions
/// @param rawPos Unchanged pixel positions from both webcams NOTE:(x: left->right; y: up->down)
/// @param returnArr return array to get solved values
/// @param printText whether to print solved positions (in newline)
void camTriangle::solvePos(float rawPos[2], float returnArr[2], bool printText=false) {
    // if(frames>=framesLim) {
    //     t2 = clock();
    //     totalDelay = 1000*((t2-t1)/(double)CLOCKS_PER_SEC);
    //     fps = totalDelay/frames;
    //     printf(" fps:%5.2f  totalDelay:%5.2f\n", fps, totalDelay);
    //     frames=0;
    //     t1 = clock();
    // }
    // else frames++;

    read_pix[0][0] = rawPos[0]-camRes[0][0]*0.5;
    read_pix[1][0] = rawPos[1]-camRes[1][0]*0.5;
    read_pix[0][1] = camRes[0][1]*0.5-rawPos[0];
    read_pix[1][1] = camRes[1][1]*0.5-rawPos[1];

    if(printText) printf(" {%4d, %4d} ", int(read_pix[0][0]), int(read_pix[1][0]));

    ang_read[0] = read_pix[0][0]*camCoef[0][0];
    ang_read[1] = read_pix[1][0]*camCoef[1][0];
    solveAngles();

    l_tri[0] = (l_hypotenuse * sin(toRADIANS(ang_tri[1])) ) / sin(toRADIANS(ang_p));
    l_tri[1] = (l_hypotenuse * sin(toRADIANS(ang_tri[0])) ) / sin(toRADIANS(ang_p));
    
    if(printText ) std::cout <<" radi(ang):["<<std::setw(5)<< std::setprecision(2) << toRADIANS(ang_tri[1]) << ":" <<std::setw(5)<< std::setprecision(3) << toRADIANS(ang_tri[0]) << "] ";


    float tempPos[2];
    tempPos[0] = camPos[0][0]+cos(toRADIANS(ang_offset[0]+ang_read[0]))*l_tri[0];
    tempPos[1] = camPos[0][1]+sin(toRADIANS(ang_offset[0]+ang_read[0]))*l_tri[0];

    tempPos[0] = camPos[1][0]+cos(toRADIANS(ang_offset[1]-ang_read[1]))*l_tri[1];
    tempPos[1] = camPos[1][1]+sin(toRADIANS(ang_offset[1]-ang_read[1]))*l_tri[1];
    
    // solvedPos[0] = filter_solvedPos[0]*tempPos[0]+(1-filter_solvedPos[0]*solvedPos[0]);
    // solvedPos[1] = filter_solvedPos[1]*tempPos[1]+(1-filter_solvedPos[1]*solvedPos[1]);

    solvedPos[0] = tempPos[0];
    solvedPos[1] = tempPos[1];
    // solvedZ = sin(toRADIANS(read_pix[0][1]*camCoef[0][1]))*solvedPos[1];


    returnArr[0] = solvedPos[0];
    returnArr[1] = solvedPos[1];

    if(printText) {
        printf("angRead{%3d,%3d} ", int(ang_read[0]),int(ang_read[1]));
        printf("angTri{%3d,%3d,%3d} ", int(ang_tri[0]),int(ang_tri[1]),int(ang_p));
        printf("l_tri{%3d,%3d,%3d} ", int(l_tri[0]),int(l_tri[1]),int(l_hypotenuse));
    }

}

