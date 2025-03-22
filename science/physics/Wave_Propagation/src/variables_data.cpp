
#include "variables_data.hpp"

pos2d<size_t> system_dim{1000, 1000};

WaveSource::WaveSource(pos2d<double> _pos, double _wavelength): pos(_pos), wavelength(_wavelength) {

}
double WaveSource::getPhaseShiftAngle(double distance) {
    return (2*M_PI)*(distance/wavelength);
}
pos2d<double> WaveSource::getPhaseShiftVector(double distance) {
    double theta = this->getPhaseShiftAngle(distance);
    return pos2d<double>(cos(theta), sin(theta));
}

BMP_SystemWaveData::BMP_SystemWaveData(size_t __width, size_t __height, size_t __numColourChannels, bool __initElements): _width(__width), _height(__height), _bitmap_colourChannels(__numColourChannels) {
    if(__width==0) throw std::invalid_argument("ERROR: BMP_SystemWaveData(size_t, size_t, size_t, bool): __width arg cannot be 0.");
    if(__height==0) throw std::invalid_argument("ERROR: BMP_SystemWaveData(size_t, size_t, size_t, bool): __height arg cannot be 0.");
    if(__numColourChannels==0) throw std::invalid_argument("ERROR: BMP_SystemWaveData(size_t, size_t, size_t, bool): __numColourChannels cannot be 0.");

    size_t numElements = _width*_height;
    this->_pos = std::vector<pos2d<double>>(numElements);
    this->_sum_amplitude = std::vector<double>(numElements);
    this->_sum_phaseVector = std::vector<pos2d<double>>(numElements);
    this->_indiv_amplitude = std::vector<std::vector<double>>(numElements);
    this->_indiv_phaseVector = std::vector<std::vector<pos2d<double>>>(numElements);
    this->_bitmap_arr = std::vector<uint8_t>(numElements*_bitmap_colourChannels);

    if(__initElements) {
        for(size_t i=0; i<numElements; i++) {
            this->_pos[i] = pos2d<double>(0, 0);
            this->_sum_amplitude[i] = 0;
            this->_sum_phaseVector[i] = pos2d<double>(0, 0);
            this->_indiv_amplitude[i] = std::vector<double>(0, 0);
            this->_indiv_phaseVector[i] = std::vector<pos2d<double>>(0, pos2d<double>(0, 0));

            for(size_t cN=0; cN<_bitmap_colourChannels; cN++) {
                this->_bitmap_arr[i+cN] = 0;
            }
        }
        
    }
}
BMP_SystemWaveData::~BMP_SystemWaveData() {

}

