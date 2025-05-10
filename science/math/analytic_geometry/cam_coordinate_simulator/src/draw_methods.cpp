
#include "draw_methods.hpp"


namespace DRMETHS {
    pos2d<double> spaceScalar(1, 1);


    std::vector<pos2d<double>> drawRect(pos2d<double> _rectCenter, pos2d<double> _rectDim, double _angle, ImU32 _col, float _thickness, RectCrnrs _startCorner, int _createDir, bool _drawRect) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        static std::vector<pos2d<double>> cornerPos(4, pos2d<double>(0, 0));
        static std::vector<pos2d<double>> returnCorners(4, pos2d<double>(0, 0));

        cornerPos.at(0) = (_rectCenter - pos2d<double>(-_rectDim.x, _rectDim.y)/2).rotate(_rectCenter, _angle); // top_right
        cornerPos.at(1) = (_rectCenter - _rectDim/2).rotate(_rectCenter, _angle);                               // top_left
        cornerPos.at(2) = (_rectCenter + pos2d<double>(-_rectDim.x, _rectDim.y)/2).rotate(_rectCenter, _angle); // bot_left
        cornerPos.at(3) = (_rectCenter + _rectDim/2).rotate(_rectCenter, _angle);                               // bot_right

        if(_drawRect) {
            for(size_t i=0; i<4; i++) {
                if(i<3) drawList->AddLine(GUINC::toImVec2(cornerPos.at(i)), GUINC::toImVec2(cornerPos.at(i+1)), _col, _thickness);
                // drawList->AddCircle(GUINC::toImVec2(cornerPos.at(i)), 100, IM_COL32(100, 200, 120, 240), 100, _thickness);
                // drawList->AddCircle(GUINC::toImVec2(cornerPos.at(i)), 2, IM_COL32(100, 100, 200, 240), 100, _thickness);
            }
            drawList->AddLine(GUINC::toImVec2(cornerPos.at(3)), GUINC::toImVec2(cornerPos.at(0)), _col, _thickness);
        }

        int cornerIdx = _startCorner;
        for(int i=0; i<4; i++) {
            switch (cornerIdx) {
                case -1: cornerIdx = 3; break;
                case  4: cornerIdx = 0; break;
                default: {
                }
            }
            returnCorners.at(i) = cornerPos.at(cornerIdx);
            cornerIdx -= _createDir;
        }
        
