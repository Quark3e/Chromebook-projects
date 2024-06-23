
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

    /**
     * Syntax of (some) GCode arguments.
     * Symbol definition
     * `,`: `a,b`    - and/or: either or both values on either side of the symbol.
     * `(`: `(a,b)`   - must: atleast one of the symbols/args inside the parenthesis has to exist.
     * `/`: `a/(b,c)`- either of the symbols must be called but never both.
     *    : `{INT}`  - letter must be accompanied with an integer number/value.
     * `+`: `a+`     - another code can be called after the code with this symbol
     */
    const std::vector<std::vector<std::string>> GCODE_Syntax {
        {"%"},
        {"M03"},
        {"M04"},
        {"M05"},
        {"M30"},
        {"G00", "(X,Y,Z)"}, //U V W
        {"G01", "(X,Y,Z)"},
        {"G02", "(X,Y,Z)", "(I,J)/(R)"},
        {"G03", "(X,Y,Z)", "(I,J)/(R)"},
        {"G04", "(P)"},
        {"G17+"},
        {"G18+"},
        {"G19+"},
        {"G20+"},
        {"G21+"},
        {"G90+"},
        {"G91+"},
        {"F{INT}"}
    };

    class GCODE_schedule {
        private:

        std::string _info_name = "GCODE_schedule";

        std::vector<std::string>                _commands_raw;  //raw lines of commands
        std::vector<std::vector<std::string>>   _commands;      //parsed commands separated into elements in a lvl1 vector


        int G_unit      = 21;   //Unit: `21`-Metric[mm]; `20`-Imperial[inch]
        int G_plane     = 17;   //Plane selection: `17`-XY; `18`-XZ; `19`-YZ
        int G_posit     = 90;   //Coordinate positioning: `90`-absolute; `91`-relative to previous
        int G_motion    =  0;
        int F_feedrate  = 10;  //Speed of movement: `{G_unit}/min`

        float ref_pos[3] = {0, 0, 0};


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

        // std::string _parse_line(std::string line, bool throwError = false);

        int  _syntax_idx(std::string arg, bool* gcode_additional=nullptr);
        bool _parse_line(std::string& line);
        std::vector<std::string> _lastParsed_args;
        std::string _parse_error_msg = "";

        public:
        bool verbose = false;

        const std::string get_errorMsg_parse() { return _parse_error_msg; }

        bool _arg_isNumber(std::string argToCheck, const int* solvedValue);
        std::string operator[](size_t i) const;
        std::vector<std::string> commands() { return this->_commands_raw; }

        GCODE_schedule(/* args */);
        ~GCODE_schedule();


        int add(std::string newCommand);
        int insert(size_t idx, std::string newCommand);
        int replace(size_t idx, std::string newCommand);
        int erase(size_t idx);
    };

}

bool IK_PATH::GCODE_schedule::_arg_isNumber(std::string argToCheck, const int* solvedValue) {
    static int _solvedValue = 0;
    try {
        _solvedValue = std::stoi(argToCheck.substr(1));
        solvedValue = &_solvedValue;
    }
    catch(const std::exception& e) {
        // std::cerr << e.what() << '\n';
        solvedValue = nullptr;
        return false;
    }
    return true;
}

/*
bool IK_PATH::GCODE_schedule::_parse_line(std::string line, bool throwError) {
    bool valid_line = false;

    bool valid_arg0 = false;
    bool valid_arg1 = false;

    std::string arg_0 = "";
    std::string arg_1 = "";

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
            bool temp_valid_arg0 = false;
            case 'G':
                switch (arg_value) {
                    case  0: this->G_motion =  0; temp_valid_arg0 = true; break;
                    case  1: this->G_motion =  1; temp_valid_arg0 = true; break;
                    case  2: this->G_motion =  2; temp_valid_arg0 = true; break;
                    case  3: this->G_motion =  3; temp_valid_arg0 = true; break;
                    case 17: this->G_plane  = 17; temp_valid_arg0 = true; break;
                    case 18: this->G_plane  = 18; temp_valid_arg0 = true; break;
                    case 19: this->G_plane  = 19; temp_valid_arg0 = true; break;
                    case 20: this->G_unit   = 20; temp_valid_arg0 = true; break;
                    case 21: this->G_unit   = 21; temp_valid_arg0 = true; break;
                    case 90: this->G_posit  = 90; temp_valid_arg0 = true; break;
                    case 91: this->G_posit  = 91; temp_valid_arg0 = true; break;
                    default: break;
                }
            break;
            case 'M': haven't started here because I'll likely never use this for this letters intended purposes in the near future
            break;
            case 'F': this->F_feedrate = arg_value; break;
            case 'P':
            if(prev_lett=="G04") {
                dwell_ms = arg_value;
            }
            break;

            default: break;
            if(temp_valid_arg0) valid_arg0 = true;
        }
        if(breakLoop) break;


        prev_lett = arg;
        _i++;
    }

    return valid_line;
}

*/

