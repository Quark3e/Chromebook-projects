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



    // // pos2d has a rotate() and getRotated() member methods that rotate a point
    // pos2d<double> pivotPoint(pos2d<double> _point, pos2d<double> _pointToPivot, double _deltaAngle);

    void drawRect(pos2d<double> _rectCenter, pos2d<double> _rectDim, double _angle, ImU32 _col, float _thickness = (1.0F));

    
    void draw_camUnit(SOC::CamU _CamU_toDraw, bool _drawFOV=false);
    void draw_camUnit(pos2d<double> _pos, double _angle, bool _drawFOV=false, double _FOV=0);
    
    void draw_angleArc(pos2d<double> _pos, double _offsCenterAngle, double _arcAngle, double _radius, double _radius_extra=0);
    
    // void draw_line(pos2d<double> _pos1, pos2d<double> _pos2);

};



#endif // HPP__CAM_COORDINATE_SIMULATOR__DRAW_METHODS