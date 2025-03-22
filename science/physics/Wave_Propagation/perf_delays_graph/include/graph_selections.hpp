#pragma once
#ifndef HPP__WavePropagation_sim_delays_graph__graph_selections
#define HPP__WavePropagation_sim_delays_graph__graph_selections

#include "includes.hpp"
#include <diy_dictionary.hpp>

extern DIY::typed_dict<std::string, void (*)()> graph_selections;


void graph__default();
void graph__totalTime();
void graph__processTime();
void graph__iterationDelays();


#endif HPP__WavePropagation_sim_delays_graph__graph_selections