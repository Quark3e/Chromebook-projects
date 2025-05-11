
#include "variables.hpp"



pos2d<int> dim__programWindow{DEFAULT__WINDOW_WIDTH, DEFAULT__WINDOW_HEIGHT};
pos2d<int> dim__draw_area(dim__programWindow);
pos2d<int> dim__infoBox_CamU{200, 200};

pos2d<int> pos__draw_area{0, 0};
pos2d<int> pos__draw_area_loc{0, 0};

pos2d<float> scal__draw_area{1, 1};


GUINC::imguiwin guiwin(false, dim__programWindow.x, dim__programWindow.y);

void _keyBind__Master_Close() {
    // exit(1);
    guiwin.exit();
}

DIY_KBH::keyBind_handler keyBinds({
    {"Master Close",    {ImGuiKey_ReservedForModCtrl, ImGuiKey_LeftCtrl, ImGuiKey_W}, _keyBind__Master_Close},
    {"MouseLeft",       {ImGuiKey_MouseLeft}},
    {"MouseRight",      {ImGuiKey_MouseRight}},
    {"Escape",          {ImGuiKey_Escape}, _keyBind__Master_Close}
});
pressed_key__struct guiKeys;


size_t selectable_itemID = 0;
int selectable_isSelected = -1;


SOC::CamSystem camSys({500, 500}, {
    {90,  75, 300, 200},
    {90, 110, 700, 200}
});

std::list<bool> infoBox_open_CamU(camSys.size(), false);




void draw_camUnit(SOC::CamU &_CamU_toDraw, bool _drawFOV, int _drawState) {
    static std::vector<ImGuiCol> camUnit_drawColour{
        int(IM_COL32(100, 10, 150, 255)),
        int(IM_COL32(150, 60, 180, 255)),
        int(IM_COL32(200, 160, 220, 255))
    };


    pos2d<double> posOffset(0, 0);
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    /// CamU draw: FOV arc
    if(_drawFOV) {
        DRMETHS::draw_angleArc(func_convPos_realToPx(_CamU_toDraw.pos()), _CamU_toDraw.angle, _CamU_toDraw.FOV, 50, 100);
    }

    /// CamU draw: Absolute center
    drawList->AddCircle(GUINC::toImVec2(func_convPos_realToPx(_CamU_toDraw.pos())), 2, IM_COL32(200,200,200,200),10);

    /// CamU draw: part: "lens"
    posOffset.x = cos(toRADIANS(_CamU_toDraw.angle))*SOC::drawCamU_lens.y/2;
    posOffset.y = sin(toRADIANS(_CamU_toDraw.angle))*SOC::drawCamU_lens.y/2;
    _CamU_toDraw.pos_shape = DRMETHS::drawRect(func_convPos_realToPx(_CamU_toDraw.pos()) - posOffset, SOC::drawCamU_lens, -(_CamU_toDraw.angle-90), camUnit_drawColour.at(_drawState), 1, DRMETHS::RectCrnrs_BotRight, -1, false);

    /// CamU draw: part: "body"
    posOffset.x += cos(toRADIANS(_CamU_toDraw.angle))*(SOC::drawCamU_lens.y+SOC::drawCamU_box.y/2);
    posOffset.y -= sin(toRADIANS(_CamU_toDraw.angle))*(SOC::drawCamU_lens.y+SOC::drawCamU_box.y/2);
    auto temp_pos_shape = DRMETHS::drawRect(func_convPos_realToPx(_CamU_toDraw.pos()) - posOffset, SOC::drawCamU_box,  -(_CamU_toDraw.angle-90), camUnit_drawColour.at(_drawState), 1, DRMETHS::RectCrnrs_TopLeft, -1, false);
    _CamU_toDraw.pos_shape.insert(_CamU_toDraw.pos_shape.end(), temp_pos_shape.begin(), temp_pos_shape.end());

    
    for(size_t i=0; i<_CamU_toDraw.pos_shape.size()-1; i++) {
        drawList->AddLine(GUINC::toImVec2(_CamU_toDraw.pos_shape.at(i)), GUINC::toImVec2(_CamU_toDraw.pos_shape.at(i+1)), camUnit_drawColour.at(_drawState), 1);
    }
    drawList->AddLine(GUINC::toImVec2(_CamU_toDraw.pos_shape.at(_CamU_toDraw.pos_shape.size()-1)), GUINC::toImVec2(_CamU_toDraw.pos_shape.at(0)), camUnit_drawColour.at(_drawState), 1);

    for(size_t i=0; i<_CamU_toDraw.pos_shape.size(); i++) {
        _CamU_toDraw.pos_shape.at(i) = func_convPos_realToPx(_CamU_toDraw.pos_shape.at(i));
    }
}

void draw_infoBox_camUnit(SOC::CamU &_CamU_ref, bool *_openPtr, pos2d<int> _infoBox_dim) {
    // static bool initialized = false;
    
    if(!*_openPtr) return;
    
    pos2d<int> infoBox_pos{0, 0};

    // if(!initialized) {
        infoBox_pos.x = static_cast<int>(func_convPos_realToPx(_CamU_ref.pos()).x) + 20; // Offset by 20 units to the right
        infoBox_pos.y = static_cast<int>(func_convPos_realToPx(_CamU_ref.pos()).y) - _infoBox_dim.y / 2; // Center vertically
        // initialized = true;
    // }

    ImGui::SetNextWindowPos(ImVec2(infoBox_pos.x, infoBox_pos.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(_infoBox_dim.x, _infoBox_dim.y));
    ImGui::Begin((std::string("CamUnitInfo_")+formatNumber(&_CamU_ref,0,0)).c_str(), _openPtr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    // if(!ImGui::IsWindowFocused()) initialized = false;

    ImGui::SetWindowFontScale(1);

    ImGui::TextWrapped("Cam Unit Information:");
    ImGui::Separator();
    ImGui::TextWrapped("Position: (%.2f, %.2f)", func_convPos_realToPx(_CamU_ref.pos()).x, func_convPos_realToPx(_CamU_ref.pos()).y);
    ImGui::TextWrapped("Angle: %.2f degrees", _CamU_ref.angle);
    ImGui::TextWrapped("Field of View: %.2f degrees", _CamU_ref.FOV);
    
    ImGui::Separator();
    ImGui::TextWrapped("Angle to object: %.2f degrees", _CamU_ref.toObjectAngle);
    // ImGui::Text("Draw State: %d", _CamU_ref.drawState);

    ImGui::Separator();
    ImGui::TextWrapped("Lens Dimensions: (%.2f, %.2f)", SOC::drawCamU_lens.x, SOC::drawCamU_lens.y);
    ImGui::TextWrapped("Body Dimensions: (%.2f, %.2f)", SOC::drawCamU_box.x, SOC::drawCamU_box.y);
    // ImGui::Text("Lens Dimensions: (%.2f, %.2f)", SOC::drawCamU_lens.x, SOC::drawCamU_lens.y);
    // ImGui::Text("Body Dimensions: (%.2f, %.2f)", SOC::drawCamU_box.x, SOC::drawCamU_box.y);

    ImGui::End();
    if(!(*_openPtr)) {
        // initialized = false;
    }
}
