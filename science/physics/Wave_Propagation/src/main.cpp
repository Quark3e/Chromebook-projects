
#include "includes.hpp"

#include "variables_data.hpp"
#include "functions_data.hpp"

#include "variables_gui.hpp"
#include "functions_gui.hpp"




int main(int argc, char** argv) {
    guiwin_nc::win_dim = ImVec2(1000, 1000);
    ImVec2 info_win_dim = ImVec2(guiwin_nc::win_dim.x, 200);
    
    std::vector<uint8_t> bitArr(int(float(numColour*guiwin_nc::win_dim.x*guiwin_nc::win_dim.y)/(pixelSpacing*pixelSpacing)), 0);

    guiwin.callback_func__running_exit = _keyBind__Master_Close;

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

