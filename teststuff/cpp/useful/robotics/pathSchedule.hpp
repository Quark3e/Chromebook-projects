
#pragma once
#ifndef HPP_PATH_SCHEDULE
#define HPP_PATH_SCHEDULE

#include <iostream>
#include <string>
#include <math.h>
#include <vector>

#include <HC_useful/useful.hpp>
#include <HC_useful/search_multithread.hpp>


#define NULL_POS_POINT 42069


namespace IK_PATH {

    struct point_coord {
        float x = NULL_POS_POINT;
        float y = NULL_POS_POINT;
        float z = NULL_POS_POINT;
    };
    struct point_orient {
        float a = NULL_POS_POINT;
        float B = NULL_POS_POINT;
        float Y = NULL_POS_POINT;
    };

    /**
     * Struct for the position and orientation info for a single point.
     * @note does not contain info relative to other instances. Just instantanious info
     */
    struct pos_point {
        float x = NULL_POS_POINT; // x coordinate
        float y = NULL_POS_POINT; // y coordinate
        float z = NULL_POS_POINT; // z coordinate
        float a = NULL_POS_POINT; // a orient (alpha tilt [yaw])
        float B = NULL_POS_POINT; // B orient (beta tilt  [pitch])
        float Y = NULL_POS_POINT; // Y orient [gamma tilt [roll]]
    };


    class Pos_schedule {
        private:
        std::string _info_name = "Pos_schedule";

        // std::vector<
        std::vector<pos_point>      _points;


        void _call_error(int code, std::string from_member="", std::string custom_error="");

        void _fill_pos_point(pos_point& _toCheck, int idx=-1);
        public:
        size_t size() { return _points.size(); }
        pos_point operator[](size_t i);
        pos_point& operator[](size_t i) const;

        Pos_schedule(/* args */);
        ~Pos_schedule();

        const float x(size_t idx);
        const float y(size_t idx);
        const float z(size_t idx);
        const float a(size_t idx);
        const float B(size_t idx);
        const float Y(size_t idx);


        void add(pos_point pos);
        void add(float x, float y, float z, float a, float B, float Y);
        void add_pos(float x, float y, float z);
        void add_tilt(float a, float B, float Y);

        void insert(size_t idx, pos_point pos);
        void insert(size_t idx, float x, float y, float z, float a, float B, float Y);
        void insert_pos(size_t idx, float x, float y, float z);
        void insert_tilt(size_t idx, float a, float B, float Y);

        void set(size_t idx, pos_point pos);
        void set(size_t idx, float x, float y, float z, float a, float B, float Y);
        void set_pos(size_t idx, float x, float y, float z);
        void set_tilt(size_t idx, float a, float B, float Y);

        void erase(size_t idx);

    };


    class GCODE_schedule {
        private:
        std::string _info_name = "GCODE_schedule";

        std::vector<std::string> _commands;
        

        int G_unit      = 21;   //Unit: `21`-Metric[mm]; `20`-Imperial[inch]
        int G_plane     = 17;   //Plane selection: `17`-XY; `18`-XZ; `19`-YZ
        int G_posit     = 90;   //Coordinate positioning: `90`-absolute; `91`-relative to previous

        int G_motion    =  0;

        int F_feedrate  =100;  //Speed of movement: `{G_unit}/min`

        const char _valid_letters[10] = {
            'G',
            'M',
            'F',
            'P',
            'S',
            'X',
            'Y',
            'Z',
            'I',
            'J',
        };

        bool _parse_line(std::string line, bool throwError = false);
        public:
        GCODE_schedule(/* args */);
        ~GCODE_schedule();
    
    };

}