/**
 * @brief find index to vector for given code (ex. "G01") in `IK_PATH::GCODE_Syntax`
 * 
 * @param arg the string of code to find in `IK_PATH::GCODE_Syntax` 2d vector
 * @param gcode_additional pointer to a boolean for whether the indexed element[0] contains `+`
 * @return `int` type of index. If index not found then it'll return `-1`.
 */
int IK_PATH::GCODE_schedule::_syntax_idx(std::string arg, bool* gcode_additional) {
    int idx = 0;
    (*gcode_additional) = false;
    for(std::vector<std::string> _codeLine: GCODE_Syntax) {
        if(_codeLine[0][_codeLine[0].length()-1]=='+') {
            if(arg==_codeLine[0].substr(0, _codeLine[0].length()-1)) {
                (*gcode_additional) = true;
                return idx;
            }
        }
        else if(_codeLine[0][0]=='F') {
            return idx;
        }
        else {
            if(arg==_codeLine[0]) return idx;
        }
        idx++;
    }
    return -1;
}

bool IK_PATH::GCODE_schedule::_parse_line(std::string &line) {
    bool gcode_additional = true;
    int successful_parsed = 0;

    /**
     * @brief parse the required code letters from IK_PATH::GCODE_Syntax.
     * @param _arg0 index to line in IK_PATH::GCODE_Syntax
     * @param _arg1 index to element in line of `_arg0` vector
     * @return `std::vector<std::string>` of the letters of codes:
     * @note example: `"(A,B)/(C)"` -> {`"AB"`, `"C"`}
     */
    static auto lambda_parseAlt = [](int _arg0, int _arg1) {
        std::vector<std::string> splits;
        if(_arg0>=IK_PATH::GCODE_Syntax.size() || _arg1>=IK_PATH::GCODE_Syntax[_arg0].size()) return splits;
        std::string toCheck = IK_PATH::GCODE_Syntax[_arg0][_arg1];

        for(size_t _c=0; _c<toCheck.length(); _c++) {
            char currChar = toCheck[_c];
            if(currChar=='(') splits.push_back(std::string(""));
            else if(
                currChar==',' || currChar=='/' || currChar==')'
            ) continue;
            else splits[splits.size()-1]+=toCheck[_c];
        }
        return splits;
    };
    
    
    std::vector<std::string> args = splitString(line, " ");
    bool isValid = true;

    int plusIter = 0;
    while(gcode_additional) {
        int arg0_idx = this->_syntax_idx(args[plusIter], &gcode_additional);
        size_t idx_syntax_size = IK_PATH::GCODE_Syntax[arg0_idx].size();

        if(arg0_idx==-1) {
            this->_parse_error_msg = "arg["+std::to_string(plusIter)+"]: \""+args[plusIter]+"\" does not exist in IK_PATH::GCODE_Syntax.";
            return false;
        }
        if((args.size()-plusIter)<idx_syntax_size) {
            this->_parse_error_msg = "input string of arguments does not contain minimum number of arguments for gcode code:\""+args[plusIter]+"\".";
            return false;
        }
        // args[0] found

        int currentArgsLen_0= plusIter;
        int currentArgsLen  = idx_syntax_size-1;
        for(int i=0; i<idx_syntax_size; i++) {
            currentArgsLen += args[i].length();
            if(i<=plusIter) currentArgsLen_0 += args[i].length();
        }

        bool __temp = true;
        // Searching for args[>0] matches
        for(size_t i=1; i<idx_syntax_size; i++) {
            __temp = false;
            //Go through GCODE_syntax[arg0_idx]{}.
            //This for() loop shouldn't occur for current codes with `+` symbol as their length is only 1.
            // -in the future if I want to use `+` symbol for codes with parameters then I think I'll need to find string
            // -start and end pos of those parameters associated with said code. (???)

            int vecCount = 0;
            std::vector<std::string> _alt = lambda_parseAlt(arg0_idx, i); // ->{"AB", "C"}
            for(std::string _ii: _alt) {
                for(int _iii=0; _iii<_ii.length(); _iii++) {
                    if(line.substr(currentArgsLen_0,currentArgsLen).find(_ii[_iii])!=std::string::npos) {
                        vecCount++;
                        break;
                    }
                }
            }
            if(vecCount==0) {
                this->_parse_error_msg = "followup arguments to code \""+args[plusIter]+"\" does not contain any of the obligatory args: \""+GCODE_Syntax[arg0_idx][i]+"\".";
                return false;
            }
            else if(vecCount>=2) {
                this->_parse_error_msg = "followup arguments to code \""+args[plusIter]+"\" contain either both of types not allowed to co-exist or has same arg repeated.";
                return false;
            }
            successful_parsed++;
        }
        if(__temp) successful_parsed++;
        plusIter++;
    }
    std::string newStr = "";
    for(std::string arg: args) newStr+=arg;
    line = newStr;

    // filter out comments and whatnot and store that in _lastParsed_args.
    std::vector<std::string> tempVec;
    for(int i=0; i<successful_parsed; i++) tempVec.push_back(args[i]);
    this->_lastParsed_args = tempVec; 

    return isValid;
}

