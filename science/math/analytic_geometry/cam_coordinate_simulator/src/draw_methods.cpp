
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
            GUINC::toImVec2(_pos-pos2d<double>(-cos(toRADIANS(_offsCenterAngle))*(_radius+100), sin(toRADIANS(_offsCenterAngle))*(_radius+100))),
            IM_COL32(250, 100, 100, 100)
        );

        /// Draw: Arc center line end point.
        drawList->AddCircle(GUINC::toImVec2(_pos-pos2d<double>(-cos(toRADIANS(_offsCenterAngle))*(_radius+100), sin(toRADIANS(_offsCenterAngle))*(_radius+100))), 10, IM_COL32(80, 80, 80, 150), 20);
        
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
    
};
