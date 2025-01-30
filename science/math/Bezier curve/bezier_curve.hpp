
#pragma once
#ifndef HPP_MATH__BEZIER_CURVE
#define HPP_MATH__BEZIER_CURVE

#include <math.h>
#include <vector>
#include <cassert>
#include <useful.hpp>

/**
 * @brief Get the coefficients for a linear function between two points
 * 
 * @param point_A `pos2d` object of point A
 * @param point_B `pos2d` object of point B
 * @return std::vector<float> of the coefficients. In this case [0] is offset `c`/`a0` and [1] is linear coefficient `k`/`a1`
 */
inline std::vector<float> getCoef_linear(
    pos2d point_A,
    pos2d point_B
) {
    assert(!(point_A.x==point_B.x && point_A.y==point_B.y));
    // assert(point_A.x != point_B.x);

    float k, c;
    pos2d delta = pos2d(point_B.x-point_A.x, point_B.y-point_A.y);
    // if(delta.x==0) {
    //     delta.x+=1; //basic solution to deal with completely vertical linear functions (im lazy)
    // }
    k = delta.y/delta.x;
    c = point_A.y - point_A.x*k;


    return std::vector<float>{c, k};
}


/**
 * @brief Get the `pos2d` object for the point/coordinate where two lines A and B intersect.
 * 
 * @param point_A0 coordinate of point 0 of line A
 * @param point_A1 coordinate of point 1 of line A
 * @param point_B0 coordinate of point 0 of line B
 * @param point_B1 coordiante of point 1 of line B
 * @param _coefs_A pointer to `std::vector<float>` object to store/save/return the coefficients of line A: {`a0`, `a1`}
 * @param _coefs_B pointer to `std::vector<float>` object to store/save/return the coefficients of line B: {`a0`, `a1`}
 * @return pos2d of the intersected points. If no intersect is found then `pos2d(-1, -1)` is returned
 */
inline pos2d getIntersect(
    pos2d point_A0,
    pos2d point_A1,
    pos2d point_B0,
    pos2d point_B1,
    std::vector<float>* _coefs_A = nullptr,
    std::vector<float>* _coefs_B = nullptr
) {
    pos2d intersect_pos(0, 0);

    std::vector<float> coefs_A = getCoef_linear(point_A0, point_A1);
    std::vector<float> coefs_B = getCoef_linear(point_B0, point_B1);

    if(_coefs_A) (*_coefs_A) = coefs_A;
    if(_coefs_B) (*_coefs_B) = coefs_B;


    if(coefs_A[1] == coefs_B[1]) {
        /// Both lines have the same tilt meaning they'll never cross
        return pos2d(-1, -1);
    }

    intersect_pos.x = ((coefs_B[0]-coefs_A[0])) / (coefs_A[1]-coefs_B[1]);
    intersect_pos.y = coefs_A[1]*intersect_pos.x + coefs_A[0];


    return intersect_pos;
}

/**
 * @brief generate a vector of 2d points that form a quadratic bezier curve
 * 
 * @param point_A 2d coordinate of point A
 * @param point_B 2d coordinate of point B
 * @param point_C 2d coordinate of crosssection point
 * @param segNum number of points beside the end points
 * @param _coeffs std::vector of std::vector for polynomial coefficients: In this case it's a linear function so two coefficients a1 and a0 (a1*x^1 + a0*x^0) for each line.
 * coefficient storage is: {`a0`, `a1`} or {`a0*x^0`, `a1*x^1`}
 * @param _coeffs_option option for how to add the n2 polynomial coefficient to container `_coeffs`:
 *  - `"0"` - `overwrite`: clear the dereferenced container and add the new coefficients onto it
 *  - `"1"` - `append`: append the new data onto the existing date from last index
 *  - `"2:i"`- `insert`: insert the new coefficients from index `i` in the vector
 * @return std::vector<pos2d> of the points that form the bezier curve
 */