std::string IK_PATH::GCODE_schedule::operator[](size_t i) const {
    if(i>=this->_commands_raw.size()) throw std::runtime_error("ERROR: "+this->_info_name+"::operator[](size_t) input index is bigger than stored commands");
    return const_cast<std::string&>(_commands_raw.at(i));
}

IK_PATH::GCODE_schedule::GCODE_schedule(/* args */) {}
IK_PATH::GCODE_schedule::~GCODE_schedule() {}

int IK_PATH::GCODE_schedule::add(std::string newCommand) {
    if(!(this->_parse_line(newCommand))) {
        if(this->verbose) std::cout << this->_parse_error_msg << std::endl;
        return 1;
    }
    this->_commands.push_back(this->_lastParsed_args);
    this->_commands_raw.push_back(newCommand);
    
    return 0;
}
int IK_PATH::GCODE_schedule::insert(size_t idx, std::string newCommand) {
    if(idx>=this->_commands.size()) throw std::runtime_error("ERROR: "+this->_info_name+"::insert(size_t, std::string) input index is bigger than stored commands");
    if(!(this->_parse_line(newCommand))) {
        if(this->verbose) std::cout << this->_parse_error_msg << std::endl;
        return 1;
    }
    this->_commands.insert(this->_commands.begin()+idx, this->_lastParsed_args);
    this->_commands_raw.insert(this->_commands_raw.begin()+idx, newCommand);
    
    return 0;
}
int IK_PATH::GCODE_schedule::replace(size_t idx, std::string newCommand) {
    if(idx>=this->_commands.size()) throw std::runtime_error("ERROR: "+this->_info_name+"::replace(size_t, std::string) input index is bigger than stored commands");
    if(!(this->_parse_line(newCommand))) {
        if(this->verbose) std::cout << this->_parse_error_msg << std::endl;
        return 1;
    }
    this->_commands[idx] = this->_lastParsed_args;
    this->_commands_raw[idx] = newCommand;
    
    return 0;
}
int IK_PATH::GCODE_schedule::erase(size_t idx) {
    if(idx>=this->_commands.size()) throw std::runtime_error("ERROR: "+this->_info_name+"::erase(size_t) input index is bigger than stored commands");
    this->_commands.erase(this->_commands.begin()+idx);
    this->_commands_raw.erase(this->_commands_raw.begin()+idx);
}




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