#pragma once
#ifndef HPP__WavePropagation_sim_delays_graph__variables
#define HPP__WavePropagation_sim_delays_graph__variables

#include "includes.hpp"
#include <diy_dictionary.hpp>

/**
 * @brief 2d pixel dimensions of the program window
 * 
 */
extern pos2d<int> dim__programWindow;

/**
 * Dictionary holding all the data/values from csv file.
 * 
 * - The numSubProcess/number-of-threads-used value is primary key.
 * - The column names of the csv file is stored as the secondary keys and each element in that key:value-pair vector holds that columns value 
 * for that given iteration.
 * 
 */
extern DIY::typed_dict<size_t, DIY::typed_dict<std::string, std::vector<float>>> Loaded_Data__perfDelays;

extern void loadCSV_file(std::string _filename);

#endif //HPP__WavePropagation_sim_delays_graph__variables