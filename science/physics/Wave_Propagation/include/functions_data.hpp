#pragma once
#ifndef HPP_WAVE_PROPAGATION__FUNCTIONS_DATA
#define HPP_WAVE_PROPAGATION__FUNCTIONS_DATA

#include "includes.hpp"
#include "variables_data.hpp"


pos2d<double> getWaveVectorSum(
    pos2d<double> _pos,
    std::vector<WaveSource> _waves
);
double getWavePhaseAmplitudeSum(
    pos2d<double> _pos,
    std::vector<WaveSource> _waves
);

#endif // HPP_WAVE_PROPAGATION__FUNCTIONS_DATA
