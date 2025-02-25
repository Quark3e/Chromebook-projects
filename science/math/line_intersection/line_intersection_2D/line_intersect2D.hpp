
#pragma once
#ifndef HPP__LINE_INTERSECT_2D

#include <cmath>
#include <vector>
// #include <useful.hpp>
#include <pos2d.hpp>


#ifndef RADIANS
#define RADIANS(deg) (deg*M_PI)/360.0
#endif //RADIANS

#ifndef DEGREES
#define DEGREES(rad) (rad*360.0)/M_PI
#endif //DEGREES



template<typename _varType>
inline std::vector<_varType> getLinearCoefficients(
    pos2d<_varType> point_A,
    pos2d<_varType> point_B
) {
    if(point_A==point_B) throw std::invalid_argument("both points can't be the same");

    pos2d<_varType> delta(point_B-point_A);
    if(delta.x==0) throw std::invalid_argument("line isn't allowed to be completely vertical");

    _varType a_1 = delta.y / delta.x;
    _varType a_0 = point_A.y - point_A.x * a_1;

    return std::vector<_varType>{a_0, a_1};
}

template<typename _varType>
inline double getTheta(
    pos2d<_varType> pos__line_end,
    pos2d<_varType> pos__line_pivot
) {
    if(pos__line_end==pos__line_pivot) throw std::invalid_argument("pos__line_end and pos__line_pivot can't be the same.");
    pos2d<_varType> delta(pos__line_end - pos__line_pivot);
    
    pos2d<_varType> magnitude = delta.modify([](_varType _var) { return (_var>0? _var : _var*(-1)); });
    // pos2d<_varType> magnitude = delta.abs();

    pos2d<int> unitVec(0, 0);
    for(size_t i=0; i<2; i++) {
        if(magnitude[i]!=0) unitVec[i] = magnitude[i];
    }

    _varType    radius  = delta.hypotenuse();
    double      theta   = DEGREES(acos(delta.x / radius));
    if(unitVec.y < 0) theta = 360 - theta;

    return theta;
}

template<typename _varType>
inline pos2d<_varType> rotateCoordinate(
    pos2d<_varType> toRotate,
    pos2d<_varType> centerOfRotation,
    double          anglesToRotate_CCW
) {
    if(toRotate==centerOfRotation) throw std::invalid_argument("toRotate and centerOfRotation argument coordinates can't be the same.");
    pos2d<_varType> delta(toRotate - centerOfRotation);

    pos2d<_varType> magnitude = delta.modify([](_varType _var) { return (_var>0? _var : _var*(-1)); });
    // pos2d<_varType> magnitude = delta.abs();

    pos2d<int> unitVec(0, 0);
    for(size_t i=0; i<2; i++) {
        if(magnitude[i]!=0) unitVec[i] = magnitude[i];
    }
    
    _varType    radius  = delta.hypotenuse();
    double      theta   = DEGREES(acos(delta.x / radius));
    if(unitVec.y < 0) theta = 360 - theta;

    return centerOfRotation + pos2d<_varType>(
        radius * cos(RADIANS(theta + anglesToRotate_CCW)),
        radius * sin(RADIANS(theta + anglesToRotate_CCW))
    );
}

template<typename _varType>
inline std::vector<pos2d<_varType>> rotateCoordinates(
    std::vector<pos2d<_varType>> coordinatesToRotate,
    pos2d<_varType>              centerOfRotation,
    double                       anglesToRotate_CCW
) {
    std::vector<pos2d<_varType>> rotated(coordinatesToRotate.size());
    for(size_t i=0; i<coordinatesToRotate.size(); i++) {
        rotated[i] = rotateCoordinate(coordinatesToRotate[i], centerOfRotation, anglesToRotate_CCW);
    }
    return rotated;
}


template<typename _varType>
inline pos2d<_varType> getLineIntersect_2D(
    pos2d<_varType> lineA_0,
    pos2d<_varType> lineA_1,
    pos2d<_varType> lineB_0,
    pos2d<_varType> lineB_1,
    bool limited_lines = false
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
    pos2d<_varType> pivot_point = (
        centerA.x + centerDelta.x/2,
        centerA.y + centerDelta.y/2
    ); 
    

    // double theta_A = getTheta(lineA_1, lineA_0);
    // double theta_B = getTheta(lineB_1, lineB_0);
    double theta_A  = DEGREES(asin(deltaA.y / deltaA.hypotenuse()));
    double theta_B  = DEGREES(asin(deltaB.y / deltaB.hypotenuse()));
    double theta_AB = abs(theta_B - theta_A);

    // double theta_newAngle = 0;

    double theta_newDelta = 0;
    
    if(abs(theta_A)>45 || abs(theta_B)>45) {
        double dir_A    = dir_A / abs(dir_A);
        double dir_B    = dir_B / abs(dir_B);

        double theta_deltaA = abs(theta_A) - 45;
        double theta_deltaB = abs(theta_B) - 45;
        
        double dir_new  = 0;
        if(abs(theta_A) > abs(theta_B)) {
            theta_newDelta = (-1.0)*dir_A*theta_deltaA;
            if(theta_AB <= 90) {} 
            else theta_newDelta += dir_A*90.0;
        }
        else { //abs(theta_A) < abs(theta_B)
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
        intersect_pos = rotateCoordinate(intersect_pos, pivot_point, 0.0-theta_newDelta);
    }

    if(limited_lines) {
        if(
            intersect_pos.x < min(lineA_0.x, lineA_1.x) || intersect_pos.x > max(lineA_0.x, lineA_1.x) || 
            intersect_pos.x < min(lineB_0.x, lineB_1.x) || intersect_pos.x > max(lineB_0.x, lineB_1.x) || 
            intersect_pos.y < min(lineA_0.y, lineA_1.y) || intersect_pos.y > max(lineA_0.y, lineA_1.y) || 
            intersect_pos.y < min(lineB_0.y, lineB_1.y) || intersect_pos.y > max(lineB_0.y, lineB_1.y)
        ) {
            throw std::runtime_error("found intersection position is outside given line limitations whilst ´limited_lines=true`");
        }
    }

    return intersect_pos;
}



#endif //HPP__LINE_INTERSECT_2D