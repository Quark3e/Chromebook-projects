#pragma once
#ifndef HPP__CAM_COORDINATE_SIMULATOR__DRAW_METHODS
#define HPP__CAM_COORDINATE_SIMULATOR__DRAW_METHODS

#include "includes.hpp"
#include "constants.hpp"


/// @brief Draw Methods
namespace DRMETHS {
    /**
     * The space scalar coefficients for converting real space to pixel dim
     * 
     * these values are used to dictate pixel width/height of the drawing space
     */
    extern pos2d<double> spaceScalar;


    enum RectCrnrs {
        RectCrnrs_TopRight  = 0,
        RectCrnrs_TopLeft   = 1,
        RectCrnrs_BotLeft   = 2,
        RectCrnrs_BotRight  = 3,
    };
    std::vector<pos2d<double>> drawRect(pos2d<double> _rectCenter, pos2d<double> _rectDim, double _angle, ImU32 _col, float _thickness = (1.0F), RectCrnrs _startCorner = RectCrnrs_TopLeft, int _createDir=1, bool _drawRect=true);

    
    void draw_camUnit(SOC::CamU &_CamU_toDraw, bool _drawFOV=false, int _CamU_idx=-1);
    
    void draw_angleArc(pos2d<double> _pos, double _offsCenterAngle, double _arcAngle, double _radius, double _radius_extra=0);
    
    // void draw_line(pos2d<double> _pos1, pos2d<double> _pos2);

};


#endif // HPP__CAM_COORDINATE_SIMULATOR__DRAW_METHODS