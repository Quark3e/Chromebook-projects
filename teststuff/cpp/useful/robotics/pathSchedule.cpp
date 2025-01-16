
#include "pathSchedule.hpp"
#include <useful.hpp>


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

int IK_PATH::GCODE_schedule::_syntax_idx(std::string arg, bool *gcode_additional_primary, bool *gcode_optional_secondary) {
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
        else if(arg[0]==_codeLine[0][0] && (arg[0]!='G' && arg[0]!='M')) {
            /// Single char matching: find special cases where arg[0] matches first char of special cases (like F{INT})
            
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
    if(idx_firstFound!=-1) {
        std::cout << "\t {"<<arg<<","<<idx_firstFound<<"}"<<std::endl;
        return idx_firstFound;
    }
    // std::cout << "|_syntax_idx()->-1|"<<std::endl;
    return -1;
}

int IK_PATH::GCODE_schedule::_parse_line(std::string &line) {

    this->_lastArgs_unparsed = line;
    bool gcode_additional_primary = true; // whether this primary allows another primary gcode argument (`+` symbol if true, otherwise false)
    bool gcode_optional_secondary = true; // whether the secondary arguments are optional (`|` symbol if true, otherwise false)
    int parsed_words = 1;

    if(this->verbose) std::cout << this->_info_name<< "::_parse_line(std::string) function call:\""<<line<<"\"" << std::endl;

    /**
     * @brief parse the required code letters from IK_PATH::GCODE_Syntax.
     * @param _arg0 index to line in IK_PATH::GCODE_Syntax
     * @param _arg1 index to element in line of `_arg0` vector
     * @return `std::vector<std::string>` of the letters of codes:
     * @note example: `"(A,B)/(C)"` -> {`"AB"`, `"C"`}
     */
    static auto lambda_parseAlt = [](int _arg0, int _arg1) {
        std::vector<std::string> splits;
        if(_arg0>=IK_PATH::GCODE_Syntax.size() || _arg1>=IK_PATH::GCODE_Syntax.at(_arg0).size()) return splits;
        std::string toCheck = IK_PATH::GCODE_Syntax.at(_arg0).at(_arg1);

        for(size_t _c=0; _c<toCheck.length(); _c++) {
            char currChar = toCheck.at(_c);
            if(currChar=='(') splits.push_back(std::string(""));
            else if(
                currChar==',' || currChar=='/' || currChar==')'
            ) continue;
            else splits.at(splits.size()-1)+=toCheck.at(_c);
        }
        return splits;
    };
    static auto lambda_ignoreReturn = [](std::string& line, IK_PATH::GCODE_schedule* ptr) {
        line = "";
        ptr->_lastParsed_args = std::vector<std::string>{""};
        return 1;
    };
    
    if(this->verbose) std::cout << this->_info_name<< "::_parse_line(std::string) splitString" << std::endl;
    // the different gcode arguments/commands split from `line`
    std::vector<std::string> args = splitString(line, " ");
    if(args.at(0).length()==0) { //return an empty container because nothing was parsed
        this->_parse_error_msg = "no args given. Args container is empty.";
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

    if(this->verbose) std::cout << this->_info_name<< "::_parse_line(std::string)  while(gcode_additional_primary) loop start." << std::endl;
    int plusIdx = 0; // iterating index to the elements of `args`
    while(plusIdx<args.size() && gcode_additional_primary) {
        // if(verbose_debug) std::cout<<"0 "; std::cout.flush();
        int arg0_idx = this->_syntax_idx(args.at(plusIdx), &gcode_additional_primary, &gcode_optional_secondary);
        if(arg0_idx<0) {
            std::cout << "ERROR: "<<this->_info_name<<"::_parse_line(std::string) argument \""<<args.at(plusIdx)<<"\" was not found as valid gCODE according to _syntax_idx()"<<std::endl;
            return 1;
        }
        if(gcode_additional_primary && plusIdx+1>=args.size()) gcode_additional_primary = false;
        // if(verbose_debug) std::cout<<"1:"<<arg0_idx << " "; std::cout.flush();

        // Number of arguments with primary and secondaries combined according to syntax:
        // ex: {"G01", "(X,Y,Z)", "(I,J)"}.size()
        size_t idx_syntax_size = IK_PATH::GCODE_Syntax.at(arg0_idx).size();

        // std::cout << idx_syntax_size << " | ";
        // if(verbose_debug) std::cout<<"2 "; std::cout.flush();
        if(arg0_idx==-1) {
            this->_parse_error_msg = "arg["+std::to_string(plusIdx)+"]: \""+args.at(plusIdx)+"\" does not exist in IK_PATH::GCODE_Syntax.";
            return -1;
        }
        if((args.size()/*-plusIdx*/)<idx_syntax_size) {
            if(!gcode_optional_secondary) {
                /// primary gcode argument/command has secondary arguments that are not optional and hasn't been given, meaning the given args doesn't match syntax requirement of args.
                this->_parse_error_msg = "input string of arguments does not contain minimum number of arguments for gcode code:\""+args.at(plusIdx)+"\".";
                return 1;
            }
            else {
                /// primary gcode argument/command has secondary arguments that ARE optional and not given, meaning it should be treated like a primary only argument meaning this is valid.

            }
        }
        // args[0] found

        int currentArgsLen_0= 0;
        int currentArgsLen  = line.length();

        bool __temp = true;

        static std::string _prevLine = "";
        if(idx_syntax_size==1) {
            std::cout << formatNumber<std::string>(line, 35, 0, "left") << " | "<<0<<" | " << formatNumber<std::string>("{}", 10, 0, "left") << " | ";
            std::cout << "gcode_addit..:"<<formatNumber<bool>(gcode_additional_primary, 5, 0, "left") <<" gcode_optio..:"<<formatNumber<bool>(gcode_optional_secondary, 5, 0, "left");
            std::cout << std::endl;
            
        }

        // Searching for args[>0] matches
        for(size_t i=1; i<idx_syntax_size && !(args.size()<idx_syntax_size && gcode_optional_secondary); i++) {
            // if(verbose_debug) std::cout<<"3.0 "; std::cout.flush();
            std::vector<std::string> _alt = lambda_parseAlt(arg0_idx, i); // ->{"AB", "C"}
            std::cout << formatNumber<std::string>(line, 35, 0, "left") << " | "<<i<<" | " << formatNumber<std::string>(formatContainer1<std::vector<std::string>>(_alt, _alt.size(), 0, 0, "left"), 10, 0, "left") << " | ";
            std::cout << "gcode_addit..:"<<formatNumber<bool>(gcode_additional_primary, 5, 0, "left") <<" gcode_optio..:"<<formatNumber<bool>(gcode_optional_secondary, 5, 0, "left");
            std::cout << std::endl;
            //one two three four one2

            // currentArgsLen_0= 0;
            // if(verbose_debug) std::cout<<"3.1 "; std::cout.flush();
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
            // if(verbose_debug) std::cout<<"3.2 "; std::cout.flush();
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
                this->_parse_error_msg = "arguments to code \""+args.at(plusIdx)+"\" does not contain any of the obligatory args: \""+GCODE_Syntax[arg0_idx][i]+"\".";
                return -1;
            }
            else if(vecCount>=2) {
                this->_parse_error_msg = "arguments to code \""+args[plusIdx]+"\" contain either both of types not allowed to co-exist or has same arg repeated.";
                return -1;
            }
            // if(verbose_debug) std::cout<<"3.3 "; std::cout.flush();
            for(int _i_c=0; _i_c<_alt[_i_alt].length(); _i_c++) {
                if(line.substr(currentArgsLen_0, currentArgsLen).find(_alt.at(_i_alt).at(_i_c))!=std::string::npos) {
                    parsed_words++;
                    // currentArgsLen_0++;
                }
            }
            
            // parsed_words += _alt[_i_alt].length();
            // parsed_words++;
        }
        // if(__temp) parsed_words++;
        // parsed_words+=idx_syntax_size;
        // if(tempPrint) std::cout << parsed_words<<std::boolalpha<<gcode_additional_primary<<std::endl;
        if(gcode_additional_primary && idx_syntax_size<=1) parsed_words++;
        plusIdx++;
    }
    std::string newStr = "";
    for(std::string arg: args) newStr+=arg+" ";
    line = newStr;
    // if(verbose_debug) std::cout<<"4 "; std::cout.flush();
    // // filter out comments and whatnot and store that in _lastParsed_args.
    // std::cout <<">> "<< parsed_words << std::endl;
    std::vector<std::string> tempVec;
    for(int i=0; i<parsed_words; i++) tempVec.push_back(args.at(i));
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
            if(this->verbose) std::cout << this->_info_name<< "::loadFile(std::string) i:"<<i<<" parsed: \""<<line<<"\""<<std::endl;
            int add_returCode = 0;
            if((add_returCode = this->add(line))) {
                if(this->verbose) std::cout << "failed to read line: "<<i<<std::endl;
                if(add_returCode==-1) return false;
            }
            // std::cout << "- - - - -"<<std::endl;
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
    if(this->verbose) std::cout << this->_info_name<< "::add(std::string) newCommand: \""<<newCommand<<"\""<<std::endl;
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

