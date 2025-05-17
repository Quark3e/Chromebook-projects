
#include "TwoCamCoordinate.hpp"

#include "useful.hpp"

camTriangle::camTriangle(const camTriangle& _copy):
    camPos(_copy.camPos),
    camRes(_copy.camRes),
    camFOV(_copy.camFOV),
    camScalar(_copy.camScalar),
    l_delta(_copy.l_delta),
    l_hypotenuse(_copy.l_hypotenuse),
    l_tri(_copy.l_tri),
    ang_offset(_copy.ang_offset),
    ang_d(_copy.ang_d),
    ang_tri(_copy.ang_tri),
    ang_p(_copy.ang_p),
    read_pix(_copy.read_pix),
    ang_read(_copy.ang_read),
    solvedPos(_copy.solvedPos),
    solvedPos_filter(_copy.solvedPos_filter)
{
    
}
camTriangle::camTriangle(std::vector<pos2d<double>> camLocation, std::vector<double> camAngleOffset) {
    // t1 = clock();

    ang_offset[0] = camAngleOffset[0];
    ang_offset[1] = camAngleOffset[1];

    camPos = camLocation;

    l_delta[0] = camPos[1][0]-camPos[0][0];
    l_delta[1] = camPos[1][1]-camPos[0][1];

    l_hypotenuse = double(sqrt(pow(l_delta[0],2)+pow(l_delta[1],2)));

    ang_d[0] = toDEGREES(atan(l_delta[1]/l_delta[0]));
    if(l_delta[1]==0) ang_d[1] = 90;
    else ang_d[1] = toDEGREES(atan(l_delta[0]/l_delta[1]));
    setcamScalars();
}
camTriangle::~camTriangle() {}

camTriangle& camTriangle::operator=(const camTriangle& _copy) {
    camPos = _copy.camPos;
    camRes = _copy.camRes;
    camFOV = _copy.camFOV;
    camScalar = _copy.camScalar;
    l_delta = _copy.l_delta;
    l_hypotenuse = _copy.l_hypotenuse;
    l_tri = _copy.l_tri;
    ang_offset = _copy.ang_offset;
    ang_d = _copy.ang_d;
    ang_tri = _copy.ang_tri;
    ang_p = _copy.ang_p;
    read_pix = _copy.read_pix;
    ang_read = _copy.ang_read;
    solvedPos = _copy.solvedPos;
    solvedPos_filter = _copy.solvedPos_filter;

    return *this;
}

void camTriangle::setFilter(double x_filter=1, double y_filter=1) {
    solvedPos_filter[0] = x_filter;
    solvedPos_filter[1] = y_filter;
}
void camTriangle::setcamScalars() {
    camScalar[0][0] = camFOV[0][0]/camRes[0][0];
    camScalar[0][1] = camFOV[0][1]/camRes[0][1];
    camScalar[1][0] = camFOV[1][0]/camRes[1][0];
    camScalar[1][1] = camFOV[1][1]/camRes[1][1];
}
void camTriangle::solveAngles() {
    ang_tri[0] = 90-ang_d[0]-ang_read[0];
    ang_tri[1] = 180-ang_offset[1]+ang_d[0]+ang_read[1];
    ang_p = 180 - std::abs(ang_tri[0]) - std::abs(ang_tri[1]);
}

void camTriangle::solvePos(std::vector<pos2d<double>> rawPxPos, bool printText) {
    // if(frames>=framesLim) {
    //     t2 = clock();
    //     totalDelay = 1000*((t2-t1)/(double)CLOCKS_PER_SEC);
    //     fps = totalDelay/frames;
    //     printf(" fps:%5.2f  totalDelay:%5.2f\n", fps, totalDelay);
    //     frames=0;
    //     t1 = clock();
    // }
    // else frames++;

    l_delta[0] = camPos[1][0]-camPos[0][0];
    l_delta[1] = camPos[1][1]-camPos[0][1];

    l_hypotenuse = double(sqrt(pow(l_delta[0],2)+pow(l_delta[1],2)));

    ang_d[0] = toDEGREES(atan(l_delta[1]/l_delta[0]));
    if(l_delta[1]==0) ang_d[1] = 90;
    else ang_d[1] = toDEGREES(atan(l_delta[0]/l_delta[1]));


    read_pix[0][0] = (-camRes[0][0]*0.5)+rawPxPos[0][0];
    read_pix[1][0] = (-camRes[1][0]*0.5)+rawPxPos[1][0];
    read_pix[0][1] = camRes[0][1]*0.5-rawPxPos[0][1];
    read_pix[1][1] = camRes[1][1]*0.5-rawPxPos[1][1];

    // if(printText) std::cout << "{"<<read_pix.at(0)<<","<<read_pix.at(1)<<"} ";

    ang_read[0] = read_pix[0][0]*camScalar[0][0];
    ang_read[1] = read_pix[1][0]*camScalar[1][0];
    solveAngles();
    ang_read[2] = read_pix[0][1]*camScalar[0][1];

    // std::cout << " camTri::ang_read"

    l_tri[0] = (l_hypotenuse * sin(toRADIANS(ang_tri[1])) ) / sin(toRADIANS(ang_p));
    l_tri[1] = (l_hypotenuse * sin(toRADIANS(ang_tri[0])) ) / sin(toRADIANS(ang_p));
    
    if(printText ) std::cout <<" radi(ang):["<<std::setw(5)<< std::setprecision(2) << toRADIANS(ang_tri[1]) << ":" <<std::setw(5)<< std::setprecision(3) << toRADIANS(ang_tri[0]) << "] ";


    std::vector<double> tempPos(3, 0);
    /// Possible solution #1
    tempPos[0] = camPos[0][0]+cos(toRADIANS(ang_offset[0]+ang_read[0]))*l_tri[0];
    tempPos[1] = camPos[0][1]+sin(toRADIANS(ang_offset[0]+ang_read[0]))*l_tri[0];
    
    /// Possible solution #2
    tempPos[0] = camPos[1][0]+cos(toRADIANS(ang_offset[1]-ang_read[1]))*l_tri[1];
    tempPos[1] = camPos[1][1]+sin(toRADIANS(ang_offset[1]-ang_read[1]))*l_tri[1];
    
    tempPos[2] = tempPos[1] * tan(toRADIANS(ang_offset[1]+ang_read[2]));

    solvedPos[0] = solvedPos_filter[0]*tempPos[0]+((1.0-solvedPos_filter[0])*solvedPos[0]);
    solvedPos[1] = solvedPos_filter[1]*tempPos[1]+((1.0-solvedPos_filter[1])*solvedPos[1]);
    solvedPos[2] = solvedPos_filter[2]*tempPos[2]+((1.0-solvedPos_filter[2])*solvedPos[2]);


    if(printText) {
        printf("angRead{%3d,%3d} ", int(ang_read[0]),int(ang_read[1]));
        printf("angTri{%3d,%3d,%3d} ", int(ang_tri[0]),int(ang_tri[1]),int(ang_p));
        printf("l_tri{%3d,%3d,%3d} ", int(l_tri[0]),int(l_tri[1]),int(l_hypotenuse));
    }

}