bool IK_PATH::GCODE_schedule::_parse_line(std::string line, bool throwError) {
    bool valid_line = false;

    bool valid_arg0 = false;
    bool valid_arg1 = false;

    std::vector<std::string> args = splitString(line, " ");
    std::vector<char> arg_letts;
    for(std::string arg: args) arg_letts.push_back(arg[0]);

    int dwell_ms = -1;

    std::string prev_lett = "";
    int _i = 0;
    for(std::string arg: args) {
        bool breakLoop = false;
        int start_letter = DIY_SEARCH_MULTITHREAD::check_existence<char>(arg[0],this->_valid_letters, 10);
        if(start_letter==-1) continue;
        int arg_value = 0;
        try {
            arg_value = std::stoi(arg.substr(1,arg.length()));
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            if(throwError) exit(1);
            continue;
        }
        
        switch (_valid_letters[start_letter]) {
            case 'G':
                switch (arg_value) {
                    case  0: this->G_motion =  0; break;
                    case  1: this->G_motion =  1; break;
                    case  2: this->G_motion =  2; break;
                    case  3: this->G_motion =  3; break;
                    case 17: this->G_plane  = 17; break;
                    case 18: this->G_plane  = 18; break;
                    case 19: this->G_plane  = 19; break;
                    case 20: this->G_unit   = 20; break;
                    case 21: this->G_unit   = 21; break;
                    case 90: this->G_posit  = 90; break;
                    case 91: this->G_posit  = 91; break;
                    default: break;
                }
            break;
            case 'M': /*haven't started here because I'll likely never use this for this letters intended purposes in the near future*/
            break;
            case 'F': this->F_feedrate = arg_value; break;
            case 'P':
            if(prev_lett=="G04") {
                dwell_ms = arg_value;
            }
            break;

        default: break;
        }
        if(breakLoop) break;


        prev_lett = arg;
        _i++;
    }

    return valid_line;
}


IK_PATH::GCODE_schedule::GCODE_schedule(/* args */) {}
IK_PATH::GCODE_schedule::~GCODE_schedule() {}



IK_PATH::pos_point IK_PATH::Pos_schedule::operator[](size_t i) {
    size_t vecSize = this->_points.size();
    if(vecSize==0) this->_call_error(0, "operator[](size_t)", "no position has been added to object");
    else if(i>=vecSize) this->_call_error(0, "operator[](size_t)");
    return _points.at(i);
}
IK_PATH::pos_point& IK_PATH::Pos_schedule::operator[](size_t i) const {
    size_t vecSize = this->_points.size();
    if(vecSize==0) throw std::runtime_error(_info_name+"::operator[](size_t): no position has been added to object");
    else if(i>=vecSize) throw std::runtime_error(_info_name+"::operator[](size_t): input index bigger than available indices");
    return const_cast<pos_point&>(_points.at(i));
}


IK_PATH::Pos_schedule::Pos_schedule(/* args */) {

}
IK_PATH::Pos_schedule::~Pos_schedule() {
}


