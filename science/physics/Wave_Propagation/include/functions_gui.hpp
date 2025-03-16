#pragma once
#ifndef HPP_WAVE_PROPAGATION__FUNCTIONS_GUI
#define HPP_WAVE_PROPAGATION__FUNCTIONS_GUI

#include "includes.hpp"
#include "variables_data.hpp"


inline void mutex_cout(std::string _print, std::mutex& _cout_mtx, std::string _end="\n", bool _flushEnd=true) {
    std::unique_lock lck_cout(_cout_mtx, std::defer_lock);
    lck_cout.lock();
    std::cout << _print << _end;
    if(_flushEnd) std::cout.flush();
    lck_cout.unlock();
}

/**
 * @brief Function to get a relative {-1.0, 1.0} scalar value for a given value in accordance to the system waves
 * 
 * @param _objPos realpos coordinate to solve the scalar for.
 * @param _system_waves `std::vector<>` of the system waves stored in `WaveSource` struct.
 * @param _lim the minimum/maximum value range
 * @param _method method of getting the scalar value for the realpos coordinate
 * @return float 
 */
float scalarMethod(pos2d<double> &_objPos, std::vector<WaveSource> &_system_waves, std::vector<double> &_lim, int _method=0);

std::vector<uint8_t> graph_colour_scale(float _val);



#endif //HPP_WAVE_PROPAGATION__FUNCTIONS_GUI
