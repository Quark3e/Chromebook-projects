#pragma once
#ifndef H_TWO_CAM_COORDINATE
#define H_TWO_CAM_COORDINATE


#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

#include <pos2d.hpp>


#ifndef M_PI
#define M_PI  3.1415926535
#endif

#ifndef toDEGREES
#define toDEGREES(RAD) ((RAD)*180/M_PI)
#endif
#ifndef toRADIANS
#define toRADIANS(DEG) ((DEG)*M_PI/180.0)
#endif

#ifndef TWOCAMCOORDINATE_DEFAULT__RES
#define TWOCAMCOORDINATE_DEFAULT__RES (pos2d<double>(640, 480))
#endif

#ifndef TWOCAMCOORDINATE_DEFAULT__FOV
#define TWOCAMCOORDINATE_DEFAULT__FOV (pos2d<double>(60, 45))
#endif

/// @brief Class to solve object position relative to the position of two webcams via seen pixel positions
class camTriangle {
    public:
    std::vector<pos2d<double>> camPos{pos2d<double>(0, 0), pos2d<double>(0, 0)};
    std::vector<pos2d<double>> camRes{TWOCAMCOORDINATE_DEFAULT__RES, TWOCAMCOORDINATE_DEFAULT__RES};
    std::vector<pos2d<double>> camFOV{TWOCAMCOORDINATE_DEFAULT__FOV, TWOCAMCOORDINATE_DEFAULT__FOV};
    std::vector<pos2d<double>> camScalar{pos2d<double>(1, 1), pos2d<double>(1, 1)};

    pos2d<double> pos_delta{-1, -1};
    // double l_hypotenuse = -1;
    std::vector<double> l_tri{-1, -1};
    
    std::vector<double> ang_offset{0, 0};
    std::vector<double> ang_d{0, 0};

    std::vector<double> ang_tri{-1, -1, -1};
    // double ang_p = 0;

    std::vector<pos2d<double>> read_pix{pos2d<double>(0, 0), pos2d<double>(0, 0)};
    std::vector<double> ang_read{0, 0, 0};

    std::vector<double> solvedPos{0, 0, 0};
    std::vector<double> solvedPos_filter{1, 1, 1};
    

    camTriangle() = default;
    camTriangle(const camTriangle& _copy);
    camTriangle(std::vector<pos2d<double>> camLocation, std::vector<double> camAngleOffset);
    ~camTriangle();

    camTriangle &operator=(const camTriangle& _copy);

    void setcamScalars();
    void solvePos(std::vector<pos2d<double>> rawPxPos, bool printText=false);
    void setFilter(double x_filter, double y_filter);

    int _printPrecision = 2;
    int _printWidth     = 0;
    friend auto operator<<(std::ostream &os, camTriangle const& m) -> std::ostream&;
};



#endif //H_TWO_CAM_COORDINATE
