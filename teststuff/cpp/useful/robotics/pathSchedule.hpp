
#pragma once
#ifndef HPP_PATH_SCHEDULE
#define HPP_PATH_SCHEDULE

#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <fstream>

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
        {" "},
        {"("},
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
        {"G28", "(X,Y,Z)"},
        {"G90+"},
        {"G91+"},
        {"F{INT}"}
    };

    class GCODE_schedule {
        private:

        std::string _info_name = "GCODE_schedule";

        std::vector<std::string>                _commands_orig; //original strings before getting parsed.
        std::vector<std::string>                _commands_raw;  //parsed raw std::string lines of commands
        std::vector<std::vector<std::string>>   _commands;      //parsed commands separated into elements in a lvl1 vector


        int G_unit      = 21;   //Unit: `21`-Metric[mm]; `20`-Imperial[inch]
        int G_plane     = 17;   //Plane selection: `17`-XY; `18`-XZ; `19`-YZ
        int G_posit     = 90;   //Coordinate positioning: `90`-absolute; `91`-relative to previous
        int G_motion    =  0;
        int F_feedrate  = 10;  //Speed of movement: `{G_unit}/min`

        float ref_pos[3] = {0, 0, 0};


        char _valid_letters[10] = {
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
        int _parse_line(std::string& line);
        std::string _lastArgs_unparsed = "";
        std::vector<std::string> _lastParsed_args;
        std::string _parse_error_msg = "";

        public:
        bool verbose = true;
        
        size_t size() { return this->_commands_raw.size(); }
        std::string get_errorMsg_parse() { return _parse_error_msg; }

        bool _arg_isNumber(std::string argToCheck, const int* solvedValue);

        std::vector<std::string> operator[](size_t i) const; //_commands
        std::string get_raw(size_t i) const; //_commands_raw
        std::string get_orig(size_t i) const; //_commands_orig

        std::vector<std::vector<std::string>> commands() { return this->_commands; }
        std::vector<std::string> commands_raw() { return this->_commands_raw; }
        std::vector<std::string> commands_orig() { return this->_commands_orig; }

        GCODE_schedule(/* args */);
        ~GCODE_schedule();

        bool loadFile(std::string filename);

        int swap(size_t i0, size_t i1);
        int add(std::string newCommand);
        int insert(size_t idx, std::string newCommand);
        int replace(size_t idx, std::string newCommand);
        int erase(size_t idx);

        int copy_in(IK_PATH::GCODE_schedule* _src, int mode=0, size_t mode2_insrt=0);
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
        if(_codeLine[0][_codeLine[0].length()-1]=='+') { //if last char is '+'
            if(arg==_codeLine[0].substr(0, _codeLine[0].length()-1)) { //if substring of arg without '+' char exists in syntax
                (*gcode_additional) = true;
                return idx;
            }
        }
        else if(arg[0]==_codeLine[0][0] && (arg[0]!='G' && arg[0]!='M')) { //single char matching: find special cases where arg[0] matches first char of special cases (like F{INT})
            // std::cout << "(F)";
            return idx;
        }
        // else if(arg[0]=='(') {
        //     return -2;
        // }
        else {
            if(arg==_codeLine[0]) {
                // std::cout << "(1)";
                return idx;
            }
        }
        idx++;
    }
    // std::cout << "|_syntax_idx()->-1|"<<std::endl;
    return -1;
}

