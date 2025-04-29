
#include "processes.hpp"
#include "useful.hpp"

void exitGui() {
    atm__running_process_calc = false;
}
void process_gui() {
    maxThreadNum = std::thread::hardware_concurrency();

    std::unique_lock<std::mutex> u_lck__cout(mtx__cout, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__ptrBMPsystem(mtx__ptrBMPsystem_Switch, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__abs_cam_pixelPos(mtx__abs_cam_pixelPos_Access, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__system_waves(mtx__system_waves_Access, std::defer_lock);
    std::unique_lock<std::mutex> u_lck__meter_per_px(mtx__meter_per_px_Access, std::defer_lock);


    GUINC::win_dim = ImVec2(system_dim.x, system_dim.y);
    ImVec2 info_win_dim = ImVec2(GUINC::win_dim.x, 200);
    
    guiwin.callback_func__running_exit = exitGui;
    
    guiwin.init();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP* bmp = al_create_bitmap(GUINC::win_dim.x/pixelSpacing, GUINC::win_dim.y/pixelSpacing);
    al_set_new_bitmap_flags(!ALLEGRO_MEMORY_BITMAP);
    // while(guiwin.running()) {
    while(atm__running_process_calc.load()) {
        size_t bitArrIdx = 0;
        ImGuiIO io = ImGui::GetIO();
        guiwin.newFrame();
        
        guiKeys.update();
        keyBinds.update(guiKeys.pressed[guiKeys.pressed.size()-1]);

        

        u_lck__ptrBMPsystem.lock();
        try {
            loadBitmap_fromBitArray(bmp, &(ptr_BMP_system_gui->get_BMP_arr()), "GRAY", GUINC::win_dim.x/pixelSpacing, GUINC::win_dim.y/pixelSpacing);
            
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        u_lck__ptrBMPsystem.unlock();

        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::Image((ImTextureID)(intptr_t)bmp, ImVec2(GUINC::win_dim.x*2, GUINC::win_dim.y*2));


        guiwin.draw()->AddLine(GUINC::toImVec2(detectLine[0]), GUINC::toImVec2(detectLine[1]), IM_COL32(10, 10, 10, 250), 2);
        pos2d<float> detectDelta = detectLine[1] - detectLine[0];
        pos2d<float> detectDelta_gap = detectDelta.getModify([detectDelta](float _var) {return (_var/roundf(detectDelta.getHypotenuse())); });

        ImGui::SetNextWindowSizeConstraints(info_win_dim, ImVec2(info_win_dim.x, GUINC::win_dim.y));
        ImGui::SetNextWindowPos(
            ImVec2(0, GUINC::win_dim.y),
            0,
            ImVec2(0, 1)
        );
        static ImGuiWindowFlags graphWin_flags = 
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse
            ;

        if(ImGui::Begin("Graph Window", NULL, graphWin_flags)) {
            ImGui::SetWindowPos(ImVec2(0, GUINC::win_dim.y-ImGui::GetWindowSize().y));
            pos2d<float> pixelPos = detectLine[0].getModify([](float _var){return _var/pixelSpacing;});
            std::vector<pos2d<double>>  elemPos;
            std::vector<float>          elemScalar;
            
            std::vector<double> amplitudeLim{-system_waves.size()*1.0, system_waves.size()*1.0};
            u_lck__ptrBMPsystem.lock();
            for(float _gap=0; _gap<=detectDelta.getHypotenuse(); _gap+=detectDelta_gap.getHypotenuse()) {
                
                float sumScal = 0;
                SystemWaveData_info_ref refObj = ptr_BMP_system_gui->at(pixelPos.x, pixelPos.y);
                pos2d<double> detectPos = refObj.pos;
                switch(scalMethod) {
                    case 0: { // sine wave amplitude sum
                        sumScal = refObj.sum_amplitude/amplitudeLim[1];
                    }
                    break;
                    case 1: { // phase shift vector sum sq
                        sumScal = pow(refObj.sum_phaseVector.getHypotenuse(), 2) / pow(amplitudeLim[1], 2);
                    }
                    break;
                    default: throw std::invalid_argument("invalid scalMethod arg.");
                }

                elemPos.push_back(detectPos);
                elemScalar.push_back(sumScal);
                
                pixelPos += detectDelta_gap.getModify([](float _var) {return _var/pixelSpacing; });
            }
            u_lck__ptrBMPsystem.unlock();
            ImVec2 childDim(info_win_dim.x-20, info_win_dim.y-20);
            // if(ImGui::BeginChild("Graph Child", childDim)) {
                ImVec2 graphWinPos = ImGui::GetWindowPos();
                ImDrawList* childDraw = ImGui::GetWindowDrawList();
                for(size_t i=1; i<elemPos.size(); i++) {
                    ImVec2 _p0((float(i-1)/elemPos.size())*childDim.x + graphWinPos.x, (1*childDim.y-0.8*childDim.y*elemScalar[i-1]) + graphWinPos.y);
                    ImVec2 _p1((float(i)  /elemPos.size())*childDim.x + graphWinPos.x, (1*childDim.y-0.8*childDim.y*elemScalar[i])   + graphWinPos.y);
                    childDraw->AddLine(_p0, _p1, IM_COL32(200, 20, 20, 150));
                }
            // }
            // ImGui::EndChild();
            // mutex_cout("thread-gui : before ImGui::Begin() end", mtx__cout);
        }
        else {
            ImGui::SetWindowPos(ImVec2(0, GUINC::win_dim.y-ImGui::GetWindowSize().y));
        }
        ImGui::End();

        // #if takePerformance_processCalc
        static ImVec2 delays_tableDim(220, 200);
        ImGui::SetNextWindowSizeConstraints(ImVec2(delays_tableDim.x+20, 200), ImVec2(delays_tableDim.x+20, 400));
        // ImGui::SetNextWindowPos(ImVec2(system_dim.x, 0), 0, ImVec2(1, 0));
        if(ImGui::Begin("Delays_calc window", NULL)) {
            if(ImGui::BeginTable("Delays_calc table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit, delays_tableDim)) {
                ImGui::TableSetupColumn("Label");
                ImGui::TableSetupColumn("Delays [sec]");
                ImGui::TableHeadersRow();
                for(size_t i=0; i<ptr_BMP_system_gui->delays__process_calc.size(); i++) {
                    if(i>=ptr_BMP_system_gui->delays__process_calc.size()-1) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("");
                    }

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text(ptr_BMP_system_gui->delays__process_calc.getKey(i).c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text(formatNumber(ptr_BMP_system_gui->delays__process_calc[i],10,3).c_str());
                }
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Total time");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(formatNumber(ptr_BMP_system_gui->delays__process_calc_totalTime, 10, 3).c_str());
                ImGui::EndTable();
            }
        }
        ImGui::End();
        // #endif //takePerformance_processCalc

        // u_lck__system_waves.lock();
        for(size_t i=0; i<system_waves.size(); i++) {
            WaveSource& src = system_waves[i];
            pos2d<int> pixel_pos = pos2d<int>(src.pos.getModify([](double _var){return (_var/meter_per_px);}).cast<int>([](double _var){return int(roundf(_var));})) - abs_cam_pixelPos;
            
            guiwin.draw()->AddCircleFilled(GUINC::toImVec2(pixel_pos), 5, IM_COL32(255, 255, 255, 255), 10);
            guiwin.draw()->AddCircle(GUINC::toImVec2(pixel_pos), 5, IM_COL32(20, 20, 20, 255), 10);
        }
        // u_lck__system_waves.unlock();
        // mutex_cout("thread-gui : after system waves unlock", mtx__cout);
        // mutex_cout("thread-gui : endFrame", mtx__cout);
        guiwin.endFrame();
    }

    al_destroy_bitmap(bmp);
}
