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

    /**
     * @brief Enum item with ascending/descending value for the four corners of a rect.
     * 
     */
    enum RectCrnrs {
        RectCrnrs_TopRight  = 0,
        RectCrnrs_TopLeft   = 1,
        RectCrnrs_BotLeft   = 2,
        RectCrnrs_BotRight  = 3,
    };
    /**
     * @brief Creates a `std::vector<pos2d<double>>` container with the corners of a rectangle to draw in 2d space (which can be drawn)
     * 
     * @param _rectCenter 2 value container for the coordinate position of the rect center.
     * @param _rectDim 2 value container for the width/height of the rect.
     * @param _angle angle of the rect object in the counter-clockwise direction. [degrees]
     * @param _col RGBA colour of the rect border.
     * @param _thickness thickness of the rect border.
     * @param _startCorner RectCrnrs enum item to the initial corner in the rect-corners container
     * @param _createDir direction of the formation of the rect from the corners [`1`: CW, `-1`: CCW]
     * @param _drawRect boolean option for whether to actually draw the rect object in current window of ImGui
     * @return std::vector<pos2d<double>> of the corners of the rectangle.
     */
    std::vector<pos2d<double>> drawRect(pos2d<double> _rectCenter, pos2d<double> _rectDim, double _angle, ImU32 _col, float _thickness = (1.0F), RectCrnrs _startCorner = RectCrnrs_TopLeft, int _createDir=1, bool _drawRect=true);

    /**
     * @brief Draw the referenced `SOCK::CamU` object onto current window of ImGui
     * 
     * @param _CamU_toDraw reference to the CamU object to draw.
     * @param _drawFOV whether to draw the CamU's FOV lines to indicate the range.
     * @param _drawState the state at which the CamU is to be draw: [`0`: default, `1`: hovered, `2`: selected]
     */
    void draw_camUnit(SOC::CamU &_CamU_toDraw, bool _drawFOV=false, int _drawState=0);
    /**
     * @brief Draw an angle arc onto current window of ImGui
     * 
     * @param _pos 2 value container for the coordinate position of the angle origo/origin.
     * @param _offsCenterAngle angle offset from the arc center angle, not a side, to 0 value (which is along the positive x axis). [degrees]
     * @param _arcAngle angle of the arc. [degrees]
     * @param _radius radius of the circle arc to draw.
     * @param _radius_extra extra length of the arc sides/radii to draw.
     */
    void draw_angleArc(pos2d<double> _pos, double _offsCenterAngle, double _arcAngle, double _radius, double _radius_extra=0);
    
    // void draw_line(pos2d<double> _pos1, pos2d<double> _pos2);

};


#endif // HPP__CAM_COORDINATE_SIMULATOR__DRAW_METHODS