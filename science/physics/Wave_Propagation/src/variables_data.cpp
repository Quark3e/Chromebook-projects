
#include "variables_data.hpp"

WaveSource::WaveSource(pos2d<double> _pos, double _wavelength): pos(_pos), wavelength(_wavelength) {

}
double WaveSource::getPhaseShiftAngle(double distance) {
    return (2*M_PI)*(distance/wavelength);
}
pos2d<double> WaveSource::getPhaseShiftVector(double distance) {
    double theta = this->getPhaseShiftAngle(distance);
    return pos2d<double>(cos(theta), sin(theta));
}

size_t pixelSpacing = 2;
pos2d<int> abs_cam_pixelPos(0, 0);

size_t numColour = 1;

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


std::vector<uint8_t> bitArr_subThread;
std::vector<uint8_t> bitArr_mainThread;

std::mutex mtx__bitArr_Transfer;
std::mutex mtx__abs_cam_pixelPos_Access;
std::mutex mtx__system_waves_Access;
std::mutex mtx__meter_per_px_Access;
std::mutex mtx_cout;

std::atomic<bool> running__process_calc;


