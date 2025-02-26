#pragma once
#ifndef _VAR__pos2d
#define _VAR__pos2d

#include <cmath>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>


template<typename _varType>
struct pos2d {
    _varType x;
    _varType y;
    pos2d() {}
    // pos2d(_varType *_arrPtr): x(_arrPtr[0]), y(_arrPtr[1]) {}
    pos2d(const pos2d &_copy) {
        this->x = _copy.x;
        this->y = _copy.y;
    }
    pos2d(_varType _x, _varType _y): x(_x), y(_y) {}
    _varType &operator[](size_t _i) {
        switch (_i) {
            case 0: return this->x;
            case 1: return this->y;
        }
        throw std::out_of_range("size_t index value is out of range.");
        return x;
    }
    _varType operator[](size_t _i) const {
        switch (_i) {
            case 0: return this->x;
            case 1: return this->y;
        }
        throw std::out_of_range("size_t index value is out of range.");
        return x;
    }

    pos2d &operator=(const pos2d<_varType>& m) {
        this->x = m.x;
        this->y = m.y;
        return *this;
    }
    bool operator<(pos2d const& m)  { return ((x-m.x)+(y-m.y))<0; }
    bool operator>(pos2d const& m)  { return ((x-m.x)+(y-m.y))>0; }
    int equalCompare_round_prec = 6;
    bool operator==(pos2d const& m) {
        return (
            (roundf(m.x*pow(10, equalCompare_round_prec))/pow(10, equalCompare_round_prec)==roundf(this->x*pow(10, equalCompare_round_prec))/pow(10, equalCompare_round_prec)) &&
            (roundf(m.y*pow(10, equalCompare_round_prec))/pow(10, equalCompare_round_prec)==roundf(this->y*pow(10, equalCompare_round_prec))/pow(10, equalCompare_round_prec))
        );
        // return (x==m.x && y==m.y);
    }
    bool operator!=(pos2d const& m) { return !(x==m.x && y==m.y); }

    operator std::string() {
        std::stringstream ss;
        ss << std::setw(_printWidth) << std::setprecision(_printPrecision);
        ss << "("<<x<<","<<y<<")";
        return ss.str();
    }

    /**
     * @brief Check whether a pos2d value is within the region given by min-max corners of a Bounding Box.
     * 
     * @param corner_min corner coordinate with minimum values.
     * @param corner_max corner coordinate with maximum values.
     * @param _includeBB whether to include the Bounding Box borner region (ex: `>=`) or not (`>`)
     * @return true is within region.
     * @return false is not within region.
     */
    bool inRegion(pos2d corner_min, pos2d corner_max, bool _includeBB=true) {
        bool inReg = false;
        if(_includeBB) {
            inReg = ((x>=corner_min.x && y>=corner_min.y) && (x<=corner_max.x && y<=corner_max.y));
        }
        else {
            inReg = ((x> corner_min.x && y> corner_min.y) && (x< corner_max.x && y< corner_max.y));
        }

        return inReg;
    }

    pos2d& operator+=(pos2d const& m) {
        x+=m.x;
        y+=m.y;
        return *this;
    }
    pos2d& operator-=(pos2d const& m) {
        x-=m.x;
        y-=m.y;
        return *this;
    }
    pos2d& operator*=(pos2d const& m) {
        x*=m.x;
        y*=m.y;
        return *this;
    }
    pos2d& operator/=(pos2d const& m) {
        x/=m.x;
        y/=m.y;
        return *this;
    }
    
    pos2d operator+(pos2d const& m) {
        return pos2d(x+m.x, y+m.y);
    }
    pos2d operator-(pos2d const& m) {
        return pos2d(x-m.x, y-m.y);
    }
    pos2d operator*(pos2d const& m) {
        return pos2d(x*m.x, y*m.y);
    }
    pos2d operator/(pos2d const& m) {
        return pos2d(x/m.x, y/m.y);
    }
    

    pos2d rounded(int decimals) {
        return pos2d(
            roundf(this->x*pow(10, decimals)) / pow(10, decimals),
            roundf(this->y*pow(10, decimals)) / pow(10, decimals)
        );
    }
    
    template<typename _lambda>
    pos2d modify(_lambda modificationFunc) {
        return pos2d(modificationFunc(x), modificationFunc(y));
    }
    pos2d abs() {
        return pos2d(
            (x>0? x : x*(-1)),
            (y>0? x : y*(-1))
        );
    }
    _varType hypotenuse() {
        return sqrt(pow(x, 2)+pow(y, 2));
    }

    // template<class _twoSizeContainerType>
    // operator _twoSizeContainerType() {
    //     _twoSizeContainerType _var;
    //     _var[0] = x;
    //     _var[1] = y;
    //     return _var;
    // }


    int _printPrecision = 2;
    int _printWidth     = 0;
    friend auto operator<<(std::ostream &os, pos2d const& m) -> std::ostream& {
        os << std::setw(m._printWidth) << std::setprecision(m._printPrecision);
        os << "("<<m.x<<","<<m.y<<")";
        return os;
    }
};

#endif //_VAR__pos2d