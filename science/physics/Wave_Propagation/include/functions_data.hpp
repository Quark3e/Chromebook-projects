#pragma once
#ifndef HPP_WAVE_PROPAGATION__FUNCTIONS_DATA
#define HPP_WAVE_PROPAGATION__FUNCTIONS_DATA

#include "includes.hpp"
#include "variables_data.hpp"


pos2d<double> getWaveVectorSum(
    pos2d<double> _pos,
    std::vector<WaveSource> _waves,
    std::vector<pos2d<double>>* _copy_indiv_phaseShift = nullptr
);
double getWavePhaseAmplitudeSum(
    pos2d<double> _pos,
    std::vector<WaveSource> _waves,
    std::vector<double>* _copy_indiv_amplitude = nullptr
);

#endif // HPP_WAVE_PROPAGATION__FUNCTIONS_DATA
