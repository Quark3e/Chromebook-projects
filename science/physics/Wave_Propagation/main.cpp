
#include <imguiwin.hpp>

#include <pos2d.hpp>

#include <vector>
#include <cmath>
#include <cstdlib>

#include <keyBind_handler.hpp>
#include <nodeChart/extra_imgui.hpp>


#include <useful.hpp>

#define unit(val) (val)/std::abs(val)


guiwin_nc::imguiwin guiwin(false);


void _keyBind__Master_Close() {
    exit(1);
}
DIY_KBH::keyBind_handler keyBinds({
    {"Master Close",    {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_W}, _keyBind__Master_Close},
    {"MouseLeft",       {ImGuiKey_MouseLeft}},
    {"MouseRight",      {ImGuiKey_MouseRight}}
});
pressed_key__struct guiKeys;


struct WaveSource {
    double          wavelength;
    pos2d<double>   pos;
    
    
    double getPhaseShiftAngle(double distance) {
        return (2*M_PI)*(distance/wavelength);
    }
    pos2d<double> getPhaseShiftVector(double distance) {
        double theta = this->getPhaseShiftAngle(distance);
        return pos2d<double>(cos(theta), sin(theta));
    }
};


double getWaveVectorSum(
    pos2d<double> pos,
    std::vector<WaveSource> _waves
) {
    pos2d<double> sumPhaseShiftVector(0, 0);

    for(size_t i=0; i<_waves.size(); i++) {
        pos2d<double> delta = pos - _waves[i].pos;
        sumPhaseShiftVector += _waves[i].getPhaseShiftVector(delta.hypotenuse());
    }
}

int main(int argc, char** argv) {



    return 0;
}


pos2d<double> solvePhaseVector() {
    
}