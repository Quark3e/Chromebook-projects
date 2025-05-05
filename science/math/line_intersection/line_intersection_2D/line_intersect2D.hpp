
#pragma once
#ifndef HPP__LINE_INTERSECT_2D

#include <cmath>
#include <cstdlib>
#include <vector>
#include <useful.hpp> //for debugging purposes
#include <pos2d.hpp>


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


#ifndef RADIANS
// Convert degrees to radians
#define RADIANS(deg) (double(deg)*(double(M_PI)/double(180.0)))
#endif //RADIANS

#ifndef DEGREES
// Convert radians to degrees
#define DEGREES(rad) (double(rad)*double(180.0))/double(M_PI)
#endif //DEGREES



template<typename _varType>
inline std::vector<_varType> getLinearCoefficients(pos2d<_varType> point_A, pos2d<_varType> point_B) {
    if(point_A==point_B) throw std::invalid_argument("both points can't be the same");

    pos2d<_varType> delta(point_B-point_A);
    if(delta.x==0) throw std::invalid_argument("line isn't allowed to be completely vertical");

    _varType a_1 = delta.y / delta.x;
    _varType a_0 = point_A.y - point_A.x * a_1;

    return std::vector<_varType>{a_0, a_1};
}

template<typename _varType>
inline double getTheta(pos2d<_varType> pos__line_end, pos2d<_varType> pos__line_pivot) {
    if(pos__line_end==pos__line_pivot) throw std::invalid_argument("pos__line_end and pos__line_pivot can't be the same.");
    pos2d<_varType> delta(pos__line_end - pos__line_pivot);
    
    pos2d<_varType> magnitude = delta.getModify([](_varType _var) { return (_var>0? _var : _var*(-1)); });
    // pos2d<_varType> magnitude = delta.std::abs();

    pos2d<int> unitVec(0, 0);
    for(size_t i=0; i<2; i++) {
        if(magnitude[i]!=0) unitVec[i] = magnitude[i];
    }

    _varType    radius  = delta.getHypotenuse();
    double      theta   = DEGREES(acos(delta.x / radius));
    if(unitVec.y < 0) theta = 360 - theta;

    return theta;
}

template<typename _varType>
inline pos2d<_varType> rotateCoordinate(pos2d<_varType> toRotate, pos2d<_varType> centerOfRotation, double anglesToRotate_CCW) {
    if(toRotate==centerOfRotation) throw std::invalid_argument("toRotate and centerOfRotation argument coordinates can't be the same.");
    pos2d<_varType> delta(toRotate - centerOfRotation);

    pos2d<_varType> magnitude = delta.getModify([](_varType _var) { return (_var>0? _var : _var*(-1)); });
    
    pos2d<int> unitVec(0, 0);
    for(size_t i=0; i<2; i++) {
        if(magnitude[i]!=0) {
            unitVec[i] = std::roundf(delta[i]/magnitude[i]);
        }
        else {
        }
    }
    
    _varType    radius  = delta.getHypotenuse();
    double      theta   = DEGREES(acos(delta.x / radius));
    if(unitVec.y < 0) theta = 360 - theta;

    pos2d<_varType> newPos(
        centerOfRotation.x +  radius * cos(RADIANS(theta + anglesToRotate_CCW)),
        centerOfRotation.y +  radius * sin(RADIANS(theta + anglesToRotate_CCW))
    );
    
    return newPos;
}

template<typename _varType>
inline std::vector<pos2d<_varType>> rotateCoordinates(std::vector<pos2d<_varType>> coordinatesToRotate, pos2d<_varType> centerOfRotation, double anglesToRotate_CCW) {
    std::vector<pos2d<_varType>> rotated(coordinatesToRotate.size());
    for(size_t i=0; i<coordinatesToRotate.size(); i++) {
        rotated[i] = rotateCoordinate(coordinatesToRotate[i], centerOfRotation, anglesToRotate_CCW);
    }
    return rotated;
}

