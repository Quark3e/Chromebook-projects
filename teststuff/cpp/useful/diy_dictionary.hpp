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
// #include <typeinfo>

#include <HC_useful/useful.hpp>


/**
 * diy dictionary where each key is an std::string
 * 
 */
class diy_dict {
    private:
        template<typename T> using vec0 = std::vector<T>;
        template<typename T> using vec1 = std::vector<std::vector<T>>;
        template<typename T> using vec2 = std::vector<std::vector<std::vector<T>>>;


        vec0<bool>          values_0_bool;
        vec0<int>           values_0_int;
        vec0<float>         values_0_float;
        vec0<double>        values_0_double;
        vec0<char>          values_0_char;
        vec0<std::string>   values_0_string;

        vec0<bool*>         values_0_bool_p;
        vec0<int*>          values_0_int_p;
        vec0<float*>        values_0_float_p;
        vec0<double*>       values_0_double_p;
        vec0<char*>         values_0_char_p;
        vec0<std::string*>  values_0_string_p;


        vec1<bool>          values_1_bool;
        vec1<int>           values_1_int;
        vec1<float>         values_1_float;
        vec1<double>        values_1_double;
        vec1<char>          values_1_char;
        vec1<std::string>   values_1_string;

        vec1<bool*>         values_1_bool_p;
        vec1<int*>          values_1_int_p;
        vec1<float*>        values_1_float_p;
        vec1<double*>       values_1_double_p;
        vec1<char*>         values_1_char_p;
        vec1<std::string*>  values_1_string_p;


        vec2<bool>          values_2_bool;
        vec2<int>           values_2_int;
        vec2<float>         values_2_float;
        vec2<double>        values_2_double;
        vec2<char>          values_2_char;
        vec2<std::string>   values_2_string;

        vec2<bool*>         values_2_bool_p;
        vec2<int*>          values_2_int_p;
        vec2<float*>        values_2_float_p;
        vec2<double*>       values_2_double_p;
        vec2<char*>         values_2_char_p;
        vec2<std::string*>  values_2_string_p;



        const std::string types[100][6] = {
            {"0_0_", "0_1_", "0_2_", "0_3_", "0_4_", "0_5_"},
            {"0_0p", "0_1p", "0_2p", "0_3p", "0_4p", "0_5p"},
            {"1_0_", "1_1_", "1_2_", "1_3_", "1_4_", "1_5_"},
            {"1_0p", "1_1p", "1_2p", "1_3p", "1_4p", "1_5p"},
            {"2_0_", "2_1_", "2_2_", "2_3_", "2_4_", "2_5_"},
            {"2_0p", "2_1p", "2_2p", "2_3p", "2_4p", "2_5p"},
        };
        vec0<std::string>   keys;       // Labels/names
        vec0<std::string>   datatype;   // What type is stored in that index
        vec0<int>           idx;        // "local" index of (what index in the correct vector) where the given element is related to



        void extend_reg(std::string key, std::string varType);
    public:
        diy_dict(/* args */);
        ~diy_dict();

        int add(std::string key, int value)
        int add(std::string key, bool value)
        int add(std::string key, float value)
        int add(std::string key, int value)
        int add(std::string key, int value)
        int add(std::string key, int value)
        int add(std::string key, int value)
        int add(std::string key, int value)

        int set(std::string key, bool value);
        bool get(std::string key);
};

diy_dict::diy_dict(/* args */) {


}

diy_dict::~diy_dict() {}


template<class T>
int diy_dict::add(std::string key, T value) {
    if()
}



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
// bool diy_dict::get(std::string key) {
//     int pos = findVectorIndex(keys, key);
//     if(pos == -1) {
//         std::cerr << "diy_dict::get() ERROR: Key doesn't exist" << std::endl;
//         std::cout << "diy_dict::get() ERROR: Key doesn't exist: returned value is incorrect" << std::endl;
//         return NULL;
//     }
//     return values.at(pos);
// }

#endif