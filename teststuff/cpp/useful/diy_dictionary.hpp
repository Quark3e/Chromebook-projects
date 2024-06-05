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
#include <typeinfo>

#include <HC_useful/useful.hpp>


/**
 * diy dictionary where each key is an std::string
 * 
 */
class diy_dict {
    private:
        const std::string types[] = {
            "bool", "int", "float", "char", "string",
            "p_bool", "p_int", "p_float", "p_char", "p_string"
        };
        std::vector<std::string>    keys;
        std::vector<std::string>    datatype;
        std::vector<int>            idx;

        std::vector<bool>           values_bool;
        std::vector<int>            values_int;
        std::vector<float>          values_float;
        std::vector<char>           values_char;
        std::vector<std::string>    values_string;

        std::vector<bool*>          values_p_bool;
        std::vector<int*>           values_p_int;
        std::vector<float*>         values_p_float;
        std::vector<char*>          values_p_char;
        std::vector<std::string*>   values_p_string;


        std::vector<std::vector<bool>>      values_vec_bool;
        std::vector<std::vector<int>>       values_int;
        std::vector<std::vector<float>>     values_float;
        std::vector<std::vector<char>>      values_char;
        std::vector<std::vector<std::string>> values_string;

        std::vector<bool*>          values_p_bool;
        std::vector<int*>           values_p_int;
        std::vector<float*>         values_p_float;
        std::vector<char*>          values_p_char;
        std::vector<std::string*>   values_p_string;

        void extend_reg(std::string key, std::string varType);
    public:
        diy_dict(/* args */);
        ~diy_dict();

        template<class T>
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