int IK_PATH::GCODE_schedule::_parse_line(std::string &line) {
    this->_lastArgs_unparsed = line;
    bool gcode_additional = true;
    int parsed_words = 1;

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
    

    static auto lambda_ignoreReturn = [](std::string& line, IK_PATH::GCODE_schedule* ptr) {
        line = "";
        ptr->_lastParsed_args = std::vector<std::string>{""};
        return 1;
    };
    std::vector<std::string> args = splitString(line, " ");
    if(args[0].length()==0) { //return an empty container because nothing was parsed
        return lambda_ignoreReturn(line, this);
        // std::cout << "(1)";
        // line = "";
        // this->_lastParsed_args = std::vector<std::string>{""}; 
        // return 1;
    }
    // std::cout <<"[0]"<< args[0]<<"|";
    /**
     * meaning of returned values: (values!=0 means non-successful parsing)
     * `-1` - error: fatal: inccorrect syntax (non-existent codes were used)
     * `0` - successful line parsing
     * `1` - error: ignored line for various reasons.
     */
    int returnVal = 0;

    // bool tempPrint = true;
    // if(args[0]!="G02") tempPrint = false;

    int plusIter = 0;
    while(gcode_additional) {
        int arg0_idx = this->_syntax_idx(args[plusIter], &gcode_additional);
        size_t idx_syntax_size = IK_PATH::GCODE_Syntax[arg0_idx].size(); //{"G01", "(X,Y,Z)", "(I,J)"}.size()

        // std::cout << idx_syntax_size << " | ";

        if(arg0_idx==-1) {
            this->_parse_error_msg = "arg["+std::to_string(plusIter)+"]: \""+args[plusIter]+"\" does not exist in IK_PATH::GCODE_Syntax.";
            return -1;
        }
        if((args.size()-plusIter)<idx_syntax_size) {
            this->_parse_error_msg = "input string of arguments does not contain minimum number of arguments for gcode code:\""+args[plusIter]+"\".";
            return -1;
        }
        // args[0] found

        int currentArgsLen_0= 0;
        int currentArgsLen  = line.length();

        bool __temp = true;
        // Searching for args[>0] matches
        for(size_t i=1; i<idx_syntax_size; i++) {
            std::vector<std::string> _alt = lambda_parseAlt(arg0_idx, i); // ->{"AB", "C"}
            //one two three four one2

            // currentArgsLen_0= 0;
            currentArgsLen_0 = 0;
            for(int ii=0; ii<parsed_words; ii++) {
                currentArgsLen_0+=args[ii].length()+1;
            }
            currentArgsLen  = line.length()-currentArgsLen_0;
            // int _ii = 0;
            // for(std::string _arg: args) {
            //     if(_ii>=i) currentArgsLen   +=_arg.length();
            //     // if(_ii<i)  currentArgsLen_0 +=_arg.length();
            //     _ii++;
            // }
            // std::cout<<" {"<<currentArgsLen_0<<", "<<currentArgsLen<<"}|"<<std::endl;

            __temp = false;
            //Go through GCODE_syntax[arg0_idx]{}.
            //This for() loop shouldn't occur for current codes with `+` symbol as their length is only 1.
            // -in the future if I want to use `+` symbol for codes with parameters then I think I'll need to find string
            // -start and end pos of those parameters associated with said code. (???)

            int vecCount = 0;
            int _veciii = 0;
            int _i_alt = 0;
            for(std::string _ii: _alt) { // {"ABC", "D"}
                for(int _iii=0; _iii<_ii.length(); _iii++) { //iterate through char in _ii string
                    // std::cout <<"["<<line.substr(currentArgsLen_0, currentArgsLen)<<"]";
                    if(line.substr(currentArgsLen_0, currentArgsLen).find(_ii[_iii])!=std::string::npos) { //if "AB[C]D" in substr
                        _i_alt = _veciii;
                        vecCount++;
                        break;
                    }
                    // std::cout << _ii<<"-["<<line.substr(currentArgsLen_0, currentArgsLen)<<"]"<<", ";
                }
                _veciii++;
            }
            if(vecCount==0) {
                this->_parse_error_msg = "arguments to code \""+args[plusIter]+"\" does not contain any of the obligatory args: \""+GCODE_Syntax[arg0_idx][i]+"\".";
                return -1;
            }
            else if(vecCount>=2) {
                this->_parse_error_msg = "arguments to code \""+args[plusIter]+"\" contain either both of types not allowed to co-exist or has same arg repeated.";
                return -1;
            }
            
            for(int _i_c=0; _i_c<_alt[_i_alt].length(); _i_c++) {
                if(line.substr(currentArgsLen_0, currentArgsLen).find(_alt[_i_alt][_i_c])!=std::string::npos) {
                    parsed_words++;
                    // currentArgsLen_0++;
                }
            }
            
            // parsed_words += _alt[_i_alt].length();
            // parsed_words++;
        }
        // if(__temp) parsed_words++;
        // parsed_words+=idx_syntax_size;
        // if(tempPrint) std::cout << parsed_words<<std::boolalpha<<gcode_additional<<std::endl;
        if(gcode_additional && idx_syntax_size<=1) parsed_words++;
        plusIter++;
    }
    std::string newStr = "";
    for(std::string arg: args) newStr+=arg+" ";
    line = newStr;

    // // filter out comments and whatnot and store that in _lastParsed_args.
    // std::cout <<">> "<< parsed_words << std::endl;
    std::vector<std::string> tempVec;
    for(int i=0; i<parsed_words; i++) tempVec.push_back(args[i]);
    this->_lastParsed_args = tempVec; 

    // std::cout << std::endl;

    return returnVal;
}

