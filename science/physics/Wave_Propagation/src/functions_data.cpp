
#include "functions_data.hpp"

pos2d<double> getWaveVectorSum(
    pos2d<double> _pos,
    std::vector<WaveSource> _waves
) {
    pos2d<double> sumPhaseShiftVector(0, 0);

    for(size_t i=0; i<_waves.size(); i++) {
        pos2d<double> delta = _pos - _waves[i].pos;
        sumPhaseShiftVector += _waves[i].getPhaseShiftVector(delta.hypotenuse());
    }

    return sumPhaseShiftVector;
}
double getWavePhaseAmplitudeSum(
    pos2d<double> _pos,
    std::vector<WaveSource> _waves
) {
    double sumAmplitude = 0;

    for(size_t i=0; i<_waves.size(); i++) {
        pos2d<double> delta = _pos - _waves[i].pos;
        sumAmplitude += sin(_waves[i].getPhaseShiftAngle(delta.hypotenuse()));
    }

    return sumAmplitude;
}


