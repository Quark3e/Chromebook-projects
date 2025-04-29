#pragma once
#ifndef HPP_LIB_SYSTEMOFCAMERAS
#define HPP_LIB_SYSTEMOFCAMERAS

#ifndef M_PI
#define M_PI  3.1415926535
#endif


#ifndef toDEGREES
#define toDEGREES(RAD) ((RAD)*180/M_PI)
#endif
#ifndef toRADIANS
#define toRADIANS(DEG) ((DEG)*M_PI/180.0)
#endif


#include <cmath>

#include <string>

#include <iostream>
#include <iomanip>
#include <sstream>

#include <vector>
#include <initializer_list>

#include "pos2d.hpp"


namespace SOC {

    struct CamU {

        double x = 0;
        double y = 0;
        double angle = 90;
        double FOV = 135;

        CamU() = default;
        CamU(const CamU &_copy);
        CamU(double _FOV, double _angle, double _x, double _y);
        ~CamU();
        double &operator[](size_t _i);
        double operator[](size_t _i) const;

        CamU &operator=(const CamU &_m);

        pos2d<double> pos();
        /**
         * @brief Get the Cam Angle to the object from the camera unit's center, with CW being >0, CCW being <0
         * 
         * @param _objPos position of the object to check the angle to.
         * @param _throwExcept_outOfFOV whether to throw an out_of_range exception if abs(solved angle) > FOV/2
         * @return `double` angle from the camera's middle.
         */
        double getCamAngle(pos2d<double> _objPos, bool _throwExcept_outOfFOV=false);

        int _printWidth     = 0;
        int _printPrecision = 2;
        operator std::string() {
            std::stringstream ss;
            ss << std::setw(_printWidth) << std::setprecision(_printPrecision);
            ss << "(" << x << "," << y << "," << angle << ")";
            return ss.str();
        }
        friend auto operator<<(std::ostream &os, CamU const& m) -> std::ostream& {
            os << std::setw(m._printWidth) << std::setprecision(m._printPrecision);
            os << "(" << m.x << "," << m.y << "," << m.angle << ")";
            return os;
        }

    };


    class CamSystem {
        private:

        pos2d<double> __objectPos{0, 0};

        std::vector<CamU>   __system_CamU;
        std::vector<double> __system_CamU_angles;

        public:
        std::string errorMsg = "";

        CamSystem() = default;
        CamSystem(const CamSystem& _copy);
        CamSystem(pos2d<double> _objPos, std::vector<CamU> _CamU_units={});
        ~CamSystem();

        CamSystem &operator=(const CamSystem &_m);
        CamU &operator[](size_t _i);
        CamU operator[](size_t _i) const;

        size_t size() const;

        int set_objectPos(pos2d<double> _newPos);

        double get_angle(size_t _i);
        pos2d<double> get_objectPos();

        CamU &add(pos2d<double> _pos, double _angle, double _FOV);
        int erase(size_t _i);

        int update();

    };
};


#endif //HPP_LIB_SYSTEMOFCAMERAS