#pragma once
#ifndef HPP__WavePropagation_sim_delays_graph__graph_selections
#define HPP__WavePropagation_sim_delays_graph__graph_selections

#include "includes.hpp"
#include <diy_dictionary.hpp>

extern DIY::typed_dict<std::string, void (*)()> graph_selections;


extern size_t selected__graph_selections;

/**
 * @brief avg times per thread-count, includes all delays shown in a decent manner (seconds/milliseconds for process/abs-calc-process time)
 * 
 */
void graph__default();
/**
 * @brief Total time per iterations.
 * 
 */
void graph__totalTime();
void graph__processTime();
void graph__iterationDelays();


#endif //HPP__WavePropagation_sim_delays_graph__graph_selections