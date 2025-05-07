
#include "includes.hpp"
#include "variables.hpp"
#include "draw_methods.hpp"
#include "PolyRegionCheck.hpp"


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
    

            int selected_camSys = -1;
            for(size_t i=0; i<camSys.size(); i++) {
                if(PRC::PosInPolygonPerimeter(GUINC::toPos2d(io.MousePos).cast<double>(), camSys[i].pos_shape)) {
                    selected_camSys = static_cast<int>(i);
                    break;
                }
            }


            if(keyBinds.clicked("MouseLeft")) {/// Left mouse button has been clicked

            }
            else if(keyBinds.pressing("MouseLeft")) { /// Left mouse button is being pressed
                if(selectable_isSelected) { /// a selected item is being moved/manipulated by left mouse button
                    // size_t selectedItemID = CARRYNUM(selectable_itemID, 0, 10);

                }
            }
            else if(keyBinds.released("MouseLeft")) { /// Left mouse button has just been released.
                selectable_isSelected = false;
            }
            else { /// No action is being performed by left mouse button
                switch (selected_camSys) {
                case -1: break;
                default:
                    
                    break;
                }

            }

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