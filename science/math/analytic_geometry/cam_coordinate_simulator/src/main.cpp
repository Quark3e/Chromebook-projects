
#include "includes.hpp"
#include "variables.hpp"
#include "draw_methods.hpp"


int main(int argc, char** argv) {


    assert(guiwin.init());
    while(guiwin.running()) {
        try {
            
            guiwin.newFrame();
            
            guiKeys.update();
            keyBinds.update(guiKeys.pressed[guiKeys.pressed.size()-1]);
            if(!guiwin.running()) break;

            ImGuiIO io = ImGui::GetIO();
            ImDrawList* winDrawList = ImGui::GetWindowDrawList();
            camSys.update();
    
            winDrawList->AddCircleFilled(GUINC::toImVec2(camSys.get_objectPos()), 10, IM_COL32(100, 30, 30, 200), 10);
    
            for(size_t i=0; i<camSys.size(); i++) {
                // DRMETHS::draw_camUnit(camSys[i].pos(), camSys[i].angle, true, camSys[i].FOV);
                DRMETHS::draw_camUnit(camSys[i], true);
            }
    
            guiwin.endFrame();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        
    }

    return 0;
}