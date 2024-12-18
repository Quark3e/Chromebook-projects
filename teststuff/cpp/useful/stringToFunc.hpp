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
    /// @brief default constructor
    StringToFunction();
    /**
     * @brief call stored function relative to name
     * @param name the string variable to find appropriate function of
     * @return `0`- function succesfully called; `1`- `name` related function not found
    */
    int call_func(std::string name);

    /// @brief print available printTypes
    void availableTypes();

    std::string exportString(
        bool clearScr,
        bool useLabel,
        std::string lineEnd,
        std::string lineSep
    );

    int add_func(std::string name, TDEF_void__ func, std::string name2="", std::string description="");
    int add_func(std::string name, TDEF_int__  func, std::string name2="", std::string description="");
};


#endif