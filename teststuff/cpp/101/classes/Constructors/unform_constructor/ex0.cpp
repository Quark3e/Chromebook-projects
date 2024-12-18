/**
 * @file ex0.cpp
 * @author your name (you@domain.com)
 * @brief try what happens if you use `{}` to initialise a class object
 * @version 0.1
 * @date 2024-06-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include <useful.hpp>

// template<class key, class value>
class ex0 {
    private:
    /* data */
    public:
    ex0(/* args */);
    ~ex0();
};
ex0::ex0(/* args */) {}
ex0::~ex0() {}


template<class key, class value>
class vecEx {
    private:
    std::vector<key> _keys;
    std::vector<value> _values;
    public:
    vecEx(/* args */) {}
    vecEx(std::vector<key> keys, std::vector<value> values);
};
template<class key, class value>
vecEx<key, value>::vecEx(std::vector<key> keys, std::vector<value> values) {
    if(keys.size() == values.size()) {
        this->_keys = keys;
        this->_values = values;
    }
}


class vecEx0 {
    private:
    std::vector<std::string>    _keys;
    std::vector<bool>           _values;
    
    bool _init_vec = false;
    public:
    vecEx0(std::vector<std::string> keys, std::vector<bool> values);
    void print();
};

vecEx0::vecEx0(std::vector<std::string> keys, std::vector<bool> values) {
    if(keys.size() == values.size()) {
        this->_keys = keys;
        this->_values = values;
        this->_init_vec = true;
    }
}
void vecEx0::print() {
    if(!this->_init_vec) return;
    for(size_t i=0; i<this->_keys.size(); i++)
        std::cout << " |"<<formatNumber(this->_keys.at(i), 5, 0)<<"| : "<<std::boolalpha<<this->_values.at(i)<<std::noboolalpha<<std::endl;
}



int main(int argc, char** argv) {
    std::vector<int> vec_int{0, 1, 2, 3, 4};
    for(int elem: vec_int) std::cout << elem << std::endl;

    vecEx<std::string, int> vecInst(
        std::vector<std::string>{"one", "two", "three"},
        std::vector<int>{1, 2, 3}
    );


    vecEx0 vecBool(
        std::vector<std::string>{"T", "T", "F", "T", "F"},
        std::vector<bool>{true, true, false, true, false}
    );
    vecBool.print();

    return 0;
}
