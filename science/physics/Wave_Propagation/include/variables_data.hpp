#pragma once
#ifndef HPP_WAVE_PROPAGATION__VARIABLES_DATA
#define HPP_WAVE_PROPAGATION__VARIABLES_DATA

#include "includes.hpp"


#define unit(val) (val)/std::abs(val)

#define scalMethod 1
#define takePerformance_processCalc true
#define takePerformance_processCalc_saveDelays false


/**
 * Pixel dimensions of the system that holds everything.
 * 
 */
extern pos2d<size_t> system_dim;

struct WaveSource {
    pos2d<double>   pos;
    double          wavelength;

    WaveSource(pos2d<double> _pos, double _wavelength);
    
    double getPhaseShiftAngle(double distance);
    pos2d<double> getPhaseShiftVector(double distance);
};

struct SystemWaveData_info_ref {
    pos2d<size_t>   &idx;
    pos2d<double>   &pos;
    
    std::vector<double>         &indiv_amplitude;
    std::vector<pos2d<double>>  &indiv_phaseVector;

    double          &sum_amplitude;
    pos2d<double>   &sum_phaseVector;

    friend auto operator<<(std::ostream &os, SystemWaveData_info_ref const& m) -> std::ostream& {
        os << m.idx << " {"<<"";
        os << "" << m.pos << "," <<"";
        os << "" << formatVector(m.indiv_amplitude, 10, 7) << ","<<"";
        os << "" << formatVector(m.indiv_phaseVector, 10, 7) << ","<<"";
        os << "" << m.sum_amplitude << ","<<"";
        os << "" << m.sum_phaseVector<<"";
        os << "}"<<"";
        return os;
    }
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
    DIY::typed_dict<std::string, double> delays__process_calc = DIY::typed_dict<std::string, double>({
        {"init prep"        , 0},
        {"sum amplitude"    , 0},
        {"sum phaseVector"  , 0},
        {"define BMP_sys"   , 0},
        {"get colour scale" , 0},
        {"define pixel col" , 0},
        {"Process time"   , 0}
    });
    float delays__process_calc_totalTime = 0;



    BMP_SystemWaveData(size_t __width, size_t __height, size_t __numColourChannels, bool __initElements=true);
    ~BMP_SystemWaveData();

    BMP_SystemWaveData& operator==(const BMP_SystemWaveData& __obj);

    SystemWaveData_info_ref at(size_t __x, size_t __y);
    // SystemWaveData_info  at(size_t __x, size_t __y) const;
    
    pos2d<double>&  at_pos(size_t __x, size_t __y);
    double&         at_sum_ampl(size_t __x, size_t __y);
    pos2d<double>&  at_sum_phaseVec(size_t __x, size_t __y);
    std::vector<double>& at_indiv_ampl(size_t __x, size_t __y);
    std::vector<pos2d<double>>& at_indiv_phaseVec(size_t __x, size_t __y);

    void resize_dim(size_t newWidth, size_t newHeight, size_t sizingMethod=0);
    void resize_colourChannelNum(size_t newChannelNum);
    
    std::vector<uint8_t>& get_BMP_arr();
};

extern BMP_SystemWaveData BMP_system__thread_calc;
extern BMP_SystemWaveData BMP_system__thread_gui;

extern BMP_SystemWaveData* ptr_BMP_system_calc; //"write" object ptr
extern BMP_SystemWaveData* ptr_BMP_system_gui;  //"read" object ptr


extern std::mutex mtx__cout;                    // terminal printing mutex
extern std::mutex mtx__ptrBMPsystem_Switch;     // mutex for switching `BMP_system__thread_` object addresses for reading/writing.
extern std::mutex mtx__abs_cam_pixelPos_Access; // mutex for accessing `abs_cam_pixelPos` 2d variable
extern std::mutex mtx__system_waves_Access;     // mutex for accessing `system_waves`
extern std::mutex mtx__meter_per_px_Access;     // mutex for accessing `meter_per_px` variable

/// @brief atomic boolean read by the calc-thread to check whether it is to continue running.
extern std::atomic<bool> atm__running_process_calc;

extern std::atomic<size_t> maxThreadNum;


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

/// @brief End point coordinates for a line that is used to pull a slice/range of data points in pixel coordinates
extern std::vector<pos2d<float>> detectLine;


// #if takePerformance_processCalc
// /**
//  * Dicitonary used to hold processed/delays result values for different parts of the calculation methods.
//  * Used to figure out delays and such. Only accessible through 
//  * 
//  */
// extern DIY::typed_dict<std::string, double> delays__process_calc__calc;
// /// @brief copy of `delays__process_calc` that is used by the gui for drawing.
// extern DIY::typed_dict<std::string, double> delays__process_calc__gui;
// extern DIY::typed_dict<std::string, double>* ptr_delays__process_calc_calc;
// extern DIY::typed_dict<std::string, double>* ptr_delays__process_calc_gui;
// /**
//  * Mutex used specifically for accessing the delays__process_calc__gui typed_dict. This mutex is used to copy over
//  * results from the calc-thread version of the dict onto the gui version.
//  */
// extern std::mutex mtx__delays_processCalc_switch;
// #endif //takePerformance_processCalc

#endif //HPP_WAVE_PROPAGATION__VARIABLES_DATA