std::vector<std::string> IK_PATH::GCODE_schedule::operator[](size_t i) const {
    if(i>=this->_commands.size()) throw std::runtime_error(this->_info_name+"::operator[](size_t) input index is bigger than stored commands");
    return const_cast<std::vector<std::string>&>(_commands.at(i));
}
std::string IK_PATH::GCODE_schedule::get_raw(size_t i) const {
    if(i>=this->_commands_raw.size()) throw std::runtime_error(this->_info_name+"::get_raw(size_t) input index is bigger than stored commands");
    return const_cast<std::string&>(_commands_raw.at(i));
}
std::string IK_PATH::GCODE_schedule::get_orig(size_t i) const {
    if(i>=this->_commands_orig.size()) throw std::runtime_error(this->_info_name+"::get_orig(size_t) input index is bigger than stored commands");
    return const_cast<std::string&>(_commands_orig.at(i));
}

IK_PATH::GCODE_schedule::GCODE_schedule(/* args */) {}
IK_PATH::GCODE_schedule::~GCODE_schedule() {}

/**
 * @brief load gcode file (ex. `.nc` extension) into class object commands storage.
 * 
 * @param filename `std::string` of filename with relative or absolute path included
 * @return `true` if file successfully loaded.
 * @return `false` if an error occurred trying to load file.
 */
bool IK_PATH::GCODE_schedule::loadFile(std::string filename) {
    std::ifstream readFile;
    readFile.open(filename);
    if(!readFile.is_open()) {
        if(this->verbose) std::cout << "Could not open filename: \""+filename+"\""<<std::endl;
        return false;
    }

    std::string line;
    for(int i=1; getline(readFile, line); i++) {
        if(this->add(line)==1) {
            if(this->verbose) std::cout << "failed to read line: "<<i<<std::endl;
            return false;
        }
    }
    return true;
}

int IK_PATH::GCODE_schedule::swap(size_t i0, size_t i1) {
    if(i0==i1) throw std::runtime_error(this->_info_name+"::swap(size_t, size_t): the two index arguments to swap can't be the same.");
    if(i0>=this->_commands.size()) throw std::runtime_error(this->_info_name+"::swap(size_t, size_t): input index `i0` is bigger than stored commands");
    if(i1>=this->_commands.size()) throw std::runtime_error(this->_info_name+"::swap(size_t, size_t): input index `i0` is bigger than stored commands");
    std::vector<std::string> tmp_commands = this->_commands[i0];
    std::string 
        tmp_commands_raw = this->_commands_raw[i0],
        tmp_commands_orig = this->_commands_orig[i0];

    this->_commands[i0]     = this->_commands[i1];
    this->_commands_raw[i0] = this->_commands_raw[i1];
    this->_commands_orig[i0]= this->_commands_orig[i1];

    this->_commands[i1]     = tmp_commands;
    this->_commands_raw[i1] = tmp_commands_raw;
    this->_commands_orig[i1]= tmp_commands_orig;

    return 0;
}

