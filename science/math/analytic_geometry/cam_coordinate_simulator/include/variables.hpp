#pragma once
#ifndef HPP__CAM_COORDINATE_SIMULATOR__VARIABLES
#define HPP__CAM_COORDINATE_SIMULATOR__VARIABLES

#include "includes.hpp"
#include "constants.hpp"

/// @brief 2d dimension of the program window
extern pos2d<int> dim__programWindow;
/// @brief 2d dimension of the drawing area
extern pos2d<int> dim__draw_area;

/// @brief 2d position of the drawing area's  top left corner
extern pos2d<int> pos__draw_area;


extern GUINC::imguiwin guiwin;

extern DIY_KBH::keyBind_handler keyBinds;
extern pressed_key__struct guiKeys;

/**
 * ID of the currently selected selectable item with sub categories in base 10
 * 
 * Structure of the total number accordance to base 10 value (ex. `2` means 10^2):
 * - `0`: ID of moveable item (objectPos, CamU[0], CamU[1])
 * - `1`: ID of type of movement (axial positioning, rotation)
 * 
 * item ID's:
 * - `000` move pos of CamU[0]
 * - `001` move pos of CamU[1]
 * - `011` rotate orient of CamU[1]
 * - `009` move objectPos
 * 
 */
extern size_t selectable_itemID;
/**
 * Whether a selectable item has been selected
 * 
 */
extern bool selectable_isSelected;


extern SOC::CamSystem camSys;


#endif //HPP__CAM_COORDINATE_SIMULATOR__VARIABLES