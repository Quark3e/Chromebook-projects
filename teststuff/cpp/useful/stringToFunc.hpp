
#pragma once
#ifndef HPP_STRING_TO_FUNC
#define HPP_STRING_TO_FUNC

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "useful.hpp"

class StringToFunction {
    typedef void    (*TDEF_void__)();
    typedef int     (*TDEF_int__)();

    std::string allFuncTypes[2] = {
        "void__",
        "int__"
    };

    std::vector<TDEF_void__>    funcVec_TDEF_void__;
    std::vector<TDEF_int__>     funcVec_TDEF_int__;


    /**
     * @brief call `*TDEF_void__` typedef function
     * @param name string associated with the function
    */
    void call_void__(std::string name);
    /**
     * @brief call `*TDEF_int__` typedef function
     * @param name string associated with the function
    */
    int call_int__(std::string name);


    /// @brief container each name respective to function
    std::vector<std::string> names;
    /// @brief container for what type of vector
    std::vector<std::string> funcType;
    /// @brief container for index of name in their respective function pointer vector
    std::vector<size_t> indices;

    public:
    StringToFunction();

    int call_func(std::string name);

    /// @brief print available printTypes
    void availableTypes();


    int add_func(std::string name, TDEF_void__ func);
    int add_func(std::string name, TDEF_int__ func);
};

void StringToFunction::call_void__(std::string name) {
    funcVec_TDEF_void__[indices[findVectorIndex(names, name)]]();
}
int StringToFunction::call_int__(std::string name) {
    funcVec_TDEF_int__[indices[findVectorIndex(names, name)]]();
}

/// @brief default constructor
StringToFunction::StringToFunction() {}

int StringToFunction::call_func(std::string name) {
    if(findVectorIndex(names, name) == -1) {
        std::cout << "ERROR: StringToFunction::add_func: name \""<<name<<"\" doesn't exists. Pick a new one"<<endl;
        return 1;
    }

    if(funcType[findVectorIndex(names,name)]==allFuncTypes[0]) call_void__(name);
    else if(funcType[findVectorIndex(names,name)]==allFuncTypes[1]) call_int__(name);

    return 0;
}

void StringToFunction::availableTypes() {
    for(int i=0; i<sizeof(allFuncTypes)/sizeof(std::string); i++) {
        std::cout<<"-\""<<allFuncTypes[i]<<"\""<< std::endl;
    }
}


int StringToFunction::add_func(
    std::string name,
    TDEF_void__ func
) {
    if(findVectorIndex(names, name) != -1) {
        std::cout << "ERROR: StringToFunction::add_func: name \""<<name<<"\" already exists. Pick a new one"<<endl;
        return 1;
    }
    indices.push_back(funcVec_TDEF_void__.size());
    funcVec_TDEF_void__.push_back(func);
    funcType.push_back(allFuncTypes[0]);

    return 0;
}
int StringToFunction::add_func(
    std::string name,
    TDEF_int__ func
) {
    if(findVectorIndex(names, name) != -1) {
        std::cout << "ERROR: StringToFunction::add_func: name \""<<name<<"\" already exists. Pick a new one"<<endl;
        return 1;
    }
    indices.push_back(funcVec_TDEF_int__.size());
    funcVec_TDEF_int__.push_back(func);
    funcType.push_back(allFuncTypes[0]);

    return 0;
}


#endif