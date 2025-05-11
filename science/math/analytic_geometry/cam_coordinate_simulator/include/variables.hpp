#pragma once
#ifndef HPP__CAM_COORDINATE_SIMULATOR__VARIABLES
#define HPP__CAM_COORDINATE_SIMULATOR__VARIABLES

#include "includes.hpp"
#include "constants.hpp"
#include "draw_methods.hpp"


/// @brief 2d dimensions of the program window
extern pos2d<int> dim__programWindow;
/// @brief 2d dimensions of the drawing area
extern pos2d<int> dim__draw_area;
/// @brief 2d dimensions of CamU info boxes
extern pos2d<int> dim__infoBox_CamU;

/// @brief 2d position of the drawing area's  top left corner
extern pos2d<int> pos__draw_area;
/// @brief Location of the center point within the draw_area's cartesian coordinate map
extern pos2d<int> pos__draw_area_loc;

/// @brief Scaling value for converting real-space to pixel-space [px/{real unit}].
extern pos2d<float> scal__draw_area;


inline pos2d<double> func_convPos_realToPx(pos2d<double> _realPos) {
    return pos2d<double>(
        _realPos.x*scal__draw_area.x - pos__draw_area_loc.x,
        (dim__draw_area.y - _realPos.y*scal__draw_area.y) - pos__draw_area_loc.y
    );
}
inline pos2d<double> func_convPos_pxToReal(pos2d<double> _pxPos) {
    return pos2d<double>(
        (_pxPos.x + pos__draw_area_loc.x)/scal__draw_area.x,
        (dim__draw_area.y - (_pxPos.y + pos__draw_area_loc.y))/scal__draw_area.y
    );
}


extern GUINC::imguiwin guiwin;

extern DIY_KBH::keyBind_handler keyBinds;
extern pressed_key__struct guiKeys;

/**
 * The states of each selectable item is depicted here by each carry number, starting from right hand side n*10^0.
 * 
 * Each selectable items ID is stored with each carry number, and the state of each selectable item is stored as the carry factor.
 * Selectable itemID: 
 *  - 10^0: objectPos
 *  - 10^1: CamU[0]
 *  - 10^2: CamU[1]
 *  - 10^3: CamU[2]
 * 
 * Item state:
 *  - 0: Default
 *  - 1: Hovered
 *  - 2: Selected: Move
 *  - 3: Selected: Rotate (only CamU)
 */
extern size_t selectable_itemID;
/**
 * Index to the selectable itemID carry number that's been selected. `-1` means nothing has been selected
 * 
 * 
 */
extern int selectable_isSelected;


extern SOC::CamSystem camSys;
/**
 * Container for boolean values determining whether the infoBox for a given CamU object is to be drawn.
 * 
 */
extern std::list<bool> infoBox_open_CamU;



/**
 * @brief Draws a camera unit with optional field of view (FOV) and state visualization.
 * 
 * @param _CamU_toDraw Reference to the camera unit object to be drawn.
 * @param _drawFOV Optional parameter to specify whether to draw the field of view (FOV). 
 *                 Defaults to false.
 * @param _drawState Optional parameter to specify the drawing state. 
 *                   Defaults to 0.
 */
void draw_camUnit(SOC::CamU &_CamU_toDraw, bool _drawFOV=false, int _drawState=0);


/**
 * @brief Draws an information box for a camera unit.
 * 
 * This function renders an information box displaying details about the specified
 * camera unit. The dimensions of the information box can be customized, and the
 * function also allows toggling its visibility.
 * 
 * @param _CamU_ref Reference to the camera unit object (SOC::CamU) for which the 
 *                  information box is to be drawn.
 * @param _openPtr Pointer to a boolean that determines whether the information box 
 *                 is open or closed. The value can be modified to reflect the 
 *                 current state of the box.
 * @param _infoBox_dim Optional parameter specifying the dimensions of the 
 *                     information box as a 2D position (default is {100, 100}).
 */
void draw_infoBox_camUnit(SOC::CamU &_CamU_ref, bool *_openPtr, pos2d<int> _infoBox_dim=pos2d<int>{100, 100});


#endif //HPP__CAM_COORDINATE_SIMULATOR__VARIABLES