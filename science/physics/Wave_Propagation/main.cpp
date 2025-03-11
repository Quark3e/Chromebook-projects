
#include <imguiwin.hpp>

#include <pos2d.hpp>

#include <vector>
#include <cmath>
#include <cstdlib>

#include <keyBind_handler.hpp>
#include <nodeChart/extra_imgui.hpp>


#include <useful.hpp>

#define unit(val) (val)/std::abs(val)


#define scalMethod 0

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
    pos2d<double>   pos;
    double          wavelength;

    WaveSource(pos2d<double> _pos, double _wavelength): pos(_pos), wavelength(_wavelength) {

    }
    
    double getPhaseShiftAngle(double distance) {
        return (2*M_PI)*(distance/wavelength);
    }
    pos2d<double> getPhaseShiftVector(double distance) {
        double theta = this->getPhaseShiftAngle(distance);
        return pos2d<double>(cos(theta), sin(theta));
    }
};



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

float scalarMethod(pos2d<double> &_objPos, std::vector<WaveSource> &_system_waves, std::vector<double> &_lim, int _method=0) {
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


double  meter_per_px    = 0.0001;

std::vector<pos2d<float>> detectLine{
    {800,  86},
    {800, 715}
};

void exitFunc() { exit(0); }
int main(int argc, char** argv) {
    guiwin_nc::win_dim = ImVec2(1000, 1000);
    ImVec2 info_win_dim = ImVec2(guiwin_nc::win_dim.x, 200);
    
    std::vector<uint8_t> bitArr(int(float(numColour*guiwin_nc::win_dim.x*guiwin_nc::win_dim.y)/(pixelSpacing*pixelSpacing)), 0);

    guiwin.callback_func__running_exit = exitFunc;

    meter_per_px = (0.06 /*(system_waves[1].pos.y-system_waves[0].pos.y)*/ / 600);
    
    std::vector<double> amplitudeLim{-system_waves.size()*1.0, system_waves.size()*1.0};

    
    guiwin.init();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP* bmp = al_create_bitmap(guiwin_nc::win_dim.x/pixelSpacing, guiwin_nc::win_dim.y/pixelSpacing);
    al_set_new_bitmap_flags(!ALLEGRO_MEMORY_BITMAP);
    while(guiwin.running()) {
        size_t bitArrIdx = 0;
        ImGuiIO io = ImGui::GetIO();
        guiwin.newFrame();
        
        guiKeys.update();
        keyBinds.update(guiKeys.pressed[guiKeys.pressed.size()-1]);
        // ALLEGRO_LOCKED_REGION* lockedReg = al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_RGB_888, ALLEGRO_LOCK_WRITEONLY);
        for(size_t y=0; y<guiwin_nc::win_dim.y; y+=pixelSpacing) {
            for(size_t x=0; x<guiwin_nc::win_dim.x; x+=pixelSpacing) {
                pos2d<double> realPos(double(x + abs_cam_pixelPos.x) * meter_per_px, double(y + abs_cam_pixelPos.y) * meter_per_px);

                std::vector<uint8_t> RGB_col = graph_colour_scale(scalarMethod(realPos, system_waves, amplitudeLim, scalMethod));

                bitArr.at(bitArrIdx+0) = RGB_col[0];
                // bitArr.at(bitArrIdx+1) = RGB_col[1];
                // bitArr.at(bitArrIdx+2) = RGB_col[2];
                // bitArrIdx+=3;
                bitArrIdx+=1;
            }
        }
        // al_unlock_bitmap(bmp);
        try {
            loadBitmap_fromBitArray(bmp, &bitArr, "GRAY", guiwin_nc::win_dim.x/pixelSpacing, guiwin_nc::win_dim.y/pixelSpacing);
            
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        
        
        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::Image((ImTextureID)(intptr_t)bmp, ImVec2(guiwin_nc::win_dim.x*pixelSpacing, guiwin_nc::win_dim.y*pixelSpacing));


        guiwin.draw()->AddLine(guiwin_nc::toImVec2(detectLine[0]), guiwin_nc::toImVec2(detectLine[1]), IM_COL32(10, 10, 10, 250), 2);
        pos2d<float> detectDelta = detectLine[1] - detectLine[0];
        pos2d<float> detectDelta_gap = detectDelta.modify([detectDelta](float _var) {return (_var/roundf(detectDelta.hypotenuse())); });
        // std::vector<float> sumScalar;

        ImGui::SetNextWindowSizeConstraints(info_win_dim, ImVec2(info_win_dim.x, guiwin_nc::win_dim.y));
        ImGui::SetNextWindowPos(
            ImVec2(0, guiwin_nc::win_dim.y),
            0,
            ImVec2(0, 1)
        );
        static ImGuiWindowFlags graphWin_flags = 
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar;

            
        if(ImGui::Begin("Graph Window", NULL, graphWin_flags)) {
            ImGui::SetWindowPos(ImVec2(0, guiwin_nc::win_dim.y-ImGui::GetWindowSize().y));
            pos2d<float> pixelPos = detectLine[0];
            std::vector<pos2d<double>>  elemPos;
            std::vector<float>          elemScalar;
            for(float _gap=0; _gap<=detectDelta.hypotenuse(); _gap+=detectDelta_gap.hypotenuse()) {
                pos2d<double> detectPos(double(pixelPos.x + abs_cam_pixelPos.x) * meter_per_px, double(pixelPos.y + abs_cam_pixelPos.y) * meter_per_px);
                float sumScal = scalarMethod(detectPos, system_waves, amplitudeLim, scalMethod);

                elemPos.push_back(detectPos);
                elemScalar.push_back(sumScal);
                
                pixelPos += detectDelta_gap;
            }
            ImVec2 childDim(info_win_dim.x-20, info_win_dim.y-20);
            // if(ImGui::BeginChild("Graph Child", childDim)) {
                ImVec2 graphWinPos = ImGui::GetWindowPos();
                ImDrawList* childDraw = ImGui::GetWindowDrawList();
                for(size_t i=1; i<elemPos.size(); i++) {
                    ImVec2 _p0((float(i-1)/elemPos.size())*childDim.x + graphWinPos.x, (1.1*childDim.y-1.1*childDim.y*elemScalar[i-1]) + graphWinPos.y);
                    ImVec2 _p1((float(i)  /elemPos.size())*childDim.x + graphWinPos.x, (1.1*childDim.y-1.1*childDim.y*elemScalar[i])   + graphWinPos.y);
                    childDraw->AddLine(_p0, _p1, IM_COL32(200, 20, 20, 150));
                }
            // }
            // ImGui::EndChild();
        }
        else {
            ImGui::SetWindowPos(ImVec2(0, guiwin_nc::win_dim.y-ImGui::GetWindowSize().y));
        }
        ImGui::End();

        for(size_t i=0; i<system_waves.size(); i++) {
            WaveSource& src = system_waves[i];
            pos2d<int> pixel_pos = pos2d<int>(src.pos.modify([](double _var){return (_var/meter_per_px);}).cast<int>([](double _var){return int(roundf(_var));})) - abs_cam_pixelPos;
        
            guiwin.draw()->AddCircleFilled(guiwin_nc::toImVec2(pixel_pos), 5, IM_COL32(255, 255, 255, 255), 10);
            guiwin.draw()->AddCircle(guiwin_nc::toImVec2(pixel_pos), 5, IM_COL32(20, 20, 20, 255), 10);
        }

        guiwin.endFrame();
    }

    al_destroy_bitmap(bmp);

    return 0;
}

