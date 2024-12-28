
#include "stringToFunc.hpp"


void StringToFunction::call_void__(std::string name) {
    funcVec_TDEF_void__[indices[findVectorIndex(names, name)[0]]]();
}
int StringToFunction::call_int__(std::string name) {
    return funcVec_TDEF_int__[indices[findVectorIndex(names, name)[0]]]();
}

StringToFunction::StringToFunction() {

}

int StringToFunction::call_func(std::string name) {
    std::vector<int> vecIdx = findVectorIndex(names, name);
    if(findVectorIndex(names, name)[0] == -1) {
        std::cout << "ERROR: StringToFunction::call_func: name \""<<name<<"\" doesn't exists. Pick a new one"<<std::endl;
        return 1;
    }

    if(funcType[findVectorIndex(names,name)[1]]==allFuncTypes[0]) call_void__(name);
    else if(funcType[findVectorIndex(names,name)[1]]==allFuncTypes[1]) call_int__(name);

    return 0;
}

void StringToFunction::availableTypes() {
    for(int i=0; i<sizeof(allFuncTypes)/sizeof(std::string); i++) {
        std::cout<<"-\""<<allFuncTypes[i]<<"\""<< std::endl;
    }
}


int StringToFunction::add_func(
    std::string name,
    TDEF_void__ func,
    std::string name2,
    std::string description
) {
    std::vector<std::string> nameToFind(2, "");
    nameToFind[0] = name;
    nameToFind[1] = name2;
    if(findVectorIndex(names, nameToFind)[1] != -1) {
        std::cout << "ERROR: StringToFunction::add_func: name \""<<name<<"\" already exists. Pick a new one"<<std::endl;
        return 1;
    }
    // cout << "start"<<std::endl;
    names.push_back(std::vector<std::string>(2, ""));
    names[names.size()-1][0] = name;
    names[names.size()-1][1] = name2;
   
    indices.push_back(funcVec_TDEF_void__.size());
    funcVec_TDEF_void__.push_back(func);
    funcType.push_back(allFuncTypes[0]);
    descriptions.push_back(description);

    return 0;
}
int StringToFunction::add_func(
    std::string name,
    TDEF_int__  func,
    std::string name2,
    std::string description
) {
    if(findVectorIndex(names, name)[1] != -1) {
        std::cout << "ERROR: StringToFunction::add_func: name \""<<name<<"\" already exists. Pick a new one"<<std::endl;
        return 1;
    }

    names.push_back(std::vector<std::string>(2, ""));
    names[names.size()-1][0] = name;
    names[names.size()-1][1] = name2;

    indices.push_back(funcVec_TDEF_int__.size());
    funcVec_TDEF_int__.push_back(func);
    funcType.push_back(allFuncTypes[0]);
    descriptions.push_back(description);

    return 0;
}


std::string StringToFunction::exportString(
    bool clearScr,
    bool useLabel = true,
    std::string lineEnd = "\n",
    std::string lineSep = " "
) {
    std::string exportStr="";

    return exportStr;
}



void    labeledFunction::call_void__(size_t _idx) {
    assert(_idx<this->funcVec_TDEF_void__.size());
    this->funcVec_TDEF_void__.at(_idx)();
}
int     labeledFunction::call_int__(size_t _idx) {
    assert(_idx<this->funcVec_TDEF_int__.size());
    return this->funcVec_TDEF_int__.at(_idx)();
}
bool    labeledFunction::call_bool__(size_t _idx) {
    assert(_idx<this->funcVec_TDEF_bool__.size());
    return this->funcVec_TDEF_bool__.at(_idx)();
}
std::string labeledFunction::call_string__(size_t _idx) {
    assert(_idx<this->funcVec_TDEF_string__.size());
    return this->funcVec_TDEF_string__.at(_idx)();
}

int labeledFunction::get_idx(std::string _name) {
    int idx = -1;
    for(size_t i=0; i<this->_names.size(); i++) {
        for(size_t ii=0; ii<this->_names.at(i).size(); ii++) {
            if(_names.at(i).at(ii)==_name) return static_cast<int>(this->_indices.at(i));
        }
    }
    return idx;
}

labeledFunction::labeledFunction() {

}


int labeledFunction::call_func(std::string _name) {
    int idx = this->get_idx(_name);
    if(idx<0) return -1;

    if(_funcType[idx]==allFuncTypes[0])                                this->call_void__(idx);
    else if(_funcType[idx]==allFuncTypes[1]) this->_returns._int     = this->call_int__(idx);
    else if(_funcType[idx]==allFuncTypes[2]) this->_returns._bool    = this->call_bool__(idx);
    else if(_funcType[idx]==allFuncTypes[3]) this->_returns._string  = this->call_string__(idx);
    return 0;
}


int labeledFunction::add_func(std::vector<std::string> _labels, TDEF_void__  _func, std::string _description) {
    int idx = -1;
    for(size_t _n=0; _n<_labels.size(); _n++) {
        idx = this->get_idx(_labels.at(_n));
        if(idx != -1) return -1;
    }
    _names.push_back(_labels);
    _indices.push_back(funcVec_TDEF_void__.size());
    funcVec_TDEF_void__.push_back(_func);
    _funcType.push_back(allFuncTypes[0]);
    _descriptions.push_back(_description);
    return 0;
}
int labeledFunction::add_func(std::vector<std::string> _labels, TDEF_int__   _func, std::string _description) {
    int idx = -1;
    for(size_t _n=0; _n<_labels.size(); _n++) {
        idx = this->get_idx(_labels.at(_n));
        if(idx != -1) return -1;
    }
    _names.push_back(_labels);
    _indices.push_back(funcVec_TDEF_int__.size());
    funcVec_TDEF_int__.push_back(_func);
    _funcType.push_back(allFuncTypes[1]);
    _descriptions.push_back(_description);
    return 0;
}
int labeledFunction::add_func(std::vector<std::string> _labels, TDEF_bool__  _func, std::string _description) {
    int idx = -1;
    for(size_t _n=0; _n<_labels.size(); _n++) {
        idx = this->get_idx(_labels.at(_n));
        if(idx != -1) return -1;
    }
    _names.push_back(_labels);
    _indices.push_back(funcVec_TDEF_bool__.size());
    funcVec_TDEF_bool__.push_back(_func);
    _funcType.push_back(allFuncTypes[2]);
    _descriptions.push_back(_description);
    return 0;
}
int labeledFunction::add_func(std::vector<std::string> _labels, TDEF_string__ _func,std::string _description) {
    int idx = -1;
    for(size_t _n=0; _n<_labels.size(); _n++) {
        idx = this->get_idx(_labels.at(_n));
        if(idx != -1) return -1;
    }
    _names.push_back(_labels);
    _indices.push_back(funcVec_TDEF_string__.size());
    funcVec_TDEF_string__.push_back(_func);
    _funcType.push_back(allFuncTypes[3]);
    _descriptions.push_back(_description);
    return 0;
}