
#include "pathSchedule.hpp"
#include <useful.hpp>


// bool IK_PATH::GCODE_schedule::_arg_isNumber(std::string argToCheck, const int* solvedValue) {
//     static int _solvedValue = 0;
//     try {
//         _solvedValue = std::stoi(argToCheck.substr(1));
//         solvedValue = &_solvedValue;
//     }
//     catch(const std::exception& e) {
//         // std::cerr << e.what() << '\n';
//         solvedValue = nullptr;
//         return false;
//     }
//     return true;
// }

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

int IK_PATH::GCODE_schedule::_syntax_idx(std::string arg, bool *gcode_additional_primary, bool *gcode_optional_secondary) {
    if(arg.size()==0) throw std::invalid_argument(this->_info_name+"::_syntax_idx(std::string, bool*, bool*) argument for `arg` cannot be empty.");
    int idx = 0;
    int idx_firstFound = -1;
    if(gcode_additional_primary) (*gcode_additional_primary) = false;
    if(gcode_optional_secondary) (*gcode_optional_secondary) = false;
    for(std::vector<std::string> _codeLine: GCODE_Syntax) {
        if(_codeLine.at(0).at(_codeLine.at(0).size()-1)=='+') {
            /// If last char of first argument in current _codeLine[0] == '+'
            if(this->verbose_debug) std::cout << " ['+' found: compr:\""<<arg<<"\" w. \""<<_codeLine.at(0).substr(0, _codeLine.at(0).size()-1)<<"] "; std::cout.flush();

            if(arg==_codeLine.at(0).substr(0, _codeLine.at(0).size()-1)) {
                /// If first argument in current _codeLine (i.e. _codeLine[0]) without '+' is the same as current arg
                if(this->verbose_debug) std::cout << "-->substr w.out. +| "; std::cout.flush();
                if(gcode_additional_primary) (*gcode_additional_primary) = true;
                return idx;
            }
            else {
                /// Current _codeLine primary argument (_codeLine[0]) contains `+` symbol but isn't same argument as `arg`.

                if(this->verbose_debug) std::cout << "-->substr w.out. + NOT| "; std::cout.flush();
                // idx = 0;
            }
        }
        else if(_codeLine.at(0).at(_codeLine.at(0).size()-1)=='|') {
            /// If last char of first argument in current _codeLine[0] == '|'

            if(arg==_codeLine.at(0).substr(0, _codeLine.at(0).size()-1)) {
                
                if(gcode_optional_secondary) (*gcode_optional_secondary) = true;
                return idx;
            }
            else {

            }
        }
        else if(arg.at(0)==_codeLine.at(0).at(0) && (arg.at(0)!='G' && arg.at(0)!='M')) {
            /// Single char matching: find special cases where arg[0] matches first char of special cases (like F{INT})
            std::cout << arg << std::endl;
            switch (arg.at(0)) {
            case 'F':{ /// `F{INT}`
                try {
                    int _tempInt = std::stoi(arg.substr(1));
                    return idx;
                }
                catch(const std::exception& e) {
                    std::cout << e.what() << '\n';
                    return -1;
                }
                break;
            }
            // case 'P': {
            //     try {
            //         int _tempInt = std::stoi(arg.substr(1));
            //         return idx;
            //     }
            //     catch(const std::exception& e) {
            //         std::cout << e.what() << '\n';
            //         return -1;
            //     }
            //     break;
            // }
            default:
                break;
            }
            
            return idx;
        }
        // else if(arg[0]=='(') {
        //     return -2;
        // }
        else {
            if(arg==_codeLine.at(0)) {
                // std::cout << "(1)";
                return idx;
            }
        }
        idx++;
    }
    if(idx_firstFound!=-1) {
        std::cout << "\t {"<<arg<<","<<idx_firstFound<<"}"<<std::endl;
        return idx_firstFound;
    }
    // std::cout << "|_syntax_idx()->-1|"<<std::endl;
    return -1;
}



