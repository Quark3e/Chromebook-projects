
#include "TwoCamCoordinate.hpp"

#include "useful.hpp"

camTriangle::camTriangle(const camTriangle& _copy):
    camPos(_copy.camPos),
    camRes(_copy.camRes),
    camFOV(_copy.camFOV),
    camScalar(_copy.camScalar),
    pos_delta(_copy.pos_delta),
    // l_hypotenuse(_copy.l_hypotenuse),
    l_tri(_copy.l_tri),
    ang_offset(_copy.ang_offset),
    ang_d(_copy.ang_d),
    ang_tri(_copy.ang_tri),
    // ang_p(_copy.ang_p),
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

    pos_delta = camPos[1]-camPos[0];

    // l_hypotenuse = double(sqrt(pow(pos_delta[0],2)+pow(pos_delta[1],2)));

    // ang_d[0] = toDEGREES(atan(pos_delta[1]/pos_delta[0]));
    // if(pos_delta[1]==0) ang_d[1] = 90;
    // else ang_d[1] = toDEGREES(atan(pos_delta[0]/pos_delta[1]));
    setcamScalars();
}
camTriangle::~camTriangle() {}

camTriangle& camTriangle::operator=(const camTriangle& _copy) {
    camPos = _copy.camPos;
    camRes = _copy.camRes;
    camFOV = _copy.camFOV;
    camScalar = _copy.camScalar;
    pos_delta = _copy.pos_delta;
    // l_hypotenuse = _copy.l_hypotenuse;
    l_tri = _copy.l_tri;
    ang_offset = _copy.ang_offset;
    ang_d = _copy.ang_d;
    ang_tri = _copy.ang_tri;
    // // ang_p = _copy.ang_p;
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
    // camScalar[0][0] = camFOV[0][0]/camRes[0][0];
    // camScalar[0][1] = camFOV[0][1]/camRes[0][1];
    // camScalar[1][0] = camFOV[1][0]/camRes[1][0];
    // camScalar[1][1] = camFOV[1][1]/camRes[1][1];

    camScalar[0] = camFOV[0]/camRes[0];
    camScalar[1] = camFOV[1]/camRes[1];
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


    pos_delta = camPos[1]-camPos[0];

    read_pix[0][0] = (-camRes[0][0]*0.5)+rawPxPos[0][0];
    read_pix[1][0] = (-camRes[1][0]*0.5)+rawPxPos[1][0];
    read_pix[0][1] = camRes[0][1]*0.5-rawPxPos[0][1];
    read_pix[1][1] = camRes[1][1]*0.5-rawPxPos[1][1];

    ang_read[0] = read_pix[0][0]*camScalar[0][0];
    ang_read[1] = read_pix[1][0]*camScalar[1][0];

    ang_read[2] = read_pix[0][1]*camScalar[0][1];

    ang_d[0] = toDEGREES(camPos[1].getTheta(camPos[0], true));
    ang_d[1] = toDEGREES(camPos[0].getTheta(camPos[1], true));

    ang_tri[0] = std::abs(ang_d[0]-(ang_offset[0]-ang_read[0])); //+ sign, assuming CW ang_read is positive, and CCW ang_offset is positive
    ang_tri[1] = std::abs(ang_d[1]-(ang_offset[1]-ang_read[1]));

    for(size_t i=0; i<2; i++) {
        if(ang_tri[i]>=180) ang_tri[i] = 360 - ang_tri[i];
        if(ang_tri[i]<=  0) ang_tri[i] = 360 + ang_tri[i];
    }

    ang_tri[2] = 180 - ang_tri[0] - ang_tri[1];

    l_tri[0] = (std::sin(toRADIANS(ang_tri[0]))*pos_delta.getHypotenuse())/std::sin(toRADIANS(ang_tri[2]));
    l_tri[1] = (std::sin(toRADIANS(ang_tri[1]))*pos_delta.getHypotenuse())/std::sin(toRADIANS(ang_tri[2]));


    std::vector<double> tempPos(3, 0);
    /// Possible solution #1
    tempPos[0] = camPos[0][0]+cos(toRADIANS(ang_offset[0]-ang_read[0]))*l_tri[1];
    tempPos[1] = camPos[0][1]+sin(toRADIANS(ang_offset[0]-ang_read[0]))*l_tri[1];
    
    /// Possible solution #2
    // tempPos[0] = camPos[1][0]+cos(toRADIANS(ang_offset[1]-ang_read[1]))*l_tri[0];
    // tempPos[1] = camPos[1][1]+sin(toRADIANS(ang_offset[1]-ang_read[1]))*l_tri[0];
    
    tempPos[2] = tempPos[1] * tan(toRADIANS(ang_read[2]));


    solvedPos[0] = solvedPos_filter[0]*tempPos[0]+((1.0-solvedPos_filter[0])*solvedPos[0]);
    solvedPos[1] = solvedPos_filter[1]*tempPos[1]+((1.0-solvedPos_filter[1])*solvedPos[1]);
    solvedPos[2] = solvedPos_filter[2]*tempPos[2]+((1.0-solvedPos_filter[2])*solvedPos[2]);

}

