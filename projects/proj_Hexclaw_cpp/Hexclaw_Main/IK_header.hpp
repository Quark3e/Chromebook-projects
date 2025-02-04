
#pragma once
#ifndef HPP__HEXCLAW_IK_HEADER
#define HPP__HEXCLAW_IK_HEADER

// #include <unistd.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <math.h>
#include <cmath>

#include <diy_dictionary.hpp>

#include "include/motion_control/motion_profiles.hpp"
// #include "global_variables.hpp"


#ifndef _VAR__pos3d
#define _VAR__pos3d
template<class _varType>
struct pos3d {
    _varType x;
    _varType y;
    _varType z;
    pos3d() {}
    pos3d(const pos3d& _copy) {
        this->x = _copy.x;
        this->y = _copy.y;
        this->z = _copy.z;
    }
    pos3d(_varType _x, _varType _y, _varType _z): x(_x), y(_y), z(_z) {}
    _varType &operator[](size_t _i) {
        switch (_i) {
            case 0: return this->x;
            case 1: return this->y;
            case 2: return this->z;
        }
        throw std::out_of_range("size_t index value is out of range.");
        return x;
    }
    _varType operator[](size_t _i) const {
        switch (_i) {
            case 0: return this->x;
            case 1: return this->y;
            case 2: return this->z;
        }
        throw std::out_of_range("size_t index value is out of range.");
        return x;
    }

    pos3d &operator=(const pos3d<_varType>& m) {
        this->x = m.x;
        this->y = m.y;
        this->z = m.z;
        return *this;
    }
    bool operator==(pos3d const& m) { return  (x==m.x && y==m.y && z==m.z); }
    bool operator!=(pos3d const& m) { return !(x==m.x && y==m.y && z==m.z); }

    int _printPrecision = 2;
    int _printWidth     = 0;
    friend auto operator<<(std::ostream &os, pos3d const& m) -> std::ostream& {
        os << std::setw(m._printWidth) << std::setprecision(m._printPrecision);
        os << "("<<m.x<<","<<m.y<<","<<m.z<<")";
        return os;
    }
};
#endif

// #ifndef _VAR__pos2d
// #define _VAR__pos2d

// template<typename _varType>
// struct pos2d {
//     _varType x;
//     _varType y;
//     pos2d() {}
//     // pos2d(_varType *_arrPtr): x(_arrPtr[0]), y(_arrPtr[1]) {}
//     pos2d(const pos2d &_copy) {
//         this->x = _copy.x;
//         this->y = _copy.y;
//     }
//     pos2d(_varType _x, _varType _y): x(_x), y(_y) {}
//     _varType &operator[](size_t _i) {
//         switch (_i) {
//             case 0: return this->x;
//             case 1: return this->y;
//         }
//         throw std::out_of_range("size_t index value is out of range.");
//         return x;
//     }
//     _varType operator[](size_t _i) const {
//         switch (_i) {
//             case 0: return this->x;
//             case 1: return this->y;
//         }
//         throw std::out_of_range("size_t index value is out of range.");
//         return x;
//     }

//     pos2d &operator=(const pos2d<_varType>& m) {
//         this->x = m.x;
//         this->y = m.y;
//         return *this;
//     }
//     bool operator==(pos2d const& m) { return  (x==m.x && y==m.y); }
//     bool operator!=(pos2d const& m) { return !(x==m.x && y==m.y); }

//     int _printPrecision = 2;
//     int _printWidth     = 0;
//     friend auto operator<<(std::ostream &os, pos2d const& m) -> std::ostream& {
//         os << std::setw(m._printWidth) << std::setprecision(m._printPrecision);
//         os << "("<<m.x<<","<<m.y<<")";
//         return os;
//     }
// };

// #endif

namespace HW_KINEMATICS  {

    

