#pragma once
#ifndef HPP_STRING_TO_FUNC
#define HPP_STRING_TO_FUNC

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <HC_useful/useful.hpp>

class StringToFunction {
    typedef void    (*TDEF_void__)();
    typedef int     (*TDEF_int__)();

    std::string allFuncTypes[2] = {
        "void__",
        "int__"
    };
    public:
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


    public:
    /// @brief container each name respective to function
    std::vector<std::vector<std::string>> names;
    /// @brief container for what type of vector
    std::vector<std::string> funcType;
    /// @brief container for descriptions of each function
    std::vector<std::string> descriptions;
    /// @brief container for index of name in their respective function pointer vector
    std::vector<size_t> indices;

    // public:
    std::vector<std::vector<std::string>> getNames() { return names; }
    std::vector<std::string> getFuncTypes() { return funcType; }
    std::vector<std::string> getDescriptions() { return descriptions; }

    StringToFunction();

    int call_func(std::string name);

    /// @brief print available printTypes
    void availableTypes();

    std::string exportString(
        bool clearScr,
        bool useLabel,
        std::string lineEnd,
        std::string lineSep
    );

    int add_func(std::string name, TDEF_void__ func, std::string name2, std::string description);
    int add_func(std::string name, TDEF_int__ func, std::string name2, std::string description);
};

void StringToFunction::call_void__(std::string name) {
    funcVec_TDEF_void__[indices[findVectorIndex(names, name)[0]]]();
}
int StringToFunction::call_int__(std::string name) {
    return funcVec_TDEF_int__[indices[findVectorIndex(names, name)[0]]]();
}

/// @brief default constructor
StringToFunction::StringToFunction() {

}

/**
 * @brief call stored function relative to name
 * @param name the string variable to find appropriate function of
 * @return `0`- function succesfully called; `1`- `name` related function not found
*/
int StringToFunction::call_func(std::string name) {
    std::vector<int> vecIdx = findVectorIndex(names, name);
    if(findVectorIndex(names, name)[0] == -1) {
        std::cout << "ERROR: StringToFunction::call_func: name \""<<name<<"\" doesn't exists. Pick a new one"<<endl;
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
    std::string name2 = "",
    std::string description = ""
) {
    std::vector<std::string> nameToFind(2, "");
    nameToFind[0] = name;
    nameToFind[1] = name2;
    if(findVectorIndex(names, nameToFind)[1] != -1) {
        std::cout << "ERROR: StringToFunction::add_func: name \""<<name<<"\" already exists. Pick a new one"<<endl;
        return 1;
    }
    // cout << "start"<<endl;
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
    TDEF_int__ func,
    std::string name2 = "",
    std::string description = ""
) {
    if(findVectorIndex(names, name)[1] != -1) {
        std::cout << "ERROR: StringToFunction::add_func: name \""<<name<<"\" already exists. Pick a new one"<<endl;
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
    std::string exportStr;

}

#endif