inline std::vector<pos2d> quadratic_bezier(
    pos2d point_A,
    pos2d point_B,
    pos2d point_C,
    int segNum,
    std::vector<std::vector<float>>* _coeffs = nullptr,
    std::string _coeffs_option = "0",
    std::string *_outStr = nullptr
) {
    if(_outStr) (*_outStr) = "";
    //numLines = segNum-1
    std::vector<pos2d> curvePoints = std::vector<pos2d>(segNum+2, pos2d());

    if(
        (point_A == point_C) ||
        (point_B == point_C) ||
        ((point_A.x == point_C.x && point_B.x == point_C.x) || (point_A.y == point_C.y && point_B.y == point_C.y))
    ) {
        if(_outStr) (*_outStr) += "special case / sameline";
        curvePoints[0] = point_A;
        for(int i=1; i<=segNum; i++) {
            curvePoints[i] = pos2d(
                point_A.x + i*((point_B.x-point_A.x)/segNum),
                point_A.y + i*((point_B.y+point_A.y)/segNum)
            );
        }
        curvePoints[segNum+1] = point_B;
        return curvePoints;
    }

    std::vector<pos2d> AC(1, point_A);
    std::vector<pos2d> CB(1, point_C);

    pos2d delta_AC(point_C.x-point_A.x, point_C.y-point_A.y);
    pos2d delta_CB(point_B.x-point_C.x, point_B.y-point_C.y);


    for(int i=1; i<=segNum; i++) {
        AC.push_back(pos2d(point_A.x+i*(delta_AC.x/(segNum)), point_A.y+i*(delta_AC.y/(segNum))));
        CB.push_back(pos2d(point_C.x+i*(delta_CB.x/(segNum)), point_C.y+i*(delta_CB.y/(segNum))));
    }
    AC.push_back(point_C);
    CB.push_back(point_B);



    static std::vector<std::vector<float>> temp_coeffs;
    if(_coeffs) {
        if(temp_coeffs.size() != segNum+1) {
            temp_coeffs = std::vector<std::vector<float>>(segNum+1, std::vector<float>{2, 0});
        }
        if(_coeffs_option[0]=='0') {
            _coeffs->clear();
        }

    }

    curvePoints[0] = point_A;
    // std::cout << "++++++"<<std::endl;
    for(int i=1; i<=segNum; i++) {
        pos2d _temp;
        try {
            _temp = getIntersect(AC[i], CB[i], AC[i-1], CB[i-1], (_coeffs? &(temp_coeffs[i]) : nullptr), ((_coeffs && i==1? &(temp_coeffs[i-1]) : nullptr)));
        }
        catch(const std::exception& e) {
            std::cout << e.what() << " || ";
            _temp = curvePoints[i-1];
        }
        if(isnan(_temp.x) || isnan(_temp.y)) _temp = curvePoints[i-1];
        
        curvePoints[i] = _temp;
        // std::cout << "AC["<<i<<"]:"<<AC[i].getStr() << " CB["<<i<<"]:"<< CB[i].getStr() << " | AC["<<i-1<<"]:"<< AC[i-1].getStr() << " CB["<<i-1<<"]:" << CB[i-1].getStr() << std::endl;
    }
    curvePoints[segNum+1] = point_B;


    if(_coeffs) {
        switch (_coeffs_option[0]) {
        case '0': {
            (*_coeffs) = temp_coeffs;
        }   break;
        case '1': {
            _coeffs->insert(_coeffs->end(), temp_coeffs.begin(), temp_coeffs.end());
        }   break;
        case '2': {
            assert(_coeffs_option.length()>2);
            auto itr = _coeffs->begin();
            advance(itr, std::stoi(_coeffs_option.substr(2)));
            _coeffs->insert(itr, temp_coeffs.begin(), temp_coeffs.end());
        }   break;
        default:
            break;
        }
    }

    return curvePoints;
}


#endif