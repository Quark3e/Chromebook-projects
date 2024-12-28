#pragma once
#ifndef HPP_STRING_TO_FUNC
#define HPP_STRING_TO_FUNC

#include <iostream>
#include <string>
#include <vector>
#include <initializer_list>
#include <algorithm>

#include <useful.hpp>

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

    /// @brief Not finished
    /// @param clearScr 
    /// @param useLabel 
    /// @param lineEnd 
    /// @param lineSep 
    /// @return 
    std::string exportString(
        bool clearScr,
        bool useLabel,
        std::string lineEnd,
        std::string lineSep
    );

    virtual int add_func(std::string name, TDEF_void__ func, std::string name2="", std::string description="");
    virtual int add_func(std::string name, TDEF_int__  func, std::string name2="", std::string description="");
};


class labeledFunction {
    typedef void        (*TDEF_void__)();
    typedef int         (*TDEF_int__)();
    typedef bool        (*TDEF_bool__)();
    typedef std::string (*TDEF_string__)();

    std::string allFuncTypes[4] = {
        "void__",
        "int__",
        "bool__",
        "string__"
    };
    std::vector<TDEF_void__>    funcVec_TDEF_void__;
    std::vector<TDEF_int__>     funcVec_TDEF_int__;
    std::vector<TDEF_bool__>    funcVec_TDEF_bool__;
    std::vector<TDEF_string__>  funcVec_TDEF_string__;

    void        call_void__(size_t _idx);
    int         call_int__(size_t _idx);
    bool        call_bool__(size_t _idx);
    std::string call_string__(size_t _idx);


    /// @brief storage for the vector of containers attached to a certain function.
    std::vector<std::vector<std::string>> _names;
    /// @brief type of the function stored for given index, limited to `allFuncTypes`
    std::vector<std::string> _funcType;
    /// @brief description for each name
    std::vector<std::string> _descriptions;
    /// @brief relative indices for the respective TDEF std::vector container according to this vectors container
    std::vector<size_t> _indices;

    /// @brief get the stored index for a given name
    /// @param _name the `std::string` name to find the index for
    /// @return int of the index. If an index isn't found then it'll return `-1`
    int get_idx(std::string _name);

    struct _typed_returns {
        int         _int    = 0;
        bool        _bool   = false;
        std::string _string = "";
    };
    
    public:
    std::vector<std::vector<std::string>>   getNames() { return _names; }
    std::vector<std::string>                getFuncTypes() { return _funcType; }
    std::vector<std::string>                getDescriptions() { return _descriptions; }

    /// @brief struct holding return values of the called functions
    _typed_returns _returns;

    labeledFunction();
    // labeledFunction(std::vector<std::string> _labels, TDEF_void__ _func);
    // labeledFunction(std::vector<std::string> _labels, TDEF_void__ _func, std::string _description="");
    // labeledFunction(std::initializer_list<std::string> _labels, TDEF_void__ _func);
    // labeledFunction(std::initializer_list<std::string> _labels, TDEF_void__ _func, std::string _description="");
    // labeledFunction(std::vector<std::vector<std::string>> _labels, std::vector<TDEF_void__> _functions);
    // labeledFunction(std::vector<std::vector<std::string>> _labels, std::vector<TDEF_void__> _functions, std::vector<std::string> _descriptions);
    // labeledFunction(std::initializer_list<std::initializer_list<std::string>> _labels, std::vector<TDEF_void__> _functions);
    // labeledFunction(std::initializer_list<std::initializer_list<std::string>> _labels, std::vector<TDEF_void__> _functions, std::initializer_list<std::string> _descriptions);
    // labeledFunction(std::vector<std::string> _labels, TDEF_int__ _func);
    // labeledFunction(std::vector<std::string> _labels, TDEF_int__ _func, std::string _description="");
    // labeledFunction(std::initializer_list<std::string> _labels, TDEF_int__ _func);
    // labeledFunction(std::initializer_list<std::string> _labels, TDEF_int__ _func, std::string _description="");
    // labeledFunction(std::vector<std::vector<std::string>> _labels, std::vector<TDEF_int__> _functions);
    // labeledFunction(std::vector<std::vector<std::string>> _labels, std::vector<TDEF_int__> _functions, std::vector<std::string> _descriptions);
    // labeledFunction(std::initializer_list<std::initializer_list<std::string>> _labels, std::vector<TDEF_int__> _functions);
    // labeledFunction(std::initializer_list<std::initializer_list<std::string>> _labels, std::vector<TDEF_int__> _functions, std::initializer_list<std::string> _descriptions);
    
    /// @brief call the function stored under given _name
    /// @param _name `std::string` name to call the function of
    /// @return int code for whether the call was successufl  [`0`] or not [`!=0`]
    int call_func(std::string _name);

    /// @brief add a function to the storage
    /// @param _labels `std::vector` of the label(s) to attach with given function
    /// @param _func `void (*)()` function to relate with label(s)
    /// @param _description description to attach to this connection
    /// @return int code for whether successful [`0`] or not [`-1`]
    int add_func(std::vector<std::string> _labels, TDEF_void__  _func, std::string _description="");
    /// @brief add a function to the storage
    /// @param _labels `std::vector` of the label(s) to attach with given function
    /// @param _func `int (*)()` function to relate with label(s)
    /// @param _description description to attach to this connection
    /// @return int code for whether successful [`0`] or not [`-1`]
    int add_func(std::vector<std::string> _labels, TDEF_int__   _func, std::string _description="");
    /// @brief add a function to the storage
    /// @param _labels `std::vector` of the label(s) to attach with given function
    /// @param _func `bool (*)()` function to relate with label(s)
    /// @param _description description to attach to this connection
    /// @return int code for whether successful [`0`] or not [`-1`]
    int add_func(std::vector<std::string> _labels, TDEF_bool__  _func, std::string _description="");
    /// @brief add a function to the storage
    /// @param _labels `std::vector` of the label(s) to attach with given function
    /// @param _func `std::string (*)()` function to relate with label(s)
    /// @param _description description to attach to this connection
    /// @return int code for whether successful [`0`] or not [`-1`]
    int add_func(std::vector<std::string> _labels, TDEF_string__ _func,std::string _description="");
};


#endif