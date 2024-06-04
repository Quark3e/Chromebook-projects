#pragma once
#ifndef HPP_DIY_DICTIONARY
#define HPP_DIY_DICTIONARY


#include <iostream>
#include <string>

#include <HC_useful/useful.hpp>


/**
 * diy dictionary where each key is an std::string
 * 
 */
class diy_dict {
    private:
        std::vector<std::string> keys;
        

        std::vector<int>            values_int;
        std::vector<float>          values_float;
        std::vector<bool>           values_bool;
        std::vector<std::string>    values_string;
    public:
        diy_dict(/* args */);
        ~diy_dict();

        int add(std::string key, bool value);
        int set(std::string key, bool value);
        bool get(std::string key);
};

diy_dict::diy_dict(/* args */) {


}

diy_dict::~diy_dict() {

}

/// @brief add a new key and value pair
/// @param key `std::string` of key
/// @param value `bool` value to set to that key
/// @return `0` if successfully added; `1` if an error occured.
int diy_dict::add(std::string key, bool value) {
    int pos = findVectorIndex(keys, key);
    if(pos != -1) {
        std::cerr << "diy_dict::add() ERROR: Key already exist" << std::endl;;
        return 1;
    }
    keys.push_back(key);
    values.push_back(value);
    return 0;
}
/// @brief set the value of a key
/// @param key `std::string` of key
/// @param value `bool` value to set to that key
/// @return `0` if successfully added; `1` if an error occured.
int diy_dict::set(std::string key, bool value) {
    int pos = findVectorIndex(keys, key);
    if(pos == -1) {
        std::cerr << "diy_dict::set() ERROR: Key doesn't exist" << std::endl;
        return 1;
    }
    values.at(pos) = value;
    return 0;
}
/// @brief get the value of a key
/// @param key `std::string` of the key to find the value of
/// @return the value of said key; if key doesn't exist then it'll return `NULL`
bool diy_dict::get(std::string key) {
    int pos = findVectorIndex(keys, key);
    if(pos == -1) {
        std::cerr << "diy_dict::get() ERROR: Key doesn't exist" << std::endl;
        std::cout << "diy_dict::get() ERROR: Key doesn't exist: returned value is incorrect" << std::endl;
        return NULL;
    }
    return values.at(pos);
}

#endif