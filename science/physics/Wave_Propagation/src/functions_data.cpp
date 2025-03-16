
#include "functions_data.hpp"

pos2d<double> getWaveVectorSum(
    pos2d<double> _pos,
    std::vector<WaveSource> _waves,
    std::vector<pos2d<double>>* _copy_indiv_phaseShift
) {
    pos2d<double> sumPhaseShiftVector(0, 0);
    pos2d<double> indivPhaseVec(0, 0);
    for(size_t i=0; i<_waves.size(); i++) {
        pos2d<double> delta = _pos - _waves[i].pos;
        indivPhaseVec = _waves[i].getPhaseShiftVector(delta.hypotenuse());
        sumPhaseShiftVector += indivPhaseVec;
        if(_copy_indiv_phaseShift) _copy_indiv_phaseShift->push_back(indivPhaseVec);
    }

    return sumPhaseShiftVector;
}
double getWavePhaseAmplitudeSum(
    pos2d<double> _pos,
    std::vector<WaveSource> _waves,
    std::vector<double>* _copy_indiv_amplitude
) {
    double sumAmplitude = 0;

    for(size_t i=0; i<_waves.size(); i++) {
        pos2d<double> delta = _pos - _waves[i].pos;
        double indivAmplit = sin(_waves[i].getPhaseShiftAngle(delta.hypotenuse()));
        sumAmplitude += indivAmplit;
        if(_copy_indiv_amplitude) _copy_indiv_amplitude->push_back(indivAmplit);
    }

    return sumAmplitude;
}


