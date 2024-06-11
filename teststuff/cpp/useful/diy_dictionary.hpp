#pragma once
#ifndef HPP_DIY_DICTIONARY
#define HPP_DIY_DICTIONARY


/**
 * @file diy_dictionary.hpp
 * @author your name (you@domain.com)
 * @brief a basic "dictionary" esque storage class
 * @version 0.1
 * @date 2024-06-05
 * 
 * @copyright Copyright (c) 2024
 * 
 * @note
 * Might need to look into pointer storage and heap allocation/de-allocation with smart
 * pointers instead of having an entire set of "value" only storage
 */

#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <iomanip>
#include <stdexcept>
#include <vector>

// #include <HC_useful/useful.hpp>
#include <HC_useful/search_multithread.hpp>



template<typename T>
std::string dict_formatNumber(
    T value,
    int strWidth = 0,
    int varPrecision = 2,
    std::string align = "right",
    bool useBoolAlpha = false
) {
    std::stringstream outStream;
    outStream << std::fixed;
    if(align=="left") outStream<<std::left;
    else if(align=="right") outStream<<std::right;
    outStream << std::setw(strWidth) << std::setprecision(varPrecision) << (useBoolAlpha ? std::boolalpha : std::noboolalpha) << value;
    return outStream.str();
}


template<typename T>
std::string prettyPrint_vec1(
    std::vector<T> vec1_inp,
    std::string align = "right",
    int decimals = 2,
    int width = 0,
    int padding = 0,
    int prettyPrint = 0,
    int left_indent = 4,
    bool useBoolAlpha = false
) {
    std::string resultString = std::string(padding, ' ')+"{";
    
    bool loopInit=false;
    for(T elem: vec1_inp) {
        if(loopInit) resultString += ",";
        else loopInit = true;
        if(prettyPrint==1 || prettyPrint==2) resultString += "\n"+std::string(left_indent, ' ');
        resultString += dict_formatNumber<T>(elem, width, decimals, align, useBoolAlpha);
    }
    if(prettyPrint==1 || prettyPrint==2) resultString += "\n";
    resultString += "}"+std::string(padding, ' ');

    return resultString;
}

template<typename T>
std::string prettyPrint_vec2(
    std::vector<std::vector<T>> vec2_inp,
    std::string align = "right",
    int decimals = 2,
    int width = 0,
    int padding = 0,
    int prettyPrint = 0,
    int left_indent = 4,
    bool useBoolAlpha = false
) {
    std::string resultString = std::string(padding, ' ')+"{";

    bool loopInit0=false, loopInit1=false;
    for(std::vector<T> vecs: vec2_inp) {
        if(loopInit1) resultString += ",";
        else { loopInit1=true; }
        if(prettyPrint==1 || prettyPrint==2) resultString += "\n"+std::string(left_indent, ' ');
        resultString += "{";
        loopInit0 = false;
        for(T elem: vecs) {
            if(loopInit0) resultString += ",";
            else {loopInit0 = true;}
            if(prettyPrint==1) resultString += "\n"+std::string(left_indent*2, ' ');
            resultString += dict_formatNumber<T>(elem, width, decimals, align, useBoolAlpha);
        }
        if(prettyPrint==1) resultString += "\n"+std::string(left_indent, ' ');
        resultString += "}";
    }
    if(prettyPrint==1 || prettyPrint==2) resultString += "\n";
    resultString += "}"+std::string(padding, ' ');
    return resultString;
}


template<typename T>
bool hasRepetitions(std::vector<T> vec) {
    bool repeated = false;
    for(T elem: vec) {
        std::vector<int> pos = DIY_SEARCH_MULTITHREAD::multithread_searchVec<T>(vec, elem, -1, -1, true, 0, false);
        if(pos.size()>1) { repeated=true; break; }
    }
    return repeated;
}

/**
 * diy dictionary where each key is an std::string
 * 
 */
class diy_dict {
    private:
        int  arg_searchVec_threadLen    = 100;
        int  arg_searchVec_numThreads   = -1;
        int  arg_searchVec_checkSpacing = 1;
        bool arg_searchVec_verbose      = false;

        bool _storage_init = false;

        std::string _bool_string(bool boolVar);

        template<typename T> using vec0 = std::vector<T>; //vector: `T` value
        template<typename T> using vec1 = std::vector<std::vector<T>>; //vector: vector of `T` value
        template<typename T> using vec2 = std::vector<std::vector<std::vector<T>>>; //vector: vector of vector of `T` value

