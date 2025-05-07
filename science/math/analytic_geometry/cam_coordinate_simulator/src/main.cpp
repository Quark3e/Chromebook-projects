
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
    

            // int selected_camSys = -1;
            if(selectable_isSelected<0) {
                selectable_itemID = 0;
                for(size_t i=0; i<camSys.size(); i++) {
                    if(PRC::PosInPolygonPerimeter(GUINC::toPos2d(io.MousePos).cast<double>(), camSys[i].pos_shape)) {
                        // selected_camSys = static_cast<int>(i);
                        selectable_itemID += 1*std::pow(10, i+1);
                    }
                    else {
                        selectable_itemID -= CARRYNUM(selectable_itemID, i+1, 10);
                    }
                }
            }


            if(keyBinds.clicked("MouseLeft")) {/// Left mouse button has been clicked
                if(selectable_isSelected>=0) throw std::logic_error("ERROR: a selectable has been seleected even though the MouseLeft just signaled to true value.");

                // selectable_isSelected = true;
                /// Iterate through every power of 10
                for(size_t n=0; n<8*sizeof(size_t)*std::log(2); n++) {
                    
                }
                std::cout << sizeof(size_t) << std::endl;

            }
            else if(keyBinds.pressing("MouseLeft")) { /// Left mouse button is being pressed
                if(selectable_isSelected>=0) { /// a selected item is being moved/manipulated by left mouse button
                    // size_t selectedItemID = CARRYNUM(selectable_itemID, 0, 10);

                }
            }
            else if(keyBinds.released("MouseLeft")) { /// Left mouse button has just been released.
                selectable_isSelected = -1;
            }
            else { /// No action is being performed by left mouse button
                

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