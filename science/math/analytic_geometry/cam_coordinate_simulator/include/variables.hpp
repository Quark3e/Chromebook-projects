#pragma once
#ifndef HPP__CAM_COORDINATE_SIMULATOR__VARIABLES
#define HPP__CAM_COORDINATE_SIMULATOR__VARIABLES

#include "includes.hpp"
#include "constants.hpp"

/// @brief 2d dimensions of the program window
extern pos2d<int> dim__programWindow;
/// @brief 2d dimensions of the drawing area
extern pos2d<int> dim__draw_area;
/// @brief 2d dimensions of CamU info boxes
extern pos2d<int> dim__infoBox_CamU;

/// @brief 2d position of the drawing area's  top left corner
extern pos2d<int> pos__draw_area;


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


#endif //HPP__CAM_COORDINATE_SIMULATOR__VARIABLES