        return returnCorners;
    }
    
    void draw_angleArc(pos2d<double> _pos, double _offsCenterAngle, double _arcAngle, double _radius, double _radius_extra) {
        static std::vector<pos2d<double>> arcPoints_line(3, pos2d<double>(0, 0));
        
        size_t numSegs = 50; //std::roundf(50*(_arcAngle/360));
        std::vector<pos2d<double>> arcPoints_arc(numSegs+1, pos2d<double>(0, 0));

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        /// Draw: Arc center line.
        drawList->AddLine(
            GUINC::toImVec2(_pos),
            GUINC::toImVec2(_pos-pos2d<double>(-cos(toRADIANS(_offsCenterAngle))*(_radius+_radius_extra), sin(toRADIANS(_offsCenterAngle))*(_radius+_radius_extra))),
            IM_COL32(250, 100, 100, 100)
        );

        /// Draw: Arc center line end point.
        drawList->AddCircle(GUINC::toImVec2(_pos-pos2d<double>(-cos(toRADIANS(_offsCenterAngle))*(_radius+_radius_extra), sin(toRADIANS(_offsCenterAngle))*(_radius+_radius_extra))), 10, IM_COL32(80, 80, 80, 150), 20);
        
        arcPoints_line.at(0).x = _pos.x + cos(toRADIANS(_offsCenterAngle+_arcAngle/2))*(_radius+_radius_extra);
        arcPoints_line.at(0).y = _pos.y - sin(toRADIANS(_offsCenterAngle+_arcAngle/2))*(_radius+_radius_extra);
        
        arcPoints_line.at(1).x = _pos.x;
        arcPoints_line.at(1).y = _pos.y;

        arcPoints_line.at(2).x = _pos.x + cos(toRADIANS(_offsCenterAngle-_arcAngle/2))*(_radius+_radius_extra);
        arcPoints_line.at(2).y = _pos.y - sin(toRADIANS(_offsCenterAngle-_arcAngle/2))*(_radius+_radius_extra);


        double _segAng = _offsCenterAngle + _arcAngle/2;
        for(size_t i=0; i<numSegs; i++) {
            arcPoints_arc.at(i).x = _pos.x + cos(toRADIANS(_segAng))*_radius;
            arcPoints_arc.at(i).y = _pos.y - sin(toRADIANS(_segAng))*_radius;
            _segAng -= _arcAngle/numSegs;
        }
        arcPoints_arc.at(numSegs).x = _pos.x + cos(toRADIANS(_offsCenterAngle-_arcAngle/2))*_radius;
        arcPoints_arc.at(numSegs).y = _pos.y - sin(toRADIANS(_offsCenterAngle-_arcAngle/2))*_radius;

        for(size_t i=0; i<arcPoints_arc.size()-1; i++)  drawList->AddLine(GUINC::toImVec2(arcPoints_arc.at(i)), GUINC::toImVec2(arcPoints_arc.at(i+1)), IM_COL32(50, 60, 55, 150));
        for(size_t i=0; i<arcPoints_line.size()-1; i++) drawList->AddLine(GUINC::toImVec2(arcPoints_line.at(i)), GUINC::toImVec2(arcPoints_line.at(i+1)), IM_COL32(50, 60, 55, 150));
    }
    

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
            draw_angleArc(_CamU_toDraw.pos(), _CamU_toDraw.angle, _CamU_toDraw.FOV, 50, 100);
        }

        /// CamU draw: Absolute center
        drawList->AddCircle(GUINC::toImVec2(_CamU_toDraw.pos()), 2, IM_COL32(200,200,200,200),10);

        /// CamU draw: part: "lens"
        posOffset.x = cos(toRADIANS(_CamU_toDraw.angle))*SOC::drawCamU_lens.y/2;
        posOffset.y = sin(toRADIANS(_CamU_toDraw.angle))*SOC::drawCamU_lens.y/2;
        _CamU_toDraw.pos_shape = drawRect(_CamU_toDraw.pos() - posOffset, SOC::drawCamU_lens, -(_CamU_toDraw.angle-90), camUnit_drawColour.at(_drawState), 1, RectCrnrs_BotRight, -1, false);

        /// CamU draw: part: "body"
        posOffset.x += cos(toRADIANS(_CamU_toDraw.angle))*(SOC::drawCamU_lens.y+SOC::drawCamU_box.y/2);
        posOffset.y -= sin(toRADIANS(_CamU_toDraw.angle))*(SOC::drawCamU_lens.y+SOC::drawCamU_box.y/2);
        auto temp_pos_shape = drawRect(_CamU_toDraw.pos() - posOffset, SOC::drawCamU_box,  -(_CamU_toDraw.angle-90), camUnit_drawColour.at(_drawState), 1, RectCrnrs_TopLeft, -1, false);
        _CamU_toDraw.pos_shape.insert(_CamU_toDraw.pos_shape.end(), temp_pos_shape.begin(), temp_pos_shape.end());

        
        for(size_t i=0; i<_CamU_toDraw.pos_shape.size()-1; i++) {
            drawList->AddLine(GUINC::toImVec2(_CamU_toDraw.pos_shape.at(i)), GUINC::toImVec2(_CamU_toDraw.pos_shape.at(i+1)), camUnit_drawColour.at(_drawState), 1);
        }
        drawList->AddLine(GUINC::toImVec2(_CamU_toDraw.pos_shape.at(_CamU_toDraw.pos_shape.size()-1)), GUINC::toImVec2(_CamU_toDraw.pos_shape.at(0)), camUnit_drawColour.at(_drawState), 1);

    }


    void draw_infoBox_camUnit(SOC::CamU &_CamU_ref, bool *_openPtr, pos2d<int> _infoBox_dim) {
        // static bool initialized = false;
        
        if(!*_openPtr) return;
        
        pos2d<int> infoBox_pos{0, 0};

        // if(!initialized) {
            infoBox_pos.x = static_cast<int>(_CamU_ref.pos().x) + 20; // Offset by 20 units to the right
            infoBox_pos.y = static_cast<int>(_CamU_ref.pos().y) - _infoBox_dim.y / 2; // Center vertically
            // initialized = true;
        // }
    
        ImGui::SetNextWindowPos(ImVec2(infoBox_pos.x, infoBox_pos.y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(_infoBox_dim.x, _infoBox_dim.y));
        ImGui::Begin((std::string("CamUnitInfo_")+formatNumber(&_CamU_ref,0,0)).c_str(), _openPtr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        // if(!ImGui::IsWindowFocused()) initialized = false;

        ImGui::SetWindowFontScale(1);
    
        ImGui::TextWrapped("Cam Unit Information:");
        ImGui::Separator();
        ImGui::TextWrapped("Position: (%.2f, %.2f)", _CamU_ref.pos().x, _CamU_ref.pos().y);
        ImGui::TextWrapped("Angle: %.2f degrees", _CamU_ref.angle);
        ImGui::TextWrapped("Field of View: %.2f degrees", _CamU_ref.FOV);
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
};
