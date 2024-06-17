
#pragma once
#include <unistd.h>
#include <iostream>
#include <string>
#include <math.h>


#include <HC_useful/diy_dictionary.hpp>

#include "HW_headers/motion_control/motion_profiles.hpp"
// #include "hexclaw_global.hpp"

using namespace std;




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
    const std::vector<std::string> setting_labels{
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

    DIY::typed_dict<std::string, bool> settings(
        setting_labels,
        std::vector<bool>{
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

    DIY::typed_dict<std::string, bool> setting_default(
        setting_labels,
        settings.values()
    );

    const DIY::typed_dict<std::string, std::string> setting_desc(
        setting_labels,
        std::vector<std::string>{
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
    float arm_link[6] = {145, 130, 75, 50, 25, 25};

    /**Weight of the load the motor at index [] is carrying. unit: [kg]*/
    float sLoadWeight[6] = {0, 0.130, 0.085, 0.051, 0.03, 0.01};

    /**Namespace scope array of values to store newly solved angles by HW_KINEMATICS::getAngles()*/
    float solved_q[6] = {0, 0, 0, 0, 0, 0};


    float _servo_lim[2] = {0, 180};
    


    float toRadians(float degrees) { return (degrees*M_PI)/180; }
    float toDegrees(float radians) { return (radians*180)/M_PI; }


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

        if(printText) printf(" P5 coords: %f %f %f\n", int(round(P5[0])), int(round(P5[1])), int(round(P5[2])));

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
                if(a1_exceed!=0) printf(" a1 exceeded beyond %d", a1_exceed*90);
                if(b1_exceed!=0) printf(" b1 exceeded beyond %d", b1_exceed*90);
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
                
                if(
                    (currOrient[0]+alpha) < 
                    getAngles(qAngles, PP, toRadians(currOrient[0]+alpha),toRadians(currOrient[1]+beta*B_dir),toRadians(currOrient[3]), 1)) {
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


#define NULL_POS_INFO 42069

struct pos_info {
    float x=NULL_POS_INFO; // x coordinate
    float y=NULL_POS_INFO; // y coordinate
    float z=NULL_POS_INFO; // z coordinate
    float a=NULL_POS_INFO; // a orient (alpha tilt [yaw])
    float B=NULL_POS_INFO; // B orient (beta tilt  [pitch])
    float Y=NULL_POS_INFO; // Y orient [gamma tilt [roll]]
};


class Pos_schedule {
    private:
    std::string _info_name = "Pos_schedule";
    /* data */
    std::vector<pos_info> _positions;

    void _call_error(int code, std::string from_member="", std::string custom_error="");

    void _fill_pos_info(pos_info& _toCheck, int idx=-1);
    public:
    size_t size() { return _positions.size(); }
    pos_info operator[](size_t i) {
        size_t vecSize = this->_positions.size();
        if(vecSize==0) this->_call_error(0, "operator[](size_t)", "no position has been added to object");
        else if(i>=vecSize) this->_call_error(0, "operator[](size_t)");
        return _positions.at(i);
    }
    pos_info& operator[](size_t i) const {
        size_t vecSize = this->_positions.size();
        if(vecSize==0) throw std::runtime_error(_info_name+"::operator[](size_t): no position has been added to object");
        else if(i>=vecSize) throw std::runtime_error(_info_name+"::operator[](size_t): input index bigger than available indices");
        return const_cast<pos_info&>(_positions.at(i));
    }

    Pos_schedule(/* args */);
    ~Pos_schedule();

    const float x(size_t idx);
    const float y(size_t idx);
    const float z(size_t idx);
    const float a(size_t idx);
    const float B(size_t idx);
    const float Y(size_t idx);

    void add(pos_info pos);
    void add(float x, float y, float z, float a, float B, float Y);
    void add_pos(float x, float y, float z);
    void add_tilt(float a, float B, float Y);

    void insert(size_t idx, pos_info pos);
    void insert(size_t idx, float x, float y, float z, float a, float B, float Y);
    void insert_pos(size_t idx, float x, float y, float z);
    void insert_tilt(size_t idx, float a, float B, float Y);

    void set(size_t idx, pos_info pos);
    void set(size_t idx, float x, float y, float z, float a, float B, float Y);
    void set_pos(size_t idx, float x, float y, float z);
    void set_tilt(size_t idx, float a, float B, float Y);

    void erase(size_t idx);

};

Pos_schedule::Pos_schedule(/* args */) {

}
Pos_schedule::~Pos_schedule() {
}


void Pos_schedule::_call_error(int code, std::string from_member, std::string custom_error) {
    if(from_member.length()!=0) from_member = "::"+from_member;
    std::string callStr = "ERROR: "+this->_info_name+from_member+": ";
    if(custom_error.length()!=0) callStr+=custom_error;
    else {
        switch (code) {
        case 0: //key not found
            callStr += " input index outside stored elements: avail. size="+std::to_string(this->_positions.size());
            break;
        // case 1: //key already exists
        //     callStr += " input index  stored elements";
        //     break;
        default:
            break;
        }
    }
    throw std::runtime_error(callStr);
}

/**
 * @brief check member variables of `pos_info` object for `NULL_POS_INFO` and replace those variable according to function
 * 
 * @param _toCheck reference to `pos_info` to check through and fill in accordingly
 * @param idx index to use to fill in in "NULL_POS_INFO" values
 */
void Pos_schedule::_fill_pos_info(pos_info& _toCheck, int idx) {
    size_t vecSize = this->_positions.size();
    if(idx>=static_cast<int>(vecSize)) this->_call_error(0, "_fill_pos_info(pos_info&, int)", "input index `idx` is bigger than size of stored elements");


    if(vecSize>0) {
        if(idx==-1) idx = vecSize-1; //use last element
        else {} //use idx element
    }
    else if(vecSize==0) idx=-1; //can't set an element: no reference element -> reference=0


    if(idx<0) {
        if(_toCheck.x==NULL_POS_INFO) _toCheck.x = (idx>-1 ? this->_positions[idx].x : 0);
        if(_toCheck.y==NULL_POS_INFO) _toCheck.y = (idx>-1 ? this->_positions[idx].y : 0);
        if(_toCheck.z==NULL_POS_INFO) _toCheck.z = (idx>-1 ? this->_positions[idx].z : 0);

        if(_toCheck.a==NULL_POS_INFO) _toCheck.a = (idx>-1 ? this->_positions[idx].a : 0);
        if(_toCheck.B==NULL_POS_INFO) _toCheck.B = (idx>-1 ? this->_positions[idx].B : 0);
        if(_toCheck.Y==NULL_POS_INFO) _toCheck.Y = (idx>-1 ? this->_positions[idx].Y : 0);
    }
    
}


const float Pos_schedule::x(size_t idx) {
    if(idx>=this->_positions.size()) this->_call_error(0, "insert(size_t, pos_info)");
    return this->_positions.at(idx).x;
}
const float Pos_schedule::y(size_t idx) {
    if(idx>=this->_positions.size()) this->_call_error(0, "insert(size_t, pos_info)");
    return this->_positions.at(idx).y;
}
const float Pos_schedule::z(size_t idx) {
    if(idx>=this->_positions.size()) this->_call_error(0, "insert(size_t, pos_info)");
    return this->_positions.at(idx).z;
}
const float Pos_schedule::a(size_t idx) {
    if(idx>=this->_positions.size()) this->_call_error(0, "insert(size_t, pos_info)");
    return this->_positions.at(idx).a;
}
const float Pos_schedule::B(size_t idx) {
    if(idx>=this->_positions.size()) this->_call_error(0, "insert(size_t, pos_info)");
    return this->_positions.at(idx).B;
}
const float Pos_schedule::Y(size_t idx) {
    if(idx>=this->_positions.size()) this->_call_error(0, "insert(size_t, pos_info)");
    return this->_positions.at(idx).Y;
}

void Pos_schedule::add(pos_info pos) {
    this->_fill_pos_info(pos);
    this->_positions.push_back(pos);
}
void Pos_schedule::add(float x, float y, float z, float a, float B, float Y) {
    pos_info tempObj{x, y, z, a, B, Y};
    this->_positions.push_back(tempObj);
}
void Pos_schedule::add_pos(float x, float y, float z) {
    pos_info tempObj{x, y, z, NULL_POS_INFO, NULL_POS_INFO, NULL_POS_INFO};
    this->_fill_pos_info(tempObj);
    this->_positions.push_back(tempObj);
}
void Pos_schedule::add_tilt(float a, float B, float Y) {
    pos_info tempObj{NULL_POS_INFO, NULL_POS_INFO, NULL_POS_INFO, a, B, Y};
    this->_fill_pos_info(tempObj);
    this->_positions.push_back(tempObj);
}


void Pos_schedule::insert(size_t idx, pos_info pos) {
    if(idx>=this->_positions.size()) this->_call_error(0, "insert(size_t, pos_info)");
    this->_fill_pos_info(pos, idx);
    this->_positions.insert(this->_positions.begin()+idx, pos);
}
void Pos_schedule::insert(size_t idx, float x, float y, float z, float a, float B, float Y) {
    if(idx>=this->_positions.size()) this->_call_error(0, "insert(size_t, float, float, float, float, float, float)");
    pos_info tempObj{x, y, z, a, B, Y};
    this->_positions.insert(this->_positions.begin()+idx, tempObj);
}
void Pos_schedule::insert_pos(size_t idx, float x, float y, float z) {
    if(idx>=this->_positions.size()) this->_call_error(0, "insert_pos(size_t, float, float, float)");
    pos_info tempObj{x, y, z, NULL_POS_INFO, NULL_POS_INFO, NULL_POS_INFO};
    this->_fill_pos_info(tempObj, idx);
    this->_positions.insert(this->_positions.begin()+idx, tempObj);
}
void Pos_schedule::insert_tilt(size_t idx, float a, float B, float Y) {
    if(idx>=this->_positions.size()) this->_call_error(0, "insert_tilt(size_t, float, float, float)");
    pos_info tempObj{NULL_POS_INFO, NULL_POS_INFO, NULL_POS_INFO, a, B, Y};
    this->_fill_pos_info(tempObj, idx);
    this->_positions.insert(this->_positions.begin()+idx, tempObj);
}

void Pos_schedule::set(size_t idx, pos_info pos) {
    if(idx>=this->_positions.size()) this->_call_error(0, "set(size_t, pos_info)");
    this->_fill_pos_info(pos, idx);
    this->_positions.at(idx) = pos;
}
void Pos_schedule::set(size_t idx, float x, float y, float z, float a, float B, float Y) {
    if(idx>=this->_positions.size()) this->_call_error(0, "set(size_t, float, float, float, float, float, float)");
    pos_info tempObj{x, y, z, a, B, Y};
    this->_positions.at(idx) = tempObj;
}
void Pos_schedule::set_pos(size_t idx, float x, float y, float z) {
    if(idx>=this->_positions.size()) this->_call_error(0, "set(size_t, float, float, float)");
    pos_info tempObj{x, y, z, NULL_POS_INFO, NULL_POS_INFO, NULL_POS_INFO};
    this->_fill_pos_info(tempObj, idx);
    this->_positions.at(idx) = tempObj;
}
void Pos_schedule::set_tilt(size_t idx, float a, float B, float Y) {
    if(idx>=this->_positions.size()) this->_call_error(0, "set(size_t, float, float, float)");
    pos_info tempObj{NULL_POS_INFO, NULL_POS_INFO, NULL_POS_INFO, a, B, Y};
    this->_fill_pos_info(tempObj, idx);
    this->_positions.at(idx) = tempObj;
}

void Pos_schedule::erase(size_t idx) {
    if(idx>=this->_positions.size()) this->_call_error(0, "erase(size_t)");
    this->_positions.erase(this->_positions.begin()+idx);
}