        template<typename T> using vec0p = std::vector<T*>; //vector: pointer: `T`
        template<typename T> using vec1p = std::vector<std::vector<T>*>; //vector: pointer: vector of `T`
        template<typename T> using vec2p = std::vector<std::vector<std::vector<T>>*>; //vector: pointer: vector of vector of `T`

        template<typename T> using pVec1 = std::vector<T>*; //pointer: vector of `T`
        template<typename T> using pVec2 = std::vector<std::vector<T>>*; //pointer: vector of vector of `T`


        //storage vectors
        vec0<bool>          values_0_bool;      //code: 0 0 0
        vec0<int>           values_0_int;       //code: 0 1 0
        vec0<float>         values_0_float;     //code: 0 2 0
        vec0<double>        values_0_double;    //code: 0 3 0
        vec0<char>          values_0_char;      //code: 0 4 0
        vec0<std::string>   values_0_string;    //code: 0 5 0

        vec0p<bool>         values_0_bool_p;    //code: 0 0 1
        vec0p<int>          values_0_int_p;     //code: 0 1 1
        vec0p<float>        values_0_float_p;   //code: 0 2 1
        vec0p<double>       values_0_double_p;  //code: 0 3 1
        vec0p<char>         values_0_char_p;    //code: 0 4 1
        vec0p<std::string>  values_0_string_p;  //code: 0 5 1


        vec1<bool>          values_1_bool;      //code: 1 0 0
        vec1<int>           values_1_int;       //code: 1 1 0
        vec1<float>         values_1_float;     //code: 1 2 0
        vec1<double>        values_1_double;    //code: 1 3 0
        vec1<char>          values_1_char;      //code: 1 4 0
        vec1<std::string>   values_1_string;    //code: 1 5 0

        vec1p<bool>         values_1_bool_p;    //code: 1 0 1
        vec1p<int>          values_1_int_p;     //code: 1 1 1
        vec1p<float>        values_1_float_p;   //code: 1 2 1
        vec1p<double>       values_1_double_p;  //code: 1 3 1
        vec1p<char>         values_1_char_p;    //code: 1 4 1
        vec1p<std::string>  values_1_string_p;  //code: 1 5 1


        vec2<bool>          values_2_bool;      //code: 2 0 0
        vec2<int>           values_2_int;       //code: 2 1 0
        vec2<float>         values_2_float;     //code: 2 2 0
        vec2<double>        values_2_double;    //code: 2 3 0
        vec2<char>          values_2_char;      //code: 2 4 0
        vec2<std::string>   values_2_string;    //code: 2 5 0

        vec2p<bool>         values_2_bool_p;    //code: 2 0 1
        vec2p<int>          values_2_int_p;     //code: 2 1 1
        vec2p<float>        values_2_float_p;   //code: 2 2 1
        vec2p<double>       values_2_double_p;  //code: 2 3 1
        vec2p<char>         values_2_char_p;    //code: 2 4 1
        vec2p<std::string>  values_2_string_p;  //code: 2 5 1


        /** (function is purely for the sake of being able to see this definiton as the code writer via intellisense popup) 
         * Type code "ID" definition:
         *  "code: a b c"
         *  - `a` -avail. range{0:2} - level of vector usage via using typedefinitons: `vec{a}` -> `0` = `vec0` = `std::vector<T>`.
         *  - `b` -avail. range{0:5} - type of datatype: in order{0:5} = {`bool`, `int`, `float`, `double`, `char`, `std::string`}.
         *  - `c` -avail. range{0:1} - boolean for if "value" is a pointer: `0`-not a pointer: ex.`vec1`; `1`-is a pointer: ex.`vec1p` 
         * 
         * Example: "code: 2 3 1"
         *      -> `std::vector<std::vector<std::vector<T>>>` `double` `*`
         *      -> `vec2p<double>`/`std::vector<std::vector<std::vector<double>>*>`
         *      »» "stores pointers to 2d vectors"
         * 
         * NOTE: When a code is set in `datatype` vector, leading 0's must be removed: `code: 0 1 2` -> `(int)12`
         * 
         * 
         * Vector level:[i*2, i*2+1][..]
         * Data type   :[..]        [i]
         * Non-pointer :[i*2]       [..]
         * Pointer:     [i*2+1]     [..]
         */
        const int dict_types[6][6] = {
            {000, 10, 20, 30, 40, 50},
            {  1, 11, 21, 31, 41, 51},
            {100,110,120,130,140,150},
            {101,111,121,131,141,151},
            {200,210,220,230,240,250},
            {201,211,221,231,241,251}
        };
        const int typeLIM[2] = {0, 251}; //smallest - biggest possible value; NOTE: does not include missing values inbetween

