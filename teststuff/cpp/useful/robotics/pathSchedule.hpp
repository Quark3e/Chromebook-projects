
#pragma once
#ifndef HPP_PATH_SCHEDULE
#define HPP_PATH_SCHEDULE

#include <iostream>
#include <string>
#include <math.h>
#include <vector>


#define NULL_POS_POINT 42069


namespace IK_PATH {

    /**
     * Struct for the position and orientation info for a single point.
     * @note does not contain info relative to other instances. Just instantanious info
     */
    struct pos_point {
        float x=NULL_POS_POINT; // x coordinate
        float y=NULL_POS_POINT; // y coordinate
        float z=NULL_POS_POINT; // z coordinate
        float a=NULL_POS_POINT; // a orient (alpha tilt [yaw])
        float B=NULL_POS_POINT; // B orient (beta tilt  [pitch])
        float Y=NULL_POS_POINT; // Y orient [gamma tilt [roll]]
        float _delay_ms=0; //milliseconds to wait
    };


    class Pos_schedule {
        private:
        std::string _info_name = "Pos_schedule";

        // std::vector<
        std::vector<pos_point>      _points;



        /* data */
        std::vector<std::string>    _commands;
        
        struct current_pos {
            float x=0;
            float y=0;
            float z=0;
        };
        struct current_tilt {
            float a=0;
            float B=0;
            float Y=0;
        };

        int     G_unit      = 21;   // Unit: 21=Metric(mm); 20=Imperial(in)
        // int     G_motion    = 0;    // Type of motions: 00/0=Rapid; 01/1=Linear Interpolation; 02/2=CW Circular Interpolation; 03/3CCW Circular Interpolation
        int     G_plane     = 17;   // Plane to work on: 17=XY; 18=ZX; 19=YZ;
        float   F_feedRate  = 100;  // Speed of motion: {G_Unit}/min


        void _call_error(int code, std::string from_member="", std::string custom_error="");

        void _fill_pos_point(pos_point& _toCheck, int idx=-1);
        public:
        size_t size() { return _points.size(); }
        pos_point operator[](size_t i) {
            size_t vecSize = this->_points.size();
            if(vecSize==0) this->_call_error(0, "operator[](size_t)", "no position has been added to object");
            else if(i>=vecSize) this->_call_error(0, "operator[](size_t)");
            return _points.at(i);
        }
        pos_point& operator[](size_t i) const {
            size_t vecSize = this->_points.size();
            if(vecSize==0) throw std::runtime_error(_info_name+"::operator[](size_t): no position has been added to object");
            else if(i>=vecSize) throw std::runtime_error(_info_name+"::operator[](size_t): input index bigger than available indices");
            return const_cast<pos_point&>(_points.at(i));
        }

        Pos_schedule(/* args */);
        ~Pos_schedule();

        const float x(size_t idx);
        const float y(size_t idx);
        const float z(size_t idx);
        const float a(size_t idx);
        const float B(size_t idx);
        const float Y(size_t idx);

