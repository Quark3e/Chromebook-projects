
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
    

            pos2d<double> posMousePos(io.MousePos.x, io.MousePos.y);

            int selectable_itemID_hovered = -1;
            /**
             * Type of hover on the itemID if the item is to be pressed.
             *  - 2: Default: Move item in cartesian 2d coordinate space
             *  - 3: Rotate: Rotate item according to a dedicated rotation point.
             * 
             */
            int selectable_itemID_typeOfHover = 2;
            if(selectable_isSelected<0) {
                selectable_itemID = 0;

                for(size_t i=0; i<camSys.size(); i++) {
                    if(camSys[i].pos_shape.size()<2) continue;

                    pos2d<double> CamU_FOV_centerLine_point(camSys[i].x+cos(toRADIANS(camSys[i].angle))*(50+100), camSys[i].y-sin(toRADIANS(camSys[i].angle))*(50+100));
                    // winDrawList->AddRect(GUINC::toImVec2(CamU_FOV_centerLine_point-pos2d<double>(10, 10)), GUINC::toImVec2(CamU_FOV_centerLine_point+pos2d<double>(10, 10)), IM_COL32(60, 80, 160, 200));
                    if(PRC::PosInPolygonPerimeter(posMousePos, camSys[i].pos_shape)) { /// Hover: type 2 selected: Move item: CamU
                        
                        selectable_itemID += 1*std::pow(10, i+1);
                        selectable_itemID_hovered = i+1;
                        selectable_itemID_typeOfHover = 2;
                    }
                    else if(posMousePos.inRegion(CamU_FOV_centerLine_point-pos2d<double>(10, 10), CamU_FOV_centerLine_point+pos2d<double>(10, 10))) { /// Hover: type 3 selected: Rotate item: CamU

                        selectable_itemID += 1*std::pow(10, i+1);
                        selectable_itemID_hovered = i+1;
                        selectable_itemID_typeOfHover = 3;
                    }
                    else { /// item is not hovered
                        selectable_itemID -= size_t(CARRYNUM(selectable_itemID, i+1, 10));
                    }

                }

                if(posMousePos.inRegion(camSys.get_objectPos()-pos2d<double>(10,10), camSys.get_objectPos()+pos2d<double>(10,10))) { /// Hover: type 2 selected: Move item: objectPos

                    selectable_itemID += 1;
                    selectable_itemID_hovered = 0;
                    selectable_itemID_typeOfHover = 2;
                }
            }

            // std::cout << selectable_isSelected << " | ";
            // std::cout << formatNumber(selectable_itemID, 8, 0, "right", true) << " ";

            static pos2d<double> moveObject_mouseOffset{0, 0};
            if(keyBinds.clicked("MouseLeft")) {/// Left mouse button has been clicked
                if(selectable_isSelected>=0) throw std::logic_error("ERROR: a selectable has been seleected even though the MouseLeft just signaled to true value.");

                if(selectable_itemID_hovered>=0) {
                    /// An item that was hovered over has been clicked.
                    selectable_isSelected = static_cast<size_t>(selectable_itemID_hovered);
                    
                    
                    if(selectable_isSelected==0) {
                        selectable_itemID += 1*std::pow(10, selectable_itemID_hovered); // +=1 because the item is already hovered over.
                        moveObject_mouseOffset = posMousePos - camSys.get_objectPos();
                    }
                    else if(selectable_isSelected>0 && selectable_isSelected<camSys.size()+1 && camSys.size()>0) {
                        SOC::CamU& camU_ref = camSys[selectable_isSelected-1];
                        switch (selectable_itemID_typeOfHover) {
                            case 2:
                            selectable_itemID += 1*std::pow(10, selectable_itemID_hovered);
                            moveObject_mouseOffset = posMousePos - camU_ref.pos();
                            
                            break;
                            case 3:
                            selectable_itemID += 2*std::pow(10, selectable_itemID_hovered);
                            moveObject_mouseOffset = posMousePos - pos2d<double>(camU_ref.x+cos(toRADIANS(camU_ref.angle))*(50+100), camU_ref.y-sin(toRADIANS(camU_ref.angle))*(50+100));
    
                            break;
                            default: throw std::logic_error("Invalid selectable_itemID_typeOfHover value: "+std::to_string(selectable_itemID_typeOfHover));
                        }

                    }
                }

            }
            else if(keyBinds.pressing("MouseLeft")) { /// Left mouse button is being pressed
                if(selectable_isSelected>=0) { /// a selected item is being moved/manipulated by left mouse button
                    if(selectable_isSelected==0) {
                        /// moving the object pos

                        camSys.set_objectPos(posMousePos-moveObject_mouseOffset);
                    }
                    else if(selectable_isSelected>0 && selectable_isSelected<camSys.size()+1 && camSys.size()>0) {
                        /// moving one of the camU objects

                        size_t typeOfSelected = CARRYNUM(selectable_itemID, selectable_isSelected, 10);
                        switch (typeOfSelected) {
                            case 2:
                            camSys[selectable_isSelected-1].x = posMousePos.x - moveObject_mouseOffset.x;
                            camSys[selectable_isSelected-1].y = posMousePos.y - moveObject_mouseOffset.y;
                            
                            break;
                            case 3:
                            camSys[selectable_isSelected-1].angle = -toDEGREES((posMousePos - moveObject_mouseOffset).getTheta(camSys[selectable_isSelected-1].pos()));
                            
                            break;
                            default: throw std::logic_error("Invalid typeOfSelected value: "+std::to_string(typeOfSelected));
                        }

                    }
                }
            }
            else if(keyBinds.released("MouseLeft")) { /// Left mouse button has just been released.
                selectable_isSelected = -1;
            }
            else { /// No action is being performed by left mouse button
            }
            
            
            static std::vector<ImGuiCol> objectPos_colours{
                int(IM_COL32(100,  30,  30, 200)),
                int(IM_COL32(150,  80,  80, 200)),
                int(IM_COL32(200, 130, 130, 200))
            };
            int objectPos_drawState = 0;
            switch (size_t(CARRYNUM(selectable_itemID, 0, 10))) {
                case 0: objectPos_drawState = 0; break;
                case 1: objectPos_drawState = 1; break;
                case 2: objectPos_drawState = 2; break;
            default:
                break;
            }
            winDrawList->AddCircleFilled(GUINC::toImVec2(camSys.get_objectPos()), 10, objectPos_colours.at(objectPos_drawState), 10);
            
            pos2d<double> CamU_FOV_centerLine_point(0, 0);
            for(size_t i=0; i<camSys.size(); i++) {
                int drawState = 0;
                switch (size_t(CARRYNUM(selectable_itemID, i+1, 10))) {
                    case 1: drawState = 1; break;
                    case 2: drawState = 2; break;
                    case 3: 
                        // CamU_FOV_centerLine_point = pos2d<double>(camSys[i].x+cos(toRADIANS(camSys[i].angle))*(50+100), camSys[i].y-sin(toRADIANS(camSys[i].angle))*(50+100));
                        winDrawList->AddCircle(ImVec2(camSys[i].x+cos(toRADIANS(camSys[i].angle))*(50+100), camSys[i].y-sin(toRADIANS(camSys[i].angle))*(50+100)), 10, IM_COL32(130, 130, 130, 150), 20, 3);
                        break;
                default:
                    drawState = 0;
                    break;
                }

                DRMETHS::draw_camUnit(camSys[i], true, drawState);
            }
            // std::cout << std::endl;
    
            guiwin.endFrame();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        
    }

    return 0;
}