bool IK_PATH::_contains_size_t(std::string arg, size_t startPos, bool call_except, std::string call_src_name, size_t *return_var) {
    try {
        size_t _retur = std::stoull(arg.substr(startPos));
        if(return_var) *return_var = _retur;
    }
    catch(const std::exception& e) {
        if(call_except) throw std::runtime_error(call_src_name+" "+e.what());
        return false;
    }
    return true;
}

int IK_PATH::GCODE_schedule::_parse_line(std::string &line) {

    /**
     * @brief parse string from GCODE_syntax
     * 
     */
    static auto lambda_parseAlt = [this](std::string arg) {
        // NOTE: max depth: 1

        std::vector<std::string> splits;
        int _depth = 0;
        for(size_t _c=0; _c<arg.size(); _c++) {
            switch (arg.at(_c)) {
            case '(':
                splits.push_back("");
                _depth++;
                break;
            case ')':
                _depth--;
                break;
            case '/':
                break;
            default:
                splits.at(splits.size()-1).push_back(arg.at(_c));
                break;
            }
        }
        return splits;
    };


    size_t pos_brace0 = line.find('(');
    size_t pos_brace1 = line.find(')');
    if(pos_brace1 < pos_brace0) {
        this->_parse_error_msg = "incorrect use of () braces.";
        return 1;
    }
    if(pos_brace0 != std::string::npos) {
        line+=" ";
        line.erase(pos_brace0, pos_brace1-pos_brace0+1);
    }
    size_t pos_semicolon = line.find(';');
    if(pos_semicolon != std::string::npos) {
        line+=" ";
        line.erase(pos_semicolon);
    }

    size_t pos_cR = line.find('\0');
    if(pos_cR != std::string::npos) {
        std::cout << "Warning: carriage return character was found in a gcode args." << std::endl;
    }
    // std::cout << "<<"<<formatNumber<std::string>(line, 40, 0, "left") <<">>"<< std::endl;

    std::vector<std::string> args = splitString(line, " ");
    std::vector<std::string> parsed_args;

    GCodeLevel  currLevel = GCodeLevel__primary;

    std::string gcode_currPrimary   = "";
    size_t      gcode_primary_num       = 0;

    std::vector<std::string> parsed_splits;
    for(size_t i=0; i<args.size(); i++) {
        bool __special_break = false;
        switch (args.at(i).at(0)) {
        case '%':
            __special_break = true;
            parsed_args.push_back(args.at(i));
            //Need to make remote_GUI display the raw lines and instead have the runSchedule member do this sort of comment filtering
            break;
        default:
            break;
        }
        if(__special_break) break;

        switch (currLevel) {
        case GCodeLevel__primary: { /// Searching for primary
            /// Checking for number in primary gcode command.
            if(_contains_size_t(args.at(i), 1, false, "_parse_line(std::string&, bool)", &gcode_primary_num)) {
                bool _charBreak = false;
                switch (args.at(i).at(0)) {
                case 'F':
                    parsed_args.push_back(args.at(i));
                    _charBreak = true;
                    break;
                }
                if(_charBreak) break;
                
                int _gcodeIdx = -1;
                /// Search for valid primary/"key" in GCODE_syntax
                if((_gcodeIdx = GCODE_syntax.find(args.at(i).at(0)+std::to_string(gcode_primary_num), false))!=-1) {
                    /// Valid gcode primary found
                    gcode_currPrimary = args.at(i).at(0)+std::to_string(gcode_primary_num);
                    currLevel = GCodeLevel__secondary;
                    parsed_args.push_back(args.at(i));
                    
                    break;
                }
                else this->_parse_error_msg = "\""+args.at(i)+"\" is not a valid gcode command.";
            }
            else this->_parse_error_msg = "GCode command \""+args.at(i)+"\" must contain integers to be valid commands.";
            return 1;
        }
        case GCodeLevel__secondary: {
            DIY::typed_dict<std::string, std::vector<std::string>> _2ndaries = GCODE_syntax.get(gcode_currPrimary);

            bool _secondaryFound = false;
            /// Check for obligatory commands.
            size_t found_groups = 0; // container to check how many obligatory groups were found.
            for(size_t s2_idx=0; s2_idx<_2ndaries.get("obl").size(); s2_idx++) { // index to current element/container in first "obl" secondary gcode arg group
                /// index to "(X,Y,Z)/(A)" from {"(X,Y,Z)/(A)", }
                /// Note: there has to be a match with every iteration of this loop, otherwise it means an obligatory command hasn't been found.

                /**
                 * Number of elements found in xor "neighbours". 
                 *  if `>=2`: both occurrances in xor's found
                 *  if `==0 && vec.size()>0`: none of the obligatory commands found.
                 */
                size_t found_parallels = 0;
                std::vector<std::string> _2nd_alt = lambda_parseAlt(_2ndaries.get("obl").at(s2_idx)); // vector of "parallels": `{"ABC", "D"}`
                for(size_t _ii=0; _ii<_2nd_alt.size(); _ii++) { // idx to current "neighbour"/element in splits/xor's: `"ABC"` from `{"ABC", "D"}`
                    size_t found = 0;
                    for(size_t _i_n=0; _i_n<_2nd_alt.at(_ii).size(); _i_n++) { // idx to current char in "neighbour": `'B'` from `"ABC"`
                        /// Iterate through every character in current xor "group"/"neighbour" split string
                        if(args.at(i).at(0)==_2nd_alt.at(_ii).at(_i_n)) {
                            /// Matching initial character found. It's either check for number or the line is invalid.
                            try {
                                std::stoi(args.at(i).substr(1));
                            }
                            catch(const std::exception& e) {
                                this->_parse_error_msg = "obl invalid GCode: command \""+args.at(i)+"\" doesn't contain valid numbers: "+e.what();
                                return 1; 
                            }
                            parsed_args.push_back(args.at(i));
                            found++;
                            i++;
                            _secondaryFound = true;
                        }
                        if(i==args.size()) break;
                    }
                    if(found>0) {
                        found_parallels++;
                        found_groups++;
                    }
                    if(i==args.size()) break;
                }
                if(_2nd_alt.size()!=0 && found_parallels!=1) {
                    if(found_parallels==0)  this->_parse_error_msg = "none of the obligatory secondary commands found for primary: "+gcode_currPrimary+" to find: "+formatContainer1<std::vector<std::string>>(_2nd_alt, _2nd_alt.size(), 0, 0, "left");
                    else                    this->_parse_error_msg = "multiple commands from separated xor groups were found: "+formatContainer1<std::vector<std::string>>(_2nd_alt, _2nd_alt.size(), 0, 0, "left");
                    return 1;
                }
                if(i==args.size()) break;
            }
            if(_2ndaries.get("obl").size()!=found_groups) {
                this->_parse_error_msg = "found groups doesn't match number of obligatory groups to find for primary gcode: "+gcode_currPrimary;
                return 1;
            }
            if(i==args.size()) break;
            /// Check for optional commands
            for(size_t s2_idx=0; s2_idx<_2ndaries.get("opt").size(); s2_idx++) { // index to current element/container in second "opt" secondary gcode arg group

                size_t found_parallels = 0;
                std::vector<std::string> _2nd_alt = lambda_parseAlt(_2ndaries.get("opt").at(s2_idx));
                for(size_t _ii=0; _ii<_2nd_alt.size(); _ii++) {
                    size_t found = 0;
                    for(size_t _i_n=0; _i_n<_2nd_alt.at(_ii).size(); _i_n++) {

                        if(args.at(i).at(0)==_2nd_alt.at(_ii).at(_i_n)) {
                            /// Matching initial character found. It's either check for number or the line is invalid.
                            try {
                                std::stoi(args.at(i).substr(1));
                            }
                            catch(const std::exception& e) {
                                this->_parse_error_msg = "opt invalid GCode: command \""+args.at(i)+"\" doesn't contain valid numbers: "+e.what();
                                return 1; 
                            }
                            
                            parsed_args.push_back(args.at(i));
                            found++;
                            i++;
                            _secondaryFound = true;
                        }
                        if(i==args.size()) break;
                    }
                    if(found>0) {
                        found_parallels++;
                    }
                    if(i==args.size()) break;
                }
                if(_2nd_alt.size()!=0 && found_parallels>1) {
                    this->_parse_error_msg = "multiple commands from separated xor groups were found: "+formatContainer1<std::vector<std::string>>(_2nd_alt, _2nd_alt.size(), 0, 0, "left");
                    return 1;
                }
                if(i==args.size()) break;
            }

            if(!_secondaryFound) i--;
            currLevel = GCodeLevel__primary;
            break;
        }
        default:
            throw std::runtime_error("what the fuck.");
        }
    }

    this->_lastArgs_unparsed = line;
    std::string _parsed_line = "";
    for(auto arg: parsed_args) {
        _parsed_line += arg + " ";
    }
    line = _parsed_line;
    this->_lastParsed_args = parsed_args;

    return 0;
}