/**
 * @brief Calculates the intersection point of two 2D lines.
 * 
 * This function determines the intersection point of two lines in a 2D plane.
 * It supports both limited and unlimited lines, and can handle precision and
 * tolerance adjustments for the calculations.
 * 
 * @tparam _varType The data type of the coordinates (e.g., float, double).
 * 
 * @param lineA_0 The starting point of line A.
 * @param lineA_1 The ending point of line A.
 * @param lineB_0 The starting point of line B.
 * @param lineB_1 The ending point of line B.
 * @param limited_lines If true, the function ensures the intersection point lies
 *                      within the bounds of the given line segments. Default is false.
 * @param precision The number of decimal places to round the intersection point to.
 *                  Default is 3.
 * @param tolerance The tolerance value for boundary checks when limited_lines is true.
 *                  Default is 0.0001.
 * 
 * @return pos2d<_varType> The intersection point of the two lines. If the lines
 *                         are parallel or do not intersect, the function returns
 *                         a default value of (-1, -1).
 * 
 * @throws std::invalid_argument If the start and end points of either line are the same.
 * @throws std::runtime_error If limited_lines is true and the intersection point
 *                            lies outside the bounds of the given line segments.
 */
template<typename _varType>
inline pos2d<_varType> getLineIntersect_2D(
    pos2d<_varType> lineA_0,
    pos2d<_varType> lineA_1,
    pos2d<_varType> lineB_0,
    pos2d<_varType> lineB_1,
    bool            limited_lines = false,
    int             precision = 3,
    double          tolerance = 0.0001
) {
    if(lineA_0==lineA_1) throw std::invalid_argument("lineA end point coordinates can't be the same.");
    if(lineB_0==lineB_1) throw std::invalid_argument("lineA end point coordinates can't be the same.");
    if(lineA_1.x<lineA_0.x) {
        auto temp   = lineA_0;
        lineA_0     = lineA_1;
        lineA_1     = temp;
    }
    if(lineB_1.x<lineB_0.x) {
        auto temp   = lineB_0;
        lineB_0     = lineB_1;
        lineB_1     = temp;
    }

    pos2d<_varType> deltaA(lineA_1 - lineA_0);
    pos2d<_varType> deltaB(lineB_1 - lineB_0);

    // Absolut 2d coordinate of the center of line A
    pos2d<_varType> centerA(lineA_0.x + deltaA.x/2, lineA_0.y + deltaA.y/2);
    // Absolut 2d coordinate of the center of line B
    pos2d<_varType> centerB(lineB_0.x + deltaB.x/2, lineB_0.y + deltaB.y/2);
    // Delta-distance between center B and center A
    pos2d<_varType> centerDelta(centerB - centerA);
    // Center coordinate position between line A and B
    pos2d<_varType> pivot_point(
        centerA.x + centerDelta.x/2 + 0.7,
        centerA.y + centerDelta.y/2 + 2.3
    ); 
    

    double theta_A  = DEGREES(asin(deltaA.y / deltaA.getHypotenuse()));
    double theta_B  = DEGREES(asin(deltaB.y / deltaB.getHypotenuse()));
    double theta_AB = std::abs(theta_B - theta_A);


    double theta_newDelta = 0;
    

    if(std::abs(theta_A)>45 || std::abs(theta_B)>45) {
        double dir_A    = dir_A / std::abs(dir_A);
        double dir_B    = dir_B / std::abs(dir_B);

        double theta_deltaA = std::abs(theta_A) - 45;
        double theta_deltaB = std::abs(theta_B) - 45;
        
        double dir_new  = 0;
        if(std::abs(theta_A) > std::abs(theta_B)) {
            theta_newDelta = (-1.0)*dir_A*theta_deltaA;
            if(theta_AB <= 90) {} 
            else theta_newDelta += dir_A*90.0;
        }
        else { //std::abs(theta_A) < std::abs(theta_B)
            theta_newDelta = (-1.0)*dir_B*theta_deltaB;
            if(theta_AB <= 90) {}
            else theta_newDelta += dir_B*90.0;
        }

    }

    /// If the lines need to be rotated, rotate them here.
    if(theta_newDelta != 0) {
        lineA_0 = rotateCoordinate(lineA_0, pivot_point, theta_newDelta);
        lineA_1 = rotateCoordinate(lineA_1, pivot_point, theta_newDelta);
        lineB_0 = rotateCoordinate(lineB_0, pivot_point, theta_newDelta);
        lineB_1 = rotateCoordinate(lineB_1, pivot_point, theta_newDelta);
    }

    
    std::vector<_varType> coeffs_A = getLinearCoefficients(lineA_0, lineA_1);
    std::vector<_varType> coeffs_B = getLinearCoefficients(lineB_0, lineB_1);


    if(coeffs_A[1] == coeffs_B[1]) {
        return pos2d<_varType>(-1, -1);
    }

    pos2d<_varType> intersect_pos(0, 0);

    intersect_pos.x = ((coeffs_B[0]-coeffs_A[0])) / (coeffs_A[1]-coeffs_B[1]);
    intersect_pos.y = coeffs_A[1]*intersect_pos.x + coeffs_A[0];


    if(theta_newDelta != 0) {
        intersect_pos = rotateCoordinate(intersect_pos, pivot_point, -theta_newDelta);

        lineA_0 = rotateCoordinate(lineA_0, pivot_point, -theta_newDelta);
        lineA_1 = rotateCoordinate(lineA_1, pivot_point, -theta_newDelta);
        lineB_0 = rotateCoordinate(lineB_0, pivot_point, -theta_newDelta);
        lineB_1 = rotateCoordinate(lineB_1, pivot_point, -theta_newDelta);
    }

    intersect_pos.x = std::roundf(intersect_pos.x*std::pow(10, precision)) / std::pow(10, precision);
    intersect_pos.y = std::roundf(intersect_pos.y*std::pow(10, precision)) / std::pow(10, precision);



    if(limited_lines) {
        std::string exceptStr = std::string("ERROR: found intersection at position: ") + std::string(intersect_pos) + std::string(" but it's outside given line limitations");

        if(intersect_pos.x < min(lineA_0.x, lineA_1.x)-_varType(tolerance)) throw std::runtime_error(exceptStr+" for x axis for line A minimum: "+formatNumber(min(lineA_0.x, lineA_1.x), 5, 2));
        if(intersect_pos.x > max(lineA_0.x, lineA_1.x)+_varType(tolerance)) throw std::runtime_error(exceptStr+" for x axis for line A maximum: "+formatNumber(max(lineA_0.x, lineA_1.x), 5, 2));
        
        if(intersect_pos.x < min(lineB_0.x, lineB_1.x)-_varType(tolerance)) throw std::runtime_error(exceptStr+" for x axis for line B minimum: "+formatNumber(min(lineB_0.x, lineB_1.x), 5, 2));
        if(intersect_pos.x > max(lineB_0.x, lineB_1.x)+_varType(tolerance)) throw std::runtime_error(exceptStr+" for x axis for line B maximum: "+formatNumber(max(lineB_0.x, lineB_1.x), 5, 2));
        
        if(intersect_pos.y < min(lineA_0.y, lineA_1.y)-_varType(tolerance)) throw std::runtime_error(exceptStr+" for y axis for line A minimum: "+formatNumber(min(lineA_0.y, lineA_1.y), 5, 2));
        if(intersect_pos.y > max(lineA_0.y, lineA_1.y)+_varType(tolerance)) throw std::runtime_error(exceptStr+" for y axis for line A maximum: "+formatNumber(max(lineA_0.y, lineA_1.y), 5, 2));
        
        if(intersect_pos.y < min(lineB_0.y, lineB_1.y)-_varType(tolerance)) throw std::runtime_error(exceptStr+" for y axis for line B minimum: "+formatNumber(min(lineB_0.y, lineB_1.y), 5, 2));
        if(intersect_pos.y > max(lineB_0.y, lineB_1.y)+_varType(tolerance)) throw std::runtime_error(exceptStr+" for y axis for line B maximum: "+formatNumber(max(lineB_0.y, lineB_1.y), 5, 2));
        
    }

    return intersect_pos;
}



#endif //HPP__LINE_INTERSECT_2D