void IK_PATH::Pos_schedule::_call_error(int code, std::string from_member, std::string custom_error) {
    if(from_member.length()!=0) from_member = "::"+from_member;
    std::string callStr = "ERROR: "+this->_info_name+from_member+": ";
    if(custom_error.length()!=0) callStr+=custom_error;
    else {
        switch (code) {
        case 0: //key not found
            callStr += " input index outside stored elements: avail. size="+std::to_string(this->_points.size());
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
 * @brief check member variables of `pos_point` object for `NULL_POS_POINT` and replace those variable according to function
 * 
 * @param _toCheck reference to `pos_point` to check through and fill in accordingly
 * @param idx index to use to fill in in "NULL_POS_POINT" values
 */
void IK_PATH::Pos_schedule::_fill_pos_point(pos_point& _toCheck, int idx) {
    size_t vecSize = this->_points.size();
    if(idx>=static_cast<int>(vecSize)) this->_call_error(0, "_fill_pos_point(pos_point&, int)", "input index `idx` is bigger than size of stored elements");


    if(vecSize>0) {
        if(idx==-1) idx = vecSize-1; //use last element
        else {} //use idx element
    }
    else if(vecSize==0) idx=-1; //can't set an element: no reference element -> reference=0


    if(idx<0) {
        if(_toCheck.x==NULL_POS_POINT) _toCheck.x = (idx>-1 ? this->_points[idx].x : 0);
        if(_toCheck.y==NULL_POS_POINT) _toCheck.y = (idx>-1 ? this->_points[idx].y : 0);
        if(_toCheck.z==NULL_POS_POINT) _toCheck.z = (idx>-1 ? this->_points[idx].z : 0);

        if(_toCheck.a==NULL_POS_POINT) _toCheck.a = (idx>-1 ? this->_points[idx].a : 0);
        if(_toCheck.B==NULL_POS_POINT) _toCheck.B = (idx>-1 ? this->_points[idx].B : 0);
        if(_toCheck.Y==NULL_POS_POINT) _toCheck.Y = (idx>-1 ? this->_points[idx].Y : 0);
    }
    
}


const float IK_PATH::Pos_schedule::x(size_t idx) {
    if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
    return this->_points.at(idx).x;
}
const float IK_PATH::Pos_schedule::y(size_t idx) {
    if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
    return this->_points.at(idx).y;
}
const float IK_PATH::Pos_schedule::z(size_t idx) {
    if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
    return this->_points.at(idx).z;
}
const float IK_PATH::Pos_schedule::a(size_t idx) {
    if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
    return this->_points.at(idx).a;
}
const float IK_PATH::Pos_schedule::B(size_t idx) {
    if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
    return this->_points.at(idx).B;
}
const float IK_PATH::Pos_schedule::Y(size_t idx) {
    if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
    return this->_points.at(idx).Y;
}


void IK_PATH::Pos_schedule::add(pos_point pos) {
    this->_fill_pos_point(pos);
    this->_points.push_back(pos);
}
void IK_PATH::Pos_schedule::add(float x, float y, float z, float a, float B, float Y) {
    pos_point tempObj{x, y, z, a, B, Y};
    this->_points.push_back(tempObj);
}
void IK_PATH::Pos_schedule::add_pos(float x, float y, float z) {
    pos_point tempObj{x, y, z, NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT};
    this->_fill_pos_point(tempObj);
    this->_points.push_back(tempObj);
}
void IK_PATH::Pos_schedule::add_tilt(float a, float B, float Y) {
    pos_point tempObj{NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT, a, B, Y};
    this->_fill_pos_point(tempObj);
    this->_points.push_back(tempObj);
}


void IK_PATH::Pos_schedule::insert(size_t idx, pos_point pos) {
    if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
    this->_fill_pos_point(pos, idx);
    this->_points.insert(this->_points.begin()+idx, pos);
}
void IK_PATH::Pos_schedule::insert(size_t idx, float x, float y, float z, float a, float B, float Y) {
    if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, float, float, float, float, float, float)");
    pos_point tempObj{x, y, z, a, B, Y};
    this->_points.insert(this->_points.begin()+idx, tempObj);
}
void IK_PATH::Pos_schedule::insert_pos(size_t idx, float x, float y, float z) {
    if(idx>=this->_points.size()) this->_call_error(0, "insert_pos(size_t, float, float, float)");
    pos_point tempObj{x, y, z, NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT};
    this->_fill_pos_point(tempObj, idx);
    this->_points.insert(this->_points.begin()+idx, tempObj);
}
void IK_PATH::Pos_schedule::insert_tilt(size_t idx, float a, float B, float Y) {
    if(idx>=this->_points.size()) this->_call_error(0, "insert_tilt(size_t, float, float, float)");
    pos_point tempObj{NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT, a, B, Y};
    this->_fill_pos_point(tempObj, idx);
    this->_points.insert(this->_points.begin()+idx, tempObj);
}

void IK_PATH::Pos_schedule::set(size_t idx, pos_point pos) {
    if(idx>=this->_points.size()) this->_call_error(0, "set(size_t, pos_point)");
    this->_fill_pos_point(pos, idx);
    this->_points.at(idx) = pos;
}
void IK_PATH::Pos_schedule::set(size_t idx, float x, float y, float z, float a, float B, float Y) {
    if(idx>=this->_points.size()) this->_call_error(0, "set(size_t, float, float, float, float, float, float)");
    pos_point tempObj{x, y, z, a, B, Y};
    this->_points.at(idx) = tempObj;
}
void IK_PATH::Pos_schedule::set_pos(size_t idx, float x, float y, float z) {
    if(idx>=this->_points.size()) this->_call_error(0, "set(size_t, float, float, float)");
    pos_point tempObj{x, y, z, NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT};
    this->_fill_pos_point(tempObj, idx);
    this->_points.at(idx) = tempObj;
}
void IK_PATH::Pos_schedule::set_tilt(size_t idx, float a, float B, float Y) {
    if(idx>=this->_points.size()) this->_call_error(0, "set(size_t, float, float, float)");
    pos_point tempObj{NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT, a, B, Y};
    this->_fill_pos_point(tempObj, idx);
    this->_points.at(idx) = tempObj;
}

void IK_PATH::Pos_schedule::erase(size_t idx) {
    if(idx>=this->_points.size()) this->_call_error(0, "erase(size_t)");
    this->_points.erase(this->_points.begin()+idx);
}



#endif