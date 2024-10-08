
#pragma once
#ifndef HPP_DIY_PERFORMANCE
#define HPP_DIY_PERFORMANCE


#include <iostream>

#include <string>
#include <sstream>
#include <iomanip>

#include <vector>
#include <list>
#include <initializer_list>

#include <time.h>
#include <chrono>
#include <ctime>

#include <HC_useful/search_multithread.hpp>


namespace PERF {



    class perf_isolated {
        private:
        std::string _info_name = "PERF::perf_isolated";

        float delayFilter = 1;

        std::vector<std::string>    _names;
        std::vector<float>          _delays_ms;
        std::vector<std::vector<std::chrono::_V2::steady_clock::time_point>>    _times;

        
        void _call_error(int code, std::string from_member="", std::string custom_error="");

        public:
        float operator[](std::string name) {
            int pos = DIY_SEARCH_MULTITHREAD::check_existence<std::string>(name, this->_names);
            if(pos<0) this->_call_error(0, "operator[](std::string)");
            return this->_delays_ms.at(pos);
        }
        
        std::vector<std::string>    names() { return this->_names; }
        std::vector<float>         delays() { return this->_delays_ms; }

        perf_isolated();
        perf_isolated(std::initializer_list<std::string> init_names);
        perf_isolated(std::vector<std::string> init_names);
        ~perf_isolated();

        int set_T0(std::string name);
        int set_T1(std::string name);

        float getDelay_ms(std::string name);

    };
    
    perf_isolated::perf_isolated(/* args */) {

    }
    perf_isolated::~perf_isolated() {}

}
void PERF::perf_isolated::_call_error(int code, std::string from_member, std::string custom_error) {
    if(from_member.length()!=0) from_member = "::"+from_member;
    std::string callStr = "ERROR: "+this->_info_name+from_member+": ";
    if(custom_error.length()!=0) callStr+=custom_error;
    else {
        switch (code) {
        case 0: //key not found
            callStr += " input name argument not found in _names storage";
            break;
        case 1: //key already exists
            callStr += " input name argument already exists in _names storage";
            break;
        default:
            break;
        }
    }
    throw std::runtime_error(callStr);
}


PERF::perf_isolated::perf_isolated(std::initializer_list<std::string> init_names) {
    if(init_names.size()==0) throw std::runtime_error("ERROR: "+this->_info_name+"(std::initializer_list<std::string>): initializer list can't have the length of 0");
    for(auto name: init_names) {
        this->_names.push_back(name);
        this->_delays_ms.push_back(0);
        this->_times.push_back(std::vector<std::chrono::_V2::steady_clock::time_point>(2,std::chrono::steady_clock::now()));
    }
}
PERF::perf_isolated::perf_isolated(std::vector<std::string> init_names) {
    if(init_names.size()==0) throw std::runtime_error("ERROR: "+this->_info_name+"(std::initializer_list<std::string>): initializer list can't have the length of 0");
    for(auto name: init_names) {
        this->_names.push_back(name);
        this->_delays_ms.push_back(0);
        this->_times.push_back(std::vector<std::chrono::_V2::steady_clock::time_point>(2,std::chrono::steady_clock::now()));
    }
}

int PERF::perf_isolated::set_T0(std::string name) {
    std::chrono::_V2::steady_clock::time_point tempTime = std::chrono::steady_clock::now();
    int pos = DIY_SEARCH_MULTITHREAD::check_existence<std::string>(name, this->_names);
    if(pos<0) { //name already exists in system
        this->_names.push_back(name);
        this->_times.push_back(std::vector<std::chrono::_V2::steady_clock::time_point>{tempTime, tempTime});
        // std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tempTime-tempTime);
        this->_delays_ms.push_back(0);
    }
    else { //name doesn't exist. create a new element(s) with said name
        this->_times[pos][0] = tempTime;
    }
    return 0;
}
int PERF::perf_isolated::set_T1(std::string name) {
    std::chrono::_V2::steady_clock::time_point tempTime = std::chrono::steady_clock::now();
    int pos = DIY_SEARCH_MULTITHREAD::check_existence<std::string>(name, this->_names);
    if(pos<0) this->_call_error(0,"set_T1(std::string)","input `name` \""+name+"\" has not been initialised with perf_isolated::set_T0()");

    this->_times[pos][1]    = tempTime;
    this->_delays_ms[pos]   = (std::chrono::duration_cast<std::chrono::milliseconds>(this->_times[pos][1]-this->_times[pos][0])).count();
}


float PERF::perf_isolated::getDelay_ms(std::string name) {
    int pos = DIY_SEARCH_MULTITHREAD::check_existence<std::string>(name, this->_names);
    if(pos<0) this->_call_error(0, "getDelay_ms(std::string)");
    return this->_delays_ms[pos];
}



#endif