    /**
     * settings:
     *  "a1:frameX"   : default: true   desc: getAngles: "frame1X = frame1X * cos(b)"
     *  "a1:a1"       : default: true   desc: getAngles: "a1 = a1 * cos(b)"
     *  "q4:default"  : default: false  desc: getAngles: "q4 = atan(frame1X / frame1Y)"
     *  "q4:a1"       : default: true   desc: getAngles: "q4 = a1"
     * "q4:a1:b:minus": default: true   desc: getAngles: "if(b<0) q4 = 0-a1; else q4=a1"
     * "q4:a1:b1:minus" default: true   desc: getAngles: "if(b1<0) q4 = 0-a1; else q4=a1"
     *  "q5:inPaper"  : default: false  desc: getAngles: "q5 = atan( [...] / (cos(b1) * cos(a1) ))"
     *  "q5:d5"       : default: false  desc: getAngles: "q5 = atan( [...] / (frame1X / tan(a1) ))"
     *  "exceedState" : default: true   desc: getAnlges: "if [...]_exceeded: positionIsReachable[0] = False"
     */
    inline const std::vector<std::string> setting_labels{
        "al:frameX",
        "a1:a1",
        "q4:default",
        "q4:a1",
        "q4:a1:b:minus",
        "q4:a1:b1:minus",
        "q5:inPaper",
        "q5:d5",
        "exceedState"
    };

    inline DIY::typed_dict<std::string, bool> settings(
        setting_labels,
        std::list<bool>{
            true,
            true,
            false,
            true,
            true,
            true,
            false,
            false,
            true
        }
    );

    inline DIY::typed_dict<std::string, bool> setting_default(
        setting_labels,
        settings.values()
    );
    
    // inline DIY::typed_dict<std::string, DIY::typed_dict<std::string

    inline const DIY::typed_dict<std::string, std::string> setting_desc(
        setting_labels,
        std::list<std::string>{
            "frame1X = frame1X * cos(b)",
            "a1 = a1 * cos(b)",
            "q4 = atan(frame1X / frame1Y)",
            "q4 = a1",
            "if(b<0) q4 = 0-a1; else q4=a1",
            "if(b1<0) q4 = 0-a1; else q4=a1",
            "q5 = atan( [...] / (cos(b1) * cos(a1) ))",
            "q5 = atan( [...] / (frame1X / tan(a1) ))",
            "if [...]_exceeded: positionIsReachable[0] = False"
        }
    );


    /**Length of the six arms. unit: [mm]*/
    inline float arm_link[6] = {145, 130, 75, 50, 25, 25};

    /**Weight of the load the motor at index [] is carrying. unit: [kg]*/
    inline float sLoadWeight[6] = {0, 0.130, 0.085, 0.051, 0.03, 0.01};

    /**Namespace scope array of values to store newly solved angles by getAngles()*/
    inline float solved_q[6] = {0, 0, 0, 0, 0, 0};


    inline float _servo_lim[2] = {0, 180};
    


    inline float toRadians(float degrees) { return (degrees*M_PI)/180; }
    inline float toDegrees(float radians) { return (radians*180)/M_PI; }