// int IK_PATH::GCODE_schedule::_parse_line(std::string &line) {
//     // GCodeLevel currLevel = GCodeLevel__primary;
//     this->_lastArgs_unparsed = line;
//     bool gcode_additional_primary = true; // whether this primary allows another primary gcode argument (`+` symbol if true, otherwise false)
//     bool gcode_optional_secondary = true; // whether the secondary arguments are optional (`|` symbol if true, otherwise false)
//     int parsed_words = 1; // counter for the number of valid parsed/found gCode commands/arguments from `IK_PATH::GCODE_Syntax`
//     if(this->verbose) std::cout << this->_info_name<< "::_parse_line(std::string) function call:\""<<line<<"\"" << std::endl;
//     /**
//      * @brief parse the required code letters from IK_PATH::GCODE_Syntax.
//      * @param _arg0 index to line in IK_PATH::GCODE_Syntax
//      * @param _arg1 index to element in line of `_arg0` vector: index to secondary arguments
//      * @return `std::vector<std::string>` of the letters of codes:
//      * @note example: {`"(A,B)/(C)"`, ...} -> {`"AB"`, `"C"`}
//      */
//     static auto lambda_parseAlt = [this](int _arg0, int _arg1) {
//         std::vector<std::string> splits;
//         if(_arg0>=IK_PATH::GCODE_Syntax.size()) throw std::invalid_argument(this->_info_name+"::lambda_parseAlt(int, int) _arg0 is bigger than available.");
//         if(_arg1>=IK_PATH::GCODE_Syntax.at(_arg0).size()) throw std::invalid_argument(this->_info_name+"::lambda_parseAlt(int, int) _arg1 is bigger than available.");
//         std::string toCheck = IK_PATH::GCODE_Syntax.at(_arg0).at(_arg1);
//         for(size_t _c=0; _c<toCheck.length(); _c++) {
//             char currChar = toCheck.at(_c);
//             if(currChar=='(') splits.push_back(std::string(""));
//             else if(
//                 currChar==',' || currChar=='/' || currChar==')'
//             ) continue;
//             else splits.at(splits.size()-1)+=toCheck.at(_c);
//         }
//         return splits;
//     };
//     static auto lambda_ignoreReturn = [](std::string& line, IK_PATH::GCODE_schedule* ptr) {
//         line = "";
//         ptr->_lastParsed_args = std::vector<std::string>{""};
//         return 1;
//     };
//     if(this->verbose) std::cout << this->_info_name<< "::_parse_line(std::string) splitString" << std::endl;
//     // the different gcode arguments/commands split from `line`
//     std::vector<std::string> args = splitString(line, " ");
//     if(args.at(0).length()==0) { //return an empty container because nothing was parsed
//         this->_parse_error_msg = "no args given. Args container is empty.";
//         return lambda_ignoreReturn(line, this);
//         // std::cout << "(1)";
//         // line = "";
//         // this->_lastParsed_args = std::vector<std::string>{""}; 
//         // return 1;
//     }
//     /**
//      * meaning of returned values: (values!=0 means non-successful parsing)
//      * `-1` - error: fatal: inccorrect syntax (non-existent codes were used)
//      * `0` - successful line parsing
//      * `1` - error: ignored line for various reasons.
//      */
//     int returnVal = 0;
//     if(this->verbose) std::cout << this->_info_name<< "::_parse_line(std::string)  while(gcode_additional_primary) loop start." << std::endl;
//     int plusIdx = 0; // iterating index to the elements of `args`
//     while(plusIdx<args.size() && gcode_additional_primary) {
//         int arg0_idx = this->_syntax_idx(args.at(plusIdx), &gcode_additional_primary, &gcode_optional_secondary);
//         if(arg0_idx<0) {
//             std::cout << "ERROR: "<<this->_info_name<<"::_parse_line(std::string&) argument \""<<args.at(plusIdx)<<"\" was not found as valid gCODE according to _syntax_idx()"<<std::endl;
//             return 1;
//         }
//         if(gcode_additional_primary && plusIdx+1>=args.size()) gcode_additional_primary = false;
//         // Number of arguments with primary and secondaries combined according to syntax:
//         // ex: {"G01", "(X,Y,Z)", "(I,J)"}.size()
//         size_t idx_syntax_size = IK_PATH::GCODE_Syntax.at(arg0_idx).size();
//         // std::cout << idx_syntax_size << " | ";
//         // if(verbose_debug) std::cout<<"2 "; std::cout.flush();
//         if(arg0_idx==-1) {
//             this->_parse_error_msg = "arg["+std::to_string(plusIdx)+"]: \""+args.at(plusIdx)+"\" does not exist in IK_PATH::GCODE_Syntax.";
//             return -1;
//         }
//         if((args.size()/*-plusIdx*/)<idx_syntax_size) {
//             if(!gcode_optional_secondary) {
//                 /// primary gcode argument/command has secondary arguments that are not optional and hasn't been given, meaning the given args doesn't match syntax requirement of args.
//                 this->_parse_error_msg = "input string of arguments does not contain minimum number of arguments for gcode code:\""+args.at(plusIdx)+"\".";
//                 return 1;
//             }
//             else {
//                 /// primary gcode argument/command has secondary arguments that ARE optional and not given, meaning it should be treated like a primary only argument meaning this is valid.
//             }
//         }
//         // args[0] found
//         size_t currentArgsLen_0 = 0;            // index position in `line` to current secondary argument position in for loop.
//         size_t currentArgsLen   = line.length();// length of `line` minux the currently iterated part index position in `currentArgsLen_0`
//         bool __temp = true;
//         static std::string _prevLine = "";
//         if(this->verbose_debug && idx_syntax_size==1) {
//             std::cout << formatNumber<std::string>(line, 35, 0, "left") << " | "<<0<<" | \"" << formatNumber<std::string>(IK_PATH::GCODE_Syntax.at(arg0_idx).at(0), 6, 0, "left")<<"\" | ";
//             std::cout << formatNumber<std::string>("{}", 10, 0, "left") << " | ";
//             std::cout << "gcode_addit..:"<<formatNumber<bool>(gcode_additional_primary, 5, 0, "left") <<" gcode_optio..:"<<formatNumber<bool>(gcode_optional_secondary, 5, 0, "left");
//             std::cout << std::endl;
//         }
//         // Searching for args[>0] matches
//         for(size_t i=1; i<idx_syntax_size && !(args.size()<idx_syntax_size && gcode_optional_secondary); i++) {
//             std::vector<std::string> _alt = lambda_parseAlt(arg0_idx, i); // ->{"AB", "C"}
//             if(this->verbose_debug) {
//                 std::cout << formatNumber<std::string>(line, 35, 0, "left") << " | "<<i<<" | \"" << formatNumber<std::string>(IK_PATH::GCODE_Syntax.at(arg0_idx).at(0), 6, 0, "left")<<"\" | ";
//                 std::cout << formatNumber<std::string>(formatContainer1<std::vector<std::string>>(_alt, _alt.size(), 0, 0, "left"), 10, 0, "left") << " | ";
//                 std::cout << "gcode_addit..:"<<formatNumber<bool>(gcode_additional_primary, 5, 0, "left") <<" gcode_optio..:"<<formatNumber<bool>(gcode_optional_secondary, 5, 0, "left");
//                 std::cout << std::endl;
//             }
//             //one two three four one2
//             currentArgsLen_0 = 0;
//             for(int ii=0; ii<parsed_words; ii++) {
//                 /// Using the number of gcode arguments/commands already "found" in line denoted by `parsed_words`, set `currentArgsLen_0` to the "current" position in `line`
//                 currentArgsLen_0+=args[ii].length()+1;
//             }
//             currentArgsLen  = line.length()-currentArgsLen_0;
//             __temp = false;
//             //Go through every secondary argument GCODE_syntax[arg0_idx]{}.
//             //This for() loop shouldn't occur for current gcodes with `+` symbol as their length is only 1.
//             // -in the future if I want to use `+` symbol for codes with parameters then I think I'll need to find string
//             // -start and end pos of those parameters associated with said code. (???)
//             int vecCount = 0;   // number of "groups" found from _alt
//             int _veciii = 0;    // index to currently checking element in _alt
//             int _i_altFound = 0;// index to found args in `_alt`
//             for(std::string _ii: _alt) { // `"ABC"` in {`"ABC"`, `"D"`}
//                 /// {"ABC", "D"}
//                 bool currFound = false;
//                 for(size_t _iii=0; _iii<_ii.length(); _iii++) { // index to each character in `_ii`
//                     /// iterate through char in _ii string
//                     if(line.substr(currentArgsLen_0, currentArgsLen).find(_ii.at(_iii)) != std::string::npos) {
//                         /// if "AB[C]D" in line.substr
//                         currFound = true;
//                         _i_altFound = _veciii;
//                         vecCount++;
//                         break;
//                     }
//                 }
//                 if(currFound) {
//                     /// Iterate through the accompanying letters/char's to make sure all the ones included has number/integer values.
//                     for(size_t _iii=0; _iii<_ii.length(); _iii++) {
//                         size_t line_charPos = 0;
//                         if((line_charPos = line.substr(currentArgsLen_0, currentArgsLen).find(_ii.at(_iii))) != std::string::npos) {
//                             try {
//                                 std::stoi(line.substr(currentArgsLen_0+line_charPos+1, currentArgsLen));
//                             }
//                             catch(const std::exception& e) {
//                                 this->_parse_error_msg = "secondary arguments do not contain necessary number/integer values for: \""+std::string(1, _ii.at(_iii))+"\".";
//                                 return -1;
//                             }
//                         }
//                     }
//                 }
//                 _veciii++;
//             }
//             if(vecCount==0) {
//                 this->_parse_error_msg = "arguments to code \""+args.at(plusIdx)+"\" does not contain any of the obligatory args: \""+GCODE_Syntax[arg0_idx][i]+"\".";
//                 return -1;
//             }
//             else if(vecCount>=2) {
//                 this->_parse_error_msg = "arguments to code \""+args[plusIdx]+"\" contain either both of types not allowed to co-exist or has same arg repeated.";
//                 return -1;
//             }
//             for(int _i_c=0; _i_c<_alt[_i_altFound].length(); _i_c++) { // index to currently searching character in _alt[_i_altFound], i.e. idx to char in "AB" from {"AB", "C"}
//                 /// Iterate through each letter in _alt[_i_altFound]. ex: "AB" in {"AB", "C"}
//                 if(line.substr(currentArgsLen_0, currentArgsLen).find(_alt.at(_i_altFound).at(_i_c))!=std::string::npos) {
//                     /// If the currently searching char from _alt.at(_i_altFound) is found in `line`, count up `parsed_words`
//                     parsed_words++;
//                     // currentArgsLen_0++;
//                 }
//             }
//             // parsed_words += _alt[_i_altFound].length();
//             // parsed_words++;
//         }
//         // if(__temp) parsed_words++;
//         // parsed_words+=idx_syntax_size;
//         if(gcode_additional_primary && idx_syntax_size<=1) parsed_words++;
//         plusIdx++;
//     }
//     std::string newStr = "";
//     for(std::string arg: args) newStr+=arg+" ";
//     line = newStr;
//     // if(verbose_debug) std::cout<<"4 "; std::cout.flush();
//     // // filter out comments and whatnot and store that in _lastParsed_args.
//     // std::cout <<">> "<< parsed_words << std::endl;
//     std::vector<std::string> tempVec;
//     for(int i=0; i<parsed_words; i++) tempVec.push_back(args.at(i));
//     this->_lastParsed_args = tempVec; 
//     // std::cout << std::endl;
//     return returnVal;
// }

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

