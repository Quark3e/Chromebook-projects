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

struct SystemWaveData_info_ref {
    pos2d<double>   &pos;
    
    std::vector<double>         &indiv_amplitude;
    std::vector<pos2d<double>>  &indiv_phaseVector;

    double          &sum_amplitude;
    pos2d<double>   &sum_phaseVector;
};

struct SystemWaveData_info {
    pos2d<double>   pos;
    
    std::vector<double>         indiv_amplitude;
    std::vector<pos2d<double>>  indiv_phaseVector;

    double          sum_amplitude;
    pos2d<double>   sum_phaseVector;
};
class BMP_SystemWaveData {
    private:

    size_t _width;
    size_t _height;

    size_t _bitmap_colourChannels;

    std::vector<pos2d<double>>  _pos;

    std::vector<double>         _sum_amplitude;
    std::vector<pos2d<double>>  _sum_phaseVector;

    std::vector<std::vector<double>>        _indiv_amplitude;
    std::vector<std::vector<pos2d<double>>> _indiv_phaseVector;

    std::vector<uint8_t> _bitmap_arr;

    public:

    BMP_SystemWaveData(size_t __width, size_t __height, size_t __numColourChannels, bool __initElements=true);
    ~BMP_SystemWaveData();

    BMP_SystemWaveData& operator==(BMP_SystemWaveData& const __obj);

    SystemWaveData_info_ref at(size_t __x, size_t __y);
    SystemWaveData_info  at(size_t __x, size_t __y) const;

    void resize_dim(size_t newWidth, size_t newHeight, size_t sizingMethod=0);
    void resize_colourChannelNum(size_t newChannelNum);
    
    std::vector<uint8_t> get_BMP_arr();
};

extern BMP_SystemWaveData BMP_system__thread_calc;
extern BMP_SystemWaveData BMP_system__thread_gui;

extern BMP_SystemWaveData* ptr_BMP_system_calc; //"write" object ptr
extern BMP_SystemWaveData* ptr_BMP_system_gui;  //"read" object ptr


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