        int loadFile(std::string filename);

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
    void Pos_schedule::_fill_pos_point(pos_point& _toCheck, int idx) {
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


    const float Pos_schedule::x(size_t idx) {
        if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
        return this->_points.at(idx).x;
    }
    const float Pos_schedule::y(size_t idx) {
        if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
        return this->_points.at(idx).y;
    }
    const float Pos_schedule::z(size_t idx) {
        if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
        return this->_points.at(idx).z;
    }
    const float Pos_schedule::a(size_t idx) {
        if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
        return this->_points.at(idx).a;
    }
    const float Pos_schedule::B(size_t idx) {
        if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
        return this->_points.at(idx).B;
    }
    const float Pos_schedule::Y(size_t idx) {
        if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
        return this->_points.at(idx).Y;
    }

    /**
     * @brief load POS_schedule header with values from dedicated file
     * 
     * @param filename `std::string` of absolute directory to the file
     * @return int value of whether it successfully loaded the contents of the file into the class.
     */
    int Pos_schedule::loadFile(std::string filename) {


        return 0;
    }

    void Pos_schedule::add(pos_point pos) {
        this->_fill_pos_point(pos);
        this->_points.push_back(pos);
    }
    void Pos_schedule::add(float x, float y, float z, float a, float B, float Y) {
        pos_point tempObj{x, y, z, a, B, Y};
        this->_points.push_back(tempObj);
    }
    void Pos_schedule::add_pos(float x, float y, float z) {
        pos_point tempObj{x, y, z, NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT};
        this->_fill_pos_point(tempObj);
        this->_points.push_back(tempObj);
    }
    void Pos_schedule::add_tilt(float a, float B, float Y) {
        pos_point tempObj{NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT, a, B, Y};
        this->_fill_pos_point(tempObj);
        this->_points.push_back(tempObj);
    }


    void Pos_schedule::insert(size_t idx, pos_point pos) {
        if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, pos_point)");
        this->_fill_pos_point(pos, idx);
        this->_points.insert(this->_points.begin()+idx, pos);
    }
    void Pos_schedule::insert(size_t idx, float x, float y, float z, float a, float B, float Y) {
        if(idx>=this->_points.size()) this->_call_error(0, "insert(size_t, float, float, float, float, float, float)");
        pos_point tempObj{x, y, z, a, B, Y};
        this->_points.insert(this->_points.begin()+idx, tempObj);
    }
    void Pos_schedule::insert_pos(size_t idx, float x, float y, float z) {
        if(idx>=this->_points.size()) this->_call_error(0, "insert_pos(size_t, float, float, float)");
        pos_point tempObj{x, y, z, NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT};
        this->_fill_pos_point(tempObj, idx);
        this->_points.insert(this->_points.begin()+idx, tempObj);
    }
    void Pos_schedule::insert_tilt(size_t idx, float a, float B, float Y) {
        if(idx>=this->_points.size()) this->_call_error(0, "insert_tilt(size_t, float, float, float)");
        pos_point tempObj{NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT, a, B, Y};
        this->_fill_pos_point(tempObj, idx);
        this->_points.insert(this->_points.begin()+idx, tempObj);
    }

    void Pos_schedule::set(size_t idx, pos_point pos) {
        if(idx>=this->_points.size()) this->_call_error(0, "set(size_t, pos_point)");
        this->_fill_pos_point(pos, idx);
        this->_points.at(idx) = pos;
    }
    void Pos_schedule::set(size_t idx, float x, float y, float z, float a, float B, float Y) {
        if(idx>=this->_points.size()) this->_call_error(0, "set(size_t, float, float, float, float, float, float)");
        pos_point tempObj{x, y, z, a, B, Y};
        this->_points.at(idx) = tempObj;
    }
    void Pos_schedule::set_pos(size_t idx, float x, float y, float z) {
        if(idx>=this->_points.size()) this->_call_error(0, "set(size_t, float, float, float)");
        pos_point tempObj{x, y, z, NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT};
        this->_fill_pos_point(tempObj, idx);
        this->_points.at(idx) = tempObj;
    }
    void Pos_schedule::set_tilt(size_t idx, float a, float B, float Y) {
        if(idx>=this->_points.size()) this->_call_error(0, "set(size_t, float, float, float)");
        pos_point tempObj{NULL_POS_POINT, NULL_POS_POINT, NULL_POS_POINT, a, B, Y};
        this->_fill_pos_point(tempObj, idx);
        this->_points.at(idx) = tempObj;
    }

    void Pos_schedule::erase(size_t idx) {
        if(idx>=this->_points.size()) this->_call_error(0, "erase(size_t)");
        this->_points.erase(this->_points.begin()+idx);
    }


    class GCODE_schedule {
    private:
        /* data */
    public:
        GCODE_schedule(/* args */);
        ~GCODE_schedule();
    };
    
    GCODE_schedule::GCODE_schedule(/* args */) {
    
    }
    
    GCODE_schedule::~GCODE_schedule() {}
    

}


#endif