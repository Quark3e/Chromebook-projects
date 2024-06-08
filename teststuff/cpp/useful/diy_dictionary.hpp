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
#include <memory>
#include <iomanip>
#include <stdexcept>

#include <HC_useful/useful.hpp>
#include <HC_useful/search_multithread.hpp>


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

        std::string bool_string(bool boolVar);

        template<typename T> using vec0 = std::vector<T>; //vector: `T` value
        template<typename T> using vec1 = std::vector<std::vector<T>>; //vector: vector of `T` value
        template<typename T> using vec2 = std::vector<std::vector<std::vector<T>>>; //vector: vector of vector of `T` value

        template<typename T> using vec0p = std::vector<T*>; //vector: pointer: `T`
        template<typename T> using vec1p = std::vector<std::vector<T>*>; //vector: pointer: vector of `T`
        template<typename T> using vec2p = std::vector<std::vector<std::vector<T>>*>; //vector: pointer: vector of vector of `T`

        template<typename T> using pVec1 = std::vector<T>*; //pointer: vector of `T`
        template<typename T> using pVec2 = std::vector<std::vector<T>>*; //pointer: vector of vector of `T`


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
         * Type code definition:
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
        vec0<std::string>   keys;       // Labels/names
        vec0<int>           datatype;   // What type is stored in that index
        vec0<int>           idx;        // "local" index of (what index in the correct vector) where the given element is related to


        int extend_reg(std::string key, int varType);

    public:
        int check_existence(std::string key, int verbose=-1);

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

        diy_dict(/* args */);
        ~diy_dict();

        template<class T>
        std::string formatNumber(T value, int strWidth, int varPrecision, std::string align="right");

        template<typename T> std::string prettyPrint_vec1(vec0<T> vec1_inp, std::string align, int decimals, int width, int padding, int prettyPrint, int left_indent);
        template<typename T> std::string prettyPrint_vec2(vec1<T> vec2_inp, std::string align, int decimals, int width, int padding, int prettyPrint, int left_indent);


        std::string str_export(
            std::string key,
            std::string codedInsert,
            std::string align,
            int decimals,
            int width,
            int padding,
            int prettyPrint,
            char emptySpace,
            int left_indent
        );
        int get_type(std::string key);

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

diy_dict::diy_dict(/* args */) {

}

diy_dict::~diy_dict() {}



// /// @brief add a new key and value pair
// /// @param key `std::string` of key
// /// @param value `bool` value to set to that key
// /// @return `0` if successfully added; `1` if an error occured.
// int diy_dict::add(std::string key, bool value) {
//     int pos = findVectorIndex(keys, key);
//     if(pos != -1) {
//         std::cerr << "diy_dict::add() ERROR: Key already exist" << std::endl;;
//         return 1;
//     }
//     keys.push_back(key);
//     values.push_back(value);
//     return 0;
// }
// /// @brief set the value of a key
// /// @param key `std::string` of key
// /// @param value `bool` value to set to that key
// /// @return `0` if successfully added; `1` if an error occured.
// int diy_dict::set(std::string key, bool value) {
//     int pos = findVectorIndex(keys, key);
//     if(pos == -1) {
//         std::cerr << "diy_dict::set() ERROR: Key doesn't exist" << std::endl;
//         return 1;
//     }
//     values.at(pos) = value;
//     return 0;
// }
// /// @brief get the value of a key
// /// @param key `std::string` of the key to find the value of
// /// @return the value of said key; if key doesn't exist then it'll return `NULL`
// bool get(std::string key) {
//     int pos = findVectorIndex(keys, key);
//     if(pos == -1) {
//         std::cerr << "get() ERROR: Key doesn't exist" << std::endl;
//         std::cout << "get() ERROR: Key doesn't exist: returned value is incorrect" << std::endl;
//         return NULL;
//     }
//     return values.at(pos);
// }

#endif