        vec0<std::string>   _keys;       // navigator vector: Labels/names
        vec0<int>           _datatype;   // navigator vector: What type is stored in that index
        vec0<int>           _idx;        // navigator vector: "local" index of (what index in the correct vector) where the given element is related to


        int _extend_reg(std::string key, int varType);

        template<typename V, typename T>void type_extend(V vecToExtend, T addValue) {
            vecToExtend.push_back(addValue);
            this->_idx.push_back(vecToExtend.size()-1);
        }

        int _erase_idx(int typeID, int _idx);

    public:

        /**
         * @brief Check whether storage containers has been initialised/added-to and thus if
         * the navigator vectors/containers has been initialised
         *
         * @return boolean of whether containers been initialised.
         */
        bool init_storage() { return this->_storage_init; }

        std::string& operator[] (int i) { return _keys.at(i); }
        std::string  operator[] (int i) const { return const_cast<std::string&>(_keys.at(i)); }

        /**
         * @brief check if key exists
         * 
         * @param key 
         * @return int index to in navigation container. 
         */
        int operator[] (std::string key) { return this->check_existence(key, -1); }

        std::vector<std::string> keys() { return this->_keys; }


        int check_existence(std::string key, int verbose);

        const std::string info_type_definition = (" \
        (function is purely for the sake of being able to see this definiton as the code writer via intellisense popup)\n \
         * Type code definition:\n \
         *  \"code: a b c\"\n \
         *  - `a` -avail. range{0:2} - level of vector usage via using typedefinitons: `vec{a}` -> `0` = `vec0` = `std::vector<T>`.\n \
         *  - `b` -avail. range{0:5} - type of datatype: in order{0:5} = {`bool`, `int`, `float`, `double`, `char`, `std::string`}.\n \
         *  - `c` -avail. range{0:1} - boolean for if \"value\" is a pointer: `0`-not a pointer: ex.`vec1`; `1`-is a pointer: ex.`vec1p`\n \
         * \n \
         * Example: \"code: 2 3 1\"\n \
         *      -> `std::vector<std::vector<std::vector<T>>>` `double` `*`\n \
         *      -> `vec2p<double>`/`std::vector<std::vector<std::vector<double>>*>`\n \
         *      »» \"stores pointers to 2d vectors\"\n \
         * \n \
         * NOTE: When a code is set in `datatype` vector, leading 0's must be removed: `code: 0 1 2` -> `(int)12` \
         * ");

        template<typename T>
        int get_template_type(T toCheck) {
            if      (std::is_same<T, bool>::value)      return 0;
            else if (std::is_same<T, int>::value)       return 10;
            else if (std::is_same<T, float>::value)     return 20;
            else if (std::is_same<T, double>::value)    return 30;
            else if (std::is_same<T, char>::value)      return 40;
            else if (std::is_same<T, std::string>::value) return 50;
            else if (std::is_same<T, bool*>::value)      return 1;
            else if (std::is_same<T, int*>::value)       return 11;
            else if (std::is_same<T, float*>::value)     return 21;
            else if (std::is_same<T, double*>::value)    return 31;
            else if (std::is_same<T, char*>::value)      return 41;
            else if (std::is_same<T, std::string*>::value) return 51;
            else if (std::is_same<T, vec0<bool>>::value)      return 100;
            else if (std::is_same<T, vec0<int>>::value)       return 110;
            else if (std::is_same<T, vec0<float>>::value)     return 120;
            else if (std::is_same<T, vec0<double>>::value)    return 130;
            else if (std::is_same<T, vec0<char>>::value)      return 140;
            else if (std::is_same<T, vec0<std::string>>::value) return 150;
            else if (std::is_same<T, pVec1<bool>*>::value)      return 101;
            else if (std::is_same<T, pVec1<int>*>::value)       return 111;
            else if (std::is_same<T, pVec1<float>*>::value)     return 121;
            else if (std::is_same<T, pVec1<double>*>::value)    return 131;
            else if (std::is_same<T, pVec1<char>*>::value)      return 141;
            else if (std::is_same<T, pVec1<std::string>*>::value) return 151;
            else if (std::is_same<T, vec1<bool>>::value)      return 200;
            else if (std::is_same<T, vec1<int>>::value)       return 210;
            else if (std::is_same<T, vec1<float>>::value)     return 220;
            else if (std::is_same<T, vec1<double>>::value)    return 230;
            else if (std::is_same<T, vec1<char>>::value)      return 240;
            else if (std::is_same<T, vec1<std::string>>::value) return 250;
            else if (std::is_same<T, pVec2<bool>*>::value)      return 201;
            else if (std::is_same<T, pVec2<int>*>::value)       return 211;
            else if (std::is_same<T, pVec2<float>*>::value)     return 221;
            else if (std::is_same<T, pVec2<double>*>::value)    return 231;
            else if (std::is_same<T, pVec2<char>*>::value)      return 241;
            else if (std::is_same<T, pVec2<std::string>*>::value) return 251;

            return -1;
        }


