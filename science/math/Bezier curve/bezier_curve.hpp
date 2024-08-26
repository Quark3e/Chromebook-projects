
#pragma once
#ifndef HPP_MATH__BEZIER_CURVE
#define HPP_MATH__BEZIER_CURVE

#include <math.h>
#include <vector>
#include <cassert>
#include <HC_useful/useful.hpp>


inline std::vector<float> getCoef_linear(
    pos2d point_A,
    pos2d point_B
) {
    float k, c;
    pos2d delta = pos2d(point_B.x-point_A.x, point_B.y-point_A.y);
    if(delta.x<=0.0001) delta.x+=0.001; //basic solution to deal with completely vertical linear functions (im lazy)
    k = delta.y/delta.x;
    c = point_A.y - point_A.x*k;

    return std::vector<float>{k, c};
}

inline pos2d getIntersect(
    pos2d point_A0,
    pos2d point_A1,
    pos2d point_B0,
    pos2d point_B1
) {
    pos2d intersect_pos(0, 0);

    std::vector<float> coefs_A = getCoef_linear(point_A0, point_A1);
    std::vector<float> coefs_B = getCoef_linear(point_B0, point_B1);

    // std::cout << "{["<<point_A0.getStr()<<"]["<<point_A1.getStr()<<"]["<<point_B0.getStr()<<"]["<<point_B1.getStr()<<"] ";
    // std::cout << "(k:"<<coefs_A[0]<<", c"
    // std::cout << "}";


    if(coefs_A[0] == coefs_B[0]) return pos2d(-1, -1);

    intersect_pos.x = ((coefs_B[1]-coefs_A[1])) / (coefs_A[0]-coefs_B[0]);
    intersect_pos.y = coefs_A[0]*intersect_pos.x + coefs_A[1];

    // std::cout << coefs_A[0]<<" "<<coefs_A[1] << " | "<<coefs_B[0]<<" "<<coefs_B[1] << std::endl;
    // assert(!isnan(intersect_pos.x));

    return intersect_pos;
}

/**
 * @brief generate a vector of 2d points that form a quadratic bezier curve
 * 
 * @param point_A 2d coordinate of point A
 * @param point_B 2d coordinate of point B
 * @param point_C 2d coordinate of crosssection point
 * @param segNum number of points beside the end points
 * @return std::vector<pos2d> of the points that form the bezier curve
 */
inline std::vector<pos2d> quadratic_bezier(
    pos2d point_A,
    pos2d point_B,
    pos2d point_C,
    int segNum
) {
    //numLines = segNum-1
    std::vector<pos2d> curvePoints = std::vector<pos2d>(segNum+2, pos2d());
    std::vector<pos2d> AC(1, point_A);
    std::vector<pos2d> CB(1, point_C);

    pos2d delta_AC(point_C.x-point_A.x, point_C.y-point_A.y);
    pos2d delta_CB(point_B.x-point_C.x, point_B.y-point_C.y);

    // std::vector<float> coefs_AC = getCoef_linear(point_A, point_C);
    // std::vector<float> coefs_CB = getCoef_linear(point_C, point_B);


    for(int i=1; i<=segNum; i++) {
        AC.push_back(pos2d(point_A.x+i*(delta_AC.x/(segNum)), point_A.y+i*(delta_AC.y/(segNum))));
        CB.push_back(pos2d(point_C.x+i*(delta_CB.x/(segNum)), point_C.y+i*(delta_CB.y/(segNum))));
    }
    AC.push_back(point_C);
    CB.push_back(point_B);


    curvePoints[0] = point_A;
    for(int i=1; i<=segNum; i++) {
        pos2d temp = getIntersect(AC[i], CB[i], AC[i-1], CB[i-1]);
        curvePoints[i] = temp;
    }
    curvePoints[segNum+1] = point_B;


    return curvePoints;
}


#endif