BMP_SystemWaveData& BMP_SystemWaveData::operator==(BMP_SystemWaveData& const __obj) {
    this->_pos = __obj._pos;
    this->_sum_amplitude = __obj._sum_amplitude;
    this->_sum_phaseVector = __obj._sum_phaseVector;
    this->_indiv_amplitude = __obj._indiv_amplitude;
    this->_indiv_phaseVector = __obj._indiv_phaseVector;
    this->_bitmap_arr = __obj._bitmap_arr;

    return *this;
}
void BMP_SystemWaveData::resize_dim(size_t newWidth, size_t newHeight, size_t sizingMethod) {
    /**
     * 
     * `0` - just resize and clear the existing data. Fastest option.
     * ´1´ - resize and add in data but don't change the data.
     * `2` - scale all sides to fit: stretch the dimensions to fit even if the x and y scalings are uneven.
     * ´3` - scale even to fit for minimum: fit according to the smallest delta: not allow cropping
     * ´4´ - scale even to fit for maximum: fit according to the biggest delta: allow cropping
     * 
     */

    if(newWidth==0) throw std::invalid_argument("ERROR: resize_dim(size_t, size_t, size_t, bool): newWidth arg cannot be 0.");
    if(newHeight==0) throw std::invalid_argument("ERROR: resize_dim(size_t, size_t, size_t, bool): newHeight arg cannot be 0.");
    if(newWidth == _width && newHeight == _height) return;

    pos2d<size_t> newDim(newWidth, newHeight);
    pos2d<long int> delta(long int(newWidth)-long int(_width), long int(newHeight)-long int(_height));
    pos2d<size_t> dim_lim(0, 0);
    pos2d<float> scalars(float(newDim.x)/float(_width), float(newDim.y)/float(_height));

    size_t newNumElements = newWidth * newHeight;

    size_t delta_minmax[2] = {0, 0};
    
    if(delta.x<delta.y) {
        delta_minmax[0] = 0;
        delta_minmax[1] = 1;
    }
    else {
        delta_minmax[0] = 1;
        delta_minmax[1] = 0;
    }
    switch (sizingMethod) {
        case 0: break;
        case 1:
            scalars.x = 1;
            scalars.y = 1;
            break;
        case 2: break;
        case 3:
            scalars = pos2d<float>(scalars[delta_minmax[0]], scalars[delta_minmax[0]]);
            break;
        case 4:
            scalars = pos2d<float>(scalars[delta_minmax[1]], scalars[delta_minmax[1]]);
            break;
        default:
            throw std::invalid_argument("invalid sizingMethod arg.");
    }
    dim_lim = (newDim.cast<float>()*scalars).cast<size_t>();

    auto temp_pos = std::vector<pos2d<double>>(newNumElements);
    auto temp_sum_amplitude = std::vector<double>(newNumElements);
    auto temp_sum_phaseVector = std::vector<pos2d<double>>(newNumElements);
    auto temp_indiv_amplitude = std::vector<std::vector<double>>(newNumElements);
    auto temp_indiv_phaseVector = std::vector<std::vector<pos2d<double>>>(newNumElements);
    auto temp_bitmap_arr = std::vector<uint8_t>(newNumElements*_bitmap_colourChannels);

    if(sizingMethod!=0) {
        size_t absPos = 0, prevAbsPos = 0;
        pos2d<size_t> prevPos(0, 0);
        for(size_t y=0; y<newDim.y; y++) {
            for(size_t x=0; x<newDim.x; x++) {
                
                if(x<dim_lim.x && y<dim_lim.y) {
                    /// Inside new bmp region.
                    prevPos.x = roundf(float(x)/scalars.x);
                    prevPos.y = roundf(float(y)/scalars.y);
                    if(prevPos.x >= _width) throw std::runtime_error("prevPos.x >= _width");
                    if(prevPos.y >= _height) throw std::runtime_error("prevPos.y >= _height");
                    prevAbsPos = _width*prevPos.y + prevPos.x;

                    temp_pos[absPos] = this->_pos[prevAbsPos];
                    temp_sum_amplitude[absPos] = this->_sum_amplitude[prevAbsPos];
                    temp_sum_phaseVector[absPos] = this->_sum_phaseVector[prevAbsPos];
                    temp_indiv_amplitude[absPos] = this->_indiv_amplitude[prevAbsPos];
                    temp_indiv_phaseVector[absPos] = this->_indiv_phaseVector[prevAbsPos];
                    temp_bitmap_arr[absPos] = this->_bitmap_arr[prevAbsPos];
                }
                else {
                    /// Outside bmp region. No value to be used from existing vectors.

                    
                }

                absPos+=1;
            }
        }
    }

    this->_pos = temp_pos;
    this->_sum_amplitude = temp_sum_amplitude;
    this->_sum_phaseVector = temp_sum_phaseVector;
    this->_indiv_amplitude = temp_indiv_amplitude;
    this->_indiv_phaseVector = temp_indiv_phaseVector;
    this->_bitmap_arr = temp_bitmap_arr;

}
void BMP_SystemWaveData::resize_colourChannelNum(size_t newChannelNum) {
    if(newChannelNum==0) throw std::invalid_argument("ERROR: resize_colourChannelNum(size_t, size_t, size_t, bool): newChannelNum arg cannot be 0.");
    if(newChannelNum==this->_bitmap_colourChannels) return;

    // long int deltaSize = long int() - long int(this->_bitmap_arr.size());
    // if(newChannelNum > this->_bitmap_colourChannels) {
    // }
    // else {
    // }

    this->_bitmap_arr = std::vector<uint8_t>(newChannelNum*_width*_height, 0);
}
SystemWaveData_info_ref BMP_SystemWaveData::at(size_t __x, size_t __y) {
    if(__x<0 || __x+1>_width) throw std::invalid_argument("at(size_t, size_t): __x out of width range of "+std::to_string(_width)+": "+std::to_string(__x));
    if(__y<0 || __y+1>_width) throw std::invalid_argument("at(size_t, size_t): __y out of height range of "+std::to_string(_height)+": "+std::to_string(__y));

    size_t absDataPos = __x+__y*_width;

    return SystemWaveData_info_ref{
        pos2d<size_t>{__x, __y},
        _pos[absDataPos],
        _indiv_amplitude[absDataPos],
        _indiv_phaseVector[absDataPos],
        _sum_amplitude[absDataPos],
        _sum_phaseVector[absDataPos]
    };
}
pos2d<double>&  BMP_SystemWaveData::at_pos(size_t __x, size_t __y) {
    if(__x<0 || __x+1>_width) throw std::invalid_argument("at_pos(size_t, size_t): __x out of width range of "+std::to_string(_width)+": "+std::to_string(__x));
    if(__y<0 || __y+1>_width) throw std::invalid_argument("at_pos(size_t, size_t): __y out of height range of "+std::to_string(_height)+": "+std::to_string(__y));
    size_t absDataPos = __x+__y*_width;
    return _pos[absDataPos];
}
double&         BMP_SystemWaveData::at_sum_ampl(size_t __x, size_t __y) {
    if(__x<0 || __x+1>_width) throw std::invalid_argument("at_sum_ampl(size_t, size_t): __x out of width range of "+std::to_string(_width)+": "+std::to_string(__x));
    if(__y<0 || __y+1>_width) throw std::invalid_argument("at_sum_ampl(size_t, size_t): __y out of height range of "+std::to_string(_height)+": "+std::to_string(__y));
    size_t absDataPos = __x+__y*_width;
    return _sum_amplitude[absDataPos];
}
pos2d<double>&  BMP_SystemWaveData::at_sum_phaseVec(size_t __x, size_t __y) {
    if(__x<0 || __x+1>_width) throw std::invalid_argument("at_sum_phaseVec(size_t, size_t): __x out of width range of "+std::to_string(_width)+": "+std::to_string(__x));
    if(__y<0 || __y+1>_width) throw std::invalid_argument("at_sum_phaseVec(size_t, size_t): __y out of height range of "+std::to_string(_height)+": "+std::to_string(__y));
    size_t absDataPos = __x+__y*_width;
    return _sum_phaseVector[absDataPos];
}
std::vector<double>& BMP_SystemWaveData::at_indiv_ampl(size_t __x, size_t __y) {
    if(__x<0 || __x+1>_width) throw std::invalid_argument("at_indiv_ampl(size_t, size_t):__x out of width range of "+std::to_string(_width)+": "+std::to_string(__x));
    if(__y<0 || __y+1>_width) throw std::invalid_argument("at_indiv_ampl(size_t, size_t):__y out of height range of "+std::to_string(_height)+": "+std::to_string(__y));
    size_t absDataPos = __x+__y*_width;
    return _indiv_amplitude[absDataPos];
}
std::vector<pos2d<double>>& BMP_SystemWaveData::at_indiv_phaseVec(size_t __x, size_t __y) {
    if(__x<0 || __x+1>_width) throw std::invalid_argument("at_indiv_phaseVec(size_t, size_t):__x out of width range of "+std::to_string(_width)+": "+std::to_string(__x));
    if(__y<0 || __y+1>_width) throw std::invalid_argument("at_indiv_phaseVec(size_t, size_t):__y out of height range of "+std::to_string(_height)+": "+std::to_string(__y));
    size_t absDataPos = __x+__y*_width;
    return _indiv_phaseVector[absDataPos];
}
// SystemWaveData_info BMP_SystemWaveData::at(size_t __x, size_t __y) const {
//     if(__x<0 || __x+1>_width) throw std::invalid_argument("__x out of width range of "+std::to_string(_width)+": "+std::to_string(__x));
//     if(__y<0 || __y+1>_width) throw std::invalid_argument("__y out of height range of "+std::to_string(_height)+": "+std::to_string(__x));
//     size_t absDataPos = __x+__y*_width;
//     return SystemWaveData_info{
//         _pos[absDataPos],
//         _indiv_amplitude[absDataPos],
//         _indiv_phaseVector[absDataPos],
//         _sum_amplitude[absDataPos],
//         _sum_phaseVector[absDataPos]
//     };
// }
std::vector<uint8_t>& BMP_SystemWaveData::get_BMP_arr() {
    return _bitmap_arr;
}