        std::string str_export(
            std::string key,
            std::string codedInsert = "",
            std::string align = "right",
            int decimals = 2,
            int width = -1,
            int padding = 0,
            int prettyPrint = 0,
            char emptySpace = ' ',
            int left_indent = 4
        );
        int get_type(std::string key);
        int get_type_size(int typeID);
        int get_type_size(std::string key);

        int rename_key(std::string key, std::string new_key);

        int delete_key(std::string key);
        

        diy_dict(/* args */);
        diy_dict(std::vector<std::string> keys, std::vector<bool>           values);
        diy_dict(std::vector<std::string> keys, std::vector<int>            values);
        diy_dict(std::vector<std::string> keys, std::vector<float>          values);
        diy_dict(std::vector<std::string> keys, std::vector<double>         values);
        diy_dict(std::vector<std::string> keys, std::vector<char>           values);
        diy_dict(std::vector<std::string> keys, std::vector<std::string>    values);

        diy_dict(std::vector<std::string> keys, std::vector<bool*>          values);
        diy_dict(std::vector<std::string> keys, std::vector<int*>           values);
        diy_dict(std::vector<std::string> keys, std::vector<float*>         values);
        diy_dict(std::vector<std::string> keys, std::vector<double*>        values);
        diy_dict(std::vector<std::string> keys, std::vector<char*>          values);
        diy_dict(std::vector<std::string> keys, std::vector<std::string*>   values);


        diy_dict(std::vector<std::string> keys, std::vector<std::vector<bool>>           values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<int>>            values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<float>>          values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<double>>         values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<char>>           values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::string>>    values);