bool IK_PATH::GCODE_schedule::loadFile(std::string filename) {
    std::ifstream readFile;
    readFile.open(filename);
    if(!readFile.is_open()) {
        if(this->verbose) std::cout << "Could not open filename: \""+filename+"\""<<std::endl;
        return false;
    }
    if(this->verbose) std::cout << this->_info_name<< "::loadFile(std::string) opened file: \""<<filename<<"\"" << std::endl;

    if(this->verbose) std::cout << this->_info_name<< "::loadFile(std::string) parsing file..\n" <<std::endl;
    std::string line;
    try {
        for(int i=1; getline(readFile, line); i++) {
            // if(this->verbose) std::cout << this->_info_name<< "::loadFile(std::string) i:"<<i<<" parsed: \""<<line<<"\" "; std::cout.flush();
            if(this->verbose) std::cout << std::setw(35) << std::left << line << " | "; std::cout.flush();
            int add_returCode = 0;
            if((add_returCode = this->add(line))) {
                // if(this->verbose) std::cout << "failed to read line: "<<i<<std::endl;
                if(add_returCode==-1) return false;
            }
        }
    }
    catch(const std::exception& e) {
        std::cout<< this->_info_name<<"::loadFile(std::string) ERROR: " << e.what() << std::endl;
        return false;
    }
    if(verbose || verbose_debug) std::cout << std::endl;
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
    // if(this->verbose) std::cout << this->_info_name<< "::add(std::string) newCommand: \""<<newCommand<<"\""<<std::endl;
    try {
        int _parse_line_returCode = 0;
        if((_parse_line_returCode = this->_parse_line(newCommand))) {
            std::cout << "ERROR:"<<std::setw(3)<<_parse_line_returCode<<"  "<<this->_info_name<<"::_parse_line(std::string&): "<< _parse_error_msg << std::endl;
            // if(this->verbose) std::cout << "IK_PATH::GCODE_schedule::add(std::string): "+this->_parse_error_msg << std::endl;
            return _parse_line_returCode;
        }
        if(this->verbose) std::cout << std::endl;
    }
    catch(const std::exception& e) {
        if(this->verbose) std::cout << this->_info_name << "::add(std::string) --> _parse_line(std::string) failed with arg: \""<<newCommand<<"\": "<<e.what()<<std::endl;
        return 1;
    }
    this->_commands.push_back(this->_lastParsed_args);
    this->_commands_raw.push_back(newCommand);
    this->_commands_orig.push_back(this->_lastArgs_unparsed);
    
    return 0;
}
int IK_PATH::GCODE_schedule::insert(size_t idx, std::string newCommand) {
    if(idx>=this->_commands.size()) throw std::runtime_error(this->_info_name+"::insert(size_t, std::string) input index is bigger than stored commands");

    try {
        int _parse_line_returCode = 0;
        if((_parse_line_returCode = this->_parse_line(newCommand))) {
            std::cout << "ERROR:"<<std::setw(3)<<_parse_line_returCode<<"  "<<this->_info_name<<"::_parse_line(std::string&): "<< _parse_error_msg << std::endl;
            // if(this->verbose) std::cout << "IK_PATH::GCODE_schedule::add(std::string): "+this->_parse_error_msg << std::endl;
            return _parse_line_returCode;
        }
    }
    catch(const std::exception& e) {
        if(this->verbose) std::cout << this->_info_name << "insert(size_t, std::string) --> _parse_line(std::string) failed with arg: \""<<newCommand<<"\": "<<e.what()<<std::endl;
        return 1;
    }

    this->_commands.insert(this->_commands.begin()+idx, this->_lastParsed_args);
    this->_commands_raw.insert(this->_commands_raw.begin()+idx, newCommand);
    this->_commands_orig.insert(this->_commands_orig.begin()+idx, this->_lastArgs_unparsed);
    
    return 0;
}
int IK_PATH::GCODE_schedule::replace(size_t idx, std::string newCommand) {
    if(idx>=this->_commands.size()) throw std::runtime_error(this->_info_name+"::replace(size_t, std::string) input index is bigger than stored commands");
    try {
        int _parse_line_returCode = 0;
        if((_parse_line_returCode = this->_parse_line(newCommand))) {
            std::cout << "ERROR:"<<std::setw(3)<<_parse_line_returCode<<"  "<<this->_info_name<<"::_parse_line(std::string&): "<< _parse_error_msg << std::endl;
            // if(this->verbose) std::cout << "IK_PATH::GCODE_schedule::add(std::string): "+this->_parse_error_msg << std::endl;
            return _parse_line_returCode;
        }
    }
    catch(const std::exception& e) {
        if(this->verbose) std::cout << this->_info_name << "add(std::string) --> _parse_line(std::string) failed with arg: \""<<newCommand<<"\": "<<e.what()<<std::endl;
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


int IK_PATH::GCODE_schedule::runSchedule(TDEF_runCode funcToCall) {
    /**
     * Parsed line interpretation
     * Constants:
     * - {"X", "Y", "Z"}
     * 
     */
    int returCode = 0;

    static auto valueIndexer = [](char letter) {
        int idx;
        switch (letter) {
            case 'U': idx = 0; break;
            case 'V': idx = 1; break;
            case 'W': idx = 2; break;
            case 'X': idx = 3; break;
            case 'Y': idx = 4; break;
            case 'Z': idx = 5; break;
            default: break;
        }
        return idx;
    };

    float new_coord[3]  = {this->ref_pos[0], this->ref_pos[1], this->ref_pos[2]};
    float nwe_orient[3] = {this->ref_tilt[0], this->ref_tilt[1], this->ref_tilt[2]};

    ///@brief {U, V, W, X, Y, Z}
    float sendValues[6];

    int arg_idx = 0;
    for(size_t i=0; i<this->_commands.size(); i++) {
        std::vector<std::string> tmp = this->_commands[i];
        if(tmp[arg_idx]=="G00") {
            
        }
        for(std::string arg: tmp) {
            if(arg=="G00") {

            }



        }

        
    }

    return returCode;
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

