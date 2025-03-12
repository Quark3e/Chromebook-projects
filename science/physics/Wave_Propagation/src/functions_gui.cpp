
#include "variables_data.hpp"
#include "functions_data.hpp"
#include "functions_gui.hpp"

float scalarMethod(pos2d<double> &_objPos, std::vector<WaveSource> &_system_waves, std::vector<double> &_lim, int _method) {
    float scal = 0;
    if(_lim.size()!=2) throw std::invalid_argument("invalid _lim arg.");
    switch(_method) {
        case 0: { // sine wave amplitude sum
            double amplSum = getWavePhaseAmplitudeSum(_objPos, _system_waves);
            return (amplSum/_lim[1]);
        }
        break;
        case 1: { // phase shift vector sum sq
            pos2d<double> vecSum = getWaveVectorSum(_objPos, _system_waves);
            return (pow(vecSum.hypotenuse(), 2) / pow(_lim[1], 2));
        }
        break;
        default: throw std::invalid_argument("invalid _method arg.");
    }
    return 0;
}

std::vector<uint8_t> graph_colour_scale(float _val) {
    if(_val < -1.0)     throw std::invalid_argument("ERROR: graph_colour_scale(float): float _val cannot be smaller than -1.0.");
    else if(_val > 1.0) throw std::invalid_argument("ERROR: graph_colour_scale(float): float _val cannot be bigger than 1.0.");

    std::vector<uint8_t> _RGB{
        uint8_t(127+_val*120),
        //0, //(_val<=0? int(255.0*_val) : 0),
        0,
        0, //(_val>=0? int(100.0*_val) : 0)
    };
    
    return _RGB;
}

