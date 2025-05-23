
#include "includes.hpp"
#include "variables.hpp"
#include "draw_methods.hpp"
#include "PolyRegionCheck.hpp"


int main(int argc, char** argv) {

    assert(infoBox_open_CamU.size()==camSys.size());

    assert(guiwin.init());
    while(guiwin.running()) {
        try {
            
            guiwin.newFrame();
            
            guiKeys.update();
            keyBinds.update(guiKeys.pressed[guiKeys.pressed.size()-1]);
            bool mouseLeft_doubleClicked = ImGui::IsMouseDoubleClicked(0);
            if(!guiwin.running()) break;

            ImGuiIO io = ImGui::GetIO();
            camSys.update();
    
            
            pos2d<double> inRealMousePos(func_convPos_pxToReal(GUINC::toPos2d(io.MousePos).cast<double>()));

            int selectable_itemID_hovered = -1;
            /**
             * Type of hover on the itemID if the item is to be pressed.
             *  - 2: Default: Move item in cartesian 2d coordinate space
             *  - 3: Rotate: Rotate item according to a dedicated rotation point.
             * 
             */
            int selectable_itemID_typeOfHover = 2;

            /// Detect cursor interaction with the different items if no item is selected currently
            if(selectable_isSelected<0) {
                selectable_itemID = 0;

                for(size_t i=0; i<camSys.size(); i++) {
                    if(camSys[i].pos_shape.size()<2) continue;

                    pos2d<double> CamU_FOV_centerLine_point(camSys[i].x+cos(toRADIANS(camSys[i].angle))*(50+100), camSys[i].y+sin(toRADIANS(camSys[i].angle))*(50+100));
                    if(PRC::PosInPolygonPerimeter(inRealMousePos, camSys[i].pos_shape)) { /// Hover: type 2 selected: Move item: CamU
                        
                        selectable_itemID += 1*std::pow(10, i+1);
                        selectable_itemID_hovered = i+1;
                        selectable_itemID_typeOfHover = 2;
                    }
                    else if(inRealMousePos.inRegion(CamU_FOV_centerLine_point-pos2d<double>(10, 10), CamU_FOV_centerLine_point+pos2d<double>(10, 10))) { /// Hover: type 3 selected: Rotate item: CamU

                        selectable_itemID += 1*std::pow(10, i+1);
                        selectable_itemID_hovered = i+1;
                        selectable_itemID_typeOfHover = 3;
                    }
                    else { /// item is not hovered
                        selectable_itemID -= size_t(CARRYNUM(selectable_itemID, i+1, 10));
                    }

                }

                if(inRealMousePos.inRegion(camSys.get_objectPos()-pos2d<double>(10,10), camSys.get_objectPos()+pos2d<double>(10,10))) { /// Hover: type 2 selected: Move item: objectPos

                    selectable_itemID += 1;
                    selectable_itemID_hovered = 0;
                    selectable_itemID_typeOfHover = 2;
                }
            }

            /// Do UI movements/actions with the varying states of the mouse left button click.
            static pos2d<double> moveObject_mouseOffset{0, 0};
            if(keyBinds.clicked("MouseLeft")) {/// Left mouse button has been clicked
                if(selectable_isSelected>=0) throw std::logic_error("ERROR: a selectable has been seleected even though the MouseLeft just signaled to true value.");

                if(selectable_itemID_hovered>=0) {
                    /// An item that was hovered over has been clicked.
                    selectable_isSelected = static_cast<size_t>(selectable_itemID_hovered);
                    
                    
                    if(selectable_isSelected==0) { /// The object pos has been chosen to be moved
                        selectable_itemID += 1*std::pow(10, selectable_itemID_hovered); // +=1 because the item is already hovered over.
                        moveObject_mouseOffset = inRealMousePos - camSys.get_objectPos();
                    }
                    else if(selectable_isSelected>0 && selectable_isSelected<camSys.size()+1 && camSys.size()>0) { /// An CamU object has been chosen
                        SOC::CamU& camU_ref = camSys[selectable_isSelected-1];
                        switch (selectable_itemID_typeOfHover) {
                            case 2: /// itemID: CamU: type of hover: move
                            selectable_itemID += 1*std::pow(10, selectable_itemID_hovered);
                            moveObject_mouseOffset = inRealMousePos - camU_ref.pos();
                            break;
                            case 3:  /// itemID: CamU: type of hover: rotate
                            selectable_itemID += 2*std::pow(10, selectable_itemID_hovered);
                            moveObject_mouseOffset = inRealMousePos - pos2d<double>(camU_ref.x+cos(toRADIANS(camU_ref.angle))*(50+100), camU_ref.y+sin(toRADIANS(camU_ref.angle))*(50+100));
    
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
                        camSys.set_objectPos(inRealMousePos-moveObject_mouseOffset);
                    }
                    else if(selectable_isSelected>0 && selectable_isSelected<camSys.size()+1 && camSys.size()>0) { /// moving one of the camU objects
                        size_t typeOfSelected = CARRYNUM(selectable_itemID, selectable_isSelected, 10);
                        switch (typeOfSelected) {
                            case 2:
                            camSys[selectable_isSelected-1].x = inRealMousePos.x - moveObject_mouseOffset.x;
                            camSys[selectable_isSelected-1].y = inRealMousePos.y - moveObject_mouseOffset.y;
                            
                            break;
                            case 3: {
                                pos2d<double> pivPoint(camSys[selectable_isSelected-1].x, camSys[selectable_isSelected-1].y);
                                // pivPoint.y += ((inRealMousePos.y-moveObject_mouseOffset.y-pivPoint.y)*2);

                                camSys[selectable_isSelected-1].angle = toDEGREES((inRealMousePos - moveObject_mouseOffset).getTheta(
                                    pos2d<double>(pivPoint.x, pivPoint.y)
                                ));
                                
                            }
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
            
            
            if(selectable_itemID_hovered>0 && mouseLeft_doubleClicked) { // If selectable CamU item has been double clicked, open its info box
                auto itr = infoBox_open_CamU.begin();
                std::advance(itr, selectable_itemID_hovered-1);
                *itr = true;
            }

            /// SystemOfCameras update, which also updates the states and variables of each internal CamU object respectively
            camSys.update();
            
            camTri.camPos = {camSys[0].pos(), camSys[1].pos()};
            camTri.ang_offset = {camSys[0].angle, camSys[1].angle};
            camTri.setcamScalars();
            static std::vector<pos2d<double>> fpv_objectPos(2, pos2d<double>{0, 480/2});
            fpv_objectPos[0].x = 640/2 + (640/2)*(camSys[0].toObjectAngle/(camSys[0].FOV/2));
            fpv_objectPos[1].x = 640/2 + (640/2)*(camSys[1].toObjectAngle/(camSys[1].FOV/2));
            camTri.solvePos(fpv_objectPos, false);



            std::cout << "ang_d:"<<formatVector(camTri.ang_d, 5) << " | ";
            std::cout << "ang_read:"<<formatVector(camTri.ang_read, 6) << " | ";
            std::cout << "lengths:"<<camTri.l_tri[0]<<", "<<camTri.l_tri[1]<<", "<<camTri.pos_delta.getHypotenuse()<< " | ";
            std::cout << "ang_tri:"<<formatVector(camTri.ang_tri, 6) << " | ";
            std::cout << formatVector(fpv_objectPos, 10) << " | " << formatVector(camTri.solvedPos, 7) << std::endl;
            
            /// ---------- Drawing ----------
            
            ImGui::Begin("window_draw_area", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
            
            ImDrawList* winDrawList = ImGui::GetWindowDrawList();

            /// Draw item: objectPos
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
            winDrawList->AddCircleFilled(GUINC::toImVec2(func_convPos_realToPx(camSys.get_objectPos())), 10, objectPos_colours.at(objectPos_drawState), 10);
            winDrawList->AddCircle(GUINC::toImVec2(func_convPos_realToPx({camTri.solvedPos[0], camTri.solvedPos[1]})), 12, IM_COL32(0, 128, 128, 140), 5, 1);
            
            for(size_t i=0; i<camSys.size(); i++) {
                int drawState = 0;
                switch (size_t(CARRYNUM(selectable_itemID, i+1, 10))) {
                    case 1: drawState = 1; break;
                    case 2: drawState = 2; break;
                    case 3: 
                        winDrawList->AddCircle(
                            GUINC::toImVec2(func_convPos_realToPx({
                                camSys[i].x+cos(toRADIANS(camSys[i].angle))*(50+100),
                                camSys[i].y+sin(toRADIANS(camSys[i].angle))*(50+100)
                            })),
                            10,
                            IM_COL32(130, 130, 130, 150),
                            20,
                            3
                        );
                        break;
                default:
                    drawState = 0;
                    break;
                }

                draw_camUnit(camSys[i], true, drawState);

                auto itr = infoBox_open_CamU.begin();
                std::advance(itr, i);
                draw_infoBox_camUnit(camSys[i], &(*itr), dim__infoBox_CamU);
            }
            

            ImGui::End(); //window_draw_area
    
            guiwin.endFrame();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        
    }

    return 0;
}