    // Inverse Kinematics - specific functions:
    /// @brief Calculate Inverse Kinematic angles from given PP and orientation
    /// @param q 6 element float() array linked via pointers to original source
    /// @param PP XYZ 3 element float() array
    /// @param a orient[0] [radians]
    /// @param b orient[1] [radians]
    /// @param Y orient[2] [radians]
    /// @param mode 0-q return in radians; 1-q return in degrees
    /// @param posOption "+" positive IK-setup; "-" negative IK_setup
    /// @param length_scalar scaling variable for each robot arm/length
    /// @param coord_scalar scaling variable for each coordinate axis in given PP
    /// @param printText whether to print non-essential-info
    /// @param printErrors whether to print essential-info/error-info
    /// @return returns true if position is reachable with given setups; false otherwise
    inline bool getAngles(
        float q[6],
        float PP[3],
        float a, float b, float Y,
        int mode = 0,
        char posOption = '-',
        float length_scalar = 1, float coord_scalar = 1,
        bool printText = false, bool printErrors = false
    ) {
        bool isReachable = true;
        float a1_exceed = 0, b1_exceed = 0;
        float frame1X=0, frame1Y=0, frame1Z = 0;

        float P5[3];
        for(int i=0; i<6; i++) {
            if(i<3) PP[i]=PP[i]*coord_scalar;
            arm_link[i]=arm_link[i]*length_scalar;
        }
        float l = (arm_link[4]+arm_link[5]) * cos(b);
        P5[0] = PP[0] - l * sin(a);
        P5[1] = PP[1] - l * cos(a);
        P5[2] = PP[2] - (arm_link[4]+arm_link[5]) * sin(b);

        if(printText) printf(" P5 coords: %d %d %d\n", int(round(P5[0])), int(round(P5[1])), int(round(P5[2])));

        if(P5[1]<0) P5[1] = 0;
        else if(P5[1]==0) {
            if(P5[0]>0) q[0] = toRadians(90);
            else if(P5[0] < 0) q[0] = toRadians(-90);
            else if(P5[0] == 0) q[0] = toRadians(0);
        }
        else q[0] = atan(-P5[0]/P5[1]);
        a = 0-a;

        if(posOption=='+') q[2] = acos((pow(P5[0], 2) + pow(P5[1], 2) + pow(P5[2] - arm_link[0], 2) - pow(arm_link[1], 2) - pow(arm_link[2] + arm_link[3], 2)) /(2 * arm_link[1] * (arm_link[2] + arm_link[3])));
        else if(posOption=='-') q[2] = acos((pow(P5[0], 2) + pow(P5[1], 2) + pow(P5[2] - arm_link[0], 2) - pow(arm_link[1], 2) - pow(arm_link[2] + arm_link[3], 2)) /(2 * arm_link[1] * (arm_link[2] + arm_link[3])));
        if(isnan(q[2])) isReachable = false;


        float lambdaVar=0, muVar=0;

        if(!sqrt(pow(P5[0], 2) + pow(P5[1], 2))==0) lambdaVar = atan((P5[2]-arm_link[0]) / sqrt(pow(P5[0], 2) + pow(P5[1], 2)));
        else isReachable = false;
        muVar = atan(((arm_link[2] + arm_link[3]) * sin(q[2])) /(arm_link[1] + (arm_link[2] + arm_link[3]) * cos(q[2])));

        if(printText) printf("lambda:%d mu:%d\n", int(round(toDegrees(lambdaVar))), int(round(toDegrees(muVar))));
        if(posOption=='+') q[1] = lambdaVar - muVar;
        else if(posOption=='-') {
            if(lambdaVar+muVar>0) q[1] = lambdaVar+muVar;
            else {
                if(printErrors) printf("q[1] error occured\n");
                isReachable = false;
            }
            q[2] = 0 - q[2];
        }

        float a1 = a - q[0];
        float b1 = b - (q[1] + q[2]);
        a1 = a1*cos(b); //debug: True
        
        if(true) {a1 = a1*cos(b);}

        if(printText) printf("a1:%d b1:%d\n",int(toDegrees(a1)),int(toDegrees(b1)));
        
        if (toDegrees(a1)>90) a1_exceed = 1;
        else if(toDegrees(a1)<-90) a1_exceed = -1;
        if(toDegrees(b1)>90) b1_exceed = 1;
        else if(toDegrees(b1)<-90) b1_exceed = -1;
        if(b1_exceed!=0 || a1_exceed!=0) {
            if(printErrors) {
                if(a1_exceed!=0) printf(" a1 exceeded beyond %f", a1_exceed*90);
                if(b1_exceed!=0) printf(" b1 exceeded beyond %f", b1_exceed*90);
                printf("\n");
            }
            isReachable = false;
        }

        frame1X = (arm_link[4]+arm_link[5])*cos(b1)*sin(a1);
        frame1X*=cos(b); //debug: True
        //frame1X = frame1X*cos(b);
        frame1Y = (arm_link[4]+arm_link[5])*cos(b1)*cos(a1);
        frame1Z = (arm_link[4]+arm_link[5])*sin(b1);
        if(printText) printf(" frame1_x:%d frame1_y:%d frame1_z:%d\n",int(frame1X),int(frame1Y),int(frame1Z));

        if(b1==0) {
            if(frame1X>0) q[3]=toRadians(90);
            else if(frame1X<0) q[3]=toRadians(-90);
            else if(frame1X==0) q[3]=toRadians(0);
            if(printText) printf("b1=0  =>  q4 was adjusted\n");
        }
        else if(b1<0 || b1>0) q[3]=atan(frame1X/frame1Z);

        if(b1>0) q[3]=0-a1;
        else q[3]=a1;

        if(b<0) q[3] = 0-a1; //debug: True
        else q[3] = a1; //debug: True
        if(b1<0) q[3] = 0-a1; //debug: True
        else q[3] = a1; //debug: True

        if(isnan(asin(sqrt(pow(frame1X, 2) + pow(frame1Z, 2)) / (arm_link[4]+arm_link[5])))) {
            if(printErrors) printf("q5 error: can't solve it\n");
            isReachable = false;
        }
        else q[4]=asin(sqrt(pow(frame1X, 2) + pow(frame1Z, 2)) / (arm_link[4]+arm_link[5]));

        if(true) {
            q[4]=asin(sqrt(pow(frame1X,2) + pow(frame1Z,2)) / (arm_link[4]+arm_link[5]));
            if(frame1Z<0) {
                q[4]=0-q[4];
                if(printText) printf("frame1_z<0  =>  q5=0-q5\n");
            }
            if(b<=M_PI/2 && b>=0-M_PI/2) q[5]=Y-q[3];
            else if(b>=M_PI/2 || b<=0-M_PI/2) q[5]=M_PI-(Y-q[3]);
        }
        for(int i=0; i<6; i++) {
            if(printText) printf(" %d",int(round(toDegrees(q[i]))));
            if(isnan(q[i])) {
                isReachable = false;
                if(printErrors) printf(" q[%d] is Not a Number\n",i);;
            }	
        }

        if(mode==0) {}
        else if(mode==1) {
            for(int i=0; i<6; i++) {
                q[i] = toDegrees(q[i]);
                solved_q[i] = q[i];
            }
        }	
        

        return isReachable;
    }

