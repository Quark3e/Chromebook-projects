
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

#include <search_multithread.hpp>


namespace PERF {


    class perf_isolated {
        private:
        std::string _info_name = "PERF::perf_isolated";

        float delayFilter = 1;

        std::vector<std::string>        _names;
        /**
         * Contains the read delays as well as a history which is averaged after a `::_readCnt[index]` number of times.
         * The last element contains the lates data.
         */
        std::vector<std::vector<float>> _delays_ms;
        std::vector<size_t>               _readCnt; //whether to save the read delays into a history and if so how many cycles until an update
        std::vector<std::vector<std::chrono::_V2::steady_clock::time_point>>    _times;

        
        void _call_error(int code, std::string from_member="", std::string custom_error="") {
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

        public:
        float operator[](std::string name) {
            int pos = DIY_SEARCH_MULTITHREAD::check_existence<std::string>(name, this->_names);
            if(pos<0) this->_call_error(0, "operator[](std::string)");
            return this->_delays_ms.at(pos).at(this->_delays_ms.at(pos).size()-1);
        }
        
        std::vector<std::string>        names() { return this->_names; }
        std::vector<std::vector<float>>delays_history() { return this->_delays_ms; }
        std::vector<float>             delays() {
            std::vector<float> _temp;
            for(size_t i=0; i<_delays_ms.size(); i++) {
                _temp.push_back(_delays_ms.at(i).at(_delays_ms[i].size()-1));
            }
            return _temp;
        }
        std::vector<size_t>              readCnt() { return this->_readCnt; }

        perf_isolated() {}
        perf_isolated(std::initializer_list<std::string> init_names) {
            if(init_names.size()==0) throw std::runtime_error("ERROR: "+this->_info_name+"(std::initializer_list<std::string>): initializer list can't have the length of 0");
            for(auto name: init_names) {
                this->_names.push_back(name);
                this->_delays_ms.push_back(std::vector<float>{0});
                this->_readCnt.push_back(1);
                this->_times.push_back(std::vector<std::chrono::_V2::steady_clock::time_point>(2,std::chrono::steady_clock::now()));
            }
        }
        perf_isolated(std::vector<std::string> init_names) {
            if(init_names.size()==0) throw std::runtime_error("ERROR: "+this->_info_name+"(std::initializer_list<std::string>): initializer list can't have the length of 0");
            for(auto name: init_names) {
                this->_names.push_back(name);
                this->_delays_ms.push_back(std::vector<float>{0});
                this->_readCnt.push_back(1);
                this->_times.push_back(std::vector<std::chrono::_V2::steady_clock::time_point>(2,std::chrono::steady_clock::now()));
            }
        }
        ~perf_isolated() {}

        int set_T0(std::string name, size_t cycle_count = 1) {
            std::chrono::_V2::steady_clock::time_point tempTime = std::chrono::steady_clock::now();
            if(cycle_count<1) cycle_count = 1;
            int pos = (this->_names.size()<1? -1 : DIY_SEARCH_MULTITHREAD::check_existence<std::string>(name, this->_names));
            if(pos<0) { //name doesn't exist in system
                this->_names.push_back(name);
                this->_times.push_back(std::vector<std::chrono::_V2::steady_clock::time_point>{tempTime, tempTime});
                // std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(tempTime-tempTime);
                this->_readCnt.push_back(cycle_count);
                this->_delays_ms.push_back(std::vector<float>{0});
            }
            else { //name already exists. Just update start time
                this->_times[pos][0] = tempTime;
            }
            return 0;
        }
        int set_T1(std::string name) {
            std::chrono::_V2::steady_clock::time_point tempTime = std::chrono::steady_clock::now();
            int pos = DIY_SEARCH_MULTITHREAD::check_existence<std::string>(name, this->_names);
            if(pos<0) this->_call_error(0,"set_T1(std::string)","input `name` \""+name+"\" has not been initialised with perf_isolated::set_T0()");
            this->_times[pos][1]    = tempTime;
            float _tempDelay = (std::chrono::duration_cast<std::chrono::milliseconds>(this->_times[pos][1]-this->_times[pos][0])).count();
            if(_delays_ms[pos].size()+1>=_readCnt[pos]) {
                float sumDelay = _tempDelay;
                for(auto delay : _delays_ms[pos]) sumDelay += delay;
                _delays_ms[pos] = std::vector<float>{sumDelay/_readCnt[pos]};
            }
            else {
                _delays_ms[pos].push_back(_tempDelay);
            }
            this->_readCnt[pos]++;
            return 0;
        }

        float getDelay_ms(std::string name) {
            int pos = DIY_SEARCH_MULTITHREAD::check_existence<std::string>(name, this->_names);
            if(pos<0) this->_call_error(0, "getDelay_ms(std::string)");
            return this->_delays_ms[pos][0];
        }

    };

}


#endif