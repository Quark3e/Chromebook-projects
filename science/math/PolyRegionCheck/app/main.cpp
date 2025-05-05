/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief Used to test PolyRegionCheck library's methods.
 * @version 0.1
 * @date 2025-05-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <iostream>
#include <vector>

#include <pos2d.hpp>
#include <keyBind_handler.hpp>
#include <nodeChart/extra_imgui.hpp>

#include <imguiwin.hpp>

#include "PolyRegionCheck.hpp"

#include "useful.hpp"

pos2d<int> dim__programWindow{500, 500};
pos2d<int> dim__draw_area{dim__programWindow};

pos2d<int> pos__draw_area{0, 0};

GUINC::imguiwin guiwin(false, dim__programWindow.x, dim__programWindow.y);
void _keyBind__Master_Close() {
    guiwin.exit();
}

DIY_KBH::keyBind_handler keyBinds({
    {"Master Close",    {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_W}, _keyBind__Master_Close},
    {"MouseLeft",       {ImGuiKey_MouseLeft}},
    {"MouseRight",      {ImGuiKey_MouseRight}},
    {"Escape",          {ImGuiKey_Escape}, _keyBind__Master_Close}
});
pressed_key__struct guiKeys;

/// @brief The main polygon to detect whether the cursor is within.
std::vector<pos2d<float>> polygonShape{
    // {200, 210},
    // {150, 69},
    // {400, 430},
    // {420, 219},
    // {10, 90}
    {float(250+100*cos(toRADIANS(0))), float(250+100*sin(toRADIANS(0)))},
    {float(250+100*cos(toRADIANS(51))), float(250+100*sin(toRADIANS(51)))},
    {float(250+100*cos(toRADIANS(100))), float(250+100*sin(toRADIANS(100)))},
    {float(250+100*cos(toRADIANS(160))), float(250+100*sin(toRADIANS(150)))},
    {float(250+100*cos(toRADIANS(205))), float(250+100*sin(toRADIANS(205)))},
    {float(250+100*cos(toRADIANS(256))), float(250+100*sin(toRADIANS(240)))},
    {float(250+100*cos(toRADIANS(310))), float(250+100*sin(toRADIANS(300)))},
    {float(250+50*cos(toRADIANS(150))), float(250+5*sin(toRADIANS(340)))}
};
/// @brief Index to the polygonPoint that is currently being moved/manipulated. If it's -1 then no point is being moved.
int moving_polygonPoint = -1;

int polygonPoint_gui_radius = 10;

std::vector<pos2d<float>> extracted_limits(2, {0, 0});

int main(int argc, char** argv) {

    assert(guiwin.init());
    while(guiwin.running()) {
        try {
            guiwin.newFrame();

            guiKeys.update();
            keyBinds.update(guiKeys.pressed[guiKeys.pressed.size()-1]);
            if(!guiwin.running()) break;

            ImGuiIO io = ImGui::GetIO();
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            

            if(keyBinds.released("MouseLeft")) moving_polygonPoint = -1;

            bool insidePolygon = PRC::PosInPolygonPerimeter<float>(GUINC::toPos2d(io.MousePos), polygonShape, &extracted_limits[0], &extracted_limits[1]);
            ImGuiCol polygon_perimeter_col = (insidePolygon? IM_COL32(100, 230, 110, 200) : IM_COL32(100, 100, 100, 180));

            
            for(size_t i=0; i<polygonShape.size()-1; i++) {
                drawList->AddLine(GUINC::toImVec2(polygonShape.at(i)), GUINC::toImVec2(polygonShape.at(i+1)), polygon_perimeter_col, 1);
                drawList->AddCircle(GUINC::toImVec2(polygonShape.at(i)), polygonPoint_gui_radius, IM_COL32(170, 110, 100, 100), 10);
            }
            drawList->AddLine(GUINC::toImVec2(polygonShape.at(polygonShape.size()-1)), GUINC::toImVec2(polygonShape.at(0)), polygon_perimeter_col, 1);
            drawList->AddCircle(GUINC::toImVec2(polygonShape.at(polygonShape.size()-1)), polygonPoint_gui_radius, IM_COL32(170, 110, 100, 100), 10);
            

            drawList->AddCircleFilled(io.MousePos, 5, IM_COL32(120, 70, 80, 200), 10);
            guiwin.endFrame();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        
    }

    return 0;
}