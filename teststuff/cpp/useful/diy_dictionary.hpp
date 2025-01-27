#pragma once
#ifndef HPP_DICTIONARY
#define HPP_DICTIONARY

/**
 * @file dictionary.hpp
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
#include <list>
#include <iterator>
#include <initializer_list>

// #include <useful.hpp>
#include <search_multithread.hpp>

using DIY_SEARCH_MULTITHREAD::hasRepetitions, DIY_SEARCH_MULTITHREAD::check_existence;

namespace DIY {


    template<typename T>
    std::string formatNumber(
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
            resultString += DIY::formatNumber<T>(elem, width, decimals, align, useBoolAlpha);
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
                resultString += DIY::formatNumber<T>(elem, width, decimals, align, useBoolAlpha);
            }
            if(prettyPrint==1) resultString += "\n"+std::string(left_indent, ' ');
            resultString += "}";
        }
        if(prettyPrint==1 || prettyPrint==2) resultString += "\n";
        resultString += "}"+std::string(padding, ' ');
        return resultString;
    }



    /**
     * Dictionary of pre-defined group of types sorted by `std::string` type "keys".(detailed definition of type and type-code/typeID's can be found in `DIY::dict::dict_types[6][6]` docstring)
     * 
     */
    class dict {
        private:
            std::string _info_name = "DIY::dict";


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

            std::string& operator[] (int i) { 
                if(static_cast<size_t>(abs(i)) >= _keys.size()) throw std::runtime_error(_info_name+": & operator arg too big: \"abs("+std::to_string(i)+")>size()\"");
                if(i<0) return _keys.at(_keys.size()+static_cast<size_t>(i));
                return _keys.at(i);
            }
            std::string  operator[] (int i) const {
                if(static_cast<size_t>(abs(i)) >= _keys.size()) throw std::runtime_error(_info_name+": & operator arg too big: \"abs("+std::to_string(i)+")>size()\"");
                if(i<0) return const_cast<std::string&>(_keys.at(_keys.size()+static_cast<size_t>(i)));
                return const_cast<std::string&>(_keys.at(i));
            }

            /**
             * @brief check if key exists
             * 
             * @param key 
             * @return int index to in navigation container. 
             */
            int operator[] (std::string key) { return check_existence<std::string>(key, this->_keys, -1); }

            std::vector<std::string> keys() { return this->_keys; }


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
            

            dict(/* args */);
            dict(std::vector<std::string> keys, std::vector<bool>           values);
            dict(std::vector<std::string> keys, std::vector<int>            values);
            dict(std::vector<std::string> keys, std::vector<float>          values);
            dict(std::vector<std::string> keys, std::vector<double>         values);
            dict(std::vector<std::string> keys, std::vector<char>           values);
            dict(std::vector<std::string> keys, std::vector<std::string>    values);

            dict(std::vector<std::string> keys, std::vector<bool*>          values);
            dict(std::vector<std::string> keys, std::vector<int*>           values);
            dict(std::vector<std::string> keys, std::vector<float*>         values);
            dict(std::vector<std::string> keys, std::vector<double*>        values);
            dict(std::vector<std::string> keys, std::vector<char*>          values);
            dict(std::vector<std::string> keys, std::vector<std::string*>   values);


            dict(std::vector<std::string> keys, std::vector<std::vector<bool>>           values);
            dict(std::vector<std::string> keys, std::vector<std::vector<int>>            values);
            dict(std::vector<std::string> keys, std::vector<std::vector<float>>          values);
            dict(std::vector<std::string> keys, std::vector<std::vector<double>>         values);
            dict(std::vector<std::string> keys, std::vector<std::vector<char>>           values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::string>>    values);

            dict(std::vector<std::string> keys, std::vector<std::vector<bool>*>          values);
            dict(std::vector<std::string> keys, std::vector<std::vector<int>*>           values);
            dict(std::vector<std::string> keys, std::vector<std::vector<float>*>         values);
            dict(std::vector<std::string> keys, std::vector<std::vector<double>*>        values);
            dict(std::vector<std::string> keys, std::vector<std::vector<char>*>          values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::string>*>   values);


            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<bool>>>           values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<int>>>            values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<float>>>          values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<double>>>         values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<char>>>           values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<std::string>>>    values);

            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<bool>>*>          values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<int>>*>           values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<float>>*>         values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<double>>*>        values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<char>>*>          values);
            dict(std::vector<std::string> keys, std::vector<std::vector<std::vector<std::string>>*>   values);

            ~dict();


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


    template<class _key_type, class _store_type>
    struct _pair {
        _key_type   key;
        _store_type value;

        _pair(_key_type _key, _store_type _value): key(_key), value(_value) {}
    };

    /**
     * @brief Store custom types in a dictionary system.
     * Can not store container types as `_key_type` has to be comparable with different instances of same type
     * 
     * @tparam _key_type data type of the key in the key/value pair
     * @tparam _store_type data type of the value in the key/value pair
     */
    template<class _key_type, class _store_type>
    class typed_dict {
        private:
            std::string _info_name = "DIY::typed_dict";
            
            std::vector<_key_type>      _keys;
            std::vector<_store_type*>   _lookup;
            std::list<_store_type>      _values;
            bool _values_modified = true;

            _key_type   _nullKey;
            _store_type _nullValue;

            bool _init_container = false;

            void _call_error(int code, std::string from_member="", std::string custom_error="") const;

            // auto _getItr_key(size_t idx);

        public:
            void _update_lookup();
            
            auto _getItr(int idx);
            auto _getItr(int idx) const;
            auto _getItr_rev(int idx);
            auto _getItr_rev(int idx) const;


            typed_dict() {}; //empty default constructor

            /**
             * @brief Construct a new typed dict object; Copy constructor
             * 
             * @param _copyDict 
             */
            typed_dict(const typed_dict<_key_type, _store_type> &_copyDict);


            typed_dict(_key_type _key, _store_type _store);
            typed_dict(std::vector<_key_type> keys, std::list<_store_type> values);
            typed_dict(std::vector<_key_type> keys, std::vector<_store_type> values);
            typed_dict(std::initializer_list<_key_type> keys, std::initializer_list<_store_type> values);

            typed_dict(_pair<_key_type, _store_type> _key_value_pair);
            typed_dict(std::initializer_list<_pair<_key_type, _store_type>> _pairs);

            /**
             * @brief Assignment operator
             * 
             * @param _assignDict 
             * @return typed_dict<_key_type, _store_type>& 
             */
            typed_dict<_key_type, _store_type> &operator=(const typed_dict<_key_type, _store_type> &_assignDict);


            _store_type& operator[] (int idx);
            _store_type  operator[] (int idx) const;


            _store_type& get(_key_type key);
            _store_type  get(_key_type key) const;

            _key_type    getKey(size_t idx) const;

            _store_type* getPtr(_key_type key);
            _store_type* getPtr_idx(int idx);

            /**
             * @brief Find the element index/position of given `key`
             * 
             * @param key the key to find index/position of
             * @param _call_except whether to call an exception if an error occurs(/index can't be found)
             * @return int the index. If none found and `_call_except==false` then it'll return `-1`
             */
            int find(_key_type key, bool _call_except=true) const;

            size_t size() const;

            _key_type key(int idx) const;
            std::vector<_key_type>      keys();
            std::list<_store_type>      values();
            std::vector<_store_type*>   lookup();

            std::string str_export(_key_type key, int width=0, int precision=2, std::string align="right", bool useBoolAlpha=false);


            int add(_key_type key, _store_type value);

            int append(std::initializer_list<_key_type> keys, std::initializer_list<_store_type> values);
            int append(std::vector<_key_type> keys, std::list<_store_type> values);

            int insert(size_t pos, _key_type key, _store_type value);
            int insert(size_t pos, std::initializer_list<_key_type> keys, std::initializer_list<_store_type> values);
            int insert(size_t pos, std::vector<_key_type> keys, std::list<_store_type> values);

            int replace(_key_type key, _store_type new_value);

            int rename(_key_type key, _key_type new_key);

            int erase(_key_type key);
            int eraseIdx(int idx);

            friend auto operator<<(std::ostream &os, typed_dict const& m) -> std::ostream& {
                // std::string _outText = "{";
                os << "{";
                for(size_t i=0; i<m._keys.size(); i++) {
                    // _outText + std::to_string(m._keys.at(i)) + ": " + std::to_string(*m._lookup.at(i));
                    // std::cout << " op-call:"<<m._keys.at(i) << " ";
                    os << m._keys.at(i) << ": ";
                    os << std::boolalpha;
                    os << *(m._lookup.at(i));
                    if(i+1 < m._keys.size()) {
                        // _outText + ", ";
                        os << ", ";
                    }
                }
                // _outText + "}";
                os << "}";
                return os;
            }
    };
    



    /**
     * @brief private member function for `throw`:ing specific errors
     *  
     * @param code designated code to what error has occurred (2). (1)
     * @param from_member `std::string` with `::`+name of member function the error occurred in
     * @param custom_error `std::string` with custom error to use instead of pre-defined codes called via `code`
     * @note (1) if `custom_error.length()!=0` then `code` will be ignored.
     * @note  
     * @note (2):
     * @note  `0`- "input key argument not found in _keys storage"
     * @note  `1`- "input key argument already exists in _keys storage"
     * @note  `2`- "class hasn't been initialized"
     * 
     * @note Will throw `std::runtime_error`
     * 
     * 
     */
    template<class _key_type, class _store_type>
    void typed_dict<_key_type, _store_type>::_call_error(int code, std::string from_member, std::string custom_error) const {
        std::string callStr = "ERROR: "+this->_info_name+from_member+": ";
        if(custom_error.length()!=0) callStr+=custom_error;
        else {
            switch (code) {
            case 0: //key not found
                callStr += " input key argument not found in _keys storage";
                break;
            case 1: //key already exists
                callStr += " input key argument already exists in _keys storage";
                break;
            case 2: //_init_containers is false
                callStr += " class containers hasn't been initialized";
                break;
            default:
                break;
            }
        }
        throw std::runtime_error(callStr);
    }


    template<class _key_type, class _store_type>
    void typed_dict<_key_type, _store_type>::_update_lookup() {
        this->_lookup.clear();
        for(auto itr=_values.begin(); itr!=_values.end(); ++itr) {
            _lookup.push_back(&*itr);
        }
    }

    template<class _key_type, class _store_type>
    auto typed_dict<_key_type, _store_type>::_getItr(int idx) {
        if(idx>=static_cast<int>(_keys.size())) this->_call_error(0, "::_getItr(int)", "arg for `idx` is too large");
        else if(abs(idx)>_keys.size()) this->_call_error(0, "::_operator[] (int)", " value for reverse indexing is too small");
        else if(idx<0) idx = static_cast<int>(_keys.size()) + idx;
        
        auto itr = _values.begin();
        std::advance(itr, idx);
        // std::cout << &*itr << "  ";
        return itr;
    }
    template<class _key_type, class _store_type>
    auto typed_dict<_key_type, _store_type>::_getItr(int idx) const {
        if(idx>=static_cast<int>(_keys.size())) this->_call_error(0, "::_getItr(int)", "arg for `idx` is too large");
        else if(abs(idx)>_keys.size()) this->_call_error(0, "::_operator[] (int)", " value for reverse indexing is too small");
        else if(idx<0) idx = static_cast<int>(_keys.size()) + idx;
        
        auto itr = _values.begin();
        advance(itr, idx);

        return itr;
    }
    template<class _key_type, class _store_type>
    auto typed_dict<_key_type, _store_type>::_getItr_rev(int idx) {
        if(idx>=static_cast<int>(_keys.size())) this->_call_error(0, "::_getItr(size_t)", "arg for `idx` is too large");
        else if(abs(idx)>_keys.size()) this->_call_error(0, "::_operator[] (int)", " value for reverse indexing is too small");
        else if(idx<0) idx = static_cast<int>(_keys.size()) + idx;
        
        auto itr = _values.rbegin();
        advance(itr, idx);

        return itr;
    }
    template<class _key_type, class _store_type>
    auto typed_dict<_key_type, _store_type>::_getItr_rev(int idx) const {
        if(idx>=static_cast<int>(_keys.size())) this->_call_error(0, "::_getItr(size_t)", "arg for `idx` is too large");
        else if(abs(idx)>_keys.size()) this->_call_error(0, "::_operator[] (int)", " value for reverse indexing is too small");
        else if(idx<0) idx = static_cast<int>(_keys.size()) + idx;
        
        auto itr = _values.rbegin();
        advance(itr, idx);

        return itr;
    }


    template<class _key_type, class _store_type>
    typed_dict<_key_type, _store_type>::typed_dict(const typed_dict<_key_type, _store_type> &_copyDict):
        _keys(_copyDict._keys),
        _values(_copyDict._values),
        _values_modified(_copyDict._values_modified),
        _nullKey(_copyDict._nullKey),
        _nullValue(_copyDict._nullValue),
        _init_container(_copyDict._init_container)
    {
        for(auto itr=_values.begin(); itr!=_values.end(); ++itr) {
            _lookup.push_back(&*itr);
        }
    }

    // template<class _key_type, class _store_type>
    // typed_dict<_key_type, _store_type>::typed_dict() {}
    template<class _key_type, class _store_type>
    typed_dict<_key_type, _store_type>::typed_dict(_key_type _key, _store_type _store) {
        this->_keys.push_back(_key);
        this->_values.push_back(_store);
        this->_lookup.push_back(&(*_values.begin()));
        this->_init_container = true;
    }
    template<class _key_type, class _store_type>
    typed_dict<_key_type, _store_type>::typed_dict(std::vector<_key_type> keys, std::list<_store_type> values) {
        if(keys.size()!=values.size()) this->_call_error(0, "::typed_dict(std::vector<_key_type>, std::list<_store_type>)", "input containers for `keys` and `values` aren't same size.");
        if(hasRepetitions<_key_type>(keys)) this->_call_error(0, "::typed_dict(std::vector<_key_type>, std::list<_store_type>)", "there are repetitions inside input argument for `keys`");
        this->_keys = keys;
        this->_values = values;
        for(auto itr=_values.begin(); itr!=_values.end(); ++itr) _lookup.push_back(&(*itr));
        this->_init_container = true;
    }
    template<class _key_type, class _store_type>
    typed_dict<_key_type, _store_type>::typed_dict(std::vector<_key_type> keys, std::vector<_store_type> values) {
        if(keys.size()!=values.size()) this->_call_error(0, "::typed_dict(std::vector<_key_type>, std::vector<_store_type>)", "input containers for `keys` and `values` aren't same size.");
        if(hasRepetitions<_key_type>(keys)) this->_call_error(0, "::typed_dict(std::vector<_key_type>, std::vector<_store_type>)", "there are repetitions inside input argument for `keys`");
        std::list<_store_type> _tempLst;
        for(size_t i=0; i<values.size(); i++) _tempLst.push_back(values.at(i));
        this->_keys = keys;
        this->_values = _tempLst;
        for(auto itr=_values.begin(); itr!=_values.end(); ++itr) _lookup.push_back(&(*itr));
        this->_init_container = true;
    }
    template<class _key_type, class _store_type>
    typed_dict<_key_type, _store_type>::typed_dict(std::initializer_list<_key_type> keys, std::initializer_list<_store_type> values) {
        if(keys.size()!=values.size()) this->_call_error(0, "::typed_dict(std::initializer_list<_key_type>, std::initializer_list<_store_type>)", "input containers for `keys` and `values` aren't same size.");
        if(hasRepetitions<_key_type>(std::vector<_key_type>(keys))) this->_call_error(0, "::typed_dict(std::initializer_list<_key_type>, std::initializer_list<_store_type>)", "there are repetitions inside input argument for `keys`");
        this->_keys = keys;
        this->_values = values;
        for(auto itr=_values.begin(); itr!=_values.end(); ++itr) _lookup.push_back(&(*itr));
        this->_init_container = true;
    }
    template<class _key_type, class _store_type>
    typed_dict<_key_type, _store_type>::typed_dict(_pair<_key_type, _store_type> _key_value_pair) {
        this->_keys.push_back(_key_value_pair.key);
        this->_values.push_back(_key_value_pair.value);
        this->_lookup.push_back(&(*_values.begin()));
        this->_init_container = true;
    }
    template<class _key_type, class _store_type>
    typed_dict<_key_type, _store_type>::typed_dict(std::initializer_list<_pair<_key_type, _store_type>> _pairs) {
        std::vector<_key_type>  vec_keys;
        std::list<_store_type>  vec_values;
        for(auto itr=_pairs.begin(); itr!=_pairs.end(); ++itr) {
            vec_keys.push_back((*itr).key);
            vec_values.push_back((*itr).value);
        }
        if(hasRepetitions<_key_type>(vec_keys)) this->_call_error(0, "::typed_dict(std::initializer_list<_pair<_key_type, _store_type>>)", "there are repetitions within the keys.");
        this->_keys = vec_keys;
        this->_values = vec_values;
        for(auto itr=_values.begin(); itr!=_values.end(); ++itr) {
            _lookup.push_back(&(*itr));
        }


        // size_t cnt = 0;
        // for(auto itr=_values.begin(); itr!=_values.end(); ++itr) {
        //     std::cout << std::setw(21)<<std::left<<_keys[cnt]<<" : ";
        //     std::cout << (&(*itr)) << " | " << _lookup[cnt] << " | "<< &_getItr(cnt) << " | " << &*_getItr(cnt);
        //     std::cout << " || " << std::boolalpha << *itr << std::endl;
        //     cnt++;
        // }
        this->_init_container = true;
    }


    template<class _key_type, class _store_type>
    typed_dict<_key_type, _store_type> &typed_dict<_key_type, _store_type>::operator=(const typed_dict<_key_type, _store_type> &_assignDict) {
        _keys       = _assignDict._keys;
        _values     = _assignDict._values;
        _values_modified = _assignDict._values_modified;
        _nullKey    = _assignDict._nullKey;
        _nullValue  = _assignDict._nullValue;
        _init_container = _assignDict._init_container;
        for(auto itr=_values.begin(); itr!=_values.end(); ++itr) {
            _lookup.push_back(&*itr);
        }
        return *this;
    }

    template<class _key_type, class _store_type>
    _store_type& typed_dict<_key_type, _store_type>::operator[] (int idx) {
        if(!_init_container) this->_call_error(2, "::_operator[] (int)");

        if(idx>=static_cast<int>(_keys.size())) this->_call_error(0, "::_operator[] (int)", " arg for index `idx` is bigger than available number of keys: "+std::to_string(_keys.size()));
        else if(abs(idx)>_keys.size()) this->_call_error(0, "::_operator[] (int)", " value for reverse indexing is too small");
        else if(idx<0) idx = static_cast<int>(_keys.size()) + idx;

        // auto itr = _values.begin();
        // std::advance(itr, idx);
        return *this->_lookup[idx];
        // return *itr;
        // return *(this->_getItr(idx));
    }
    template<class _key_type, class _store_type>
    _store_type typed_dict<_key_type, _store_type>::operator[] (int idx) const {
        if(!_init_container) this->_call_error(2, "::_operator[] (int) const");

        if(idx>=static_cast<int>(_keys.size())) this->_call_error(0, "::_operator[] (int)", " arg for index `idx` is bigger than available number of keys: "+std::to_string(_keys.size()));
        else if(abs(idx)>_keys.size()) this->_call_error(0, "::_operator[] (int)", " value for reverse indexing is too small");
        else if(idx<0) idx = static_cast<int>(_keys.size()) + idx;

        return const_cast<_store_type&>(*(this->_getItr(idx)));
    }
    
    template<class _key_type, class _store_type>
    _store_type& typed_dict<_key_type, _store_type>::get(_key_type key) {
        if(!_init_container) this->_call_error(2, "::get(_key_type)");

        int idx=-1;
        for(size_t i=0; i<_keys.size(); i++) {
            if(_keys[i]==key) {
                idx = i;
                break;
                // return *(this->_getItr(i));
            }
        }
        if(idx<0) this->_call_error(0, "::get(_key_type)");

        // auto itr = _values.begin();
        // std::advance(itr, idx);
        return *this->_lookup[idx];
        // return *(this->_getItr(idx));
    }
    template<class _key_type, class _store_type>
    _store_type  typed_dict<_key_type, _store_type>::get(_key_type key) const {
        if(!_init_container) this->_call_error(2, "::get(_key_type) const"); 

        for(size_t i=0; i<_keys.size(); i++) {
            if(_keys[i]==key) return const_cast<_store_type&>(*(this->_getItr(i)));
        }
        this->_call_error(0, "::get(_key_type)");
        return *(this->_getItr(0));
    }

    template<class _key_type, class _store_type>
    _key_type typed_dict<_key_type, _store_type>::getKey(size_t idx) const {
        return this->_keys.at(idx);
    }


    template<class _key_type, class _store_type>
    _store_type* typed_dict<_key_type, _store_type>::getPtr(_key_type key) {
        if(!_init_container) this->_call_error(2, "::getPtr(_key_type)");

        for(size_t i=0; i<_keys.size(); i++) {
            if(_keys[i]==key) return _lookup[i];
        }
        this->_call_error(0, "::getPtr(_key_type)");
    }
    template<class _key_type, class _store_type>
    _store_type* typed_dict<_key_type, _store_type>::getPtr_idx(int idx) {
        if(!_init_container) this->_call_error(2, "::getPtr_idx(int)");
        
        if(idx>=static_cast<int>(_keys.size())) this->_call_error(0, "::getPtr_idx(int)", "idx is bigger than stored number of keys");
        else if(abs(idx)>_keys.size()) this->_call_error(0, "::getPtr_idx(int)", "argument for `idx` goes too far into the negative. There are "+std::to_string(_keys.size())+" stored. Input arg was: "+std::to_string(idx));
        else if(idx<0) idx = static_cast<int>(_keys.size()) + idx;

        return this->_lookup[idx];
    }

    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::find(_key_type key, bool _call_except) const {
        for(int i=0; i<_keys.size(); i++) {
            if(_keys[i]==key) return i;
        }
        if(_call_except) this->_call_error(0, "::find(_key_type, bool)");
        return -1;
    }


    template<class _key_type, class _store_type>
    size_t typed_dict<_key_type, _store_type>::size() const { return this->_keys.size(); }

    template<class _key_type, class _store_type>
    _key_type typed_dict<_key_type, _store_type>::key(int idx) const {
        if(!_init_container) this->_call_error(2, "::key(int)");

        if(idx>=static_cast<int>(_keys.size())) this->_call_error(0, "::key(int)", " arg for index `idx` is bigger than available number of keys: "+std::to_string(_keys.size()));
        else if(abs(idx)>_keys.size()) this->_call_error(0, "::key (int)", " value for reverse indexing is too small");
        else if(idx<0) idx = static_cast<int>(_keys.size()) + idx;

        return this->_keys.at(idx);
    }

    template<class _key_type, class _store_type>
    std::vector<_key_type> typed_dict<_key_type, _store_type>::keys() { return _keys; }

    template<class _key_type, class _store_type>
    std::list<_store_type> typed_dict<_key_type, _store_type>::values() { return _values; }

    template<class _key_type, class _store_type>
    std::vector<_store_type*> typed_dict<_key_type, _store_type>::lookup() { return _lookup; }

    template<class _key_type, class _store_type>
    std::string typed_dict<_key_type, _store_type>::str_export(
        _key_type key,
        int width,
        int precision,
        std::string align,
        bool useBoolAlpha
    ) {
        int pos = check_existence<_key_type>(key, _keys);
        if(pos<0) this->_call_error(0, "::str_export(_key_type, int, int, std::string, bool)");
        return formatNumber<_store_type>(*(_lookup[pos]), width, precision, align, useBoolAlpha);
    }

    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::add(_key_type key, _store_type value) {
        if(check_existence<_key_type>(key, this->_keys)!=-1) this->_call_error(1, "::add(_key_type, _store_type)");
        this->_keys.push_back(key);
        this->_values.push_back(value);
        this->_lookup.push_back(&*(this->_getItr(-1)));

        if(!this->_init_container) this->_init_container = true;
        this->_values_modified = true;
        return 0;
    }
    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::append(std::initializer_list<_key_type> keys, std::initializer_list<_store_type> values) {
        if(keys.size()!=values.size()) this->_call_error(0, "::append(std::initializer_list<_key_type>, std::initializer_list<_store_type>)", " arguments aren't the same size");
        if(hasRepetitions<_key_type>(keys)) this->_call_error(0, "::append(std::initializer_list<_key_type>, std::initializer_list<_store_type>)", " input argument for keys has repetitions of elements");
        
        for(auto elem: keys) { if(check_existence<_key_type>(elem, _keys)!=-1) { this->_call_error(1,"::append(std::initializer_list<_key_type>, std::initializer_list<_store_type>)"); } } //check if any of the new keys exist in _keys container

        this->insert(this->size(), std::vector<_key_type>(keys), std::list<_store_type>(values));

        return 0;
    }
    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::append(std::vector<_key_type> keys, std::list<_store_type> values) {
        if(keys.size()!=values.size()) this->_call_error(0, "::append(std::vector<_key_type>, std::list<_store_type>)", " arguments aren't the same size");
        if(hasRepetitions<_key_type>(keys)) this->_call_error(0, "::append(std::vector<_key_type>, std::list<_store_type>)", " input argument for keys has repetitions of elements");
        for(auto elem: keys) { if(check_existence<_key_type>(elem, _keys)!=-1) { this->_call_error(1,"::append(std::vector<_key_type>, std::list<_store_type>)"); } }
        this->insert(this->size(), keys, values);
        
        return 0;
    }

    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::insert(size_t pos, _key_type key, _store_type value) {
        if(check_existence<_key_type>(key, this->_keys)!=-1) this->_call_error(1, "::insert(size_t, _key_type, _store_type)");
        this->_keys.insert(this->_keys.begin()+pos, key);
        auto itr = _values.begin();
        std::advance(itr, pos);
        this->_values.insert(itr, value);
        // itr = _look
        this->_lookup.insert(this->_lookup.begin()+pos, &*_getItr(pos));
        this->_values_modified = true;
        return 0;
    }
    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::insert(size_t pos, std::initializer_list<_key_type> keys, std::initializer_list<_store_type> values) {
        if(keys.size()!=values.size()) this->_call_error(0, "::insert(size_t, std::initializer_list<_key_type>, std::initializer_list<_store_type>)", " arguments aren't the same size");
        if(hasRepetitions<_key_type>(keys)) this->_call_error(0, "::insert(size_t, std::initializer_list<_key_type>, std::initializer_list<_store_type>)", " input argument for keys has repetitions of elements");
        for(auto elem: keys) { if(check_existence<_key_type>(elem, this->_keys)!=-1) { this->_call_error(1,"::insert(size_t, std::initializer_list<_key_type>, std::initializer_list<_store_type>)"); } }
        
        this->insert(pos, std::vector<_key_type>(keys), std::list<_store_type>(values));

        // this->_keys.insert(this->_keys.begin()+pos, keys.begin(), keys.end());
        // this->_values.insert(this->_values.begin()+pos, values.begin(), values.end());
        // std::vector<_store_type*> tmpPtr(values.size(), nullptr);
        // auto itr = _values.begin();
        // advance(itr, pos);
        // for(size_t i=0; i<values.size(); i++) {
        //     tmpPtr[i] = &*itr;
        //     ++itr;
        // }
        // this->_lookup.insert(this->_lookup.begin()+pos, tmpPtr.begin(), tmpPtr.end());
        // this->_values_modified = true;
        return 0;
    }
    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::insert(size_t pos, std::vector<_key_type> keys, std::list<_store_type> values) {
        /// Core function for overloads and similar
        if(keys.size()!=values.size()) this->_call_error(0, "::insert(size_t, std::vector<_key_type>, std::vector<_store_type>)", " arguments aren't the same size");
        if(hasRepetitions<_key_type>(keys)) this->_call_error(0, "::insert(size_t, std::vector<_key_type>, std::vector<_store_type>)", " input argument for keys has repetitions of elements");
        for(auto elem: keys) { if(check_existence<_key_type>(elem, this->_keys)!=-1) { this->_call_error(1,"::insert(std::vector<_key_type>, std::vector<_store_type>)"); } }


        this->_keys.insert(this->_keys.begin()+pos, keys.begin(), keys.end());
        auto itr = _values.begin();
        std::advance(itr, pos);
        this->_values.insert(itr, values.begin(), values.end());

        std::vector<_store_type*> tmpPtr(values.size(), nullptr);
        itr = _values.begin();
        advance(itr, pos);
        for(size_t i=0; i<values.size(); i++) {
            tmpPtr[i] = &*itr;
            ++itr;
        }
        this->_lookup.insert(this->_lookup.begin()+pos, tmpPtr.begin(), tmpPtr.end());

        this->_values_modified = true;
        return 0;
    }

    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::replace(_key_type key, _store_type new_value) {
        int pos = check_existence<_key_type>(key, this->_keys);
        if(pos<0) this->_call_error(0, "::replace(_key_type, _store_type)");
        // this->_values.at(pos) = new_value;

        *(this->_lookup[pos]) = new_value;

        return 0;
    }

    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::rename(_key_type key, _key_type new_key) {
        int pos = check_existence<_key_type>(key, this->_keys);
        if(pos<0) this->_call_error(0, "::rename(_key_type, _key_type)");
        // if(key==new_key) this->_call_error(0, "::rename(_key_type, _key_type)", "new_key is the same as key");
        int pos2= check_existence<_key_type>(new_key, this->_keys);
        if(key!=new_key && pos2!=-1) this->_call_error(0, "::rename(_key_type, _key_type)", "second parameter argument for `new_key` \""+new_key+"\" already exists in dictionary keys");
        this->_keys.at(pos) = new_key;
        return 0;
    }

    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::erase(_key_type key) {
        int pos = check_existence<_key_type>(key, this->_keys);
        if(pos<0) this->_call_error(0, "::erase(_key_type)");
        
        this->_keys.erase(this->_keys.begin()+pos);
        auto itr = _values.begin();
        std::advance(itr, pos);
        this->_values.erase(itr);
        this->_lookup.erase(this->_lookup.begin()+pos);
        this->_values_modified = true;
        return 0;
    }
    template<class _key_type, class _store_type>
    int typed_dict<_key_type, _store_type>::eraseIdx(int idx) {
        if(!_init_container) this->_call_error(2, "::eraseIdx(int)");

        if(idx>=static_cast<int>(_keys.size())) this->_call_error(0, "::eraseIdx(int)", " arg for index `idx` is bigger than available number of keys: "+std::to_string(_keys.size()));
        else if(abs(idx)>_keys.size()) this->_call_error(0, "::eraseIdx(int)", " value for reverse indexing is too small");
        else if(idx<0) idx = static_cast<int>(_keys.size()) + idx;

        this->_keys.erase(this->_keys.begin()+idx);
        auto itr = _values.begin();
        std::advance(itr, idx);
        this->_values.erase(itr);
        this->_lookup.erase(this->_lookup.begin()+idx);
        this->_values_modified = true;
        return 0;
    }
}


#endif