        diy_dict(std::vector<std::string> keys, std::vector<std::vector<bool>*>          values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<int>*>           values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<float>*>         values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<double>*>        values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<char>*>          values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::string>*>   values);


        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<bool>>>           values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<int>>>            values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<float>>>          values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<double>>>         values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<char>>>           values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<std::string>>>    values);

        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<bool>>*>          values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<int>>*>           values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<float>>*>         values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<double>>*>        values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<char>>*>          values);
        diy_dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<std::string>>*>   values);

        ~diy_dict();


        int add(std::string key, bool value);
        int add(std::string key, int value);
        int add(std::string key, float value);
        int add(std::string key, double value);
        int add(std::string key, char value);
        int add(std::string key, std::string value);

        int add(std::string key, bool* ptr);
        int add(std::string key, int* ptr);
        int add(std::string key, float* ptr);
        int add(std::string key, double* ptr);
        int add(std::string key, char* ptr);
        int add(std::string key, std::string* ptr);


        int add(std::string key, std::vector<bool> value);
        int add(std::string key, std::vector<int> value);
        int add(std::string key, std::vector<float> value);
        int add(std::string key, std::vector<double> value);
        int add(std::string key, std::vector<char> value);
        int add(std::string key, std::vector<std::string> value);

        int add(std::string key, std::vector<bool>* ptr);
        int add(std::string key, std::vector<int>* ptr);
        int add(std::string key, std::vector<float>* ptr);
        int add(std::string key, std::vector<double>* ptr);
        int add(std::string key, std::vector<char>* ptr);
        int add(std::string key, std::vector<std::string>* ptr);


        int add(std::string key, std::vector<std::vector<bool>> value);
        int add(std::string key, std::vector<std::vector<int>> value);
        int add(std::string key, std::vector<std::vector<float>> value);
        int add(std::string key, std::vector<std::vector<double>> value);
        int add(std::string key, std::vector<std::vector<char>> value);
        int add(std::string key, std::vector<std::vector<std::string>> value);

        int add(std::string key, std::vector<std::vector<bool>>* ptr);
        int add(std::string key, std::vector<std::vector<int>>* ptr);
        int add(std::string key, std::vector<std::vector<float>>* ptr);
        int add(std::string key, std::vector<std::vector<double>>* ptr);
        int add(std::string key, std::vector<std::vector<char>>* ptr);
        int add(std::string key, std::vector<std::vector<std::string>>* ptr);


        int edit(std::string key, bool value);
        int edit(std::string key, int value);
        int edit(std::string key, float value);
        int edit(std::string key, double value);
        int edit(std::string key, char value);
        int edit(std::string key, std::string value);

        int edit(std::string key, bool* ptr);
        int edit(std::string key, int* ptr);
        int edit(std::string key, float* ptr);
        int edit(std::string key, double* ptr);
        int edit(std::string key, char* ptr);
        int edit(std::string key, std::string* ptr);


        int edit(std::string key, std::vector<bool> value);
        int edit(std::string key, std::vector<int> value);
        int edit(std::string key, std::vector<float> value);
        int edit(std::string key, std::vector<double> value);
        int edit(std::string key, std::vector<char> value);
        int edit(std::string key, std::vector<std::string> value);

        int edit(std::string key, std::vector<bool>* ptr);
        int edit(std::string key, std::vector<int>* ptr);
        int edit(std::string key, std::vector<float>* ptr);
        int edit(std::string key, std::vector<double>* ptr);
        int edit(std::string key, std::vector<char>* ptr);
        int edit(std::string key, std::vector<std::string>* ptr);


        int edit(std::string key, std::vector<std::vector<bool>> value);
        int edit(std::string key, std::vector<std::vector<int>> value);
        int edit(std::string key, std::vector<std::vector<float>> value);
        int edit(std::string key, std::vector<std::vector<double>> value);
        int edit(std::string key, std::vector<std::vector<char>> value);
        int edit(std::string key, std::vector<std::vector<std::string>> value);

        int edit(std::string key, std::vector<std::vector<bool>>* ptr);
        int edit(std::string key, std::vector<std::vector<int>>* ptr);
        int edit(std::string key, std::vector<std::vector<float>>* ptr);
        int edit(std::string key, std::vector<std::vector<double>>* ptr);
        int edit(std::string key, std::vector<std::vector<char>>* ptr);
        int edit(std::string key, std::vector<std::vector<std::string>>* ptr);

                
        bool        get0_bool_  (std::string key);
        int         get0_int_   (std::string key);
        float       get0_float_ (std::string key);
        double      get0_double_(std::string key);
        char        get0_char_  (std::string key);
        std::string get0_string_(std::string key);

        bool*       get0_boolP  (std::string key);
        int*        get0_intP   (std::string key);
        float*      get0_floatP (std::string key);
        double*     get0_doubleP(std::string key);
        char*       get0_charP  (std::string key);
        std::string* get0_stringP(std::string key);


        std::vector<bool>           get1_bool_  (std::string key);
        std::vector<int>            get1_int_   (std::string key);
        std::vector<float>          get1_float_ (std::string key);
        std::vector<double>         get1_double_(std::string key);
        std::vector<char>           get1_char_  (std::string key);
        std::vector<std::string>    get1_string_(std::string key);

        std::vector<bool>*          get1_boolP  (std::string key);
        std::vector<int>*           get1_intP   (std::string key);
        std::vector<float>*         get1_floatP (std::string key);
        std::vector<double>*        get1_doubleP(std::string key);
        std::vector<char>*          get1_charP  (std::string key);
        std::vector<std::string>*   get1_stringP(std::string key);


        std::vector<std::vector<bool>>        get2_bool_  (std::string key);
        std::vector<std::vector<int>>         get2_int_   (std::string key);
        std::vector<std::vector<float>>       get2_float_ (std::string key);
        std::vector<std::vector<double>>      get2_double_(std::string key);
        std::vector<std::vector<char>>        get2_char_  (std::string key);
        std::vector<std::vector<std::string>> get2_string_(std::string key);

        std::vector<std::vector<bool>>*        get2_boolP  (std::string key);
        std::vector<std::vector<int>>*         get2_intP   (std::string key);
        std::vector<std::vector<float>>*       get2_floatP (std::string key);
        std::vector<std::vector<double>>*      get2_doubleP(std::string key);
        std::vector<std::vector<char>>*        get2_charP  (std::string key);
        std::vector<std::vector<std::string>>* get2_stringP(std::string key);



};




#endif