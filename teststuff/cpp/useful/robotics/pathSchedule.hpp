
#pragma once
#ifndef HPP_PATH_SCHEDULE
#define HPP_PATH_SCHEDULE

#include <iostream>
#include <iomanip>

#include <string>
#include <math.h>
#include <vector>
#include <fstream>

#include <useful.hpp>
#include <search_multithread.hpp>


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

        /**
         * @brief check member variables of `pos_point` object for `NULL_POS_POINT` and replace those variable according to function
         * 
         * @param _toCheck reference to `pos_point` to check through and fill in accordingly
         * @param idx index to use to fill in in "NULL_POS_POINT" values
         */
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
     * `|`: `a|`     - another code of same type can be called with this symbol, but doesn't have to be called. Without it, any following arguments must be included.
     * 
     */
    inline const std::vector<std::vector<std::string>> GCODE_Syntax {
        {" "},
        {"("},
        {"%"},
        {"M03"},
        {"M04"},
        {"M05"},
        {"M30"},
        {"G00", "(X,Y,Z)/(U,V,W)"}, // Linear interpolation: instant motion. U V W = a b y //tilt change is assumed to be a constant
        {"G01", "(X,Y,Z)/(U,V,W)"}, // Linear interpolation: uses feedrate like G02 and G03
        {"G02", "(X,Y,Z)", "(I,J)/(R)"}, // Circular interpolation: CW
        {"G03", "(X,Y,Z)", "(I,J)/(R)"}, // Circular interpolation: CCW
        {"G04", "P{INT}"}, // Dwell/Pause for `P` number of milliseconds
        {"G17+"}, // G_plane: XY
        {"G18+"}, // G_plane: XZ
        {"G19+"}, // G_plane: YZ
        {"G20+"}, // G_unit:  Imperial[inch]
        {"G21+"}, // G_unit:  Metric[mm]
        {"G28|", "(X,Y,Z)"}, // Return to home position, possibly by moving to given coord first then home.
        {"G90+"}, // G_positioning: absolute (coord)
        {"G91+"}, // G_positioning: relative (coord)
        {"A0+"},  // A_positioning: absolute (orient)
        {"A1+"},  // A_positioning: relative (orient)
        {"F{INT}"}
    };


    inline void null_TDEF_runCode(float X, float Y, float Z, float a, float b, float y) {}


    class GCODE_schedule {
        private:
        /**
         * type for function to callin 
         * Parameters: (X, Y, Z, a, b, y)
         */
        typedef void    (*TDEF_runCode)(float, float, float, float, float, float);

        std::string _info_name = "GCODE_schedule";

        std::vector<std::string>                _commands_orig; //original strings before getting parsed.
        std::vector<std::string>                _commands_raw;  //parsed raw std::string lines of commands
        std::vector<std::vector<std::string>>   _commands;      //parsed commands separated into elements in a lvl1 vector


        int G_unit      = 21;   //Unit: `21`-Metric[mm]; `20`-Imperial[inch]
        int G_plane     = 17;   //Plane selection: `17`-XY; `18`-XZ; `19`-YZ
        int G_posit     = 90;   //Coordinate positioning: `90`-absolute; `91`-relative to previous
        int G_motion    =  0;
        int A_posit     =  0;   //Orientation positioning: `0`-absolute, `1`-relative
        int F_feedrate  = 10;   //Speed of movement: `{G_unit}/min`

        float ref_pos[3] = {0, 0, 0}; // last input coordinate (updated by each new command/coordinate-point)
        float ref_tilt[3]= {0, 0, 0}; // last input orientation (updated by each new command/orient-point)


        /**
         * @brief find index to vector for given primary gcode (ex. "G01") in `IK_PATH::GCODE_Syntax`
         * 
         * @param arg the string of code to find in `IK_PATH::GCODE_Syntax` 2d vector
         * @param gcode_additional_primary pointer to a boolean for whether the indexed element[0] contains `+`: meaning another primary gcode
         * argument is allowed to be after this arg.
         * @param gcode_optional_secondary pointer to a boolean for whether the indexed element[0] contains `|`: meaning the following secondary gcode
         * arguments are optional to be added.
         * @return `int` type of index. If index not found then it'll return `-1`.
         */
        int  _syntax_idx(std::string arg, bool *gcode_additional_primary=nullptr, bool *gcode_optional_secondary=nullptr);
        int _parse_line(std::string& line);
        std::string _lastArgs_unparsed = "";
        std::vector<std::string> _lastParsed_args;
        std::string _parse_error_msg = "";

        public:
        bool verbose = true;
        bool verbose_debug = false;
        
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

        /**
         * @brief load gcode file (ex. `.nc` extension) into class object commands storage.
         * 
         * @param filename `std::string` of filename with relative or absolute path included
         * @return `true` if file successfully loaded.
         * @return `false` if an error occurred trying to load file.
         */
        bool loadFile(std::string filename);

        int swap(size_t i0, size_t i1);
        int add(std::string newCommand);
        int insert(size_t idx, std::string newCommand);
        int replace(size_t idx, std::string newCommand);
        int erase(size_t idx);

        int copy_in(IK_PATH::GCODE_schedule* _src, int mode=0, size_t mode2_insrt=0);

        int runSchedule(TDEF_runCode funcToCall = null_TDEF_runCode);

    };

}



#endif