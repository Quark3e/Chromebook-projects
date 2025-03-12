#pragma once
#ifndef HPP_WAVE_PROPAGATION__VARIABLES_DATA
#define HPP_WAVE_PROPAGATION__VARIABLES_DATA

#include "includes.hpp"


#define unit(val) (val)/std::abs(val)

#define scalMethod 1


struct WaveSource {
    pos2d<double>   pos;
    double          wavelength;

    WaveSource(pos2d<double> _pos, double _wavelength);
    
    double getPhaseShiftAngle(double distance);
    pos2d<double> getPhaseShiftVector(double distance);
};


/// @brief Pixel spacing between each data point pixel
extern size_t pixelSpacing;
/// @brief pixel coordinate of the cam
extern pos2d<int> abs_cam_pixelPos;

/// @brief Generic incomplete variable to declare how many bytes the colour format has.
extern size_t numColour;

/// @brief Vector of `WaveSource`'s in the current system
extern std::vector<WaveSource> system_waves;

/// @brief Conversion for how many meters are represented per pixel.
extern double meter_per_px;

/// @brief End point coordinates for a line that is used to pull a slice/range of data points
extern std::vector<pos2d<float>> detectLine;


#endif //HPP_WAVE_PROPAGATION__VARIABLES_DATA