BMP_SystemWaveData BMP_system__thread_calc{system_dim.x/pixelSpacing, system_dim.y/pixelSpacing, 1};
BMP_SystemWaveData BMP_system__thread_gui{ system_dim.x/pixelSpacing, system_dim.y/pixelSpacing, 1};

BMP_SystemWaveData* ptr_BMP_system_calc = &BMP_system__thread_calc; //"write" object ptr
BMP_SystemWaveData* ptr_BMP_system_gui  = &BMP_system__thread_gui;  //"read" object ptr


std::mutex mtx__cout;
std::mutex mtx__ptrBMPsystem_Switch;
std::mutex mtx__abs_cam_pixelPos_Access;
std::mutex mtx__system_waves_Access;
std::mutex mtx__meter_per_px_Access;

std::atomic<bool> atm__running_process_calc{true};

std::atomic<size_t> maxThreadNum{0};


size_t pixelSpacing = 1;
pos2d<int> abs_cam_pixelPos(0, 0);

size_t numColour = 2;

std::vector<WaveSource> system_waves{
    // WaveSource{{0.02, 0.0185}, 5*pow(10, -4)},

    WaveSource{{0.02, 0.0385}, 5*pow(10, -4)},
    WaveSource{{0.02, 0.0390}, 5*pow(10, -4)},
    WaveSource{{0.02, 0.0395}, 5*pow(10, -4)},
    WaveSource{{0.02, 0.0400}, 5*pow(10, -4)},
    WaveSource{{0.02, 0.0405}, 5*pow(10, -4)},
    WaveSource{{0.02, 0.0410}, 5*pow(10, -4)},
    WaveSource{{0.02, 0.0415}, 5*pow(10, -4)},
    
    // WaveSource{{0.02, 0.0615}, 5*pow(10, -4)}
};

double meter_per_px = 0.0001;

std::vector<pos2d<float>> detectLine{
    {800,  86},
    {800, 715}
};


// #if takePerformance_processCalc
// DIY::typed_dict<std::string, double> delays__process_calc__calc({
//     {"init prep"        , 0},
//     {"sum amplitude"    , 0},
//     {"sum phaseVector"  , 0},
//     {"define BMP_sys"   , 0},
//     {"get colour scale" , 0},
//     {"define pixel col" , 0},
//     {"Total abs time"   , 0}
// });
// DIY::typed_dict<std::string, double> delays__process_calc__gui(delays__process_calc__calc);
// DIY::typed_dict<std::string, double>* ptr_delays__process_calc_calc = &delays__process_calc__calc;
// DIY::typed_dict<std::string, double>* ptr_delays__process_calc_gui  = &delays__process_calc__gui;
// std::mutex mtx__delays_processCalc_switch;

// #endif //takePerformance_processCalc