    /// @brief find a valid yaw/pitch orient for given 3-axis coordinate values
    /// @param PP 3-axis coordinate values. {`x`, `y`, `z`}
    /// @param currOrient current tilt orient
    /// @param retOrient pointer to array container for result orients.
    /// @param qAngles servo motor angles to find valid orient for
    /// @return whether the function managed to find a valid orient
    inline bool findValidOrient(float PP[3], float currOrient[3], float retOrient[2], float qAngles[6]) {
        int B_dir = 0;
        int orientAcc[2] = {10, 10};

        retOrient[0] = currOrient[0];
        retOrient[1] = currOrient[1];

        if(currOrient[1]<=0) B_dir = -1;
        else B_dir = 1;
        for(int alpha=0; alpha<=180; alpha+=orientAcc[0]) {
            for(int beta=0; beta<=180; beta+=orientAcc[1]) {
                
                // ???
                // if((currOrient[0]+alpha) < getAngles(qAngles, PP, toRadians(currOrient[0]+alpha),toRadians(currOrient[1]+beta*B_dir),toRadians(currOrient[3]), 1)) {
                if(getAngles(qAngles, PP, toRadians(currOrient[0]+alpha),toRadians(currOrient[1]+beta*B_dir),toRadians(currOrient[3]), 1)) {
                    retOrient[0] = currOrient[0] + alpha;
                    retOrient[1] = currOrient[1] + beta*B_dir;
                    return true;
                }
                if(getAngles(qAngles, PP, toRadians(currOrient[0]-alpha),toRadians(currOrient[1]+beta*B_dir),toRadians(currOrient[3]), 1)) {
                    retOrient[0] = currOrient[0] - alpha;
                    retOrient[1] = currOrient[1] + beta*B_dir;
                    return true;
                }
            }
            for(int beta=0; beta<=180; beta+=orientAcc[1]) {
                if(getAngles(qAngles, PP, toRadians(currOrient[0]+alpha),toRadians(currOrient[1]+beta*(-B_dir)),toRadians(currOrient[3]), 1)) {
                    retOrient[0] = currOrient[0] + alpha;
                    retOrient[1] = currOrient[1] + beta*(-B_dir);
                    return true;
                }
                if(getAngles(qAngles, PP, toRadians(currOrient[0]-alpha),toRadians(currOrient[1]+beta*(-B_dir)),toRadians(currOrient[3]), 1)) {
                    retOrient[0] = currOrient[0] - alpha;
                    retOrient[1] = currOrient[1] + beta*(-B_dir);
                    return true;
                }
            }
        }
        return false;
    }

}


#endif