int IK_PATH::GCODE_schedule::add(std::string newCommand) {
    if(this->_parse_line(newCommand)==-1) {
        if(this->verbose) std::cout << "IK_PATH::GCODE_schedule::add(std::string): "+this->_parse_error_msg << std::endl;
        return 1;
    }
    this->_commands.push_back(this->_lastParsed_args);
    this->_commands_raw.push_back(newCommand);
    this->_commands_orig.push_back(this->_lastArgs_unparsed);
    
    return 0;
}
int IK_PATH::GCODE_schedule::insert(size_t idx, std::string newCommand) {
    if(idx>=this->_commands.size()) throw std::runtime_error(this->_info_name+"::insert(size_t, std::string) input index is bigger than stored commands");
    if(this->_parse_line(newCommand)==-1) {
        if(this->verbose) std::cout << this->_parse_error_msg << std::endl;
        return 1;
    }
    this->_commands.insert(this->_commands.begin()+idx, this->_lastParsed_args);
    this->_commands_raw.insert(this->_commands_raw.begin()+idx, newCommand);
    this->_commands_orig.insert(this->_commands_orig.begin()+idx, this->_lastArgs_unparsed);
    
    return 0;
}
int IK_PATH::GCODE_schedule::replace(size_t idx, std::string newCommand) {
    if(idx>=this->_commands.size()) throw std::runtime_error(this->_info_name+"::replace(size_t, std::string) input index is bigger than stored commands");
    if(this->_parse_line(newCommand)==-1) {
        if(this->verbose) std::cout << this->_parse_error_msg << std::endl;
        return 1;
    }
    this->_commands[idx] = this->_lastParsed_args;
    this->_commands_raw[idx] = newCommand;
    this->_commands_orig[idx] = this->_lastArgs_unparsed;
    
    return 0;
}
int IK_PATH::GCODE_schedule::erase(size_t idx) {
    if(idx>=this->_commands.size()) throw std::runtime_error(this->_info_name+"::erase(size_t) input index is bigger than stored commands");
    this->_commands.erase(this->_commands.begin()+idx);
    this->_commands_raw.erase(this->_commands_raw.begin()+idx);
    this->_commands_orig.erase(this->_commands_orig.begin()+idx);
}


int IK_PATH::GCODE_schedule::copy_in(IK_PATH::GCODE_schedule* _src, int mode, size_t mode2_insrt) {
    if(_src->size()==0) throw std::runtime_error(this->_info_name+"::copy_in(IK_PATH::GCODE_schedule*, int, size_t): in, int modeput object doesn't have any values");
    if(mode2_insrt>this->_commands.size()) throw std::runtime_error(this->_info_name+"::copy_in(IK_PATH::GCODE_schedule*, int, size_t): input value for mode2_insrt is bigger than stored size");
    
    std::vector<std::vector<std::string>> tmp_commands;
    std::vector<std::string> tmp_commands_raw;
    std::vector<std::string> tmp_commands_orig;
    if(mode==0 || mode==2) {
        tmp_commands = _src->commands();
        tmp_commands_raw = _src->commands_raw();
        tmp_commands_orig = _src->commands_orig();
    }

    switch (mode) {
        case 0: //append
        this->_commands.insert(this->_commands.end(), tmp_commands.begin(), tmp_commands.end());
        this->_commands_raw.insert(this->_commands_raw.end(), tmp_commands_raw.begin(), tmp_commands_raw.end());
        this->_commands_orig.insert(this->_commands_orig.end(), tmp_commands_orig.begin(), tmp_commands_orig.end());
        break;
        case 1: //overwrite
        this->_commands = tmp_commands;
        this->_commands_raw = tmp_commands_raw;
        this->_commands_orig = tmp_commands_orig;
        break;
        case 2: //insert at an index
        this->_commands.insert(this->_commands.begin()+mode2_insrt, tmp_commands.begin(), tmp_commands.end());
        this->_commands_raw.insert(this->_commands_raw.end()+mode2_insrt, tmp_commands_raw.begin(), tmp_commands_raw.end());
        this->_commands_orig.insert(this->_commands_orig.end()+mode2_insrt, tmp_commands_orig.begin(), tmp_commands_orig.end());
        break;
    default:
        break;
    }

    return 0;
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